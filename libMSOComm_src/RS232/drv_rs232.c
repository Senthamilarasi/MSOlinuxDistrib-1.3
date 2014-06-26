#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

#include "libMSO.h"
#include "MSO100.h"

#include "drv_rs232.h"

#define USE_LOG		// Adjust log level in debug.c

/*******************************************************************************
-- extern variable declaration
*******************************************************************************/
extern void dbg(int level, const char *fmt, ...);

/*******************************************************************************
-- static variable declaration
*******************************************************************************/
char   *default_tty = "/dev/ttyS0";
struct termios configuration;
struct termios sauvegarde;
int    fd_tty = -1;
char   g_c_strDevice[20];


/*****************************************************************************/
/*****************************************************************************/
void setspeed (struct termios * config, speed_t vitesse)
{
	cfsetispeed(config, vitesse);
	cfsetospeed(config, vitesse);
}

/*****************************************************************************/
/*****************************************************************************/
I RS232_CloseHandle(void)
{
#ifdef USE_LOG
    dbg(2, "Enter RS232_CloseHandle");
#endif
    if(fd_tty < 0)
    	return RS232ERR_INIT;

    close(fd_tty);

    /* restauration de la configuration originale */
    fd_tty = open(g_c_strDevice, O_RDWR | O_NONBLOCK);
    if(fd_tty)
    {
        tcsetattr(fd_tty, TCSANOW, &sauvegarde);
        close(fd_tty);
    }
    fd_tty = -1;

    return RS232_OK;
}

/*****************************************************************************/
/*****************************************************************************/
I RS232_Initialize( C *   i_strName,
                    UC    i_uc_XonValue, 
                    UC    i_uc_XoffValue,
                    UL 	  i_dw_BaudRate)
{
    I l_i_Ret = 0;
    I l_i_Flag;

#ifdef USE_LOG
    dbg(2, "Enter RS232_Initialize");
#endif

    if(i_strName == NULL)
        return RS232ERR_BADPARAM;

    if( strlen(i_strName) <= sizeof(g_c_strDevice))
    {
        strcpy(g_c_strDevice, i_strName);
    }
    else
    {
        strcpy(g_c_strDevice, default_tty);
	dbg(1, "Use default serial Port: %s", g_c_strDevice);
    }

    // Open device
    fd_tty = open(g_c_strDevice, O_RDWR | O_NONBLOCK | O_NOCTTY);
    if (fd_tty < 0) 
    {
	perror(g_c_strDevice);
	return RS232ERR_ERROR;
    }
    l_i_Flag = fcntl(fd_tty, F_GETFL, 0);
    fcntl(fd_tty, F_SETFL, l_i_Flag & ~O_NONBLOCK);

    // Get actual configuration
    if (tcgetattr(fd_tty, &sauvegarde) != 0)
    {
	perror("tcgetattr");
	return RS232ERR_ERROR;
    }

    // Set New serial configuration
    bzero(&configuration,sizeof(configuration));

    configuration.c_cflag =  CS8 | CREAD | CLOCAL | IXON | IXOFF;
    configuration.c_iflag = 0;
    configuration.c_lflag = 0;
    configuration.c_oflag = 0;
    configuration.c_cc[VSTART] = i_uc_XonValue;  /* Xon Value: 0x11 */
    configuration.c_cc[VSTOP] = i_uc_XoffValue;  /* Xoff Value: 0x13 */
    configuration.c_cc[VTIME] = 0x80;            /* Timeout 128x1/10 sec*/
    configuration.c_cc[VMIN] = 0x00;             /* */

    l_i_Ret = RS232_SetBaudRate(i_uc_XonValue, i_uc_XoffValue, i_dw_BaudRate);
    if (tcsetattr(fd_tty, TCSANOW, &configuration) < 0)
    {
	perror("tcsetattr");
	return RS232ERR_ERROR;
    }

    dbg(1, "Serial Port configuration : %s", i_strName);
    dbg(2, "---> Xon Value: 0x%02X", i_uc_XonValue);
    dbg(2, "---> Xoff Value: 0x%02X", i_uc_XoffValue);
    dbg(2, "---> Baud rate: %ld", i_dw_BaudRate);

    return l_i_Ret;
}


