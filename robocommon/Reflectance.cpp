/**
 * \file
 * \brief Reflectance sensor driver implementation.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements the driver for the bot front sensor.
 */

#include "Platform.h"
#if PL_HAS_LINE_SENSOR
#include "Reflectance.h"
#include "LED_IR.h"
#include "WAIT1.h"
#include "RefCnt.h" /* timer counter to measure reflectance */
#include "IR1.h"
#include "IR2.h"
#include "IR3.h"
#include "IR4.h"
#include "IR5.h"
#include "IR6.h"
#include "UTIL1.h"
#include "FRTOS1.h"
#include "Event.h"
#include "RoboConsole.h"
#if PL_HAS_BUZZER
  #include "Buzzer.h"
#endif
#if PL_HAS_CONFIG_NVM
  #include "NVM_Config.h"
#endif

#define REF_NOF_SENSORS 6 /* number of sensors */
#define REF_SENSOR1_IS_LEFT   1 /* sensor number one is on the left side */
constexpr uint8_t REF_MIN_LINE_VAL = 0x60;   /* minimum value indicating a line */
constexpr uint8_t REF_MIN_NOISE_VAL = 0x40;   /* values below this are not added to the weighted sum */
#define REF_USE_WHITE_LINE    0  /* if set to 1, then the robot is using a white (on black) line, otherwise a black (on white) line */

#define REF_START_STOP_CALIB  1 /* start/stop calibration commands */
#define REF_MEASURE_TIMEOUT   1 /* use timeout for measurement */

#if REF_MEASURE_TIMEOUT
  #define REF_SENSOR_TIMEOUT_US  1500  /* after this time, consider no reflection (black). Must be smaller than the timeout period of the RefCnt timer! */
  #define REF_SENSOR_TIMOUT_VAL  ((RefCnt_CNT_INP_FREQ_U_0/1000)*REF_SENSOR_TIMEOUT_US)/1000
#endif

#if REF_START_STOP_CALIB
  //static xSemaphoreHandle REF_StartStopSem = NULL;
#endif


typedef enum {
  REF_STATE_INIT,
  REF_STATE_NOT_CALIBRATED,
  REF_STATE_START_CALIBRATION,
  REF_STATE_CALIBRATING,
  REF_STATE_STOP_CALIBRATION,
  REF_STATE_READY
} RefStateType;
static volatile RefStateType refState = REF_STATE_INIT; /* state machine state */

static LDD_TDeviceData *timerHandle;

typedef struct SensorFctType_ {
  void (*SetOutput)(void);
  void (*SetInput)(void);
  void (*SetVal)(void);
  bool (*GetVal)(void);
} SensorFctType;

typedef uint16_t SensorTimeType;
#define MAX_SENSOR_VALUE  ((SensorTimeType)-1)

/* calibration min/max values */
typedef struct SensorCalibT_ {
  SensorTimeType minVal[REF_NOF_SENSORS];
  SensorTimeType maxVal[REF_NOF_SENSORS];
} SensorCalibT;

static int16_t refCenterLineVal=0; /* 0 means no line, >0 means line is below sensor 0, 1000 below sensor 1 and so on */
static SensorCalibT SensorCalibMinMax; /* min/max calibration data in SRAM */
static SensorTimeType SensorRaw[REF_NOF_SENSORS]; /* raw sensor values */
static SensorTimeType SensorCalibrated[REF_NOF_SENSORS]; /* 0 means white/min value, 1000 means black/max value */

/* Functions as wrapper around macro. */
static void S1_SetOutput(void) { IR1_SetOutput(); }
static void S1_SetInput(void) { IR1_SetInput(); }
static void S1_SetVal(void) { IR1_SetVal(); }
static bool S1_GetVal(void) { return IR1_GetVal(); }

static void S2_SetOutput(void) { IR2_SetOutput(); }
static void S2_SetInput(void) { IR2_SetInput(); }
static void S2_SetVal(void) { IR2_SetVal(); }
static bool S2_GetVal(void) { return IR2_GetVal(); }

static void S3_SetOutput(void) { IR3_SetOutput(); }
static void S3_SetInput(void) { IR3_SetInput(); }
static void S3_SetVal(void) { IR3_SetVal(); }
static bool S3_GetVal(void) { return IR3_GetVal(); }

static void S4_SetOutput(void) { IR4_SetOutput(); }
static void S4_SetInput(void) { IR4_SetInput(); }
static void S4_SetVal(void) { IR4_SetVal(); }
static bool S4_GetVal(void) { return IR4_GetVal(); }

