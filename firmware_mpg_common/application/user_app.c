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
extern AntSetupDataType G_stAntSetupData;                         /* From ant.c */

extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentData[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static u32 UserApp_u32DataMsgCount = 0;             /* Counts the number of ANT_DATA packets received */
static u32 UserApp_u32TickMsgCount = 0;             /* Counts the number of ANT_TICK packets received */

static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */
static u8 UserApp_u8Message[48];
static u8 UserApp_u8TempMessage[]={0,0,0,0,0,0,0,0};
static u8 UserApp_u8MessageCount = 0;
static u8 u8ADDR = 0;
static u8 u8continue=0;
static u8 u8position=0;
static bool bGetNewCMD = 0; 
static bool bflag=1;
static bool bPaired = 0;
/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/
u8 u8i = 0;
u8 u8j = 0;
/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/
u8 StringToHex(u8* ptr_)
{
  u8 u8TempVar;
  u8 u8Ans;
  u8TempVar = *ptr_;
  if(u8TempVar > 96 && u8TempVar < 103)
  {
    u8Ans = (u8TempVar-87) * 10;
  }
  else if(u8TempVar > 64 && u8TempVar < 71)
  {
    u8Ans = (u8TempVar-55) * 10;
  }
  else if(u8TempVar > 47 && u8TempVar < 58)
  {
    u8Ans = (u8TempVar-48) * 10;
  }
  
  u8TempVar = *(ptr_ + 1);
  if(u8TempVar > 96 && u8TempVar < 103)
  {
    u8Ans += u8TempVar - 87;
  }
  else if(u8TempVar > 64 && u8TempVar < 71)
  {
    u8Ans += u8TempVar - 55;
  }
  else if(u8TempVar > 47 && u8TempVar < 58)
  {
    u8Ans += u8TempVar - 48;
  }
  
  return u8Ans;
}

u32 StringTou32Num(u8 *ptr)
{
  u32 u32Ans = 0;
  while(*ptr > 47 && *ptr < 58)
  {
    u32Ans = u32Ans*10 + *ptr - 48;
    ptr++;
  }
  return u32Ans;
}


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
  u8 au8WelcomeMessage[] = "ANT SLAVE DEMO";
  u8 au8Instructions[] = "B0 toggles radio";
  
  /* Clear screen and place start messages */
#ifdef MPG1
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 
  LCDMessage(LINE2_START_ADDR, au8Instructions); 

  /* Start with LED0 in RED state = channel is not configured */
  LedOn(RED);
#endif /* MPG1 */

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
  if( AntChannelConfig(ANT_SLAVE) )
  {
    /* Channel is configured, so change LED to yellow */
#ifdef MPG1
    LedOff(RED);
    LedOn(YELLOW);
#endif /* MPG1 */

    UserApp_StateMachine = UserAppSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
#ifdef MPG1
    LedBlink(RED, LED_4HZ);
#endif /* MPG1 */
    UserApp_StateMachine = UserAppSM_Error;
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
  /* Look for BUTTON 0 to open channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue open channel and change LED0 from yellow to blinking green to indicate channel is opening */
    AntOpenChannel();
    LedOff(YELLOW);
    LedBlink(GREEN, LED_2HZ); 
    /* Set timer and advance states */
    UserApp_u32Timeout = G_u32SystemTime1ms;
    UserApp_StateMachine = UserAppSM_WaitChannelOpen;
  }
    
} /* end UserAppSM_Idle() */
/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to open */
static void UserAppSM_WaitChannelOpen(void)
{
  /* Monitor the channel status to check if channel is opened */
  if(AntRadioStatus() == ANT_OPEN)
  {
    LedOn(GREEN);
    if(!bPaired)
      UserApp_StateMachine = UserAppSM_ChannelOpen;
    else
      UserApp_StateMachine = UserAppSM_control;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp_u32Timeout, TIMEOUT_VALUE) )
  {
    AntCloseChannel();
    LedOff(GREEN);
    LedOn(YELLOW);    
    UserApp_StateMachine = UserAppSM_Idle;
  }
    
} /* end UserAppSM_WaitChannelOpen() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Channel is open, so monitor data */
static void UserAppSM_ChannelOpen(void)
{
  static u8 u8LastState = 0xff;
  static u8 au8TickMessage[] = "EVENT x\n\r";  /* "x" at index [6] will be replaced by the current code */
  static u8 au8DataContent[] = "xxxxxxxxxxxxxxxx";
  static u8 au8LastAntData[ANT_APPLICATION_MESSAGE_BYTES] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  static u8 au8TestMessage[] = {0,0,0,0,0,0,0,0};
  static u8 au8StateMessage[] = {0,0,0,0,0,0,0,0};

  bool bGotNewData;
  static bool Wait = 0;
  static bool bsync =0;
  LedOn(RED);
  /* Check for BUTTON0 to close channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue close channel and change LED to blinking green to indicate channel is closing */
    AntCloseChannel();
    u8LastState = 0xff;
    LedOff(YELLOW);
    LedOff(BLUE);
    LedBlink(GREEN, LED_2HZ);

    /* Set timer and advance states */
    UserApp_u32Timeout = G_u32SystemTime1ms;
    UserApp_StateMachine = UserAppSM_WaitChannelClose;
  } /* end if(WasButtonPressed(BUTTON0)) */
  
   
  /* Always check for ANT messages */
  if( AntReadData() )
  {
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      UserApp_u32DataMsgCount++;
      /* Check if the new data is the same as the old data and update as we go */
      for(u8 i = 0; i < ANT_APPLICATION_MESSAGE_BYTES; i++)
      {
          au8DataContent[2 * i] = HexToASCIICharUpper(G_au8AntApiCurrentData[i] / 16);
          au8DataContent[2 * i + 1] = HexToASCIICharUpper(G_au8AntApiCurrentData[i] % 16);
      }
      
      LCDClearChars(LINE2_START_ADDR, 20); 
      LCDMessage(LINE2_START_ADDR, au8DataContent); 
      if(bsync)
      {
        bsync= 0;
        bPaired = 1;
        au8TestMessage[0] = u8ADDR;
        au8TestMessage[7] = 0xff ;
        AntQueueBroadcastMessage(au8TestMessage);
        UserApp_StateMachine = UserAppSM_control;
      }
      
      if(Wait && (G_au8AntApiCurrentData[7] == u8ADDR))
      {
        Wait = 0;
        bsync = 1;
        au8TestMessage[0] = 0;
        au8TestMessage[1] = u8ADDR;
        au8TestMessage[7] = 0xff;
        AntQueueBroadcastMessage(au8TestMessage);
      }
      
       if(G_au8AntApiCurrentData[0]== 0 && bflag && G_au8AntApiCurrentData[7] != 0 )
        {
          u8ADDR = G_au8AntApiCurrentData[7];
          au8TestMessage[0] = u8ADDR;
          UserApp_u8TempMessage[0]=u8ADDR;
          au8TestMessage[7] = 0xff;
          Wait = 1;
          bflag = 0;
          AntQueueBroadcastMessage(au8TestMessage);
        }
       //AntQueueBroadcastMessage(au8TestMessage);
 } /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
    } /* end else if(G_eAntApiCurrentMessageClass == ANT_TICK) */
  }
