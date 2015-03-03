/*
 * RealMain.cpp
 *
 *  Created on: Feb 24, 2015
 *      Author: adrian
 */

#include "RealMain.h"

/**
 * C++ world main function
 */
void realMain()
{
	for (;;)
	{
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
