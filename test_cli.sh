#!/bin/bash

# Este script inicia o servidor e vários clientes para simular concorrência.
# REQUER: O executável 'chat_server' e 'chat_client' compilados.

SERVER_EXEC="./chat_server"
CLIENT_EXEC="./chat_client"

# 1. Inicia o servidor em background
echo ">>> 1. Iniciando o Servidor em background..."
$SERVER_EXEC &
SERVER_PID=$!
sleep 1 # Tempo para o servidor inicializar o bind/listen

if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo "Erro: Servidor nao iniciou. Verifique se a porta esta livre."
    exit 1
fi
echo "Servidor iniciado com PID: $SERVER_PID"
echo "------------------------------------------------"

# Funcao que simula a interacao de um cliente
run_client() {
    CLIENT_ID=$1
    MESSAGE="Ola, sou o cliente $CLIENT_ID!"
    
    echo ">>> CLIENTE $CLIENT_ID: Conectando e enviando: \"$MESSAGE\""
    
    # Executa o cliente e injeta comandos
    # 1. Envia a mensagem
    # 2. Envia 'quit'
    echo -e "$MESSAGE\nquit" | $CLIENT_EXEC
    
    echo "<<< CLIENTE $CLIENT_ID: Conexao encerrada."
    echo "------------------------------------------------"
}

# 2. Simula multiplos clientes (concorrencia minima)
echo ">>> 2. Simulando multiplos clientes..."
run_client 1 &
run_client 2 &
run_client 3 &

# Espera todas as threads/processos clientes terminarem
wait

# 3. Encerra o servidor
echo ">>> 3. Encerrando o Servidor (enviando sinal SIGINT para o PID)..."
kill -SIGINT $SERVER_PID
sleep 1
echo "Servidor encerrado."
echo "------------------------------------------------"
echo "VERIFIQUE o arquivo de LOG para confirmar as 3 conexões e 3 mensagens."