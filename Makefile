CC=gcc
# libs third party
LIBS=-pthread 

# Headers
IDIR=include
CFLAGS=-I$(IDIR)
DEPS=$(IDIR)/*.h

# Sources
SDIR = src
SRC = $(wildcard src/*.c)

# Objects
BDIR = build
OBJ=$(patsubst src/%.c, build/%.o, $(SRC))

$(BDIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

city: $(OBJ)
	$(CC) -o bin/$@ $^ $(CFLAGS) $(LIBS)

git:
	(git commit -a; git push)
	
clean:
	rm -f $(BDIR)/*.o 

# CC := gcc
# SRCDIR := src
# BUILDDIR := build

# TEST := test/test.c
# TARGET := bin/city
# TESTTARGET := bin/test_city

# SRC := $(shell find $(SRCDIR) -type f -name *.c)
# OBJ := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SRC:.c=.o))
# CFLAGS := -Wall
# LIB := -pthread -L lib
# INC := -I./include/

# $(BUILDDIR)/%.o: $(SRC) $(INC) 
# 	@mkdir -p $(BUILDDIR)
# 	$(CC) -c -o $@ $< $(CFLAGS)    

# $(TARGET): $(OBJ) 
# 	$(CC) -o $@ $^ $(CFLAGS) $(LIB)

# clean:
# 	rm $(BUILDDIR)/*.o $(TARGET)


# test:  
# 	$(CC) $(CFLAGS) $(test) $(INC) $(LIB) -o $(TESTTARGET)