/*****************************************************************************/
/*****************************************************************************/
I RS232_SetBaudRate(  UC    i_uc_XonValue, 
                      UC    i_uc_XoffValue,
                      UL    i_dw_BaudRate)
{
#ifdef USE_LOG
    dbg(2, "Enter RS232_SetBaudRate");
#endif
    // B0 B50 B75 B110 B134 B150 B200 B300 B600 B1200 B1800 B2400 B4800 B9600 B19200 B38400 B57600 B115200 B230400
    dbg(1, "   ---> Set Baud Rate: %ld", i_dw_BaudRate);

    if (i_dw_BaudRate <= 50)
	setspeed(&configuration, B50);
    else if (i_dw_BaudRate <= 75)
    	setspeed(&configuration, B75);
    else if (i_dw_BaudRate <= 110)
    	setspeed(&configuration, B110);
    else if (i_dw_BaudRate <= 134)
    	setspeed(&configuration, B134);
    else if (i_dw_BaudRate <= 150)
    	setspeed(&configuration, B150);
    else if (i_dw_BaudRate <= 200)
    	setspeed(&configuration, B200);
    else if (i_dw_BaudRate <= 300)
    	setspeed(&configuration, B300);
    else if (i_dw_BaudRate <= 600)
    	setspeed(&configuration, B600);
    else if (i_dw_BaudRate <= 1200)
   	setspeed(&configuration, B1200);
    else if (i_dw_BaudRate <= 1800)
    	setspeed(&configuration, B1800);
    else if (i_dw_BaudRate <= 2400)
    	setspeed(&configuration, B2400);
    else if (i_dw_BaudRate <= 4800)
    	setspeed(&configuration, B4800);
    else if (i_dw_BaudRate <= 9600)
    	setspeed(&configuration, B9600);
    else if (i_dw_BaudRate <= 19200)
    	setspeed(&configuration, B19200);
    else if (i_dw_BaudRate <= 38400)
    	setspeed(&configuration, B38400);
    else if (i_dw_BaudRate <= 57600)
    	setspeed(&configuration, B57600);
    else if (i_dw_BaudRate <= 115200)
    	setspeed(&configuration, B115200);
    else if (i_dw_BaudRate <= 230400)
    	setspeed(&configuration, B230400);
    else if (i_dw_BaudRate <= 460800)
    	setspeed(&configuration, B460800);
    else
// Default Baud rate
    	setspeed(&configuration, B115200);

    configuration.c_cc[VSTART] = i_uc_XonValue;  /* Xon Value: 0x11 */
    configuration.c_cc[VSTOP] = i_uc_XoffValue;  /* Xoff Value: 0x13 */

    if(tcsetattr(fd_tty, TCSANOW, &configuration) < 0) 
    {
	perror("tcsetattr");
	return RS232ERR_ERROR;
    }

    tcflush(fd_tty, TCIOFLUSH);

    return RS232_OK;
}

/*****************************************************************************/
/*****************************************************************************/
I RS232_Close(void)
{
#ifdef USE_LOG
    dbg(2, "Enter RS232_Close");
#endif
    RS232_CloseHandle();
    return RS232_OK;	
}


/*****************************************************************************/
/*****************************************************************************/
I RS232_Write( UC *i_puc_Buf,
               UL i_ul_Size,
               UL *o_pul_BytesWritten)
{
    UL	l_ul_NbBytesWritten;
    I   i;

#ifdef USE_LOG
    dbg(4, "Enter RS232_Write");
#endif

    i = 0;

    if(fd_tty < 0)
    	return RS232ERR_INIT;

    l_ul_NbBytesWritten = write(fd_tty, i_puc_Buf, i_ul_Size);

    if(l_ul_NbBytesWritten > 0)
    {
        if(l_ul_NbBytesWritten != i_ul_Size)
	{
	    return RS232ERR_WRITE_TIMEOUT;
	}
        else 
	{
	    if(o_pul_BytesWritten != NULL)
    	        *o_pul_BytesWritten = l_ul_NbBytesWritten;
	}
    }
    else if(l_ul_NbBytesWritten == 0)
    {
        perror("Rs232 Write 0");
	return RS232ERR_WRITE_TIMEOUT;
    }
    else if(l_ul_NbBytesWritten < 0)
    {
        perror("Rs232 Write <0");
	return RS232ERR_WRITE_TIMEOUT;
    }

#ifdef USE_LOG
    dbg(3, "RS232_Write: ---> i_ul_Size %ld RS232 Write: %ld", i_ul_Size, l_ul_NbBytesWritten);
    if(l_ul_NbBytesWritten > 0)
    {
        for(i=0; i<l_ul_NbBytesWritten; i++)
	    dbg(3, "RS232_Write: ---> 0x%02X : %c", i_puc_Buf[i], (isprint(i_puc_Buf[i]) ? i_puc_Buf[i]:' '));
    }
#endif

    return RS232_OK;	
}

