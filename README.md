Livy Bottomley & Molly Fry



This is a car alarm and automatic headlight control system. When the driver sits down in the car, a welcome message is displayed. When both a driver and a passenger are sitting in the car,
and both are wearing seat belts, ignition is allowed to begin. When the ignition button is pressed under these conditions, the engine starts. While the engine is running, the user has the 
option to select the headlight mode from the options ON, OFF, and AUTO. If ON is selected then the headlights turn on, if OFF is selected then the headlights turn off, and if AUTO is selected
then the headlights turn on or off depending on the light of the environment the system is in. When the engine turns off, both of the headlights turn off. 

Ignition Subsystem

|                     Specification                    |  Test Result  |                         Comment                       |
| ---------------------------------------------------- | ------------- |-------------------------------------------------------|
| Engine start is enabled when both seats are          | Pass          | The error messages are printed on the serial monitor. |
| occupied and both seat belts are fastened.           |               |                                                       |
| Otherwise an error message is printed.               |               |                                                       |
|                                                      |               |                                                       |
| The engine starts when the green LED is on and       | Pass          |                                                       |
| the ignition button is pressed.                      |               |                                                       |
|                                                      |               |                                                       |
| The engine remains on even if the driver or passenger| Pass          |                                                       |
| remove their seatbelts or exit the vehicle.          |               |                                                       |
|                                                      |               |                                                       |
| While the engine is running, turn it off by pressing |  Pass         |                                                       |
| and releasing the ignition button.                   |               |                                                       |


Headlight Subsystem

|                     Specification                    |  Test Result  |                       Comment                      |
| ---------------------------------------------------- | ------------- |----------------------------------------------------|
| While the engine is running, the user selects ON or  | Pass          | These values are selected by the potentiometer.    |
| OFF to turn the headlights on or off.                |               |                                                    |
|                                                      |               |                                                    |
| If the user selects AUTO, the headlights turn on or  |  Pass         | Auto is selected by using the potentiometer.       |
| off depending on the light threshold levels of the   |               |                                                    |
| surrounding environment the system is in.            |               |                                                    |
|                                                      |               |                                                    |
| Both headlights turn off when the engine is off.     | Pass          |                                                    |


