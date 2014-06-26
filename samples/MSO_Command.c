#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libMSO.h"
#include "MSO_TestBio.h"
#include "Ilv_definitions.h"


extern UC	g_uc_ExportScore;


// Database function
/***************************************************************************/
/**
* Delete user of index i_ul_IndexUser in database of index 0 
*
*/
int DeleteUserIndex(MORPHO_HANDLE i_h_Mso100Handle)
{
    I   l_i_Ret;
    UC	l_uc_Status;
    UL  l_ul_EmbeddedError;
    UL	l_ul_IndexUser;
    C	l_ac_String[32];

    fprintf(stdout, "Enter Index of User to delete:\n->");
    fgets(l_ac_String, 32, stdin);
    sscanf(l_ac_String, "%ld", &l_ul_IndexUser);

    l_i_Ret = MSO_BioDB_DeleteUser( i_h_Mso100Handle,
                                    0,                   //UC              i_uc_IndexDB,
                                    l_ul_IndexUser,      //UL              i_ul_IndexUser,
                                    &l_uc_Status,        //PUC             o_puc_ILV_Status,
                                    &l_ul_EmbeddedError  //PUL             o_pul_EmbeddedError
                                  );
    if(l_i_Ret==0 && l_uc_Status==0 && l_ul_EmbeddedError==0)
    {
        fprintf(stdout, "Delete user with index %ld in Base index 0 -----> OK\n", l_ul_IndexUser);
	l_i_Ret = 0;
    }
    else
    {
        fprintf(stdout, "MSO_BioDB_DeleteUser ret=%d status=0x%02X EmbeddedError=%ld\n",
                                                       l_i_Ret, l_uc_Status, l_ul_EmbeddedError);
        if(l_uc_Status != 0)
            ConvertError(l_uc_Status);
	l_i_Ret = -1;
    }

    return l_i_Ret;
}


/***************************************************************************/
/**
* Delete all user in database of index 0 
*
*/
int DeleteAllUser(MORPHO_HANDLE i_h_Mso100Handle)
{
    I   l_i_Ret;
    UC	l_uc_Status;
    UL  l_ul_EmbeddedError;

    l_i_Ret = MSO_BioDB_EraseDb( i_h_Mso100Handle,
                                 0,                         //UC   i_uc_IndexDB,
                                 &l_uc_Status,              //PUC  o_puc_ILV_Status,
                                 &l_ul_EmbeddedError        //PUL  o_pul_EmbeddedError
                               );
    if(l_i_Ret==0 && l_uc_Status==0 && l_ul_EmbeddedError==0)
    {
        fprintf(stdout, "Erase all user in Base index 0 -----> OK\n");
	l_i_Ret = 0;
    }
    else
    {
        fprintf(stdout, "MSO_BioDB_EraseDb ret=%d status=0x%02X EmbeddedError=%ld\n",
                                                      l_i_Ret, l_uc_Status, l_ul_EmbeddedError);
        if(l_uc_Status != 0)
            ConvertError(l_uc_Status);
	l_i_Ret = -1;
    }

    return l_i_Ret;
}

/***************************************************************************/
/**
* Destroy database of index 0 
*
*/
int DestroyDatabase(MORPHO_HANDLE i_h_Mso100Handle)
{
    I   l_i_Ret;
    UC	l_uc_Status;
    UL  l_ul_EmbeddedError;
    
    l_i_Ret = MSO_BioDB_DestroyDb( i_h_Mso100Handle,
                                   0,
                                   &l_uc_Status,              //PUC  o_puc_ILV_Status,
                                   &l_ul_EmbeddedError        //PUL  o_pul_EmbeddedError
                                 );
    if(l_i_Ret == 0 && l_uc_Status==0 && l_ul_EmbeddedError ==0)
    {
        fprintf(stdout, "Destroy Base index 0 -----> OK\n");
	l_i_Ret = 0;
    }
    else
    {
        fprintf(stdout, "MSO_BioDB_DestroyDb ret=%d status=0x%02X EmbeddedError=%ld\n",
                                                        l_i_Ret, l_uc_Status, l_ul_EmbeddedError);
        if(l_uc_Status != 0)
            ConvertError(l_uc_Status);
	l_i_Ret = -1;
    }

    return l_i_Ret;
}

