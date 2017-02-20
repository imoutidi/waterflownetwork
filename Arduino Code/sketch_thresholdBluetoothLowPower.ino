//In this sketch we read the pulses from the flow meter 
//and transmit them to the serial port 
//We make the transmision when the flowmeter stops to detect pulses
//and if the pulses exeed a given threshold

//The software serial library helps to transmit data with the bluetooth device
// using the serial port
#include <SoftwareSerial.h>
//The LowPower library is used in order to power down the arduino and save power
#include "LowPower.h"

        // Declaring variables
        SoftwareSerial blueSerial(10,11);   //10 reading pin RX, 11 transmit pin TX, for the bluetooth device
        int flowPin = 2;                   // Arduino flowmeter pin number
        int pulsesThreshold = 400;         // Threshold of pulses in case of a leak	
	float pulsesCounter;               // Main pulses counter
	float pulsesAux;                   // Auxiliary counter
	float pulsesPrev;                  // Auxiliary counter to track pulses activity between loops
        int loopsNumber = 0;

        /**
	 *  Interrupt Service Routine for interrupt 0 (ISR0)
	 *  ISR0 services an interrupt condition on Pin 2 - whenever voltage on that pin rises.
	 */ 
	void rpm ()    
	{
          loopsNumber = 0;
	  pulsesCounter++; // Every RISING pulse causes pulsesCounter to increase by one.
	}

	void setup()
	{
	  pinMode(flowPin, INPUT);           // Initialize the digital pin as an input
	  //Serial.begin(9600);              // Starting serial port (USB)
          blueSerial.begin(9600);            // Starting serial port (Bluetooth)
          attachInterrupt(0, rpm, RISING);   // Interrupt is attached to rpm function
	  sei();                             // Enable interrupt 0 on Pin 2 for a RISING signal.
	}

	void loop ()
	{
	      cli();                         // Disable interrupts to check the counter
	      pulsesAux = pulsesCounter;     // Copy the ISR variable (main counter). Don't corrupt the counting process
	      sei();                         // Enable interrupts

	      // If pulses are non-zero and doesn't change during a loop (delay time: ~1sec), 
	      // send the data to the Serial port
	      if ( ((pulsesAux != 0)  && (pulsesPrev == pulsesAux)) || ((pulsesAux != 0) && (pulsesAux > pulsesThreshold))) {    // pulsesPrev == pulsesAux means that the flow has stoped

                  blueSerial.print("pulses:");         // Sending the string via blutooth
                  blueSerial.println(pulsesAux, DEC);  // Sending the pulses counter via bluetooth
	          cli();               // Disable interrupts
	          pulsesCounter = 0;   // Reset main counter
	          sei();               // Enable interrupts
	          pulsesPrev = 0;      // Reset previous counter    
	          pulsesAux = 0;       // Reset auxiliary counter
	      }
              //if the flow stops for a number of loops we power down the arduino
              if (pulsesPrev == pulsesAux){
                  loopsNumber++;
              }                
              if (loopsNumber > 8){
                 LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
              }              
                            
	      cli();                   // Disable interrupts to copy the pulses count
	      pulsesPrev = pulsesAux; 
	      sei();                   // Enable interrupts
	      delay(1000);             // Delay time between loops 1sec
	}













