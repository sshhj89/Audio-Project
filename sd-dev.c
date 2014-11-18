/* Read files from an SD card into the files array */
#include <SD.h>

// char *files[3]={"sample1.mp3", "sample2.mp3", "sample3.mp3"};
int fileCount = 0;
File dir; 

char root[] = "/";

countFiles(root);
void countFiles(const char *path) {
	Serial.println("Counting files...");
	while(true) {
		File file =  dir.openNextFile();
		if (! file) {
			// no more files
			Serial.println("Out of files.");
		   break;
		}
		if (file.isDirectory()) {
		 	//do nothing
		} else {
			fileCount++;
			Serial.print(fileCount);
		}
		file.close();
	}
}

char *files[fileCount];

getFiles(root);

void getFiles(const char *path) {
	int i = 0;
	while(true) {
		File file =  path.openNextFile();
		if (! file) {
			// no more files
			Serial.println("Out of files.");
		   break;
		}
		if (file.isDirectory()) {
		 	//do nothing
		} else {
			i++;
			fileCount++;
			Serial.print(file.name());
			*files[i] = file.name();
		}
		file.close();
}