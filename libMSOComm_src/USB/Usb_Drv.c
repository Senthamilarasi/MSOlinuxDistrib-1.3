#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <usb.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>

#include "libMSO.h"
#include "MSO100.h"

#include "Usb_Drv.h"
#include "Usb_Drv_Interface.h"

I	g_i_EndpointIn;
I	g_i_interface;


extern void dbg(int level, const char *fmt, ...);

pthread_mutex_t g_h_Mutex = PTHREAD_MUTEX_INITIALIZER;
int g_i_WriteEvent = 0;

//#define USE_LOG
//#define USE_DBG_LIBUSB

#ifndef USE_LOG
	#define dbg(...) {do{}while(0);}
#endif

/*****************************************************************************/
/*****************************************************************************/
int SpUsb_OpenEx
(
	struct usb_dev_handle      **o_h_Mso100Handle,
	PC                         i_pc_String,
	UL                         i_ul_TimeOut,
	PVOID                      io_pv_Param
)
{
	I l_i_Ret;
	I l_i_i;
	PT_MSO_USB_DEVICE_PROPERTIES l_px_Devices;
	UL l_ul_NbDevice;
	struct usb_device *l_x_dev = NULL;

	dbg(2, "Enter SpUsb_OpenEx");

#ifdef USE_DBG_LIBUSB
	usb_set_debug(2);
#endif

	pthread_mutex_init(&g_h_Mutex, NULL);

	l_i_Ret = SpUsb_EnumDevices( &l_px_Devices, &l_ul_NbDevice);

	for(l_i_i=0; l_i_i<(I)l_ul_NbDevice; l_i_i++)
	{
		if(strcmp(i_pc_String, (PC)(l_px_Devices+l_i_i)->m_puc_SerialNumber) == 0)
		{
			l_x_dev = (l_px_Devices+l_i_i)->m_px_device;
			break;
		}
	}

	if( l_x_dev != NULL)
	{
		*o_h_Mso100Handle = usb_open(l_x_dev);
	}
	else
	{
		SpUsb_ReleaseEnumDevices( &l_px_Devices, l_ul_NbDevice);
		return SPUSB_DEVICE_NOT_PRESENT;
	}

	if( *o_h_Mso100Handle == NULL)
	{
		SpUsb_ReleaseEnumDevices( &l_px_Devices, l_ul_NbDevice);
		return SPUSB_ERR;
	}
	
	switch(l_x_dev->descriptor.idProduct)
	{
		case PRODUCTID_MSO350:
			g_i_EndpointIn = 0x81;
			g_i_interface = 0;
			break;
		default:
			g_i_EndpointIn = 0x83;
			g_i_interface = 1;
			break;
	}

	l_i_Ret = usb_set_configuration(*o_h_Mso100Handle, 1);//l_x_dev->config->bConfigurationValue);
	dbg(2, "SpUsb_OpenEx: usb_set_configuration = %d", l_i_Ret);

	l_i_Ret = usb_claim_interface(*o_h_Mso100Handle, g_i_interface);
	dbg(2, "SpUsb_OpenEx: usb_claim_interface = %d", l_i_Ret);

	SpUsb_ReleaseEnumDevices( &l_px_Devices, l_ul_NbDevice);
	return l_i_Ret;
}
/*****************************************************************************/
/*****************************************************************************/
int SpUsb_Close
(
	struct usb_dev_handle      **i_h_Mso100Handle
)
{
	I l_i_Ret = 0;

	dbg(2, "Enter SpUsb_Close");

	if(*i_h_Mso100Handle != NULL)
		l_i_Ret = usb_close(*i_h_Mso100Handle);
	
	if(l_i_Ret == 0)
		*i_h_Mso100Handle = NULL;

	pthread_mutex_destroy(&g_h_Mutex);
	
	return l_i_Ret;
}


