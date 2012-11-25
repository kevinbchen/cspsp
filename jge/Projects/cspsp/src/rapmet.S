	.set noreorder

#include "pspstub.s"

	STUB_START "rapmet",0x40090000,0x00010005
	STUB_FUNC  0x831F284A,CheckTempAR
	STUB_END
