# Compilador C++
CXX = g++

# Flags de compilação
CXXFLAGS = -std=c++17 -Wall -g -pthread

# Nome do executável
TARGET = logger_test

# Arquivos fonte
SRCS = main.cpp Logger.cpp

# Arquivos objeto
OBJS = $(SRCS:.cpp=.o)

# Regra principal: compila o programa
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) 
# Regra para compilar arquivos .cpp para .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ 
# Regra para limpar os arquivos gerados
clean:
	rm -f $(OBJS) $(TARGET) 