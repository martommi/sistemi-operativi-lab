CC = gcc
CFLAGS = -Iinclude -Isrc # tells the compiler where to look for .h

# Server
SERVER_SRC = src/server.c src/auth/session.c
SERVER_OBJ = $(SERVER_SRC:.c=.o)
SERVER_TARGET = server

# Client
CLIENT_SRC = src/client/client.c src/client/client-dispatcher.c src/auth/session.c src/net/protocol.c src/net/request.c src/net/response.c
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
CLIENT_TARGET = client

# Client handler (aka server instance)
CLIENT_HANDLER_SRC = src/server/client_handler.c src/server/server-dispatcher.c src/net/protocol.c src/net/request.c src/auth/session.c
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
	mkdir -p obj bin/internal etc data
	mv *.o obj/
	mv client server bin/
	mv client_handler bin/internal/.client_handler

clean:
	rm -f client server
	rm -f bin/internal/.client_handler
	rm -f *.o
	rm -f obj/*.o

setup: install
	@echo "[SETUP] Loading test users..."
	@mkdir -p etc
	@echo -e "admin:admin123\nagent:agent123" > etc/users-config.dat
	@chmod 600 etc/users-config.dat