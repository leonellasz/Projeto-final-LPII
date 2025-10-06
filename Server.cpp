// Servidor Básico TCP (Tema A)

#include "Server.hpp"
#include <cstring> // Para memset
#include <arpa/inet.h> // Para inet_ntoa, etc.

// Construtor
Server::Server(Logger& log_ref, int port) 
    : logger(log_ref), running(false), addrlen(sizeof(address)) {
    
    // 1. Criação do socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        logger.log("[ERROR] Falha na criacao do socket do servidor.");
        exit(EXIT_FAILURE);
    }
    logger.log("[INFO] Socket do servidor criado com sucesso.");

    // Configuração de Reuso de Porta (REUSEADDR|REUSEPORT)
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        logger.log("[WARNING] setsockopt falhou. Continuando...");
    }

    // Configurações do endereço do servidor
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(port); 
    
    // 2. Bind: Anexar o socket ao endereço e porta
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        logger.log("[ERROR] Falha no bind. Porta ja em uso ou permissao negada.");
        exit(EXIT_FAILURE);
    }
    logger.log("[INFO] Bind realizado com sucesso na porta " + std::to_string(port));
}

// Destrutor
Server::~Server() {
    if (server_fd > 0) {
        close(server_fd);
    }
}

void Server::start() {
    // 3. Listen: Coloca o socket em modo de escuta (listening)
    if (listen(server_fd, MAX_PENDING_CONNECTIONS) < 0) {
        logger.log("[ERROR] Falha no listen.");
        exit(EXIT_FAILURE);
    }

    running = true;
    logger.log("[INFO] Servidor escutando por conexoes na porta " + std::to_string(ntohs(address.sin_port)));

    // Inicia o loop principal de aceitação de conexões
    run_accept_loop();
}

void Server::stop() {
    running = false;
    logger.log("[WARNING] Servidor sinalizado para parar...");
    // Fechar o socket força o 'accept' a falhar, interrompendo o loop
    if (server_fd > 0) {
        close(server_fd);
    }
}

void Server::run_accept_loop() {
    while (running) {
        int new_socket;
        
        // 4. Accept: Espera por uma nova conexão
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        
        if (new_socket < 0) {
            if (running) {
                logger.log("[ERROR] Falha no accept.");
            }
            continue;
        }

        // Uma nova conexão foi aceita
        std::string client_ip = inet_ntoa(address.sin_addr);
        std::string log_msg = "Nova conexao aceita! Cliente: " + client_ip + 
                              ", Socket ID: " + std::to_string(new_socket);
        logger.log("[CONN] " + log_msg);
        
        // 5. Lidar com o cliente em uma thread separada
        std::thread t(&Server::handle_new_client, this, new_socket);
        t.detach(); 
    }
}

void Server::handle_new_client(int client_socket) {
    char buffer[1024] = {0};
    ssize_t valread;
    
    // Leitura mínima
    valread = read(client_socket, buffer, 1024);

    if (valread > 0) {
        std::string message(buffer, valread);
        std::string log_msg = "Mensagem recebida do cliente (" + std::to_string(client_socket) + 
                              "): \"" + message + "\"";
        logger.log("[DATA] " + log_msg);
        
        // Envia ACK de volta ao cliente (Retransmissão/Resposta mínima para Etapa 2)
        std::string ack_msg = "ACK: Mensagem recebida pelo servidor.";
        send(client_socket, ack_msg.c_str(), ack_msg.length(), 0);
        
    } else if (valread == 0) {
        logger.log("[WARNING] Cliente " + std::to_string(client_socket) + " desconectou normalmente.");
    } else {
        logger.log("[ERROR] Erro ao ler do socket " + std::to_string(client_socket));
    }
    
    // Fechar a conexão
    close(client_socket);
    logger.log("[CONN] Conexao encerrada com o cliente " + std::to_string(client_socket));
}