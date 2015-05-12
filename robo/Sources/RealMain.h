/*!
 * \file RealMain.h
 * \brief RealMain
 *
 */

#ifndef REALMAIN_H_
#define REALMAIN_H_

#include "LegacyArgsCommand.h"

void APP_DebugPrint(unsigned char *str);

#ifdef __cplusplus
extern "C" {
#endif

void _main();

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* REALMAIN_H_ */