/*****************************************************************************/
/*****************************************************************************/
int SpUsb_WriteFrame
(
	struct usb_dev_handle      *i_h_Mso100Handle,
	UL                         i_ul_Timeout,
	PUC                        i_puc_Data,
	UL                         i_ul_DataSize
)
{
	PUC		l_puc_BufferToSend;
	PUC		l_puc_PointerToCopy;
	I		l_i_Ret;
	UL		l_ul_len;
	UL		l_ul_lenToWrite;
	UL      l_ul_Timeout;
#ifdef USE_LOG
	I		l_i_i;
#endif
	dbg(2, "Enter SpUsb_WriteFrame: Timeout 0x%08X", i_ul_Timeout);

	if(i_ul_Timeout == SPUSB_TIMEOUT_INFINITE)
		l_ul_Timeout = 20000;
	else
		l_ul_Timeout = i_ul_Timeout;

	l_ul_lenToWrite = i_ul_DataSize + ( SPUSB_SYNC_FRAME_LEN + 2*sizeof(UL) + SPUSB_END_FRAME_LEN );
	l_puc_BufferToSend = (PUC)malloc ( l_ul_lenToWrite );
	if ( l_puc_BufferToSend == NULL ) 
	{
		l_i_Ret = SPUSB_NO_MEMORY;
	}

	l_puc_PointerToCopy = l_puc_BufferToSend;

	// Copy entete
	memcpy ( l_puc_PointerToCopy, SPUSB_SYNC_FRAME, SPUSB_SYNC_FRAME_LEN );
	l_puc_PointerToCopy += SPUSB_SYNC_FRAME_LEN;

	// Copy Longueur
	memcpy ( l_puc_PointerToCopy, &i_ul_DataSize, sizeof(UL) );
	l_puc_PointerToCopy += sizeof(UL);

	// Copy ~Longueur
	l_ul_len = ~i_ul_DataSize;
	memcpy ( l_puc_PointerToCopy, &l_ul_len, sizeof(UL) );
	l_puc_PointerToCopy += sizeof(UL);

	// Copy trame
	memcpy ( l_puc_PointerToCopy, i_puc_Data, i_ul_DataSize );
	l_puc_PointerToCopy += i_ul_DataSize;

	// Copy fin de trame
	memcpy ( l_puc_PointerToCopy, SPUSB_END_FRAME, SPUSB_END_FRAME_LEN );
	l_puc_PointerToCopy += SPUSB_END_FRAME_LEN;

	// Emission	
#ifdef USE_LOG
	dbg(4, "Write Data %ld", l_ul_lenToWrite);
	for(l_i_i=0; l_i_i<(I)l_ul_lenToWrite; l_i_i++)
		dbg(4, "buf[%d]= 0x%02X : %c", l_i_i,
			l_puc_BufferToSend[l_i_i], 
			(isprint(l_puc_BufferToSend[l_i_i]) ? l_puc_BufferToSend[l_i_i]:' '));
#endif

	g_i_WriteEvent = 1;
	pthread_mutex_lock(&g_h_Mutex);

	l_i_Ret = usb_claim_interface(i_h_Mso100Handle, g_i_interface);
	if(l_i_Ret != 0)
		dbg(1, "write: usb_claim_interface err %d\n", l_i_Ret);

	l_i_Ret = usb_bulk_write( i_h_Mso100Handle,
							0x02,
							(char*)l_puc_BufferToSend,
							(I)l_ul_lenToWrite,
							l_ul_Timeout);

	if(l_i_Ret != (I)l_ul_lenToWrite)
		dbg(1, "write: usb_bulk_write err %d != %d \n", l_i_Ret, l_ul_lenToWrite);

	l_i_Ret = usb_release_interface(i_h_Mso100Handle, g_i_interface);
	if(l_i_Ret != 0)
		dbg(1, "write: usb_release_interface err %d\n", l_i_Ret);

	pthread_mutex_unlock(&g_h_Mutex);
	g_i_WriteEvent = 0;
	free ( l_puc_BufferToSend );

	return l_i_Ret;
}