/***************************************************************************/
/**
* Create database of index 0 
* - 2 Additionnal fields
* - Max User 100
* - 2 fingers per user
* - Standard PK
*/
int CreateDataBase(MORPHO_HANDLE i_h_Mso100Handle)
{
    I  		l_i_Ret;
    UC		l_uc_Status;
    T_FIELD     l_x_BaseAddField[2];

    memset((PC)&l_x_BaseAddField, 0, sizeof(l_x_BaseAddField));
    // Add Field 1
    l_x_BaseAddField[0].m_us_FieldMaxSize = 15;
    sprintf((PC)l_x_BaseAddField[0].m_auc_FieldName, "Field1");
    l_x_BaseAddField[0].m_uc_Right = PUBLIC;

    // Add Field 2
    l_x_BaseAddField[1].m_us_FieldMaxSize = 15;
    sprintf((PC)l_x_BaseAddField[1].m_auc_FieldName, "Field2");
    l_x_BaseAddField[1].m_uc_Right = PUBLIC;

    l_i_Ret = MSO_BioDB_CreateDb( i_h_Mso100Handle,
                                  0,                             //UC       i_uc_IndexDB,
                                  100,                           //US       i_us_UserMax,
                                  2,                             //UC       i_uc_NbFinger,
                                  0,                             //UC       i_uc_NormalizedPK_Type,
                                  2,                             //UC       i_uc_NbAddField,
                                  (PT_FIELD)&l_x_BaseAddField,   //PT_FIELD i_px_AddField,
                                  &l_uc_Status                   //PUC      o_puc_ILV_Status
                                );
    if(l_i_Ret == 0 && l_uc_Status==0)
    {
        fprintf(stdout, "Create Base index 0 -----> OK\n");
    }
    else
    {
        fprintf(stdout, "MSO_BioDB_CreateDb ret=%d status=0x%02X\n", l_i_Ret, l_uc_Status);
        if(l_uc_Status != 0)
            ConvertError(l_uc_Status);
        l_i_Ret = -1;
    }

    return l_i_Ret;
}

/***************************************************************************/
/**
* Get Base Configuration
*
*/
int GetBaseConfig(MORPHO_HANDLE i_h_Mso100Handle)
{
    I  		l_i_Ret;
    UC		l_uc_Status;
    UL  	l_ul_EmbeddedError;
    I		l_i_i;
    T_FIELD     l_x_BaseAddField[2];
    UL 		l_ul_AddFieldNb;
    UC		l_uc_FingerNb;
    UL		l_ul_CurrentRecord;
    UC		l_uc_NormalizedPK_Type;
    UL		l_ul_MaxRecord;

    memset((PC)&l_x_BaseAddField, 0, sizeof(l_x_BaseAddField));
    l_ul_AddFieldNb = 2;
    l_uc_FingerNb = 0;
    l_ul_CurrentRecord = 0;
    l_i_Ret = MSO_BioDB_GetBaseConfig( i_h_Mso100Handle,
                                       0,                           //UC              i_uc_IndexDB,
                                       &l_uc_FingerNb,              //PUC             o_puc_FingerNb,
                                       &l_ul_MaxRecord,             //PUL             o_pul_MaxRecord,
                                       &l_ul_CurrentRecord,         //PUL             o_pul_CurrentRecord,
                                       &l_ul_AddFieldNb,    	    //PUL             io_pul_AddFieldNb,
                                       (PT_FIELD)&l_x_BaseAddField, //PT_FIELD        o_px_AddField,
                                       &l_uc_NormalizedPK_Type,     //PUC             o_puc_NormalizedPK_Type,
                                       &l_uc_Status,                //PUC             o_puc_ILV_Status,
                                       &l_ul_EmbeddedError          //PUL             o_pul_EmbeddedError
                                     );
    if(l_i_Ret==0 && l_uc_Status==0 && l_ul_EmbeddedError==0)
    {
        fprintf(stdout, "Get Base Config -----> OK\n");
        fprintf(stdout, "\tNormalized PK:         %d\n", l_uc_NormalizedPK_Type);
        fprintf(stdout, "\tFinger Number:         %d\n", l_uc_FingerNb);
        fprintf(stdout, "\tMax record number:     %ld\n", l_ul_MaxRecord);
        fprintf(stdout, "\tCurrent Record number: %ld\n", l_ul_CurrentRecord);
        fprintf(stdout, "\tAdd field Number:      %ld\n", l_ul_AddFieldNb);
        for(l_i_i=0; l_i_i<l_ul_AddFieldNb; l_i_i++)
            fprintf(stdout, "\t-->%s\n", l_x_BaseAddField[l_i_i].m_auc_FieldName);
    }
    else
    {
        fprintf(stdout, "MSO_BioDB_GetBaseConfig ret=%d status=0x%02X EmbeddedError=%ld\n",
                                                        l_i_Ret, l_uc_Status, l_ul_EmbeddedError);
        if(l_uc_Status != 0)
            ConvertError(l_uc_Status);
        l_i_Ret = -1;
    }

    return l_i_Ret;
}

