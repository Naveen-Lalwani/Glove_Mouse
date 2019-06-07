/*********************************************************************************
 *                                 Glove Mouse                                   *
 *********************************************************************************
 * Author: Naveen lalwani  														 *
 * Email: naveen.lalwani2014@gmail.com											 *
 * 																				 *
 *********************************************************************************
 * This code presents the basic impplementation of wearable mouse which can work *
 * mid air without the need of a hard base and uses accelerometer to change the  *
 * position of the mouse pointer on the screen and flex sensors to initiate the  *
 * click mechanism.																 *
 *          																	 *
 * Feel free to use this code in your project. Happy building.                   *
 *																				 *
 *********************************************************************************
 * Note: You need an Arduino Leonardo for this project because of the board's 	 *
 * 		 USB HID (Human Iterface Device) abilitites which allows the board to 	 *
 * 		 behave (act) as a USB human interface devices. This allows the Leonardo * 
 * 		 to appear to a connected computer as a mouse and keyboard, in addition  *
 *		 to a virtual (CDC) serial / COM port.									 *
 *********************************************************************************
 */
#include <Mouse.h>      // Mouse Library

int zout = A1;          // Analog Pin for measuring Z value  
int yout = A2;     		// Analog Pin for measuring Y value 
int xout = A3;          // Analog Pin for measuring X value
int leftClick = A4; 	// Analog Pin for flex sensor 1
int rightClick = A5;	// Analog Pin for flex Sensor 2

int range = 20;         // Output range of X or Y or Z movements (Mapped Value) 
int responseDelay = 8;  // Delay of the mouse between movements, in milliseconds 
int threshold = range/8;    // Resting threshold 
int center =  range/2;      // Resting position value

int minima[] = {1023, 1023};   // actual analogRead minima bcoz of 10 bit ADC
int maxima[] = {0, 0};         
int axis[] = {zout, xout};     // pin numbers for {z, x} 
int mouseReading[2];

/**
  * readAxis: Function to read an axis (0 or 1 for z or x) and scale the analog 
  * 		  input range to a range from 0 to ADC range (Here 1024)
  * @param axisNumber 0 for Z axis, 1 for X axis because mouse is used keeping 
  *					  hand along Y axis
  * @return integer value by which mouse pointer should move along the respective
  * 		axis    
  */ 
int readAxis(int axisNumber);

void setup() {
	Serial.begin(9600); 
	Mouse.begin(); 
}

void loop() {
	// Read and scale the two axes.
    int xReading = readAxis(0);
    int zReading = readAxis(1);

    Mouse.move(xReading, zReading, 0); //Mouse.move(xPos, yPos, wheel); 
    delay(responseDelay);

    // Read for clicks. 
    int flex1read = analogRead(leftClick); 
    int flex2read = analogRead(rightClick);

	/* Minima for flex sensor = 300
	 * Maxima for flex semsor = 90. 
	 * Values recorded from interfacing the sensor. You can check these by 
	 * recording the value of flex sensor on no flexing and maximum flexing
	 * on Serial Monitor.
	 * The values are mapped from [90, 300] range to [0, 100] range.
	 */
    int flex1active = map(flex1read, 90, 300, 0, 100);  
    int flex2active = map(flex2read, 90, 300, 0, 100);

    // Left Click
  	if (flex1active <= 15) { 
  		Mouse.click(MOUSE_LEFT);  
  		// Mouse.click(arg) where, arg = MOUSE_LEFT, MOUSE_RIGHT..   
  		delay(180);           
  	}         

  	// Right Click
  	if(flex2active <= 25) { 
  		Mouse.click(MOUSE_RIGHT);
  		delay(500); 
  	}

 	/* NOTE: To delay in the left click block is less as compared to right click 
 	 *       block of code to check for the double left click.
 	 */  
  
  	delay(responseDelay); 
}  


int readAxis(int axisNumber) {
	int distance = 0;    // distance from center of the output range 
	int reading = analogRead(axis[axisNumber]);

    /**
     * If the current reading exceeds the max or min for this axis, reset the 
     * max or min so that mapped value range increases or decreases.
     *
     * This is necessary becuase the accelerometer max and min values change
     * randomly and thus, fixed max and min values will not give desired 
     * results.
     */

    if (reading < minima[axisNumber]) {
    	minima[axisNumber] = reading; 
    }
  
    if (reading > maxima[axisNumber]) { 
  	    maxima[axisNumber] = reading; 
    }

    // Map the reading from the analog input range to the output range 
    reading = map(reading, minima[axisNumber], maxima[axisNumber], 0 , range);
  
    /* If the output reading is outside from the rest position threshold,then
     * calculate the distance from the center.
     */  
    if (abs(reading - center) >= threshold) { 
    	distance = (reading - center); 
    }

  	// The X axis needs to be inverted in order to map the movement correctly
	if (axisNumber == 1)  { 
		distance = -distance; 
	}
  
  	return distance; 
}