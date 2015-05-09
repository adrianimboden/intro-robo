/**
 * \file
 * \brief Accelerometer Module
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module to calculate the speed based on the quadrature counter.
 */

#include "Platform.h" /* interface to the platform */

#if PL_HAS_ACCEL
#include "Accel.h"    /* our own interface */
extern "C"{
#include "MMA1.h"     /* interface to accelerometer */
}

void ACCEL_GetValues(int16_t *x, int16_t *y, int16_t *z) {
  int16_t xmg, ymg, zmg;
  
  xmg = MMA1_GetXmg();
  ymg = MMA1_GetYmg();
  zmg = MMA1_GetZmg();
  *x = xmg;
  *y = ymg;
  *z = zmg;
}

void ACCEL_Deinit(void) {
  (void)MMA1_Deinit();
}

uint8_t ACCEL_LowLevelInit(void) {
  uint8_t res;
  
  res = MMA1_Init(); /* this might communicate to the device using I2C, so it needs interrupts enabled */
  if (res!=ERR_OK) {
    return res;
  }
  res = ACCEL_Enable();
  if (res!=ERR_OK) {
    return res;
  }
  res = MMA1_SetFastMode(FALSE); /* disable fast mode of the sensor: otherwise we cannot read individual sensor values */
  if (res!=ERR_OK) {
    return res;
  }
  return ERR_OK;
}

uint8_t ACCEL_Enable(void) {
  uint8_t res;
  
  res = MMA1_Enable();
  return res;
}

uint8_t ACCEL_isEnabled(bool *isEnabled) {
  uint8_t res;
  
  res = MMA1_isEnabled(isEnabled);
  return res;
}

/*! \brief Initialises module */
void ACCEL_Init(void) {
#if !PL_HAS_RTOS /* with an RTOS, the interrupts are disabled here. Need to do this in a task */
  ACCEL_LowLevelInit();
#endif
}

#if PL_HAS_SHELL

