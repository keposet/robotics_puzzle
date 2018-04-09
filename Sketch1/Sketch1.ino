/*
 Name:		Sketch1.ino
 Created:	4/4/2018 17:17:52
 Author:	jason
*/

//rotary code taken from https://howtomechatronics.com/tutorials/arduino/rotary-encoder-works-use-arduino/



// ----BEGIN Variable creation


//Joystick Pins 
#define jSW 7
#define	jX	0 // orange
#define	jY 1 //yellow
#define xLED 6
#define yLED 5

// joystick values using a 10k resistor will yield values roughly between 0 - 310
int jsXTarget = 50;
int jsYTarget= 220;
int lightValX = 50; 
int lightValY = 50;
int jsTolerance = 2; // allow for the sensor to have 4 degrees of wiggle room 
int lvChange = 35; // amount to inc/decrement status lights
int jsXpos; 
int jsYpos;


//stage flag
//boolean rgbState = true;
boolean rgbState = false;
boolean rPass = false;
boolean gPass = false;
boolean bPass = false;

boolean jsState = true;
boolean xPass = false;
boolean yPass = false;

//------END	Variable creation

//------ Helper Methods
void debugPrint(String message, int val) {
	Serial.println(message + val);
}

// Interrupt routine runs when SW reads LOW switched input pin when SW read HIGH again








void blink(int target) {
	digitalWrite(target, HIGH);
	delay(250);
	digitalWrite(target, LOW);
	delay(250);
}

int changeLight(int target, int val, boolean brighter) {

	
	debugPrint("old val", lightValX);
	debugPrint("brighter", brighter);
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
	debugPrint("new val	", val);
	analogWrite(target, val);


	delay(50);

	return val;
}



void printJoystick() {
	debugPrint("X axis	", analogRead(jX));
	debugPrint("Y axis	", analogRead(jY));
}

//--- END Helper Methods

// the setup function runs once when you press reset or power the board
void setup() {
	// put your setup code here, to run once:


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
}



//void jsDecision(int lightPin, int sensorPin, int sVal, int lVal, int target, ) {
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
			if (abs(sVal - target) < abs(prev - target))
			{
				if (isX) { lightValX = changeLight(lightPin, lVal, true); }
				else { lightValY = lightValX = changeLight(lightPin, lVal, true); }
			}
			else if (abs(sVal - target) > abs(prev - target));
			{
				if (isX) { lightValX = changeLight(lightPin, lVal, true); }
				else { lightValY = lightValX = changeLight(lightPin, lVal, true); }
			}
		}

		if (isX) { jsXpos = sVal; }
		else{ jsYpos = sVal; }

}


// the loop function runs over and over again until power down or reset
void loop() {
	

	if (jsState) {
		//printJoystick();
		int currX = analogRead(jX);
		int currY = analogRead(jY);
		if (currX > (jsXpos + jsTolerance) || currX < (jsXpos - jsTolerance)) {
			//debugPrint("arX	", currX); 
			//debugPrint("tolerance", jsXpos + jsTolerance);
			jsDecision(true, currX);
			debugPrint("X axis	", currX);
		}
		else if(152 < currX < 159)
		{
			lightValX = 50;
			analogWrite(xLED, lightValX);
		}
		
		if (currY > jsYpos + jsTolerance || currY < jsYpos - jsTolerance) {
			jsDecision(false, currY);
		}
		else if (152 < currX < 159)
		{
			lightValY = 50;
			analogWrite(yLED, lightValY);
		}
		
		
	}
	
	//digitalWrite(Blue, LOW);
	delay(500);
}
