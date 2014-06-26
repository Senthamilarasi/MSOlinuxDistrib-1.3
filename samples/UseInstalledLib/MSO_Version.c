#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <libMSO/libMSO.h>

#define MSO_VERSION_VERSION            "1.0"

#define DEFAULT_SERIAL_PORT             "/dev/ttyS0"
#define DEFAULT_BAUD_RATE               115200

extern int MSO_Connect( MORPHO_HANDLE  	*i_h_Mso100Handle,
                 	int 		i_i_ConnectionMode,      // COM_USB or COM_RS232
                 	char* 		i_c_StrDevice,           // Serial port device /dev/ttyS0
                 	int 		i_i_speed,               // Baud Rate
                 	int*		o_pi_IdProduct);         // Return type of product

extern int ConvertError(UC i_uc_Status);

/*****************************************************************************/
/*****************************************************************************/
void usage(void)
{
    fprintf(stdout, "---------------- MSO_Version Options------------------ \n");
    fprintf(stdout, "Connection mode     : -c (Default: USB) \n");
    fprintf(stdout, "---->  -c USB       : Usb Connection \n");
    fprintf(stdout, "---->  -c RS232     : Serial Connection:  Port /dev/ttyS0  \n");
    fprintf(stdout, "Speed (RS232)       : -s (Default: 115200) \n");
    fprintf(stdout, "---->  -s <speed>   : Set baud rate to speed \n");
    fprintf(stdout, "Device Name (RS232) : -d (Default: /dev/ttyS0) \n");
    fprintf(stdout, "---->  -d <device>  : Set device name to device \n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Example :\n");
    fprintf(stdout, "----> MSO_Version -c USB\n");
    fprintf(stdout, "----> MSO_Version -c RS232 -s 115200\n");
    fprintf(stdout, "\n");

}

/*****************************************************************************/
/*****************************************************************************/
void show_banner(void)
{
    fprintf(stdout, "\n--------------------------------------------------\n");
    fprintf(stdout, "MSO_Version Sample program - Version %s\nCompiled: "__DATE__"\n", MSO_VERSION_VERSION);
    fprintf(stdout, "Sagem Defense Securite Copyright (C) 2006\n");
    fprintf(stdout, "Type 'MSO_Version -h' for available options\n");
    fprintf(stdout, "--------------------------------------------------\n\n");
}


