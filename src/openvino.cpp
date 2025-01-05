#include "openvino.hpp"

static int global_height{ 0 };
static int global_width{ 0 };

static std::vector<float> FitLine(std::vector<cv::Point2f> points)
{
    cv::Vec4f lines;
    cv::fitLine(points, lines, cv::DIST_L2, 0, 0.01, 0.01);
    float k = lines[1] / lines[0];
    float lefty = (-lines[2] * k) + lines[3];
    float righty = ((768 - 1 - lines[2]) * k) + lines[3];

    return { 0.0, lefty, 768.0, righty, k };
}

static double CaculateDistance(const cv::Point2f& point1, const cv::Point2f& point2)
{
    return std::sqrt(std::pow(point1.x - point2.x, 2) + std::pow(point1.y - point2.y, 2));
}

static std::vector<std::vector<cv::Point2f>> GetPossibleLines(const cv::Mat& mask)
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

static void Preprocess(const cv::Mat& img, cv::Mat& rgb_img, cv::Mat& gray_img, cv::Mat& blob)
{
    if (img.channels() != 3)
    {
        cv::cvtColor(img, rgb_img, cv::COLOR_GRAY2BGR);
        gray_img = img.clone();
    }
    else
    {
        rgb_img = img.clone();
        cv::cvtColor(img, gray_img, cv::COLOR_BGR2GRAY);
    }

    cv::resize(rgb_img, rgb_img, cv::Size(global_width, global_height));
    cv::resize(gray_img, gray_img, cv::Size(global_width, global_height));
    rgb_img.convertTo(rgb_img, CV_32F);
    rgb_img = ((rgb_img / 255.0f) - 0.5f) / 0.5f;

    blob = cv::dnn::blobFromImage(rgb_img, 1.0, cv::Size(global_width, global_height), cv::Scalar(0, 0, 0), true, false);
}

