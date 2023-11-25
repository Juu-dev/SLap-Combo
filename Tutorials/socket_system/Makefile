CC=gcc
CFLAGS=-I.

# Tên file cho server và client
SERVER=server
CLIENT=client

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER).c
	$(CC) -o $(SERVER) $(SERVER).c $(CFLAGS)

$(CLIENT): $(CLIENT).c
	$(CC) -o $(CLIENT) $(CLIENT).c $(CFLAGS)

clean:
	rm -f $(SERVER) $(CLIENT)
