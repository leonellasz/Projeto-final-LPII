# Relatório de Desenvolvimento e Análise de Concorrência

**Autor:** Maria Júlia Leonella da Silva Carlos
**Matrícula:** 20230047551

### Introdução

Quando comecei este projeto de chat, o objetivo era aplicar na prática os conceitos de programação concorrente que aprendi na disciplina. Este documento explica os três maiores desafios técnicos que encontrei durante o desenvolvimento e como usei threads, mutexes e outros mecanismos para resolvê-los. Para validar as soluções, usei uma IA como uma espécie de "colega de projeto" para discutir os problemas e as abordagens.

### Problema 1: O Servidor Travava com Vários Clientes

Logo no começo, percebi que, embora o servidor funcionasse bem com um cliente, ele travava ou se comportava de forma imprevisível quando vários clientes tentavam se conectar ou se registrar ao mesmo tempo.

**Causa:** O problema era uma clássica **condição de corrida** (*race condition*). Várias threads estavam tentando modificar as mesmas variáveis globais do servidor — especificamente, os mapas `std::map` que guardam os usuários e suas credenciais. Como `std::map` não é seguro para ser modificado por múltiplas threads ao mesmo tempo, sua estrutura interna estava sendo corrompida.

**Solução:** Para resolver, apliquei o conceito de **exclusão mútua**. Criei um `std::mutex` que funciona como um "cadeado" para todo o estado compartilhado do servidor. Antes de qualquer thread ler ou modificar os mapas, ela precisa adquirir esse cadeado. Para garantir que o cadeado fosse sempre liberado, usei o `std::lock_guard`. Ele trava o mutex quando é criado e o libera automaticamente quando sai de escopo, o que é uma forma muito mais segura e limpa de gerenciar locks.

### Problema 2: O Cliente "Congelava" ao Digitar

A primeira versão do cliente era muito ruim de usar. Enquanto eu estava digitando uma mensagem, a tela não atualizava com o que os outros usuários diziam. O chat parecia congelado até que eu pressionasse Enter.

**Causa:** O problema era o **I/O bloqueante**. A thread principal do programa ficava "presa" em uma de duas funções: ou em `std::getline`, esperando a minha digitação, ou em `recv`, esperando uma mensagem do servidor. Ela não conseguia fazer as duas coisas ao mesmo tempo.

**Solução:** A saída foi reestruturar o cliente para usar duas threads. Agora, a arquitetura funciona assim:
* A **thread principal** cuida apenas da interação com o usuário (ler o que eu digito com `getline` e enviar).
* Uma **thread secundária** (`receiver_thread`) foi criada com uma única tarefa: ficar em um loop infinito, esperando por mensagens do servidor com `recv` e imprimindo-as na tela assim que chegam.

Com isso, as duas operações bloqueantes rodam em paralelo, e o cliente se tornou responsivo e verdadeiramente em tempo real.

### Problema 3: O Servidor Ficava Lento com Muitas Mensagens

Notei que, com muitos usuários conversando, o servidor todo ficava mais lento. Investiguei e descobri que o culpado era o sistema de logs.

**Causa:** Cada thread de cliente estava parando sua execução para escrever a mensagem de log diretamente no arquivo em disco. A escrita em disco (I/O) é uma operação muito lenta, e isso estava se tornando um gargalo que atrasava o processamento das mensagens principais do chat.

**Solução:** Para otimizar, implementei o padrão de design **Produtor-Consumidor**.
* As **threads dos clientes** (as "produtoras") agora só fazem o trabalho rápido: jogam a string de log em uma fila (`std::queue`) e voltam imediatamente para suas tarefas.
* Criei uma **única thread de background** (a "consumidora") dentro da classe `Logger`, cuja única função é pegar as mensagens dessa fila, uma por uma, e salvá-las no arquivo com calma.

Isso desacoplou as threads principais da lentidão do I/O de disco, eliminando o gargalo e tornando o servidor muito mais performático sob carga.

