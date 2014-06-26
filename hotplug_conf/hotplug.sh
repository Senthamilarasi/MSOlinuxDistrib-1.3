#!/bin/bash

# Install script for udev or hotplug rules
# Valid commands are:
# ./hotplug.sh install udev
# ./hotplug.sh install hotplug
# ./hotplug.sh remove udev
# ./hotplug.sh remove hotplug

if [ $1 == "install" ]
then
	if [ $2 == "udev" ]
	then
		if [ -x /etc/udev ]
		then
			echo "------------Install udev rules------------------"
		#	if [ -x /etc/udev/agents.d ]
		#	then
		#		if [ -x /etc/udev/agents.d/usb ]
		#		then
		#			echo "find /etc/udev/agents.d/usb directory"
		#		else
		#			echo "Create /etc/udev/agents.d/usb directory"
		#			mkdir /etc/udev/agents.d/usb
		#		fi
		#	else
		#		echo "Create /etc/udev/agents.d directory"
		#		echo "Create /etc/udev/agents.d/usb directory"
		#		mkdir /etc/udev/agents.d
		#		mkdir /etc/udev/agents.d/usb
		#	fi
		#	echo "--> Install script usbsagem in /etc/udev/agents.d/usb"
        #		install -m 755 udev/usbsagem /etc/udev/agents.d/usb

			if [ -x /etc/udev/rules.d ]
			then
				echo "--> Install 70-sagem.rules in /etc/udev/rules.d"
        			install -m 644 udev/70-sagem.rules /etc/udev/rules.d
			else
				echo "can't find /etc/udev/rules.d directory"
			fi
		else
			echo "/etc/udev directory not present, try install with hotplug"
		fi

        		echo " "

	elif [ $2 == "hotplug" ]
	then
		if [ -x /etc/hotplug/usb ]
		then
        		echo "------------Install Hotplug script--------------"
			echo "--> Install sagem.usermap file in /etc/hotplug/usb"
        		install -m 644 hotplug/sagem.usermap /etc/hotplug/usb
			echo "--> Install usbsagem file in /etc/hotplug/usb"
        		install -m 755 hotplug/usbsagem /etc/hotplug/usb
        		echo " "
		else
			echo "Can't find /etc/hotplug/usb directory, try install with udev"
		fi
	else
		echo " "
		echo "Verify that you have rights on usb virtual file system"	
		echo " "
	fi

elif [ $1 == "remove" ]
then
	if [ $2 == "udev" ]
	then
		echo "------------UnInstall udev rules------------------"
		echo "--> remove /etc/udev/rules.d/70-sagem.rules file"
		rm -f /etc/udev/rules.d/70-sagem.rules 
		echo "--> remove /etc/udev/agents.d/usb/usbsagem file"
        	rm -f /etc/udev/agents.d/usb/usbsagem 
        	echo " "

	elif [ $2 == "hotplug" ]
	then
        	echo "------------UnInstall Hotplug script--------------"
		echo "--> remove /etc/hotplug/usb/sagem.usermap file"
		rm -f /etc/hotplug/usb/sagem.usermap
		echo "--> remove /etc/hotplug/usb/usbsagem file"
        	rm -f /etc/hotplug/usb/usbsagem
        	echo " "
	fi
fi
