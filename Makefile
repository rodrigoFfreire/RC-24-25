CC = g++
CCFLAGS =-Wall -Wextra -std=c++17

CLIENT_TARGET	= ./player
SERVER_TARGET	= ./GS
DB_DIR			= .data

CLIENT_SRCS := $(shell find client -name '*.cpp')
SERVER_SRCS := $(shell find server -name '*.cpp')
COMMON_SRCS := $(shell find common -name '*.cpp')

all: clean $(CLIENT_TARGET) $(SERVER_TARGET)

server: clean-server $(SERVER_TARGET)

client: clean-client $(CLIENT_TARGET)


$(CLIENT_TARGET):
	$(CC) $(CCFLAGS) $(CLIENT_SRCS) $(COMMON_SRCS) -o $(CLIENT_TARGET)

$(SERVER_TARGET):
	$(CC) $(CCFLAGS) $(SERVER_SRCS) $(COMMON_SRCS) -o $(SERVER_TARGET)

clean-client:
	@$(RM) $(CLIENT_TARGET)

clean-server:
	@$(RM) $(SERVER_TARGET)

clean-db:
	@$(RM) ./$(DB_DIR)/GAMES/*
	@$(RM) ./$(DB_DIR)/SCORES/*

clean: clean-client clean-server clean-db

.PHONY: all clean clean-client clean-server clean-db server client
