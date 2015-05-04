/**
 * \file
 * \brief Tachometer Module
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module to calculate the speed based on the quadrature counter.
 */

#include "Platform.h" /* interface to the platform */
#if PL_HAS_MOTOR_TACHO
#include "Tacho.h"    /* our own interface */
extern "C"{
#include "Q4CLeft.h"
#include "Q4CRight.h"
#include "UTIL1.h"
#include "FRTOS1.h"
}

  /*! \todo Set appropriate values for NOF_ROUND_PULSE, TACHO_SAMPLE_PERIOD_MS and NOF_HISTORY */
#define NOF_ROUND_PULSE      (12*75) // ?
  /*!< \todo number of pulses per round, given by encoder hardware, and we are detecting 4 steps for each period. Useful to calculate revolutions per time unit. */
#define TACHO_SAMPLE_PERIOD_MS (5) // ?	maybe 2
  /*!< \todo speed sample period in ms. Make sure that speed is sampled at the given rate. */
#define NOF_HISTORY (16U+1U) // ? 16U+1U to give the compiler the chance to make shifting instead division
  /*!< number of samples for speed calculation (>0):the more, the better, but the slower. */

static volatile uint16_t TACHO_LeftPosHistory[NOF_HISTORY], TACHO_RightPosHistory[NOF_HISTORY];
  /*!< for better accuracy, we calculate the speed over some samples */
static volatile uint8_t TACHO_PosHistory_Index = 0;
  /*!< position index in history */

static int32_t TACHO_currLeftSpeed = 0, TACHO_currRightSpeed = 0;
  /*!< position index in history */

int32_t TACHO_GetSpeed(bool isLeft) {
  if (isLeft) {
    return TACHO_currLeftSpeed;
  } else {
    return TACHO_currRightSpeed;
  }
}

void TACHO_CalcSpeed(void) {
  /*! \todo Implement/change function as needed, make sure implementation below matches your needs */
  /* we calculate the speed as follow:
                              1000         
  steps/sec =  delta * ----------------- 
                       samplePeriod (ms) 
  As this function may be called very frequently, it is important to make it as efficient as possible!
   */
  int16_t deltaLeft, deltaRight, newLeft, newRight, oldLeft, oldRight;
  int32_t speedLeft, speedRight;
  bool negLeft, negRight;

  EnterCritical();
  oldLeft = (int16_t)TACHO_LeftPosHistory[TACHO_PosHistory_Index]; /* oldest left entry */
  oldRight = (int16_t)TACHO_RightPosHistory[TACHO_PosHistory_Index]; /* oldest right entry */
  if (TACHO_PosHistory_Index==0) { /* get newest entry */
    newLeft = (int16_t)TACHO_LeftPosHistory[NOF_HISTORY-1];
    newRight = (int16_t)TACHO_RightPosHistory[NOF_HISTORY-1];
  } else {
    newLeft = (int16_t)TACHO_LeftPosHistory[TACHO_PosHistory_Index-1];
    newRight = (int16_t)TACHO_RightPosHistory[TACHO_PosHistory_Index-1];
  }
  ExitCritical();
  deltaLeft = oldLeft-newLeft; /* delta of oldest position and most recent one */
  /* use unsigned arithmetic */
  if (deltaLeft < 0) {
    deltaLeft = -deltaLeft;
    negLeft = TRUE;
  } else {
    negLeft = FALSE;
  }
  deltaRight = oldRight-newRight; /* delta of oldest position and most recent one */
  /* use unsigned arithmetic */
  if (deltaRight < 0) {
    deltaRight = -deltaRight;
    negRight = TRUE;
  } else {
    negRight = FALSE;
  }
  /* calculate speed. this is based on the delta and the time (number of samples or entries in the history table) */
  speedLeft = (int32_t)(deltaLeft * 1000/(TACHO_SAMPLE_PERIOD_MS*(NOF_HISTORY-1)));
  if (negLeft) {
    speedLeft = -speedLeft;
  }
  speedRight = (int32_t)(deltaRight * 1000/(TACHO_SAMPLE_PERIOD_MS*(NOF_HISTORY-1)));
  if (negRight) {
    speedRight = -speedRight;
  }
  TACHO_currLeftSpeed = -speedLeft; /* store current speed in global variable */
  TACHO_currRightSpeed = -speedRight; /* store current speed in global variable */
}

void TACHO_Sample(void) {
  /*! \todo Implement/change function as needed, make sure implementation below matches your needs */
  static int cnt = 0;
  /* get called from the RTOS tick counter. Divide the frequency. */
  cnt += portTICK_RATE_MS;
  if (cnt < TACHO_SAMPLE_PERIOD_MS) {
    return;
  }
  cnt = 0; /* reset counter */
  /* left */
  TACHO_LeftPosHistory[TACHO_PosHistory_Index] = Q4CLeft_GetPos();
  TACHO_RightPosHistory[TACHO_PosHistory_Index] = Q4CRight_GetPos();
  TACHO_PosHistory_Index++;
  if (TACHO_PosHistory_Index >= NOF_HISTORY) {
    TACHO_PosHistory_Index = 0;
  }
}

#if PL_HAS_SHELL
/*!
 * \brief Prints the system low power status
 * \param io I/O channel to use for printing status
 */
static void TACHO_PrintStatus(const CLS1_StdIOType *io) {
  unsigned char buf[sizeof("-1234567890")];

  //TACHO_CalcSpeed(); /*! \todo only temporary until this is done periodically */
  CLS1_SendStatusStr((unsigned char*)"Tacho", (unsigned char*)"\r\n", io->stdOut);

  CLS1_SendStatusStr((unsigned char*)"  L speed ", (unsigned char*)"", io->stdOut);

  //TODO refactor this ugly conversion
  UTIL1_Num32sToStr(buf, sizeof(buf), TACHO_GetSpeed(TRUE));

  CLS1_SendStatusStr(buf, (unsigned char*)"", io->stdOut);

  CLS1_SendStr((unsigned char*)" steps/sec\r\n", io->stdOut);

  CLS1_SendStatusStr((unsigned char*)"  R speed ", (unsigned char*)"", io->stdOut);

  //TODO refactor this ugly conversion
  UTIL1_Num32sToStr(buf, sizeof(buf), TACHO_GetSpeed(FALSE));
  CLS1_SendStatusStr(buf, (unsigned char*)"", io->stdOut);

  CLS1_SendStr((unsigned char*)" steps/sec\r\n", io->stdOut);
}

/*! 
 * \brief Prints the help text to the console
 * \param io I/O channel to be used
 */
static void TACHO_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"tacho", (unsigned char*)"Group of tacho commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows tacho help or status\r\n", io->stdOut);
}

uint8_t TACHO_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
  if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, (char*)"tacho help")==0) {
    TACHO_PrintHelp(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, (char*)"tacho status")==0) {
    TACHO_PrintStatus(io);
    *handled = TRUE;
  }
  return ERR_OK;
}
#endif /* PL_HAS_SHELL */

void TACHO_Deinit(void) {
}

void TACHO_Init(void) {
  TACHO_currLeftSpeed = 0;
  TACHO_currRightSpeed = 0;
  TACHO_PosHistory_Index = 0;
}

#endif /* PL_HAS_MOTOR_TACHO */

