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
#include <iostream>
#include <algorithm>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "QVector"

int main(int argc, char* argv[])
{
    std::cerr << "Hello world!\n";
    
    cv::Mat img = cv::imread("C:/Users/anony/Desktop/1.jpg");
    cv::imshow("image", img);
    std::cerr << img.cols << ", " << img.rows << "\n";
    cv::waitKey(100);

    QVector<int> arr{1, 2, 3};
    std::for_each(arr.begin(), arr.end(), [](int num){std::cerr << num << ", ";});
    
    
    return 0;
}