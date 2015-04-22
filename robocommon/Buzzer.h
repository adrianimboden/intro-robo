/**
 * \file
 * \brief Buzzer driver interface.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This is the interface to the buzzer.
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include <LegacyArgsCommand.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Shell parser routine.
 * \param cmd Pointer to command line string.
 * \param handled Pointer to status if command has been handled. Set to TRUE if command was understood.
 * \param io Pointer to stdio handle
 * \return Error code, ERR_OK if everything was ok.
 */
  uint8_t BUZ_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Let the buzzer sound for a specified time.
 * \param freqHz Frequency of the sound. Ignored if the buzzer is not supporting it.
 * \param durationMs Duration in milliseconds.
 * \return Error code, ERR_OK if everything is fine.
 */
uint8_t BUZ_Beep(uint16_t freqHz, uint16_t durationMs);

void BUZ_BlockingBeep(uint16_t freqHz, uint16_t durationMs);

/*!
 * \brief Initialization of the driver
 */
void BUZ_Init(void);

/*!
 * \brief De-initialization of the driver
 */
void BUZ_Deinit(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* BUZZER_H_ */
