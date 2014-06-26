#include <stdio.h>
#include <string.h>

#include <usb.h>

#include "libMSO.h"
#include "MSO100.h"


int MSO_Connect( MORPHO_HANDLE  *i_ph_Mso100Handle,
				int i_i_ConnectionMode,		// COM_USB or COM_RS232
				char * i_c_StrDevice,			// Serial port device /dev/ttyS0
				int i_i_speed,				// Baud Rate
				int *o_pi_IdProduct)			// Return type of product
{
	I                               l_i_Ret;
	I                               l_i_i;
	UL                              l_ul_NbDevice;
	PT_MSO_USB_DEVICE_PROPERTIES    l_px_Devices;
	struct usb_device               *dev;
	C                               l_ac_string[32];

	l_i_Ret = -1;
	l_px_Devices = NULL;
	dev = NULL;
	l_ul_NbDevice = 0;

	// Com USB
	if(i_i_ConnectionMode == 0)
	{
		if( strncmp(i_c_StrDevice, "/proc/bus/", 10) == 0 )
		{
			fprintf(stdout, "Open device: %s\n", i_c_StrDevice);
			l_i_Ret = MSO_InitCom(i_ph_Mso100Handle, COM_USB, i_c_StrDevice, i_i_speed);
		}
		else
		{
			l_i_Ret = MSO_Usb_EnumDevices( &l_px_Devices, &l_ul_NbDevice);
			if(l_i_Ret < 0)
			{
				fprintf(stdout, "Error USB Enum Devices\n");
				return -1;
			}

			printf("---List of connected devices (Number: %ld)---\n", l_ul_NbDevice);
			for(l_i_i=0; l_i_i < (I)l_ul_NbDevice; l_i_i++)
					printf("\t%d:  %s - %s\n", l_i_i, (l_px_Devices+l_i_i)->m_puc_FriendlyName,
												(l_px_Devices+l_i_i)->m_puc_SerialNumber);

			if(l_ul_NbDevice == 0)
			{
				fprintf(stdout, "No Usb device connected\n");
				l_i_Ret = MSO_Usb_ReleaseEnumDevices(&l_px_Devices, l_ul_NbDevice);
				return -1;
			}
			if(l_ul_NbDevice == 1)
			{
				l_i_i = 0;
				dev = l_px_Devices->m_px_device;
			}
			else if(l_ul_NbDevice>1)
			{
				fprintf(stdout, "Select -> ");
				while(1)
				{
					if(fgets(l_ac_string, 32, stdin) == NULL)
						fprintf(stdout, "->Error\n");
					if(sscanf(l_ac_string, "%d", &l_i_i) == 1)
					{
						if(l_i_i>=0 && l_i_i <l_ul_NbDevice)
							break;
					}
					fprintf(stdout, "Select -> ");
				}
				
				if(l_i_i>=l_ul_NbDevice)
					l_i_i = l_ul_NbDevice-1;
				dev = (l_px_Devices+l_i_i)->m_px_device;
			}

			fprintf(stdout, "Connect to %s - %s\n", (l_px_Devices+l_i_i)->m_puc_FriendlyName,
													(l_px_Devices+l_i_i)->m_puc_SerialNumber);

			*o_pi_IdProduct = dev->descriptor.idProduct;
			l_i_Ret = MSO_InitCom(i_ph_Mso100Handle, COM_USB, (PC)(l_px_Devices+l_i_i)->m_puc_SerialNumber, 0);
			l_i_Ret = MSO_Usb_ReleaseEnumDevices(&l_px_Devices, l_ul_NbDevice);
		}
	}
	// Com RS232
	else if(i_i_ConnectionMode == 1)
	{
		l_i_Ret = MSO_InitCom(i_ph_Mso100Handle, COM_RS232, i_c_StrDevice, i_i_speed);
	}


	// Send Cancel and Clean buffer
	if(l_i_Ret == 0)
	{
		int nb_retry = 2;
		MSO_Cancel(*i_ph_Mso100Handle);
		while(nb_retry > 0)
		{
			I l_i_RecvRet;
			PUC l_puc_data = NULL;
			UL l_ul_len = 0;
			l_i_RecvRet = MSO_ComReceive( *i_ph_Mso100Handle,
										500,				//UL		i_ul_Timeout,
										&l_puc_data,		//PUC*		io_ppuc_Data,
										&l_ul_len			//PUL		o_pul_DataSize
											);

			//printf("MSO_ComReceive ret %d, read %ld\n", l_i_RecvRet, l_ul_len);
			if(l_ul_len > 0)
				MSO_FrameRelease(*i_ph_Mso100Handle, &l_puc_data);

			if(l_ul_len == 0)
				nb_retry--;
		}
	}

	return l_i_Ret;
}

