#ifndef __Crc_h
#define __Crc_h

/*******************************************************************************
-- ComputeCrc16 implementation
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

void ComputeCrc16 (PUC i_puc_Buffer, UL i_ul_Len, PUC io_puc_CrcH, PUC io_puc_CrcL);

#ifdef __cplusplus
}
#endif

#endif /*__Crc_h*/
