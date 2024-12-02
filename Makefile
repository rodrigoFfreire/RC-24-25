CC = g++
CCFLAGS =-g -Wall -Wextra -std=c++17
#CCFLAGS = -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused

CLIENT_TARGET = ./player
SERVER_TARGET = ./GS

CLIENT_SRCS := $(shell find client -name '*.cpp')
SERVER_SRCS := $(shell find server -name '*.cpp')
COMMON_SRCS := $(shell find common -name '*.cpp')


all: clean $(CLIENT_TARGET) $(SERVER_TARGET)

server: clean_server $(SERVER_TARGET)

client: clean_client $(CLIENT_TARGET)


$(CLIENT_TARGET):
	$(CC) $(CCFLAGS) $(CLIENT_SRCS) $(COMMON_SRCS) -o $(CLIENT_TARGET)

$(SERVER_TARGET):
	$(CC) $(CCFLAGS) $(SERVER_SRCS) $(COMMON_SRCS) -o $(SERVER_TARGET)

clean_client:
	rm -f $(CLIENT_TARGET)

clean_server:
	rm -f $(SERVER_TARGET)

clean: clean_client clean_server

.PHONY: all clean clean_client clean_server server client