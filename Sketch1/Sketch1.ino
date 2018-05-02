/*
 Name:		Sketch1.ino
 Created:	4/4/2018 17:17:52
 Author:	jason
*/

//rotary code taken from https://howtomechatronics.com/tutorials/arduino/rotary-encoder-works-use-arduino/
// very helpful post on serial communication https://startingelectronics.org/software/arduino/learn-to-program-course/19-serial-input/
#include <ctype.h>

// ----BEGIN Variable creation

//Rotary Pins
#define PinCLK 4 //grey
#define PinDT 3 //black
#define PinSW 2 //yellow

// rotary flag and values
int rgbInputVal = 0;
int prevRGBInputVal; 
int rotCurrentState; // current value(great dnb producer) of the clk pin
int rotPrevState;	// previous value of clk pin
volatile boolean ButtonPressed;  // need volatile for Interrupts

//RGB pins -- these must be pwm 
#define Red 11
#define Green 10
#define Blue 9

//Joystick Pins 
#define jSW 7
#define	jX	0 // orange
#define	jY 1 //yellow
#define xLED 6
#define yLED 5

//colour values
int rgbPin = 9; // used for switching input channel 
int prevPin;
int rTrgt = 222;
int bTrgt = 97;
int gTrgt = 115; 

// joystick values using a 10k resistor will yield values roughly between 0 - 310
int jsXTarget = 50;
int jsYTarget= 220;
int lightValX = 50; 
int lightValY = 50;
int jsTolerance = 5; // allow for the sensor to have 10 degrees of wiggle room 
int lvChange = 35; // amount to inc/decrement status lights
int jsXpos; 
int jsYpos;


//stage flags
//boolean rgbState = true;
boolean rgbState = false;
boolean rPass = false;
boolean gPass = false;
boolean bPass = false;

//boolean jsState = false;
boolean jsState = true;
boolean xPass = false;
boolean yPass = false;

//data collection 
int dataIndex =0; //tracks position of data
#define DataSize 5 //max size of all target val
#define RLedTar 'A' //flag for red target
#define GLedTar 'B' // flag for green
#define BLedTar 'C' // flag for blue
#define xTarg 'D' // flag for joystick X
#define yTarg 'E' // flag for JS Y
String rx_Str = "";
char rx_temp; 
/*char rTargetArr[3];
char bTargetArr[3];
char gTargetArr[3];
char xTargetArr[3];
char yTargetArr[3];
*/




//------END	Variable creation

//------ Helper Methods
void debugPrint(String message, int val) {
	Serial.println(message + val);
}

void printRotaryEncoderValues() {
	debugPrint("current clk ", rotCurrentState);
	debugPrint("Previous clk", rotPrevState);
	debugPrint("current dt", digitalRead(PinDT));
	debugPrint("switch", digitalRead(PinSW));
}

void printJoystick() {
	debugPrint("X axis	", analogRead(jX));
	debugPrint("Y axis	", analogRead(jY));
}



// ensures that values sent to the rgb led are within 0-255
int enforceLimit(int num) {
	if (num > 255) {
		num = 0; 
	} else if (num < 0){
		num = 255; 
	}
	return num;
}

// value checking for r/g/b stages 
void checkRGBTarget(int rgb, int val ) {
	int target;
	if (rgb == Red) {
		target = rTrgt;
	}
	else if (rgb == Green){
		target = gTrgt;
	}
	else if (rgb == Blue){
		target = bTrgt;
	}
	//debugPrint("before if rgb ", rgbInputVal);
	if (abs(target - val) <= 10) {
		debugPrint("rgbInputVal ", rgbInputVal);
		debugPrint("target	", target);
		debugPrint("val param	", val);
		debugPrint("difference", abs(target - val));
		// win 
		blink(rgb);
		blink(rgb);
		blink(rgb);
		if (rgb == Red) {
			rPass = true;
			rgbInputVal = 0;
		}
		else if (rgb == Green)
		{
			gPass = true;
			rgbInputVal = 0;
		}
		else if (rgb == Blue)
		{
			bPass = true; 
			rgbInputVal = 0;
		}
		rgbStateGate();
	}
}

//flag checking r/g/b and overall success
void rgbStateGate() {
	if (rPass == true && gPass == true && bPass == true) {
		rgbState = false; 
		debugPrint("you won", 1337);
		debugPrint("you won", 1337);
		debugPrint("you won", 1337);
		debugPrint("you won", 1337);
		debugPrint("you won", 1337);
		debugPrint("you won", 1337);
		jsState = true;
	}
}

