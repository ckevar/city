CC := gcc
SRCDIR := src
BUILDDIR := build
TARGET := bin/city
TESTTARGET := test/city

SRC := $(shell find $(SRCDIR) -type f -name *.c)
OBJ := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, %(SRC:.c)=.o)
CFLAGS := -Wall
LIB := -pthread -L lib
INC := -I include

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)%.c
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<	 

clean:
	rm -r $(BUILDDIR)/*.o $(TARGET)

$(TESTTARGET): 
	@echo $(SRC)