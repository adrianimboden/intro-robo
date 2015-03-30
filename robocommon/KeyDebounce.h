/**
 * \file
 * \brief Key debouncing interface.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements debouncing for multiple keys.
 */
#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif
/*!
 * \brief Kicks the debouncing state machine.
 */
void KEYDBNC_Process(void);

/*!
 * \brief Driver initialization.
 */
void KEYDBNC_Init(void);

/*!
 * \brief Driver de-initialization.
 */
void KEYDBNC_Deinit(void);
