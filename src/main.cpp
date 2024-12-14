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

#include "boost/filesystem.hpp"
#include "boost/asio.hpp"

#include "asio.hpp"

void TestEncoding()
{
	QString str("中文");
	std::string std_str("中文");
	std::cerr << "std string: " << std_str << "\n";

	str = QString::fromLocal8Bit("中文");
	str = QString::fromLocal8Bit(std_str.c_str());
	qDebug() << "QString: " << str << "\n";
}

void TestException()
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

void TestCRobot()
{
	E_LinkState state{ eLINK_ERRORSTOP };
	int temp = 7;

	memcpy(&state, &temp, sizeof(temp));

	std::cerr << state << "\n";
}

void TestQVector()
{

}

void TestBoostSystem()
{

}

void TestBoostFilesystem()
{
	boost::filesystem::path dir("D:/BaiduNetdiskDownload");
	for (auto& entry : boost::filesystem::directory_iterator(dir))
	{
		std::cerr << entry.path().string() << "\n";
	}
}

void AsioServer()
{
	using tcp = boost::asio::ip::tcp;

	boost::asio::io_context io_context;

	tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));

	tcp::socket socket(io_context);
	acceptor.accept(socket);
	std::vector<char> recv_buf(10 * 1024);
	while (true)
	{
		std::size_t recv_size = socket.read_some(boost::asio::buffer(recv_buf.data(), recv_buf.size()));
		std::cerr << "receive data: " << std::string(recv_buf.data(), recv_size) << "\n";
		std::string message{ "Hello from server" };
		boost::asio::write(socket, boost::asio::buffer(message));
	}

	return;
}

void AsioClient()
{
	asio::io_context io_context;
	auto address = asio::ip::make_address("127.0.0.1");
	asio::ip::tcp::endpoint endpoint(address, 1234);
	asio::ip::tcp::socket socket(io_context);
	asio::error_code ec;
	socket.connect(endpoint, ec);
	if (!ec)
	{
		std::cerr << "Connect successful\n";
		std::vector<char> buf(10 * 1024);
		while (true)
		{
			std::string msg;
			std::cerr << "input send message: \n";
			std::cin >> msg;
			socket.write_some(asio::buffer(msg.data(), msg.size()));
			std::size_t recv_size = socket.read_some(asio::buffer(buf.data(), buf.size()));
			std::cerr << "receive data: " << std::string(buf.data(), recv_size) << "\n";
		}
	}
	else
	{
		std::cerr << "connect failed, error message: " << ec.message() << "\n";
	}
}

int main(int argc, char* argv[])
{
	InitLogger();
	auto log = spdlog::get("logger");
	std::string mode{ "robot" };

	cxxopts::Options options("Robot", "robot");
	options.add_options()("m,mode", "mode", cxxopts::value<std::string>());
	try
	{
		auto result = options.parse(argc, argv);
		mode = result["mode"].as<std::string>();
	}
	catch (...)
	{
		log->warn("parse argument failed\n");
	}

	if (mode == "robot")
	{
		APP::RunAPP(argc, argv);
	}
	else if (mode == "config")
	{
		Config::ConfigurationMain();
	}
	else if (mode == "help")
	{
	}
	else if (mode == "filesystem")
	{
		TestBoostFilesystem();
	}
	else if (mode == "tcp_server")
	{
		AsioServer();
	}
	else if (mode == "tcp_client")
	{
		AsioClient();
	}
	else
	{
		log->warn("invalid mode: {}", mode);
	}

	return 0;
}