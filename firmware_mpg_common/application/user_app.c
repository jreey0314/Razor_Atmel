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
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */ static u16 u16Counter=0;



/******************************t****************************************************************************************
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
  PWMAudioSetFrequency(BUZZER1, 50);
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
#define doo 261
#define re 293
#define mi 329
#define fa 349
#define so 392
#define la 440
#define si 493
#define dooo 261*2
#define ree 293*2
#define mii 329*2
#define faa 349*2
#define soo 392*2
#define loot 30
/* Wait for a message to be queued */
static void UserAppSM_Idle(void)
{
 
	static u32 u32frequency[]={so,so,mii,mii,ree,dooo,dooo,dooo,ree,ree,mii,
mii,so,so,ree,ree,mii,mii,mii,mii,so,so,ree,ree,mii,mii,loot,loot,ree,ree,ree,ree,mii,
faa,faa,faa,mii,mii,ree,ree,si,si,dooo,dooo,mi,mi,la,la,dooo,dooo,mi,mi,la,la,si,si,
	loot,loot,si,si,si,si,mii,soo,soo,soo,mii,mii,dooo,dooo,si,si,
	la,la,faa,faa,loot,loot,soo,soo,faa,faa,mii,mii,so,so,mii,mii,loot,loot,faa,faa,
mii,mii,dooo,dooo,faa,faa,ree,ree,loot,loot,ree,ree,dooo,dooo,mii,mii,
	loot,loot,ree,ree,dooo,dooo,mii,mii,mii,mii,ree,ree,dooo,dooo,dooo,dooo};
  static u16 u16Counter=0;
  static u16 u16pause=1;
  static u16 i=0;
  u16Counter++;
 PWMAudioSetFrequency(BUZZER1,u32frequency[i]);
        PWMAudioOn(BUZZER1);
	if(u16Counter==450&&(u16pause%2==0|u16pause==5|u16pause==33|u16pause==65|u16pause==77|u16pause==89|u16pause==101))
	{
		PWMAudioOff(BUZZER1);
	}
    if(u16Counter==500)
      {
        i++;
		u16pause++;
        if(i==125)
        {
          i=0;
        }
       
        u16Counter=0;
      }
}
  
    
 /* end UserAppSM_Idle() */
     

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
