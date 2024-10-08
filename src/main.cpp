/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author anony
 * \date   October 2024
 *********************************************************************/
#include "base_headers.hpp"

int main(int argc, char* argv[])
{
	InitLogger();

	auto log = spdlog::get("logger");
	log->info("test spdlog logger");

	std::clog << "Hello, world\n";

	return 0;
}