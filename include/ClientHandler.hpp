#pragma once

class ClientHandler {
public:
    ClientHandler(int client_socket);
    void handle(); 
private:
   
    int client_socket_;
};