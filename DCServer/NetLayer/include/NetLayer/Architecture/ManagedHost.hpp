#pragma once

#include "../Common/Common.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Architecture/Payload.hpp"
#include "../Architecture/ManagedSendBuf.hpp"
#include "../Architecture/ManagedRecvBuf.hpp"

namespace nl
{
    namespace Impl
    {
        class BusyFut
        {
        private:
            std::atomic<bool> wrapperAlive{true}; // construct flag first!
            std::future<void> fut;

            template <typename TF>
            void loop(const TF& mFn)
            {
                while(wrapperAlive) {
                    NL_DEBUGLO() << "waiting\n";
                    mFn();
                }

                NL_DEBUGLO() << (wrapperAlive ? "alive" : "dead!!");
            }

        public:
            template <typename TF>
            BusyFut(const TF& mFn)
                : fut{std::async(std::launch::async, [this, mFn]
                                 {
                                     loop(mFn);
                                 })}
            {
            }

            BusyFut(BusyFut&& mX) : wrapperAlive(true), fut(std::move(mX.fut))
            {
            }

            ~BusyFut()
            {
                wrapperAlive = false;
                NL_DEBUGLO() << "beforeget";
                fut.get(); // block, so it sees wrapperAlive before it is
                           // destroyed.
                NL_DEBUGLO() << "aftgerget";
            }

            void stop() { wrapperAlive = false; }
            bool isBusy() const { return wrapperAlive; }
        };
    }

    // TODO: dumber class than ManagedHost that is NOT async

    template <typename TFProcess>
    class ManagedHost
    {
    private:
        // This host's ip.
        IpAddr ip;

        // This host's port.
        Port port;

        // This host's socket.
        ScktUdp sckt;

        // Local host -> send queue/buf -> internet
        Impl::ManagedSendBuf mpbSend;

        // Internet -> recv queue/buf -> local host
        Impl::ManagedRecvBuf mpbRecv;

        // TODO:
        // Threads:
        Impl::BusyFut futSend{[this]
                              {
                                  mpbSend.sendLoop(sckt);
                              }};

        Impl::BusyFut futRecv{[this]
                              {
                                  mpbRecv.recvLoop(sckt);
                              }};

        Impl::BusyFut futProc{[this]
                              {
                                  processImpl();
                              }};

        TFProcess fnProcess;

        void tryBindSocket()
        {
            if(sckt.bind(port) != sf::Socket::Done) {
                throw std::runtime_error("Error binding socket");
            }
            else
            {
                ssvu::lo() << "Socket successfully bound to port " << port
                           << "\n";
            }
        }

        void processImpl()
        {
            auto payload(mpbRecv.dequeue());
            fnProcess(*this, payload.data, payload.target);
        }

    public:
        template <typename TF>
        ManagedHost(Port mPort, TF&& mFnProcess)
            : ip{IpAddr::getLocalAddress()}, port{mPort}, fnProcess{mFnProcess}
        {

            sckt.setBlocking(false);
            tryBindSocket();
        }

        ~ManagedHost()
        {
            sckt.unbind();
            stop();
            NL_DEBUGLO() << "Destroyed ManagedHost\n";
        }

        bool isBusy() const noexcept
        {
            return futSend.isBusy() || futRecv.isBusy();
        }

        void stop()
        {
            futSend.stop();
            futRecv.stop();
        }


        void send(Impl::Payload& mPayload) { mpbSend.enqueue(mPayload); }

        template <typename... Ts>
        void send(const Impl::PayloadTarget& mTarget, Ts&&... mXs)
        {
            auto p(Impl::makePayload(mTarget, FWD(mXs)...));
            send(p);
        }
    };

    // TODO: fix!
    template <typename TFProcess>
    inline auto makeManagedHost(Port mPort, TFProcess&& mFnProcess)
    {
        nl::ManagedHost<decltype(mFnProcess)> result{mPort, mFnProcess};
        return result;
    }
}
