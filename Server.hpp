#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <string>
#include "Logger.hpp"     // Inclui sua classe de logging

// Define a porta padrão
#define PORT 8080 
// Define o limite de conexões pendentes para o listen()
#define MAX_PENDING_CONNECTIONS 10

class Server {
private:
    // **ORDEM CORRIGIDA para evitar -Wreorder**
    // 1. A referência do Logger deve ser a primeira.
    Logger& logger; 
    
    // 2. Outros membros:
    int server_fd; // File descriptor do socket do servidor
    struct sockaddr_in address; // Estrutura para informações do endereço
    int addrlen; // Tamanho da estrutura de endereço
    bool running; // Flag para controlar o loop principal

public:
    // Construtor: recebe a referência do Logger e a porta
    Server(Logger& log_ref, int port = PORT); 
    ~Server();

    // Métodos de controle
    void start(); // Inicializa o socket, bind, listen e inicia o loop accept
    void stop(); 
    void run_accept_loop(); // O loop principal que chama accept()

private:
    // Método para tratar um novo cliente (executado em uma thread)
    void handle_new_client(int client_socket); 
};

#endif // SERVER_HPP