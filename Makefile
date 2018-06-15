CC = gcc
CFLAGS = -Wall -g -std=c99 -fno-stack-protector
LD = gcc
LDFLAGS = -lm
INCLUDE_DIR = inc
LIBS_DIR = lib
SRC_DIR = src
EXEC = exec
OBJ = main.o $(LIBS_DIR)/funcoes.o
ZIP = arquivos

all: $(EXEC)

$(EXEC): $(OBJ)
	$(LD) -o $@ $^ -I$(INCLUDE_DIR) $(LDFLAGS)

main.o:main.c
	$(CC) -c -o $@ $< $(CFLAGS) -I$(INCLUDE_DIR)

$(LIBS_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) -c -o $@ $< $(CFLAGS) -I$(INCLUDE_DIR)

clean:
	rm -f $(OBJ) $(EXEC) $(ZIP).zip

run:
	./$(EXEC)

zip:
	rm -f $(ZIP) && zip -r $(ZIP) * #-x "Casos/*" # o -x "Casos/" eh soh caso vc tenho a pasta dos casos de teste nesse diretorio
