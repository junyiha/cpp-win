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


int main(int argc, char* argv[])
{
	InitLogger();

	auto log = spdlog::get("logger");
	log->info("test spdlog logger");

	std::clog << "Hello, world\n";

	Config::ConfigurationMain();

	APP::RunAPP(argc, argv);

	log->warn("will quit...");

	return 0;
}