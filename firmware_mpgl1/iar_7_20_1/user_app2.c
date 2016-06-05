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


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */
static u8 UserApp_au8MyName[] = "S""H""A""N""G"" ""L""I""U";
static u8 UserApp_CursorPosition;

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
  
  /***********************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************/

  LCDCommand(LCD_HOME_CMD);  
  UserApp_CursorPosition = LINE1_START_ADDR;
/* Write name and button labels */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, UserApp_au8MyName);
 
  LCDMessage(LINE2_START_ADDR, "0");

  LCDMessage(LINE2_START_ADDR + 6, "1");
  LCDMessage(LINE2_START_ADDR + 13, "2");
  LCDMessage(LINE2_END_ADDR, "3");
 
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
   /* Home the cursor */
  LCDCommand(LCD_HOME_CMD); 

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
    static bool bCursorOn=FALSE;
    /*BUTTON0 toggles the cursor on and off*/
    if(WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      if(bCursorOn)
      {
      /*Cursor is on, soturn if off*/
      LCDCommand(LCD_DISPLAY_CMD|LCD_DISPLAY_ON);
      bCursorOn=FALSE;
    }
    else
    {
      /*Cursor is off,so turn it on*/
      LCDCommand(LCD_DISPLAY_CMD|LCD_DISPLAY_ON|LCD_DISPLAY_CURSOR|LCD_DISPLAY_BLINK);
      bCursorOn=TRUE;
    }
    }
    if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    
    /* Handle the two special cases or just the regular case */
    if(UserApp_CursorPosition == LINE1_END_ADDR)
    {
      UserApp_CursorPosition = LINE2_START_ADDR;
    }

    else if (UserApp_CursorPosition == LINE2_END_ADDR)
    {
      LCDClearChars(UserApp_CursorPosition, 1);
      UserApp_CursorPosition = LINE1_START_ADDR;
    }
    
    /* Otherwise just increment one space */
    else
    {
      LCDClearChars(UserApp_CursorPosition, 1);
      UserApp_CursorPosition++;      
    }
    /* New position is set, so update */
    LCDCommand(LCD_ADDRESS_CMD | UserApp_CursorPosition);
  } /* end BUTTON3 */
  
  
  
   if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);

      /* Handle the two special cases or just the regular case */
    
    if(UserApp_CursorPosition == LINE1_START_ADDR)
    {
      UserApp_CursorPosition = LINE2_END_ADDR;
      LCDMessage(UserApp_CursorPosition, "3");
    }

    else if (UserApp_CursorPosition == LINE2_START_ADDR)
    {
      UserApp_CursorPosition = LINE1_END_ADDR;
    }
    else if(UserApp_CursorPosition==LINE2_START_ADDR+1)
    {
      UserApp_CursorPosition--;
      LCDMessage(UserApp_CursorPosition, "0");
    }
    else if(UserApp_CursorPosition==LINE2_START_ADDR+7)
    {
      UserApp_CursorPosition--;
      LCDMessage(UserApp_CursorPosition, "1");
    }
     else if(UserApp_CursorPosition==LINE2_START_ADDR+14)
    {
      UserApp_CursorPosition--;
      LCDMessage(UserApp_CursorPosition, "2");
    }
    /* Otherwise just increment one space */
    else if(UserApp_CursorPosition<=LINE1_END_ADDR)
    {
      UserApp_CursorPosition--;
      LCDMessage(UserApp_CursorPosition,&UserApp_au8MyName[UserApp_CursorPosition]);
    }
    else
    {
      UserApp_CursorPosition--;
    }
    
    /* New position is set, so update */

  

  LCDCommand(LCD_ADDRESS_CMD | UserApp_CursorPosition); 
    
  } /* end BUTTON2 */
  
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    
    /* Handle the two special cases or just the regular case */
    if(UserApp_CursorPosition == LINE1_END_ADDR)
    {
      UserApp_CursorPosition = LINE2_START_ADDR;
    }

    else if (UserApp_CursorPosition == LINE2_END_ADDR)
    {
      LCDClearChars(UserApp_CursorPosition, 1);
      UserApp_CursorPosition = LINE1_START_ADDR;
    }
    
    /* Otherwise just increment one space */
    else
    {
      UserApp_CursorPosition++;      
    }
    /* New position is set, so update */
    LCDCommand(LCD_ADDRESS_CMD | UserApp_CursorPosition);
    LCDMessage(UserApp_CursorPosition, UserApp_au8MyName);
    
  } /* end BUTTON1 */
  
  
  
  
  
  
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
