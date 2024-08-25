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

int main(int argc, char* argv[])
{
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