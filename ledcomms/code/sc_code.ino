/***********************************************************************************************/
// Summer Challenge Code - VLC Texting using the VLC Transciever shield
//
// Instructions:                                
//   Connect the arduino to the USB Port. 
//   Go to Tools and ensure that Board is set to Arduino Leonardo. 
//   Go to Tools and set Port to the port which the Arduino is attached to. 
//   Click the upload button and wait for the program to upload to the Arduino. 
//   Open the Serial Monitor in the top right corner (magnifying glass button). 
//   Wait for your partner to do the same.
//   Text away!!
/***********************************************************************************************/
String receivedMessage = "";  // Empty string to hold recieved output
String outMessage = "";       // Empty string to hold input

/**************************************/
/*************** SETUP ****************/
/**************************************/
void setup(){ 
  Serial1.begin(9600);  // begin Serial communication with Arduino
  Serial.begin(9600);   // begin Serial communication with Arduino
  while(!Serial);       // tells Arduino to do nothing while not communicating over Serial (required for Leonardo)
  while(!Serial1);      // tells Arduino to do nothing while not communicating over Serial (required for Leonardo)
} 

/**************************************/
/************* MAIN LOOP **************/
/**************************************/
void loop() {
  char inChar;          
  outMessage = "";                        // Initialize the outgoing message 
  receivedMessage = "";                   // Initialize the receieved message
  if (Serial.available() > 0) {
  
    while (Serial.available() > 0) {      // check if at least 1 char is available 
      inChar = Serial.read();             // read any characters typed in the Serial monitor and set inChar equal to them
      outMessage.concat(inChar);          // add inChar to outMessage
      delay(1);
    }
   
    if (outMessage != "") {               // if the message typed is not a blank string
      outMessage.concat('\n');            // add '\n' to the end of the string
      Serial.println("Sent:  ");          // say what you sent
      Serial.println(outMessage);         // Print the message in Serial Monitor 1 
      Serial1.print(outMessage);          // Send to Arduino 2
    }
  }
  
  if (Serial1.available() > 0) {          // check if something has been sent to TX         
    char outChar = 0;                     // placeholder character
    while (outChar != '\n') {             // until a '\n' is read...
      if (Serial1.available() > 0) {      // ensure that valid characters are sent
        outChar = Serial1.read();         // if so, set outChar equal to the characters sent
        receivedMessage.concat(outChar);  // concatonate each character to the received message at the other Arduino
      }
      delay(1);
    }
    Serial.println("Received: ");         // say what you received on the other end
    Serial.println(receivedMessage);      // print the received message on the other monitor
  }
}
