# Mafefile  for libMSO, libMSOComm and sample programs
#
#install prefix
PREFIX = /usr/local

# udev or hotplug
HOTPLUG_MODE = udev

###############################################################################
#detination directory
DEST_DIR = libMSO

#install directory
DEST_LIB_DIR = $(PREFIX)/lib/$(DEST_DIR)
DEST_INC_DIR = $(PREFIX)/include/$(DEST_DIR)

# Compiler
CC = gcc

# Debug yes or no
export DEBUG = yes

.PHONY: all
all: lib samples

# Compile Librairies
.PHONY: lib
lib: 
	@test -z "lib" || mkdir -p "lib"
	$(MAKE) -C libMSOComm_src
	$(MAKE) -C libMSO_src
#	strip --strip-unneeded lib/*

# Compile samples	
.PHONY: samples
samples: 
	$(MAKE) -C samples

# Make documentation
.PHONY: doc
doc:
	doxygen Doxygen.config

# Clean Sample Objs files
.PHONY: clean
clean:
	$(MAKE) -C libMSOComm_src clean
	$(MAKE) -C libMSO_src clean
	$(MAKE) -C samples clean

# Clean Sample Objs, lib and bin files
.PHONY: cleanall
cleanall:
	rm -rf doc
	$(MAKE) -C libMSOComm_src cleanall 
	$(MAKE) -C libMSO_src cleanall 
	$(MAKE) -C samples cleanall 


# Install librairies and hotplug script
.PHONY: install
install: lib
	@echo "------------Install libraries-------------------"
	rm -rf $(DEST_LIB_DIR)
	mkdir $(DEST_LIB_DIR)
	install -p -m 755 lib/libMSO.a  $(DEST_LIB_DIR)
	install -p -m 755 lib/libMSOComm.a $(DEST_LIB_DIR)
	install -p -m 755 lib/libMSO.so.*.* $(DEST_LIB_DIR)
	install -p -m 755 lib/libMSOComm.so.*.* $(DEST_LIB_DIR)
	cd $(DEST_LIB_DIR) && ln -sf `ls libMSO.so.*.*` libMSO.so
	cd $(DEST_LIB_DIR) && ln -sf `ls libMSOComm.so.*.*` libMSOComm.so
	@echo ""
	@echo "------------Install Include file----------------"
	rm -rf $(DEST_INC_DIR)
	mkdir $(DEST_INC_DIR)
	install -p -m 644 include/libMSO.h $(DEST_INC_DIR)
	install -p -m 644 include/libMSO_Def.h $(DEST_INC_DIR)
	install -p -m 644 include/libMSO_Struct.h $(DEST_INC_DIR)
	@echo ""
	@cd hotplug_conf && sh hotplug.sh install $(HOTPLUG_MODE)
	@echo "-----------Update ldconfig path-----------------"
	@echo "--> Add $(DEST_LIB_DIR) to /etc/ld.so.conf and run /sbin/ldconfig"
	/bin/grep -v $(DEST_LIB_DIR) /etc/ld.so.conf > /etc/ld.so.conf.tmp
	echo "$(DEST_LIB_DIR)" >> /etc/ld.so.conf.tmp
	rm -rf /etc/ld.so.conf 
	mv /etc/ld.so.conf.tmp /etc/ld.so.conf	
	/sbin/ldconfig
	@echo ""
	@echo "Install OK"
	@echo ""

# UnInstall librairies and hotplug script
.PHONY: uninstall
uninstall:
	@echo "-------------UnInstall libraries-----------------"
	rm -rf $(DEST_LIB_DIR)
	@echo ""
	@echo "------------UnInstall Include file----------------"
	rm -rf $(DEST_INC_DIR)
	@echo ""
	@cd hotplug_conf && sh hotplug.sh remove $(HOTPLUG_MODE)
	@echo "-----------Update ldconfig path-----------------"
	@echo "--> Remove $(DEST_LIB_DIR) in /etc/ld.so.conf and run /sbin/ldconfig"
	/bin/grep -v $(DEST_LIB_DIR) /etc/ld.so.conf > /etc/ld.so.conf.tmp
	rm -rf /etc/ld.so.conf 
	mv /etc/ld.so.conf.tmp /etc/ld.so.conf	
	/sbin/ldconfig
	@echo ""
	@echo "UnInstall OK"
	@echo ""

