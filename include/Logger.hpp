

#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <fstream> 

class Logger {
public:
    static Logger& getInstance();
    void log(const std::string& message);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void processQueue();

    std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<std::string> message_queue_;
    std::thread consumer_thread_;
    std::atomic<bool> stop_flag_{false};

    std::ofstream log_file_; 
};