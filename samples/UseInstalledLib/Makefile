all:
	gcc -o MSO_Version MSO_Version.c MSO_Connect.c  MSO_errors.c -lusb /usr/local/lib/libMSO/libMSO.a /usr/local/lib/libMSO/libMSOComm.a
	gcc -o MSO_Version_shared MSO_Version.c MSO_Connect.c  MSO_errors.c -lusb -L/usr/local/lib/libMSO -lMSO -lMSOComm

clean:
	rm -f MSO_Version MSO_Version_shared *.o
