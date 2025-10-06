CXX = g++
SRCDIR = src
INCDIR = include
BUILDDIR = build
BINDIR = bin
CXXFLAGS = -std=c++17 -Wall -pthread -I$(INCDIR)
SERVER_EXEC = $(BINDIR)/chat_server
CLIENT_EXEC = $(BINDIR)/chat_client
SERVER_SRCS = $(SRCDIR)/main.cpp $(SRCDIR)/Server.cpp $(SRCDIR)/Logger.cpp
CLIENT_SRCS = $(SRCDIR)/client_cli.cpp
SERVER_OBJS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SERVER_SRCS))
CLIENT_OBJS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(CLIENT_SRCS))
all: $(SERVER_EXEC) $(CLIENT_EXEC)
$(SERVER_EXEC): $(SERVER_OBJS)
	@mkdir -p $(BINDIR)
	@echo "Linkando o Servidor: $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
$(CLIENT_EXEC): $(CLIENT_OBJS)
	@mkdir -p $(BINDIR)
	@echo "Linkando o Cliente: $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	@echo "Compilando: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@
clean:
	@echo "Limpando arquivos gerados..."
	rm -rf $(BINDIR) $(BUILDDIR)
	rm -f *.log