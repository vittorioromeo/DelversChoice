#pragma once

#include "../Common/Common.hpp"
#include "../Payload/Payload.hpp"
#include "../Utils/Retry.hpp"

namespace nl
{
    namespace Tunnel
    {
        class TCPListener
        {
        private:
            ListenerTcp sckt;
            std::future<void> _accept_fut;
            std::map<PAddress, std::unique_ptr<sf::TcpSocket>> _connection_map;

            std::vector<std::unique_ptr<sf::TcpSocket>> _free_conns;

            std::atomic<bool> _busy{false};

            auto get_free_socket()
            {
                // Cleanup dead sockets
                std::vector<decltype(std::begin(_connection_map))> to_delete;
                for(auto itr(std::begin(_connection_map));
                    itr != std::end(_connection_map); ++itr)
                {
                    auto& s(itr->second);
                    /*
                                      std::size_t dummy;

                                      if( socket.receive(&dummy, 0, dummy) ==
                       sf::Socket::Disconnected ) {
                                          // Opposite side wants to disconnect.
                                          // Time to lament that there is no
                       clean way
                                          // to perform a clean connection
                       termination with SFML.
                                      }
                  */

                    if(s->getLocalPort() == 0)
                    {
                        nl::debugLo() << "Dead socket put into free list\n\n";
                        _free_conns.emplace_back(std::move(s));
                        to_delete.emplace_back(itr);
                    }
                }

                for(auto itr : to_delete) _connection_map.erase(itr);

                if(_free_conns.empty())
                {
                    for(auto i(0u); i < 5; ++i)
                    {
                        _free_conns.emplace_back(
                            std::make_unique<sf::TcpSocket>());

                        _free_conns.back()->setBlocking(false);
                    }
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
                                    if(sckt.accept(*fs) ==
                                        sf::Socket::Status::Done)
                                    {

                                        auto r_ip = fs->getRemoteAddress();
                                        auto r_port = fs->getRemotePort();


                                        PAddress key{r_ip, r_port};

                                        nl::debugLo()
                                            << "Accepted TCP connection\n"
                                            << key << "\n\n";

                                        _connection_map[key] = std::move(fs);

                                        return true;
                                    }

                                    return false;
                                });

                            if(retry_res) continue;

                            // nl::debugLo()
                            //  << "Retry accept TCP connection failed\n";

                            // If retry failed:
                            _free_conns.emplace_back(std::move(fs));
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
                for(auto& c : _connection_map)
                {
                    auto& a(c.first);
                    auto& s(c.second);

                    if(s->receive(p.data) == sf::Socket::Done)
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
                const auto& ta(p.address);
                if(_connection_map.count(ta) == 0)
                {
                    nl::debugLo() << "Can't send to unconnected" << ta << "\n";
                    return false;
                }

                return _connection_map[ta]->send(p.data) == sf::Socket::Done;
            }
        };

        class TCPSckt
        {
        private:
            ScktTcp sckt;

        public:
            TCPSckt(const TCPSckt&) = delete;
            TCPSckt& operator=(const TCPSckt&) = delete;

            TCPSckt(TCPSckt&&) = default;
            TCPSckt& operator=(TCPSckt&&) = default;

            template <typename... Ts>
            TCPSckt(Ts&&... xs)
                : sckt{FWD(xs)...}
            {
                sckt.setBlocking(false);
            }

            auto bound() const noexcept { return sckt.getLocalPort() != 0; }

            ~TCPSckt() { sckt.disconnect(); }

            bool bind(IpAddr ip, Port port, sf::Time timeout = sf::Time::Zero)
            {
                return sckt.connect(ip, port, timeout) == sf::Socket::Done;
            }

            auto receive_payload(Payload& p)
            {
                return sckt.receive(p.data) == sf::Socket::Done;
            }

            auto send_payload(Payload& p)
            {
                return sckt.send(p.data) == sf::Socket::Done;
            }
        };
    }
}
