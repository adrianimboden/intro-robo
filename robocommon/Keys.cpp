/**
 * \file
 * \brief Key/Switch driver implementation.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements a generic keyboard driver for up to 7 Keys.
 * It is using macros for maximum flexibility with minimal code overhead.
 */

#include "Platform.h"
#if PL_HAS_KEYS
  #include "Keys.h"
#if PL_HAS_EVENT
  #include "Event.h"
#endif
#if PL_HAS_DEBOUNCE
  #include "KeyDebounce.h"
#endif

void KEY_Scan(void) {
#if PL_HAS_DEBOUNCE
  KEYDBNC_Process();
#else
  /*! \todo check handling all keys */
#if PL_NOF_KEYS >= 1 && PL_KEY_POLLED_KEY1
  if (KEY1_Get()) { /* key pressed */
	  eventQueue.setEvent(Event::Sw1Pressed);
  }
#endif
#if PL_NOF_KEYS >= 2 && PL_KEY_POLLED_KEY2
  if (KEY2_Get()) { /* key pressed */
	  eventQueue.setEvent(Event::Sw2Pressed);
  }
#endif
#if PL_NOF_KEYS >= 3 && PL_KEY_POLLED_KEY3
  if (KEY3_Get()) { /* key pressed */
	  eventQueue.setEvent(Event::Sw3Pressed);
  }
#endif
#if PL_NOF_KEYS >= 4 && PL_KEY_POLLED_KEY4
  if (KEY4_Get()) { /* key pressed */
	  eventQueue.setEvent(Event::Sw4Pressed);
  }
#endif
#if PL_NOF_KEYS >= 5 && PL_KEY_POLLED_KEY5
  if (KEY5_Get()) { /* key pressed */
	  eventQueue.setEvent(Event::Sw5Pressed);
  }
#endif
#if PL_NOF_KEYS >= 6 && PL_KEY_POLLED_KEY6
  if (KEY6_Get()) { /* key pressed */
	  eventQueue.setEvent(Event::Sw6Pressed);
  }
#endif
#if PL_NOF_KEYS >= 7 && PL_KEY_POLLED_KEY7
  if (KEY7_Get()) { /* key pressed */
	  eventQueue.setEvent(Event::Sw7Pressed);
  }
#endif
#endif
}

#if PL_HAS_KBI
void KEY_OnInterrupt(KEY_Buttons button) {
#if PL_HAS_DEBOUNCE
  KEYDBNC_Process();
#else
  switch(button) {
#if PL_NOF_KEYS >= 1
    case KEY_BTN1: eventQueue.setEvent(Event::Sw1Pressed); break;
#endif
#if PL_NOF_KEYS >= 2
    case KEY_BTN2: eventQueue.setEvent(Event::Sw2Pressed); break;
#endif
#if PL_NOF_KEYS >= 3
    case KEY_BTN3: eventQueue.setEvent(Event::Sw3Pressed); break;
#endif
#if PL_NOF_KEYS >= 4
    case KEY_BTN4: eventQueue.setEvent(Event::Sw4Pressed); break;
#endif
#if PL_NOF_KEYS >= 5
    case KEY_BTN5: eventQueue.setEvent(Event::Sw5Pressed); break;
#endif
#if PL_NOF_KEYS >= 6
    case KEY_BTN6: eventQueue.setEvent(Event::Sw6Pressed); break;
#endif
#if PL_NOF_KEYS >= 6
    case KEY_BTN7: eventQueue.setEvent(Event::Sw7Pressed); break;
#endif
    default:
      /* unknown? */
      break;
  } /* switch */
#endif
}

void KEY_EnableInterrupts(void) {
#if PL_NOF_KEYS >= 1 && !PL_KEY_POLLED_KEY1
  SW1_Enable();
#endif
#if PL_NOF_KEYS >= 2 && !PL_KEY_POLLED_KEY2
  SW2_Enable();
#endif
#if PL_NOF_KEYS >= 3 && !PL_KEY_POLLED_KEY3
  SW3_Enable();
#if PL_HAS_KBI_NMI
  /* necessary to clear interrupts on Port A */
  PORT_PDD_ClearPinInterruptFlag(PORTA_BASE_PTR, ExtIntLdd4_PIN_INDEX);
  PORT_PDD_SetPinInterruptConfiguration(PORTA_BASE_PTR, 4, PORT_PDD_DMA_ON_FALLING);
#endif
#endif
#if PL_NOF_KEYS >= 4 && !PL_KEY_POLLED_KEY4
  SW4_Enable();
#endif
#if PL_NOF_KEYS >= 5 && !PL_KEY_POLLED_KEY5
  SW5_Enable();
#endif
#if PL_NOF_KEYS >= 6 && !PL_KEY_POLLED_KEY6
  SW6_Enable();
#endif
#if PL_NOF_KEYS >= 7 && !PL_KEY_POLLED_KEY7
  SW7_Enable();
#endif
}

void KEY_DisableInterrupts(void) {
#if PL_NOF_KEYS >= 1 && !PL_KEY_POLLED_KEY1
  SW1_Disable();
#endif
#if PL_NOF_KEYS >= 2 && !PL_KEY_POLLED_KEY2
  SW2_Disable();
#endif
#if PL_NOF_KEYS >= 3 && !PL_KEY_POLLED_KEY3
  SW3_Disable();
#if PL_HAS_KBI_NMI
  PORT_PDD_SetPinInterruptConfiguration(PORTA_BASE_PTR, 4, PORT_PDD_INTERRUPT_DMA_DISABLED);
#endif
#endif
#if PL_NOF_KEYS >= 4 && !PL_KEY_POLLED_KEY4
  SW4_Disable();
#endif
#if PL_NOF_KEYS >= 5 && !PL_KEY_POLLED_KEY5
  SW5_Disable();
#endif
#if PL_NOF_KEYS >= 6 && !PL_KEY_POLLED_KEY6
  SW6_Disable();
#endif
#if PL_NOF_KEYS >= 7 && !PL_KEY_POLLED_KEY7
  SW7_Disable();
#endif
}

#ifdef __cplusplus
extern "C" {
#endif

void PORTA_OnInterrupt(void) {
  void Cpu_ivINT_PORTA(void); /* prototype of ISR in Cpu.c */

  Cpu_ivINT_PORTA(); /* call interrupt handler created by the ExtInt components */
}
#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* PL_HAS_KBI */

/*! \brief Key driver initialization */
void KEY_Init(void) {
  /* nothing needed for now */
}

/*! \brief Key driver de-initialization */
void KEY_Deinit(void) {
  /* nothing needed for now */
}

#endif /* PL_HAS_KEYS */
