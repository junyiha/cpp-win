/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */
// STL
#include <iostream>
#include <algorithm>
#include <string>

// opencv
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"

// Qt
#include "QtWidgets/QWidget"
#include "QtWidgets/QApplication"
#include "QPushButton"

// spdlog
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

void InitLogger()
{
    auto logger = spdlog::basic_logger_mt("file_logger", "logs.txt");
    spdlog::flush_every(std::chrono::milliseconds(100));
    spdlog::flush_on(spdlog::level::info);
    logger->info("This is an info message");

}

int main(int argc, char* argv[])
{
    InitLogger();

    spdlog::info("Welecom cpp win project!!!\n");
    auto logger = spdlog::get("file_logger");
    if (logger)
    {
        logger->warn("This is a warning message from the same logger");
    }
    else 
    {
        spdlog::error("Logger not found");
    }
    std::cerr << "Hello world!\n";
    if (argc <=1)
    {
        std::clog << "Invalid program argument!!!\n";
        return -1;
    }

    std::string path{argv[1]};
    
    cv::Mat img = cv::imread(path);
    cv::imshow("image", img);
    std::cerr << img.cols << ", " << img.rows << "\n";
    cv::waitKey(100);

    QVector<int> arr{1, 2, 3};
    std::for_each(arr.begin(), arr.end(), [](int num){std::cerr << num << "\n";});
    
    QApplication app(argc, argv);
    QWidget w;
    w.show();
    auto obj = new QPushButton;
    std::clog << "QPushButton className(): " << obj->metaObject()->className() << "\n";

    return app.exec();
    // return 0;
}