/*****************************************************************************/
/*****************************************************************************/
int SpUsb_ReadFrame
(
	struct usb_dev_handle      *i_h_Mso100Handle,
	UL                         i_ul_Timeout,
	PUC*                       io_ppuc_DataReceive,
	PUL                        o_pul_DataSize
)
{
	I	l_i_Ret;
	I	l_i_RetRead;
	PUC	l_puc_Temp;
	UL	l_ul_SizeTemp=0;
	UL	l_ul_SizeTempComp1=0;
	UC	*l_auc_ReceiveData; //[MAX_USB_FRAME_SIZE]; 
	UL	l_ul_NbReadChar;
	UL	l_ul_DataSize;
	UL	l_ul_Timeout;
#ifdef USE_LOG
	I	l_i_i;
#endif

	l_auc_ReceiveData = malloc(MAX_USB_FRAME_SIZE);
	if(l_auc_ReceiveData == NULL)
		return -1;

	dbg(3, "Enter SpUsb_ReadFrame timeout: 0x%08X", i_ul_Timeout);

	if(i_ul_Timeout == SPUSB_TIMEOUT_INFINITE)
		l_ul_Timeout = 500;
	else
		l_ul_Timeout = i_ul_Timeout;

	// Initialisation
	memset(l_auc_ReceiveData,0, sizeof(l_auc_ReceiveData));
	l_ul_NbReadChar = 0; 
	*o_pul_DataSize = 0;
	l_ul_DataSize = 0;

	// Read SYNC Frame
	while( l_ul_NbReadChar < 12)
	{
		pthread_mutex_lock(&g_h_Mutex);
		l_i_Ret = usb_claim_interface(i_h_Mso100Handle, g_i_interface);
		if(l_i_Ret != 0)
			dbg(1, "Read1: usb_claim_interface err %d\n", l_i_Ret);

		l_i_RetRead = usb_bulk_read(i_h_Mso100Handle,
									g_i_EndpointIn,
									(char*)l_auc_ReceiveData+l_ul_NbReadChar,
									MAX_USB_FRAME_SIZE,
									l_ul_Timeout);

		l_i_Ret = usb_release_interface(i_h_Mso100Handle, g_i_interface);
		if(l_i_Ret != 0)
			dbg(1, "Read1: usb_claim_interface err %d\n", l_i_Ret);

		pthread_mutex_unlock(&g_h_Mutex);

		if(l_i_RetRead <= 0)
		{
			dbg(1, "SpUsb_ReadFrame: Error usb_bulk_read %d", l_i_RetRead);
			usleep(10000);
			if((i_ul_Timeout == SPUSB_TIMEOUT_INFINITE) && (l_i_RetRead == -ETIMEDOUT))
				continue;
			break;
		}
		else
		{
			l_ul_NbReadChar = l_ul_NbReadChar + l_i_RetRead;
		}

		if(g_i_WriteEvent == 1)
		{
			dbg(1, "Write request\n");
			usleep(10000);
		}

		dbg(1, "SpUsb_ReadFrame: 1 - usb_bulk_read = %d", l_i_RetRead);
	}

#ifdef USE_LOG
	for(l_i_i=0; l_i_i<12; l_i_i++)
		dbg(4, "SpUsb_ReadFrame: l_auc_ReceiveData[%d]= 0x%02X : %c",
				l_i_i,
				l_auc_ReceiveData[l_i_i], 
				(isprint(l_auc_ReceiveData[l_i_i]) ? l_auc_ReceiveData[l_i_i]:' '));
#endif

	l_puc_Temp = l_auc_ReceiveData;

	l_i_Ret = memcmp(l_puc_Temp, SPUSB_SYNC_FRAME, SPUSB_SYNC_FRAME_LEN);
	l_puc_Temp += SPUSB_SYNC_FRAME_LEN;

	if(l_i_Ret == 0)
	{
		memcpy ( &l_ul_SizeTemp, l_puc_Temp,  sizeof(UL) );
		l_puc_Temp += sizeof(UL);

		memcpy ( &l_ul_SizeTempComp1, l_puc_Temp, sizeof(UL) );
		l_puc_Temp += sizeof(UL);

		if(l_ul_SizeTemp == ~l_ul_SizeTempComp1)
		{
			l_ul_DataSize = l_ul_SizeTemp;
	
			if(l_ul_NbReadChar >  l_ul_DataSize+12+2)
				dbg(1, "Warning SpUsb_ReadFrame:  %d > %d",l_ul_NbReadChar, l_ul_DataSize+12+2);

			pthread_mutex_lock(&g_h_Mutex);
			l_i_Ret = usb_claim_interface(i_h_Mso100Handle, g_i_interface);
			if(l_i_Ret != 0)
				dbg(3,"Read2: usb_claim_interface err %d\n", l_i_Ret);
	
			while(l_ul_NbReadChar < l_ul_DataSize+12+2)
			{
				l_i_RetRead = usb_bulk_read( i_h_Mso100Handle,
										g_i_EndpointIn,
										(char*)l_auc_ReceiveData+l_ul_NbReadChar,
										(l_ul_DataSize+12+2)-l_ul_NbReadChar,// MAX_USB_FRAME_SIZE,
										l_ul_Timeout);
				if(l_i_RetRead <= 0)
				{
					dbg(1, "SpUsb_ReadFrame 2: Error usb_bulk_read");
					break;
				}
				l_ul_NbReadChar = l_ul_NbReadChar + l_i_RetRead;
				dbg(1, "SpUsb_ReadFrame: 2 - usb_bulk_read = %d", l_i_RetRead);
			}

			// Realease Interface
			l_i_Ret = usb_release_interface(i_h_Mso100Handle, g_i_interface);
			if(l_i_Ret != 0)
				dbg(3, "Read2: usb_release_interface err %d\n", l_i_Ret);
			pthread_mutex_unlock(&g_h_Mutex);
	
			l_puc_Temp = l_auc_ReceiveData+12+l_ul_DataSize;
			l_i_Ret = memcmp(l_puc_Temp, SPUSB_END_FRAME, SPUSB_END_FRAME_LEN);
			if(l_i_Ret==0)
			{
				dbg(3, "memcmp ok %d", l_ul_DataSize);
				*o_pul_DataSize = l_ul_DataSize;
				*io_ppuc_DataReceive = malloc(l_ul_DataSize * sizeof(UC));
				memcpy(*io_ppuc_DataReceive, l_auc_ReceiveData+12, *o_pul_DataSize);
			}

			#ifdef USE_LOG
				for(l_i_i=0; l_i_i<l_ul_DataSize+2; l_i_i++)
					dbg(5, "SpUsb_ReadFrame: l_auc_ReceiveData[%d]= 0x%02X : %c", l_i_i, 
					l_auc_ReceiveData[l_i_i+12], 
					(isprint(l_auc_ReceiveData[l_i_i+12]) ? l_auc_ReceiveData[l_i_i+12]:' ')
				   );
			#endif
		}
		else
		{
			fprintf(stderr,"Error Usb Frame %ld %ld\n", l_ul_SizeTemp, ~l_ul_SizeTempComp1);
		}
	}

	if(l_auc_ReceiveData)
		free(l_auc_ReceiveData);
	
	if(l_ul_DataSize == 0)
		return SPUSB_ERR_TIMEOUT;

	return l_i_Ret;
}


