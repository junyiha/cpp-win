#include "asio_network.hpp"

namespace net
{
    Session::Session(tcp::socket socket)
        : socket_(std::move(socket)), buffer_(10 * 1024)
    {
        start();
    }

    void Session::start()
    {
        new std::thread([this]() {
            try
            {
                while (true)
                {
                    std::size_t read_size = socket_.read_some(asio::buffer(buffer_.data(), buffer_.size()));
                    std::string recv_msg(buffer_.data(), read_size);
                    std::cerr << "receive data: " << recv_msg << "\n";
                    recv_msg += recv_msg;
                    socket_.write_some(asio::buffer(recv_msg.data(), recv_msg.size()));

                }
            }
            catch (asio::system_error& e)
            {
                std::cerr << "catch exception, message: " << e.what() << "\n";
            }
        });
    }

    Server::Server(asio::io_context& io_context, const tcp::endpoint& endpoint)
        :acceptor_(io_context, endpoint)
    {
        do_accept();
    }

    void Server::do_accept()
    {
        acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
            if (!ec)
            {
                session_list.emplace_back(std::make_unique<Session>(std::move(socket)));
            }

            do_accept();
        });
    }

    Client::Client(asio::io_context& io_context, const tcp::endpoint& endpoint)
        :io_context_(io_context), socket_(io_context), endpoint_(endpoint)
    {
        do_connect();
    }

    void Client::do_connect()
    {
        socket_.async_connect(endpoint_, [this](std::error_code ec) {
            if (!ec)
            {
                std::cerr << "connect succeeded\n";
                is_connect.store(true);
            }
            else
            {
                std::cerr << "connect failed\n";
                is_connect.store(false);
                do_connect();
            }
        });
    }

    void Client::Write(const std::vector<char>& data)
    {
        if (!is_connect.load())
        {
            std::cerr << "fatal error: not connect\n";
            do_connect();
            return;
        }
        try
        {
            socket_.write_some(asio::buffer(data.data(), data.size()));
        }
        catch (asio::system_error& e)
        {
            std::cerr << "catch exception, message: " << e.what() << "\n";
            is_connect.store(false);
            socket_.close();
        }
    }

    std::vector<char> Client::Read()
    {
        std::vector<char> recv_data(10 * 1024);
        if (!is_connect.load())
        {
            std::cerr << "fatal error: not connect\n";
            do_connect();
            return recv_data;
        }
        try
        {
            std::size_t len = socket_.read_some(asio::buffer(recv_data.data(), recv_data.size()));
            recv_data.resize(len);
        }
        catch (asio::system_error& e)
        {
            std::cerr << "catch exception, message: " << e.what() << "\n";
            is_connect.store(false);
            socket_.close();
        }

        return recv_data;
    }
}  // namespace net