/* end AntReadData() */
  if(bPaired)
  {
    
      UserApp_StateMachine = UserAppSM_control;
      LedOn(RED);
  }
  
  /* A slave channel can close on its own, so explicitly check channel status */
  if(AntRadioStatus() != ANT_OPEN)
  {
#ifdef MPG1
    LedBlink(GREEN, LED_2HZ);
    LedOff(BLUE);
#endif /* MPG1 */
    u8LastState = 0xff;
    
    UserApp_u32Timeout = G_u32SystemTime1ms;
    UserApp_StateMachine = UserAppSM_WaitChannelClose;
  } /* if(AntRadioStatus() != ANT_OPEN) */
      
} /* end UserAppSM_ChannelOpen() */




static void UserAppSM_control(void)
{
  static u8 u8LastState = 0xff;
  static u8 au8DataContent[] = "xxxxxxxxxxxxxxxx";
  static u8 au8LastAntData[ANT_APPLICATION_MESSAGE_BYTES] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  static u8 au8TestMessage[] = {0,0,0,0,0,0,0,0};
  static u8 au8StateMessage[] = {0,0,0,0,0,0,0,0};
  static bool bReceiving = 0;
  static u8 u8CommendTime=0;
  static u8 au8String[]="xx";
  u8 u8CurrentReadingPlace = 0;
  /* Check for BUTTON0 to close channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue close channel and change LED to blinking green to indicate channel is closing */
    AntCloseChannel();
    u8LastState = 0xff;  
    /* Set timer and advance states */
    UserApp_u32Timeout = G_u32SystemTime1ms;
    UserApp_StateMachine = UserAppSM_WaitChannelClose;
  } /* end if(WasButtonPressed(BUTTON0)) */
  
   
  /* Always check for ANT messages */
  if( AntReadData() )
  {
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      UserApp_u32DataMsgCount++;
      /* Check if the new data is the same as the old data and update as we go */
      for(u8 i = 0; i < ANT_APPLICATION_MESSAGE_BYTES; i++)
      {
          au8DataContent[2 * i] = HexToASCIICharUpper(G_au8AntApiCurrentData[i] / 16);
          au8DataContent[2 * i + 1] = HexToASCIICharUpper(G_au8AntApiCurrentData[i] % 16);
      }
      LCDClearChars(LINE2_START_ADDR, 20); 
      LCDMessage(LINE2_START_ADDR, au8DataContent);
      au8String[0] = HexToASCIICharUpper(u8ADDR / 16);
      au8String[01] = HexToASCIICharUpper(u8ADDR % 16);
      LCDMessage(LINE2_START_ADDR+18,au8String);
      
      if(G_au8AntApiCurrentData[u8CurrentReadingPlace++] == u8ADDR)
      {
        
        bool bGetEnd = 0;
        u8j++;
        if((G_au8AntApiCurrentData[u8CurrentReadingPlace] == 0xFc) && !bReceiving)
        {
          bReceiving = 1;
        }
        else if(bReceiving && (G_au8AntApiCurrentData[u8CurrentReadingPlace] != 0xcc))
        {
          UserApp_u8MessageCount = 0;
          bGetNewCMD = 0;
        }
        u8CurrentReadingPlace++;
        if(bReceiving)
        {
          while(u8CurrentReadingPlace < 8)
          {
            if(G_au8AntApiCurrentData[u8CurrentReadingPlace] != 0xCF)
            {
              UserApp_u8Message[UserApp_u8MessageCount++] = G_au8AntApiCurrentData[u8CurrentReadingPlace++];
            }
            else
            {
              u8CommendTime++;
              bGetEnd = 1;
              UserApp_u8Message[UserApp_u8MessageCount] = G_au8AntApiCurrentData[u8CurrentReadingPlace];
              bReceiving = 0;
              bGetNewCMD = 1;
              LedOff(RED);
              UserApp_StateMachine = UserAppSM_ReactCMD;
              UserApp_u8TempMessage[0]=u8ADDR;
              UserApp_u8TempMessage[1]=UserApp_u8Message[0];
              
              break;
            }
          }
          
        }
        
        
      }
    
      UserApp_u8TempMessage[7]=u8CommendTime;
      AntQueueBroadcastMessage(UserApp_u8TempMessage);
    } 
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {      
    } /* end else if(G_eAntApiCurrentMessageClass == ANT_TICK) */
  }
      
       //AntQueueBroadcastMessage(au8TestMessage);
  /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */
 
    

