#!/bin/bash

# teste_final.sh: Prova de conceito com 20 clientes sequenciais.

# Define cores para a saída
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # Sem Cor

USER_COUNT=20

echo "-------------------------------------------"
echo "INICIANDO TESTE FINAL ($USER_COUNT CLIENTES)..."
echo "-------------------------------------------"

# --- 1. PREPARAÇÃO ---
echo "[FASE 1] Preparando o ambiente..."
rm -f chat_server.log
echo "user1:pass1" > users.txt
for i in $(seq 2 $USER_COUNT); do echo "user${i}:pass${i}" >> users.txt; done
echo "Ambiente limpo. $USER_COUNT usuários criados."
sleep 1 # Pausa para garantir a escrita do arquivo no disco
echo ""

# --- 2. EXECUÇÃO ---
echo "[FASE 2] Executando o servidor e os clientes em sequência..."
./bin/chat_server &
SERVER_PID=$!
echo "Servidor iniciado em segundo plano (PID: $SERVER_PID)."
sleep 2 

for i in $(seq 1 $USER_COUNT); do
    echo "--- Iniciando simulação para o Cliente $i (user$i) ---"
    (echo "1"; sleep 0.5; echo "user$i"; sleep 0.5; echo "pass$i"; sleep 1; echo "Olá a todos, sou o user$i!"; sleep 1; echo "quit") | ./bin/chat_client
    echo "--- Cliente $i finalizou a execução. ---"
    sleep 1
done

# --- 3. FINALIZAÇÃO ---
echo ""
echo "[FASE 3] Finalizando o servidor..."
if ps -p $SERVER_PID > /dev/null; then
   kill $SERVER_PID
   wait $SERVER_PID 2>/dev/null
   echo "Servidor finalizado com sucesso."
else
   echo "AVISO: Servidor não estava mais em execução."
fi
echo ""

# --- 4. VERIFICAÇÃO ---
echo "[FASE 4] Verificando os logs..."
ALL_TESTS_PASSED=true
if ! grep -q "Credenciais de $USER_COUNT usuario(s) carregadas" chat_server.log; then ALL_TESTS_PASSED=false; fi
LOGIN_COUNT=$(grep -c "autenticado com sucesso" chat_server.log)
if [ "$LOGIN_COUNT" -ne $USER_COUNT ]; then ALL_TESTS_PASSED=false; fi
MESSAGE_COUNT=$(grep -c "Mensagem de 'user" chat_server.log)
if [ "$MESSAGE_COUNT" -ne $USER_COUNT ]; then ALL_TESTS_PASSED=false; fi

# --- 5. RESULTADO ---
echo "-------------------------------------------"
if [ "$ALL_TESTS_PASSED" = true ]; then
    echo -e "${GREEN}RESULTADO: TESTE FINAL COM $USER_COUNT CLIENTES PASSOU COM SUCESSO!${NC}"
else
    echo -e "${RED}RESULTADO: HOUVE FALHAS NO TESTE FINAL.${NC}"
fi
echo "-------------------------------------------"