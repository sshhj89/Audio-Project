#include <SD.h>

File root;
File entry;

void setup()
{
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
    }
    
    Serial.print("Initializing SD card...");
    // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
    // Note that even if it's not used as the CS pin, the hardware SS pin 
    // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
    // or the SD library functions will not work. 
    pinMode(4, OUTPUT);

    if (!SD.begin(4)) {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");

    root = SD.open("/");

    printDirectory(root);

    Serial.println("done!");
}

void loop()
{
    // nothing happens after setup finishes.
}

void printDirectory(File dir) {
    while(true) {
     
        entry =  dir.openNextFile();
        if (!entry) {
            // no more files
            Serial.println("**no more files**");
            break;
        }
            if (entry.isDirectory()) {
            // Serial.print("is a Dir");
            //do nothing
        } else if (entry.size() != 0){
            // files have sizes, directories do not
            // Serial.print("found a file");
            Serial.print(entry.name());
            Serial.print("\n");
            // Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}