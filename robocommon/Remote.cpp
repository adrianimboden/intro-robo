/**
 * \file
 * \brief Radio Remote Module
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module to handle accelerometer values passed over the Radio.
 */

#include "Platform.h" /* interface to the platform */
#if PL_HAS_REMOTE
#include "Remote.h"
#include "FRTOS1.h"
//#include "CLS1.h"
#include "UTIL1.h"
//#include "Shell.h"
#if PL_HAS_ACCEL
  #include "Accel.h"
#endif
#if PL_HAS_PID
  #include "PID.h"
#endif
#if PL_HAS_MOTOR
  #include "Motor.h"
#endif
#if PL_HAS_RADIO
  #include "RNet_App.h"
  #include "RNet_AppConfig.h"
#endif
#if PL_HAS_DRIVE
  #include "Drive.h"
#endif
#if PL_HAS_LED
  #include "LED.h"
#endif
#if PL_HAS_JOYSTICK
  #include "AD1.h"
#endif

static bool REMOTE_isOn = FALSE;
static bool REMOTE_isVerbose = FALSE;
static bool REMOTE_useJoystick = TRUE;
static bool REMOTE_useAccelerometer = FALSE;
#if PL_HAS_JOYSTICK
static uint16_t midPointX, midPointY;
#endif

#if PL_APP_ACCEL_CONTROL_SENDER
static int8_t ToSigned8Bit(uint16_t val, bool isX) {
  int32_t tmp;

  if (isX) {
    tmp = (int32_t)val-midPointX;
  } else {
    tmp = (int32_t)val-midPointY;
  }
  if (tmp>0) {
    tmp = (tmp*128)/0x7fff;
  } else {
    tmp = (-tmp*128)/0x7fff;
    tmp = -tmp;
  }
  if (tmp<-128) {
    tmp = -128;
  } else if (tmp>127) {
    tmp = 127;
  }
  return (int8_t)tmp;
}

static uint8_t APP_GetXY(uint16_t *x, uint16_t *y, int8_t *x8, int8_t *y8) {
  uint8_t res;
  uint16_t values[2];

  res = AD1_Measure(TRUE);
  if (res!=ERR_OK) {
    return res;
  }
  res = AD1_GetValue16(&values[0]);
  if (res!=ERR_OK) {
    return res;
  }
  if (x!=NULL) {
    *x = values[0];
  }
  if (y!=NULL) {
    *y = values[1];
  }
  /* transform into -128...127 with zero as mid position */
  if (x8!=NULL) {
    *x8 = ToSigned8Bit(values[0], TRUE);
  }
  if (y8!=NULL) {
    *y8 = ToSigned8Bit(values[1], FALSE);
  }
  return ERR_OK;
}

