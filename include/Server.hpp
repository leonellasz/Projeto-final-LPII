

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Logger.hpp"

#define PORT 8080 
#define MAX_PENDING_CONNECTIONS 10

class Server {
private:
    Logger& logger; 
    
    int server_fd;
    struct sockaddr_in address;
    int addrlen;
    bool running;

    std::mutex clients_mutex_; 
    std::map<std::string, std::string> user_credentials_;
    std::map<int, std::string> authenticated_users_;
    std::map<std::string, int> username_to_socket_;

public:

    Server(Logger& log_ref, int port = PORT); 
    ~Server();
    void start();
    void stop(); 
    void run_accept_loop();

private:
    void handle_new_client(int client_socket); 
    void broadcast_message(const std::string& message, int sender_socket);
    void remove_client(int client_socket);
    void handle_private_message(int sender_socket, const std::string& full_message); 
};

#endif 