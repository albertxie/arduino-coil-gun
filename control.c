/*
* Arduino Coil Controller
* 
* Program used to control three coils used to generate 
* electromagnetic fields in order to propel a ferromagnetic projectile.
*
* Version:						release 1.0 
* Author: 						Albert Xie, Connor McMillan
* Revised: 						January 9, 2014
* Serial Connection: 			BAUD 9600
* Serial Port (Linux): 			/dev/ttyACM0
* Serial Port (Windows):		COM0
*/

//Declaring constants
char command = ' ';				//fire command
char clr = 'c';                 //clear command
char initDebug = 'd';           //debug sequence command
int onTime = 75; 				//milliseconds of power provided to coil
int relay[] = {2, 3, 4};		//array of relays that control the coils
int sensor[] = {1, 0};			//array of light sensors
int threshold[] = {990, 1000};  //minimum threshold used to trigger the photo-gates
boolean debugger = true;		//set true to enable debugging messages


/*
* function used to set pin mode of relay and light sensors
* and to create a serial connection between Arduino to computer
*/
void setup()
{
  //initializing relay pins
  for(int x = 0; x< (sizeof(relay) / sizeof(int)); x++)
  {
    pinMode(relay[x], OUTPUT);
  }

  //initializing light sensor pins
  for(int x = 0; x< (sizeof(sensor) / sizeof(int)); x++)
  {
    pinMode(sensor[x], INPUT);
  }

  //creating BAUD 9600 serial connection
  Serial.begin(9600);

  if(debugger) Serial.println("EMMA Controller - ONLINE\n");
}

/*
* main function
*/
void loop()
{
  //checking for a valid serial connection
  if(Serial.available() > 0)
  {
    //get user input from serial
    char input = Serial.read();

    //checking for fire command from serial
    if(input == command)
    {
  	  //setting up variables to get velocity between start an photo gate 1
      int startTime = millis();
      int dT;
      int v;
      
      //enable relay 1
      digitalWrite(relay[0], HIGH);
      
      //using a infinite loop to get a constant stream of values from the photo-gate
      while(true)
      {
      	//enables built int safe, the coil cannot be powered for more than 75 ms
      	//if the analogReading on the sensors detects a pass
      	//turns off the coil and calculate the average velocity of the projectile
      	// from the start to the photo gate
        if(checkTime(startTime, onTime) || (analogRead(sensor[0]) < threshold[0]))
        {
          digitalWrite(relay[0], LOW);
          dT = millis() - startTime;
          v = dT/8;
          Serial.println("Coil 1 fired.");
          break;
        }
      }

      //setting up variables to get velocity between start an photo gate 1
      int startTime2 = millis();
      int dT2;
      int v2;
      int t2;

      //enable relay 2
      digitalWrite(relay[1], HIGH);

      //using a infinite loop to get a constant stream of values from the photo gate
      while(true)
      {
      	//using the predetermined 
        t2 = 4/v;

        ///enables built in safety, the coil cannot be powered for more than 75 ms
      	//if the analogReading on the sensors detects a pass
      	//turns off the coil and calculate the average velocity of the projectile
      	// from the start to the photo-gate
        if(checkTime(startTime2, t2) || analogRead(sensor[1]) < threshold[1])
        {
          digitalWrite(relay[1], LOW);
          dT2 = millis() - startTime2;
          v2 = dT2 / 8;
          Serial.println("Coil 2 fired.");
          break;
        }
      }
      
      //since there isn't a third photo gate, it is possible to use the last known velocity to approximate the time required to 
      int dT3 = 4/v2;

      //toggling the final relay with a dT3 milliseconds in between
      digitalWrite(relay[2], HIGH);
      delay(20);
      digitalWrite(relay[2], LOW);
      Serial.println("Coil 3 fired.");

      //safety measure to ensure that the jumpers, breadboard
      //and the coils won't get too hot by shutting down the 
      //system to 3 seconds
      Serial.println("Firing complete, cooling down.");
      delay(3000);
      Serial.println("Cool Down Complete.\n\nSystem Ready");
    }
    
    //if the input reads the command to clear all data
    else if(input == clr)
    {
      resetAll();
    }

    //if the input reads the command to debug and run diagnostic sequence
    else if(input == initDebug)
    {
      debug();
    }

    //command error
    else
    {
      Serial.println("Invalid Command");
    }
  }
}

//function as a work around to the delay method
//combined with a while loop, this function allows the program to delay without
//pausing the entire system
//@param int initial time - the initial to be compared
//@param int total - the amount of time the function has to delay
boolean checkTime(int initial, int total)
{
  if(millis() - initial >= total)
  {
    return true;
  }
  
  else
  {
    return false;
  }
}

//function to reset variables and relays to default position
void resetAll()
{
  if(debugger) { Serial.println("Reset sequence initiated.");}
  
  //iterates through 
  for(int x = 0; x< (sizeof(relay) / sizeof(int)); x++)
  {
    digitalWrite(relay[x], LOW);
  }
  
  if (debugger)
  {
    Serial.println("Relays off.");
    Serial.println("Reset sequence complete.\n");
  }
}

//function to diagnose circuit and relay for faulty connections
void debug()
{
  Serial.println("Debugging process initiated.");

  //iterating through relay array and turn each array on and off for 500 milliseconds
  for(int x = 0; x< (sizeof(relay) / sizeof(int)); x++)
  {
    digitalWrite(relay[x], HIGH);
    delay(500);
    digitalWrite(relay[x], LOW);
    delay(500);
    Serial.println("Coil: " + String(x) + " tested. at" + String(relay[x]));
  }

  //iterating through each photo gate and checks to determine if
  //the default reading is within +/- 10 of the predetermined value
  for(int x = 0; x< (sizeof(sensor) / sizeof(int)); x++)
  {
    int reading = analogRead(sensor[x]);

    if((threshold[x] -10 <= reading) && (threshold[x] + 10 >= reading))
    {
      Serial.println("Light sensor " + String(x) + " within accepted range. - " + String(reading) + " "+String(sensor[x]));
    }

    else
    {
      Serial.println("WARNING: Light sensor " + String(x) + " requires calibration! - " + String(reading)+" "+String(sensor[x]));
    }
  }
  Serial.println("Debug sequence complete.\n");
}



