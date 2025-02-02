//=====[Libraries]=============================================================


#include "mbed.h"
#include "arm_book_lib.h"




//=====[Declaration of private defines]========================================


#define LIGHT_LEVEL_NUMBER_OF_AVG_SAMPLES     10
#define POTENTIOMETER_MAX_AUTO_LIGHT_LEVEL     0.66
#define POTENTIOMETER_MIN_AUTO_LIGHT_LEVEL     0.33
#define DAYLIGHT_LEVEL_CUTOFF                  0.0
#define DUSK_LEVEL                             0.5
#define DAY_LEVEL                              0.8
#define TIME_INCREMENT_MS                     10
#define DUSK_LEVEL_DELAY_MS                 1000
#define DAY_LEVEL_DELAY_MS                  2000


//=====[Declaration and initialization of public global objects]===============




DigitalIn ignitionButton(BUTTON1); // Simulates the ignition button
DigitalIn pSeatSens(D6);      // Passenger seat sensor
DigitalIn dSeatSens(D5);      // Driver seat sensor


DigitalIn pSeatBelt(D4);      // Passenger seatbelt switch
DigitalIn dSeatBelt(D3);      // Driver seatbelt switch
AnalogIn lightSensor(A0);   // Light level sensor
AnalogIn headlightMode(A1);


DigitalOut ignitionLed(LED1); // Green LED: Ignition enabled
DigitalOut engineLed(LED2);   // Blue LED: Engine started
DigitalOut headlight1(D2);  // headlight 1 LED
DigitalOut headlight2(D1);  // headlight 2 LED




DigitalInOut alarmBuzzer(PE_10); // Alarm Buzzer for inhibited ignition




BufferedSerial uartUsb(USBTX, USBRX, 115200); // UART for messages




//=====[Declaration and initialization of public global variables]=============
typedef enum {
   IDLE,
   IGNITION_ON,
   ENGINE_ON
} IgnitionState_t;




typedef enum {
   HEADLIGHT_OFF_STATE,
   HEADLIGHT_ON_STATE,
   HEADLIGHT_AUTO_STATE
} HeadlightState_t;




float lightReadingAvg = 0.0;


float lightReadingsArray[LIGHT_LEVEL_NUMBER_OF_AVG_SAMPLES];


float headlightModeReading = 0.0;


int totalLightDelayTime_ON = 0;


int totalLightDelayTime_OFF = 0;




//static float analoglight_sensor(float analogReading);




bool engineRunning = false; // Tracks if the engine is running
bool driverSeated = false;  // tracks if the headlight mode is auto
bool reasonInhibition = false;


bool ignitionButtonReleased = false;
int ignitionReleaseTimes = 0;




IgnitionState_t ignitionState = IDLE;
HeadlightState_t headlightState = HEADLIGHT_OFF_STATE;






//=====[Declarations (prototypes) of private functions]========================




static float getAvgLightLevel();




//=====[Declarations (prototypes) of public functions]=========================




void inputsInit();
void outputsInit();
void checkSystemState();
void handleIgnition();
void engineStop();
void handleHeadlights();
void displayInhibitionReasons();
void lightSensorInit();
void headlightUpdateMode();
float lightSensorRead();




//=====[Main function, the program entry point after power on or reset]========




int main()
{
   inputsInit();
   outputsInit();
   lightSensorInit();
   while( true ) {
       checkSystemState();
       handleIgnition();
       handleHeadlights();
       delay( TIME_INCREMENT_MS );
   }
}


 //=====[Implementations of public functions]===================================


void checkSystemState()
{
   if ( dSeatSens && !driverSeated ) {
       uartUsb.write( "Welcome to enhanced alarm system model 218-W24 \r\n", 49 );
       driverSeated = true;
   }


   if ( ignitionState == ENGINE_ON ) {
       return;
   }
  
   if ( dSeatSens && pSeatSens && dSeatBelt && pSeatBelt ) {
       ignitionLed = ON;
   } else {
       ignitionLed = OFF;
   }
}




void handleIgnition()
{
   switch ( ignitionState ) {
       case IDLE:
           if ( ignitionButton ) {
               if ( ignitionLed ) {
                   ignitionState = IGNITION_ON;
               } else {
                   alarmBuzzer.output();
                   if ( !reasonInhibition ) {
                       reasonInhibition = true;
                       uartUsb.write( "Ignition inhibited. \r\n", 22 );
                       displayInhibitionReasons();
                   }
               }
           }
           break;


       case IGNITION_ON:
           ignitionLed = OFF;
           engineLed = ON;
           uartUsb.write( "Engine started. \r\n", 18 );
           engineRunning = true;
           ignitionState = ENGINE_ON;
           alarmBuzzer.input();
           ignitionReleaseTimes = 0;
           break;
      
       case ENGINE_ON:
           if ( ignitionButton ){
               ignitionButtonReleased = false;
               if ( ignitionReleaseTimes < 1 ) {
                   delay( TIME_INCREMENT_MS );
                   if ( !ignitionButton ) {
                       ignitionButtonReleased = true;
                       ignitionReleaseTimes++;
                   }
               }
           }


           if ( ignitionReleaseTimes >= 1 ) {
               engineStop();
           }
           break;


   }
}






