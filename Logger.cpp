#include "Logger.hpp"
#include <chrono> // Para std::this_thread::sleep_for

Logger& Logger::getInstance() {
    static Logger instance; // Criada apenas uma vez
    return instance;
}

// O construtor inicia a thread consumidora em segundo plano
Logger::Logger() {
    consumer_thread_ = std::thread(&Logger::processQueue, this);
}

// O destrutor sinaliza para a thread parar e espera ela terminar
Logger::~Logger() {
    stop_flag_ = true;  // Sinaliza para a thread parar
    cv_.notify_one();   // Acorda a thread caso ela esteja esperando
    if (consumer_thread_.joinable()) {
        consumer_thread_.join(); // Espera a thread terminar sua execução
    }
}

// Método "Produtor": qualquer thread pode chamar para adicionar um log
void Logger::log(const std::string& message) {
    {
        // Bloqueia o mutex para acesso exclusivo à fila
        std::lock_guard<std::mutex> lock(mtx_);
        message_queue_.push(message);
    } // O lock é liberado aqui
    
    // Notifica a thread consumidora que um item foi adicionado
    cv_.notify_one();
}

// Método "Consumidor": executado pela thread interna para processar a fila
void Logger::processQueue() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx_);
        
        // Espera (dorme) até que a fila não esteja vazia OU o sinal de parada seja recebido
        // O lambda previne "despertares espúrios"
        cv_.wait(lock, [this] { return !message_queue_.empty() || stop_flag_; });

        // Se for para parar E a fila estiver vazia, podemos sair do loop
        if (stop_flag_ && message_queue_.empty()) {
            break;
        }

        // Pega a mensagem, remove da fila
        std::string message = message_queue_.front();
        message_queue_.pop();
        
        lock.unlock(); // Libera o lock antes de fazer I/O (boa prática)

        std::cout << "[LOG] " << message << std::endl;
    }
}