static portTASK_FUNCTION(RemoteTask, pvParameters) {
  (void)pvParameters;
#if PL_HAS_JOYSTICK
  (void)APP_GetXY(&midPointX, &midPointY, NULL, NULL);
#endif
  FRTOS1_vTaskDelay(1000/portTICK_RATE_MS);
  for(;;) {
    if (REMOTE_isOn) {
#if PL_HAS_ACCEL
      if (REMOTE_useAccelerometer) {
        uint8_t buf[6];
        int16_t x, y, z;

        /* send periodically accelerometer messages */
        ACCEL_GetValues(&x, &y, &z);
        buf[0] = (uint8_t)(x&0xFF);
        buf[1] = (uint8_t)(x>>8);
        buf[2] = (uint8_t)(y&0xFF);
        buf[3] = (uint8_t)(y>>8);
        buf[4] = (uint8_t)(z&0xFF);
        buf[5] = (uint8_t)(z>>8);
        if (REMOTE_isVerbose) {
          uint8_t txtBuf[48];

          UTIL1_strcpy(txtBuf, sizeof(txtBuf), (unsigned char*)"TX: x: ");
          UTIL1_strcatNum16s(txtBuf, sizeof(txtBuf), x);
          UTIL1_strcat(txtBuf, sizeof(txtBuf), (unsigned char*)" y: ");
          UTIL1_strcatNum16s(txtBuf, sizeof(txtBuf), y);
          UTIL1_strcat(txtBuf, sizeof(txtBuf), (unsigned char*)" z: ");
          UTIL1_strcatNum16s(txtBuf, sizeof(txtBuf), z);
          UTIL1_strcat(txtBuf, sizeof(txtBuf), (unsigned char*)" to addr 0x");
    #if RNWK_SHORT_ADDR_SIZE==1
          UTIL1_strcatNum8Hex(txtBuf, sizeof(txtBuf), RNETA_GetDestAddr());
    #else
          UTIL1_strcatNum16Hex(txtBuf, sizeof(txtBuf), RNETA_GetDestAddr());
    #endif
          UTIL1_strcat(txtBuf, sizeof(txtBuf), (unsigned char*)"\r\n");
          SHELL_SendString(txtBuf);
        }
        (void)RAPP_SendPayloadDataBlock(buf, sizeof(buf), RAPP_MSG_TYPE_ACCEL, RNETA_GetDestAddr(), RPHY_PACKET_FLAGS_REQ_ACK);
        LED1_Neg();
      }
#endif
#if PL_HAS_JOYSTICK
      if (REMOTE_useJoystick) {
        uint8_t buf[2];
        uint16_t x = 0;
        uint16_t y = 0;
        int8_t x8 = 0;
        int8_t y8 = 0;

        /* send periodically accelerometer messages */
        APP_GetXY(&x, &y, &x8, &y8);
        buf[0] = x8;
        buf[1] = y8;
        if (REMOTE_isVerbose) {
          uint8_t txtBuf[48];

          UTIL1_strcpy(txtBuf, sizeof(txtBuf), (unsigned char*)"TX: x: ");
          UTIL1_strcatNum8s(txtBuf, sizeof(txtBuf), x8);
          UTIL1_strcat(txtBuf, sizeof(txtBuf), (unsigned char*)" y: ");
          UTIL1_strcatNum8s(txtBuf, sizeof(txtBuf), y8);
          UTIL1_strcat(txtBuf, sizeof(txtBuf), (unsigned char*)" to addr 0x");
    #if RNWK_SHORT_ADDR_SIZE==1
          UTIL1_strcatNum8Hex(txtBuf, sizeof(txtBuf), RNETA_GetDestAddr());
    #else
          UTIL1_strcatNum16Hex(txtBuf, sizeof(txtBuf), RNETA_GetDestAddr());
    #endif
          UTIL1_strcat(txtBuf, sizeof(txtBuf), (unsigned char*)"\r\n");
          //SHELL_SendString(txtBuf);
        }
        //(void)RAPP_SendPayloadDataBlock(buf, sizeof(buf), RAPP_MSG_TYPE_JOYSTICK_XY, RNETA_GetDestAddr(), RPHY_PACKET_FLAGS_REQ_ACK);
        (void)RAPP_SendPayloadDataBlock(buf, sizeof(buf), RAPP_MSG_TYPE_JOYSTICK_XY, REMOTE_ADDR_ROBO, RPHY_PACKET_FLAGS_REQ_ACK);
        LED1_Neg();
      }
#endif
      FRTOS1_vTaskDelay(200/portTICK_RATE_MS);
    } else {
      FRTOS1_vTaskDelay(1000/portTICK_RATE_MS);
    }
  } /* for */
}
#endif

#if PL_HAS_MOTOR
static void REMOTE_HandleMotorMsg(int16_t speedVal, int16_t directionVal, int16_t z) {
  #define SCALE_DOWN 30
  #define MIN_VALUE  250 /* values below this value are ignored */
  #define DRIVE_DOWN 1

  if (!REMOTE_isOn) {
    return;
  }
  if (z<-900) { /* have a way to stop motor: turn FRDM USB port side up or down */
#if PL_HAS_DRIVE
    DRV_SetSpeed(0, 0);
#else
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
#endif
  } else if ((directionVal>MIN_VALUE || directionVal<-MIN_VALUE) && (speedVal>MIN_VALUE || speedVal<-MIN_VALUE)) {
    int16_t speed, speedL, speedR;
    
    speed = speedVal/SCALE_DOWN;
    if (directionVal<0) {
      if (speed<0) {
        speedR = speed+(directionVal/SCALE_DOWN);
      } else {
        speedR = speed-(directionVal/SCALE_DOWN);
      }
      speedL = speed;
    } else {
      speedR = speed;
      if (speed<0) {
        speedL = speed-(directionVal/SCALE_DOWN);
      } else {
        speedL = speed+(directionVal/SCALE_DOWN);
      }
    }
#if PL_HAS_DRIVE
    DRV_SetSpeed(speedL*SCALE_DOWN/DRIVE_DOWN, speedR*SCALE_DOWN/DRIVE_DOWN);
#else
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), speedL);
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), speedR);
#endif
  } else if (speedVal>100 || speedVal<-100) { /* speed */
#if PL_HAS_DRIVE
    DRV_SetSpeed(speedVal, speedVal);
#else
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), -speedVal/SCALE_DOWN);
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), -speedVal/SCALE_DOWN);
#endif
  } else if (directionVal>100 || directionVal<-100) { /* direction */
#if PL_HAS_DRIVE
    DRV_SetSpeed(directionVal/DRIVE_DOWN, -directionVal/DRIVE_DOWN);
#else
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), -directionVal/SCALE_DOWN);
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), (directionVAl/SCALE_DOWN));
#endif
  } else { /* device flat on the table? */
#if PL_HAS_DRIVE
    DRV_SetSpeed(0, 0);
#else
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
#endif
  }
}
#endif

