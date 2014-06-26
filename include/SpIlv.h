#ifndef __SPILV_H
#define __SPILV_H

#include "Ilv_definitions.h"

#define I_L_MAX_SIZE	9	// (I)1 + (L)2 + (I étendu)2 + (L étendu)4

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************************************************/
I ILV_Init
(
	UC *i_puc_ILV, 
	UL *io_ul_ILVSize, 
	US i_us_I
);

/************************************************************/
I ILV_Check
(
	UC *i_puc_ILV, 
	UL i_ul_ILVSize
);

/************************************************************/
I ILV_AddValue
(
	UC *i_puc_ILV, 
	UL *
	io_ul_ILVSize, 
	UC* i_puc_Value, 
	UL i_ul_ValueSize
);

/************************************************************/
US 	ILV_GetI
(
	UC *i_puc_ILV
);

/************************************************************/
UL 	ILV_GetL
(
	UC *i_puc_ILV
);

/************************************************************/
void ILV_SetL
(
	UC *i_puc_ILV,
	UL *io_ul_ILVSize,
	UL i_ul_LValue
);

/************************************************************/
UC	*ILV_GetV
(
	UC *i_puc_ILV
);

/************************************************************/
UL	ILV_GetSize
(
	UC *i_puc_ILV
);

/************************************************************/
I ILV_GetValue
(
	UC *i_puc_ILV,
	UL *io_ul_Pos,
	UC *i_puc_Value,
	UL i_ul_ValueSize
);

/************************************************************/
I	ILV_GetILV
(
	PUC	i_puc_ILV, 
	PUS	o_pul_I, 
	PUL	o_pul_L, 
	PUC*	o_ppuc_V
);


/************************************************************/
I	ILV_SetILV
(
	PT_BUF	io_px_BufIlv, 
	US	i_us_I, 
	PT_BUF	i_px_Data,
	PUL	o_pul_IlvSize
);

/************************************************************/
I	ILV_AddILV
(
	PT_BUF	io_px_BufIlv,
	UL	i_ul_SizeData, 
	PUC	i_puc_Data,
	PUL	o_pul_IlvSize
);

/************************************************************/
I	ILV_GetErrorILV
(
	PUC	i_puc_V, 
	PC	o_pc_ILV_Status, 
	PI	o_pi_EmbeddedError
);

/************************************************************/
UL	SizeOfI
(
	UC *i_puc_ILV
);

/************************************************************/
UL	SizeOfL
(
	UC *i_puc_ILV
);

/************************************************************/
UL	SizeOfIandL
(
	UC *i_puc_ILV
);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__SPILV_H*/

