#pragma once

#include "../Common/Common.hpp"
#include "../Utils/Retry.hpp"
#include "../Architecture/BusyFut.hpp"
#include "../Architecture/ThreadSafeQueue.hpp"
#include "../Architecture/Payload.hpp"
#include "../Architecture/ManagedSendBuf.hpp"
#include "../Architecture/ManagedRecvBuf.hpp"

namespace nl
{

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

        Impl::BusyFut futProc{
            [this]
            {
                Impl::Payload p;

                // TODO:
                auto ok(mpbRecv.tsq.try_dequeue_for(100ms, p));

                if(ok) {
                    fnProcess(*this, p.data, p.target);
                }
            }};

        TFProcess fnProcess;

        void tryBindSocket()
        {
            if(retry(5, [this]
                     {
                         return sckt.bind(port) == sf::Socket::Done;
                     }))
            {
                ssvu::lo() << "Socket successfully bound to port " << port
                           << "\n";
            }
            else
            {
                throw std::runtime_error("Error binding socket");
            }
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
            futProc.stop();
        }


        void send(Impl::Payload& mPayload)
        {
            // TODO:
            mpbSend.tsq.try_enqueue_for(100ms, mPayload);
        }

        template <typename... Ts>
        void send(const Impl::PayloadTarget& mTarget, Ts&&... mXs)
        {
            auto p(Impl::mkPayload(mTarget, FWD(mXs)...));
            send(p);
        }
    };
}