// Bio Function
/***************************************************************************/
/**
* Enroll User.
* Captures and enrolls live fingers
* - 1 User ID and 2 additionnal fields 
* - 2 fingers per user
* - 3 acquisitions per finger
* - Standard PK
*/
int EnrollUser( MORPHO_HANDLE   i_h_Mso100Handle, 
		UL		i_ul_AsynchronousEvnt,
		US		i_us_Timeout,
		PT_EXPORT_PK    io_px_ExportPk,      //can be NULL
                PT_EXPORT_IMAGE io_px_ExportImage    //can be NULL
		)
{
    I  		l_i_Ret;
    UC		l_uc_Status;
    T_BUF       l_x_addField[3];
    UC		l_uc_EnrollStatus;
    UL		l_ul_UserDBIndex;
    UC		l_auc_UserID[25];
    UC		l_auc_AddField1[25];
    UC		l_auc_AddField2[25];

    fprintf(stdout,"---Captures and enrolls live fingers---\n");

    fprintf(stdout, "Enter UserID:\n->");
    fgets((PC)l_auc_UserID, 25, stdin);
    l_auc_UserID[strlen((PC)l_auc_UserID)-1] = 0;		// Suppress '\n'

    fprintf(stdout, "Enter Additionnal field 1:\n->");
    fgets((PC)l_auc_AddField1, 25, stdin);
    l_auc_AddField1[strlen((PC)l_auc_AddField1)-1] = 0;	// Suppress '\n'

    fprintf(stdout, "Enter Additionnal field 2:\n->");
    fgets((PC)l_auc_AddField2, 25, stdin);
    l_auc_AddField2[strlen((PC)l_auc_AddField2)-1] = 0;	// Suppress '\n'

    l_x_addField[0].m_ul_Size =  strlen((PC)l_auc_UserID)+1;
    l_x_addField[0].m_puc_Buf = l_auc_UserID;

    l_x_addField[1].m_ul_Size = strlen((PC)l_auc_AddField1)+1;
    l_x_addField[1].m_puc_Buf = l_auc_AddField1;

    l_x_addField[2].m_ul_Size = strlen((PC)l_auc_AddField2)+1;
    l_x_addField[2].m_puc_Buf = l_auc_AddField2;

    l_i_Ret = MSO_Bio_Enroll( i_h_Mso100Handle,
                              0,                        //UC            i_uc_IndexDB,
                              i_us_Timeout,             //US            i_us_Timeout,
                              0,                        //UC            i_uc_EnrollmentType,
                              2,                        //UC            i_uc_NbFinger,
                              1,                        //UC            i_uc_SaveRecord,
                              0,                        //UC            i_uc_NormalizedPK_Type,
                              3,                        //UC            i_uc_NbAddField,
                              (PT_BUF)&l_x_addField,    //PT_BUF        i_px_AddField,
                              i_ul_AsynchronousEvnt,    //UL 		i_ul_AsynchronousEvnt,
                              io_px_ExportPk,     	//PT_EXPORT_PK  io_px_ExportPk,      //can be NULL
                              io_px_ExportImage,        //PT_EXPORT_IMAGE io_px_ExportImage, //can be NULL
                              &l_uc_EnrollStatus,       //PUC           o_puc_EnrollStatus,
                              &l_ul_UserDBIndex,        //PUL           o_pul_UserDBIndex,
                              &l_uc_Status              //PUC           o_puc_ILV_Status
                             );
    if(l_i_Ret==0 && l_uc_Status==0)
    {
        fprintf(stdout, "Enroll -----> OK\n");
        fprintf(stdout, "Enroll Status: %d\n", l_uc_EnrollStatus);
        fprintf(stdout, "User Database Index: %ld\n", l_ul_UserDBIndex);
    }
    else
    {
        fprintf(stdout, "MSO_Bio_Enroll ret=%d status=0x%02X EnrollStatus=%d\n",
                                               l_i_Ret, l_uc_Status, l_uc_EnrollStatus);
        if(l_uc_Status != 0)
            ConvertError(l_uc_Status);
        l_i_Ret = -1;
    }

    return l_i_Ret;
}

