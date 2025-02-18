/**
 * @file multi_thread.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-02-14
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "base_headers.hpp"

class Task
{
public:
    Task() = default;
    virtual ~Task() = default;

    void CreateTask()
    {
        std::thread temp_thread = std::thread([this]() {
            while (true)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cerr << "Task is running\n";
            }
        });

        m_thread = std::move(temp_thread);
    }

    void JoinTask()
    {
        m_thread.join();
    }

    void ThreadExitWithoutJoin()
    {
        auto t = std::thread([] {
            std::cout << "Hello world!!!\n";
        });

        std::this_thread::sleep_for(std::chrono::seconds(1));

        t.join();  // 如果不调用join()/detach()，则会调用std::terminate()函数
    }

    void TaskWithConditionVariable()
    {
        auto t = [this]() {
            std::this_thread::sleep_for(std::chrono::seconds(7));
            Stop();
        };

        Up();
        std::thread(t).detach();
    }

    std::string GetStatus() const
    {
        return status;
    }

private:
    bool Up()
    {
        std::cerr << "Up\n";
        status = "Up";
        return true;
    }

    bool Down()
    {
        std::cerr << "Down\n";
        status = "Down";
        return true;
    }

    bool Stop()
    {
        std::cerr << "Stop\n";
        status = "Stop";
        return true;
    }

private:
    std::thread m_thread;
    std::mutex mtx;
    std::condition_variable cv;
    std::string status;
};