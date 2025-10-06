# Define o compilador C++
CXX = g++

# Diretórios
SRCDIR = src
INCDIR = include
BUILDDIR = build
BINDIR = bin

# Flags de compilação
# -std=c++17, -Wall, -pthread, -I$(INCDIR) (inclui diretório de headers)
CXXFLAGS = -std=c++17 -Wall -pthread -I$(INCDIR)

# Executáveis
SERVER_EXEC = $(BINDIR)/chat_server
CLIENT_EXEC = $(BINDIR)/chat_client

# Arquivos fonte
SERVER_SRCS = $(SRCDIR)/main.cpp $(SRCDIR)/Server.cpp $(SRCDIR)/Logger.cpp
CLIENT_SRCS = $(SRCDIR)/client_cli.cpp

# Arquivos objeto (transforma .cpp em .o no diretório de build)
SERVER_OBJS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SERVER_SRCS))
CLIENT_OBJS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(CLIENT_SRCS))

# Regra principal
all: $(SERVER_EXEC) $(CLIENT_EXEC)

# Regra para Linkar o Servidor
$(SERVER_EXEC): $(SERVER_OBJS)
	@mkdir -p $(BINDIR)
	@echo "Linkando o Servidor: $@"
	$(CXX) $(CXXFLAGS) $^ -o $@

# Regra para Linkar o Cliente
$(CLIENT_EXEC): $(CLIENT_OBJS)
	@mkdir -p $(BINDIR)
	@echo "Linkando o Cliente: $@"
	$(CXX) $(CXXFLAGS) $^ -o $@

# Regra genérica para compilar (.cpp -> .o)
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	@echo "Compilando: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para limpar os arquivos gerados (inclui logs, bins e builds)
clean:
	@echo "Limpando arquivos gerados..."
	rm -rf $(BINDIR) $(BUILDDIR)
	rm -f *.log