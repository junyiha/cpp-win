/**
 * @file onnxruntime.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include "base_headers.hpp"
#include "onnxruntime_cxx_api.h"
#include "opencv2/opencv.hpp"

void LoadModel();

void ShowImage(const std::string& image_path);