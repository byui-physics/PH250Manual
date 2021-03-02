/////////////////////////////////////////////////////////
// Writes random data to SD card on Datalogger shield.
/////////////////////////////////////////////////////////
#include <SPI.h> // Allow SPI interfacing, SD Card
#include <SD.h>

#include "RTClib.h" // Adafruit

// Sketch logs data for 30s, then closes file
// SD card won't write until
// file object is closed or flushed
const unsigned long MAX_TIME = 30000;
unsigned long start_ts;
bool running = true;

// SD PIN on our RobotDyn SD/RTC shields
const int CS_PIN = 9;
File logger;
RTC_DS1307 rtc;

// Used in RTClib's DateTime.toString()
const char FORMAT[] = "YYYY-MM-DD hh:mm:ss";
const size_t FORMAT_LEN = (sizeof(FORMAT) / sizeof(char));

char date_buf[FORMAT_LEN];
char filename[24];

int data; // "data"

void setup() 
{
	// Open serial communications and wait for port to open:
	Serial.begin(9600);
	// wait for serial port to connect. 
	// Needed for native USB port only
	while (!Serial); 
	
	Serial.print("Initializing RTC...");
	if (!rtc.begin()) {
		Serial.println("Couldn't find RTC");
		while (1);
	}
	
	//*****************************************************
	// the ! means not. 
	// So this will exicute if the rtc is not running.
	if (!rtc.isrunning()) { 
		Serial.println("RTC has not been set!");
		// following line sets the RTC to the date & time  
		// this sketch was compiled
		// rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
		// This line sets the RTC with an explicit 
		// date & time,for example to set
		// January 21, 2014 at 3am you would call:
		// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
	}
	
	Serial.println("done!");
	
	Serial.print("Initializing SD card...");
	
	if (!SD.begin(CS_PIN)) { 
		Serial.println("failed!");
		while (1);
	}
	Serial.println("done!");
	
	// Create folder for logs if it doesn't already exist
	if (!SD.exists("/LOGS/"))
		SD.mkdir("/LOGS/");
	
	// find the first file LOGxxx.TXT that doesn't exist,
	// then create, open and use that file
	for (int logn = 0; logn < 1000; logn++) {
		sprintf(filename, "/LOGS/LOG%03d.TXT", logn);
		if (!SD.exists(filename)) {
			logger = SD.open(filename, FILE_WRITE);
			Serial.print("Opened \'");
			Serial.print(filename);
			Serial.println("\' for logging.");
			break;
		}
	}
	if (!logger) {
		Serial.print("Failed to open file!");
		while (1);
	}
	
	// Seed random func with noise
	randomSeed(analogRead(0)); 
	start_ts = millis();
}

void loop () 
{
	if (running) {
		if (millis() - start_ts < MAX_TIME) {
			DateTime now = rtc.now();
			
			memcpy(date_buf, FORMAT, FORMAT_LEN);
			now.toString(date_buf);
			
			// random int is our "data"
			data = random(-100, 100); 
			
			logger.print(date_buf);
			logger.print(",");
			logger.print(now.unixtime());
			logger.print(",");
			logger.println(data);
			
			// write same data to serial
			Serial.print(date_buf);
			Serial.print(",");
			Serial.print(now.unixtime());
			Serial.print(",");
			Serial.println(data);
			
			delay(750);	    	
		}
		else {
			// Time has elapsed. Write to file and close.
			running = false;
			logger.close();
			Serial.println("Closed file.");
		}
	}
}