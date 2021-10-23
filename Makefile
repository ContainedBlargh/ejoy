CC=gcc
CFLAGS=-std=c11 -ggdb -Wall -pedantic -O3 -Wparentheses

main: main.o
	$(CC) -o ejoy main.o -ldiscord -lcurl -lcrypto -lpthread -lm
