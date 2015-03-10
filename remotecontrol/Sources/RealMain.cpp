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

class DisableInterrupts
{
public:
	DisableInterrupts()
	{
		//... disable
	}

	~DisableInterrupts()
	{
		//... enable
	}

private:
	//state
};

template <typename Fn>
void runWithDisabledInterrupts(Fn fn)
{

	//state
	//disable
	fn();
	//enable
}

/**
 * C++ world main function
 */
void realMain()
{
	DisableInterrupts disable;
	runWithDisabledInterrupts([&]
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
	});
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
