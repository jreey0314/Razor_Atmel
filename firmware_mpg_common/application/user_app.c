/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/
/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserAppFlags;                       /* Global state flags */
volatile u8 G_u8NameCounter;
volatile u32 G_u32UserAppFlag;

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
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */
static u8 au8UserInputBuffer[USER_INPUT_BUFFER_SIZE];
static u8 TopMessage[]="Shang Liu";
static u8 u8myname[]="loves tangxiaonan";
static u8 u8MYNAME[]="LOVES TANGXIAONAN";
static u8 u8NameBuffer[200];
static u8 u8CharacterNumberClear[]="\r\nCharacter Count Cleared!\r\n";


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
Function: UserAppInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserAppInitialize(void)
{

  G_u8NameCounter=0;
  LCDCommand(LCD_CLEAR_CMD);
  LedPWM(LCD_RED, LED_PWM_100);
  LedPWM(LCD_GREEN, LED_PWM_0);
  LedPWM(LCD_BLUE, LED_PWM_100);
  LCDMessage(LINE1_START_ADDR,TopMessage); 
  /*
  for(u8 i = 0; i < USER_INPUT_BUFFER_SIZE; i++)
  {
    au8UserInputBuffer[i] = 0;
  }
 */ 
  /*Test comment for GitHub */
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp_StateMachine = UserAppSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp_StateMachine = UserAppSM_FailedInit;
  }
} /* end UserAppInitialize() */
/*----------------------------------------------------------------------------------------------------------------------
Function UserAppRunActiveState()
Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.
Requires:
  - State machine function pointer points at current state
Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserAppRunActiveState(void)
{
  UserApp_StateMachine();
} /* end UserAppRunActiveState */
/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/
/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserAppSM_Idle(void)
{
  static u8 u8NumCharsMessage[] = "\n\rCharacters in buffer: ";
  static u8 u8EmptyMessage[]="\n\rEMPTY!\n\r";
  static u8 u8BufferMessage[]   = "\n\rBuffer contents:\n\r";
  u8 u8CharCount;
  static u8 u8CharacterNumber=0;
  static u8 u8i=0;
  static u8 u8j=0;
  static u8 u8k=0;
  /* Print message with number of characters in scanf buffer */
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    LCDClearChars(LINE2_START_ADDR , 20);
    u8i=0;
  }  
   if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1); 
    DebugPrintNumber(u8CharacterNumber);
    DebugLineFeed();
  }  
    if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2); 
    u8CharacterNumber=0;
    DebugPrintf(u8CharacterNumberClear);
    DebugLineFeed();
  } 
  
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3); 
    DebugPrintf(u8NameBuffer);
    DebugLineFeed();
  } 
  
  
    /* Read the buffer and print the contents */
    u8CharCount = DebugScanf(au8UserInputBuffer);
    //au8UserInputBuffer[u8CharCount] = '\0';  
    if(u8CharCount > 0)
    {
      u8CharacterNumber++;
      if(au8UserInputBuffer[0]==u8myname[u8k]|au8UserInputBuffer[0]==u8MYNAME[u8k])
      {
        u8NameBuffer[u8j]=au8UserInputBuffer[0];
        u8j++;
        u8k++;
        if(u8k==17)
        {
          u8k=0;
          G_u8NameCounter++;
          G_u32UserAppFlags=1;
          G_u32UserAppFlag=1;
        }
      }     
      LCDMessage(LINE2_START_ADDR+u8i,au8UserInputBuffer);
      DebugLineFeed();
      
      u8i++;
      if(u8i==20)
      {
        u8i=0;
      }
      if(u8i==1)
      {
         LCDClearChars(LINE2_START_ADDR+1 , 19);
      }
    }
    
  
 
} /* end UserAppSM_Idle() */
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{
  
} /* end UserAppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
 /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