static uint8_t PrintStatus(const CLS1_StdIOType *io) {
  unsigned char buf[24];
  int8_t temperature;
  uint16_t val;
  uint8_t val8;
  bool isEnabled;
  int16_t val16s;

  CLS1_SendStatusStr((unsigned char*)"MMA1", (unsigned char*)"\r\n", io->stdOut);

  UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"0x");
  UTIL1_strcatNum8Hex(buf, sizeof(buf), (uint8_t)MMA1_I2C_ADDR);
  UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  CLS1_SendStatusStr((unsigned char*)"  I2C addr : ", buf, io->stdOut);

  if (MMA1_isEnabled(&isEnabled)!=ERR_OK) {
    UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"FAIL!\r\n");
  } else {
    if (isEnabled) {
      UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"yes\r\n");
    } else {
      UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"no\r\n");
    }
  }
  CLS1_SendStatusStr((unsigned char*)"  enabled : ", buf, io->stdOut);

  CLS1_SendStatusStr((unsigned char*)"  raw : ", (unsigned char*)"0x", io->stdOut);
  val = MMA1_MeasureGetRawX();
  buf[0] = '\0';
  UTIL1_strcatNum16Hex(buf, sizeof(buf), (uint16_t)val);
  CLS1_SendStr(buf, io->stdOut);
  CLS1_SendStr((unsigned char*)" (", io->stdOut);
  CLS1_SendNum16s((int16_t)val, io->stdOut);
  CLS1_SendStr((unsigned char*)"), 0x", io->stdOut);

  val = MMA1_MeasureGetRawY();
  buf[0] = '\0';
  UTIL1_strcatNum16Hex(buf, sizeof(buf), (uint16_t)val);
  CLS1_SendStr(buf, io->stdOut);
  CLS1_SendStr((unsigned char*)" (", io->stdOut);
  CLS1_SendNum16s((int16_t)val, io->stdOut);
  CLS1_SendStr((unsigned char*)"), 0x", io->stdOut);

  val = MMA1_MeasureGetRawZ();
  buf[0] = '\0';
  UTIL1_strcatNum16Hex(buf, sizeof(buf), (uint16_t)val);
  CLS1_SendStr(buf, io->stdOut);
  CLS1_SendStr((unsigned char*)" (", io->stdOut);
  CLS1_SendNum16s((int16_t)val, io->stdOut);
  CLS1_SendStr((unsigned char*)")\r\n", io->stdOut);

  CLS1_SendStatusStr((unsigned char*)"  calibOffset : ", (unsigned char*)"", io->stdOut);
  CLS1_SendNum16s(MMA1_GetXOffset(), io->stdOut);
  CLS1_SendStr((unsigned char*)" ", io->stdOut);
  CLS1_SendNum16s(MMA1_GetYOffset(), io->stdOut);
  CLS1_SendStr((unsigned char*)" ", io->stdOut);
  CLS1_SendNum16s(MMA1_GetZOffset(), io->stdOut);
  CLS1_SendStr((unsigned char*)"\r\n", io->stdOut);

  CLS1_SendStatusStr((unsigned char*)"  calib 1g : ", (unsigned char*)"", io->stdOut);
  CLS1_SendNum16s(MMA1_GetX1gValue(), io->stdOut);
  CLS1_SendStr((unsigned char*)" ", io->stdOut);
  CLS1_SendNum16s(MMA1_GetY1gValue(), io->stdOut);
  CLS1_SendStr((unsigned char*)" ", io->stdOut);
  CLS1_SendNum16s(MMA1_GetZ1gValue(), io->stdOut);
  CLS1_SendStr((unsigned char*)"\r\n", io->stdOut);

  CLS1_SendStatusStr((unsigned char*)"  GetX,Y,Z : ", (unsigned char*)"", io->stdOut);
  CLS1_SendNum16s(MMA1_GetX(), io->stdOut);
  CLS1_SendStr((unsigned char*)" ", io->stdOut);
  CLS1_SendNum16s(MMA1_GetY(), io->stdOut);
  CLS1_SendStr((unsigned char*)" ", io->stdOut);
  CLS1_SendNum16s(MMA1_GetZ(), io->stdOut);
  CLS1_SendStr((unsigned char*)" (raw+offset)\r\n", io->stdOut);

  CLS1_SendStatusStr((unsigned char*)"  mg X,Y,Z : ", (unsigned char*)"", io->stdOut);
  CLS1_SendNum16s(MMA1_GetXmg(), io->stdOut);
  CLS1_SendStr((unsigned char*)" ", io->stdOut);
  CLS1_SendNum16s(MMA1_GetYmg(), io->stdOut);
  CLS1_SendStr((unsigned char*)" ", io->stdOut);
  CLS1_SendNum16s(MMA1_GetZmg(), io->stdOut);
  CLS1_SendStr((unsigned char*)" (milli-g)\r\n", io->stdOut);

  if (MMA1_GetTemperature(&temperature)==ERR_OK) {
    UTIL1_Num8sToStr(buf, sizeof(buf), temperature);
    UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"°C (offset ");
    UTIL1_strcatNum8s(buf, sizeof(buf), MMA1_DIE_TEMP_OFFSET);
    UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"°C)\r\n");
  } else {
    UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"FAILED\r\n");
  }
  CLS1_SendStatusStr((unsigned char*)"  Temperature : ", buf, io->stdOut);

  if (MMA1_WhoAmI(&val8)==ERR_OK) {
    UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"0x");
    UTIL1_strcatNum8Hex(buf, sizeof(buf), val8);
    UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  } else {
    UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"FAILED\r\n");
  }
  CLS1_SendStatusStr((unsigned char*)"  Who am I : ", buf, io->stdOut);

  CLS1_SendStatusStr((unsigned char*)"  GetMagX,Y,Z : ", (unsigned char*)"", io->stdOut);
  val16s = 0;
  (void)MMA1_GetMagX(&val16s);
  CLS1_SendNum16s(val16s, io->stdOut);
  CLS1_SendStr((unsigned char*)" ", io->stdOut);
  (void)MMA1_GetMagY(&val16s);
  CLS1_SendNum16s(val16s, io->stdOut);
  CLS1_SendStr((unsigned char*)" ", io->stdOut);
  (void)MMA1_GetMagZ(&val16s);
  CLS1_SendNum16s(val16s, io->stdOut);
  CLS1_SendStr((unsigned char*)"\r\n", io->stdOut);

  return ERR_OK;
}

static uint8_t PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"MMA1", (unsigned char*)"Group of MMA1 commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Print help or status information\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  calibrate x|y|z", (unsigned char*)"Performs accelerometer calibration\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  swreset", (unsigned char*)"Performs a software reset\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  enable|disable", (unsigned char*)"Enables or disables the sensor\r\n", io->stdOut);
  return ERR_OK;
}


uint8_t ACCEL_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io)
{
  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, "MMA1 help")==0) {
    *handled = TRUE;
    return PrintHelp(io);
  } else if ((UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0) || (UTIL1_strcmp((char*)cmd, "MMA1 status")==0)) {
    *handled = TRUE;
    return PrintStatus(io);
  } else if (UTIL1_strcmp((char*)cmd, (char*)"MMA1 calibrate x")==0) {
    MMA1_CalibrateX1g();
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"MMA1 calibrate y")==0) {
    MMA1_CalibrateY1g();
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"MMA1 calibrate z")==0) {
    MMA1_CalibrateZ1g();
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"MMA1 enable")==0) {
    MMA1_Enable();
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"MMA1 disable")==0) {
    MMA1_Disable();
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"MMA1 swreset")==0) {
    *handled = TRUE;
    if (MMA1_SwReset()!=ERR_OK) {
      CLS1_SendStr((unsigned char*)"SW reset failed!\r\n", io->stdErr);
      return ERR_FAILED;
    }
  }
  return ERR_OK;
}
#endif

#endif /* PL_HAS_ACCEL */

