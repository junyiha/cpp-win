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

struct Context_t
{
    Ort::Env* env;
    Ort::Session* session;
    std::vector<char*> input_names;
    std::vector<char*> output_names;
    std::vector<int64_t> input_shape;
    std::vector<int64_t> output_shape;
};

class Detector
{

public:
    bool Create(std::string id, std::wstring path);

private:
    std::map<std::string, Context_t> m_detector_map;
};

void LoadModel();

void ShowImage(const std::string& image_path);