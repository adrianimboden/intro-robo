/*
 * RealMain.cpp
 *
 *  Created on: Feb 24, 2015
 *      Author: adrian
 */

#include "RealMain.h"

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#include "Platform_local.h"
#include "WAIT1.h"

/**
 * C++ world main function
 */
void realMain()
{
	for (;;)
	{
		Led_Red_On();
		WAIT1_Waitms(20);
		Led_Green_On();
		WAIT1_Waitms(20);
		Led_Blue_On();
		WAIT1_Waitms(20);

		Led_Red_Off();
		WAIT1_Waitms(20);
		Led_Green_Off();
		WAIT1_Waitms(20);
		Led_Blue_Off();
		WAIT1_Waitms(20);
	}
}



#ifdef __cplusplus
extern "C" {
#endif

/**
 * Forward to C++ world
 */
void _main()
{
	realMain();
}

#ifdef __cplusplus
}  /* extern "C" */
#endif