/***************************************************************************/
/**
* identify a user.
* Identifies a live finger against the local database 
*/
int Identify( MORPHO_HANDLE   i_h_Mso100Handle,
	      UL              i_ul_AsynchronousEvnt,
              US              i_us_Timeout)
{
    I  		l_i_Ret;
    I  		l_i_i;
    UC		l_uc_Status;
    UL		l_ul_UserDBIndex;
    UC		l_uc_MatchingResult;
    UL		l_ul_AddFieldNb;
    UL		l_ul_Score = 0;
    PUL		l_pul_Score;
    
    T_BUF       l_x_UserID;
    UC		l_auc_UserID[25];
    T_BUF       l_x_addField[2];
    UC		l_auc_AddField1[25];
    UC		l_auc_AddField2[25];

    fprintf(stdout,"---Identifies a live finger against the local database---\n");

    if(g_uc_ExportScore)
	l_pul_Score = &l_ul_Score;
    else
	l_pul_Score = NULL;

    memset(l_auc_UserID, 0, sizeof(l_auc_UserID));
    memset(l_auc_AddField1, 0, sizeof(l_auc_AddField1));
    memset(l_auc_AddField2, 0, sizeof(l_auc_AddField2));
    memset(&l_x_UserID, 0, sizeof(l_x_UserID));
    memset(&l_x_addField, 0, sizeof(l_x_addField));

    l_x_UserID.m_puc_Buf = l_auc_UserID;
    l_x_addField[0].m_puc_Buf = l_auc_AddField1;
    l_x_addField[1].m_puc_Buf = l_auc_AddField2;
    l_ul_AddFieldNb = 2;

    l_i_Ret = MSO_Bio_Identify( i_h_Mso100Handle,
                                0,                     //UC                      i_uc_UidDB,
                                i_us_Timeout,          //US                      i_us_Timeout,
                                5,                     //US                      i_us_MatchingTreshold,
                                i_ul_AsynchronousEvnt, //UL                      i_ul_AsynchronousEvent,
                                &l_uc_MatchingResult,  //PUC                     o_puc_MatchingResult,
                                &l_ul_UserDBIndex,     //PUL                     o_pul_UserDBIndex,
                                (PT_BUF)&l_x_UserID,   //PT_BUF                  o_px_UserID,
                                &l_ul_AddFieldNb,      //PUL                     io_pul_AddFieldNb,
                                (PT_BUF)&l_x_addField, //PT_BUF                  o_px_AddFieldValue,
                                l_pul_Score,           //PUL                     o_pul_score,
                                &l_uc_Status           //PUC                     o_puc_ILV_Status
                             );

    if(l_i_Ret==0 && l_uc_Status==0)
    {
        fprintf(stdout, "Identify -----> OK\n");
        fprintf(stdout, "MatchingResult=%d score=%ld\n", l_uc_MatchingResult, l_ul_Score);
        if(l_uc_MatchingResult == ILVSTS_HIT)
        {
            fprintf(stdout, "User Identified--> index: %ld\n", l_ul_UserDBIndex);
	    fprintf(stdout, "\tUser ID: %s\n", l_x_UserID.m_puc_Buf);
            for(l_i_i = 0; l_i_i < l_ul_AddFieldNb; l_i_i++)
                fprintf(stdout, "\tAdd Field %d: %s\n", l_i_i, l_x_addField[l_i_i].m_puc_Buf);
        }
        else
            fprintf(stdout, "User Not Identified !! \n");
    }
    else
    {
        fprintf(stdout, "MSO_Bio_Identify ret=%d status=0x%02X\n", l_i_Ret, l_uc_Status);
        if(l_uc_Status != 0)
            ConvertError(l_uc_Status);
        l_i_Ret = -1;
    }

    return l_i_Ret;
}

