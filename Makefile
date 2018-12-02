CC := gcc
SRCDIR := src
BUILDDIR := build

TEST := test/test.c
TARGET := bin/city
TESTTARGET := bin/test_city

SRC := $(shell find $(SRCDIR) -type f -name *.c)
OBJ := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SRC:.c=.o))
CFLAGS := -Wall
LIB := -pthread -L lib
INC := -I include

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIB)

$(BUILDDIR)/%.o: $(SRC)  
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $< 

clean:
	rm -r $(BUILDDIR) $(TARGET)

test: 
	$(CC) $(CFLAGS) $(test) $(INC) $(LIB) -o $(TESTTARGET)