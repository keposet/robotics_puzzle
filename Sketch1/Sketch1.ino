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
int rgbInputVal = 0;
int prevRGBInputVal; 
int rotCurrentState; // current value(great dnb producer) of the clk pin
int rotPrevState;	// previous value of clk pin
volatile boolean ButtonPressed;  // need volatile for Interrupts

//RGB pins -- these must be pwm 
#define Red 11
#define Green 10
#define Blue 9


//colour values
int rgbPin = 9; // used for switching input channel 
int prevPin;
int rTrgt = 222;
int bTrgt = 180;
int gTrgt = 115; 

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
	if (lVal - target <= 10 && lVal - target >= -10 ) {
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
		debugPrint("you won", 1337);
		debugPrint("you won", 1337);
		debugPrint("you won", 1337);
		debugPrint("you won", 1337);
		debugPrint("you won", 1337);
		debugPrint("you won", 1337);
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

void printRotaryEncoderValues() {
	debugPrint("current clk ", rotCurrentState);
	debugPrint("Previous clk", rotPrevState);
	debugPrint("current dt", digitalRead(PinDT));
	debugPrint("switch", digitalRead(PinSW));
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
		prevRGBInputVal = rgbInputVal;
		// If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
		if (digitalRead(PinDT) != rotCurrentState) {
			rgbInputVal = prevRGBInputVal + 6;
			rgbInputVal = enforceLimit(rgbInputVal);
		}
		else {
			rgbInputVal = prevRGBInputVal - 6;
			rgbInputVal = enforceLimit(rgbInputVal);
		}
		//take position and map it 
		// use map output as input for light 

		debugPrint("Position: ", rgbInputVal);
		analogWrite(rgbPin, rgbInputVal);
		// need to check against win state
		checkRGBTarget(rgbPin, rgbInputVal);

	}
	rotPrevState = rotCurrentState; // Updates the previous state of the outputA with the current state
}
*/



// the loop function runs over and over again until power down or reset
void loop() {
	if (rgbState) {
		rotCurrentState = digitalRead(PinCLK); // 0 or 1 Reads the "current" state of the twisty bit
											   // If the previous and the current state of the outputA are different, that means a Pulse has occured

		if (rotCurrentState != rotPrevState) { // current clk vs old clk
			prevRGBInputVal = rgbInputVal; // store old input 
			// If the DT state is different to the CLK state, that means the encoder is rotating clockwise
			// increment the value
			if (digitalRead(PinDT) != rotCurrentState) {
				rgbInputVal += 2;
				rgbInputVal = enforceLimit(rgbInputVal);
			}
			else {
				// decrement
				rgbInputVal -= 2;
				rgbInputVal = enforceLimit(rgbInputVal);
			}


			debugPrint("Position: ", rgbInputVal);
			analogWrite(rgbPin, rgbInputVal);
			// need to check against win state
			checkRGBTarget(rgbPin, rgbInputVal);
		}
	}
		
		
	//digitalWrite(Blue, LOW);
	//delay(1000);
}