/*****************************************************************************/
/*****************************************************************************/
int SpUsb_EnumDevices
(
	PT_MSO_USB_DEVICE_PROPERTIES    *o_ppx_Devices,
	PUL                 	    o_pul_NbDevices
)
{
	struct usb_bus *busses;
	struct usb_bus *bus;
	struct usb_device *deviceTemp;
	struct usb_dev_handle  *l_h_Mso100Handle;
	C   l_ac_SN[24];
	C   l_ac_Bus[256];
	UC	l_uc_Length; 

	dbg(2, "Enter SpUsb_EnumDevices");

	*o_pul_NbDevices = 0;
	usb_init();
	usb_find_busses();
	usb_find_devices();
	busses = usb_get_busses();

	*o_ppx_Devices = malloc(MAX_USB_DEVICE*sizeof(T_MSO_USB_DEVICE_PROPERTIES));
	memset(*o_ppx_Devices, 0, MAX_USB_DEVICE*sizeof(*o_ppx_Devices));

	for (bus=busses; bus; bus=bus->next) 
	{
		for (deviceTemp=bus->devices; deviceTemp; deviceTemp=deviceTemp->next) 
		{
			if (deviceTemp->descriptor.idVendor == SAGEM_VENDOR_ID)
			{ 
				if( *o_pul_NbDevices >= MAX_USB_DEVICE )
				{
					fprintf(stderr, "Device detected: Increase MAX_USB_DEVICE\n");
					break;
				}

				memset(l_ac_Bus, 0, sizeof(l_ac_Bus));
				memset(l_ac_SN, 0, sizeof(l_ac_SN));
				switch(deviceTemp->descriptor.idProduct)
				{
					case PRODUCTID_CBM:                   
						(*o_ppx_Devices)[*o_pul_NbDevices].m_uc_Index = *o_pul_NbDevices;
						l_h_Mso100Handle = usb_open(deviceTemp);
						usb_get_string_simple(l_h_Mso100Handle, 3, l_ac_SN, 24);  
						sprintf(l_ac_Bus,"/proc/bus/usb/%s/%s", deviceTemp->bus->dirname, deviceTemp->filename); 
						l_uc_Length = strlen(l_ac_Bus);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath = malloc(l_uc_Length+1);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath, l_ac_Bus, l_uc_Length+1);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber = malloc(24);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber, l_ac_SN, 24);
						
						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_FriendlyName = malloc(8);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_FriendlyName, SOFTWAREID_CBM, 7);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_px_device = deviceTemp;
						usb_close(l_h_Mso100Handle);

						dbg(2, "SpUsb_EnumDevices: %s: %s", (*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath, 
										(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber);

						(*o_pul_NbDevices)++;
						break;
						
					case PRODUCTID_MSO100:
						(*o_ppx_Devices)[*o_pul_NbDevices].m_uc_Index = *o_pul_NbDevices;
						l_h_Mso100Handle = usb_open(deviceTemp);
						usb_get_string_simple(l_h_Mso100Handle, 3, l_ac_SN, 24);  
						sprintf(l_ac_Bus,"/proc/bus/usb/%s/%s", deviceTemp->bus->dirname, deviceTemp->filename); 
						l_uc_Length = strlen(l_ac_Bus);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath = malloc(l_uc_Length+1);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath, l_ac_Bus, l_uc_Length+1);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber = malloc(24);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber, l_ac_SN, 24);
												
						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_FriendlyName = malloc(8);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_FriendlyName, SOFTWAREID_MSO100, 7);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_px_device = deviceTemp;
						usb_close(l_h_Mso100Handle);

						dbg(2, "SpUsb_EnumDevices: %s: %s", (*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath, 
										(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber);

						(*o_pul_NbDevices)++;              
						break;
				
					case PRODUCTID_MSO300:
						(*o_ppx_Devices)[*o_pul_NbDevices].m_uc_Index = *o_pul_NbDevices;
						l_h_Mso100Handle = usb_open(deviceTemp);
						usb_get_string_simple(l_h_Mso100Handle, 3, l_ac_SN, 24);  
						sprintf(l_ac_Bus,"/proc/bus/usb/%s/%s", deviceTemp->bus->dirname, deviceTemp->filename); 
						l_uc_Length = strlen(l_ac_Bus);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath = malloc(l_uc_Length+1);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath, l_ac_Bus, l_uc_Length+1);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber = malloc(24);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber, l_ac_SN, 24);
						
						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_FriendlyName = malloc(8);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_FriendlyName, SOFTWAREID_MSO300, 7);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_px_device = deviceTemp;
						usb_close(l_h_Mso100Handle);

						dbg(2, "SpUsb_EnumDevices: %s: %s", (*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath, 
										(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber);

						(*o_pul_NbDevices)++;
						break;
						
					case PRODUCTID_MSO350:
						(*o_ppx_Devices)[*o_pul_NbDevices].m_uc_Index = *o_pul_NbDevices;
						l_h_Mso100Handle = usb_open(deviceTemp);
						usb_get_string_simple(l_h_Mso100Handle, 3, l_ac_SN, 24);  
						sprintf(l_ac_Bus,"/proc/bus/usb/%s/%s", deviceTemp->bus->dirname, deviceTemp->filename); 
						l_uc_Length = strlen(l_ac_Bus);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath = malloc(l_uc_Length+1);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath, l_ac_Bus, l_uc_Length+1);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber = malloc(24);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber, l_ac_SN, 24);
						
						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_FriendlyName = malloc(8);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_FriendlyName, SOFTWAREID_MSO350, 7);
						
						(*o_ppx_Devices)[*o_pul_NbDevices].m_px_device = deviceTemp;
						usb_close(l_h_Mso100Handle);

						dbg(2, "SpUsb_EnumDevices: %s: %s", (*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath, 
										(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber);

						(*o_pul_NbDevices)++;
						break;

				case PRODUCTID_MSOTEST:
						(*o_ppx_Devices)[*o_pul_NbDevices].m_uc_Index = *o_pul_NbDevices;
						l_h_Mso100Handle = usb_open(deviceTemp);
						usb_get_string_simple(l_h_Mso100Handle, 3, l_ac_SN, 24);  
						sprintf(l_ac_Bus,"/proc/bus/usb/%s/%s", deviceTemp->bus->dirname, deviceTemp->filename); 
						l_uc_Length = strlen(l_ac_Bus);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath = malloc(l_uc_Length+1);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath, l_ac_Bus, l_uc_Length+1);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber = malloc(24);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber, l_ac_SN, 24);
	
						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_FriendlyName = malloc(8);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_FriendlyName, SOFTWAREID_MSOTEST, 7);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_px_device = deviceTemp;
						usb_close(l_h_Mso100Handle);

						dbg(2, "SpUsb_EnumDevices: %s: %s", (*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath, 
										(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber);

						(*o_pul_NbDevices)++;
						break;
						
				case PRODUCTID_1350:
						(*o_ppx_Devices)[*o_pul_NbDevices].m_uc_Index = *o_pul_NbDevices;
						l_h_Mso100Handle = usb_open(deviceTemp);
						usb_get_string_simple(l_h_Mso100Handle, 3, l_ac_SN, 24);  
						sprintf(l_ac_Bus,"/proc/bus/usb/%s/%s", deviceTemp->bus->dirname, deviceTemp->filename); 
						l_uc_Length = strlen(l_ac_Bus);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath = malloc(l_uc_Length+1);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath, l_ac_Bus, l_uc_Length+1);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber = malloc(24);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber, l_ac_SN, 24);
						
						(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_FriendlyName = malloc(8);
						memcpy((*o_ppx_Devices)[*o_pul_NbDevices].m_puc_FriendlyName, SOFTWAREID_MSO1310, 7);

						(*o_ppx_Devices)[*o_pul_NbDevices].m_px_device = deviceTemp;
						usb_close(l_h_Mso100Handle);

						dbg(2, "SpUsb_EnumDevices: %s: %s", (*o_ppx_Devices)[*o_pul_NbDevices].m_puc_DevicePath, 
										(*o_ppx_Devices)[*o_pul_NbDevices].m_puc_SerialNumber);

						(*o_pul_NbDevices)++;
						break;
					default:
						fprintf(stderr, "Unknown Product %d\n", deviceTemp->descriptor.idProduct);
				}
			}
		}
	}
	return 0;
}

