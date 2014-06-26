int MSO_Connect( MORPHO_HANDLE * i_h_Mso100Handle, 
				 int i_i_ConnectionMode,	// COM_USB or COM_RS232
				 char *i_c_StrDevice,	// Serial port device /dev/ttyS0
				 int i_i_speed,	// Baud Rate
				 int *o_pi_IdProduct	// Return type of product
				);

int ConvertError(UC i_uc_Status);

int DeleteUserIndex(MORPHO_HANDLE i_h_Mso100Handle);
int DeleteAllUser(MORPHO_HANDLE i_h_Mso100Handle);
int DestroyDatabase(MORPHO_HANDLE i_h_Mso100Handle);
int CreateDataBase(MORPHO_HANDLE i_h_Mso100Handle);
int GetBaseConfig(MORPHO_HANDLE i_h_Mso100Handle);

int VerifyMatch(MORPHO_HANDLE i_h_Mso100Handle,
				PT_BUF_PK i_px_Buf_PkSrc,
				PT_BUF_PK i_px_Buf_PkRef,
				UC i_uc_NbPkRef);

int IdentifyMatch(MORPHO_HANDLE i_h_Mso100Handle, PT_BUF_PK i_px_Buf_Pk);

int Verify(MORPHO_HANDLE i_h_Mso100Handle,
	   PT_BUF_PK i_x_Buf_Pk,
	   UC i_uc_NbFinger, UL i_ul_AsynchronousEvnt, US i_us_Timeout);

int Identify(MORPHO_HANDLE i_h_Mso100Handle, UL i_ul_AsynchronousEvnt, US i_us_Timeout);

int EnrollUser( MORPHO_HANDLE i_h_Mso100Handle, 
				UL i_ul_AsynchronousEvnt, 
				US i_us_Timeout, 
				PT_EXPORT_PK io_px_ExportPk,	//can be NULL
				PT_EXPORT_IMAGE io_px_ExportImage	//can be NULL
			);


int ListDataBaseUser(MORPHO_HANDLE i_h_Mso100Handle);

int AddBaseRecord(MORPHO_HANDLE i_h_Mso100Handle, PT_BUF_PK i_x_Buf_Pk, UC i_uc_NbPk);

int capture( MORPHO_HANDLE	i_h_Mso100Handle,
			 UL				i_ul_AsynchronousEvnt,
			 US				i_us_Timeout,
			 PT_EXPORT_PK	io_px_ExportPk,
			 UC 			i_uc_PkFormat,
			 PT_EXPORT_IMAGE io_px_ExportImage );
