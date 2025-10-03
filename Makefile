# Define o compilador C++
CXX = g++

# Define as flags de compilação
# -std=c++17: Requisito para std::thread
# -Wall: Habilita todos os warnings
# -pthread: Linka com a biblioteca de threads
CXXFLAGS = -std=c++17 -Wall -pthread

# Define os arquivos de código-fonte
SERVER_SRCS = main.cpp Server.cpp Logger.cpp
CLIENT_SRCS = client_cli.cpp

# Define os arquivos objeto (o 'patsubst' cria .o a partir de .cpp)
SERVER_OBJS = $(patsubst %.cpp, %.o, $(SERVER_SRCS))
CLIENT_OBJS = $(patsubst %.cpp, %.o, $(CLIENT_SRCS))

# Define os executáveis
SERVER_EXEC = chat_server
CLIENT_EXEC = chat_client

# Regra principal (default): Depende dos executáveis
all: $(SERVER_EXEC) $(CLIENT_EXEC)

# Regra para compilar o Servidor (depende dos seus arquivos .o)
$(SERVER_EXEC): $(SERVER_OBJS)
	@echo "Linkando o Servidor: $(SERVER_EXEC)"
	$(CXX) $(CXXFLAGS) $(SERVER_OBJS) -o $(SERVER_EXEC)

# Regra para compilar o Cliente (depende do seu arquivo .o)
$(CLIENT_EXEC): $(CLIENT_OBJS)
	@echo "Linkando o Cliente: $(CLIENT_EXEC)"
	$(CXX) $(CXXFLAGS) $(CLIENT_OBJS) -o $(CLIENT_EXEC)

# Regra genérica para compilar arquivos .cpp em .o
# Garante que cada .cpp é compilado ANTES da linkagem
%.o: %.cpp
	@echo "Compilando: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para limpar os arquivos gerados
clean:
	@echo "Limpando arquivos gerados..."
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS) $(SERVER_EXEC) $(CLIENT_EXEC)
	rm -f *.log