// blinks any light sent to it 
void blink(int target) {
	digitalWrite(target, HIGH);
	delay(250);
	digitalWrite(target, LOW);
	delay(250);
}

// brightens/dims led as status indicator for JS stage
int changeLight(int target, int val, boolean brighter) {

	
	//debugPrint("old val", lightValX);
	//debugPrint("brighter", brighter);
	if (brighter) {
		val += lvChange;
		if (val >= 255)
		{
			val = 255;
		}
	}
	else{
		val -= lvChange;
		if (val <= 0)
		{
			val = 0;
		}
	
	}
	//debugPrint("new val	", val);
	//analogWrite(target, val);
	//debugPrint("value", val);
	analogWrite(target, val);
	delay(50);

	return val;
}


//--- END Helper Methods

// Interrupt routine runs when SW reads LOW switched input pin when SW read HIGH again
void isr() {
	delay(4);  // delay for Debouncing
			   //debugPrint("pinState ", digitalRead(PinSW));
	if (digitalRead(PinSW) == 1) {
		rgbPin++;
		if (rgbPin > 11) rgbPin = 9;
		rgbInputVal = 0; // zero out input for next light
						 //debugPrint("pin ", rgbPin);
	}
}


/*
int targetArrayFiller() {
	//debugPrint("in filler", 0);
	

	for (int i = 1, ii = 0; i < DataSize; i++, ii++) {
		targetArray[ii] = valHolder[i];
		//Serial.print(valHolder[i]);
		Serial.print(targetArray[i]);

	}
	for (int i = 1; i < 3; i++) {
		Serial.println(targetArray[i]);
		//Serial.println("in the for");
	}

	int ret = atoi(targetArray);
	Serial.println("ret " + ret);
	return ret;
}

*/

int targetBuilder(String message) {
	String temp = "";

	for (int i = 0; i < sizeof(message); i++) {
		if (isDigit(message[i]))
		{
			temp += message[i];
		}
	}

	return temp.toInt();


}

// watches for data sent from pi
void serialEvent() {
	while (Serial.available()) {
		char c = Serial.read();
		if (dataIndex < DataSize) {
			if (isAlphaNumeric(c)) {
				rx_Str += c;
				//debugPrint("hello", dataIndex);
			}
			dataIndex++;
		}
		else
		{
			if (isAlpha(rx_Str[0])) {
				if (rx_Str[0] == RLedTar) {
					//debugPrint("size of rx_str", sizeof(rx_Str));
					rTrgt = targetBuilder(rx_Str);
				}
				else if (rx_Str[0] == BLedTar)
				{
					bTrgt = targetBuilder(rx_Str);
				}
				else if (rx_Str[0] == GLedTar) {
					 gTrgt = targetBuilder(rx_Str);
				}
				else if (rx_Str[0] == xTarg) {
					jsXTarget = targetBuilder(rx_Str);
				}
				else if (rx_Str[0] == yTarg) {
					jsYTarget = targetBuilder(rx_Str);
				}
				rx_Str = "";
			}
			dataIndex = 0;
			//debugPrint("dataIndex", dataIndex);
		}
		
	}
}

// the setup function runs once when you press reset or power the board
void setup() {
	// put your setup code here, to run once:
	// degreee 
	pinMode(PinCLK, INPUT);
	// direction
	pinMode(PinDT, INPUT);
	//switch  
	pinMode(PinSW, INPUT);
	digitalWrite(PinSW, HIGH); // Pull-Up resistor for switch
	attachInterrupt(0, isr, LOW); // interrupt 0 always connected to pin 2 on Arduino UNO

	rotPrevState = digitalRead(PinCLK);

	//setup lights for O/P
	pinMode(Red, OUTPUT);
	pinMode(Blue, OUTPUT);
	pinMode(Green, OUTPUT);

	//Turn all off to begin
	digitalWrite(Red,LOW);
	digitalWrite(Blue,LOW);
	digitalWrite(Green,LOW);

	//joystick setup
	pinMode(jX, OUTPUT);
	digitalWrite(jX, LOW);
	pinMode(jY, OUTPUT);
	digitalWrite(jY, LOW);
	pinMode(jSW, INPUT);
	digitalWrite(jSW, HIGH);
	jsXpos = analogRead(jX);
	jsYpos = analogRead(jY);

	// joystick status LED
	pinMode(xLED, LOW);
	analogWrite(xLED, lightValX);
	delay(200);
	analogWrite(xLED, 0);
	pinMode(yLED, LOW);
	analogWrite(yLED, lightValY);
	delay(200);
	analogWrite(yLED, 0);

	//logging
	Serial.begin(9600);

	//debugPrint("setup rgb	", rgbInputVal);
}