/*****************************************************************************/
/*****************************************************************************/
int RS232_Read(UC    *i_puc_Buf,
               UL    i_ul_Size,
               UL    i_dw_ReadTotalTimeoutConstant)
{
    I	l_i_NbBytesRead;

#ifdef USE_LOG
    dbg(4, "Enter RS232_Read");
#endif

    l_i_NbBytesRead = 0;
    *i_puc_Buf = 0xFF;

    l_i_NbBytesRead = read(fd_tty, i_puc_Buf, i_ul_Size);
    if(l_i_NbBytesRead > 0)
    {
#ifdef USE_LOG
        dbg(3, "RS232_Read: i_ul_Size: %ld, Timeout: %ld---> Read: %d, Char--> 0x%02X :%c", 
					i_ul_Size, i_dw_ReadTotalTimeoutConstant, 
					l_i_NbBytesRead, i_puc_Buf[0], 
					(isprint(i_puc_Buf[0]) ? i_puc_Buf[0]:' ') );

#endif
	if( l_i_NbBytesRead != i_ul_Size)
	    return RS232ERR_READ_TIMEOUT;	
    }
    else if(l_i_NbBytesRead == 0)
    {
        perror("Rs232 read 0");
	return RS232ERR_READ_TIMEOUT;	
    }
    else
    {
        perror("Rs232 read < 0");
	return RS232ERR_READ_TIMEOUT;	
    }

    return RS232_OK; 
}

/*****************************************************************************/
/*****************************************************************************/
I RS232_Break(UL i_ul_Time)
{
    I	l_i_Ret;

#ifdef USE_LOG
    dbg(2, "Enter RS232_Read");
    dbg(3, "RS232_Break: %ld", i_ul_Time);
#endif 

    l_i_Ret = tcsendbreak (fd_tty, 0);//i_ul_Time*100);
    if(l_i_Ret < 0)
    {
        perror("tcsendbreak");
        return RS232ERR_ERROR;
    }

    tcflush(fd_tty, TCIOFLUSH);
    if(l_i_Ret < 0)
    {
        perror("tcflush");
        return RS232ERR_ERROR;
    }

    return RS232_OK;
}

/*****************************************************************************/
/*****************************************************************************/
I RS232_IsInitialize(void)
{
#ifdef USE_LOG
    dbg(2, "Enter RS232_IsInitialize");
#endif 

    if(fd_tty < 0)
    	return FALSE;
    else
	return TRUE;

    return RS232_OK;
}

/*****************************************************************************/
/*****************************************************************************/
UL RS232_ReturnHandlePort(void)
{
#ifdef USE_LOG
    dbg(2, "Enter RS232_ReturnHandlePort");
#endif 
  return fd_tty;
}

/*****************************************************************************/
/*****************************************************************************/
I RS232_GetConfig(void)
{
    struct termios l_x_conf;

#ifdef USE_LOG
    dbg(2, "Enter RS232_GetConfig");
#endif 

    tcgetattr(fd_tty, &l_x_conf); 

    dbg(1, "RS232_GetConfig: c_cflag = 0x%02X", l_x_conf.c_cflag); 
    dbg(1, "RS232_GetConfig: c_iflag = 0x%02X", l_x_conf.c_iflag); 
    dbg(1, "RS232_GetConfig: c_lflag = 0x%02X", l_x_conf.c_lflag); 
    dbg(1, "RS232_GetConfig: c_oflag = 0x%02X", l_x_conf.c_oflag); 
    dbg(1, "RS232_GetConfig: VTIME = 0x%02X", l_x_conf.c_cc[VTIME]); 
    dbg(1, "RS232_GetConfig: VMIN  = 0x%02X", l_x_conf.c_cc[VMIN]); 
    dbg(1, "RS232_GetConfig: XON   = 0x%02X", l_x_conf.c_cc[VSTART]); 
    dbg(1, "RS232_GetConfig: XOFF  = 0x%02X", l_x_conf.c_cc[VSTOP]); 

    return RS232_OK;
}

/*****************************************************************************/
/*****************************************************************************/
I    RS232_Clear(void)
{
    if(fd_tty < 0)
        return RS232ERR_INIT;

    tcflush(fd_tty, TCIOFLUSH);

    return RS232_OK;
}

