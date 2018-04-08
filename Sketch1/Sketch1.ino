/*
 Name:		Sketch1.ino
 Created:	4/4/2018 17:17:52
 Author:	jason
*/

//rotary code taken from https://howtomechatronics.com/tutorials/arduino/rotary-encoder-works-use-arduino/



// ----BEGIN Variable creation
//Rotary Pins
#define PinCLK 4 //grey
#define PinDT 3 //black
#define PinSW 2 //yellow

// rotary flag and values
int rotPosition = 0;
int rotPrevPos; 
int rotCurrentState;
int rotPrevState;
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

// joystick values
int jsXTarget = 700;
int jsYTarget= 700;
int lightValX = 100; 
int lightValY = 100;
int jsXpos; 
int jsYpos;

//colour values
int rgbPin = 9; // used for switching input channel 
int prevPin;
int rTrgt = 242;
int bTrgt = 180;
int gTrgt = 3; 

//stage flag
boolean rgbState = true;
boolean rPass = false;
boolean gPass = false;
boolean bPass = false; 

//------END	Variable creation

//------ Helper Methods
void debugPrint(String message, int val) {
	Serial.println(message + val);
}

// Interrupt routine runs when SW reads LOW switched input pin when SW read HIGH again
void isr() {
	delay(4);  // delay for Debouncing
	//debugPrint("pinState ", digitalRead(PinSW));
	if(digitalRead(PinSW) == 1){
		rgbPin++;
		if (rgbPin > 11) rgbPin = 9;

		debugPrint("pin ", rgbPin);
	}
}

int enforceLimit(int num) {
	if (num > 255) {
		num = 0; 
	} else if (num <0){
		num = 255; 
	}
	return num;
}

// will need current light value, light pin, and target
void checkRGBTarget(int rgb, int lVal) {
	int target;
	if (rgb == Red) {
		target = rTrgt;
	}
	else if (rgb == Green)
	{
		target = gTrgt;
	}
	else if (rgb == Blue)
	{
		target = bTrgt;
	}
	if (lVal - target <= 10 && target - lVal >= -10 ) {
		debugPrint("lval ", lVal);
		debugPrint("target", target);
		// win 
		blink(rgb);
		blink(rgb);
		blink(rgb);
		if (rgb == Red) {
			rPass = true;
		}
		else if (rgb == Green)
		{
			gPass = true;
		}
		else if (rgb == Blue)
		{
			bPass = true; 
		}
		rgbStateGate();
	}
}

void rgbStateGate() {
	if (rPass == true && gPass == true && bPass == true) {
		rgbState = false; 
	}
}

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

	//logging
	Serial.begin(9600);
}

/*
void rgbAction() {
	debugPrint("rgbAction", 0);
	rotCurrentState = digitalRead(PinCLK); // Reads the "current" state of the outputA
										   // If the previous and the current state of the outputA are different, that means a Pulse has occured
	debugPrint("rotCurrentState ", rotCurrentState);
	debugPrint("rot Previous", rotPrevState);
	if (rotCurrentState != rotPrevState) {
		rotPrevPos = rotPosition;
		// If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
		if (digitalRead(PinDT) != rotCurrentState) {
			rotPosition = rotPrevPos + 6;
			rotPosition = enforceLimit(rotPosition);
		}
		else {
			rotPosition = rotPrevPos - 6;
			rotPosition = enforceLimit(rotPosition);
		}
		//take position and map it 
		// use map output as input for light 

		debugPrint("Position: ", rotPosition);
		analogWrite(rgbPin, rotPosition);
		// need to check against win state
		checkRGBTarget(rgbPin, rotPosition);

	}
	rotPrevState = rotCurrentState; // Updates the previous state of the outputA with the current state
}
*/

//void jsDecision(int lightPin, int sensorPin, int sVal, int lVal, int target, ) {
void jsDecision(boolean isX , int position) {
	//debugPrint("X axis", analogRead(jX));
	//debugPrint("Y axis", analogRead(jY));
	
	int lightPin;
	int sensorPin;
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

	//debugPrint("X axis", analogRead(jX));
	//debugPrint("Y axis", analogRead(jY));
	if (jsXpos != analogRead(jX)){ jsDecision(true, analogRead(jX));}
	if (jsYpos != analogRead(jY)) { jsDecision(false, analogRead(jY)); }
	
	delay(750);
}

// the loop function runs over and over again until power down or reset
void loop() {
	debugPrint("top of loop", 0);
	if (rgbState) {
		//debugPrint("rgb phase", 0);
		//rgbAction();
		debugPrint("rgbAction", 0);
		rotCurrentState = digitalRead(PinCLK); // Reads the "current" state of the outputA
											   // If the previous and the current state of the outputA are different, that means a Pulse has occured
		debugPrint("rotCurrentState ", rotCurrentState);
		debugPrint("rot Previous", rotPrevState);
		if (rotCurrentState != rotPrevState) {
			rotPrevPos = rotPosition;
			// If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
			if (digitalRead(PinDT) != rotCurrentState) {
				rotPosition = rotPrevPos + 6;
				rotPosition = enforceLimit(rotPosition);
			}
			else {
				rotPosition = rotPrevPos - 6;
				rotPosition = enforceLimit(rotPosition);
			}
			//take position and map it 
			// use map output as input for light 

			debugPrint("Position: ", rotPosition);
			analogWrite(rgbPin, rotPosition);
			// need to check against win state
			checkRGBTarget(rgbPin, rotPosition);
			rotPrevState = rotCurrentState; // Updates the previous state of the outputA with the current state
		}
		
	}
	else
	{		
		jsSense();
	}
	//digitalWrite(Blue, LOW);
	delay(500);
}
