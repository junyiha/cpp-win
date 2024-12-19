/****************************************************************
 * \file   main.cpp
 * \brief
 *
 * \author anony
 * \date   October 2024
 *********************************************************************/
#include "cxxopts.hpp"

#include "base_headers.hpp"
#include "application.hpp"
#include "configuration.hpp"
#include "counter_event.hpp"
#include "asio_network.hpp"

#include "boost/filesystem.hpp"
#include "boost/asio.hpp"

#include "asio.hpp"

int TestEncoding(int argc, char* argv[])
{
	QString str("中文");
	std::string std_str("中文");
	std::cerr << "std string: " << std_str << "\n";

	str = QString::fromLocal8Bit("中文");
	str = QString::fromLocal8Bit(std_str.c_str());
	qDebug() << "QString: " << str << "\n";
	return 0;
}

int TestException(int argc, char* argv[])
{
	std::vector<int> arr(3, 0);

	try
	{
		std::cerr << arr.at(3) << "\n";
	}
	catch (std::out_of_range& e)
	{
		std::cerr << e.what() << "\n";
	}
	return 0;
}

enum E_LinkState
{
	eLINK_ERRORSTOP = 0,
	eLINK_DISABLED,
	eLINK_STANDSTILL,
	eLINK_STOPPING,
	eLINK_HOMING,
	eLINK_MOVING
};

int TestCRobot(int argc, char* argv[])
{
	E_LinkState state{ eLINK_ERRORSTOP };
	int temp = 7;

	memcpy(&state, &temp, sizeof(temp));

	std::cerr << state << "\n";
	return 0;
}

int TestQVector(int argc, char* argv[])
{

	return 0;
}

int TestBoostFilesystem(int argc, char* argv[])
{
	boost::filesystem::path dir("D:/BaiduNetdiskDownload");
	for (auto& entry : boost::filesystem::directory_iterator(dir))
	{
		std::cerr << entry.path().string() << "\n";
	}
	return 0;
}

void DoSession(asio::ip::tcp::socket socket)
{
	std::vector<char> buffer(10 * 1024);
	while (true)
	{
		try
		{
			std::size_t read_size = socket.read_some(asio::buffer(buffer.data(), buffer.size()));
			std::string recv_msg(buffer.data(), read_size);
			std::cerr << "receive data: " << recv_msg << "\n";
			recv_msg += recv_msg;
			socket.write_some(asio::buffer(recv_msg.data(), recv_msg.size()));
		}
		catch (asio::system_error& e)
		{
			std::cerr << "catch exception, message: " << e.what() << "\n";
			break;
		}
	}
}

void ServerHandler(const asio::error_code& ec, asio::ip::tcp::socket peer)
{
	if (!ec)
	{
		new std::thread(DoSession, std::move(peer));
	}

	// 回调接收链接
}

int AsioServer(int argc, char* argv[])
{
	using tcp = asio::ip::tcp;

	asio::io_context io_context;
	asio::error_code ec;
	tcp::acceptor acceptor(io_context, tcp::endpoint(asio::ip::tcp::v4(), 9990));

	acceptor.async_accept(io_context, ServerHandler);

	while (true)
	{
		io_context.run();
	}

	return 0;
}

int TestClassServer(int argc, char* argv[])
{
	asio::io_context io_context;
	asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), 9990);

	net::Server server(io_context, endpoint);

	io_context.run();

	std::cerr << "TestClassServer exit...\n";

	return 0;
}

int TestClassClient(int argc, char* argv[])
{
	asio::io_context io_context;
	asio::ip::tcp::resolver resolver(io_context);
	auto address = asio::ip::make_address("127.0.0.1");
	asio::ip::tcp::endpoint endpoint(address, 9990);
	net::Client client(io_context, endpoint);
	std::thread t([&io_context]() { io_context.run(); });

	std::vector<char> buf(1024);
	while (true)
	{
		std::cerr << "input message: \n";
		std::cin.getline(buf.data(), buf.size());
		client.Write(buf);
		std::cerr << "receive data: " << client.Read().data() << "\n";
	}

	return 0;
}

void connect_handler(const asio::error_code& error)
{
	if (!error)
	{
		std::cerr << "connect succeeded\n";
	}
	else
	{
		std::cerr << "connect failed\n";
	}
}

void DoConnect(asio::ip::tcp::socket& socket)
{
	auto address = asio::ip::make_address("127.0.0.1");
	asio::ip::tcp::endpoint endpoint(address, 9990);
	asio::error_code ec;
	socket.async_connect(endpoint, connect_handler);
}

int AsioClient(int argc, char* argv[])
{
	asio::io_context io_context;
	asio::ip::tcp::socket socket(io_context);
	DoConnect(socket);

	io_context.post([&]() {
		std::vector<char> buf(10 * 1024);
		asio::error_code ec;
		while (true)
		{
			std::string msg;
			std::cerr << "input send message: \n";
			std::cin >> msg;
			// std::this_thread::sleep_for(std::chrono::seconds(1));
			bool temp_res = socket.is_open();
			if (!temp_res)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				std::cerr << "reconnect to server\n";
				DoConnect(socket);
				continue;
			}

			socket.write_some(asio::buffer(msg.data(), msg.size()));
			asio::error_code ec;
			std::size_t recv_size;
			recv_size = socket.read_some(asio::buffer(buf.data(), buf.size()));
			std::cerr << "receive data: " << std::string(buf.data(), recv_size) << "\n";
		}
	});

	std::thread io_thread([&]() {io_context.run(); });

	io_thread.join();

	return 0;
}

int main(int argc, char* argv[])
{
	std::map<std::string, std::function<int(int, char**)>> FuncMap =
	{
		{"server", AsioServer},
		{"client", AsioClient},
		{"test_encoding", TestEncoding},
		{"test_exception", TestException},
		{"test_crobot", TestCRobot},
		{"test_qvector", TestQVector},
		{"test_boost", TestBoostFilesystem},
		{"TestClassServer", TestClassServer},
		{"TestClassClient", TestClassClient}
	};

	InitLogger();
	auto log = spdlog::get("logger");
	std::string mode{ "robot" };

	cxxopts::Options options("Robot", "robot");
	options.add_options()("m,mode", "mode", cxxopts::value<std::string>()->default_value("server"));
	try
	{
		auto result = options.parse(argc, argv);
		mode = result["mode"].as<std::string>();
	}
	catch (...)
	{
		log->warn("parse argument failed\n");
	}

	auto it = FuncMap.find(mode);
	if (it != FuncMap.end())
	{
		it->second(argc, argv);
	}
	else
	{
		log->error("invalid argument: {}", mode);
	}
}