#if PL_HAS_MOTOR
static int16_t scaleJoystickTo1K(int8_t val) {
  /* map speed from -128...127 to -1000...+1000 */
  int tmp;

  if (val>0) {
    tmp = ((val*10)/127)*100;
  } else {
    tmp = ((val*10)/128)*100;
  }
  if (tmp<-1000) {
    tmp = -1000;
  } else if (tmp>1000) {
    tmp = 1000;
  }
  return tmp;
}

static int16_t scaleJoystickTo4K(int8_t val) {
  /* map speed from -128...127 to -4000...+4000 */
  int tmp;

  if (val>0) {
    tmp = ((val*10)/127)*400;
  } else {
    tmp = ((val*10)/128)*400;
  }
  if (tmp<-4000) {
    tmp = -4000;
  } else if (tmp>4000) {
    tmp = 4000;
  }
  return tmp;
}
#endif

uint8_t REMOTE_HandleRemoteRxMessage(RAPP_MSG_Type type, uint8_t size, uint8_t *data, RNWK_ShortAddrType srcAddr, bool *handled, RPHY_PacketDesc *packet) {
#if PL_HAS_SHELL
#if PL_HAS_MOTOR
  uint8_t buf[48];
#endif
#endif
#if PL_HAS_MOTOR
  int16_t x, y, z;
#endif
  
  (void)size;
  (void)packet;
  switch(type) {
#if PL_HAS_MOTOR
    case RAPP_MSG_TYPE_ACCEL: /* Rx of message, <type><size><data> where data is x,y,z (each 16bit) */
      *handled = TRUE;
      /* get data value */
      x = (data[0])|(data[1]<<8);
      y = (data[2])|(data[3]<<8);
      z = (data[4])|(data[5]<<8);
      if (REMOTE_isVerbose) {
        UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"RX: x: ");
        UTIL1_strcatNum16s(buf, sizeof(buf), x);
        UTIL1_strcat(buf, sizeof(buf), (unsigned char*)" y: ");
        UTIL1_strcatNum16s(buf, sizeof(buf), y);
        UTIL1_strcat(buf, sizeof(buf), (unsigned char*)" z: ");
        UTIL1_strcatNum16s(buf, sizeof(buf), z);
        UTIL1_strcat(buf, sizeof(buf), (unsigned char*)" from addr 0x");
  #if RNWK_SHORT_ADDR_SIZE==1
        UTIL1_strcatNum8Hex(buf, sizeof(buf), srcAddr);
  #else
        UTIL1_strcatNum16Hex(buf, sizeof(buf), srcAddr);
  #endif
        UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
        //SHELL_SendString(buf);
      }
#if PL_HAS_MOTOR
      if (REMOTE_useAccelerometer) {
        REMOTE_HandleMotorMsg(y, x, z);
      }
#endif
      return ERR_OK;
#endif

#if PL_HAS_MOTOR
    case RAPP_MSG_TYPE_JOYSTICK_XY: /* values are -128...127 */
      {
        int8_t x, y;
        int16_t x1000, y1000;

        *handled = TRUE;
        x = *data; /* get x data value */
        y = *(data+1); /* get y data value */
        if (REMOTE_isVerbose) {
          UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"x/y: ");
          UTIL1_strcatNum8s(buf, sizeof(buf), (int8_t)x);
          UTIL1_chcat(buf, sizeof(buf), ',');
          UTIL1_strcatNum8s(buf, sizeof(buf), (int8_t)y);
          UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
          //SHELL_SendString(buf);
        }
  #if 0 /* using shell command */
        UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"motor L duty ");
        UTIL1_strcatNum8s(buf, sizeof(buf), scaleSpeedToPercent(x));
        SHELL_ParseCmd(buf);
        UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"motor R duty ");
        UTIL1_strcatNum8s(buf, sizeof(buf), scaleSpeedToPercent(y));
        SHELL_ParseCmd(buf);
  #endif
        /* filter noise around zero */
        if (x>-5 && x<5) {
          x = 0;
        }
        if (y>-5 && y<5) {
          y = 0;
        }
        //x1000 = scaleJoystickTo1K(x);
        x1000 = scaleJoystickTo4K(x);
        //y1000 = scaleJoystickTo1K(y);
        y1000 = scaleJoystickTo4K(y);
        if (REMOTE_useJoystick) {
          REMOTE_HandleMotorMsg(y1000, x1000, 0); /* first param is forward/backward speed, second param is direction */
        }
      }
      break;
