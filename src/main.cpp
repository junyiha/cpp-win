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

	std::string message{ "Hello from server" };
	boost::asio::write(socket, boost::asio::buffer(message));

	return;
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
	else
	{
		log->warn("invalid mode: {}", mode);
	}

	return 0;
}