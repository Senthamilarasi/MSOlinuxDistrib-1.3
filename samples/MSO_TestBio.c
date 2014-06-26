#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "libMSO.h"
#include "Ilv_definitions.h"
#include "MSO_TestBio.h"

#define MSO_TEST_BIO_VERSION	"1.0"

#define DEFAULT_SERIAL_PORT		"/dev/ttyS0"
#define DEFAULT_BAUD_RATE		115200


#define CORRECT_LEVEL2(a)	if (a == 0x00)	   \
								a = 0x02;      \
							else if (a == 0x40)\
								a = 0x48;      \
							else if (a == 0x80)\
								a = 0xB0;      \
							else if (a == 0xC0)\
								a = 0xFE;      \

#define CORRECT_LEVEL1(a)	if (a == 0x00)     \
								a = 0x00;      \
							else if (a == 0x80)\
								a = 0xFF;      \


SDL_Surface *screen;
SDL_Surface *img;
SDL_Event event;
SDL_Color colors[256];
US g_us_ScreenNbRow;
US g_us_ScreenNbCol;
UC g_uc_IsVideoInit;
UC g_uc_IsSDL_ttfInit;
MORPHO_HANDLE g_h_Mso100Handle;

UC g_uc_ExportScore;

/*****************************************************************************/
/*****************************************************************************/
void usage (void)
{
	fprintf (stdout, "---------------- MSO_TestBio Options------------------ \n");
	fprintf (stdout, "Connection mode         : -c (Default: USB) \n");
	fprintf (stdout, "---->  -c USB           : Usb Connection \n");
	fprintf (stdout, "---->  -c RS232         : Serial Connection:  Port /dev/ttyS0  \n");
	fprintf (stdout, "Speed (RS232)           : -s (Default: 115200) \n");
	fprintf (stdout, "---->  -s <speed>       : Set baud rate to <speed> \n");
	fprintf (stdout, "Device Name (RS232)     : -d (Default: /dev/ttyS0) \n");
	fprintf (stdout, "---->  -d <device>      : Set device name to device \n");
	fprintf (stdout, "Timeout                 : -t Timeout for acquisition function (Default: Infinite)\n");
	fprintf (stdout, "---->  -t <timeout>     : Set Timeout to <timeout> second, 0=> infinite timeout\n");
	fprintf (stdout, "Asynchronous event      : -e (Default: on)\n");
	fprintf (stdout, "---->  -e EVENT_OFF     : Set asynchronous event off\n");
	fprintf (stdout, "---->  -e EVENT_ON      : Set asynchronous event on\n");
	fprintf (stdout, "Export Score            : -x (Default: SCORE_ON)\n");
	fprintf (stdout, "---->  -x SCORE_OFF     : Export score off\n");
	fprintf (stdout, "---->  -x SCORE_ON      : Export score on\n");
	fprintf (stdout, "Export PK (Enroll)      : -p (Default: EXPORT_PK_OFF)\n");
	fprintf (stdout, "---->  -p EXPORT_PK_OFF : Export PK off\n");
	fprintf (stdout, "---->  -p EXPORT_PK_ON  : Export PK on\n");
	fprintf (stdout, "Export Image (Enroll)   : -i (Default: EXPORT_IMG_OFF)\n");
	fprintf (stdout, "---->  -i EXPORT_IMG_OFF: Export Image off\n");
	fprintf (stdout, "---->  -i EXPORT_IMG_ON : Export Image on\n");
	fprintf (stdout, "\n");
	fprintf (stdout, "Examples :\n");
	fprintf (stdout, "----> MSO_TestBio -c USB -p EXPORT_PK_ON \n");
	fprintf (stdout, "----> MSO_TestBio -c RS232 -s 57600 -e off \n");
	fprintf (stdout, "\n");
	fprintf (stdout, "In order to test verify, verify match and identify match function\n");
	fprintf (stdout, "you can create a pkcomp file with EXPORT_PK_ON option in Enroll function\n");
	fprintf (stdout, "\n");

}

/*****************************************************************************/
/*****************************************************************************/
void show_banner (void)
{
	fprintf (stdout, "\n--------------------------------------------------\n");
	fprintf (stdout, "MSO_TestBio Sample program - Version %s\nCompiled: " __DATE__
					 "\n", MSO_TEST_BIO_VERSION);
	fprintf (stdout, "Sagem Defense Securite Copyright (C) 2006\n");
	fprintf (stdout, "Type 'MSO_TestBio -h' for available options\n");
	fprintf (stdout, "--------------------------------------------------\n\n");
}

/*****************************************************************************/
/*****************************************************************************/
SDL_Surface *CreateTextSurface(char *texte, char *font_face, short font_size, SDL_Color fgColor, SDL_Color bgColor)
{
	TTF_Font *font;
	SDL_Surface *text;
	
	font = TTF_OpenFont (font_face, font_size);
	if (!font)
	{
		return NULL;
	}
	// text = TTF_RenderText_Solid(font, texte, fgColor);
	// text = TTF_RenderText_Blended(font, texte, fgColor);
	text = TTF_RenderText_Shaded (font, texte, fgColor, bgColor);
	
	if (text == NULL)
		fprintf (stderr, "Error create surface with text : %s\n", SDL_GetError ());
	
	TTF_CloseFont (font);
	
	return text;
}


