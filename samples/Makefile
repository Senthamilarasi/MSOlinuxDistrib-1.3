# Compilateur
CC = gcc

# DEBUG= "no" or "yes"
# DEBUG = yes

# Flags
ifeq ($(DEBUG),yes)
CFLAGS = -Wall -g -Os -fPIC
else
CFLAGS = -Wall -Os -fPIC
endif

# libMSOComm need to be link with libusb
LIB_USB = -lusb
LIB_SDL = -lSDL -lSDL_ttf

# Sagem lib
LIBPATH = -L../lib
LIB_STATIC_SAGEM = ../lib/libMSO.a ../lib/libMSOComm.a
LIB_SAGEM = -lMSO -lMSOComm


# Include directory
SRCINCLUDE = -I../include -I../wsq

# Exec path
EXEC_PATH = bin
EXEC_STATIC = MSO_Version MSO_TestBio MSO_TestThread
EXEC_SHARED = MSO_Version_shared MSO_TestBio_shared

MSO_Version_SRC = MSO_Version.c MSO_Connect.c MSO_errors.c
MSO_Version_OBJ = $(MSO_Version_SRC:.c=.o)

MSO_TestBio_SRC = MSO_TestBio.c MSO_Connect.c MSO_errors.c MSO_Command.c
MSO_TestBio_OBJ = $(MSO_TestBio_SRC:.c=.o)

MSO_TestThread_SRC = MSO_TestThread.c MSO_Connect.c MSO_errors.c MSO_Command.c
MSO_TestThread_OBJ = $(MSO_TestThread_SRC:.c=.o)

.PHONY: all
all: $(EXEC_STATIC) $(EXEC_SHARED)

%.o: %.c
	$(CC) -c $(CFLAGS) $(SRCINCLUDE) $< -o $@

.PHONY: MSO_Version MSO_Version_shared
MSO_Version: $(MSO_Version_OBJ)
	$(CC) $(MSO_Version_OBJ) -o $(EXEC_PATH)/MSO_Version $(CFLAGS) $(SRCINCLUDE) $(LIB_STATIC_SAGEM) $(LIB_USB)
MSO_Version_shared: $(MSO_Version_OBJ)
	$(CC) $(MSO_Version_OBJ) -o $(EXEC_PATH)/MSO_Version_shared $(CFLAGS) $(SRCINCLUDE) $(LIBPATH) $(LIB_SAGEM) $(LIB_USB)

.PHONY: MSO_TestBio MSO_TestBio_shared
MSO_TestBio: $(MSO_TestBio_OBJ)
	$(CC) $(MSO_TestBio_OBJ) -o $(EXEC_PATH)/MSO_TestBio $(CFLAGS) $(SRCINCLUDE) \
								$(LIB_STATIC_SAGEM) $(LIB_USB) $(LIB_SDL)
MSO_TestBio_shared: $(MSO_TestBio_OBJ)
	$(CC) $(MSO_TestBio_OBJ) -o $(EXEC_PATH)/MSO_TestBio_shared $(CFLAGS) $(SRCINCLUDE) $(LIBPATH) \
								$(LIB_SAGEM) $(LIB_USB) $(LIB_SDL)

.PHONY: MSO_TestThread MSO_TestThread_shared
MSO_TestThread: $(MSO_TestThread_OBJ)
	$(CC) $(MSO_TestThread_OBJ) -o $(EXEC_PATH)/MSO_TestThread $(CFLAGS) $(SRCINCLUDE) \
									$(LIB_STATIC_SAGEM) $(LIB_USB) $(LIB_SDL)
MSO_TestThread_shared: $(MSO_TestThread_OBJ)
	$(CC) $(MSO_TestThread_OBJ) -o $(EXEC_PATH)/MSO_TestThread_shared $(CFLAGS) $(SRCINCLUDE) $(LIBPATH) \
									$(LIB_SAGEM) $(LIB_USB) $(LIB_SDL)


.PHONY: clean
clean:
	rm -f $(MSO_Version_OBJ) $(MSO_TestBio_OBJ) $(MSO_TestThread_OBJ)

.PHONY: cleanall
cleanall:
	rm -f $(MSO_Version_OBJ) $(MSO_TestBio_OBJ) $(MSO_TestThread_OBJ)
	cd $(EXEC_PATH) && rm -f $(EXEC_STATIC) $(EXEC_SHARED)

