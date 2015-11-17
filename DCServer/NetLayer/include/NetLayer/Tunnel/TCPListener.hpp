#pragma once

#include "../Common/Common.hpp"
#include "../Payload/Payload.hpp"
#include "../Utils/Retry.hpp"
#include "../Utils/MkUniqueLock.hpp"
#include "./TCPListenerConn.hpp"

namespace nl
{
    namespace Tunnel
    {
        class TCPListener
        {
        private:
            using ConnUPtr = std::unique_ptr<TCPListenerConn>;

            ListenerTcp sckt;
            std::future<void> _accept_fut;

            static constexpr int _temp_ctr_max{10};
            std::map<PAddress, ConnUPtr> _connection_map;
            std::vector<ConnUPtr> _free_conns;

            std::atomic<bool> _busy{false};

            std::mutex _listener_mutex;

            void create_free_sockets()
            {
                for(auto i(0u); i < 5; ++i)
                {
                    _free_conns.emplace_back(
                        std::make_unique<TCPListenerConn>());
                }
            }

            auto get_free_socket()
            {
                // Cleanup dead sockets
                std::vector<decltype(std::begin(_connection_map))> to_delete;
                for(auto itr(std::begin(_connection_map));
                    itr != std::end(_connection_map); ++itr)
                {
                    auto& s(itr->second);

                    s->recv_dummy_or_inc_ctr();
                    if(s->ctr() > _temp_ctr_max)
                    {
                        nl::debugLo() << "Dead socket put into free list\n"
                                      << s->ip() << ":" << s->port() << "\n\n";

                        s->reset_ctr();
                        _free_conns.emplace_back(std::move(s));
                        to_delete.emplace_back(itr);
                    }
                }

                for(auto itr : to_delete)
                {
                    _connection_map.erase(itr);
                }

                if(_free_conns.empty())
                {
                    create_free_sockets();
                }

                auto fs(std::move(_free_conns.back()));
                _free_conns.pop_back();

                return fs;
            }

            void start_accept_fut()
            {
                _busy = true;
                _accept_fut = std::async(std::launch::async, [this]
                    {
                        while(_busy)
                        {
                            auto fs(get_free_socket());

                            auto retry_res = retry(5, [this, &fs]
                                {
                                    if(sckt.accept(fs->sckt()) ==
                                        sf::Socket::Status::Done)
                                    {
                                        auto r_ip = fs->ip();
                                        auto r_port = fs->port();

                                        PAddress key{r_ip, r_port};

                                        nl::debugLo()
                                            << "Accepted TCP connection\n"
                                            << key << "\n\n";

                                        {
                                            auto l(make_unique_lock(
                                                _listener_mutex));

                                            _connection_map[key] =
                                                std::move(fs);
                                        }

                                        return true;
                                    }

                                    return false;
                                });

                            if(retry_res) continue;

                            // nl::debugLo()
                            //  << "Retry accept TCP connection failed\n";

                            // If retry failed:
                            {
                                auto l(make_unique_lock(_listener_mutex));
                                _free_conns.emplace_back(std::move(fs));
                            }
                        }
                    });
            }

        public:
            TCPListener(const TCPListener&) = delete;
            TCPListener& operator=(const TCPListener&) = delete;

            TCPListener(TCPListener&&) = default;
            TCPListener& operator=(TCPListener&&) = default;

            template <typename... Ts>
            TCPListener(Ts&&... xs)
                : sckt{FWD(xs)...}
            {
                sckt.setBlocking(false);
            }

            auto bound() const noexcept { return sckt.getLocalPort() != 0; }

            ~TCPListener()
            {
                auto l(make_unique_lock(_listener_mutex));

                _busy = false;
                sckt.close();
            }

            bool bind(Port port)
            {
                if(sckt.listen(port) == sf::Socket::Error)
                {
                    return false;
                }

                start_accept_fut();
                return true;
            }

            auto receive_payload(Payload& p)
            {
                auto l(make_unique_lock(_listener_mutex));

                for(auto& c : _connection_map)
                {
                    auto& a(c.first);
                    auto& s(c.second);

                    if(s->sckt().receive(p.data) == sf::Socket::Done)
                    {
                        p.address = a;

                        /*
                        nl::debugLo() << "Received data:\n"
                                      << "\tmap addr: " << c.first << "\n"
                                      << "\tpckt addr: " << p.address << "\n\n";
*/

                        return true;
                    }
                }

                return false;
            }

            auto send_payload(Payload& p)
            {
                auto l(make_unique_lock(_listener_mutex));

                const auto& ta(p.address);
                if(_connection_map.count(ta) == 0)
                {
                    nl::debugLo() << "Can't send to unconnected" << ta << "\n";
                    return false;
                }

                return _connection_map[ta]->sckt().send(p.data) ==
                       sf::Socket::Done;
            }
        };
    }
}
