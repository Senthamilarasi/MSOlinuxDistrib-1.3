#include <string.h>

#include "libMSO.h"
#include "MSO100.h"
#include "SpIlv.h"


#define I_EXTENSION_SIZE (sizeof(US)) /* Size of the extension Id */

#define GET_US(i_p_UC)  ( (US) (((US)*( (PUC)(i_p_UC)    +1))<<8  ) | (*((PUC)(i_p_UC))) )
#define GET_UL(i_puc_US)( (UL) (((UL) (GET_US((i_puc_US)+2))<<16 ) | (UL)(GET_US(i_puc_US) ) ) )

#define SET_US(i_p_UC,i_US)( (*(i_p_UC)=(UC)(i_US)),(*(i_p_UC+1)=(UC)((i_US)>>8)) )
#define SET_UL(i_p_UC,i_UL)( SET_US(i_p_UC,(US)i_UL), SET_US(i_p_UC+2,(US)((i_UL)>>16)))

#define SWAP_US(i_US) ( (i_US << 8)|(i_US & 0xFF) )

UL SizeOfI ( UC *i_puc_ILV )
{
	if ( *i_puc_ILV==ILV_ESCAPE )
		return sizeof(UC)+sizeof(UC);
	else
		return sizeof(UC);
}

UL SizeOfL ( UC *i_puc_ILV ) 
{
	UL l_ul_L_Value = 0;

	l_ul_L_Value = GET_US ( i_puc_ILV + SizeOfI(i_puc_ILV) );

	if ( l_ul_L_Value == (UL)0xFFFF )
		return sizeof(US)+sizeof(UL);
	else
		return sizeof(US);
}

UL SizeOfIandL ( UC *i_puc_ILV ) 
{
	UL l_ul_L_Value;

	l_ul_L_Value = SizeOfI(i_puc_ILV) + SizeOfL(i_puc_ILV);

	if ( ( *i_puc_ILV >= ILV_EXTENDED ) && ( *i_puc_ILV != ILV_ESCAPE ) )
		l_ul_L_Value += I_EXTENSION_SIZE;

	return l_ul_L_Value;
}

/*******************************************************************************
-- ILV_Init
*******************************************************************************/
I ILV_Init(UC *i_puc_ILV, UL *io_ul_ILVSize, US i_us_I)
{
 	*io_ul_ILVSize = 0;

	if ( (i_us_I >> 8) == ILV_ESCAPE )
	{
		i_puc_ILV[0] = ILV_ESCAPE;			/*ESC*/
		i_puc_ILV[1] = (UC)(i_us_I & 0xFF);		/*ID*/
	}
	else if ( i_us_I >= ILV_EXTENDED )
	{
		i_puc_ILV[0] = ILV_EXTENDED;			/*ID*/
		i_puc_ILV[3] = (UC)(i_us_I);			/*ID EXT*/
		i_puc_ILV[4] = (UC)(i_us_I>>8);			/*ID EXT*/
		*io_ul_ILVSize += I_EXTENSION_SIZE;
	}
	else
		i_puc_ILV[0] = (UC)(i_us_I & 0xFF);		/*ID*/

	i_puc_ILV[1] = (UC)(*io_ul_ILVSize);		/*LSB of Length*/
	i_puc_ILV[2] = 0x00;/* no nedd to do this : (UC)(io_ul_ILVSize>>8)*/	/*MSB of Length*/

	*io_ul_ILVSize += 3;
	
	return RETURN_NO_ERROR;
}

/*******************************************************************************
-- ILV_Check
*******************************************************************************/
I ILV_Check(UC *i_puc_ILV, UL i_ul_ILVSize)
{
	UL		l_ul_L_Value;
	I		l_s_Ret;
	US		l_us_I;

	l_s_Ret = RETURN_NO_ERROR;

	if(i_ul_ILVSize < 3 || i_ul_ILVSize < ( SizeOfI(i_puc_ILV)+2 ) || i_ul_ILVSize < SizeOfIandL(i_puc_ILV))
	{
		return ILV_ERROR_BAD_ILV_SIZE;
	}

	/*get the size of V*/
	l_ul_L_Value = ILV_GetL(i_puc_ILV) + SizeOfIandL(i_puc_ILV);

	if ( i_ul_ILVSize != l_ul_L_Value )
	{
		return ILV_ERROR_BAD_ILV_SIZE;
	}

	if ((UC)*i_puc_ILV == (UC)ILV_EXTENDED)
	{
		l_us_I = (US)GET_US(i_puc_ILV+SizeOfI(i_puc_ILV)+SizeOfL(i_puc_ILV));

		if(l_us_I < 0x100)
		{
			return ILV_ERROR_INVALID_ILV;
		}
	}

	return RETURN_NO_ERROR;
}

