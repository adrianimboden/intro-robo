/*!
 * \file RealMain.cpp
 * \brief RealMain
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
