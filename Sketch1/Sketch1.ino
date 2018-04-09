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
int lightValX = 100; 
int lightValY = 100;
int jsXpos; 
int jsYpos;


//stage flag
//boolean rgbState = true;
boolean rgbState = false;
boolean rPass = false;
boolean gPass = false;
boolean bPass = false; 

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
	
	if (brighter) {val += 10;}
	else{val -= 10;}
	analogWrite(target, val);

	return val;
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

	//logging
	Serial.begin(9600);
}



//void jsDecision(int lightPin, int sensorPin, int sVal, int lVal, int target, ) {
void jsDecision(boolean isX , int position) {
	//debugPrint("X axis", analogRead(jX));
	//debugPrint("Y axis", analogRead(jY));
	
	int lightPin, sensorPin;
	int sVal;
	int lVal;
	int target;
	int curr;
	int prev;
	
	if (isX) {
		int lightPin = xLED;
		int sensorPin = jX;
		int sVal = position; 
		int lVal = lightValX;
		int target = jsXTarget;
	}
	else
	{
		int lightPin = yLED;
		int sensorPin = jY;
		int sVal = position;
		int lVal = lightValY;
		int target = jsYTarget;
	}
	
		curr = analogRead(sensorPin);
		prev = sVal;
		//jsXpos = analogRead(jX);
		if (curr - target <= 50)
		{
			blink(xLED);
			blink(xLED);
			blink(xLED);
			// won something yaya
		}
		else
		{
			// closer to target
			if (curr - target < prev - target)
			{
				lVal = changeLight(lightPin, lVal, true);
			}
			else // further from target
			{
				lVal = changeLight(lightPin, lVal, false);
			}
		}
		if (isX) { jsXpos = curr; }
		else { jsYpos = curr; }; 

}

void jsSense() {

	debugPrint("X axis", analogRead(jX));
	debugPrint("Y axis", analogRead(jY));
	if(jsXpos != analogRead(jX)){ jsDecision(true, analogRead(jX));}
	//if (jsYpos != analogRead(jY)) { jsDecision(false, analogRead(jY)); }
	
	
}

// the loop function runs over and over again until power down or reset
void loop() {
	
	if (rgbState) {
	
	}
	else
	{		
		jsSense();
	}
	//digitalWrite(Blue, LOW);
	delay(500);
}