/*****************************************************************************/
/*****************************************************************************/
int main(int argc, char *argv[])
{
    I                    	l_i_Ret;
    UC                   	l_uc_Status;
    MORPHO_HANDLE        	g_h_Mso100Handle;
    I				l_i_speed;
    C                           l_ac_Device[32];
    UC				l_uc_ConnectionType;
    I                           l_i_IdProduct;
    UC                   	l_auc_DescProduct[1024];
    UC                   	l_auc_DescSensor[1024];
    UC                   	l_auc_DescSoftware[1024];
    UC				l_auc_SerialNumber[MSO_SERIAL_NUMBER_LEN+1];
    UC				l_uc_SecuConfig;
    US				l_us_SecuMaxFAR;
    I                           option;
    C                           *liste_options = "c:s:d:h";
    
    l_uc_ConnectionType = 0;
    g_h_Mso100Handle = NULL;
    l_i_speed = DEFAULT_BAUD_RATE;
    sprintf(l_ac_Device, "%s",  DEFAULT_SERIAL_PORT);
    memset(l_auc_SerialNumber, 0, MSO_SERIAL_NUMBER_LEN+1);

    show_banner();

    // Options
    while ((option = getopt(argc, argv, liste_options)) != -1)
    {
        switch (option)
        {
            case 'c' :
                if(strcmp(optarg, "RS232") == 0)
                {
                    fprintf(stdout, "Use Serial Port\n");
                    l_uc_ConnectionType = 1;
                }
                else if(strcmp(optarg, "USB") == 0)
                {
                    fprintf(stdout, "Use USB bus\n");
                    l_uc_ConnectionType = 0;
                }
                break;
	    case 's' :
		sscanf(optarg, "%d", &l_i_speed);
		break;
 	    case 'd' :
                sscanf(optarg, "%s", l_ac_Device);
                break;
  	    case 'h' :
                usage();
                return 0;
            case '?' :
                fprintf(stdout, "Bad option\n");
                usage();
                return 0;
	}
    }

    // Connection to MSO 
    l_i_Ret = MSO_Connect(&g_h_Mso100Handle, l_uc_ConnectionType, l_ac_Device, l_i_speed, &l_i_IdProduct);
    if ((l_i_Ret != 0) || (g_h_Mso100Handle == NULL))
    {
        fprintf(stdout, "Error Connection\n");
        return -1;
    }
    
    // Send GetDescriptor ILV 
    l_i_Ret = MSO_GetDescriptor( g_h_Mso100Handle,
                                 ID_FORMAT_TEXT,                  //UC      i_uc_DescFormat,
                                 &l_uc_Status,       		  //PUC     o_puc_ILV_Status,
                                 l_auc_DescProduct,               //VOID*   i_pv_DescProduct,
                                 1024,                            //UL      i_ul_SizeOfDescProduct,
                                 l_auc_DescSensor,                //VOID*   i_pv_DescSensor,
                                 1024,                            //UL      i_ul_SizeOfDescSensor,
                                 l_auc_DescSoftware,              //VOID*   i_pv_DescSoftware,
                                 1024                             //UL      i_ul_SizeOfDescSoftware
                              );
    if( (l_i_Ret == RETURN_NO_ERROR) && (l_uc_Status == ILV_OK))
    {
        fprintf(stdout, "\n----------------------------\n");
        fprintf(stdout, "MSO_GetDescriptor:\n");
        fprintf(stdout, "-->Product:\n%s\n",l_auc_DescProduct);
        fprintf(stdout, "-->Sensor:\n%s\n",l_auc_DescSensor);
        fprintf(stdout, "-->Software:\n%s\n",l_auc_DescSoftware);
    }
    else
    {
        fprintf(stdout, "Error MSO_GetDescriptor\n");
	ConvertError(l_uc_Status);
    }

    l_i_Ret = MSO_SECU_GetSerialNumber( g_h_Mso100Handle,
        				l_auc_SerialNumber,	//UC  o_auc_SerialNumber[MSO_SERIAL_NUMBER_LEN],
        				&l_uc_SecuConfig,	//PUC o_puc_SecuConfig,
        				&l_us_SecuMaxFAR,	//PUS o_pus_SecuMaxFAR,
        				&l_uc_Status		//PUC o_puc_ILV_Status
				      );
    if( (l_i_Ret == RETURN_NO_ERROR) && (l_uc_Status == ILV_OK))
    {
        fprintf(stdout, "\n----------------------------\n");
        fprintf(stdout, "MSO Get Security configuration:\n");
        fprintf(stdout, "-->Serial Number:%s\n", l_auc_SerialNumber);
        fprintf(stdout, "-->SecuConfig: 0x%02X\n",l_uc_SecuConfig);
	if(l_uc_SecuConfig & SECU_TUNNELING)
           fprintf(stdout, "\tThe MorphoSmart uses the tunneling protocol\n");
	if(l_uc_SecuConfig & SECU_OFFERED_SECURITY)
           fprintf(stdout, "\tThe MorphoSmart uses the offered security protocol\n");
	else
           fprintf(stdout, "\tThe Communication is not secured\n");

        if(l_uc_SecuConfig & 0x10)
           fprintf(stdout, "\tThe MorphoSmart never export its matching score\n");
	else
           fprintf(stdout, "\tThe MorphoSmart can export its matching score\n");
	
	
        fprintf(stdout, "-->SecuMaxFAR: %d\n",l_us_SecuMaxFAR);
        fprintf(stdout, "----------------------------\n\n");                        
    }
    else
    {
        fprintf(stdout, "Error MSO_GetDescriptor\n");
	ConvertError(l_uc_Status);
    }

                      
    l_i_Ret = MSO_CloseCom( &g_h_Mso100Handle);
    if(l_i_Ret != RETURN_NO_ERROR) 
    {
        fprintf(stdout, "Error MSO_CloseCom\n");
    }

    return 0;
}
