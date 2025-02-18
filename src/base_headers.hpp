/*****************************************************************//**
 * \file   base_headers.hpp
 * \brief
 *
 * \author anony
 * \date   October 2024
 *********************************************************************/
#ifndef BASE_HEADERS_HPP
#define BASE_HEADERS_HPP
#pragma once

#include <iostream>
#include <chrono>
#include <bitset>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <memory>
#include <condition_variable>

 // spdlog
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

void InitLogger();

#endif // BASE_HEADERS_HPP