#endif

    default:
      break;
  } /* switch */
  return ERR_OK;
}

#if PL_HAS_JOYSTICK
static void StatusPrintXY(CLS1_ConstStdIOType *io) {
  uint16_t x, y;
  int8_t x8, y8;
  uint8_t buf[64];

  if (APP_GetXY(&x, &y, &x8, &y8)==ERR_OK) {
    UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"X: 0x");
    UTIL1_strcatNum16Hex(buf, sizeof(buf), x);
    UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"(");
    UTIL1_strcatNum8s(buf, sizeof(buf), x8);
    UTIL1_strcat(buf, sizeof(buf), (unsigned char*)") Y: 0x");
    UTIL1_strcatNum16Hex(buf, sizeof(buf), y);
    UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"(");
    UTIL1_strcatNum8s(buf, sizeof(buf), y8);
    UTIL1_strcat(buf, sizeof(buf), (unsigned char*)")\r\n");
  } else {
    UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"GetXY() failed!\r\n");
  }
  CLS1_SendStatusStr((unsigned char*)"  analog", buf, io->stdOut);
}
#endif

static void REMOTE_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"remote", (unsigned char*)"Group of remote commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows remote help or status\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  on|off", (unsigned char*)"Turns the remote on or off\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  verbose on|off", (unsigned char*)"Turns the verbose mode on or off\r\n", io->stdOut);
#if PL_HAS_JOYSTICK
  CLS1_SendHelpStr((unsigned char*)"  accel on|off", (unsigned char*)"Use accelerometer\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  joystick on|off", (unsigned char*)"Use joystick\r\n", io->stdOut);
#endif
}

static void REMOTE_PrintStatus(const CLS1_StdIOType *io) {
  CLS1_SendStatusStr((unsigned char*)"remote", (unsigned char*)"\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  remote ", REMOTE_isOn?(unsigned char*)"on\r\n":(unsigned char*)"off\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  accel ", REMOTE_useAccelerometer?(unsigned char*)"on\r\n":(unsigned char*)"off\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  joystick ", REMOTE_useJoystick?(unsigned char*)"on\r\n":(unsigned char*)"off\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  verbose ", REMOTE_isVerbose?(unsigned char*)"on\r\n":(unsigned char*)"off\r\n", io->stdOut);
#if PL_HAS_JOYSTICK
  StatusPrintXY(io);
#endif
}

uint8_t REMOTE_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
  uint8_t res = ERR_OK;

  if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, (char*)"remote help")==0) {
    REMOTE_PrintHelp(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, (char*)"remote status")==0) {
    REMOTE_PrintStatus(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote on")==0) {
    REMOTE_isOn = TRUE;
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote off")==0) {
#if PL_HAS_MOTOR
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
#endif
    REMOTE_isOn = FALSE;
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote verbose on")==0) {
    REMOTE_isVerbose = TRUE;
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote verbose off")==0) {
    REMOTE_isVerbose = FALSE;
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote accel on")==0) {
    REMOTE_useAccelerometer = TRUE;
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote accel off")==0) {
    REMOTE_useAccelerometer = FALSE;
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote joystick on")==0) {
    REMOTE_useJoystick = TRUE;
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote joystick off")==0) {
    REMOTE_useJoystick = FALSE;
    *handled = TRUE;
  }
  return res;
}

bool REMOTE_GetOnOff(void) {
  return REMOTE_isOn;
}

void REMOTE_SetOnOff(bool on) {
  REMOTE_isOn = on;
}

void REMOTE_Deinit(void) {
  /* nothing to do */
}

/*! \brief Initializes module */
void REMOTE_Init(void) {
  REMOTE_isOn = TRUE;
  REMOTE_isVerbose = FALSE;
  REMOTE_useJoystick = TRUE;
#if PL_IS_ROBO
  REMOTE_useAccelerometer = TRUE;
  (void)RNWK_SetThisNodeAddr(REMOTE_ADDR_ROBO);
#else
  REMOTE_useAccelerometer = FALSE;
  (void)RNWK_SetThisNodeAddr(REMOTE_ADDR_CONTROL);
#endif
#if PL_APP_ACCEL_CONTROL_SENDER
  if (FRTOS1_xTaskCreate(RemoteTask, "Remote", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, NULL) != pdPASS) {
    for(;;){} /* error */
  }
#endif
}
#endif /* PL_HAS_REMOTE */