static void S5_SetOutput(void) { IR5_SetOutput(); }
static void S5_SetInput(void) { IR5_SetInput(); }
static void S5_SetVal(void) { IR5_SetVal(); }
static bool S5_GetVal(void) { return IR5_GetVal(); }

static void S6_SetOutput(void) { IR6_SetOutput(); }
static void S6_SetInput(void) { IR6_SetInput(); }
static void S6_SetVal(void) { IR6_SetVal(); }
static bool S6_GetVal(void) { return IR6_GetVal(); }

static const SensorFctType SensorFctArray[REF_NOF_SENSORS] = {
  {S1_SetOutput, S1_SetInput, S1_SetVal, S1_GetVal},
  {S2_SetOutput, S2_SetInput, S2_SetVal, S2_GetVal},
  {S3_SetOutput, S3_SetInput, S3_SetVal, S3_GetVal},
  {S4_SetOutput, S4_SetInput, S4_SetVal, S4_GetVal},
  {S5_SetOutput, S5_SetInput, S5_SetVal, S5_GetVal},
  {S6_SetOutput, S6_SetInput, S6_SetVal, S6_GetVal},
};

#if REF_START_STOP_CALIB
void REF_CalibrateStartStop(void) {
  if (refState==REF_STATE_NOT_CALIBRATED || refState==REF_STATE_CALIBRATING || refState==REF_STATE_READY) {
    //(void)xSemaphoreGive(REF_StartStopSem);
  }
}
#endif

/*!
 * \brief Measures the time until the sensor discharges
 * \param raw Array to store the raw values.
 * \return ERR_OVERFLOW if there is a timeout, ERR_OK otherwise
 */
static void REF_MeasureRaw(SensorTimeType raw[REF_NOF_SENSORS]) {
  uint8_t cnt; /* number of sensor */
  uint8_t i;
  RefCnt_TValueType timerVal;

  LED_IR_On(); /* IR LED's on */
  WAIT1_Waitus(200); /*! \todo adjust time as needed */

  for(i=0;i<REF_NOF_SENSORS;i++) {
    SensorFctArray[i].SetOutput(); /* turn I/O line as output */
    SensorFctArray[i].SetVal(); /* put high */
    raw[i] = MAX_SENSOR_VALUE;
  }
  WAIT1_Waitus(50); /* give some time to charge the capacitor */
  (void)RefCnt_ResetCounter(timerHandle); /* reset timer counter */
  for(i=0;i<REF_NOF_SENSORS;i++) {
    SensorFctArray[i].SetInput(); /* turn I/O line as input */
  }
  (void)RefCnt_ResetCounter(timerHandle); /* reset timer counter */
  do {
    cnt = 0;
    timerVal = RefCnt_GetCounterValue(timerHandle);
#if REF_MEASURE_TIMEOUT
    if (timerVal>REF_SENSOR_TIMOUT_VAL) {
      break; /* get out of while loop */
    }
#endif
    for(i=0;i<REF_NOF_SENSORS;i++) {
      if (raw[i]==MAX_SENSOR_VALUE) { /* not measured yet? */
        if (SensorFctArray[i].GetVal()==0) {
          raw[i] = RefCnt_GetCounterValue(timerHandle);
        }
      } else { /* have value */
        cnt++;
      }
    }
  } while(cnt!=REF_NOF_SENSORS);
  LED_IR_Off();
}

static void REF_CalibrateMinMax(SensorTimeType min[REF_NOF_SENSORS], SensorTimeType max[REF_NOF_SENSORS], SensorTimeType raw[REF_NOF_SENSORS]) {
  int i;
  
  REF_MeasureRaw(raw);
  for(i=0;i<REF_NOF_SENSORS;i++) {
    if (raw[i] < min[i]) {
      min[i] = raw[i];
    }
    if (raw[i]> max[i]) {
      max[i] = raw[i];
    }
  }
}

static void ReadCalibrated(SensorTimeType calib[REF_NOF_SENSORS], SensorTimeType raw[REF_NOF_SENSORS]) {
  int i;
  int32_t x, denominator;
  REF_MeasureRaw(raw);
  for(i=0;i<REF_NOF_SENSORS;i++) {
    x = 0;
    denominator = SensorCalibMinMax.maxVal[i]-SensorCalibMinMax.minVal[i];
    if (denominator!=0) {
      x = (((int32_t)raw[i]-SensorCalibMinMax.minVal[i])*1000)/denominator;
    }
    if (x<0) {
      x = 0;
    } else if (x>1000) {
      x = 1000;
    }
    calib[i] = x;
  }
}