/***************************************************************************/
/**
* Verify function.
* Captures a live finger and checks if it matches with one of input Pks
*/
int Verify( MORPHO_HANDLE   i_h_Mso100Handle,
	    PT_BUF_PK       i_x_Buf_Pk,	    
	    UC		    i_uc_NbFinger,
	    UL              i_ul_AsynchronousEvnt,
            US              i_us_Timeout)
{
    I  		l_i_Ret;
    UC		l_uc_Status;
    UC		l_uc_MatchingResult;
    UL		l_ul_Score = 0;
    PUL		l_pul_Score = 0;
    
    fprintf(stdout,"---Captures a live finger and checks if it matches with one of input Pks---\n");

    if(g_uc_ExportScore)
	l_pul_Score = &l_ul_Score;
    else
	l_pul_Score = NULL;

    l_uc_MatchingResult = 0;
    l_ul_Score = 0;
    l_uc_Status = 0;

    l_i_Ret = MSO_Bio_Verify( i_h_Mso100Handle,
                              i_us_Timeout,           //US          i_us_Timeout,
                              5,                      //US          i_us_MatchingTreshold,
                              i_uc_NbFinger,          //UC          i_uc_NbFinger,
                              i_x_Buf_Pk, 	      //PT_BUF_PK   i_x_Buf_Pk
                              i_ul_AsynchronousEvnt,  //UL          i_ul_AsynchronousEvent,
                              &l_uc_MatchingResult,   //PUC         o_puc_MatchingResult,
                              l_pul_Score,            //PUL         o_pul_score,
                              &l_uc_Status            //PUC         o_puc_ILV_Status
                            );
    if(l_i_Ret==0 && l_uc_Status==0)
    {
        fprintf(stdout, "Verify -----> OK\n");
        fprintf(stdout, "MatchingResult=%d score=%ld\n", l_uc_MatchingResult, l_ul_Score);
        if(l_uc_MatchingResult == ILVSTS_HIT)
        {
            fprintf(stdout, "Verify-->  Verify OK\n");
        }
        else
            fprintf(stdout, "Verify-->  Verify NOT OK\n");
    }
    else
    {
        fprintf(stdout, "MSO_Bio_Verify ret=%d status=0x%02X\n", l_i_Ret, l_uc_Status);
        if(l_uc_Status != 0)
            ConvertError(l_uc_Status);
        l_i_Ret = -1;
    }

    return l_i_Ret;
}

/***************************************************************************/
/**
* Identify match function.
* Identifies biometric data sent against the local database  
*
*/
int IdentifyMatch( MORPHO_HANDLE   i_h_Mso100Handle,
	    	   PT_BUF_PK       i_px_Buf_Pk    
		 )
{

    I  		l_i_Ret;
    UC		l_uc_Status;
    UC		l_uc_MatchingResult;
    UL		l_ul_Score = 0;
    PUL		l_pul_Score;
    UL		l_ul_UserDBIndex;
    UC		l_auc_UserID[25];
    T_BUF       l_x_UserID;

    fprintf(stdout,"---Identifies biometric data sent against the local database---\n");

    if(g_uc_ExportScore)
	l_pul_Score = &l_ul_Score;
    else
	l_pul_Score = NULL;

    memset(&l_x_UserID, 0, sizeof(l_x_UserID));
    memset(l_auc_UserID, 0, sizeof(l_auc_UserID));
    l_x_UserID.m_puc_Buf = l_auc_UserID;

    l_i_Ret = MSO_Bio_IdentifyMatch( i_h_Mso100Handle,
                                     0,                         //UC        i_uc_UidDB,
                                     5,                         //US        i_us_MatchingTreshold,
                                     1,                 	//UC        i_uc_NbPk, (Must be set to 1)
                                     i_px_Buf_Pk,    		//PT_BUF_PK i_px_Pk,
                                     &l_uc_MatchingResult,      //PUC       o_puc_MatchingResult,
                                     &l_ul_UserDBIndex,         //PUL       o_pul_UserDBIndex,
                                     (PT_BUF)&l_x_UserID,       //PT_BUF    o_px_UserID,
                                     l_pul_Score,               //PUL       o_pul_score,
                                     &l_uc_Status               //PUC       o_puc_ILV_Status
                                   );
    if(l_i_Ret==0 && l_uc_Status==0)
    {
        fprintf(stdout, "Identify Match -----> OK\n");
        fprintf(stdout, "MatchingResult=%d score=%ld\n", l_uc_MatchingResult, l_ul_Score);
        if(l_uc_MatchingResult == ILVSTS_HIT)
            fprintf(stdout, "Identify Match--> name: %s index: %ld\n", l_x_UserID.m_puc_Buf, l_ul_UserDBIndex);
        else
            fprintf(stdout, "Identify Match KO: Not Identified !! \n");
    }
    else
    {
        fprintf(stdout, "MSO_Bio_Identify Match ret=%d status=0x%02X\n", l_i_Ret, l_uc_Status);
        if(l_uc_Status != 0)
            ConvertError(l_uc_Status);
        l_i_Ret = -1;
    }

    return l_i_Ret;
}

