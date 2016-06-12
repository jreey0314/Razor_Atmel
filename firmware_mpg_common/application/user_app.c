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
extern AntSetupDataType G_stAntSetupData;                         /* From ant.c */

extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentData[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */
static u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0};


static u32 UserApp_u32DataMsgCount = 0;   /* ANT_DATA packets received */
static u32 UserApp_u32TickMsgCount = 0;   /* ANT_TICK packets received */
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
   /* All discrete LEDs to off */
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(BLUE);
  LedOff(CYAN);
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(ORANGE);
  LedOff(RED);
  /*Test comment for GitHub */
  /* If good initialization, set state to Idle */
 /* Configure ANT for this application */
  G_stAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  G_stAntSetupData.AntSerialLo         = ANT_SERIAL_LO_USERAPP;
  G_stAntSetupData.AntSerialHi         = ANT_SERIAL_HI_USERAPP;
  G_stAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  G_stAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  G_stAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  G_stAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  G_stAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  G_stAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;
  /* If good initialization, set state to Idle */
  
  if( AntChannelConfig(£¡ANT_MASTER) )
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
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue open channel and change LED0 from yellow to blinking green to indicate channel is opening */
    AntOpenChannel();

    LedOff(RED);
    LedOn(BLUE);
    
    /* Set timer and advance states */
    UserApp_u32Timeout = G_u32SystemTime1ms;
    UserApp_StateMachine = UserAppSM_WaitChannelOpen;
  }
} /* end UserAppSM_Idle() */
static void UserAppSM_WaitChannelOpen(void) //Open the channel         
{
  /* Monitor the channel status to check if channel is opened */
  if(AntRadioStatus() == ANT_OPEN)
  {
    
    UserApp_StateMachine = UserAppSM_ChannelOpen;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp_u32Timeout, TIMEOUT_VALUE) )
  {
    AntCloseChannel();
   LedOn(RED);
    LedOff(BLUE);
    UserApp_StateMachine = UserAppSM_Idle;
  }
} /* end UserAppSM_Error() */     


static void UserAppSM_ChannelOpen(void)          
{
  static u8 u8TextMessage[] = {0,0,0,0,0,0,0,0};
  static u8 u8TempString[] = "xxxxxxxxxxxxxxxx";
  static u8 u8CodeMessage[]={'A','B','C','D','E','F','0','1'};
  static u8 u8WrongMessage[]={0,0,0,0,0,0,0,0};
  static bool bGetResponse = 0;
  static bool bWaitResponse = 0;
  static bool clk = 0;
  static u8 u8ResponseMessage[]={'A','B','C','D','E','F','1','1'};
  static u8 u8Temp1=0;
  if(WasButtonPressed(BUTTON1))
    {
      /* Got the button, so complete one-time actions before next state */
      ButtonAcknowledge(BUTTON1);
      
      /* Queue close channel, initialize the u8LastState variable and change LED to blinking green to indicate channel is closing */
      AntCloseChannel();
      //u8LastState = 0xff;
      LedOn(RED);
      LedOff(BLUE);
      
      
      /* Set timer and advance states */
      UserApp_u32Timeout = G_u32SystemTime1ms;
      UserApp_StateMachine = UserAppSM_WaitChannelClose;
    } /* end if(WasButtonPressed(BUTTON0)) */
  if(AntRadioStatus() != ANT_OPEN)
    {
      
      LedOff(BLUE);
      //u8LastState = 0xff;
      
      UserApp_u32Timeout = G_u32SystemTime1ms;
      UserApp_StateMachine = UserAppSM_WaitChannelClose;
    } /* if(AntRadioStatus() != ANT_OPEN) */
  if( AntReadData() )
    {
       /* New data message: check what it is */
      if(G_eAntApiCurrentMessageClass == ANT_DATA)
      {
        UserApp_u32DataMsgCount++;
        for(u8 i = 0; i < ANT_DATA_BYTES; i++)
        {
         u8TempString[2 * i]     = HexToASCIICharUpper(G_au8AntApiCurrentData[i] / 16);
         u8TempString[2 * i + 1] = HexToASCIICharUpper(G_au8AntApiCurrentData[i] % 16);
        }
      u8Temp1=u8TempString[0]*10+u8TempString[1];
      u8Temp3=u8TempString[4]*10+u8TempString[5];
      u8Temp4=u8TempString[6]*10+u8TempString[7];
      
        /*conventions*/
        switch(u8Temp1)
        {
        case 00 :;break;/*empty message*/
        case 01 :
          if(u8TempString[2]=='0'&&u8TempString[3]=='1')
        {
              LedPWM(u8Temp3,u8Temp4);
              if(u8TempString[8]==1)
              {
                LedOff(ORANGE);
              }
              
        }
        else if(u8TempString[2]==0&&u8TempString[3]==2)
        {
          
        }
        else if(u8TempString[2]==0&&u8TempString[3]==4)
        {
          
        }
          break;/*commend signal LED light*/
        case 02 :break;/*commend all LED lights*/
        case 04 :break;/*Text message to display on the LCD*/
        case 08:break;/*commend BUZZER*/
        case 10:break;/*commend LCD background color*/
        case 20:break;/*signal of shake hands*/
        case 40:break;/*reset*/
        case 80:break;/*return all device state*/
        default:AntQueueBroadcastMessage(u8WrongMessage);
        
        }
       
        
      
        
        DebugPrintf("ANT_DATA:");
        DebugPrintf(u8TempString);
        DebugPrintf("\n\r");
         
          LCDMessage(LINE2_START_ADDR,u8TempString);
      } /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */

      else if(G_eAntApiCurrentMessageClass == ANT_TICK)
      {

        UserApp_u32TickMsgCount++;
        if(WasButtonPressed(BUTTON2))
        {
          ButtonAcknowledge(BUTTON2);
          
          u8TextMessage[7]++;
          if(u8TextMessage[7] == 0)
          {
            u8TextMessage[6]++;
            if(u8TextMessage[6] == 0)
            {
              u8TextMessage[5]++;
            }
          }
        }

        if(WasButtonPressed(BUTTON3))
        {
          ButtonAcknowledge(BUTTON3);
          AntQueueBroadcastMessage(u8TextMessage);

        }
       
        //AntQueueBroadcastMessage(u8TextMessage);
        
        for(u8 i = 0; i < ANT_DATA_BYTES; i++)
        {
         u8TempString[2 * i]     = HexToASCIICharUpper(G_au8AntApiCurrentData[i] / 16);
         u8TempString[2 * i + 1] = HexToASCIICharUpper(G_au8AntApiCurrentData[i] % 16);
        }
        DebugPrintf("ANT_TICK:");
        DebugPrintf(u8TempString);
        DebugPrintf("\n\r");
      } /* end else if(G_eAntApiCurrentMessageClass == ANT_TICK) */
    } /* end AntReadData() */
} /* end UserAppSM_Error() */

static void UserAppSM_WaitChannelClose()
{
  /* Monitor the channel status to check if channel is closed */
  if(AntRadioStatus() == ANT_CLOSED)
  {
    

    UserApp_StateMachine = UserAppSM_Idle;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp_u32Timeout, TIMEOUT_VALUE) )
  {
  

    UserApp_StateMachine = UserAppSM_Error;
  }
    
}
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
/*--------------------------------------------------------------------------------------------------------------------*/------------------*/
