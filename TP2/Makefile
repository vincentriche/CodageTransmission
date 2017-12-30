# Makefile Generique

EXT = c
CXX = gcc
EXEC = main

CXXFLAGS = -W -g -Wall
LDFLAGS = 

OBJDIR = obj
SRC = $(wildcard *.$(EXT))
OBJ = $(SRC:.$(EXT)=.o)
OBJ := $(addprefix $(OBJDIR)/, $(OBJ))

all: $(EXEC)

$(EXEC): $(OBJ)
	@$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.$(EXT)
	@$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	@rm -f data/*C*
	@rm -f data/*D*
	@rm -rf $(OBJDIR)/*.o
	@rm -f $(EXEC)

install: $(EXEC)
	@cp $(EXEC) /usr/bin/