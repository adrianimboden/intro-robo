/**
 * \file
 * \brief Reflectance sensor driver interface.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements a driver for the reflectance sensor array.
 */

#ifndef REFLECTANCE_H_
#define REFLECTANCE_H_

#include "Platform.h"
#if PL_HAS_LINE_SENSOR

/*!
 * \brief returns the current line value (weighted average).
 */
uint16_t REF_GetLineValue(void);

/*!
 * \brief Driver Deinitialization.
 */
void REF_Deinit(void);

/*!
 * \brief Driver Initialization.
 */
void REF_Init(void);

class IOStream;

void REF_PrintStatus(IOStream& ioStream);

#endif /* PL_HAS_LINE_SENSOR */

#endif /* REFLECTANCE_H_ */
