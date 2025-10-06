#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic> // Para o booleano de parada

// Classe Monitor que encapsula toda a lógica de logging concorrente
class Logger {
public:
    // Método estático para obter a única instância (Singleton)
    static Logger& getInstance();

    // Método que as threads usarão para enfileirar uma mensagem de log
    void log(const std::string& message);

private:
    // Construtor e destrutor privados
    Logger();
    ~Logger();

    // Prevenir cópias para garantir que é um Singleton
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Função que será executada pela thread consumidora
    void processQueue();

    // Membros para sincronização e dados
    std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<std::string> message_queue_;
    std::thread consumer_thread_;
    std::atomic<bool> stop_flag_{false}; // Sinaliza para a thread terminar
};