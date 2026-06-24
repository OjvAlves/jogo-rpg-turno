# Diretório dos headers
INCLUDES = -I"./Arquivos de Cabeçalho"

# Flags de compilação
CFLAGS = -Wall -g $(INCLUDES) -std=c11

# Detecta sistema operacional
UNAME_S := $(shell uname -s)

# Configuração do pkg-config para Allegro
ALLEGRO_PKGS = allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_primitives-5

# Flags específicas para Windows
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    CFLAGS += -D_WIN32
    ALLEGRO_FLAGS = $(shell pkg-config --cflags $(ALLEGRO_PKGS) 2>nul)
    ALLEGRO_LIBS = $(shell pkg-config --libs $(ALLEGRO_PKGS) 2>nul)
    EXECUTABLE = game.exe
else
    # Linux/macOS
    DETECTED_OS := $(UNAME_S)
    CFLAGS += -D_POSIX_C_SOURCE=200112L
    ALLEGRO_FLAGS = $(shell pkg-config --cflags $(ALLEGRO_PKGS))
    ALLEGRO_LIBS = $(shell pkg-config --libs $(ALLEGRO_PKGS))
    EXECUTABLE = game
endif

# Verifica se Allegro está instalado
CHECK_ALLEGRO := $(shell pkg-config --exists $(ALLEGRO_PKGS) && echo "found")
ifneq ($(CHECK_ALLEGRO),found)
    $(warning Allegro não encontrado via pkg-config!)
    $(warning No Linux, instale com: sudo apt-get install liballegro5-dev)
    $(warning No macOS, instale com: brew install allegro)
    $(warning No Windows (MSYS2): pacman -S mingw-w64-x86_64-allegro)
endif

# Lista completa de arquivos fonte
SRCS = \
    main.c \
    utils.c \
    renderer.c \
    game.c \
    cartas.c \
    jogador.c \
    inimigo.c

# Gera lista de objetos
OBJS = $(SRCS:.c=.o)

# Executável final
OUT = $(EXECUTABLE)

# Regra padrão: compila tudo
all: $(OUT)

# Regra para compilar o executável
$(OUT): $(OBJS)
	gcc $(OBJS) -o $(OUT) $(CFLAGS) $(ALLEGRO_FLAGS) $(ALLEGRO_LIBS) -lm

# Regra genérica para compilar arquivos .c em .o
%.o: %.c
	gcc -c $< -o $@ $(CFLAGS) $(ALLEGRO_FLAGS)

# Dependências específicas
game.o: game.c game.h cartas.h jogador.h inimigo.h constants.h utils.h
cartas.o: cartas.c cartas.h
renderer.o: renderer.c renderer.h game.h constants.h utils.h
inimigo.o: inimigo.c inimigo.h cartas.h game.h

# Limpeza
clean:
	rm -f $(OBJS) $(OUT) game.exe *.o

# Informações do sistema
info:
	@echo "Sistema detectado: $(DETECTED_OS)"
	@echo "Executável: $(EXECUTABLE)"
	@echo "Allegro encontrado: $(CHECK_ALLEGRO)"

# Para ajudar a listar as regras disponíveis
help:
	@echo "Comandos disponíveis:"
	@echo "  make all      - Compila o projeto (padrão)"
	@echo "  make clean    - Remove arquivos objeto e executável"
	@echo "  make info     - Mostra informações do sistema"
	@echo "  make help     - Mostra esta ajuda"

# Define a regra padrão
.PHONY: all clean help info