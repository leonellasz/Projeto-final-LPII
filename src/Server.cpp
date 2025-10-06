// ===== COLE ESTE CÓDIGO C++ EM  src/Server.cpp =====

#include "Server.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

Server::Server(Logger& log_ref, int port) 
    : logger(log_ref), addrlen(sizeof(address)), running(false) {
    std::ifstream user_file("users.txt");
    std::string line;
    if (user_file.is_open()) {
        while (std::getline(user_file, line)) {
            std::stringstream ss(line);
            std::string username, password;
            if (std::getline(ss, username, ':') && std::getline(ss, password)) {
                user_credentials_[username] = password;
            }
        }
        user_file.close();
        logger.log("[INFO] Credenciais de " + std::to_string(user_credentials_.size()) + " usuario(s) carregadas.");
    } else {
        logger.log("[WARNING] Arquivo 'users.txt' nao encontrado.");
    }
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { logger.log("[ERROR] Falha na criacao do socket."); exit(EXIT_FAILURE); }
    logger.log("[INFO] Socket do servidor criado com sucesso.");
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { logger.log("[WARNING] setsockopt falhou."); }
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(port); 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { logger.log("[ERROR] Falha no bind."); exit(EXIT_FAILURE); }
    logger.log("[INFO] Bind realizado com sucesso na porta " + std::to_string(port));
}
Server::~Server() { if (server_fd > 0) { close(server_fd); } }
void Server::start() {
    if (listen(server_fd, 1024) < 0) { logger.log("[ERROR] Falha no listen."); exit(EXIT_FAILURE); }
    running = true;
    logger.log("[INFO] Servidor escutando por conexoes na porta " + std::to_string(ntohs(address.sin_port)));
    run_accept_loop();
}
void Server::stop() {
    running = false;
    logger.log("[WARNING] Servidor sinalizado para parar...");
    if (server_fd > 0) { shutdown(server_fd, SHUT_RDWR); close(server_fd); }
}
void Server::run_accept_loop() {
    while (running) {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) { if (running) logger.log("[ERROR] Falha no accept."); continue; }
        logger.log("[CONN] Nova conexao de " + std::string(inet_ntoa(address.sin_addr)) + ", Socket: " + std::to_string(new_socket));
        std::thread t(&Server::handle_new_client, this, new_socket);
        t.detach(); 
    }
}
void Server::handle_new_client(int client_socket) {
    char buffer[1024];
    bool is_authenticated = false;
    std::string username;
    while (!is_authenticated && running) {
        memset(buffer, 0, 1024);
        ssize_t valread = read(client_socket, buffer, 1024);
        if (valread <= 0) {
            logger.log("[CONN] Cliente " + std::to_string(client_socket) + " desconectou antes de autenticar.");
            close(client_socket);
            return;
        }
        std::stringstream ss(std::string(buffer, valread));
        std::string command, user, pass;
        ss >> command >> user >> pass;
        if (command == "AUTH") {
            std::lock_guard<std::mutex> lock(clients_mutex_);
            if (user_credentials_.count(user) && user_credentials_.at(user) == pass) {
                if (username_to_socket_.count(user)) {
                    send(client_socket, "AUTH_FAIL_LOGGEDIN", 18, 0);
                    logger.log("[AUTH] Falha: Usuario '" + user + "' ja esta logado.");
                } else {
                    is_authenticated = true;
                    username = user;
                    send(client_socket, "AUTH_SUCCESS", 12, 0);
                    logger.log("[AUTH] Usuario '" + username + "' autenticado com sucesso.");
                }
            } else {
                send(client_socket, "AUTH_FAIL", 9, 0);
                logger.log("[AUTH] Falha na autenticacao para o usuario '" + user + "'.");
            }
        } else if (command == "REGISTER") {
            std::lock_guard<std::mutex> lock(clients_mutex_);
            if (user_credentials_.count(user)) {
                send(client_socket, "REGISTER_FAIL_EXISTS", 20, 0);
            } else {
                user_credentials_[user] = pass;
                std::ofstream user_file("users.txt", std::ios::app);
                if (user_file.is_open()) user_file << user << ":" << pass << std::endl;
                send(client_socket, "REGISTER_SUCCESS", 16, 0);
                logger.log("[REGISTER] Usuario '" + user + "' registrado com sucesso.");
            }
        }
    }
    if (!is_authenticated) { return; }
    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        authenticated_users_[client_socket] = username;
        username_to_socket_[username] = client_socket;
    }
    broadcast_message("[SERVER] " + username + " entrou no chat!", -1);
    while (running) {
        try {
            memset(buffer, 0, 1024);
            ssize_t valread = read(client_socket, buffer, 1024);
            if (valread > 0) {
                std::string message(buffer, valread);
                if (message.rfind("/priv ", 0) == 0) {
                    handle_private_message(client_socket, message);
                } else {
                    std::string formatted_msg = "[" + username + "]: " + message;
                    logger.log("[DATA] Mensagem de '" + username + "': \"" + message + "\"");
                    broadcast_message(formatted_msg, client_socket);
                }
            } else { break; }
        } catch (const std::out_of_range& oor) {
            logger.log("[WARNING] Excecao (out_of_range) na thread do cliente " + username + ". Provavelmente uma desconexão simultânea. Finalizando thread de forma segura.");
            break; 
        } catch (const std::exception& e) {
            logger.log("[ERROR] Excecao inesperada na thread do cliente " + username + ": " + e.what());
            break; 
        }
    }
    logger.log("[CONN] Usuario '" + username + "' desconectou.");
    broadcast_message("[SERVER] " + username + " saiu do chat.", -1);
    remove_client(client_socket);
    close(client_socket);
}
void Server::handle_private_message(int sender_socket, const std::string& full_message) {
    std::stringstream ss(full_message);
    std::string command, target_username, private_message;
    ss >> command >> target_username;
    std::getline(ss, private_message);
    if (!private_message.empty() && private_message[0] == ' ') { private_message.erase(0, 1); }
    std::string sender_username;
    bool sent = false;
    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        sender_username = authenticated_users_.at(sender_socket);
        if (username_to_socket_.count(target_username)) {
            int target_socket = username_to_socket_.at(target_username);
            std::string formatted_msg = "(Privado de " + sender_username + "): " + private_message;
            send(target_socket, formatted_msg.c_str(), formatted_msg.length(), 0);
            sent = true;
        }
    }
    if (sent) {
        std::string confirmation_msg = "[SERVER] Mensagem enviada para " + target_username + ".";
        send(sender_socket, confirmation_msg.c_str(), confirmation_msg.length(), 0);
        logger.log("[PRIV] '" + sender_username + "' enviou msg para '" + target_username + "'.");
    } else {
        std::string error_msg = "[SERVER] Erro: Usuario '" + target_username + "' nao encontrado ou offline.";
        send(sender_socket, error_msg.c_str(), error_msg.length(), 0);
        logger.log("[PRIV] '" + sender_username + "' tentou enviar msg para usuario offline '" + target_username + "'.");
    }
}
void Server::broadcast_message(const std::string& message, int sender_socket) {
    std::vector<int> targets;
    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        for (auto const& [socket, user] : authenticated_users_) {
            if (socket != sender_socket) {
                targets.push_back(socket);
            }
        }
    }
    for (int target_socket : targets) {
        send(target_socket, message.c_str(), message.length(), 0);
    }
}
void Server::remove_client(int client_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    if (authenticated_users_.count(client_socket)) {
        const std::string username = authenticated_users_.at(client_socket);
        authenticated_users_.erase(client_socket);
        username_to_socket_.erase(username);
    }
}