/***************************************************************************/
/*
* Verify Match function.
* Verifies if the input PkSrc matches against list of input PkRef
*/
int VerifyMatch( MORPHO_HANDLE   i_h_Mso100Handle,
	    	 PT_BUF_PK       i_px_Buf_PkSrc,	    
	    	 PT_BUF_PK       i_px_Buf_PkRef,	    
		 UC		 i_uc_NbPkRef
	       )
{
    I  		l_i_Ret;
    UC		l_uc_Status;
    UC		l_uc_MatchingResult;
    UL		l_ul_Score = 0;
    PUL		l_pul_Score;
    UC		l_uc_ListRefIndex;

    fprintf(stdout,"---Verifies if the input PkSrc matches against list of input PkRef---\n");

    if(g_uc_ExportScore)
	l_pul_Score = &l_ul_Score;
    else
	l_pul_Score = NULL;

    l_i_Ret = MSO_Bio_VerifyMatch( i_h_Mso100Handle,
                                   1,                       //UC        i_uc_NbPkSrc (must be set to 1)
                                   i_px_Buf_PkSrc,          //PT_BUF_PK i_px_PkSrc
                                   i_uc_NbPkRef,            //UC        i_uc_NbPkRef
                                   i_px_Buf_PkRef,  	    //PT_BUF_PK i_px_PkRef
                                   5,                       //US        i_us_MatchingTreshold,
                                   &l_uc_MatchingResult,    //PUC       o_puc_MatchingResult,
                                   &l_uc_ListRefIndex,      //PUC       o_puc_ListRefIndex,
                                   l_pul_Score,             //PUL       o_pul_score,
                                   &l_uc_Status             //PUC       o_puc_ILV_Status
                                 );

    if(l_i_Ret==0 && l_uc_Status==0)
    {
        fprintf(stdout, "Verify Match -----> OK\n");
        fprintf(stdout, "MatchingResult=%d score=%ld\n", l_uc_MatchingResult, l_ul_Score);
        if(l_uc_MatchingResult == ILVSTS_HIT)
        {
            fprintf(stdout, "Verify Match OK--> index: %d\n", l_uc_ListRefIndex);
        }
        else
            fprintf(stdout, "Verify Match KO: Not Identified !! \n");
    }
    else
    {
        fprintf(stdout, "MSO_Bio_VerifyMatch ret=%d status=0x%02X\n", l_i_Ret, l_uc_Status);
        if(l_uc_Status != 0)
            ConvertError(l_uc_Status);
        l_i_Ret = -1;
    }

    return l_i_Ret;
}

