#include <AFMotor.h>
#define pushButtonPin A5
#define dipSwitchPin1 A2
#define dipSwitchPin2 A3
#define dipSwitchPin3 A4

//5V & GND just changes what ON triggers; if you set it to 5V, it becomes HIGH; if connected to GND, ON becomes LOW
//INPUT_PULLUP in pinMode(), it needs to be connected to GND 

AF_DCMotor DCmotor(1, MOTOR12_1KHZ);
int curFloor = 1; // assumes the machine is physically at floor 0



void setup() {
  // put your setup code here, to run once:
  pinMode(pushButtonPin, INPUT_PULLUP);  // set the pushButtonPin to being input, with a pullup resistor connected
  Serial.begin (115200);    // set up Serial library at 115200 bps

  pinMode(dipSwitchPin1, INPUT_PULLUP); //pinmode for dipSwitch1  
  pinMode(dipSwitchPin2, INPUT_PULLUP); //pinmode for dipSwitch2
  pinMode(dipSwitchPin3, INPUT_PULLUP); //pinmode for dipSwitch3


  // Turn off motor 
  DCmotor.run(RELEASE);
}

void loop()
{
  waitForButtonPush(); // wait for infinity

  int targetfloor = getDipSwitchFloor();

  Serial.print("Going from floor: [");
  Serial.print(curFloor);
  Serial.print("] to: ");
  Serial.println(targetfloor);
  
  goToFloor(targetfloor);
  Serial.print("Done. We are now at floor: ");
  Serial.println(curFloor);
}







int getDipSwitchFloor(){ //Convert binary to decimal returns this value as the target floor
    int state1=digitalRead(dipSwitchPin1);
    int state2=digitalRead(dipSwitchPin2);
    int state3=digitalRead(dipSwitchPin3);
    if (state1 == 0 && state2 == 0 && state3 == 0){
        return 0;//NOT A FLOOR IGNORE
    }else if (state1 == 0 && state2 == 0 && state3 == 1){ //001 = 1
        return 1;
    }else if (state1 == 0 && state2 == 1 && state3 == 0){ //010 = 2
        return 2;
    }else if (state1 == 0 && state2 == 1 && state3 == 1){ //011 = 3
        return 3;
    }else if (state1 == 1 && state2 == 0 && state3 == 0){ //100 = 4
        return 4;
    }else if (state1 == 1 && state2 == 0 && state3 == 1){ //101 = 5
        return 5;
    }else if (state1 == 1 && state2 == 1 && state3 == 0){ //110 = 6
        return 6;
    }
}


void waitForButtonPush(){
    while(true){
      if(digitalRead(pushButtonPin) == LOW)break;
      delay(10);
    }
}

void waitForIRChange(){
    bool prevState = digitalRead(A0);
    while(true){
    delay(100);
    if(digitalRead(A0) != prevState)break;
    }
}

void waitForFloorChange()
{
  Serial.print("  - Waiting for floor level to change: ... ");
  waitForIRChange();
  delay(250);
  waitForIRChange();
  delay(250);
  Serial.println("Done!");
}

void goToFloor(int targetFloor)
{
  int difference = targetFloor - curFloor; if(difference==0)return;
  int direction = difference > 0 ? FORWARD : BACKWARD;
  int steps = abs(difference);

  DCmotor.run(direction);
  DCmotor.setSpeed(255);

  while(steps>0)
  {
    waitForFloorChange();
    steps--;
  }
  
  DCmotor.run(RELEASE);
  curFloor = targetFloor;
}
