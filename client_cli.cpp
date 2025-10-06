#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

// Definir a porta e o endereço do servidor (deve corresponder ao Server.hpp)
#define SERVER_IP "127.0.0.1"
#define PORT 8080

int main(int argc, char const *argv[]) {
    int client_fd = 0;
    struct sockaddr_in serv_addr;
    std::string message;
    
    std::cout << "--- Cliente Chat CLI (Protótipo Etapa 2) ---" << std::endl;

    // 1. Criação do socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "\n[ERRO] Falha na criacao do socket do cliente." << std::endl;
        return -1;
    }

    // Configura o endereço do servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converte o endereço IPv4 e checa validade
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "\n[ERRO] Endereco Invalido/Nao suportado." << std::endl;
        return -1;
    }

    // 2. Conexão ao servidor
    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "\n[ERRO] Falha na conexao. O servidor esta rodando na porta " << PORT << "?" << std::endl;
        return -1;
    }

    std::cout << "Conectado com sucesso ao servidor " << SERVER_IP << ":" << PORT << "." << std::endl;

    // 3. Loop de Envio e Recebimento
    std::cout << "Digite sua mensagem (ou 'quit' para sair):" << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, message);

        if (message == "quit") {
            break;
        }
        
        if (message.empty()) {
            continue;
        }

        // Envia a mensagem
        send(client_fd, message.c_str(), message.length(), 0);
        std::cout << "Mensagem enviada. Esperando ACK..." << std::endl;

        // Recebe a confirmação (ACK) do servidor
        char buffer[1024] = {0};
        ssize_t valread = recv(client_fd, buffer, 1024, 0);
        
        if (valread > 0) {
            std::cout << "[ACK] Servidor respondeu: " << std::string(buffer, valread) << std::endl;
        } else if (valread == 0) {
             std::cout << "[INFO] Servidor encerrou a conexão." << std::endl;
             break;
        } else {
             std::cerr << "[ERRO] Erro ao receber dados." << std::endl;
        }
    }

    // 4. Fechar a conexão
    close(client_fd);
    std::cout << "Conexao encerrada." << std::endl;
    
    return 0;
}