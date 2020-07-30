IDIR = inc
LDIR = lib
LIBS = configreader
CC=gcc
CP=cp

CP_FLAGS = -i

CONFIG_DIR = config
CONFIG_FILE = default.cfg

HOME_CONFIG_DIR = .config
HOME_CONFIG_FILE = simpleshell.cfg

ODIR=bin

SDIR=src

_DEPS = syscalls.h memmanager.h termfuncs.h settingmanager.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = shell.o syscalls.o memmanager.o termfuncs.o settingmanager.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< -I$(IDIR) 

simpleshell: $(OBJ)
	$(CC) -o $@ $^ -L$(LDIR) -l$(LIBS)
	$(CP) $(CP_FLAGS) $(CONFIG_DIR)/$(CONFIG_FILE) ${HOME}/$(HOME_CONFIG_DIR)/$(HOME_CONFIG_FILE) 

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
