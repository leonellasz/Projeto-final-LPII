#include "Server.hpp"
#include "Logger.hpp"
#include <signal.h> // Para lidar com sinais (ex: Ctrl+C)
#include <iostream>

Server* chat_server = nullptr;

// Função para lidar com o sinal de interrupção (Ctrl+C)
void signal_handler(int signum) {
    if (chat_server) {
        chat_server->stop();
    }
    // O destrutor do Logger será chamado automaticamente.
    exit(signum);
}

int main(int argc, char const *argv[]) {
    // Configura o handler para SIGINT (Ctrl+C)
    signal(SIGINT, signal_handler);

    // 1. Inicializa o Logger (Usando o padrão Singleton)
    // O erro 'Logger::Logger() is private' foi corrigido por esta linha.
    Logger& my_logger = Logger::getInstance();
    
    // 2. Logging Inicial (Adaptado para o método 'log' existente)
    my_logger.log("[INFO] Iniciando o sistema concorrente (Etapa 2 - Servidor)...");

    try {
        // Cria e inicia o Servidor na porta padrão (8080)
        Server server_instance(my_logger, PORT);
        chat_server = &server_instance; // Armazena ponteiro para o handler
        server_instance.start();

    } catch (const std::exception& e) {
        // Logging de erro adaptado
        my_logger.log("[ERROR] Excecao nao tratada no main: " + std::string(e.what()));
        return 1;
    }

    // O programa só sairá quando o loop do servidor for interrompido pelo stop()
    my_logger.log("[INFO] Servidor finalizado. Saindo do programa.");
    return 0;
}