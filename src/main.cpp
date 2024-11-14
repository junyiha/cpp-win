/*****************************************************************//**
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

void TestEncoding()
{
	QString str("中文");
	std::string std_str("中文");
	std::cerr << "std string: " << std_str << "\n";

	str = QString::fromLocal8Bit("中文");
	str = QString::fromLocal8Bit(std_str.c_str());
	qDebug() << "QString: " << str << "\n";
}

int main(int argc, char* argv[])
{
	InitLogger();
	auto log = spdlog::get("logger");
	std::string mode{ "robot" };

	TestEncoding();
	Loop();
	return 0;
	//return Config::ConfigurationMain();
	//return APP::RunAPP(argc, argv);

	cxxopts::Options options("Robot", "robot");
	options.add_options()
		("m,mode", "mode", cxxopts::value<std::string>());
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
	else
	{
		log->warn("invalid mode: {}", mode);
	}


	return 0;
}