/*****************************************************************************/
/*****************************************************************************/
int SpUsb_ReadFrameReleaseEx
(
	struct usb_dev_handle      *i_h_Mso100Handle,
	PUC*                       io_ppuc_Data
)
{
	dbg(2, "Enter SpUsb_ReadFrameReleaseEx");

	if(*io_ppuc_Data)
		free(*io_ppuc_Data);
		
	*io_ppuc_Data = NULL;

	return 0;
}

/*****************************************************************************/
/*****************************************************************************/
int SpUsb_ReleaseEnumDevices
( 
	PT_MSO_USB_DEVICE_PROPERTIES *o_ppx_DeviceProperties,
	unsigned long                i_ul_NbreDevices
)
{
	I  l_i_i;

	dbg(2, "Enter SpUsb_ReleaseEnumDevices");

	for(l_i_i=0; l_i_i< (I)i_ul_NbreDevices; l_i_i++)
	{
		if( (*o_ppx_DeviceProperties)[l_i_i].m_puc_SerialNumber )
			free((*o_ppx_DeviceProperties)[l_i_i].m_puc_SerialNumber);
			
		if( (*o_ppx_DeviceProperties)[l_i_i].m_puc_FriendlyName )
			free((*o_ppx_DeviceProperties)[l_i_i].m_puc_FriendlyName);

		if( (*o_ppx_DeviceProperties)[l_i_i].m_puc_DevicePath )
			free((*o_ppx_DeviceProperties)[l_i_i].m_puc_DevicePath);
	}

	if( *o_ppx_DeviceProperties != NULL)
		free(*o_ppx_DeviceProperties);

	*o_ppx_DeviceProperties = NULL;
		
	return 0;
}