/*******************************************************************************
-- ILV_AddValue
*******************************************************************************/
I ILV_AddValue(UC *i_puc_ILV, UL *io_ul_ILVSize, UC* i_puc_Value, UL i_ul_ValueSize)
{
	UL l_ul_L_Value=0;
	
	/*get the old size of V*/
	l_ul_L_Value = ILV_GetL(i_puc_ILV);

	/*set new L value*/
	ILV_SetL(i_puc_ILV,io_ul_ILVSize,l_ul_L_Value+i_ul_ValueSize);

	/*add value at the end*/
	memcpy(i_puc_ILV+*io_ul_ILVSize,i_puc_Value,(I)i_ul_ValueSize);

	/*update ILV size*/
	*io_ul_ILVSize += i_ul_ValueSize;

	return RETURN_NO_ERROR;
}

/*******************************************************************************
-- ILV_GetValue
*******************************************************************************/
I ILV_GetValue(UC *i_puc_ILV, UL *io_ul_Pos, UC* i_puc_Value, UL i_ul_ValueSize)
{
	UL l_ul_L_Value=0;
	PUC	l_puc_V;
	
	
	l_ul_L_Value = ILV_GetL(i_puc_ILV);
	if (i_ul_ValueSize>(l_ul_L_Value-*io_ul_Pos))
		return ILVERR_BADPARAMETER;
	
	l_puc_V = ILV_GetV(i_puc_ILV);

	l_puc_V=(PUC)((UL)l_puc_V + *io_ul_Pos);

	memcpy(i_puc_Value,l_puc_V,(I)i_ul_ValueSize);
	
	*io_ul_Pos+=+i_ul_ValueSize;

	return RETURN_NO_ERROR;
}

/*******************************************************************************
-- ILV_GetI
*******************************************************************************/
US ILV_GetI ( UC *i_puc_ILV )
{
	US l_us_I;

	if ((UC)*i_puc_ILV == (UC)ILV_ESCAPE)
	{
		l_us_I = (US)( (ILV_ESCAPE << 8) | (US)(*(i_puc_ILV+1)));
	}
	else if ((UC)*i_puc_ILV == (UC)ILV_EXTENDED)
	{
		l_us_I = (US)GET_US(i_puc_ILV+SizeOfI(i_puc_ILV)+SizeOfL(i_puc_ILV));
	}
	else
		l_us_I = (UC)*i_puc_ILV;

	return l_us_I;

}

/*******************************************************************************
-- ILV_GetL
*******************************************************************************/
UL ILV_GetL(UC *i_puc_ILV)
{
	UL	l_ul_L_Value=0;
	PUC	l_puc_Ptr;

	l_puc_Ptr = i_puc_ILV+SizeOfI(i_puc_ILV);

	l_ul_L_Value = (UL)GET_US( l_puc_Ptr );

	if (l_ul_L_Value == (UL)0xFFFF)
		l_ul_L_Value = GET_UL( l_puc_Ptr+sizeof(US) );

	if ( ( *i_puc_ILV >= ILV_EXTENDED ) && ( *i_puc_ILV != ILV_ESCAPE ) )
		l_ul_L_Value -= I_EXTENSION_SIZE;

	return l_ul_L_Value;
}

/*******************************************************************************
-- ILV_GetSize
*******************************************************************************/
UL	ILV_GetSize(UC *i_puc_ILV)
{
	return (ILV_GetL(i_puc_ILV)+SizeOfIandL(i_puc_ILV));
}

/*******************************************************************************
-- ILV_GetV
*******************************************************************************/
UC	*ILV_GetV(UC *i_puc_ILV)
{
	return (i_puc_ILV+SizeOfIandL(i_puc_ILV));
}

/*******************************************************************************
-- ILV_SetL
*******************************************************************************/
void ILV_SetL(UC *i_puc_ILV,UL *io_ul_ILVSize,UL i_ul_LValue)
{
	UL	l_ul_oldL_Value=0;
	UL	l_ul_i;
	UC	*l_puc_tmp;
	
	if ( ( *i_puc_ILV >= ILV_EXTENDED ) && ( *i_puc_ILV != ILV_ESCAPE ) )
		i_ul_LValue += I_EXTENSION_SIZE;

	l_ul_oldL_Value = ILV_GetL(i_puc_ILV);
	if (l_ul_oldL_Value>=(UL)0xFFFF)
	{
		if (i_ul_LValue>=(UL)0xFFFF)
		{
			//c'etait un UL et c'est toujours un UL
			SET_UL ( i_puc_ILV+SizeOfI(i_puc_ILV)+sizeof(US), i_ul_LValue );
		}
		else
		{
			//c'etait un UL et c'est un US
			SET_US ( i_puc_ILV+SizeOfI(i_puc_ILV), i_ul_LValue );

			//on decale les V de 4 octets à gauche
			l_puc_tmp = i_puc_ILV+SizeOfI(i_puc_ILV)+sizeof(US)+sizeof(UL);
			for (l_ul_i=0;l_ul_i<i_ul_LValue;l_ul_i++)
			{
				l_puc_tmp[l_ul_i-sizeof(UL)] = l_puc_tmp[l_ul_i];
			}

			*io_ul_ILVSize=(*io_ul_ILVSize) - sizeof(UL);
		}
	}
	else
	{
		if (i_ul_LValue<(UL)0xFFFF)
		{
			//c'etait un US et c'est toujours un US
			SET_US ( i_puc_ILV+SizeOfI(i_puc_ILV), i_ul_LValue );
		}
		else
		{
			//c'etait un US et c'est un UL
			//on decale les V de 4 octets à droites
			l_puc_tmp = i_puc_ILV+SizeOfI(i_puc_ILV)+sizeof(US);
			for (l_ul_i=(*io_ul_ILVSize)- (SizeOfI(i_puc_ILV)+sizeof(US));l_ul_i>0;l_ul_i--) 
			{
				l_puc_tmp[l_ul_i-1+sizeof(UL)] = l_puc_tmp[l_ul_i-1];
			}

			l_ul_oldL_Value = (UL)0xFFFF;
			SET_US ( i_puc_ILV+SizeOfI(i_puc_ILV), l_ul_oldL_Value );
			SET_UL ( i_puc_ILV+SizeOfI(i_puc_ILV)+sizeof(US), i_ul_LValue );
			*io_ul_ILVSize=(*io_ul_ILVSize) + sizeof(UL);			
		}
	}
}

