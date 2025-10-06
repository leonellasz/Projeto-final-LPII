#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <atomic>
#include <cstdlib>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
std::atomic<bool> connected(true);

void receive_messages(int client_fd) {
    char buffer[1024];
    while (connected) {
        memset(buffer, 0, 1024);
        ssize_t valread = recv(client_fd, buffer, 1024, 0);
        if (valread > 0) {
            std::cout << "\r" << std::string(buffer, valread) << std::endl;
            std::cout << "> " << std::flush;
        } else if (valread <= 0) {
            if (connected) {
                std::cout << "\r[INFO] Conexao com o servidor perdida." << std::endl;
                connected = false;
            }
            break;
        }
    }
}

void handle_login(int client_fd, std::string& out_username) {
    std::string username, password, command;
    std::cout << "Usuario: ";
    std::getline(std::cin, username);
    std::cout << "Senha: ";
    std::getline(std::cin, password);

    command = "AUTH " + username + " " + password;
    send(client_fd, command.c_str(), command.length(), 0);

    char buffer[1024] = {0};
    ssize_t valread = recv(client_fd, buffer, 1024, 0);
    std::string response(buffer, valread);

    if (response == "AUTH_SUCCESS") {
        out_username = username;
    } else if (response == "AUTH_FAIL_LOGGEDIN") {
        std::cout << "Erro: Este usuario ja esta logado em outro cliente." << std::endl;
    } else {
        std::cout << "Login falhou. Verifique usuario e senha." << std::endl;
    }
}

void handle_register(int client_fd) {
    std::string username, password, command;
    std::cout << "Escolha um novo usuario: ";
    std::getline(std::cin, username);
    std::cout << "Escolha uma nova senha: ";
    std::getline(std::cin, password);

    command = "REGISTER " + username + " " + password;
    send(client_fd, command.c_str(), command.length(), 0);

    char buffer[1024] = {0};
    ssize_t valread = recv(client_fd, buffer, 1024, 0);
    std::string response(buffer, valread);

    if (response == "REGISTER_SUCCESS") {
        std::cout << "Usuario registrado com sucesso! Pressione Enter para voltar ao menu." << std::endl;
    } else if (response == "REGISTER_FAIL_EXISTS") {
        std::cout << "Erro: Este nome de usuario ja existe. Pressione Enter para voltar ao menu." << std::endl;
    } else {
        std::cout << "Erro desconhecido durante o registro. Pressione Enter para voltar ao menu." << std::endl;
    }
    std::string dummy;
    std::getline(std::cin, dummy);
}

int main() {
    int client_fd = 0; 
    struct sockaddr_in serv_addr;
    
    std::cout << "--- Cliente Chat CLI (Final) ---" << std::endl;

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { std::cerr << "Erro no socket\n"; return -1; }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) { std::cerr << "Endereco invalido\n"; return -1; }

    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "\n[ERRO] Falha na conexao." << std::endl;
        return -1;
    }

    std::string username = ""; 
    while (username.empty() && connected) {
        std::cout << "\nEscolha uma opcao:\n1. Fazer Login\n2. Registrar\n> ";
        std::string choice;
        if (!std::getline(std::cin, choice)) break; 

        if (choice == "1") {
            handle_login(client_fd, username);
        } else if (choice == "2") {
            handle_register(client_fd);
            system("clear");
        } else {
            std::cout << "Opcao invalida." << std::endl;
        }
    }
    
    if (!username.empty()) {
        std::cout << "\nAutenticado com sucesso! Bem-vindo ao chat, " << username << "!" << std::endl;
        std::cout << "Digite 'quit' a qualquer momento para sair." << std::endl;
        std::cout << "Para enviar uma mensagem privada, use: /priv <usuario> <mensagem>" << std::endl;
        
        std::thread receiver_thread(receive_messages, client_fd);
        
        std::string message;
        while (connected) {
            std::cout << "> " << std::flush;
            if (!std::getline(std::cin, message)) {
                connected = false;
                break;
            }
            if (!connected) break;
            
            if (message == "quit") {
                connected = false;
            } else if (!message.empty()) {
                send(client_fd, message.c_str(), message.length(), 0);
            }
        }

        shutdown(client_fd, SHUT_RDWR); 
        if (receiver_thread.joinable()) {
            receiver_thread.join();
        }
    }

    close(client_fd);
    std::cout << "Conexao encerrada." << std::endl;
    
    return 0;
}