#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "libMSO.h"
#include "MSO_TestBio.h"


pthread_t	acq_thread;
void *pf_acq_thread(void*);

MORPHO_HANDLE g_h_Mso100Handle;
UC g_uc_ExportScore = 0;
UL g_ul_cnt = 0;

I CommandCallback (PVOID i_pv_context, I i_i_command, PVOID i_pv_param)
{
	T_MORPHO_CALLBACK_IMAGE_STATUS l_x_ImageStructure;
	PUC l_puc_EnrollmentCmd;
	C l_c_String[256];
	UC l_uc_quality;

	if (i_i_command == MORPHO_CALLBACK_COMMAND_CMD)
	{
		switch (*((PI) i_pv_param))
		{
			case MORPHO_REMOVE_FINGER:
				printf("REMOVE FINGER\n");
			break;
			case MORPHO_MOVE_NO_FINGER:
				printf ("NO FINGER\n");
			break;
			case MORPHO_LATENT:
				printf ("LATENT\n");
			break;
			case MORPHO_MOVE_FINGER_UP:
				printf ("MOVE UP\n");
			break;
			case MORPHO_MOVE_FINGER_DOWN:
				printf ("MOVE DOWN\n");
			break;
			case MORPHO_MOVE_FINGER_LEFT:
				printf ("MOVE LEFT\n");
			break;
			case MORPHO_MOVE_FINGER_RIGHT:
				printf ("MOVE RIGHT\n");
			break;
			case MORPHO_PRESS_FINGER_HARDER:
				printf ("Press Harder\n");
			break;
			case MORPHO_FINGER_OK:
				printf ("Acquisition ok\n");
			break;
			default:
			break;
		}
	}

	if (i_i_command == MORPHO_CALLBACK_ENROLLMENT_CMD)
	{
		l_puc_EnrollmentCmd = (PUC) i_pv_param;
		switch (l_puc_EnrollmentCmd[0])
		{
			case 1:
				switch (l_puc_EnrollmentCmd[2])
				{
				case 1:
					sprintf (l_c_String,
						"Place Right Forefinger -> Capture %d/%d\n",
						l_puc_EnrollmentCmd[2], l_puc_EnrollmentCmd[3]);
					printf (l_c_String);
					break;
			
				case 2:
					sprintf (l_c_String,
						"Right Forefinger Again -> Capture %d/%d\n",
						l_puc_EnrollmentCmd[2], l_puc_EnrollmentCmd[3]);
					printf(l_c_String);
					break;
			
				case 3:
					sprintf (l_c_String,
						"Right Forefinger Again -> Capture %d/%d\n",
						l_puc_EnrollmentCmd[2], l_puc_EnrollmentCmd[3]);
					printf(l_c_String);
					break;
				}
				break;
	
			case 2:
			switch (l_puc_EnrollmentCmd[2])
			{
				case 1:
					sprintf (l_c_String,
						"Place Left Forefinger -> Capture %d/%d\n",
						l_puc_EnrollmentCmd[2], l_puc_EnrollmentCmd[3]);
					printf(l_c_String);
					break;
			
				case 2:
					sprintf (l_c_String,
						"Left Forefinger Again -> Capture %d/%d\n",
						l_puc_EnrollmentCmd[2], l_puc_EnrollmentCmd[3]);
					printf (l_c_String);
					break;
			
				case 3:
					sprintf (l_c_String,
						"Left Forefinger Again -> Capture %d/%d\n",
						l_puc_EnrollmentCmd[2], l_puc_EnrollmentCmd[3]);
					printf (l_c_String);
					break;
			}
			break;
		}
	}

	if (i_i_command == MORPHO_CALLBACK_IMAGE_CMD)
	{
		memcpy( &l_x_ImageStructure.m_x_ImageHeader, 
				(T_MORPHO_IMAGE_HEADER *) i_pv_param,
				sizeof (T_MORPHO_IMAGE_HEADER));
		l_x_ImageStructure.m_puc_Image = (PUC) i_pv_param + sizeof (T_MORPHO_IMAGE_HEADER);
	//	printf("Async Image %ld: %dx%d\n", g_ul_cnt++,
	//				l_x_ImageStructure.m_x_ImageHeader.m_us_NbRow,
	//				l_x_ImageStructure.m_x_ImageHeader.m_us_NbCol);
	}

	if (i_i_command == MORPHO_CALLBACK_CODEQUALITY)
	{
		l_uc_quality = *(PUC) i_pv_param;
		sprintf(l_c_String, "Code Quality : %d\n", l_uc_quality);
		printf(l_c_String);
	}

	if (i_i_command == MORPHO_CALLBACK_DETECTQUALITY)
	{
		l_uc_quality = *(PUC) i_pv_param;
		sprintf (l_c_String, "Detect Quality : %d\n", l_uc_quality);
		printf(l_c_String);
	}

	return 0;
}


