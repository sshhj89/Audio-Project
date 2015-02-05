#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <avr/wdt.h>

// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  //Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

#define REC_BUTTON 8
static int num = 0;

char** fileName = NULL;
int currentFile = 0;
int totalFile = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Library Test");
  pinMode(REC_BUTTON, INPUT_PULLUP);
  
  // initialise the music player
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println("Couldn't find VS1053, do you have the right pins defined?");
     while (1);
  }
  Serial.println(F("VS1053 found"));

 // musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
 
  
  if (!SD.begin(CARDCS)) {
    Serial.println("SD failed, or not present");
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  
//  File firstRoot = SD.open("/",FILE_READ);
   printDirectory( SD.open("/",FILE_READ), 0);
 // firstRoot.flush();
//  firstRoot.close();
  // Set volume for left, right channels. lower numbers == louder volume!
  //musicPlayer.setVolume(20,20);
  
 // if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT))
//    Serial.println("DREQ pin is not an interrupt pin");
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
     Serial.println("DREQ pin is not an interrupt pin");
}

File recording;  // the file we will save our recording to
#define RECBUFFSIZE 128  // 64 or 128 bytes.
uint8_t recording_buffer[RECBUFFSIZE];
uint8_t isRecording = false;

void loop() {  

  if( digitalRead(REC_BUTTON) == HIGH && musicPlayer.playingMusic == false)
  {
    if (! musicPlayer.startPlayingFile("sample3.ogg")  ) {
      Serial.println("Could not open file track001.mp3");
      while (1);
    }
  }

  while (musicPlayer.playingMusic) {
    
    if(digitalRead(REC_BUTTON) == LOW)
    {
       musicPlayer.stopPlaying();
       //delay(1000);
       if(musicPlayer.stopped() && !isRecording)
       {
         musicPlayer.reset();
       if (! musicPlayer.prepareRecordOgg("v44k1q05.img")) {
           Serial.println("Couldn't load plugin!");
         while (1);    
        }
        break;
       }
       
    }
  }
  
  
  if(musicPlayer.playingMusic == false)
  {
    Serial.println("comin");
    musicPlayer.stopPlaying();
    delay(2000);
    if(musicPlayer.stopped())
    {
     //software_Reset();
       //software_Reboot();
      // SPI.setClockDivider(SPI_CLOCK_DIV128);
      // printDirectory( SD.open("/",FILE_READ), 0);
    
      
      
    }
    delay(200);
    
  }
  
  if (!isRecording && !digitalRead(REC_BUTTON)) {
    Serial.println("Begin recording");
    isRecording = true;
    
    // Check if the file exists already
    char filename[15];
    strcpy(filename, "RECORD00.OGG");
    for (uint8_t i = 0; i < 100; i++) {
      filename[6] = '0' + i/10;
      filename[7] = '0' + i%10;
      // create if does not exist, do not open existing, write, sync after write
      if (! SD.exists(filename)) {
        Serial.println("exist filename");
        break;
      }
    }
    Serial.print("Recording to "); Serial.println(filename);
    recording = SD.open(filename, FILE_WRITE);
    if (! recording) {
       Serial.print("Recording value: "); Serial.println(recording);
       Serial.println("Couldn't open file to record!");
       while (1);
    }
    musicPlayer.startRecordOgg(false); // use microphone (for linein, pass in 'false')
    //delay(10);
  }
  
 if (isRecording)
    saveRecordedData(isRecording);
    
 if (isRecording && digitalRead(REC_BUTTON)) {
    Serial.println("End recording");
    musicPlayer.stopRecordOgg();
    isRecording = false;
    // flush all the data!
    saveRecordedData(isRecording);
    // close it up
    recording.close();
   // delay(100);
    musicPlayer.reset();
  
  }
  
  
  
}

void software_Reset()
// Restarts program from beginning but 
// does not reset the peripherals and registers
{
  asm volatile ("  jmp 0");  
} 


void software_Reboot()
{
  wdt_enable(WDTO_15MS);
  while(1)
  {
  }
}

uint16_t saveRecordedData(boolean isrecord) {
  uint16_t written = 0;
  //delay(10);
    // read how many words are waiting for us
  uint16_t wordswaiting = musicPlayer.recordedWordsWaiting();

  
  // try to process 256 words (512 bytes) at a time, for best speed
  while (wordswaiting > 256) {
   
    // for example 128 bytes x 4 loops = 512 bytes
    for (int x=0; x < 512/RECBUFFSIZE; x++) {
      // fill the buffer!
      for (uint16_t addr=0; addr < RECBUFFSIZE; addr+=2) {
        uint16_t t = musicPlayer.recordedReadWord();
        //Serial.println(t, HEX);
        recording_buffer[addr] = t >> 8; 
        recording_buffer[addr+1] = t;
      //   Serial.print("value 1: "); Serial.println(recording_buffer[addr] , HEX);
      //   Serial.print("value 2: "); Serial.println(recording_buffer[addr+1], HEX);
      }
      if (! recording.write(recording_buffer, RECBUFFSIZE)) {
            Serial.print("Couldn't write "); Serial.println(RECBUFFSIZE); 
            while (1);
      }
    }
    // flush 512 bytes at a time
    recording.flush();
    written += 256;
    wordswaiting -= 256;
  }
  
  
  wordswaiting = musicPlayer.recordedWordsWaiting();
  if (!isrecord) {
    Serial.print(wordswaiting); Serial.println(" remaining");
    // wrapping up the recording!
    uint16_t addr = 0;
    for (int x=0; x < wordswaiting-1; x++) {
      // fill the buffer!
      uint16_t t = musicPlayer.recordedReadWord();
      recording_buffer[addr] = t >> 8; 
      recording_buffer[addr+1] = t;
      if (addr > RECBUFFSIZE) {
          if (! recording.write(recording_buffer, RECBUFFSIZE)) {
                Serial.println("Couldn't write!");
                while (1);
          }
          recording.flush();
          addr = 0;
      }
    }
    if (addr != 0) {
      if (!recording.write(recording_buffer, addr)) {
        Serial.println("Couldn't write!"); while (1);
      }
      written += addr;
    }
    musicPlayer.sciRead(VS1053_SCI_AICTRL3);
    if (! (musicPlayer.sciRead(VS1053_SCI_AICTRL3) & _BV(2))) {
       recording.write(musicPlayer.recordedReadWord() & 0xFF);
       written++;
    }
    recording.flush();
  }

  return written;
}

/// File listing helper
void printDirectory(File dir, int numTabs) {
   int count = 0;
   totalFile = 0;
   Serial.print("num: ");
   Serial.println(num);
    num++;

   while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       //dir.rewindDirectory();
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
      if(!String(entry.name()).equals("V44K1Q05.IMG"))
       {
               
                count++;
        }
     }
     entry.close();
   }
  
   totalFile = count;
   Serial.print("total Count: ");
   Serial.println(totalFile);
 
}

