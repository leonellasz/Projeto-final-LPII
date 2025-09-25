#include "Logger.hpp"
#include <vector>
#include <sstream> // Para construir as strings de log

// Função que será executada por cada thread
void worker_task(int thread_id) {
    for (int i = 0; i < 50; ++i) {
        std::stringstream ss;
        ss << "Mensagem " << i << " da Thread " << thread_id;
        
        // Usando o Logger para registrar a mensagem
        Logger::getInstance().log(ss.str());
    }
}

int main() {
    std::cout << "Iniciando teste de logging concorrente..." << std::endl;

    const int NUM_THREADS = 10;
    std::vector<std::thread> threads;

    // Cria e inicia as threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(worker_task, i + 1);
    }

    // Espera todas as threads terminarem
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    std::cout << "Teste finalizado. A instancia do Logger sera destruida agora." << std::endl;
    // O destrutor do Logger será chamado aqui, quando o main terminar.
    // Ele garantirá que todos os logs restantes na fila sejam processados antes de o programa fechar.

    return 0;
}