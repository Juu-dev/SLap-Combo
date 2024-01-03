CC=gcc
CFLAGS=-I.

# Tên file cho server và client
SERVER=server
DATABASE=database
MUTEX=mutex
CLIENT_SERVER=client_server
CLIENT=client
GAME=game_p2p

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER).c $(DATABASE).c $(DATABASE).h $(MUTEX).c $(MUTEX).h
	$(CC) -o $(SERVER) $(SERVER).c $(DATABASE).c ${MUTEX}.c $(CFLAGS) -lsqlite3 -pthread

$(CLIENT): $(CLIENT).c $(CLIENT_SERVER).c $(CLIENT_SERVER).h $(GAME).c $(GAME).h
	$(CC) -o $(CLIENT) $(CLIENT).c $(CLIENT_SERVER).c ${GAME}.c $(CFLAGS)

share: $(CLIENT).c $(CLIENT_SERVER).c $(CLIENT_SERVER).h $(GAME).c $(GAME).h
	$(CC) -shared -o libclient.so -fPIC $(CLIENT).c $(CLIENT_SERVER).c ${GAME}.c $(CFLAGS)

clean:
	rm -f $(SERVER) $(CLIENT)
