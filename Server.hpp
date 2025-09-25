#pragma once
#include <vector>
#include <thread>
#include <mutex>

class Server {
public:
    Server(int port);
    void start();

private:
    void accept_connections();
    // Outros membros: socket do servidor, lista de clientes, mutex, etc.
    int server_socket_;
    int port_;
};