void displayInhibitionReasons() {
   if ( !dSeatSens ){
       uartUsb.write( "Driver seat not occupied.\r\n", 27 );
   }
   if ( !pSeatSens ){
       uartUsb.write( "Passenger seat not occupied\r\n", 30 );
   }
   if ( !dSeatBelt ){
       uartUsb.write( "Driver seatbelt not fastened\r\n", 30 );
   }
   if ( !pSeatBelt ){
       uartUsb.write( "Passenger seatbelt not fastened\r\n", 33 );
   }
}
         
void inputsInit()
{
   // Configure inputs as pull-down to ensure default state is LOW
   ignitionButton.mode( PullDown );
   pSeatSens.mode( PullDown );
   dSeatSens.mode( PullDown );
   pSeatBelt.mode( PullDown );
   dSeatBelt.mode( PullDown );
   alarmBuzzer.input(); //turns it off because when it's input it's not connected to ground
}




void outputsInit()
{
   // Initialize outputs to OFF
   ignitionLed = OFF;
   engineLed = OFF;
   headlight1 = OFF;
   headlight2 = OFF;
}






void engineStop()
{
   engineLed = OFF;
   uartUsb.write( "Engine stopped.\r\n", 17 );
   ignitionState = IDLE;
   engineRunning = false;
   headlightState = HEADLIGHT_OFF_STATE;
   handleHeadlights();
}




void handleHeadlights() {
   headlightUpdateMode();
   if ( engineRunning ) {
       switch ( headlightState ) {
           case HEADLIGHT_ON_STATE:
               headlight1 = ON;
               headlight2 = ON;
           break;
           case HEADLIGHT_OFF_STATE:
               headlight1 = OFF;
               headlight2 = OFF;
           break;
           case HEADLIGHT_AUTO_STATE:
               if ( lightSensorRead() <= DUSK_LEVEL ){
                   totalLightDelayTime_ON = totalLightDelayTime_ON + TIME_INCREMENT_MS;
                   if ( totalLightDelayTime_ON >= DUSK_LEVEL_DELAY_MS ) {
                       totalLightDelayTime_OFF = 0;
                       totalLightDelayTime_ON = 0;
                       headlight1 = ON;
                       headlight2 = ON;
                   }


               } else if ( lightSensorRead() > DUSK_LEVEL && lightSensorRead() <= DAY_LEVEL ){
                   totalLightDelayTime_ON = 0;
                   totalLightDelayTime_OFF = 0;


               } else if ( lightSensorRead() > DAY_LEVEL ){
                   totalLightDelayTime_OFF = totalLightDelayTime_OFF + TIME_INCREMENT_MS;
                   if ( totalLightDelayTime_OFF >= DAY_LEVEL_DELAY_MS ){
                       totalLightDelayTime_OFF = 0;
                       totalLightDelayTime_ON = 0;
                       headlight1 = OFF;
                       headlight2 = OFF;
                   }
               }
           break;
       }
   } else {
       headlight1 = OFF;
       headlight2 = OFF;
   }
}


void headlightUpdateMode() {
   headlightModeReading = headlightMode.read();
  
   if ( headlightModeReading <= POTENTIOMETER_MIN_AUTO_LIGHT_LEVEL ) {
       headlightState = HEADLIGHT_AUTO_STATE;
   } else if ( headlightModeReading > POTENTIOMETER_MIN_AUTO_LIGHT_LEVEL &&
               headlightModeReading <= POTENTIOMETER_MAX_AUTO_LIGHT_LEVEL ) {
       headlightState = HEADLIGHT_ON_STATE;
   } else {
       headlightState = HEADLIGHT_OFF_STATE;
   }
}




void lightSensorUpdate() {
   static int lightSensorSampleIndex = 0;
   float lightReadingsSum = 0.0;
   int i;


   lightReadingsArray[lightSensorSampleIndex] = lightSensor.read();
   lightSensorSampleIndex++;


   if ( lightSensorSampleIndex >= LIGHT_LEVEL_NUMBER_OF_AVG_SAMPLES ) {
       lightSensorSampleIndex = 0;
   }


   lightReadingsSum = 0.0;


   for ( int i = 0; i < LIGHT_LEVEL_NUMBER_OF_AVG_SAMPLES; i++ ) {
       lightReadingsSum = lightReadingsSum + lightReadingsArray[i];
   }


   lightReadingAvg = lightReadingsSum/LIGHT_LEVEL_NUMBER_OF_AVG_SAMPLES;
}


float lightSensorRead() {
   lightSensorUpdate();
   return lightReadingAvg;
}


void lightSensorInit() {
   int i;


   for ( int i = 0; i < LIGHT_LEVEL_NUMBER_OF_AVG_SAMPLES; i++ ) {
       lightReadingsArray[i] = 0;
   }
}