/***********************************************************************
descriptions :
***********************************************************************/
I ILV_GetILV
(
	PUC	i_puc_ILV, 
	PUS	o_pus_I, 
	PUL	o_pul_L, 
	PUC*	o_ppuc_V
)
{
	*o_pus_I	= (US)ILV_GetI(i_puc_ILV);

	*o_pul_L	= ILV_GetL(i_puc_ILV);

	*o_ppuc_V	= ILV_GetV(i_puc_ILV);

	return RETURN_NO_ERROR;
}

/***********************************************************************
descriptions :

  o_pul_IlvSize can't be null

***********************************************************************/
I ILV_SetILV
(
	   PT_BUF	io_px_BufIlv, 
	   US		i_us_I, 
	   PT_BUF	i_px_Data,
	   PUL		o_pul_IlvSize)
{
	I	l_i_Ret;
	UL	l_ul_ILVSize;

	l_ul_ILVSize = 0;

	l_i_Ret = ILV_Init(io_px_BufIlv->m_puc_Buf, &l_ul_ILVSize, i_us_I);
	if (l_i_Ret)
		return l_i_Ret;

	if( io_px_BufIlv->m_ul_Size < (l_ul_ILVSize+i_px_Data->m_ul_Size) )
		return ILV_ERROR_NOT_ENOUGH_MEMORY;

	l_i_Ret = ILV_AddValue(io_px_BufIlv->m_puc_Buf, &l_ul_ILVSize, i_px_Data->m_puc_Buf,i_px_Data->m_ul_Size);
	if (l_i_Ret)
		return l_i_Ret;

	if(o_pul_IlvSize)
		*o_pul_IlvSize = ILV_GetSize(io_px_BufIlv->m_puc_Buf);

	return RETURN_NO_ERROR;
}

/***********************************************************************
descriptions :

***********************************************************************/
I ILV_AddILV
(
	PT_BUF	io_px_BufIlv,
	UL	i_ul_SizeData, 
	PUC	i_puc_Data,
	PUL	o_pul_IlvSize
)
{
	UL	l_ul_IlvSize;
	I	l_i_Ret;

	l_ul_IlvSize = ILV_GetSize(io_px_BufIlv->m_puc_Buf);

	if(io_px_BufIlv->m_ul_Size < (l_ul_IlvSize+i_ul_SizeData) )
	{
		return ILV_ERROR_NOT_ENOUGH_MEMORY;
	}

	l_i_Ret = ILV_AddValue(io_px_BufIlv->m_puc_Buf, &l_ul_IlvSize, i_puc_Data,i_ul_SizeData);
	
	if (l_i_Ret)
		return l_i_Ret;

	if(o_pul_IlvSize)
		*o_pul_IlvSize = l_ul_IlvSize;

	return l_i_Ret;
}
/***********************************************************************
descriptions :

  o_pc_ILV_Status can be null 
  o_pi_EmbeddedError can be null 
***********************************************************************/
I ILV_GetErrorILV
(
	PUC		i_puc_V, 
	PC		o_pc_ILV_Status, 
	PI		o_pi_EmbeddedError
)
{
	C	l_c_ILV_Status;

	l_c_ILV_Status = *i_puc_V;

	if(o_pi_EmbeddedError)
	{
		if(l_c_ILV_Status)
			memcpy(o_pi_EmbeddedError,i_puc_V+1,sizeof(UL));
		else
			*o_pi_EmbeddedError = RETURN_NO_ERROR;
	}

	if(o_pc_ILV_Status)
		*o_pc_ILV_Status = l_c_ILV_Status;

	return RETURN_NO_ERROR;
}

