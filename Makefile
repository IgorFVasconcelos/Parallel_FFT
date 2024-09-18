# Variáveis
CC = mpicc
CFLAGS = -I./headers
LDFLAGS = -lm
SRC_DIR = ./src
OBJ_DIR = ./obj
MAIN_DIR = ./main
TARGET = $(MAIN_DIR)/main_program

# Arquivos fonte e objetos
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))
MAIN_FILE = $(MAIN_DIR)/main.c

# Regra padrão para compilar o projeto
all: $(TARGET)

# Regra para criar o executável
$(TARGET): $(OBJ_FILES) $(MAIN_FILE) | $(OBJ_DIR) main_dir
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Regra para compilar os arquivos .c em .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Cria o diretório de objetos se não existir
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Cria o diretório da pasta main se não existir
main_dir:
	mkdir -p $(MAIN_DIR)

# Limpa arquivos objeto e o executável
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Limpa apenas os arquivos objeto
clean-obj:
	rm -rf $(OBJ_DIR)

# Limpa apenas o executável
clean-bin:
	rm -f $(TARGET)