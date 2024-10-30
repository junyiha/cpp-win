/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author anony
 * \date   October 2024
 *********************************************************************/
#include "base_headers.hpp"
#include "application.hpp"
#include "configuration.hpp"
#include "cxxopts.hpp"


int main(int argc, char* argv[])
{
	InitLogger();
	auto log = spdlog::get("logger");
	std::string mode{ "robot" };

	//return Config::ConfigurationMain();
	return APP::RunAPP(argc, argv);

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