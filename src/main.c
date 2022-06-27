#include <Wire.h>
#include <NMEAGPS.h>
#include <NeoSWSerial.h>
#include <String.h>

#define BaudRate 9600
#define Addr 0x1C // MMA8452 I2C address is 0x1C(28)

char link[100] = "http://maps.google.com/maps?z=2&q=";
static void adjustTime( NeoGPS::time_t & UTCtime );

NeoSWSerial sim800(9, 10) ;
NMEAGPS Neo6M;
gps_fix fix;
#define ssNeo6M Serial

void setup()
{
  sim800.begin(BaudRate); // Initialise SIM800L Communication
  Wire.begin();  // Initialise I2C Communication as MASTER
  Serial.begin(BaudRate);   // Initialise Serial Communication

  SettingsI2C();
  delay(1000);
}

void loop()
{
  unsigned int data[7];

  Wire.requestFrom(Addr, 7);   // Request 7 bytes of data

  // Read 7 bytes of data
  // staus, xAccl lsb, xAccl msb, yAccl lsb, yAccl msb, zAccl lsb, zAccl msb
  if (Wire.available() == 7)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
    data[6] = Wire.read();
  }

  // Convert the data to 12-bits
  int xAccl = ((data[1] * 256) + data[2]) / 16;
  if (xAccl > 2047)
  {
    xAccl -= 4096;
  }

  int yAccl = ((data[3] * 256) + data[4]) / 16;
  if (yAccl > 2047)
  {
    yAccl -= 4096;
  }

  int zAccl = ((data[5] * 256) + data[6]) / 16;
  if (zAccl > 2047)
  {
    zAccl -= 4096;
  }

  if (Neo6M.available(ssNeo6M)) {
    fix = Neo6M.read();
    if (fix.valid.location && fix.valid.date && fix.valid.time) {
     if (zAccl > 1000) // moment of impact
        {
        adjustTime(fix.dateTime);

        String TimeYear = String (fix.dateTime.year);
        String TimeMonth = String (fix.dateTime.month);
        String TimeDay = String (fix.dateTime.date);
        String TimeYMD = TimeDay + "." + TimeMonth + "." + TimeYear;

        String TimeHours = String (fix.dateTime.hours);
        String TimeMinutes = String (fix.dateTime.minutes);
        String TimeSeconds = String (fix.dateTime.seconds);
        String TimeHMS = TimeHours + ":" + TimeMinutes + ":" + TimeSeconds;

        String latitude = String(fix.latitude(), 6);
        String longitude = String(fix.longitude(), 6);
        
        // licentagsm.000webhostapp.com is my website
        // data, ora, latitudine, longitudine are my headers for the table on database
        String url = "AT+HTTPPARA=URL,licentagsm.000webhostapp.com/write_data.php?data=" + TimeYMD + "&ora=" + TimeHMS + "&latitudine=" + latitude + "&longitudine=" + longitude;
        
        // GPRS connection
        sim800.println("AT+CGATT=1");
        delay(1000);
        sim800.println("AT+SAPBR=3,1,CONTYPE,GPRS");
        delay(1000);
        sim800.println("AT+SAPBR=3,1,APN,net");
        delay(1000);
        sim800.println("AT+SAPBR=1,1");
        delay(1000);
        sim800.println("AT+HTTPINIT");
        delay(1000);
        sim800.println("AT+HTTPPARA=CID,1");
        delay(1000);
        sim800.println(url);
        delay(1000);
        sim800.println("AT+HTTPACTION=0");
        delay(5000);
        sim800.println("AT+HTTPREAD");
        delay(1000);
        sim800.println("AT+HTTPTERM");
        delay(1000);
        sim800.println("AT+SAPBR=0,1");
        delay(2000);

        // SMS sending
        sim800.println("AT+CMGF=1");   
        delay(1000);  
        sim800.println("AT+CMGS=\"ZZxxxxxxxxx\"\r"); // ZZ-country code xxxxxxxxx - phone number
        delay(2000);
        sim800.println("I need help!"); // message
        sim800.print(link);
        sim800.print( latitudine);
        sim800.print( ',' );
        sim800.print( longitudine);
        delay(1000);
        sim800.println((char)26);
        delay(1000);
        sim800.println("AT+CMGF=0");
        delay(1000);

        sim800.println("AT+CIPSHUT");
        delay(3000);
      }
    }
    delay(1000);
  }

}


void SettingsI2C() {

  Wire.beginTransmission(Addr);  // Start I2C Transmission
  Wire.write(0x2A);  // Select control register
  Wire.write(0x00);   // StandBy mode
  Wire.endTransmission();  // Stop I2C Transmission

  Wire.beginTransmission(Addr);   // Start I2C Transmission
  Wire.write(0x2A);  // Select control register
  Wire.write(0x01); // Active mode
  Wire.endTransmission(); // Stop I2C Transmission
  
  Wire.beginTransmission(Addr); // Start I2C Transmission
  Wire.write(0x0E); // Select control register
  Wire.write(0x00); // Set range to +/- 2g
  Wire.endTransmission(); // Stop I2C Transmission 
}

static void adjustTime( NeoGPS::time_t & UTCtime )
{
  NeoGPS::clock_t seconds = UTCtime;

  //  Daylight Savings Time rule
  static NeoGPS::time_t  changeover;
  static NeoGPS::clock_t springForward, fallBack;

  if ((springForward == 0) || (changeover.year != UTCtime.year)) {
    changeover.year    = UTCtime.year;
    changeover.month   = 3;
    changeover.date    = 30; // last Sunday
    changeover.hours   = 2; // am
    changeover.minutes = 0;
    changeover.seconds = 0;
    changeover.set_day();
    // Step back to the 4th Sunday, if day != SUNDAY
    changeover.date -= (changeover.day - NeoGPS::time_t::SUNDAY);
    springForward = (NeoGPS::clock_t) changeover;

    changeover.month   = 11;
    changeover.date    = 30; // last Sunday
    changeover.hours   = 3 - 1; // am, adjusted for DST in effect at that time
    changeover.set_day();
    // Step back to the 4th Sunday, if day != SUNDAY
    changeover.date -= (changeover.day - NeoGPS::time_t::SUNDAY);
    fallBack = (NeoGPS::clock_t) changeover;
  }

  // Set these values to the offset of your timezone from GMT
  static const int32_t         zone_hours   = +2L; // CET  // Now is set for Romanian hour
  static const int32_t         zone_minutes =  0L; // zero for CET
  static const NeoGPS::clock_t zone_offset  =
    zone_hours   * NeoGPS::SECONDS_PER_HOUR +
    zone_minutes * NeoGPS::SECONDS_PER_MINUTE;

  seconds += zone_offset;

  if ((springForward <= seconds) && (seconds < fallBack))
    seconds += NeoGPS::SECONDS_PER_HOUR;

  UTCtime = seconds; // convert back to a structure
}
