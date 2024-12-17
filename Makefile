# Compiler Settings
CC = g++
CCFLAGS =-Wall -Wextra -std=c++17

# Binary targets, directories and other files
CLIENT_TARGET	= ./player
SERVER_TARGET	= ./GS

DB_DIR			= .data
CLIENT_DIR		= client
COMMON_DIR		= common
SERVER_DIR		= server

README			= readme.txt
AUTO_AV			= 2024_2025_proj_auto_avaliacao.xlsx

# Source files
CLIENT_SRCS 	:= $(shell find $(CLIENT_DIR) -name '*.cpp')
SERVER_SRCS 	:= $(shell find $(SERVER_DIR) -name '*.cpp')
COMMON_SRCS 	:= $(shell find $(COMMON_DIR) -name '*.cpp')

# Other variables
G_NO			:= 65

# ALL: Cleans and compiles client and server
all: clean $(CLIENT_TARGET) $(SERVER_TARGET)

# SERVER: Cleans and compiles server
server: clean-server $(SERVER_TARGET)

# CLIENT: Cleans and compiles client
client: clean-client $(CLIENT_TARGET)

# ZIP: Creates submission zip file
zip:
	zip -r proj_$(G_NO).zip $(CLIENT_DIR) $(COMMON_DIR) $(SERVER_DIR) $(README) $(AUTO_AV) Makefile

# CLEAN: Cleans everything
clean: clean-client clean-server clean-db

# CLEAN-CLIENT: Cleans client binary
clean-client:
	@$(RM) $(CLIENT_TARGET)

# CLEAN-SERVER: Cleans server binary
clean-server:
	@$(RM) $(SERVER_TARGET)

# CLEAN-DB: Cleans the database
clean-db:
	@$(RM) -rf ./$(DB_DIR)/GAMES/*
	@$(RM) -rf ./$(DB_DIR)/SCORES/*

$(CLIENT_TARGET):
	$(CC) $(CCFLAGS) $(CLIENT_SRCS) $(COMMON_SRCS) -o $(CLIENT_TARGET)

$(SERVER_TARGET):
	$(CC) $(CCFLAGS) $(SERVER_SRCS) $(COMMON_SRCS) -o $(SERVER_TARGET)


.PHONY: all server client zip clean clean-client clean-server clean-db
