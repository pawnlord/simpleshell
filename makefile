IDIR = inc
CC=gcc

ODIR=bin

SDIR=src

_DEPS = syscalls.h memmanager.h termfuncs.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = shell.o syscalls.o memmanager.o termfuncs.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< -I$(IDIR)

simpleshell: $(OBJ)
	$(CC) -o $@ $^ 

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
