// include SPI, Music Maker and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create shield object
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

// Random File to Play
char randomFTP[20];
int fileToPlay;
  
void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Simple Test");

  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));
  
  if (!SD.begin(CARDCS)) {                          // if SD card not found
    Serial.println(F("SD failed, or not present"));
    while (1);                                        // don't do anything
  }

  // list files in SD card
  printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels
  // each int = -0.5dB, therefore higher number = lower volume
  musicPlayer.setVolume(0,0);

  // If DREQ is on an interrupt pin we can do background audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
}

void loop() {
  uint16_t gpio_out = 0;

  // get gpio input for control
  gpio_out = musicPlayer.GPIO_digitalRead();
  Serial.println(gpio_out);

  // get serial input for control
  char c = Serial.read();

  // if we get a 'b' on the serial console, begin playing
  if((c == 'b')||(gpio_out == 2)) 
  {
    musicPlayer.stopPlaying();
    fileToPlay = random(1, 28);
    sprintf(randomFTP, "(%d).mp3", fileToPlay);
    Serial.print("Playing ");
    Serial.println(randomFTP);
    musicPlayer.startPlayingFile(randomFTP);
  }
  else if(gpio_out == 4) 
  {
    musicPlayer.stopPlaying();
    musicPlayer.startPlayingFile("facili.mp3");
  }
  else if(gpio_out == 8) 
  {
    musicPlayer.stopPlaying();
    musicPlayer.startPlayingFile("blastin.mp3");
  }
  else if(gpio_out == 16) 
  {
    musicPlayer.stopPlaying();
    musicPlayer.startPlayingFile("darude.mp3");
  }
  delay(100);
}


/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
