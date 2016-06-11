/**********************************************************************************************************************
File: user_app2.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app2 as a template:
 1. Copy both user_app2.c and user_app2.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app2" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp22" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP2" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app2.c file to complete the second function 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp2Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp2RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
extern volatile u32 G_u32UserAppFlags;   
/* Global state flags */
extern volatile u8 G_u8NameCounter;
extern volatile u32 G_u32UserAppFlag; 

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[];                     /* From debug.c */
extern u8 G_u8DebugScanfCharCount;
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp2_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp2_StateMachine;            /* The state machine function pointer */
static u32 UserApp2_u32Timeout;                      /* Timeout counter used across states */
static u8 au8UserInputBuffer[USER_INPUT_BUFFER_SIZE];
static u8 TopMessage[]="A3.Shang Liu";
static u8 u8myname[]="shang";
static u8 u8MYNAME[]="SHANG";
static u8 u8NameBuffer[200];
static u8 u8CharacterNumberClear[]="\r\nCharacter Count Cleared!\r\n";
static void UserApp2SM_Idle(void);           
static void UserApp2SM_FailedInit(void);
static u16 u16TimeCounter=0;
/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp2Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp2Initialize(void)
{
  /*Test comment for GitHub */
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp2_StateMachine = UserApp2SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp2_StateMachine = UserApp2SM_FailedInit;
  }
} /* end UserApp2Initialize() */
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp2RunActiveState()
Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.
Requires:
  - State machine function pointer points at current state
Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp2RunActiveState(void)
{
  UserApp2_StateMachine();
} /* end UserApp2RunActiveState */
/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/
/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserApp2SM_Idle(void)
{
  if(G_u32UserAppFlag==1)
  {
     
    LedBlink(RED,LED_8HZ); 
    LedBlink(ORANGE,LED_4HZ);
    LedBlink(YELLOW,LED_2HZ); 
    LedBlink(GREEN,LED_1HZ);
    LedBlink(CYAN,LED_1HZ);
    LedBlink(BLUE,LED_2HZ); 
    LedBlink(PURPLE,LED_4HZ); 
    LedBlink(WHITE,LED_8HZ);
    DebugPrintNumber(G_u8NameCounter);
    DebugLineFeed();
    G_u32UserAppFlag=0;
  }
  if(G_u32UserAppFlags==1)
  {
 
  u16TimeCounter++;
  if(u16TimeCounter==5000)
  {
    
    G_u32UserAppFlags=0;
    LedOff(PURPLE);
    LedOff(RED); 
    LedOff(ORANGE);
    LedOff(YELLOW); 
    LedOff(GREEN);
    LedOff(CYAN);
    LedOff(BLUE); 
    LedOff(PURPLE); 
    LedOff(WHITE);
    u16TimeCounter=0;
 
  }
  }
} /* end UserApp2SM_Idle() */
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp2SM_Error(void)          
{
  
} /* end UserApp2SM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp2SM_FailedInit(void)          
{
    
} /* end UserApp2SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
