#include "onnxruntime.hpp"

std::vector<float> FitLine(std::vector<cv::Point2f> points)
{
    cv::Vec4f lines;
    cv::fitLine(points, lines, cv::DIST_L2, 0, 0.01, 0.01);
    float k = lines[1] / lines[0];
    float lefty = (-lines[2] * k) + lines[3];
    float righty = ((768 - 1 - lines[2]) * k) + lines[3];

    return { 0.0, lefty, 768.0, righty, k };
}

double CaculateDistance(const cv::Point2f& point1, const cv::Point2f& point2)
{
    return std::sqrt(std::pow(point1.x - point2.x, 2) + std::pow(point1.y - point2.y, 2));
}

std::vector<std::vector<cv::Point2f>> GetPossibleLines(const cv::Mat& mask)
{
    std::vector<std::vector<cv::Point2f>> possible_lines;
    cv::GaussianBlur(mask, mask, cv::Size(5, 5), 0);
    cv::threshold(mask, mask, 0, 255, cv::THRESH_OTSU);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    if (contours.empty())
    {
        std::cerr << "contours empty\n";
        return possible_lines;
    }
    for (int i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours.at(i));
        if (area < 768 * 0.2)
        {
            continue;
        }

        cv::RotatedRect min_rect = cv::minAreaRect(contours.at(i));
        cv::Mat points;
        std::vector<cv::Point2f> rect_points;
        cv::boxPoints(min_rect, points);

        for (int j = 0; j < points.rows; j++)
        {
            rect_points.push_back(points.at<cv::Point2f>(j, 0));
        }

        for (int j = 0; j < rect_points.size() - 1; j++)
        {
            for (int k = 0; k < rect_points.size() - j - 1; k++)
            {
                if (rect_points.at(k).x > rect_points.at(k + 1).x)
                {
                    cv::Point temp = rect_points.at(k);
                    rect_points.at(k) = rect_points.at(k + 1);
                    rect_points.at(k + 1) = temp;
                }
            }
        }

        cv::Point2f point_left = cv::Point((rect_points.at(0).x + rect_points.at(1).x) / 2, (rect_points.at(0).y + rect_points.at(1).y) / 2);
        cv::Point2f point_right = cv::Point((rect_points.at(2).x + rect_points.at(3).x) / 2, (rect_points.at(2).y + rect_points.at(3).y) / 2);
        possible_lines.push_back({ point_left, point_right });
    }

    return possible_lines;
}

