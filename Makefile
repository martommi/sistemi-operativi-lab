CC = gcc
CFLAGS = -Iinclude -Isrc # tells the compiler where to look for .h

# Server
SERVER_SRC = src/server/server.c $(wildcard src/auth/*.c) $(wildcard src/utils/*-utils.c)
SERVER_OBJ = $(SERVER_SRC:.c=.o)
SERVER_TARGET = server

# Client
CLIENT_SRC = $(wildcard src/client/*.c) $(wildcard src/auth/*.c) $(wildcard src/utils/*.c) $(wildcard src/net/*.c)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
CLIENT_TARGET = client

# Client handler
CLIENT_HANDLER_SRC = $(filter-out src/server/server.c, $(wildcard src/server/*.c)) $(wildcard src/net/*.c) $(wildcard src/auth/*.c) $(wildcard src/ticket/*.c) $(wildcard src/utils/*.c) 
CLIENT_HANDLER_OBJ = $(CLIENT_HANDLER_SRC:.c=.o)
CLIENT_HANDLER_TARGET = client_handler

all: $(SERVER_TARGET) $(CLIENT_TARGET) $(CLIENT_HANDLER_TARGET)

$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT_HANDLER_TARGET): $(CLIENT_HANDLER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install:
	mkdir -p obj bin/internal data/users data/tickets
	find src/ -name '*.o' -exec mv {} obj/ \;
	mv client server bin/
	mv client_handler bin/internal/

clean:
	rm -f bin/client server
	rm -f bin/internal/client_handler
	rm -f *.o
	rm -f obj/*.o

setup:
	@mkdir -p log
	@echo "[SETUP] Cleaning previous build..."
	@$(MAKE) clean > log/setup.log 2>&1
	@echo "[SETUP] Compiling all targets..."
	@$(MAKE) all >> log/setup.log 2>&1
	@echo "[SETUP] Installing binaries and directories..."
	@$(MAKE) install >> log/setup.log 2>&1
	@echo "[SETUP] Loading test users..."