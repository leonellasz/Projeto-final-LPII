#pragma once

class ClientHandler {
public:
    ClientHandler(int client_socket);
    void handle(); // Este método será executado em uma thread

private:
    // Membros: socket do cliente, etc.
    int client_socket_;
};