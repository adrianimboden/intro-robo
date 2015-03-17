/**
 * \file
 * \brief Mealy Sequential Machine.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Here all the Mealy interfaces are implemented.
 *
  Following Mealy Sequential State Machine is implemented:
  \dot
  digraph mealy_graph {
      node [shape=ellipse];
      rankdir=LR;
      I    [fillcolor=lightblue,style=filled,label="init" ];
      A    [fillcolor=lightblue,style=filled,label="A:0000" ];
      B    [fillcolor=lightblue,style=filled,label="B:1000" ];
      C    [fillcolor=lightblue,style=filled,label="C:0100"];
      D    [fillcolor=lightblue,style=filled,label="D:0010"];
      E    [fillcolor=lightblue,style=filled,label="E:0001"];
      I -> A;
      A -> A [label="a/all LED off"];
      A -> B [label="b/LED1 on"];
      B -> B [label="b/LED1 on"];
      B -> C [label="a/LED2 on"];
      C -> C [label="a/LED2 on"];
      C -> D [label="b/LED3 on"];
      D -> D [label="b/LED3 on"];
      D -> E [label="a/LED4 on"];
      E -> E [label="a/LED4 on"];
      E -> B [label="b/LED1 on"];
  }
  \enddot
  This state machine will change one LED for each level change:
  \image html MealyLed.jpg
 */

#include "Platform.h" /* interface to the platform */
#if PL_HAS_MEALY
#include "Mealy.h"    /* our own interface */
#include "LED.h"      /* interface to LED driver */
#include "Keys.h"     /* interface to switch 1 */
/*!
 \brief Enumeration for the LEDs we want to emit. Note that they are encoded in bits
*/
typedef enum LED_Enum {
#if PL_NOF_LEDS>=1
  LED1=(1<<0), /*!< LED1 is on */
#endif
#if PL_NOF_LEDS>=2
  LED2=(1<<1), /*!< LED2 is on */
#endif
#if PL_NOF_LEDS>=3
  LED3=(1<<2), /*!< LED3 is on */
#endif
#if PL_NOF_LEDS>=4
  LED4=(1<<3),  /*!< LED4 is on */
#endif
  LED_NOFLEDS
} LED_Enum; /*!< state machine states */

/*!
 \brief Enumeration of all our states in the state machine
*/
typedef enum MealyState {
  A, /*!< Original/Entry state: all LEDs are off */
  B, /*!< first is on */
  C, /*!< next is on */
  D, /*!< next is on */
  E,  /*!< next is on */
  F,  /*!< next is on */
  G,  /*!< next is on */
  H,  /*!< next is on */
  I,  /*!< next is on */
  J,  /*!< next is on */
} MealyState; /*!< state machine states */

/*!
 \brief Input states for the Mealy Sequential Machine.
*/
typedef enum InputState {
  INPUT_a=0, /*!< Must be zero, used as index into ::tbl */
  INPUT_b=1  /*!< Must be one, used as index into ::tbl */
} InputState;

static MealyState state; /*!< The current state of our machine */

/*!
  \brief Array of 5 states (::A,::B,::C,::D,::E), 2 input states (::INPUT_a, ::INPUT_b) and 4 LEDs as output states.
*/
#if PL_NOF_LEDS==1
/*! \todo adopt for your number of LEDs */
const uint8_t tbl[2][2][2] = /* format: {next,output} */
/*     input a   input b */
{
/*A*/ {{A,0},    {B,LED1}},       /*!< State A: with input_a, remain in A; with input_b: go to B and turn on LED1 */
/*B*/ {{A,0},    {B,LED1}}
};
#elif PL_NOF_LEDS==2
const uint8_t tbl[3][2][2] = /* format: {next,output} */
   /*     input a   input b */
 {
   /*A*/ {{A,0},    {B,LED1}},       /*!< State A: with input_a, remain in A; with input_b: go to B and turn on LED1 */
   /*B*/ {{C,LED2}, {B,LED1}},
   /*C*/ {{C,LED2}, {B,LED1}},
 };
#elif PL_NOF_LEDS==3
const uint8_t tbl[9][2][2] = /* format: {next,output} */
   /*     input a   input b */
 {
   /*A*/ {{A,0},    {B,LED1}},       /*!< State A: with input_a, remain in A; with input_b: go to B and turn on LED1 */
   /*B*/ {{C,LED2}, {B,LED1}},
   /*C*/ {{C,LED2}, {D,LED3}},
   /*D*/ {{E,LED1}, {D,LED3}},
   /*E*/ {{E,LED1}, {F,LED2}},
   /*F*/ {{G,LED1}, {F,LED2}},
   /*G*/ {{G,LED1}, {H,LED2}},
   /*I*/ {{I,LED3}, {H,LED2}},
   /*J*/ {{I,LED3}, {B,LED1}},
 };
#elif PL_NOF_LEDS==4
const uint8_t tbl[5][2][2] = /* format: {next,output} */
   /*     input a   input b */
 {
   /*A*/ {{A,0},    {B,LED1}},       /*!< State A: with input_a, remain in A; with input_b: go to B and turn on LED1 */
   /*B*/ {{C,LED2}, {B,LED1}},
   /*C*/ {{C,LED2}, {D,LED3}},
   /*D*/ {{E,LED4}, {D,LED3}},
   /*E*/ {{E,LED4}, {B,LED1}},
 };
#endif

/*!
  \brief Method to return which input key has been pressed for the Mealy Sequential Machine.
  \return Returns ::INPUT_a or ::INPUT_b
 */
static InputState GetInput(void) {
  if (KEY1_Get()==0) { /* a, not pressed */
    return INPUT_a;
  } else { /* b, pressed */
    return INPUT_b;
  }
}

/*!
 * \brief Turn the LEDs on or off depending on the led set.
 * \param [in] set Bit set of LEDs. A one indicates to turn the LED on, a zero will turn it off.
 */
static void LEDPut(const uint8_t set) {
#if PL_NOF_LEDS>=1
  LED1_Put((set&LED1)!=0);
#endif
#if PL_NOF_LEDS>=2
  LED2_Put((set&LED2)!=0);
#endif
#if PL_NOF_LEDS>=3
  LED3_Put((set&LED3)!=0);
#endif
#if PL_NOF_LEDS>=4
  LED4_Put((set&LED4)!=0);
#endif
}

/*!
\brief Mealy Sequential Machine to test the LEDs.
       The machine has the internal states A, B C, D and E,
       input states a and b and output states O = {set of LED};
*/
void MEALY_Step(void) {
  InputState i;

  i = GetInput(); /* get input state */
  LEDPut(tbl[state][i][1]); /* output the next state */
  state = (MealyState)(tbl[state][i][0]);  /* read out next internal state */
}

/*! \brief Initializes the Mealy state machine */
void MEALY_Init(void) {
  state = A;
}

/*! \brief De-initializes the Mealy state machine */
void MEALY_Deinit(void) {
}
#endif