### Conclusão

Este projeto foi uma ótima oportunidade para ver a teoria de concorrência na prática. O que mais ficou claro para mim é que threads e mutexes servem para duas coisas principais: primeiro, para garantir que o programa **funcione corretamente**, usando mutexes para evitar que os dados compartilhados sejam corrompidos. Segundo, para fazer com que o programa seja **rápido e responsivo**, usando threads para separar tarefas lentas (como I/O) das rápidas. Resolver esses desafios foi fundamental para transformar os conceitos da sala de aula em conhecimento prático.

---
### Prompts Utilizados na Investigação

1.  *Estou construindo meu servidor de chat e ele funciona bem com um cliente, mas começa a travar ou se comportar de forma estranha quando vários clientes se conectam ao mesmo tempo. Suspeito que o problema seja o acesso simultâneo ao `std::map` onde guardo os usuários, o que pode estar causando uma condição de corrida. Qual é a abordagem padrão em C++ para proteger uma estrutura de dados como essa do acesso concorrente por múltiplas threads?*

2.  *Meu cliente de chat tem um grande problema de usabilidade: a função `std::getline` bloqueia o programa enquanto espera o usuário digitar, o que o impede de chamar `recv` para ver se novas mensagens chegaram do servidor. O resultado é que o chat só atualiza depois que eu envio uma mensagem. Como posso reestruturar o cliente, provavelmente com threads, para que ele consiga receber dados da rede e esperar pela digitação do usuário ao mesmo tempo?*

3.  *Percebi que meu servidor fica mais lento quando muitos clientes estão conversando e gerando logs. Minha implementação atual faz com que cada thread de cliente escreva diretamente no arquivo de log, e acredito que a espera pela escrita no disco (I/O) está se tornando um gargalo. Pensei em usar uma fila para gerenciar as mensagens de log. Qual padrão de design posso usar para que as threads de clientes apenas 'despachem' o log rapidamente e deixem o trabalho lento de escrita para uma thread separada?*
### Mapeamento Requisitos-Código

Esta seção mapeia os requisitos do projeto (gerais e específicos do tema) para as seções correspondentes no código-fonte, demonstrando a aplicação prática dos conceitos estudados e o cumprimento dos objetivos.

