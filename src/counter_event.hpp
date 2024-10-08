/*****************************************************************//**
 * \file   counter_event.hpp
 * \brief    + 找到一个更好的方法替换计数法，需要解决的问题：
                + 启动一个设备，瞬时启动，但不知道什么时间停止。所以约定一个时间段，当走完时间段即表示设备运动结束，给出一个信号。
                + 在设备运行期间，主线程不会阻塞，可以随时终止设备的运行
                + 设备运行结束后主线程可以知道设备结束了
 * 
 * \author anony
 * \date   October 2024
 *********************************************************************/
#include "base_headers.hpp"


void Loop();