/***************************************************************************/
/*
*/
int ListDataBaseUser(MORPHO_HANDLE   i_h_Mso100Handle)
{
    I				   l_i_Ret;
    I				   l_i_i;
    UC				   l_uc_Status;
    UL				   ul_NbTransport; 
    PT_TRANSPORT_PUBLIC_LIST_FIELD l_px_TransportPublicField;

    UC 				   l_uc_FingerNb;
    UC 				   l_uc_NormalizedPK_Type;
    UL 				   l_ul_MaxRecord;
    UL 				   l_ul_CurrentRecord;
    UL 				   l_ul_AddFieldNb;
    T_FIELD        		   l_x_BaseAddField[2];
    UL				   l_ul_EmbeddedError;

    // GetBaseConfig to get current number user in database
    memset((PC)&l_x_BaseAddField, 0, sizeof(l_x_BaseAddField));
    l_ul_AddFieldNb = 2;
    l_i_Ret = MSO_BioDB_GetBaseConfig( i_h_Mso100Handle,
                                       0,                           //UC              i_uc_IndexDB,
                                       &l_uc_FingerNb,              //PUC             o_puc_FingerNb,
                                       &l_ul_MaxRecord,             //PUL             o_pul_MaxRecord,
                                       &l_ul_CurrentRecord,         //PUL             o_pul_CurrentRecord,
                                       &l_ul_AddFieldNb,            //PUL             io_pul_AddFieldNb,
                                       (PT_FIELD)&l_x_BaseAddField, //PT_FIELD        o_px_AddField,
                                       &l_uc_NormalizedPK_Type,     //PUC             o_puc_NormalizedPK_Type,
                                       &l_uc_Status,                //PUC             o_puc_ILV_Status,
                                       &l_ul_EmbeddedError          //PUL             o_pul_EmbeddedError
                                     );
    if(l_i_Ret==0 && l_uc_Status==0)
    {
        fprintf(stdout, "Current User Number in Database:%ld\n", l_ul_CurrentRecord);
    }
    else
    {
        fprintf(stdout, "MSO_BioDB_GetBaseConfig ret=%d status=0x%02X\n", l_i_Ret, l_uc_Status);
        if(l_uc_Status != 0)
            ConvertError(l_uc_Status);
        return -1;
    }


    ul_NbTransport = l_ul_CurrentRecord;	// Must be >= to number of current record in database.
			        	        // Retrieve Current number with GetBaseConfig
    l_px_TransportPublicField = malloc(sizeof(T_TRANSPORT_PUBLIC_LIST_FIELD)*ul_NbTransport); 
    if(l_px_TransportPublicField == NULL)
    {
        perror("error malloc");
        return -1;
    }
    memset(l_px_TransportPublicField, 0, sizeof(T_TRANSPORT_PUBLIC_LIST_FIELD)*ul_NbTransport);

    for(l_i_i=0; l_i_i<ul_NbTransport; l_i_i++)
    {
        l_px_TransportPublicField[l_i_i].m_puc_Data = malloc(128);
        if(l_px_TransportPublicField[l_i_i].m_puc_Data == NULL)
	{
            perror("error malloc");
            l_i_Ret = -1;
            break;
        }
        memset(l_px_TransportPublicField[l_i_i].m_puc_Data, 0, 128);
        l_px_TransportPublicField[l_i_i].m_ul_DataLenght = 128;
    }

    if(l_i_Ret == 0)
    {
        l_i_Ret = MSO_BioDB_GetPublicListData( i_h_Mso100Handle,
          				       0,			     //UC  i_uc_IndexDB,
        				       0,			     //UL  i_ul_UidData,
        				       &ul_NbTransport,	             //PUL io_pul_NbTranport,
        				       l_px_TransportPublicField,    //PT_TRANSPORT_PUBLIC_LIST_FIELD  io_ax_Strct,
        				       &l_uc_Status		     //PUC o_puc_ILV_Status
					     );



        if(l_i_Ret==0 && l_uc_Status==0)
        {
            fprintf(stdout, "List User -----> OK\n");
            fprintf(stdout, "User Number: %ld\n", ul_NbTransport);
            for(l_i_i=0; l_i_i<ul_NbTransport; l_i_i++)
	        fprintf(stdout, "\tIndex %ld: %s\n", l_px_TransportPublicField[l_i_i].m_ul_UserIndex,
		  			             l_px_TransportPublicField[l_i_i].m_puc_Data);
        }
        else
        {
            fprintf(stdout, "MSO_BioDB_GetPublicListData ret=%d status=0x%02X\n", l_i_Ret, l_uc_Status);
            if(l_uc_Status != 0)
                ConvertError(l_uc_Status);
            l_i_Ret = -1;
        }
    }

    for(l_i_i=0; l_i_i<ul_NbTransport; l_i_i++)
    {
        if(l_px_TransportPublicField[l_i_i].m_puc_Data != NULL)
            free(l_px_TransportPublicField[l_i_i].m_puc_Data);
    }
    if(l_px_TransportPublicField != NULL)
        free(l_px_TransportPublicField);

    return l_i_Ret;
}


int AddBaseRecord( MORPHO_HANDLE   i_h_Mso100Handle,
		   PT_BUF_PK       i_px_Buf_Pk,
		   UC		   i_uc_NbPk)
{
    I	l_i_Ret;
    UC	l_uc_Status;
    UC	l_uc_BaseStatus;
    UL  l_ul_IndexUser;
    T_BUF       l_x_UserID;
    T_BUF       l_x_addField[2];

    UC          l_auc_UserID[25];
    UC          l_auc_AddField1[25];
    UC          l_auc_AddField2[25];

    fprintf(stdout,"---Add Base Record:---\n");

    fprintf(stdout, "Enter UserID:\n->");
    fgets((PC)l_auc_UserID, 25, stdin);
    l_auc_UserID[strlen((PC)l_auc_UserID)-1] = 0;               // Suppress '\n'

    fprintf(stdout, "Enter Additionnal field 1:\n->");
    fgets((PC)l_auc_AddField1, 25, stdin);
    l_auc_AddField1[strlen((PC)l_auc_AddField1)-1] = 0; // Suppress '\n'

    fprintf(stdout, "Enter Additionnal field 2:\n->");
    fgets((PC)l_auc_AddField2, 25, stdin);
    l_auc_AddField2[strlen((PC)l_auc_AddField2)-1] = 0; // Suppress '\n'

    l_x_UserID.m_ul_Size =  strlen((PC)l_auc_UserID)+1;
    l_x_UserID.m_puc_Buf = l_auc_UserID;

    l_x_addField[0].m_ul_Size = strlen((PC)l_auc_AddField1)+1;
    l_x_addField[0].m_puc_Buf = l_auc_AddField1;

    l_x_addField[1].m_ul_Size = strlen((PC)l_auc_AddField2)+1;
    l_x_addField[1].m_puc_Buf = l_auc_AddField2;

    l_i_Ret = MSO_BioDBAddBaseRecord( i_h_Mso100Handle,	//MORPHO_HANDLE          i_h_Mso100Handle,
       				      0,		//UC                     i_uc_IndexDB,
        			      i_uc_NbPk,	//UC                     i_uc_NbPk,
        			      i_px_Buf_Pk,	//PT_BUF_PK              i_px_Pk,
        			      &l_x_UserID,	//PT_BUF                 i_px_UserId,
        			      2,		//UC                     i_uc_NbAddField,
        			      (PT_BUF)&l_x_addField,	//PT_BUF                 i_px_AddField,
        			      &l_ul_IndexUser,  //PUL                    o_pul_IndexUser,
        			      &l_uc_Status,	//PUC                    o_puc_ILV_Status,
        			      &l_uc_BaseStatus, //PUC                    o_puc_Base_Status,
        			      0			//BOOL                   i_b_NoCheckOnTemplate
				   );
    if(l_i_Ret==0 && l_uc_Status==0)
    {
        fprintf(stdout, "Add Base Record -----> OK\n");
        fprintf(stdout, "User Index: %ld\n", l_ul_IndexUser);
    }
    else
    {
        fprintf(stdout, "MSO_BioDBAddBaseRecord ret=%d status=0x%02X BaseStatus=0x%02X\n", l_i_Ret, l_uc_Status, l_uc_BaseStatus);
        if(l_uc_Status != 0)
            ConvertError(l_uc_Status);
        l_i_Ret = -1;
    }


    return l_i_Ret;
}


