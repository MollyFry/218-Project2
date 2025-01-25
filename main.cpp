//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"


//=====[Declaration and initialization of public global objects]===============

DigitalIn ignitionButton(D7); // Simulates the ignition button
DigitalIn pSeatSens(D6);      // Passenger seat sensor
DigitalIn dSeatSens(D5);      // Driver seat sensor

DigitalIn pSeatBelt(D4);      // Passenger seatbelt switch
DigitalIn dSeatBelt(D3);      // Driver seatbelt switch

DigitalOut ignitionLed(LED1); // Green LED: Ignition enabled
DigitalOut engineLed(LED2);   // Blue LED: Engine started



DigitalInOut alarmBuzzer(PE_10); // Alarm Buzzer for inhibited ignition

BufferedSerial uartUsb(USBTX, USBRX, 115200); // UART for messages

//=====[Declaration and initialization of public global variables]=============

bool ignitionAttempted = false; // Tracks if the ignition button has been pressed
bool engineRunning = false; // Tracks if the engine is running

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit();
void outputsInit();

void checkSystemState();
void handleIgnition();

// void resetSystem();

// void displayReasonsForInhibition();

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    inputsInit();
    outputsInit();
    while (true){
        checkSystemState();
        handleIgnition();
        //if (ignitionAttempted == false){
        //    checkSystemState();
        //    handleIgnition();
        //}
    }
}

    

    //=====[Implementations of public functions]===================================
    

void checkSystemState()
{
    //while (dSeatSens == OFF){ }

    //    uartUsb.write( "Welcome to enhanced alarm system model 218-W24\r\n", 48 );

    if ( dSeatSen ) {
         uartUsb.write( "Welcome to enhanced alarm system model 218-W24\r\n", 48 );
    }

    if ( ignitionButton ) {
        return;
    }

    if ( dSeatSens && pSeatSens && dSeatBelt && pSeatBelt ) {
        ignitionLed = ON;
    } else {
        ignitionLed = OFF;
    }

    //while (ignitionButton == OFF) {
    //    if (dSeatSens && pSeatSens && dSeatBelt && pSeatBelt) {
    //        ignitionLed = ON; // Enable ignition (Green LED ON)
    //    } else {
     //       ignitionLed = OFF; // Disable ignition (Green LED OFF)
     //   }
//}

}

void handleIgnition()
{

    if ( engineRunning ) {
        engineLed = ON;
        return;
    }
    if (ignitionLed == ON) {
             // Normal ignition
        ignitionLed = OFF; // Turn off the green LED
        engineLed = ON;    // Turn on the blue LED
        uartUsb.write("Engine started.\r\n", 17);
        engineRunning = true;
        ignitionAttempted = true; //exit loop when ignition has been attempted

    } else {
                // Ignition inhibited

                //sound alarm
        alarmBuzzer.output();

        uartUsb.write("Ignition inhibited.\r\n", 20);

           

    if (!dSeatSens){
            uartUsb.write("Driver seat not occupied\r\n", 27);
            ignitionAttempted = true; //exit loop when ignition has been attempted

    } if (!pSeatSens){
        uartUsb.write("Passenger seat not occupied\r\n", 30);
        ignitionAttempted = true; //exit loop when ignition has been attempted

        } if (!dSeatBelt){
            uartUsb.write("Driver seatbelt not fastened\r\n", 30);
            ignitionAttempted = true; //exit loop when ignition has been attempted

         } if (!pSeatBelt){
            uartUsb.write("Passenger seatbelt not fastened\r\n", 33);
            ignitionAttempted = true; //exit loop when ignition has been attempted
        }
    }
    }
            
        

void inputsInit()
            {
            // Configure inputs as pull-down to ensure default state is LOW
                ignitionButton.mode(PullDown);

                pSeatSens.mode(PullDown);
                dSeatSens.mode(PullDown);



                alarmBuzzer.input();
        //turns it off because when it's input it's not connected to ground
            }
void outputsInit()
            {
            // Initialize outputs to OFF
                ignitionLed = OFF;
                engineLed = OFF;

            }