/* end AntReadData() */
  
  
  /* A slave channel can close on its own, so explicitly check channel status */
  if(AntRadioStatus() != ANT_OPEN)
  {
#ifdef MPG1
    LedBlink(GREEN, LED_2HZ);
    LedOff(BLUE);
#endif /* MPG1 */
    u8LastState = 0xff;
    
    UserApp_u32Timeout = G_u32SystemTime1ms;
    UserApp_StateMachine = UserAppSM_WaitChannelClose;
  } /* if(AntRadioStatus() != ANT_OPEN) */
      
} /* end UserAppSM_ChannelOpen() */

static void UserAppSM_ReactCMD()
{
  u8i++;
  
    if(UserApp_u8Message[0]==0)
  {
    UserApp_StateMachine=UserAppSM_start;
  }
  else if(UserApp_u8Message[0]==1)
  {
    UserApp_StateMachine=UserAppSM_Turnofflight;
  }
  else if(UserApp_u8Message[0]==2)
  {
    UserApp_StateMachine=UserAppSM_TurnonPWM;
  }
  else if(UserApp_u8Message[0]==3)
  {
    UserApp_StateMachine=UserAppSM_TurnonBlink;
  }
  else if(UserApp_u8Message[0]==4)
  {
    UserApp_StateMachine=UserAppSM_LCDDisplay;
 //   
  }
  else if(UserApp_u8Message[0]==5)
  {
    UserApp_StateMachine=UserAppSM_Buzzer;
  }
   else if(UserApp_u8Message[0]==6)
  {
    UserApp_StateMachine=UserAppSM_TurnOffBuzzer;
  }
  else if(UserApp_u8Message[0]==7)
  {
    UserApp_StateMachine=UserAppSM_TurnOnBuzzerForTime;
  }
  else if(UserApp_u8Message[0]==8)
  {
    UserApp_StateMachine=UserAppSM_ReSet;
  }
  else if(UserApp_u8Message[0]==9)
  {
    UserApp_StateMachine=UserAppSM_CloseChannel;
  }
  UserApp_u8MessageCount = 0;
  UserApp_u8Message[UserApp_u8MessageCount] = 0xCF;
}