/*
 * Operates the same as read calibrated, but also returns an
 * estimated position of the robot with respect to a line. The
 * estimate is made using a weighted average of the sensor indices
 * multiplied by 1000, so that a return value of 1000 indicates that
 * the line is directly below sensor 0, a return value of 2000
 * indicates that the line is directly below sensor 1, 2000
 * indicates that it's below sensor 2000, etc. Intermediate
 * values indicate that the line is between two sensors. The
 * formula is:
 *
 * 1000*value0 + 2000*value1 + 3000*value2 + ...
 * --------------------------------------------
 * value0 + value1 + value2 + ...
 *
 * By default, this function assumes a dark line (high values)
 * surrounded by white (low values). If your line is light on
 * black, set the optional second argument white_line to true. In
 * this case, each sensor value will be replaced by (1000-value)
 * before the averaging.
 */
static int ReadLine(SensorTimeType calib[REF_NOF_SENSORS], SensorTimeType raw[REF_NOF_SENSORS], bool white_line) {
  int i;
  unsigned long avg; /* this is for the weighted total, which is long */
  /* before division */
  unsigned int sum; /* this is for the denominator which is <= 64000 */
  unsigned int mul; /* multiplication factor, 0, 1000, 2000, 3000 ... */
  int value;

  (void)raw; /* unused */
  avg = 0;
  sum = 0;
  mul = 1000;
#if REF_SENSOR1_IS_LEFT
  for(i=0;i<REF_NOF_SENSORS;i++) {
#else
  for(i=REF_NOF_SENSORS-1;i>=0;i--) {
#endif
    value = calib[i];
    if(white_line) {
      value = 1000-value;
    }
    /* only average in values that are above a noise threshold */
    if(value > REF_MIN_NOISE_VAL) {
      avg += ((long)value)*mul;
      sum += value;
    }
    mul += 1000;
  }
  return avg/sum;
}

uint16_t REF_GetLineValue(void) {
  return refCenterLineVal;
}

static void REF_Measure(void) {
  ReadCalibrated(SensorCalibrated, SensorRaw);
  refCenterLineVal = ReadLine(SensorCalibrated, SensorRaw, REF_USE_WHITE_LINE);
}

//static uint8_t PrintHelp(const CLS1_StdIOType *io) {
//  CLS1_SendHelpStr((unsigned char*)"ref", (unsigned char*)"Group of Reflectance commands\r\n", io->stdOut);
//  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Print help or status information\r\n", io->stdOut);
//  return ERR_OK;
//}
//
static const char* REF_GetStateString(void) {
  switch (refState) {
    case REF_STATE_INIT:                return "INIT";
    case REF_STATE_NOT_CALIBRATED:      return "NOT CALIBRATED";
    case REF_STATE_START_CALIBRATION:   return "START CALIBRATION";
    case REF_STATE_CALIBRATING:         return "CALIBRATING";
    case REF_STATE_STOP_CALIBRATION:    return "STOP CALIBRATION";
    case REF_STATE_READY:               return "READY";
    default:
      break;
  } /* switch */
  return "UNKNOWN";
}


template <typename TName, typename TStr>
void writeStatus(IOStream& out, const TName& name, const TStr& val)
{
	out << "  " << name << " " << val << "\r\n";
}

template <typename TName, typename TValue>
void writeHexStatus(IOStream& out, const TName& name, TValue val)
{
	out << "  " << name << " 0x" << numberToHex(val) << "\r\n";
}

template <typename TName, typename TValues>
void writeHexValuesLine(IOStream& out, const TName& name, const TValues& values)
{
	out << "  " << name;
	for (const auto& value : values)
	{
		out << " 0x" << numberToHex(value);
	}
	out << "\r\n";
}

template <size_t Size, typename T>
std::array<T, Size> makeArray(T* src)
{
	std::array<T, Size> arr;
	for (auto i = 0u; i < Size; ++i)
	{
		arr[i] = src[i];
	}
	return arr;
}

void REF_PrintStatus(IOStream& out)
{
//  unsigned char buf[24];
//  int i;

	out << "reflectance\r\n";
	writeStatus(out,		"state           ", REF_GetStateString());
	writeHexStatus(out,		"min noise       ", REF_MIN_NOISE_VAL);
	writeHexStatus(out,		"REF_MIN_LINE_VAL", REF_MIN_LINE_VAL);
#if REF_MEASURE_TIMEOUT
	writeHexStatus(out,		"timeout         ", REF_SENSOR_TIMEOUT_US);
#endif
	writeHexStatus(out,		"line val        ", refCenterLineVal);

	writeHexValuesLine(out,	"raw val         ", makeArray<REF_NOF_SENSORS>(SensorRaw));
	writeHexValuesLine(out,	"min val         ", makeArray<REF_NOF_SENSORS>(SensorCalibMinMax.minVal));
	writeHexValuesLine(out,	"max val         ", makeArray<REF_NOF_SENSORS>(SensorCalibMinMax.maxVal));
	writeHexValuesLine(out,	"calib val       ", makeArray<REF_NOF_SENSORS>(SensorCalibrated));
}
//
//byte REF_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
//  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, "ref help")==0) {
//    *handled = TRUE;
//    return PrintHelp(io);
//  } else if ((UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0) || (UTIL1_strcmp((char*)cmd, "ref status")==0)) {
//    *handled = TRUE;
//    return PrintStatus(io);
//  }
//  return ERR_OK;
//}

static void REF_StateMachine(void) {
  int i;
  void *p;

  Console& console = getConsole();

  switch (refState) {
  	  case REF_STATE_INIT:
  		  console.getUnderlyingIoStream()->write("INFO: No calibration data present.\r\n");
  		  p = NVMC_GetReflectanceData();
  		  if(p == NULL){
  			refState = REF_STATE_NOT_CALIBRATED;
  		  }
  		  else {
  			  SensorCalibMinMax = *(SensorCalibT*)p;
  			  refState = REF_STATE_READY;
  		  }
  		  break;

  	  case REF_STATE_NOT_CALIBRATED:
  		  REF_MeasureRaw(SensorRaw);
  		  if (eventQueue.getAndResetEvent(Event::RefStartStopCalibration)) {
  			  refState = REF_STATE_START_CALIBRATION;
  		  }
  		  break;

  	  case REF_STATE_START_CALIBRATION:
  		  console.getUnderlyingIoStream()->write("start calibration...\r\n");
  		  for(i=0;i<REF_NOF_SENSORS;i++) {
  			  SensorCalibMinMax.minVal[i] = MAX_SENSOR_VALUE;
  			  SensorCalibMinMax.maxVal[i] = 0;
  			  SensorCalibrated[i] = 0;
  		  }
  		  refState = REF_STATE_CALIBRATING;
  		  break;

  	  case REF_STATE_CALIBRATING:
  		  REF_CalibrateMinMax(SensorCalibMinMax.minVal, SensorCalibMinMax.maxVal, SensorRaw);
#if PL_HAS_BUZZER
  		  (void)BUZ_Beep(300, 20);
#endif
  		  if (eventQueue.getAndResetEvent(Event::RefStartStopCalibration)) {
  			  refState = REF_STATE_STOP_CALIBRATION;
  		  }
  		  break;

  	  case REF_STATE_STOP_CALIBRATION:
  		  console.getUnderlyingIoStream()->write("...stopping calibration.\r\n");
  		  NVMC_SaveReflectanceData((void *)&SensorCalibMinMax,sizeof(SensorCalibT));
  		  refState = REF_STATE_READY;
  		  break;

  	  case REF_STATE_READY:
  		  REF_Measure();
  		  if (eventQueue.getAndResetEvent(Event::RefStartStopCalibration)) {
  			  refState = REF_STATE_START_CALIBRATION;
  		  }
  		  break;
  } /* switch */
}

static portTASK_FUNCTION(ReflTask, pvParameters) {
  (void)pvParameters; /* not used */
  for(;;) {
    REF_StateMachine();
    FRTOS1_vTaskDelay(10/portTICK_RATE_MS);
  }
}

void REF_Deinit(void) {
}

void REF_Init(void) {
  refState = REF_STATE_INIT;
  timerHandle = RefCnt_Init(NULL);
  /*! \todo You might need to adjust priority or other task settings */
  if (FRTOS1_xTaskCreate(ReflTask, "Refl", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL) != pdPASS) {
	  ASSERT(false); /* error */
  }
}
#endif /* PL_HAS_LINE_SENSOR */
