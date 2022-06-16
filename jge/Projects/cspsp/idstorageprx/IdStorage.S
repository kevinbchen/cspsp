	.set noreorder

#include "pspstub.s"

	STUB_START "IdStorage",0x40090000,0x00020005
	STUB_FUNC  0xBE861F44,ReadKey
	STUB_FUNC  0x5E4510CC,WriteKey
	STUB_END
