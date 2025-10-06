#include "Server.hpp"
#include "Logger.hpp"
#include <signal.h> 
#include <iostream>

Server* chat_server = nullptr;


void signal_handler(int signum) {
    if (chat_server) {
        chat_server->stop();
    }
    
    exit(signum);
}

int main(int argc, char const *argv[]) {
    
    signal(SIGINT, signal_handler);

   
    Logger& my_logger = Logger::getInstance();
    
   
    my_logger.log("[INFO] Iniciando o sistema concorrente (Etapa Final - Servidor)...");

    try {
       
        Server server_instance(my_logger, PORT);
        chat_server = &server_instance; 
        server_instance.start();

    } catch (const std::exception& e) {
       
        my_logger.log("[ERROR] Excecao nao tratada no main: " + std::string(e.what()));
        return 1;
    }

    
    my_logger.log("[INFO] Servidor finalizado. Saindo do programa.");
    return 0;
}