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
#include <bitset>

// opencv
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"

// Qt
#include "QtWidgets/QWidget"
#include "QtWidgets/QApplication"
#include "QPushButton"

// spdlog
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

// 
#include "GlobalVariable.h"
#include "temp.h"

void InitLogger()
{
    auto logger = spdlog::basic_logger_mt("file_logger", "logs.txt");
    spdlog::flush_every(std::chrono::milliseconds(100));
    spdlog::flush_on(spdlog::level::info);
    logger->info("This is an info message");

}

void TestOpencv(std::string img_path)
{
    auto log = spdlog::get("file_logger");
    cv::Mat img = cv::imread(img_path);
    cv::imshow("image", img);
    std::cerr << img.cols << ", " << img.rows << "\n";
    cv::waitKey(100);

    cv::VideoCapture video_capture;
    video_capture.open(0);

    if (video_capture.isOpened())
    {
        log->info("camera is opened!");
        cv::Mat frame;
        while (true)
        {
            video_capture >> frame;
            cv::imshow("camera", frame);
        }
    }
    else 
    {
        log->error("camera is not opened!");
    }
}

int TestQt(int argc, char* argv[])
{
    QVector<int> arr{1, 2, 3};
    std::for_each(arr.begin(), arr.end(), [](int num){std::cerr << num << "\n";});
    int c_arr[3];
    arr.data();
    for (int i = 0; i < 3; i++)
    {
        std::cerr << c_arr[i] << ",";
    }
    
    QApplication app(argc, argv);
    QWidget w;
    w.show();
    auto obj = new QPushButton;
    std::clog << "QPushButton className(): " << obj->metaObject()->className() << "\n";

    return app.exec();
}

int16_t SpliceByte(uint8_t high_byte, uint8_t low_byte)
{
    uint16_t result_byte = (static_cast<uint16_t>(high_byte) << 8) | low_byte;

    return static_cast<int16_t>(result_byte);
}

void TestBit()
{
    uint8_t val{0b0000'1010};
    uint8_t res = val & 0b0000'0011;

    std::cerr << "val: " << static_cast<int>(val) << ", res: " << static_cast<int>(res) << "\n";

    std::vector<uint8_t> arr{0b0000'1010, 0b0000'0010, 0b0000'0010, 0b0000'0010};
    std::for_each(arr.begin(), arr.end(), [](uint8_t val){std::cerr << static_cast<int>(val) << ",";});

    uint8_t high_byte = arr.at(1);
    uint8_t low_byte = arr.at(2);

    uint16_t result_byte = (static_cast<uint16_t>(high_byte) << 8) | low_byte;
    std::cerr << "hight byte: " << std::bitset<8>(high_byte) << ", low byte: " << std::bitset<8>(low_byte) << ", result: " << std::bitset<16>(result_byte) << "\n";

    std::vector<uint8_t> buf(8, 0);

    buf[0] = static_cast<uint8_t>(buf.size());
    buf[1] = 0x04;
    buf[2] = 0x0F;
    buf[3] = 0x00;
    buf[4] = 0x0A;
    buf[5] = 0x01;
    buf[6] = 0x08;

    buf[7] = std::accumulate(buf.begin(), buf.end(), 0);
    std::for_each(buf.begin(), buf.end(), [](uint8_t val){std::cerr << "val: " << static_cast<int>(val) << ", ";});
}

struct Data_t
{
    std::vector<int> data;
    Data_t()
    {
        data.resize(6);
    }
};

void TestBasic()
{
    Data_t data;
    
    for (int i = 0; i < 6; i++)
    {
        std::cerr << POSITION_HOME_ARR[i] << "\n";
    }

    if (0)
    {
        std::cerr << "0 is true\n";
    }
    else 
    {
        std::cerr << "0 is false\n";
    }

    int temp_val_x = 0;
    int temp_val_y = 0;
    int temp_val_z = 0;
    int temp_val_rx = 0;
    int temp_val_ry = 0;
    int temp_val_rz = 0;

    if (temp_val_x || temp_val_y || temp_val_z || 
        temp_val_rx || temp_val_ry || temp_val_rz)
    {
        std::cerr << "0 is true\n";
    }
    else 
    {
        std::cerr << "0 is false\n";
    }

    temp_val_y = 1;
    if (temp_val_x || temp_val_y || temp_val_z || 
        temp_val_rx || temp_val_ry || temp_val_rz)
    {
        std::cerr << "true\n";
    }
    else 
    {
        std::cerr << "false\n";
    }

    std::vector<int> tmp_arr(10);
    std::vector<int> tmp_arr2{0, 0, 1, 0};
}

int main(int argc, char* argv[])
{
    InitLogger();
    
    TestBasic();

    TestBit();
    return 0;

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
    
    TestOpencv(path);

    return TestQt(argc, argv);
}