void LoadModel()
{
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
    Ort::SessionOptions session_options;
    std::wstring model_path{ L"D:/Robot/models/pp_liteseg_stdc1_softmax_20241021.onnx" };
    model_path = L"C:/Users/anony/Documents/GitHub/cpp-win/data/ppseg_model_wb_20241120.onnx";
    Ort::Session session(env, model_path.c_str(), session_options);

    std::size_t num_input_nodes = session.GetInputCount();
    std::size_t num_output_nodes = session.GetOutputCount();

    Ort::AllocatorWithDefaultOptions allocator;
    std::string input_name = session.GetInputName(0, allocator);
    std::string output_name = session.GetOutputName(0, allocator);

    auto input_info = session.GetInputTypeInfo(0);
    auto input_shape = input_info.GetTensorTypeAndShapeInfo().GetShape();
    auto input_dims = input_shape[0];

    std::vector<char*> input_names;
    for (int i = 0; i < num_input_nodes; i++)
    {
        auto input_info = session.GetInputTypeInfo(i);
        auto input_name = session.GetInputName(i, allocator);
        input_names.push_back(input_name);
        auto input_shape = input_info.GetTensorTypeAndShapeInfo().GetShape();
        auto input_dims = input_shape[0];
        std::cerr << "input name: " << input_name << "\n";
        std::cerr << "input shape(BCHW): ";
        for (auto dim : input_shape)
        {
            std::cerr << dim << ", ";
        }
        std::cerr << "\n";
    }

    std::vector<char*> output_names;
    for (int i = 0; i < num_output_nodes; i++)
    {
        auto output_info = session.GetOutputTypeInfo(i);
        auto output_name = session.GetOutputName(i, allocator);
        output_names.push_back(output_name);
        auto output_shape = output_info.GetTensorTypeAndShapeInfo().GetShape();
        auto output_dims = output_shape[0];
        std::cerr << "output name: " << output_name << "\n";
        std::cerr << "output shape(BCHW): ";
        for (auto dim : output_shape)
        {
            std::cerr << dim << ", ";
        }
        std::cerr << "\n";
    }

    std::string image_path{ "C:/Users/anony/Documents/GitHub/cpp-win/data/LineCam_6_2024-12-23-15-59-55.png" };
    // std::string image_path{ "C:/Users/anony/Documents/GitHub/cpp-win/data/LineCam_3_2024-12-23-15-59-54.png" };

    cv::Mat image = cv::imread(image_path);
    if (image.empty())
    {
        std::cerr << "load image failed\n";
        return;
    }
    cv::Mat image_gray = image.clone();
    cv::resize(image_gray, image_gray, cv::Size(input_shape.at(2), input_shape.at(3)));
    if (image.channels() != 3)
    {
        cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);
    }
    cv::resize(image, image, cv::Size(input_shape.at(2), input_shape.at(3)));
    image.convertTo(image, CV_32F);
    image = ((image / 255.0f) - 0.5f) / 0.5f;
    cv::Mat blob = cv::dnn::blobFromImage(image, 1.0, cv::Size(input_shape.at(2), input_shape.at(3)), cv::Scalar(0, 0, 0), true, false);

    std::vector<Ort::Value> input_tensors;
    auto allocator_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    input_tensors.emplace_back(Ort::Value::CreateTensor<float>(allocator_info, blob.ptr<float>(), blob.total(), input_shape.data(), input_shape.size()));

    std::vector<Ort::Value> output_tensors = session.Run(Ort::RunOptions{ nullptr }, &input_names.at(0), input_tensors.data(), input_tensors.size(), output_names.data(), output_names.size());
    if (output_tensors.size() != num_output_nodes)
    {
        std::cerr << "output tensors size not equal to num_output_nodes\n";
        return;
    }
    if (!output_tensors.front().IsTensor())
    {
        std::cerr << "output tensor is not a tensor\n";
        return;
    }

    auto output_shape = output_tensors.front().GetTensorTypeAndShapeInfo().GetShape();
    std::vector<int> mask_sz(output_shape.begin(), output_shape.end());
    cv::Mat res(mask_sz, CV_32F, output_tensors.front().GetTensorMutableData<float>());

    res = res.reshape(0, { static_cast<int>(output_shape.at(1)), static_cast<int>(output_shape.at(2) * output_shape.at(3)) }).t();
    res = res.reshape(static_cast<int>(output_shape.at(1)), { static_cast<int>(output_shape.at(2)), static_cast<int>(output_shape.at(3)) });

    cv::Mat argmax_image(res.rows, res.cols, CV_8UC1);

    for (int row = 0; row < res.rows; row++)
    {
        for (int col = 0; col < res.cols; col++)
        {
            float* ptr = const_cast<float*>(res.ptr<float>(row, col));
            int max_idx{ 0 };
            float max_value = ptr[0];
            for (int i = 1; i < res.channels(); i++)
            {
                if (ptr[i] > max_value)
                {
                    max_value = ptr[i];
                    max_idx = i;
                }
            }
            argmax_image.at<uchar>(row, col) = max_idx;
        }
    }

    double mask_sum = cv::sum(argmax_image)[0];
    if (mask_sum < 100)
    {
        std::cerr << "mask sum less than 100\n";
        return;
    }

    cv::Mat inkMask = cv::Mat::zeros(argmax_image.size(), CV_8UC1);
    cv::Mat referMask = cv::Mat::zeros(argmax_image.size(), CV_8UC1);
    for (int row = 0; row < argmax_image.rows; row++)
    {
        for (int col = 0; col < argmax_image.cols; col++)
        {
            if (argmax_image.at<uchar>(row, col) == 1)
            {
                inkMask.at<uchar>(row, col) = 255;
            }
            else if (argmax_image.at<uchar>(row, col) == 2)
            {
                referMask.at<uchar>(row, col) = 255;
            }
        }
    }
    auto possible_lines = GetPossibleLines(referMask);

    if (possible_lines.empty())
    {
        std::cerr << "possibleLines empty\n";
        return;
    }
    double max_dist = 0;
    int max_index = 0;
    for (int i = 0; i < possible_lines.size(); i++)
    {
        double dist_i = possible_lines.at(i).at(0).y > possible_lines.at(i).at(1).y ? possible_lines.at(i).at(0).y : possible_lines.at(i).at(1).y;

        CaculateDistance(possible_lines.at(i).at(0), possible_lines.at(i).at(1));

        if (dist_i < 768 * 0.15)
        {
            continue;
        }

        if (possible_lines.at(i).at(0).y < 768 * 0.5)
        {
            continue;
        }

        cv::Point2f p1((possible_lines.at(i).at(0).x + possible_lines.at(i).at(1).x) / 2, (possible_lines.at(i).at(0).y + possible_lines.at(i).at(1).y) / 2);
        if (p1.x + 30 > image_gray.cols || p1.y + 30 > image_gray.rows)
        {
            continue;
        }

        cv::Rect rect(p1.x, p1.y, 30, 30);
        cv::Mat roi = image_gray(rect);
        double mean_color = cv::mean(roi)[0];
        if (mean_color > 180)
        {
            continue;
        }

        if (dist_i > max_dist)
        {
            max_dist = dist_i;
            max_index = i;
        }
    }

    std::vector<float> line_res;
    if (max_index != -1)
    {
        line_res = FitLine(possible_lines.at(max_index));
    }

    if (line_res.at(0) >= 0.0 && line_res.at(1) >= 0.0 && line_res.at(2) >= 0.0 && line_res.at(3) >= 0.0)
    {

    }
    else
    {
        std::cerr << "line_res invalid\n";
        return;
    }

    if (line_res.empty())
    {
        std::cerr << "line_res empty\n";
        return;
    }
    cv::line(image, cv::Point(line_res.at(0), line_res.at(1)), cv::Point(line_res.at(2), line_res.at(3)), cv::Scalar(0, 0, 255), 2);

    possible_lines = GetPossibleLines(inkMask);
    double dgree_ref = atan((line_res.at(3) - line_res.at(1)) / (line_res.at(2) - line_res.at(0))) * 180 / CV_PI;

    double min_dist = 100000;
    int min_index = -1;
    for (int i = 0; i < possible_lines.size(); i++)
    {
        double dist_i = CaculateDistance(possible_lines.at(i).at(0), possible_lines.at(i).at(1));
        if (dist_i < 768 * 0.15)
        {
            continue;
        }

        double max_ref_y = line_res.at(1) > line_res.at(3) ? line_res.at(1) : line_res.at(3);
        double max_ink_y = possible_lines.at(i).at(0).y < possible_lines.at(i).at(1).y ? possible_lines.at(i).at(1).y : possible_lines.at(i).at(0).y;
        if (max_ink_y > max_ref_y)
        {
            continue;
        }

        double slope_i = (possible_lines.at(i).at(1).y - possible_lines.at(i).at(0).y) / (possible_lines.at(i).at(1).x - possible_lines.at(i).at(0).x);
        double dgree_i = atan(slope_i) * 180 / CV_PI;
        if (std::abs(dgree_i - dgree_ref) > 10)
        {
            continue;
        }

        if (dist_i > max_dist)
        {
            max_dist = dist_i;
            max_index = i;
        }

        double ref_ink_dist_y = std::abs(max_ink_y - max_ref_y);
        if (min_dist > ref_ink_dist_y)
        {
            min_dist = ref_ink_dist_y;
            min_index = i;
        }
    }

    if (min_index != -1)
    {
        line_res = FitLine(possible_lines.at(min_index));
    }
    else
    {
        std::cerr << "min_index invalid\n";
        return;
    }
    std::vector<float> inkLine;
    if (line_res.at(0) >= 0.0 && line_res.at(1) >= 0.0 && line_res.at(2) >= 0.0 && line_res.at(3) >= 0.0)
    {
        inkLine = line_res;
    }
    else
    {
        std::cerr << "line_res invalid\n";
        return;
    }

    if (inkLine.empty())
    {
        std::cerr << "inkLine empty\n";
        return;
    }
    cv::line(image, cv::Point(inkLine.at(0), inkLine.at(1)), cv::Point(inkLine.at(2), inkLine.at(3)), cv::Scalar(0, 255, 0), 2);

    cv::imshow("inkMask", inkMask);
    cv::imshow("referMask", referMask);
    cv::imshow("image", image);
    cv::waitKey(0);
}

void ShowImage(const std::string& image_path)
{
    cv::Mat image = cv::imread(image_path);
    cv::imshow("image", image);
    std::cerr << "rows: " << image.rows << ", cols: " << image.cols << "\n";
    cv::waitKey(0);
}