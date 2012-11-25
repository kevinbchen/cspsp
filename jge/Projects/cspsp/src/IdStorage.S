	.set noreorder

#include "pspstub.s"

	STUB_START "IdStorage",0x40090000,0x00040005
	STUB_FUNC  0xBE861F44,ReadKey
	STUB_FUNC  0x5E4510CC,WriteKey
	STUB_FUNC  0x971C5683,CreateKey
	STUB_FUNC  0x3E808F59,DeleteKey
	STUB_END