| Requisito | Arquivo(s) Relevante(s) | Implementação e Justificativa |
| :--- | :--- | :--- |
| **1. Threads (Geral)** | `Server.cpp`, `client_cli.cpp`, `Logger.cpp` | **Servidor:** Adota o modelo "thread-por-cliente", onde a função `run_accept_loop()` instancia uma `std::thread` para cada nova conexão aceita. Isso permite o processamento simultâneo e isolado de múltiplos clientes. <br> **Cliente:** Utiliza duas threads para resolver o problema de I/O bloqueante: a thread principal para `std::getline` (entrada do usuário) e uma `receiver_thread` dedicada para `recv` (recebimento de dados da rede), garantindo a responsividade da interface. <br> **Logger:** Possui uma `consumer_thread_` interna, que processa a fila de logs em segundo plano. |
| **2. Exclusão Mútua (Geral)** | `Server.hpp`, `Server.cpp`, `Logger.cpp` | **Servidor:** Um `std::mutex` (`clients_mutex_`) protege todo o estado compartilhado (mapas de usuários, credenciais e acesso ao `users.txt`). O `std::lock_guard` é usado em todas as seções críticas para garantir a atomicidade das operações e prevenir *race conditions*. <br> **Logger:** Um mutex (`mtx_`) protege o acesso à fila de mensagens (`message_queue_`), garantindo que a adição de logs por múltiplas threads seja segura. |
| **3. Variáveis de Condição (Geral)** | `Logger.hpp`, `Logger.cpp` | A classe `Logger` utiliza uma `std::condition_variable` (`cv_`) para implementar o padrão Produtor-Consumidor de forma eficiente. A thread consumidora "dorme" com `cv_.wait()`, sendo acordada apenas quando há novas mensagens, o que evita o uso de *busy-waiting* (polling), economizando recursos do sistema. |
| **4. Monitores (Geral)** | `Logger.hpp`, `Logger.cpp` | A classe `Logger` é uma implementação prática do padrão **Monitor**. Ela encapsula o recurso compartilhado (a fila de logs) e todas as primitivas de sincronização (`mutex` e `condition_variable`) em um único objeto, oferecendo uma interface pública (`log()`) que é inerentemente thread-safe. |
| **5. Sockets (Geral)** | `Server.cpp`, `client_cli.cpp` | A API de Sockets Berkeley é a base da comunicação em rede. O servidor usa a sequência `socket()`, `bind()`, `listen()`, `accept()`, e o cliente usa `socket()` e `connect()`. Ambos utilizam `send()` e `read()`/`recv()` para a troca de dados. |
| **6. Gerenciamento de Recursos (Geral)**| `Server.cpp`, `Logger.cpp`| O princípio RAII é aplicado com `std::lock_guard`. Os destrutores (`~Server`, `~Logger`) garantem a liberação de recursos, como o fechamento do socket do servidor e do arquivo de log. A thread do logger é finalizada de forma segura com `join()`. |
| **7. Tratamento de Erros (Geral)** | `Server.cpp`, `client_cli.cpp`, `main.cpp` | Os valores de retorno de chamadas de sistema críticas (ex: `socket()`, `bind()`) são verificados. Erros são reportados no log do servidor ou no `stderr` do cliente, com encerramento do programa em casos irrecuperáveis. |
| **8. Logging Concorrente (Tema)** | `Logger.hpp`, `Logger.cpp`, `Server.cpp` | Foi implementado um sistema de logging concorrente do zero na classe `Logger`, em vez de usar `libtslog`, para demonstrar de forma mais aprofundada o domínio dos conceitos de mutex, variáveis de condição e do padrão Monitor, que são requisitos centrais do projeto. Os logs são salvos de forma segura em `chat_server.log`. |
| **9. Documentação (Geral)** | `DiagramaEtapa3.jpg` | Um diagrama de sequência foi criado para ilustrar o fluxo de comunicação entre múltiplos clientes e o servidor, incluindo a conexão, o broadcast de mensagens e a desconexão. |
| **10. Build (Geral)** | `Makefile` | Um `Makefile` automatiza a compilação do servidor (`chat_server`) e do cliente (`chat_client`), definindo as flags necessárias (`-std=c++17`, `-pthread`) e gerenciando as dependências dos arquivos. |
| **11. Uso de IA/LLMs (Geral)** | `RELATORIO.md` | O relatório de análise crítica foi desenvolvido em diálogo com um LLM para investigar e validar as decisões de design de concorrência, conforme detalhado no próprio documento. |
| **Broadcast de Mensagens (Tema)** | `Server.cpp` | A função `broadcast_message` é chamada quando uma mensagem pública é recebida. Ela itera sobre a lista de usuários autenticados e envia a mensagem para todos, exceto o remetente original. O acesso à lista é protegido por mutex. |
| **Autenticação e Registro (Opcional)** | `Server.cpp`, `client_cli.cpp`, `users.txt` | Um sistema de contas completo foi implementado. O servidor gerencia os comandos `AUTH` e `REGISTER`, validando credenciais e persistindo novos usuários no arquivo `users.txt`. Todo o processo é protegido por mutex para garantir a atomicidade das operações. |
| **Mensagens Privadas (Opcional)** | `Server.cpp` | A funcionalidade foi implementada com o comando `/priv <usuário> <mensagem>`. O servidor possui uma lógica (`handle_private_message`) para parsear o comando, encontrar o socket do destinatário no mapa `username_to_socket_` e enviar a mensagem diretamente, garantindo a privacidade. |