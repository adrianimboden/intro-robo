/*
 * Music.h
 *
 *  Created on: 07.12.2014
 *      Author: Erich Styger
 */

#ifndef MUSIC_H_
#define MUSIC_H_

#include "LegacyArgsCommand.h"
//#include "CLS1.h" /* shell interface */
/*!
 * \brief Module command line parser
 * \param cmd Pointer to the command
 * \param handled Return value if the command has been handled by parser
 * \param io Shell standard I/O handle
 * \return Error code, ERR_OK if everything is OK
 */
uint8_t MUSIC_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);

typedef enum {
	MUSIC_POINT,
	MUSIC_BUTT_KICK,
	MUSIC_FOOL,
	MUSIC_GO_EYES,
	MUSIC_AHH,
	MUSIC_TOUGH,
	MUSIC_THEME_LAST /* sentinel, must be last in list! */
} MUSIC_Theme;

/*!
 * \brief Play a theme
 * \param theme Theme to play
 */
void MUSIC_PlayTheme(MUSIC_Theme theme);

/*!
 * \brief Used to find out if we are still playing a song.
 * \return TRUE if playing, FALSE if not.
 */
bool MUSIC_IsPlaying(void);

/*!
 * \brief Starts playing music
 */
void MUSIC_Start(void);

/*!
 * \brief Stops playing music
 */
void MUSIC_Stop(void);

/*!
 * \brief Driver initialization.
 */
void MUSIC_Init(void);

/*!
 * \brief Driver de-initialization.
 */
void MUSIC_Deinit(void);

#endif /* MUSIC_H_ */