//heavy lifting for Joystick stage
void jsDecision(boolean isX , int position) {
	
	int lightPin, sensorPin, sVal, lVal, target, curr, prev;
	
	
	sVal = position;
	if (isX) {
		lightPin = xLED;
		sensorPin = jX;
		prev = jsXpos;
		lVal = lightValX;
		target = jsXTarget;

	}
	else
	{
		lightPin = yLED;
		sensorPin = jY;
		prev = jsYpos; 
		lVal = lightValY;
		target = jsYTarget;
	}
	
		if (abs(sVal - target) <= 10) // win state
		{
			blink(lightPin);
			blink(lightPin);
			blink(lightPin);
			if (isX) { xPass = true; } 
			else { yPass = true;}

			if (xPass && yPass) {

				debugPrint("you won", 1337);
				jsState = false;
			}
		}
		else // progress info
		{
			// closer to target
			// need to actually write the value to the light
			if (abs(sVal - target) < abs(prev - target))
			{
				if (isX) { lightValX = changeLight(lightPin, lVal, true); }
				else { lightValY = changeLight(lightPin, lVal, true); }
			}
			else if (abs(sVal - target) > abs(prev - target));
			{
				if (isX) { lightValX = changeLight(lightPin, lVal, true); }
				else { lightValY = changeLight(lightPin, lVal, true); }
			}
		}

		// update position with new value
		if (isX) { jsXpos = sVal; }
		else{ jsYpos = sVal; }

}

 
// the loop function runs over and over again until power down or reset
void loop() {
	//debugPrint("start loop rgb	", rgbInputVal);
	if (rgbState) {
		
		rotCurrentState = digitalRead(PinCLK); // 0 or 1 Reads the "current" state of the twisty bit
											   // If the previous and the current state of the outputA are different, that means a Pulse has occured
		//printRotaryEncoderValues();
		if (rotCurrentState != rotPrevState) { // current clk vs old clk change has happened, which direction?
			
			prevRGBInputVal = rgbInputVal; // store old input 
			// If the DT state is different to the CLK state, that means the encoder is rotating clockwise
			// increment the value
			if (digitalRead(PinDT) != rotCurrentState) { // clockwise 
				rgbInputVal += 5;
				rgbInputVal = enforceLimit(rgbInputVal);
				debugPrint("light	", rgbInputVal);
				
			}
			else { // counter clockwise 
				// decrement
				rgbInputVal -= 5;
				rgbInputVal = enforceLimit(rgbInputVal);
			}


			//debugPrint("Position: ", rgbInputVal);
			analogWrite(rgbPin, rgbInputVal);
			//debugPrint("Position: ", rgbInputVal);
			// need to check against win state
			checkRGBTarget(rgbPin, rgbInputVal);
			delay(50); // slows down sensing process to keep from running inc/decrement many times in 1 second
		}
	}
	
	// issue: light does not get brighter and darker nicely 
	if (jsState) {
		printJoystick();

		// read in current positions
		int currX = analogRead(jX);
		int currY = analogRead(jY);

		// check that current position is outside the joystick's wiggle factor
		if (currX > (jsXpos + jsTolerance) || currX < (jsXpos - jsTolerance)) {
			//debugPrint("arX	", currX); 
			//debugPrint("tolerance", jsXpos + jsTolerance);
			jsDecision(true, currX);
			//debugPrint("X axis	", currX);
		}
		else if(140 < currX < 170) // check for a return to neutral position 
		{
			lightValX = 50;
			analogWrite(xLED, lightValX);
		}
		
		if (currY > jsYpos + jsTolerance || currY < jsYpos - jsTolerance) {
			jsDecision(false, currY);
		}
		else if (140 < currY < 170)
		{
			lightValY = 50;
			analogWrite(yLED, lightValY);
		}
		
		
	}
	
	
	
}