static bool ProcessReferMask(cv::Mat referMask, cv::Mat image_gray, std::vector<float>& line_res)
{
    auto possible_lines = GetPossibleLines(referMask);

    if (possible_lines.empty())
    {
        std::cerr << "possibleLines empty\n";
        return false;
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
        if (p1.x + 30 > global_width || p1.y + 30 > global_height)
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

    if (max_index == -1)
    {
        return false;
    }

    line_res = FitLine(possible_lines.at(max_index));

    if (line_res.empty())
    {
        std::cerr << "line_res empty\n";
        return false;
    }

    bool result = std::all_of(line_res.begin(), line_res.end() - 1, [](float val) { return val >= 0.0; });
    if (!result)
    {
        std::cerr << "line_res invalid\n";
        return false;
    }

    return true;
}

static bool ProcessInkMask(cv::Mat inkMask, cv::Mat image_gray, const std::vector<float>& refer_line, std::vector<float>& line_res)
{
    auto possible_lines = GetPossibleLines(inkMask);
    double dgree_ref = atan((refer_line.at(3) - refer_line.at(1)) / (refer_line.at(2) - refer_line.at(0))) * 180 / CV_PI;

    double min_dist = 100000;
    int min_index = -1;
    double max_dist = 0;
    int max_index = 0;
    for (int i = 0; i < possible_lines.size(); i++)
    {
        double dist_i = CaculateDistance(possible_lines.at(i).at(0), possible_lines.at(i).at(1));
        if (dist_i < 768 * 0.15)
        {
            continue;
        }

        double max_ref_y = refer_line.at(1) > refer_line.at(3) ? refer_line.at(1) : refer_line.at(3);
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

    if (min_index == -1)
    {
        std::cerr << "min_index invalid\n";
        return false;

    }

    line_res = FitLine(possible_lines.at(min_index));

    if (line_res.empty())
    {
        std::cerr << "line_res empty\n";
        return false;
    }

    bool result = std::all_of(line_res.begin(), line_res.end() - 1, [](float val) { return val >= 0.0; });
    if (!result)
    {
        std::cerr << "line_res invalid\n";
        return false;
    }

    return true;
}

static bool Postprocess(cv::Mat res, cv::Mat image_gray, std::vector<float>& refer_line, std::vector<float>& ink_line)
{
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
        return false;
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
    bool result{ false };
    result = ProcessReferMask(referMask, image_gray, refer_line);
    if (!result)
    {
        std::cerr << "ProcessReferMask failed\n";
        return false;
    }

    result = ProcessInkMask(inkMask, image_gray, refer_line, ink_line);
    if (!result)
    {
        std::cerr << "ProcessInkMask failed\n";
        return false;
    }

    return true;
}

void TestOpenVino()
{
    std::cerr << ov::get_openvino_version() << std::endl;

    std::string model_path{ "D:/Robot/models/pp_liteseg_stdc1_softmax_20241021.onnx" };

    ov::Core core;

    auto model = core.read_model(model_path);
    ov::AnyMap config = {
        {ov::hint::performance_mode.name(), ov::hint::PerformanceMode::LATENCY}
    };

    auto begin = std::chrono::system_clock::now();
    auto compiled_model = core.compile_model(model, "GPU", config);
    auto duration = std::chrono::system_clock::now() - begin;
    std::cerr << "compile model's time: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "\n";

    // 4. 查询模型输入输出信息
    std::cout << "Model Inputs:" << std::endl;
    for (const auto& input : compiled_model.inputs())
    {
        std::cout << "  - Name: " << input.get_any_name() << std::endl;
        std::cout << "    Shape: " << input.get_shape() << std::endl;
    }

    std::cout << "Model Outputs:" << std::endl;
    for (const auto& output : compiled_model.outputs())
    {
        std::cout << "  - Name: " << output.get_any_name() << std::endl;
        std::cout << "    Shape: " << output.get_shape() << std::endl;
    }

    auto inputs = compiled_model.inputs();
    auto outputs = compiled_model.outputs();
    auto input_shape = inputs[0].get_shape();

    global_height = input_shape[2];
    global_width = input_shape[3];

    std::string image_path{ "C:/Users/anony/Documents/GitHub/cpp-win/data/LineCam_6_2024-12-23-15-59-55.png" };

    cv::Mat image = cv::imread(image_path);
    if (image.empty())
    {
        std::cerr << "load image failed\n";
        return;
    }

    cv::Mat image_gray, image_rgb, blob;
    Preprocess(image, image_rgb, image_gray, blob);
    image = image_rgb;

    ov::InferRequest infer_request = compiled_model.create_infer_request();
    ov::Tensor input_tensor(ov::element::f32, input_shape, blob.ptr<float>());

    infer_request.set_tensor(inputs[0].get_any_name(), input_tensor);

    infer_request.infer();

    auto output = infer_request.get_output_tensor(0);
    auto output_shape = output.get_shape();
    std::vector<int> mask_size(output_shape.begin(), output_shape.end());
    cv::Mat res(mask_size, CV_32F, output.data<float>());

    res = res.reshape(0, { static_cast<int>(output_shape.at(1)), static_cast<int>(output_shape.at(2) * output_shape.at(3)) }).t();
    res = res.reshape(static_cast<int>(output_shape.at(1)), { static_cast<int>(output_shape.at(2)), static_cast<int>(output_shape.at(3)) });

    std::vector<float> ink_line;
    std::vector<float> refer_line;
    bool result = Postprocess(res, image_gray, refer_line, ink_line);
    if (!result)
    {
        std::cerr << "Postprocess failed\n";
        return;
    }

    cv::line(image, cv::Point(refer_line.at(0), refer_line.at(1)), cv::Point(refer_line.at(2), refer_line.at(3)), cv::Scalar(0, 0, 255), 2);
    cv::line(image, cv::Point(ink_line.at(0), ink_line.at(1)), cv::Point(ink_line.at(2), ink_line.at(3)), cv::Scalar(0, 255, 0), 2);

    cv::imshow("image", image);
    cv::waitKey(0);
}