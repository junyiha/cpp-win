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
#include "onnxruntime.hpp"
#include "openvino.hpp"
#include "libevent.hpp"

#if BOOST_FLAG
#include "boost/filesystem.hpp"
#include "boost/asio.hpp"
#endif

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

#ifdef BOOST_FLAG
int TestBoostFilesystem(int argc, char* argv[])
{
	boost::filesystem::path dir("D:/BaiduNetdiskDownload");
	for (auto& entry : boost::filesystem::directory_iterator(dir))
	{
		std::cerr << entry.path().string() << "\n";
	}
	return 0;
}
#endif

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
	std::thread t([&io_context]() { while (true) { io_context.run(); }});

	std::vector<char> buf(1024);
	while (true)
	{
		if (!client.IsConnect())
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}
		std::cerr << "input message: \n";
		std::cin.getline(buf.data(), buf.size());
		client.Write(buf);
		std::cerr << "receive data: " << client.Read().data() << "\n";
	}

	return 0;
}

int TestApplication(int argc, char* argv[])
{
	return APP::RunAPP(argc, argv);
}

int TestOnnxRuntime(int argc, char* argv[])
{
	LoadModel();
	return 0;
}

int TestShowImage(int argc, char* argv[])
{
	ShowImage("C:/Users/anony/Documents/GitHub/cpp-win/data/1.jpg");
	return 0;
}

int TestOpenVINO(int argc, char* argv[])
{
	TestOpenVino();

	return 0;
}

int TestLibevent(int argc, char* argv[])
{
	test_libevent();


	return 0;
}

int main(int argc, char* argv[])
{
	std::map<std::string, std::function<int(int, char**)>> FuncMap =
	{
		{"test_encoding", TestEncoding},
		{"test_exception", TestException},
		{"test_crobot", TestCRobot},
		{"test_qvector", TestQVector},
		#ifdef BOOST_FLAG
		{"test_boost", TestBoostFilesystem},
		#endif
		{"TestClassServer", TestClassServer},
		{"TestClassClient", TestClassClient},
		{"TestApplication", TestApplication},
		{"TestOnnxRuntime", TestOnnxRuntime},
		{"TestShowImage", TestShowImage},
		{"TestOpenVINO", TestOpenVINO},
		{"TestLibevent", TestLibevent}
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