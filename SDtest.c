#include <SD.h>

File root;
File entry;
int count; 

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

    //reopens the root file for each op to avoid a bug indicating there are no more files to read.
    //http://stackoverflow.com/questions/16971084/how-to-recheck-file-in-arduino-sd-card
    root = SD.open("/");
    // if(root) {
    //     countFiles(root);
    //     root.close();
    // }
    // else {
    //     Serial.println("Failed to open directory");
    // }

    // char *filesArr[count]; //***NOT DECLARED IN THE RIGHT PLACE?****

    root = SD.open("/");
    if(root) {
        writeDirectory(root);        
        root.close();
    }
    else {
        Serial.println("Failed to open directory");
    }
}

void loop()
{
    // nothing happens after setup finishes.
}
int countFiles(File dir) {
    Serial.println("Counting files...");
    while(true) {
     
        entry =  dir.openNextFile();
        if (!entry) {
            // no more files
            Serial.println("No more files to count.");
            break;
        }
            if (entry.isDirectory()) {
            //Serial.print("is a Dir");
            //do nothing
        } else if (entry.size() != 0){
            // files have sizes, directories do not
            count++;
            Serial.println(count);
            Serial.print("\n");
        }
    }
}

void writeDirectory(File dir) {
    countFiles(root);
    // root.close();

    char *filesArr[count]; //***NOT DECLARED IN THE RIGHT PLACE?****
    Serial.println("Writing files...");
    // while(true) {
        for(int i = 0; i <= count; i++){
            Serial.println(root); //returns 1?
            entry =  dir.openNextFile();
            Serial.println(entry); //returns 0? 
            if (!entry) {
                // no more files
                Serial.println("No more files to write.");
                Serial.println(*filesArr);
                // Serial.println(&filesArr[i]);
                break;
            }
            else if (entry.isDirectory()) {
                Serial.print("is a Dir");
                //do nothing
            } else if (entry.size() != 0){
                // files have sizes - directories do not
                Serial.print(entry.name());
                filesArr[i] = entry.name();
                Serial.print("\n");
            }
            else{
                Serial.print("got to the end ");
            }
            entry.close();
        }
        entry.close();
    // }
}
/*
Initializing SD card...initialization done.
Counting files...
1

2

3

4

5

6

No more files to count.
Writing files...
1
0
No more files to write.

1
0
No more files to write.

1
0
No more files to write.

1
0
No more files to write.

1
0
No more files to write.

1
0

*/

/* without While loop
Initializing SD card...initialization done.
Counting files...
1

2

3

4

5

6

No more files to count.
Writing files...
1
0
No more files to write.
*/
