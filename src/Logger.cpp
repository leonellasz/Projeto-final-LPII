
#include "Logger.hpp"
#include <chrono>    
#include <iomanip>   
#include <sstream>   
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}


Logger::Logger() {
     
    log_file_.open("chat_server.log", std::ios::app);
    consumer_thread_ = std::thread(&Logger::processQueue, this);
}


Logger::~Logger() {
    stop_flag_ = true;
    cv_.notify_one();
    if (consumer_thread_.joinable()) {
        consumer_thread_.join();
    }
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

void Logger::log(const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        message_queue_.push(message);
    } 
    cv_.notify_one();
}


void Logger::processQueue() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx_);
        
        cv_.wait(lock, [this] { return !message_queue_.empty() || stop_flag_; });

        if (stop_flag_ && message_queue_.empty()) {
            break;
        }

        
        std::string message = message_queue_.front();
        message_queue_.pop();
        
        lock.unlock();

        
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X"); 
        

        if (log_file_.is_open()) {
            log_file_ << ss.str() << " | " << message << std::endl;
        }

      
    }
}