static void UserAppSM_start(void)
{  
  
    LedOn(UserApp_u8Message[1]-48);
    UserApp_StateMachine=UserAppSM_control;/*signal LED*/  
}
static void UserAppSM_Turnofflight()
{
  LedOff(UserApp_u8Message[1]-48);
  UserApp_StateMachine=UserAppSM_control;/*signal LED*/   
}
static void UserAppSM_TurnonPWM()
{
  static u8 u8TempNumber=0;
  
  u8TempNumber=StringToHex(&UserApp_u8Message[3]);
  LedPWM(UserApp_u8Message[1]-48,u8TempNumber);/*PWM mode*/
  UserApp_StateMachine=UserAppSM_control;
}
static void UserAppSM_TurnonBlink()
{
  static u8 u8tempmessage=0;
  switch(UserApp_u8Message[3])
  {
  case '0':u8tempmessage=1000;break;
  case '1':u8tempmessage=500;break;
  case '2':u8tempmessage=250;break;
  case '4':u8tempmessage=125;break;
  case '8':u8tempmessage=63;break;
  default:break;
  }
  LedBlink(UserApp_u8Message[1]-48,u8tempmessage);
  UserApp_StateMachine=UserAppSM_control;
}
static void UserAppSM_LCDDisplay()
{
    static u8 u8TempString[40];
    u8 i = 1;
    while(UserApp_u8Message[i] != 0xCF)
    {
      u8TempString[i-1] = UserApp_u8Message[i];
      i++;
    }
    u8TempString[i-1] = '\0';
    LCDClearChars(LINE1_START_ADDR, 20); 
    LCDMessage(LINE1_START_ADDR, u8TempString); 
    UserApp_StateMachine=UserAppSM_control;
}

static void UserAppSM_Buzzer()
{
  if(UserApp_u8Message[1]=='1')
  {
  PWMAudioSetFrequency(BUZZER1,StringTou32Num(&UserApp_u8Message[3]));
  PWMAudioOn(BUZZER1);
  }
  else if(UserApp_u8Message[1]=='2')
  {
  PWMAudioSetFrequency(BUZZER1,StringTou32Num(&UserApp_u8Message[3]));
  PWMAudioOn(BUZZER2);
  }
  UserApp_StateMachine=UserAppSM_control;
}
static void UserAppSM_TurnOffBuzzer()
{
   if(UserApp_u8Message[1]=='1')
   {
     PWMAudioOff(BUZZER1);
   }
   else if(UserApp_u8Message[1]=='2')
   {
     PWMAudioOff(BUZZER2);
   }
  UserApp_StateMachine=UserAppSM_control;
}



static void UserAppSM_TurnOnBuzzerForTime()
{
  static u32 u32LastTime= 0 ;
  static u32 u32Counter = 0;
  static bool bSetted = 0;
  if(!bSetted)
  {
    u8 i = 3;
    bSetted = 1;
    while (UserApp_u8Message[i] > 47 && UserApp_u8Message[i] <58)
    { 
      i++;
    }
    u32LastTime = StringTou32Num(&UserApp_u8Message[i+1]);
    if(UserApp_u8Message[1]=='1')
    {
      PWMAudioSetFrequency(BUZZER1,StringTou32Num(&UserApp_u8Message[3]));
      PWMAudioOn(BUZZER1);
    }
    else if(UserApp_u8Message[1]=='2')
    {
      PWMAudioSetFrequency(BUZZER2,StringTou32Num(&UserApp_u8Message[3]));
      PWMAudioOn(BUZZER2);
    }
  }
  u32Counter++;
  if(u32Counter == u32LastTime)
  {
    u32Counter = 0;
    bSetted = 0;
    UserApp_StateMachine=UserAppSM_TurnOffBuzzer;
  }
}
static void UserAppSM_ReSet()
{
  
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(BLUE);
  LedOff(CYAN);
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(ORANGE);
  LedOff(RED);
  PWMAudioOff(BUZZER1);
  PWMAudioOff(BUZZER2);
  LCDClearChars(LINE1_START_ADDR, 20); 
  LCDMessage(LINE1_START_ADDR,"Reseted!");
  UserApp_StateMachine=UserAppSM_control;
}
static void UserAppSM_CloseChannel()
{
  AntCloseChannel();
    /* Set timer and advance states */
  UserApp_u32Timeout = G_u32SystemTime1ms;
  UserApp_StateMachine = UserAppSM_WaitChannelClose;
}




















/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to close */
static void UserAppSM_WaitChannelClose(void)
{
  /* Monitor the channel status to check if channel is closed */
  if(AntRadioStatus() == ANT_CLOSED)
  {
#ifdef MPG1
    LedOff(GREEN);
    LedOn(YELLOW);
#endif /* MPG1 */
    UserApp_StateMachine = UserAppSM_Idle;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp_u32Timeout, TIMEOUT_VALUE) )
  {
#ifdef MPG1
    LedOff(GREEN);
    LedOff(YELLOW);
    LedBlink(RED, LED_4HZ);
#endif /* MPG1 */
    UserApp_StateMachine = UserAppSM_Error;
  }
    
} /* end UserAppSM_WaitChannelClose() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{

} /* end UserAppSM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