/*****************************************************************************/
/*****************************************************************************/
I SpUsb_WaitForReboot
(
	struct usb_dev_handle **i_ph_Mso100Handle,
	UL						i_ul_WriteTimeout,
	PUC						i_puc_Data,
	UL						i_ul_DataSize,
	UL						i_ul_WaitTimeOut
)
{
	I			l_i_Ret;
	C			l_ac_SN[256];
	struct timeval 	tp_start;
	struct timeval 	tp_current;
	long long 		l_l_DiffTime_usec;
	long long 		l_l_Timout_usec;

	memset(l_ac_SN, 0, 256);

	dbg(3, "Enter SpUsb_WaitForReboot: i_ul_WriteTimeout %ld, i_ul_WaitTimeOut %ld",
							i_ul_WriteTimeout, i_ul_WaitTimeOut);

	l_i_Ret = usb_get_string_simple(*i_ph_Mso100Handle, 3, l_ac_SN, 256);
	dbg(2, "SpUsb_WaitForReboot: usb_get_string_simple %d: %s", l_i_Ret, l_ac_SN);

	l_i_Ret = SpUsb_WriteFrame( *i_ph_Mso100Handle,
								i_ul_WriteTimeout,               // UL   i_ul_Timeout,
								i_puc_Data,                     // PUC  i_puc_Data,
								i_ul_DataSize                   // UL   i_ul_DataSize
							);
	dbg(2, "SpUsb_WaitForReboot: SpUsb_WriteFrame %d", l_i_Ret);

	l_i_Ret = SpUsb_Close(i_ph_Mso100Handle);
	dbg(2, "SpUsb_WaitForReboot: SpUsb_Close %d", l_i_Ret);

	l_l_Timout_usec =  i_ul_WaitTimeOut * 1000;
	sleep(1);

	l_i_Ret = -1;
	gettimeofday(&tp_start,NULL);
	while(l_i_Ret != 0)
	{
		usleep(100000);
		l_i_Ret = SpUsb_OpenEx( i_ph_Mso100Handle,
								l_ac_SN, 		//PC    i_pc_String,
								i_ul_WaitTimeOut,       //UL    i_ul_TimeOut,
								NULL                    //PVOID io_pv_Param
								);
		dbg(2, "SpUsb_WaitForReboot: SpUsb_OpenEx %d", l_i_Ret);

		gettimeofday(&tp_current, NULL);
		l_l_DiffTime_usec = (tp_current.tv_sec - tp_start.tv_sec)*1000000L +
							(tp_current.tv_usec - tp_start.tv_usec);
		if( l_l_DiffTime_usec > l_l_Timout_usec )
		{
			dbg(2, "SpUsb_WaitForReboot: Reboot Timeout");
			l_i_Ret = -1;
			break;
		}

		dbg(2, "SpUsb_WaitForReboot: Reboot currentime %lld", l_l_DiffTime_usec);
	}

	return l_i_Ret;
}

/*****************************************************************************/
/*****************************************************************************/