/*****************************************************************************/
/*****************************************************************************/
I DisplayText (char *i_c_String, int i_i_field)
{
	SDL_Surface *text;		// Surface containing text
	SDL_Color fg;			// Foreground color
	SDL_Color bg;			// Background color
	SDL_Rect coords;
	SDL_Rect Rect_field;

	if ((screen == NULL) || (g_uc_IsSDL_ttfInit == 0) || (g_uc_IsVideoInit == 0))
	{
		fprintf (stdout, "%s\n", i_c_String);
		return -1;
	}

	fg.r = 0;
	fg.g = 0;
	fg.b = 0;
	bg.r = 255;
	bg.g = 255;
	bg.b = 255;
	text = CreateTextSurface (i_c_String, "Vera.ttf", 10, fg, bg);
	if (text == NULL)
	{
		fprintf (stdout, "%s\n", i_c_String);
		return -1;
	}

	Rect_field.x = 0;
	Rect_field.y = screen->h - i_i_field * text->h;
	Rect_field.w = screen->w;
	Rect_field.h = text->h;

	if (SDL_FillRect (screen, &Rect_field, 180) < 0)
		fprintf (stderr, "Error SDL_FillRect: %s\n", SDL_GetError ());
	
	coords.x = screen->w / 2 - text->w / 2;
	coords.y = screen->h - i_i_field * text->h;
	if (SDL_BlitSurface (text, NULL, screen, &coords) < 0)
		fprintf (stderr, "Error BlitSurface : %s\n", SDL_GetError ());
	
	SDL_FreeSurface (text);
	SDL_UpdateRect (screen, 0, Rect_field.y, Rect_field.w, Rect_field.h);

	return 0;
}

/*****************************************************************************/
/*****************************************************************************/
I ConvertImage(	PUC i_puc_Img,
				US i_us_NbRow,
				US i_us_NbCol,
				UC i_uc_NbBitsPerPixel,
				PUC o_puc_Img8BitsPerPixel)
{
	I l_i_ImageSize;
	PUC l_puc_image8bits;
	I l_i_Cnt;

	l_puc_image8bits = o_puc_Img8BitsPerPixel;
	
	l_i_ImageSize = i_us_NbRow * i_us_NbCol;
	switch (i_uc_NbBitsPerPixel)
	{
	case 8:
		memcpy (o_puc_Img8BitsPerPixel, i_puc_Img, l_i_ImageSize);
		break;
	case 4:
		for (l_i_Cnt = 0; l_i_Cnt < l_i_ImageSize / 2; l_i_Cnt++)
		{
			l_puc_image8bits[2 * l_i_Cnt] = (i_puc_Img[l_i_Cnt] & 0xF0) + 0x08;
			l_puc_image8bits[2 * l_i_Cnt + 1] =((i_puc_Img[l_i_Cnt] & 0x0F) << 4) + 0x08;
		}
		break;
	case 2:
		for (l_i_Cnt = 0; l_i_Cnt < l_i_ImageSize / 4; l_i_Cnt++)
		{
			l_puc_image8bits[4 * l_i_Cnt] = (i_puc_Img[l_i_Cnt] & 0xC0);
			l_puc_image8bits[4 * l_i_Cnt + 1] = ((i_puc_Img[l_i_Cnt] & 0x30) << 2);
			l_puc_image8bits[4 * l_i_Cnt + 2] = ((i_puc_Img[l_i_Cnt] & 0x0C) << 4);
			l_puc_image8bits[4 * l_i_Cnt + 3] = ((i_puc_Img[l_i_Cnt] & 0x03) << 6);
			CORRECT_LEVEL2 (l_puc_image8bits[4 * l_i_Cnt])
			CORRECT_LEVEL2 (l_puc_image8bits[4 * l_i_Cnt + 1])
			CORRECT_LEVEL2 (l_puc_image8bits[4 * l_i_Cnt + 2])
			CORRECT_LEVEL2 (l_puc_image8bits[4 * l_i_Cnt + 3])
		}
		break;
	case 1:
		for (l_i_Cnt = 0; l_i_Cnt < l_i_ImageSize / 8; l_i_Cnt++)
		{
			l_puc_image8bits[8 * l_i_Cnt] = (UC) (i_puc_Img[l_i_Cnt] & 0x80);
			l_puc_image8bits[8 * l_i_Cnt + 1] = (UC) ((i_puc_Img[l_i_Cnt] & 0x40) << 1);
			l_puc_image8bits[8 * l_i_Cnt + 2] = (UC) ((i_puc_Img[l_i_Cnt] & 0x20) << 2);
			l_puc_image8bits[8 * l_i_Cnt + 3] = (UC) ((i_puc_Img[l_i_Cnt] & 0x10) << 3);
			l_puc_image8bits[8 * l_i_Cnt + 4] = (UC) ((i_puc_Img[l_i_Cnt] & 0x08) << 4);
			l_puc_image8bits[8 * l_i_Cnt + 5] = (UC) ((i_puc_Img[l_i_Cnt] & 0x04) << 5);
			l_puc_image8bits[8 * l_i_Cnt + 6] = (UC) ((i_puc_Img[l_i_Cnt] & 0x02) << 6);
			l_puc_image8bits[8 * l_i_Cnt + 7] = (UC) ((i_puc_Img[l_i_Cnt] & 0x01) << 7);
			CORRECT_LEVEL1 (l_puc_image8bits[8 * l_i_Cnt])
			CORRECT_LEVEL1 (l_puc_image8bits[8 * l_i_Cnt + 1])
			CORRECT_LEVEL1 (l_puc_image8bits[8 * l_i_Cnt + 2])
			CORRECT_LEVEL1 (l_puc_image8bits[8 * l_i_Cnt + 3])
			CORRECT_LEVEL1 (l_puc_image8bits[8 * l_i_Cnt + 4])
			CORRECT_LEVEL1 (l_puc_image8bits[8 * l_i_Cnt + 5])
			CORRECT_LEVEL1 (l_puc_image8bits[8 * l_i_Cnt + 6])
			CORRECT_LEVEL1 (l_puc_image8bits[8 * l_i_Cnt + 7])
		}
		break;
	default:
		fprintf (stderr, "Invalid value i_uc_NbBitsPerPixel: %d\n", i_uc_NbBitsPerPixel);
		return -1;
	}

	return 0;
}


/*****************************************************************************/
/*****************************************************************************/
I InitScreen ()
{
	I l_i_i;
	const SDL_VideoInfo *vi;

	/*
	* Initialisation de SDL 
	*/
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "Error SDL: %s\n", SDL_GetError ());
		return -1;
	}
	g_uc_IsVideoInit = 1;
	atexit (SDL_Quit);

	/*
	* initialisation de SDL_ttf: affichage de texte dans une fenetre SDL 
	*/
	if (TTF_Init () < 0)
	{
		fprintf (stderr, "Error init SDL_TTF: %s\n", SDL_GetError ());
		return -1;
	}
	g_uc_IsSDL_ttfInit = 1;
	
	g_us_ScreenNbRow = 420;
	g_us_ScreenNbCol = 420;

	for (l_i_i = 0; l_i_i < 256; l_i_i++)
	{
		colors[l_i_i].r = l_i_i;
		colors[l_i_i].g = l_i_i;
		colors[l_i_i].b = l_i_i;
	}
	
	screen = SDL_SetVideoMode (g_us_ScreenNbCol,
					g_us_ScreenNbRow + 60,
					8, SDL_RESIZABLE | SDL_HWPALETTE);
	
	SDL_SetColors (screen, colors, 0, 256);
	
	vi = SDL_GetVideoInfo ();
	if (vi && vi->wm_available)
		SDL_WM_SetCaption ("Capture Window", "Icone SDL");
	
	sleep (1);
	return 0;
}

/*****************************************************************************/
/*****************************************************************************/
I CloseScreen ()
{
	if (g_uc_IsSDL_ttfInit == 1)
		TTF_Quit ();
	
	if (g_uc_IsVideoInit == 1)
	{
		SDL_FreeSurface (screen);
		SDL_Quit ();
	}

	return 0;
}

/*****************************************************************************/
/*****************************************************************************/
I Display_Image (PUC i_puc_Img, US i_us_NbRow, US i_us_NbCol,
		UC i_uc_NbBitsPerPixel)
{
	I l_i_Ret;
	SDL_Rect coords;
	SDL_Rect Rect;
	PUC l_puc_image8bits;
	
	if ((i_uc_NbBitsPerPixel <= 0) || (i_us_NbRow <= 0) || (i_us_NbCol <= 0))
		return -1;

	l_puc_image8bits = malloc (648 * 488);
	l_i_Ret =
		ConvertImage (i_puc_Img, i_us_NbRow, i_us_NbCol,
			i_uc_NbBitsPerPixel, l_puc_image8bits);
	if (l_i_Ret < 0)
		return -1;
	
	img = SDL_AllocSurface (SDL_SWSURFACE, i_us_NbCol,	// img->w,
							i_us_NbRow,	// img->h,
							8,	// img->format->BitsPerPixel,
							0x00,	// 0xF800, // img->format->Rmask,
							0x00,	// 0x07E0, // img->format->Gmask,
							0x00,	// 0x001F, //img->format->Bmask,
							0x00	// img->format->Amask
		);
	SDL_SetColors (img, colors, 0, 256);
	
	coords.x = screen->w / 2 - img->w / 2;
	coords.y = g_us_ScreenNbRow / 2 - img->h / 2;
	coords.w = i_us_NbCol;
	coords.h = i_us_NbRow;
	
	memcpy (img->pixels, (void *) l_puc_image8bits, i_us_NbCol * i_us_NbRow);

	Rect.x = 0;
	Rect.y = 0;
	Rect.w = g_us_ScreenNbCol;
	Rect.h = g_us_ScreenNbRow;
	SDL_FillRect (screen, &Rect, 0);
	SDL_BlitSurface (img, 0, screen, &coords);
	SDL_UpdateRect (screen, Rect.x, Rect.y, Rect.w, Rect.h);
	SDL_FreeSurface (img);
	
	if (l_puc_image8bits != NULL)
		free (l_puc_image8bits);

	return 0;
}


/*****************************************************************************/
/*****************************************************************************/
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
		DisplayText ("REMOVE FINGER", 1);
		break;
		case MORPHO_MOVE_NO_FINGER:
		DisplayText ("NO FINGER", 1);
		break;
		case MORPHO_LATENT:
		DisplayText ("LATENT", 1);
		break;
		case MORPHO_MOVE_FINGER_UP:
		DisplayText ("MOVE UP", 1);
		break;
		case MORPHO_MOVE_FINGER_DOWN:
		DisplayText ("MOVE DOWN", 1);
		break;
		case MORPHO_MOVE_FINGER_LEFT:
		DisplayText ("MOVE LEFT", 1);
		break;
		case MORPHO_MOVE_FINGER_RIGHT:
		DisplayText ("MOVE RIGHT", 1);
		break;
		case MORPHO_PRESS_FINGER_HARDER:
		DisplayText ("Press Harder", 1);
		break;
		case MORPHO_FINGER_OK:
		DisplayText ("Acquisition ok", 1);
		break;
		default:
		fprintf (stdout, "Async message %d\n", *(PI) i_pv_param);
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
							"Place Right Forefinger -> Capture %d/%d",
							l_puc_EnrollmentCmd[2], l_puc_EnrollmentCmd[3]);
						DisplayText (l_c_String, 2);
					break;
				
					case 2:
						sprintf (l_c_String,
							"Right Forefinger Again -> Capture %d/%d",
							l_puc_EnrollmentCmd[2], l_puc_EnrollmentCmd[3]);
						DisplayText (l_c_String, 2);
					break;
				
					case 3:
						sprintf (l_c_String,
							"Right Forefinger Again -> Capture %d/%d",
							l_puc_EnrollmentCmd[2], l_puc_EnrollmentCmd[3]);
						DisplayText (l_c_String, 2);
					break;
				}
			break;
	
			case 2:
				switch (l_puc_EnrollmentCmd[2])
				{
					case 1:
						sprintf (l_c_String,
							"Place Left Forefinger -> Capture %d/%d",
							l_puc_EnrollmentCmd[2], l_puc_EnrollmentCmd[3]);
						DisplayText (l_c_String, 2);
					break;
		
				case 2:
						sprintf (l_c_String,
							"Left Forefinger Again -> Capture %d/%d",
							l_puc_EnrollmentCmd[2], l_puc_EnrollmentCmd[3]);
						DisplayText (l_c_String, 2);
					break;
		
				case 3:
					sprintf (l_c_String,
						"Left Forefinger Again -> Capture %d/%d",
						l_puc_EnrollmentCmd[2], l_puc_EnrollmentCmd[3]);
					DisplayText (l_c_String, 2);
				break;
				}
			break;
		}
	}

	if (i_i_command == MORPHO_CALLBACK_IMAGE_CMD)
	{
		memcpy (&l_x_ImageStructure.m_x_ImageHeader,
			(T_MORPHO_IMAGE_HEADER *) i_pv_param,
			sizeof (T_MORPHO_IMAGE_HEADER));
		l_x_ImageStructure.m_puc_Image =
		(PUC) i_pv_param + sizeof (T_MORPHO_IMAGE_HEADER);
	
		Display_Image (l_x_ImageStructure.m_puc_Image,
			l_x_ImageStructure.m_x_ImageHeader.m_us_NbRow,
			l_x_ImageStructure.m_x_ImageHeader.m_us_NbCol,
			l_x_ImageStructure.m_x_ImageHeader.m_uc_NbBitsPerPixel);
		SDL_PollEvent (&event);
		if (event.type == SDL_QUIT)
		MSO_Cancel (g_h_Mso100Handle);
	
	}

	if (i_i_command == MORPHO_CALLBACK_CODEQUALITY)
	{
		l_uc_quality = *(PUC) i_pv_param;
		sprintf (l_c_String, "Code Quality : %d", l_uc_quality);
		DisplayText (l_c_String, 3);
	}
	
	if (i_i_command == MORPHO_CALLBACK_DETECTQUALITY)
	{
		l_uc_quality = *(PUC) i_pv_param;
		sprintf (l_c_String, "Detect Quality : %d", l_uc_quality);
		DisplayText (l_c_String, 3);
	}

	return 0;
}


/*****************************************************************************/
/*****************************************************************************/
I
SaveBufInFile (PT_BUF i_px_Buf, PC i_pc_String, PC i_pc_Ext)
{
	FILE *fp;
	C l_ac_String[256];
	I l_i_Ret;
	
	fprintf (stdout, "%s", i_pc_String);
	fprintf (stdout, "Enter File name : -> ");
	fgets (l_ac_String, 256, stdin);
	l_ac_String[strlen (l_ac_String) - 1] = 0;	// Suppress '\n' 
	if (strstr (l_ac_String, i_pc_Ext) == NULL)
		strcat (l_ac_String, i_pc_Ext);
	
	fp = fopen (l_ac_String, "wb");
	if (fp != NULL)
	{
		l_i_Ret = fwrite (i_px_Buf->m_puc_Buf, 1, i_px_Buf->m_ul_Size, fp);
		fclose (fp);
	
		fprintf (stdout, "--> Save %d bytes in %s\n", l_i_Ret, l_ac_String);
	}
	else
	{
		fprintf (stdout, "Can't open file %s for writing\n", l_ac_String);
	}

	return 0;
}

/*****************************************************************************/
/*****************************************************************************/
I OpenPkFile (PT_BUF_PK i_px_Buf_Pk, PC i_pc_String)
{
	FILE *fp;
	C l_ac_String[256];
	I l_i_Ret;
	
	fprintf (stdout, "---Open PkComp V2 file: %s -----\n", i_pc_String);
	fprintf (stdout, "Enter File name (*.pkc): -> ");
	fgets (l_ac_String, 256, stdin);
	l_ac_String[strlen (l_ac_String) - 1] = 0;	// Suppress '\n' 
	
	fp = fopen (l_ac_String, "rb");
	if (fp != NULL)
	{
		i_px_Buf_Pk->m_uc_IdPk = ID_PKCOMP;
		i_px_Buf_Pk->m_uc_Index = 0;
		l_i_Ret = fread (i_px_Buf_Pk->m_x_Buf.m_puc_Buf, 1, 512, fp);
		i_px_Buf_Pk->m_x_Buf.m_ul_Size = (UL) l_i_Ret;
		fclose (fp);
	
		fprintf (stdout, "--> Read %d bytes in %s\n", l_i_Ret, l_ac_String);
	}
	else
	{
		l_i_Ret = -1;
		fprintf (stdout, "Can't open file %s for reading\n", l_ac_String);
	}
	
	return l_i_Ret;
}

/*****************************************************************************/
/*****************************************************************************/
I OpenImgFile (PUC i_puc_Img, I i_i_SizeBuf)
{
	FILE *fp;
	C l_ac_String[256];
	I l_i_Ret;
	
	fprintf (stdout, "---Open Image Raw file:  -----\n");
	fprintf (stdout, "Enter File name: -> ");
	fgets (l_ac_String, 256, stdin);
	l_ac_String[strlen (l_ac_String) - 1] = 0;	// Suppress '\n' 
	
	fp = fopen (l_ac_String, "rb");
	if (fp != NULL)
	{
		l_i_Ret = fread (i_puc_Img, 1, i_i_SizeBuf, fp);
		fclose (fp);
	
		fprintf (stdout, "--> Read %d bytes in %s\n", l_i_Ret, l_ac_String);
	}
	else
	{
		l_i_Ret = -1;
		fprintf (stdout, "Can't open file %s for reading\n", l_ac_String);
	}
	
	return l_i_Ret;
}

/*****************************************************************************/
/*****************************************************************************/
I CommandMenu ()
{
	C l_ac_string[32];
	I l_i_i = 0;
	
	fprintf (stdout, "-------------------- MSO ILV Command ----------------\n");
	fprintf (stdout, "\tDestroy Base index 0                         --->(1)\n");
	fprintf (stdout, "\tCreate Base                                  --->(2)\n");
	fprintf (stdout, "\tErase all user in Base                       --->(3)\n");
	fprintf (stdout, "\tErase a User                                 --->(4)\n");
	fprintf (stdout, "\tGet Base config                              --->(5)\n");
	fprintf (stdout, "\tEnroll                                       --->(6)\n");
	fprintf (stdout, "\tidentify                                     --->(7)\n");
	fprintf (stdout, "\tVerify                                       --->(8)\n");
	fprintf (stdout, "\tIdentify Match                               --->(9)\n");
	fprintf (stdout, "\tVerify Match                                 --->(10)\n");
	fprintf (stdout, "\tList User in Database                        --->(11)\n");
	fprintf (stdout, "\tAdd Base Record                              --->(12)\n");
	fprintf (stdout, "\tDisplay raw image                            --->(13)\n");
	fprintf (stdout, "\tQuit                                         --->(0)\n");
	
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
			if (l_i_i >= 0 && l_i_i <= 13)
			break;
		}
		fprintf (stdout, "Select -> ");
	}

	return l_i_i;
}

/*****************************************************************************/
/*****************************************************************************/
int main (int argc, char *argv[])
{
	I l_i_Ret;
	I l_i_i;
	UC l_uc_ConnectionType;
	I l_i_speed;
	C l_ac_Device[32];
	int l_i_IdProduct;
	int option;
	char *liste_options = "c:s:d:t:e:x:p:i:h";
	UC l_uc_loop;
	
	US l_us_Timeout;
	UL l_ul_AsynchronousEvnt;
	
	T_EXPORT_PK l_x_ExportPk;
	PT_EXPORT_PK l_px_ExportPk;
	T_EXPORT_IMAGE l_x_ExportImage;
	PT_EXPORT_IMAGE l_px_ExportImage;
	T_BUF_PK l_x_Buf_Pk[2];
	
	PUC l_puc_ImgBuf;
	T_MORPHO_IMAGE_HEADER l_x_ImgHdr;


	// Initialisation
	g_uc_IsVideoInit = 0;
	g_uc_IsSDL_ttfInit = 0;
	l_uc_ConnectionType = 0;
	g_h_Mso100Handle = NULL;
	l_i_speed = DEFAULT_BAUD_RATE;
	sprintf (l_ac_Device, "%s", DEFAULT_SERIAL_PORT);
	
	g_uc_ExportScore = 1;
	l_us_Timeout = 0;
	l_ul_AsynchronousEvnt = MORPHO_CALLBACK_DETECTQUALITY |
		MORPHO_CALLBACK_CODEQUALITY |
		MORPHO_CALLBACK_IMAGE_CMD |
		MORPHO_CALLBACK_COMMAND_CMD | MORPHO_CALLBACK_ENROLLMENT_CMD;
	l_px_ExportPk = NULL;
	l_px_ExportImage = NULL;
	
	show_banner ();
	// Get Options
	while ((option = getopt (argc, argv, liste_options)) != -1)
	{
		switch (option)
		{
			case 'c':
				if (strcmp (optarg, "RS232") == 0)
				{
					fprintf (stdout, "Use Serial Port\n");
					l_uc_ConnectionType = 1;
				}
				else if (strcmp (optarg, "USB") == 0)
				{
					fprintf (stdout, "Use USB bus\n");
					l_uc_ConnectionType = 0;
				}
			break;
			case 's':
				sscanf (optarg, "%d", &l_i_speed);
				break;
			case 'd':
				sscanf (optarg, "%s", l_ac_Device);
				break;
			case 't':
				sscanf (optarg, "%hu", &l_us_Timeout);
				fprintf (stdout, "Set Timeout to %d\n", l_us_Timeout);
				break;
			case 'e':
				if (strcmp (optarg, "EVENT_ON") == 0)
				{
					fprintf (stdout, "Asynchronous event ON\n");
				}
				else if (strcmp (optarg, "EVENT_OFF") == 0)
				{
					fprintf (stdout, "Asynchronous event OFF\n");
					l_ul_AsynchronousEvnt = 0;
				}
				break;
			case 'x':
				if (strcmp (optarg, "SCORE_ON") == 0)
				{
					fprintf (stdout, "Export Score on\n");
					g_uc_ExportScore = 1;
				}
				else if (strcmp (optarg, "SCORE_OFF") == 0)
				{
					fprintf (stdout, "Export Score off\n");
					g_uc_ExportScore = 0;
				}
				break;
			case 'p':
				if (strcmp (optarg, "EXPORT_PK_ON") == 0)
				{
					fprintf (stdout, "Export Pk on\n");
					l_px_ExportPk = &l_x_ExportPk;
				}
				else if (strcmp (optarg, "EXPORT_PK_OFF") == 0)
				{
					fprintf (stdout, "Export Pk off\n");
					l_px_ExportPk = NULL;
				}
				break;
			case 'i':
				if (strcmp (optarg, "EXPORT_IMG_ON") == 0)
				{
					fprintf (stdout, "Export Image on\n");
					l_px_ExportImage = &l_x_ExportImage;
				}
				else if (strcmp (optarg, "EXPORT_IMG_OFF") == 0)
				{
					fprintf (stdout, "Export Image off\n");
					l_px_ExportImage = NULL;
				}
				break;
			case 'h':
				usage ();
				return 0;
			case '?':
				fprintf (stdout, "Bad option\n");
				usage ();
				return 0;
		}
	}

	// Connection to MSO 
	l_i_Ret = MSO_Connect( &g_h_Mso100Handle, 
							l_uc_ConnectionType, 
							l_ac_Device,
							l_i_speed, 
							&l_i_IdProduct);
	if ((l_i_Ret != 0) || (g_h_Mso100Handle == NULL))
	{
		fprintf (stdout, "Error Connection\n");
		return -1;
	}
	// Register Asynchronous event Callback Function
	l_i_Ret = MSO_RegisterAsyncILV( g_h_Mso100Handle, 
									l_ul_AsynchronousEvnt,
									(T_pFuncILV_Buffer) CommandCallback,
									NULL );

	// main loop
	l_uc_loop = 1;
	while (l_uc_loop)
	{
		switch ((l_i_i = CommandMenu ()))
		{
			default:
				fprintf (stdout, "unrecognize command: %d\n", l_i_i);
				break;

			case 0:
				l_uc_loop = 0;
				break;

			case 1:
				// Destroy base
				DestroyDatabase (g_h_Mso100Handle);
				break;
	
			case 2:
				// Create base
				CreateDataBase (g_h_Mso100Handle);
				break;
	
			case 3:
				// Erase all user in Base
				DeleteAllUser (g_h_Mso100Handle);
				break;
	
			case 4:
				// Delete User
				DeleteUserIndex (g_h_Mso100Handle);
				break;
	
			case 5:
				// Get BaseConfig
				GetBaseConfig (g_h_Mso100Handle);
				break;
	
			case 6:
				// Enroll
				if (l_px_ExportPk != NULL)
				{
					l_x_ExportPk.io_px_BiometricData = malloc (2 * sizeof (T_BUF));
					l_x_ExportPk.i_uc_ExportMinutiae = 1;
					for (l_i_i = 0; l_i_i < 2; l_i_i++)
					{
						l_x_ExportPk.io_px_BiometricData[l_i_i].m_ul_Size = 512;
						l_x_ExportPk.io_px_BiometricData[l_i_i].m_puc_Buf = malloc (512);
						memset (l_x_ExportPk.io_px_BiometricData[l_i_i].m_puc_Buf, 0, 512);
					}
				}
	
				if (l_px_ExportImage != NULL)
				{
					l_x_ExportImage.io_px_BufImage = malloc (2 * sizeof (T_BUF));
					l_x_ExportImage.i_uc_ExportImageType = 0;
					l_x_ExportImage.i_uc_CompressionType = ID_COMPRESSION_NULL;
					for (l_i_i = 0; l_i_i < 2; l_i_i++)
					{
						l_x_ExportImage.io_px_BufImage[l_i_i].m_ul_Size = 416 * 416 + 12;
						l_x_ExportImage.io_px_BufImage[l_i_i].m_puc_Buf =
							malloc (416 * 416 + 12);
						memset (l_x_ExportImage.io_px_BufImage[l_i_i].m_puc_Buf,
							0, 416 * 416 + 12);
					}
				}
	
				if (l_ul_AsynchronousEvnt > 0)
				{
					if (InitScreen () < 0)
					{
						MSO_UnregisterAllAsyncILV (g_h_Mso100Handle);
						l_ul_AsynchronousEvnt = 0;
					}
				}
	
				l_i_Ret = EnrollUser( 	g_h_Mso100Handle, 
										l_ul_AsynchronousEvnt,
										l_us_Timeout,
										l_px_ExportPk,
										l_px_ExportImage
									);
	
				if (l_ul_AsynchronousEvnt > 0)
					CloseScreen ();
	
				if (l_px_ExportPk != NULL)
				{
					if (l_i_Ret == 0)
					{
						SaveBufInFile (l_px_ExportPk->io_px_BiometricData,
							"\n-----Pk1: Save File in PkComp V2 format (*.pkc):-------\n",
							".pkc");
						SaveBufInFile (l_px_ExportPk->io_px_BiometricData + 1,
							"\n-----Pk2: Save File in PkComp V2 format (*.pkc):-------\n",
							".pkc");
					}
	
					for (l_i_i = 0; l_i_i < 2; l_i_i++)
					{
						if (l_px_ExportPk->io_px_BiometricData[l_i_i].m_puc_Buf != NULL)
							free (l_px_ExportPk->io_px_BiometricData[l_i_i].m_puc_Buf);
					}
					if (l_px_ExportPk->io_px_BiometricData != NULL)
						free (l_px_ExportPk->io_px_BiometricData);
				}
	
				if (l_px_ExportImage != NULL)
				{
					if (l_i_Ret == 0)
					{
						SaveBufInFile (l_px_ExportImage->io_px_BufImage,
								"\n----Finger 1: Save Image in Raw Format (*.raw)----\n",
								".raw");
						SaveBufInFile (l_px_ExportImage->io_px_BufImage + 1,
								"\n----Finger 2: Save Image in Raw Format (*.raw):----\n",
								".raw");
			
						memcpy ((PUC) & l_x_ImgHdr,
							l_x_ExportImage.io_px_BufImage->m_puc_Buf,
							sizeof (T_MORPHO_IMAGE_HEADER));
					fprintf (stdout, "\n---- Image Header Information ----\n");
					fprintf (stdout, "Header Revision:  %d\n",
						l_x_ImgHdr.m_uc_HeaderRevision);
					fprintf (stdout, "Header size:      %d\n",
						l_x_ImgHdr.m_uc_HeaderSize);
					fprintf (stdout, "Nb Row:           %d\n", l_x_ImgHdr.m_us_NbRow);
					fprintf (stdout, "Nb Col:           %d\n", l_x_ImgHdr.m_us_NbCol);
					fprintf (stdout, "Res X:            %d\n", l_x_ImgHdr.m_us_ResX);
					fprintf (stdout, "Res Y:            %d\n", l_x_ImgHdr.m_us_ResY);
					fprintf (stdout, "Compression Type: %d\n",
						l_x_ImgHdr.m_uc_CompressionType);
					fprintf (stdout, "Compression Param:%d\n",
						l_x_ImgHdr.m_uc_NbBitsPerPixel);
					}
	
					for (l_i_i = 0; l_i_i < 2; l_i_i++)
					{
						if (l_x_ExportImage.io_px_BufImage[l_i_i].m_puc_Buf != NULL)
							free (l_x_ExportImage.io_px_BufImage[l_i_i].m_puc_Buf);
					}
					if (l_x_ExportImage.io_px_BufImage != NULL)
						free (l_x_ExportImage.io_px_BufImage);
				}	
				break;
	
			case 7:
				// Identify
				if (l_ul_AsynchronousEvnt > 0)
				{
					if (InitScreen () < 0)
					{
						MSO_UnregisterAllAsyncILV (g_h_Mso100Handle);
						l_ul_AsynchronousEvnt = 0;
					}
				}
			
				Identify(g_h_Mso100Handle, l_ul_AsynchronousEvnt, l_us_Timeout);
				if (l_ul_AsynchronousEvnt > 0)
					CloseScreen ();
				break;
	
			case 8:
				// Verify
				for (l_i_i = 0; l_i_i < 2; l_i_i++)
				{
					l_x_Buf_Pk[l_i_i].m_x_Buf.m_ul_Size = 512;
					l_x_Buf_Pk[l_i_i].m_x_Buf.m_puc_Buf = malloc (512);
					memset (l_x_Buf_Pk[l_i_i].m_x_Buf.m_puc_Buf, 0, 512);
				}
		
				l_i_Ret = OpenPkFile ((PT_BUF_PK) & l_x_Buf_Pk[0], "Pk1 File");
				if (l_i_Ret < 0)
					break;
				l_i_Ret = OpenPkFile ((PT_BUF_PK) & l_x_Buf_Pk[1], "Pk2 File");
					if (l_i_Ret < 0)
				break;
	
				if (l_ul_AsynchronousEvnt > 0)
				{
					if (InitScreen () < 0)
					{
						MSO_UnregisterAllAsyncILV (g_h_Mso100Handle);
						l_ul_AsynchronousEvnt = 0;
					}
				}
	
				Verify( g_h_Mso100Handle,
						(PT_BUF_PK) & l_x_Buf_Pk,
						2,
						l_ul_AsynchronousEvnt,
						l_us_Timeout );
	
				if (l_ul_AsynchronousEvnt > 0)
					CloseScreen ();
	
				for (l_i_i = 0; l_i_i < 2; l_i_i++)
				{
					if (l_x_Buf_Pk[l_i_i].m_x_Buf.m_puc_Buf != NULL)
					free (l_x_Buf_Pk[l_i_i].m_x_Buf.m_puc_Buf);
				}	
				break;
	
			case 9:
				// Identify Match
				l_x_Buf_Pk[0].m_x_Buf.m_ul_Size = 512;
				l_x_Buf_Pk[0].m_x_Buf.m_puc_Buf = malloc (512);
				memset (l_x_Buf_Pk[0].m_x_Buf.m_puc_Buf, 0, 512);
			
				l_i_Ret = OpenPkFile ((PT_BUF_PK) & l_x_Buf_Pk[0], "Pk1 file");
				if (l_i_Ret < 0)
				break;
			
				IdentifyMatch(g_h_Mso100Handle, (PT_BUF_PK) & l_x_Buf_Pk);
			
				if (l_x_Buf_Pk[0].m_x_Buf.m_puc_Buf != NULL)
					free (l_x_Buf_Pk[0].m_x_Buf.m_puc_Buf);
			
				break;
			
			case 10:
				for (l_i_i = 0; l_i_i < 2; l_i_i++)
				{
					l_x_Buf_Pk[l_i_i].m_x_Buf.m_ul_Size = 512;
					l_x_Buf_Pk[l_i_i].m_x_Buf.m_puc_Buf = malloc (512);
					memset (l_x_Buf_Pk[l_i_i].m_x_Buf.m_puc_Buf, 0, 512);
				}
			
				l_i_Ret = OpenPkFile ((PT_BUF_PK) & l_x_Buf_Pk[0], "Pk Source file");
				if (l_i_Ret < 0)
					break;
				l_i_Ret = OpenPkFile ((PT_BUF_PK) & l_x_Buf_Pk[1], "Pk Ref file");
				if (l_i_Ret < 0)
					break;
			
				VerifyMatch(g_h_Mso100Handle, 
							(PT_BUF_PK) & l_x_Buf_Pk[0],
							(PT_BUF_PK) & l_x_Buf_Pk[1],
							1);
	
				for (l_i_i = 0; l_i_i < 2; l_i_i++)
				{
					if (l_x_Buf_Pk[l_i_i].m_x_Buf.m_puc_Buf != NULL)
						free (l_x_Buf_Pk[l_i_i].m_x_Buf.m_puc_Buf);
				}			
				break;
	
			case 11:
				ListDataBaseUser (g_h_Mso100Handle);
				break;
		
			case 12:
				for (l_i_i = 0; l_i_i < 2; l_i_i++)
				{
					l_x_Buf_Pk[l_i_i].m_x_Buf.m_ul_Size = 512;
					l_x_Buf_Pk[l_i_i].m_x_Buf.m_puc_Buf = malloc (512);
					memset (l_x_Buf_Pk[l_i_i].m_x_Buf.m_puc_Buf, 0, 512);
				}
			
				l_i_Ret = OpenPkFile ((PT_BUF_PK) & l_x_Buf_Pk[0], "Add Record--> pk1:");
				if (l_i_Ret < 0)
					break;
				l_i_Ret = OpenPkFile ((PT_BUF_PK) & l_x_Buf_Pk[1], "Add Record--> pk2:");
				if (l_i_Ret < 0)
					break;
				AddBaseRecord (g_h_Mso100Handle, (PT_BUF_PK) & l_x_Buf_Pk, 2);
				break;
			
			case 13:
				// Display raw Image ( Obtained with enroll ILV export image activated)
				l_puc_ImgBuf = malloc (416 * 416 + 12);
				l_i_Ret = OpenImgFile (l_puc_ImgBuf, 416 * 416 + 12);
				if (l_i_Ret <= 0)
				{
					if (l_puc_ImgBuf != NULL)
						free (l_puc_ImgBuf);
					break;
				}
	
				memcpy ((PUC) & l_x_ImgHdr, l_puc_ImgBuf, sizeof (T_MORPHO_IMAGE_HEADER));
				fprintf (stdout, "\n---- Image Header Information ----\n");
				fprintf (stdout, "Header Revision:  %d\n", l_x_ImgHdr.m_uc_HeaderRevision);
				fprintf (stdout, "Header size:      %d\n", l_x_ImgHdr.m_uc_HeaderSize);
				fprintf (stdout, "Nb Row:           %d\n", l_x_ImgHdr.m_us_NbRow);
				fprintf (stdout, "Nb Col:           %d\n", l_x_ImgHdr.m_us_NbCol);
				fprintf (stdout, "Res X:            %d\n", l_x_ImgHdr.m_us_ResX);
				fprintf (stdout, "Res Y:            %d\n", l_x_ImgHdr.m_us_ResY);
				fprintf (stdout, "Compression Type: %d\n", l_x_ImgHdr.m_uc_CompressionType);
				fprintf (stdout, "Compression Param:%d\n",l_x_ImgHdr.m_uc_NbBitsPerPixel);
	
				if (l_i_Ret != (I) ((l_x_ImgHdr.m_us_NbRow * l_x_ImgHdr.m_us_NbCol) + sizeof (T_MORPHO_IMAGE_HEADER)))
				{
					fprintf (stdout, "\nInvalid Image !\n");
					if (l_puc_ImgBuf != NULL)
						free (l_puc_ImgBuf);
					break;
				}
			
				if (InitScreen () < 0)
					break;
		
				l_i_i = 1;
				while (l_i_i)
				{
					Display_Image (l_puc_ImgBuf + sizeof (T_MORPHO_IMAGE_HEADER),
							l_x_ImgHdr.m_us_NbRow, l_x_ImgHdr.m_us_NbCol, 8);
					while (SDL_PollEvent (&event))
					{
						switch (event.type)
						{
							case SDL_KEYDOWN:
								if (event.key.keysym.sym == SDLK_q)
								l_i_i = 0;
								break;
							case SDL_QUIT:
								l_i_i = 0;
								break;
						}
					}
				}
	
				if (l_puc_ImgBuf != NULL)
					free (l_puc_ImgBuf);
				CloseScreen ();
				break;

		}//swhitch command menu
	}//while main loop

	// Disconnect to MSO
	l_i_Ret = MSO_CloseCom (&g_h_Mso100Handle);
	if (l_i_Ret != RETURN_NO_ERROR)
	{
		fprintf (stdout, "Error MSO_CloseCom\n");
	}

	return 0;
}