int capture( MORPHO_HANDLE	i_h_Mso100Handle,
			 UL				i_ul_AsynchronousEvnt,
			 US				i_us_Timeout,
			 PT_EXPORT_PK	io_px_ExportPk,
			 UC 			i_uc_PkFormat,
			 PT_EXPORT_IMAGE io_px_ExportImage )
{
    I  		l_i_Ret;
    UC		l_uc_Status;
    T_BUF       l_x_addField[3];
    UC		l_uc_EnrollStatus;
    UL		l_ul_UserDBIndex;
    UC		l_auc_UserID[25];
    UC		l_auc_AddField1[25];
    UC		l_auc_AddField2[25];

    fprintf(stdout,"---Captures one finger---\n");

    strcpy((PC)l_auc_UserID, "UserID");
    strcpy((PC)l_auc_AddField1, "field1");
    strcpy((PC)l_auc_AddField2, "field2");

    l_x_addField[0].m_ul_Size =  strlen((PC)l_auc_UserID)+1;
    l_x_addField[0].m_puc_Buf = l_auc_UserID;

    l_x_addField[1].m_ul_Size = strlen((PC)l_auc_AddField1)+1;
    l_x_addField[1].m_puc_Buf = l_auc_AddField1;

    l_x_addField[2].m_ul_Size = strlen((PC)l_auc_AddField2)+1;
    l_x_addField[2].m_puc_Buf = l_auc_AddField2;

    l_i_Ret = MSO_Bio_Enroll( i_h_Mso100Handle,
                              0,                        //UC            i_uc_IndexDB,
                              i_us_Timeout,             //US            i_us_Timeout,
                              0,                        //UC            i_uc_EnrollmentType,
                              1,                        //UC            i_uc_NbFinger,
                              0,                        //UC            i_uc_SaveRecord,
                              i_uc_PkFormat,            //UC            i_uc_NormalizedPK_Type,
                              3,                        //UC            i_uc_NbAddField,
                              (PT_BUF)&l_x_addField,    //PT_BUF        i_px_AddField,
                              i_ul_AsynchronousEvnt,    //UL			i_ul_AsynchronousEvnt,
                              io_px_ExportPk,    		//PT_EXPORT_PK  io_px_ExportPk,      //can be NULL
                              io_px_ExportImage,        //PT_EXPORT_IMAGE io_px_ExportImage, //can be NULL
                              &l_uc_EnrollStatus,       //PUC           o_puc_EnrollStatus,
                              &l_ul_UserDBIndex,        //PUL           o_pul_UserDBIndex,
                              &l_uc_Status              //PUC           o_puc_ILV_Status
                             );
    if(l_i_Ret==0 && l_uc_Status==0)
    {
        fprintf(stdout, "Capture -----> OK\n");
    }
    else
    {
        fprintf(stdout, "MSO_Bio_Enroll ret=%d status=0x%02X EnrollStatus=%d\n",
                                               l_i_Ret, l_uc_Status, l_uc_EnrollStatus);
        if(l_uc_Status != 0)
            ConvertError(l_uc_Status);
        l_i_Ret = -1;
    }

    return l_i_Ret;
}
