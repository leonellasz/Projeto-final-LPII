# Projeto Final LPII: Servidor de Chat Concorrente (C++)

## Etapa 2: Protótipo CLI de Comunicação (`v2-cli`)


Esta etapa estabeleceu a fundação de rede do projeto (Tema A: Servidor de Chat Multiusuário), implementando um protótipo funcional de cliente e servidor TCP, com concorrência mínima e o sistema de *logging* thread-safe integrado.



## 🎯 Objetivos Cumpridos

 Requisito do Trabalho | Descrição da Implementação | Status |

 **Cliente/Servidor Mínimo em Rede** | Implementação de um `chat_server` TCP (porta 8080) e um `chat_client` CLI que se conecta e envia mensagens.  ✅ Concluído 
 **Comunicação Básica** | O cliente envia uma mensagem e o servidor a recebe e envia um **ACK** de volta, validando a troca de dados. ✅ Concluído 
 **Concorrência Mínima** | O servidor utiliza `std::thread` (função `std::thread t(&Server::handle_new_client, ...)` com `t.detach()`) para lidar com cada nova conexão.  ✅ Concluído 
 **Logging Integrado (`libtslog`)** | A classe **`Logger`** (implementada na Etapa 1 como Singleton/Monitor) está integrada para registrar eventos críticos (`[CONN]`, `[DATA]`, `[ERROR]`) de forma *thread-safe*. | ✅ Concluído |
 **Scripts de Teste** | Criação do script `test_cli.sh` para simular múltiplos clientes conectando e enviando dados simultaneamente.  ✅ Concluído 


## 🛠️ Validação (Teste de Concorrência)

Abaixo está o log de execução que demonstra a concorrência mínima e o *logging* em ação, validando que o servidor aceita e processa múltiplas conexões simultaneamente:

**Comando de Execução:** `./test_cli.sh`