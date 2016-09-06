#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "common.h"
#include "lua_alg.h"

#include <stdint.h>

typedef struct                   // <--------|
{                                //          |
     uint8_t L;                  //          |
     uint8_t H;                  //          |16BIT与
} INT_08BIT_2;                   //          |
                                 //          |8BIT转换
typedef union                    //          |
{	                             //          |
    uint16_t Int16Bit;           //          |
    INT_08BIT_2  Int08Bit;       //          |
} INT_16_08;               		 // <--------|

uint16_t const CRC_Tab[] =  {
    //CRCTB1: ;?256??
	0x00000,0x01021,0x02042,0x03063,
	0x04084,0x050a5,0x060c6,0x070e7,
	0x08108,0x09129,0x0a14a,0x0b16b,
	0x0c18c,0x0d1ad,0x0e1ce,0x0f1ef,
	0x01231,0x00210,0x03273,0x02252,
	0x052b5,0x04294,0x072f7,0x062d6,
	0x09339,0x08318,0x0b37b,0x0a35a,
	0x0d3bd,0x0c39c,0x0f3ff,0x0e3de,
	0x02462,0x03443,0x00420,0x01401,
	0x064e6,0x074c7,0x044a4,0x05485,
	0x0a56a,0x0b54b,0x08528,0x09509,
	0x0e5ee,0x0f5cf,0x0c5ac,0x0d58d,
	0x03653,0x02672,0x01611,0x00630,
	0x076d7,0x066f6,0x05695,0x046b4,
	0x0b75b,0x0a77a,0x09719,0x08738,
	0x0f7df,0x0e7fe,0x0d79d,0x0c7bc,
	0x048c4,0x058e5,0x06886,0x078a7,
	0x00840,0x01861,0x02802,0x03823,
	0x0c9cc,0x0d9ed,0x0e98e,0x0f9af,
	0x08948,0x09969,0x0a90a,0x0b92b,
	0x05af5,0x04ad4,0x07ab7,0x06a96,
	0x01a71,0x00a50,0x03a33,0x02a12,
	0x0dbfd,0x0cbdc,0x0fbbf,0x0eb9e,
	0x09b79,0x08b58,0x0bb3b,0x0ab1a,
	0x06ca6,0x07c87,0x04ce4,0x05cc5,
	0x02c22,0x03c03,0x00c60,0x01c41,
	0x0edae,0x0fd8f,0x0cdec,0x0ddcd,
	0x0ad2a,0x0bd0b,0x08d68,0x09d49,
	0x07e97,0x06eb6,0x05ed5,0x04ef4,
	0x03e13,0x02e32,0x01e51,0x00e70,
	0x0ff9f,0x0efbe,0x0dfdd,0x0cffc,
	0x0bf1b,0x0af3a,0x09f59,0x08f78,
    //CRCTB2: ;?256??
	0x09188,0x081a9,0x0b1ca,0x0a1eb,
	0x0d10c,0x0c12d,0x0f14e,0x0e16f,
	0x01080,0x000a1,0x030c2,0x020e3,
	0x05004,0x04025,0x07046,0x06067,
	0x083b9,0x09398,0x0a3fb,0x0b3da,
	0x0c33d,0x0d31c,0x0e37f,0x0f35e,
	0x002b1,0x01290,0x022f3,0x032d2,
	0x04235,0x05214,0x06277,0x07256,
	0x0b5ea,0x0a5cb,0x095a8,0x08589,
	0x0f56e,0x0e54f,0x0d52c,0x0c50d,
	0x034e2,0x024c3,0x014a0,0x00481,
	0x07466,0x06447,0x05424,0x04405,
	0x0a7db,0x0b7fa,0x08799,0x097b8,
	0x0e75f,0x0f77e,0x0c71d,0x0d73c,
	0x026d3,0x036f2,0x00691,0x016b0,
	0x06657,0x07676,0x04615,0x05634,
	0x0d94c,0x0c96d,0x0f90e,0x0e92f,
	0x099c8,0x089e9,0x0b98a,0x0a9ab,
	0x05844,0x04865,0x07806,0x06827,
	0x018c0,0x008e1,0x03882,0x028a3,
	0x0cb7d,0x0db5c,0x0eb3f,0x0fb1e,
	0x08bf9,0x09bd8,0x0abbb,0x0bb9a,
	0x04a75,0x05a54,0x06a37,0x07a16,
	0x00af1,0x01ad0,0x02ab3,0x03a92,
	0x0fd2e,0x0ed0f,0x0dd6c,0x0cd4d,
	0x0bdaa,0x0ad8b,0x09de8,0x08dc9,
	0x07c26,0x06c07,0x05c64,0x04c45,
	0x03ca2,0x02c83,0x01ce0,0x00cc1,
	0x0ef1f,0x0ff3e,0x0cf5d,0x0df7c,
	0x0af9b,0x0bfba,0x08fd9,0x09ff8,
	0x06e17,0x07e36,0x04e55,0x05e74,
	0x02e93,0x03eb2,0x00ed1,0x01ef0
};						   
							   
unsigned short AlgCRC16(unsigned char *str, int size)
{          
    uint32_t     i;
    INT_16_08	 crc;
    INT_16_08	 y;

    crc.Int16Bit = 0;

    for( i = 0 ; i < size ; i++ )
    {
        y.Int16Bit    = CRC_Tab[ str[ i ] ^ crc.Int08Bit.H ];
        y.Int08Bit.H ^= crc.Int08Bit.L;
        crc.Int16Bit  = y.Int16Bit;
    }

    return( crc.Int16Bit );
}

static int _calcCRC16(lua_State *L)
{
	unsigned char *data;
	unsigned short res;
	int len;

	//检查栈空间
	luaL_checkstack(L, 1, "LUA Stack OverFlow");

	//获取参数
	data = luaL_checkstring(L, -2);
	len = (unsigned int)luaL_checkinteger(L, -1);

	res = AlgCRC16(data, len);
	lua_pushnumber(L, res);
	
	return 1;
}

static const struct luaL_Reg g_alglib [] = {
	{"crc16", _calcCRC16},
	{NULL, NULL} /* sentinel */
};

int ALG_Regist(lua_State *L)
{
	luaL_newlib(L, g_alglib);
	lua_setglobal(L, "alg");

	return ERR_OK;
}

