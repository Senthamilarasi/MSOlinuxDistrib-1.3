# Library Version
MSOCOMM_LIB_VERSION = 1.2

# Library Name
MSOCOMM_LIB_NAME = libMSOComm

# flag debug: yes or no
# DEBUG = yes 

######################################################################################

TARGET_SHARED = $(MSOCOMM_LIB_NAME).so.$(MSOCOMM_LIB_VERSION)
TARGET_STATIC = $(MSOCOMM_LIB_NAME).a

# Path to the lib
MSOCOMM_LIB_PATH = ../lib

# Compilateur
CC = gcc

# Flags
ifeq ($(DEBUG),yes)
CFLAGS = -Wall -g -O4 -fPIC
else
CFLAGS = -Wall -O4 -fPIC
endif

# Include
SRCINCLUDE = -I./USB -I./RS232 -I../include 


RS232_DRV_LINUX = ./RS232
USB_DRV_LINUX = ./USB

libMSOComm_SRCS = \
	$(RS232_DRV_LINUX)/Crc.c \
	$(RS232_DRV_LINUX)/drv_rs232.c \
	$(RS232_DRV_LINUX)/sp_rs232.c \
	$(USB_DRV_LINUX)/Usb_Drv.c \
	log.c \
	MSO_Drv.c

libMSOComm_OBJS = $(libMSOComm_SRCS:.c=.o) 

.PHONY: all
all: $(TARGET_STATIC) $(TARGET_SHARED) 

.PHONY: $(TARGET_STATIC) 
$(TARGET_STATIC): $(libMSOComm_OBJS)
	ar ruv $(MSOCOMM_LIB_PATH)/$(TARGET_STATIC) $(libMSOComm_OBJS)
	ranlib $(MSOCOMM_LIB_PATH)/$(TARGET_STATIC)

.PHONY: $(TARGET_SHARED) 
$(TARGET_SHARED): $(MSO_Drv_OBJS)
	$(CC) -o $(MSOCOMM_LIB_PATH)/$@ -shared $(libMSOComm_OBJS) $(CFLAGS) $(SRCINCLUDE)
	cd $(MSOCOMM_LIB_PATH) && ln -sf `ls $(MSOCOMM_LIB_NAME).so.*.*` $(MSOCOMM_LIB_NAME).so

%.o: %.c 
	$(CC) -c $(CFLAGS) $(SRCINCLUDE) $< -o $@

.PHONY: clean
clean:
	rm -f $(libMSOComm_OBJS)

.PHONY: cleanall
cleanall:
	rm -f $(MSOCOMM_LIB_PATH)/$(TARGET_STATIC) 
	rm -f $(MSOCOMM_LIB_PATH)/$(TARGET_SHARED) 
	rm -f $(MSOCOMM_LIB_PATH)/$(MSOCOMM_LIB_NAME).so 
	rm -f $(libMSOComm_OBJS)