void *pf_acq_thread(void *param)
{
	int l_i_Ret;
	unsigned short l_us_Timeout = 0;
	unsigned long l_ul_AsynchronousEvnt;

	l_ul_AsynchronousEvnt = MORPHO_CALLBACK_DETECTQUALITY 	|
							MORPHO_CALLBACK_CODEQUALITY 	|
							MORPHO_CALLBACK_IMAGE_CMD 		|
							MORPHO_CALLBACK_COMMAND_CMD 	|
							MORPHO_CALLBACK_ENROLLMENT_CMD;

	l_i_Ret = MSO_RegisterAsyncILV ( param, 
									l_ul_AsynchronousEvnt, 
									(T_pFuncILV_Buffer) CommandCallback,
									NULL
									);
	
	l_i_Ret = Identify(param, l_ul_AsynchronousEvnt, l_us_Timeout);

	MSO_UnregisterAllAsyncILV (param);
	l_ul_AsynchronousEvnt = 0;
	fflush(stdout);

//	pthread_exit((void*)1);
	return (void*)0;
}


int CommandMenu ()
{
	char l_ac_string[32];
	int l_i_i = 0;
	
	fprintf (stdout, "-------------- MSO ILV Command --------------\n");
	fprintf (stdout, "\tQuit -------------------->(0)\n");
	fprintf (stdout, "\tIdentify   -------------->(1)\n");
	fprintf (stdout, "\tCancel Aquisition   ----->(2)\n");
	
	fprintf (stdout, "Select -> ");
	while (1)
	{
		if (fgets (l_ac_string, 32, stdin) == NULL)
		{
		fprintf (stdout, "->Error\n");
		return -1;
		}
		if (sscanf (l_ac_string, "%d", &l_i_i) == 1)
		{
		if (l_i_i >= 0 && l_i_i <= 15)
		break;
		}
		fprintf (stdout, "Select -> ");
	}
	
	return l_i_i;
}


int main(int argc, char **argv)
{

	int l_i_Ret;
	int l_i_IdProduct;
	int l_i_loop;
	int l_i_i;

	l_i_Ret = MSO_Connect( &g_h_Mso100Handle, 0, "/dev/ttyS0", 115200, &l_i_IdProduct);
	if ((l_i_Ret != 0) || (g_h_Mso100Handle == NULL))
	{
		fprintf (stdout, "Error Connection\n");
		return -1;
	}

	l_i_loop = 1;
	while (l_i_loop)
	{
		switch ((l_i_i = CommandMenu ()))
		{
			default:
				fprintf (stdout, "unrecognize command: %d\n", l_i_i);
				break;
			case 0:
				l_i_loop = 0;
				break;

			case 1:
				if (pthread_create (&acq_thread, 0, pf_acq_thread, (void *) g_h_Mso100Handle) != 0)
				{
					perror ("can't create usb_thread\n");
					exit(1);
				}
				break;

			case 2:
				l_i_Ret = MSO_Cancel(g_h_Mso100Handle);
				fprintf (stdout, "Cancel Acquisition %d\n", l_i_Ret);
				break;
		}
	}

	MSO_CloseCom(&g_h_Mso100Handle);
	return 0;
}



