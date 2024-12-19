/**
 * @file asio_network.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-19
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#ifndef ASIO_NETWORK_HPP
#define ASIO_NETWORK_HPP

#include <iostream>
#include "asio.hpp"

namespace net
{
    using tcp = asio::ip::tcp;
    class Session
    {
    public:
        Session(tcp::socket socket);

    private:
        void start();

    private:
        tcp::socket socket_;
        std::vector<char> buffer_;
    };

    class Server
    {
    public:
        Server(asio::io_context& io_context, const tcp::endpoint& endpoint);

    private:
        void do_accept();

    private:
        tcp::acceptor acceptor_;
        std::list<std::unique_ptr<Session>> session_list;
    };

    class Client
    {
    public:
        Client(asio::io_context& io_context, const tcp::endpoint& endpoint);

        void Write(const std::vector<char>& data);

        std::vector<char> Read();

    private:
        void do_connect();

    private:
        asio::io_context& io_context_;
        tcp::socket socket_;
        std::atomic<bool> is_connect;
        tcp::endpoint endpoint_;
    };
}  // namespace net
#endif  // ASIO_NETWORK_HPP