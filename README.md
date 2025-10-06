# Projeto Final LPII: Servidor de Chat Concorrente (C++)

Este repositório contém o desenvolvimento de um servidor de chat multiusuário em C++, como parte do projeto final da disciplina de LPII. O sistema é construído sobre sockets TCP e utiliza `std::thread` para gerenciar múltiplas conexões de clientes simultaneamente, com um sistema completo de autenticação, registro de usuários e comunicação pública/privada.

## Versão Final: Servidor de Chat Robusto e Funcional

Esta é a versão final do projeto, que evoluiu do protótipo inicial para um sistema de chat completo e robusto, com as seguintes funcionalidades:

### 🎯 Funcionalidades Implementadas

* **Sistema de Autenticação e Persistência:** Os usuários precisam se autenticar com `usuário` e `senha` para entrar no chat. As credenciais são carregadas e salvas no arquivo `users.txt`.
* **Registro de Novos Usuários:** Um novo usuário pode se registrar diretamente pelo cliente, e suas credenciais são salvas automaticamente no `users.txt`.
* **Comunicação Pública e Privada:** Mensagens enviadas por um cliente são retransmitidas para todos os outros (broadcast), e mensagens privadas podem ser enviadas com o comando `/priv <usuario> <mensagem>`.
* **Gerenciamento de Presença:** O servidor notifica todos os usuários quando alguém entra ou sai do chat.
* **Alta Concorrência e Robustez:** O servidor utiliza `std::thread` para lidar com cada cliente de forma isolada e foi depurado para eliminar condições de corrida, garantindo estabilidade.
* **Logging Thread-Safe:** A classe `Logger` registra de forma segura todos os eventos importantes (conexões, autenticações, mensagens, erros) em `chat_server.log`.
* **Desligamento Gracioso:** O servidor captura o sinal `Ctrl+C` (`SIGINT`) para finalizar de forma segura.

### 🛠️ Como Compilar e Executar

#### Pré-requisitos
* Compilador C++ com suporte a C++17
* `make`

#### Compilação
O projeto utiliza um `Makefile` que automatiza todo o processo. Para compilar, execute na raiz do projeto:
```bash
make
## 🧪 Como Testar

O projeto inclui um script de teste simples (`teste_simples.sh`) para validar a funcionalidade principal do servidor (inicialização, autenticação, recebimento de mensagem e desconexão).

1.  **Dê permissão de execução ao script** (apenas na primeira vez):
    ```bash
    chmod +x teste_simples.sh
    ```
2.  **Execute o teste:**
    ```bash
    ./teste_simples.sh
    ```
O script executará um teste automatizado com um único cliente e verificará os logs para confirmar se todas as ações foram processadas corretamente, exibindo um relatório de sucesso ou falha no final.
