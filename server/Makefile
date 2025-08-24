# Compiler settings
CC = g++
CFLAGS = -Wall -g -L$(JSONDIR)

# DEBUG print
# 0: DEBUG NOT, 1: DEBUG MODE
DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG
endif

# Service name
TARGET = program-service

# file root
SRCDIR = src
OBJDIR = obj
LIBDIR = ../lib
JSONDIR = ../lib/json
CROWDIR = ../lib/crow

# Source file
SRCS = $(wildcard $(SRCDIR)/*.cpp)

# Object file
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Include directories
INCLUDES = -I$(SRCDIR) -I$(JSONDIR) -I$(CROWDIR)

# Library link
LIBS = -L/usr/lib/x86_64-linux-gnu -lssl -lcrypto -ljsoncpp -lboost_system -lpthread

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS) 

# Compile
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

# clean OBJ and TARGET
clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean