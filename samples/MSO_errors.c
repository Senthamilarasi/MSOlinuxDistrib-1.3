#include <stdio.h>
#include "libMSO.h"


// Error table struct
typedef struct
{
	UC	m_uc_ErrorCode;
	UC  m_auc_ErrorString[256];
} T_ERROR_TABLE;

// Error Table with string explanation
T_ERROR_TABLE g_x_ErrorTable[] =
		{ 
			{ ILV_OK,						"Successful result "},								 		// 0x00
			{ ILVERR_ERROR,					"An error occurred" },										// 0xFF
			{ ILVERR_BADPARAMETER,			"Input parameters are not valid" }, 						// 0xFE
			{ ILVERR_INVALID_MINUTIAE,		"The minutiae is not valid" },      				 		// 0xFD
			{ ILVERR_INVALID_USER_ID,		"The record identifier does not exist in the database" },	// 0xFC
			{ ILVERR_INVALID_USER_DATA,		"The user data are not valid" },    			 			// 0xFB 
			{ ILVERR_TIMEOUT,				"No response after defined time" }, 						// 0xFA 
			{ ILVERR_INVALID_ID_PROTOCOL,	"The protocole used is not valid" },						// 0xF9
			{ ILVERR_ALREADY_ENROLLED,		"The person is already in the base"},						// 0xF8
			{ ILVERR_BASE_NOT_FOUND,		"The specified base does not exist"},   					// 0xF7
			{ ILVERR_BASE_ALREADY_EXISTS,	"The specified base already exist"},    					// 0xF6
			{ ILVERR_BIO_IN_PROGRESS,		"Command received during biometric processing"},			// 0xF5
			{ ILVERR_CMD_INPROGRESS,		"Command received while another command is running"},		// 0xF4
			{ ILVERR_FLASH_INVALID,			"Flash type invalid" },										// 0xF3
			{ ILVERR_NO_SPACE_LEFT,			"Not Enough memory for the creation of a database" },		// 0xF2
			{ ILVERR_FIELD_NOT_FOUND, 		"Field does not exist"  },   								// 0xE9
			{ ILVERR_FIELD_INVALID,  		"Field size or field name is invalid"  },					// 0xE8
			{ ILVERR_SECURITY_MODE,  		"The request is not compatible with security mode" },		// 0xE7
			{ ILVERR_USER_NOT_FOUND, 		"The searched user is not found" },							// 0xE6
			{ ILVERR_CMDE_ABORTED,   		"Commanded has been aborted by the user" },					// 0xE5
			{ ILVERR_SAME_FINGER,   		"There are two templates of the same finger" },				// 0xE4
			{ ILVERR_NO_HIT,				"Presented finger does not match" },						// 0xE3
			{ ILVERR_FFD,					"False finger detected"},									// 0xDB 
			{ ILVERR_MOIST_FINGER,			"Too moist finger detected" },								// 0xDA
		};

UL	g_ul_NbError = sizeof(g_x_ErrorTable)/sizeof(T_ERROR_TABLE);

/*****************************************************************************/
/*****************************************************************************/
int ConvertError(UC i_uc_Status)
{
	int l_i_i;

	for(l_i_i=0; l_i_i<g_ul_NbError; l_i_i++)
	{
		if(g_x_ErrorTable[l_i_i].m_uc_ErrorCode == i_uc_Status)
		{
			fprintf(stderr, "Error: %s\n", g_x_ErrorTable[l_i_i].m_auc_ErrorString);
			break;
		}
	}

	if( l_i_i == g_ul_NbError)
	{
		fprintf(stderr, "Error 0x%02X: Unknown error \n", i_uc_Status);
		return -1;
	}

	return 0;
}
