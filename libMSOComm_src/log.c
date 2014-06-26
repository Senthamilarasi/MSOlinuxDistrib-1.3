#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>


int   g_dbg_Level = 1;		// Debug level: 0: none, 1: Init, 2: function calls, 3: Read/Write ... 
int   g_dbg_time = 0;		// Display time: 1: Display on each line, 0: no display
int   g_dbg_file = 0;		// Log in file: 1: File, 0: stdout


void dbg(int level, const char *fmt, ...);
void LogMessage(char* i_c_FileName, int mode, const char *fmt, ...);

/*****************************************************************************/
/*****************************************************************************/
void dbg(int level, const char *fmt, ...)
{
    va_list args;
    char buf[256];

    if(g_dbg_Level >= level)
    {
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        if( g_dbg_file == 1)
            LogMessage("Driver.log", g_dbg_time, buf);
	else
            LogMessage(NULL, g_dbg_time, buf);
        va_end(args);
    }
}

/*****************************************************************************/
/*****************************************************************************/
void LogMessage(char* i_c_FileName, int mode, const char *fmt, ...)
{
    va_list args;
    char buf[256];
    char tmp[256];
    time_t t;
    struct tm *tm;
    FILE *fp=NULL;

    if(i_c_FileName != NULL)
    {
        if ( (fp = fopen(i_c_FileName, "a")) == NULL )
        {
            fprintf(stdout, "Erreur ouverture %s", i_c_FileName);
            return;
        }
    }

    t = time(NULL);
    tm = localtime(&t);

    va_start(args, fmt);
    vsprintf(buf, fmt, args);

    if(mode==1)
    {
        sprintf(tmp, "%02d/%02d/%02d %02d:%02d:%02d -> %s\n", tm->tm_mday,
                                                              tm->tm_mon + 1,
                                                              tm->tm_year + 1900,
                                                              tm->tm_hour,
                                                              tm->tm_min,
                                                              tm->tm_sec,
                                                              buf);

    }
    else
    {
	sprintf(tmp, "%s\n", buf);
    }

    if(fp == NULL)
    {
        fprintf(stdout, tmp);
    }
    else
    {
        fprintf(fp, tmp);
        fclose(fp);
    }

    va_end(args);
}

