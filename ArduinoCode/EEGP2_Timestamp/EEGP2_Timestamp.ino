

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif
//Provide the token generation process info.
#include <addons/TokenHelper.h>
//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

#include <DHT.h>

//For timestamp
#include <NTPClient.h>
#include <WiFiUdp.h>

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

/* 1. Define the WiFi credentials */

#define WIFI_SSID "Chus :3"
#define WIFI_PASSWORD "wifichus"
//#define WIFI_SSID "AndroidAP"
//#define WIFI_PASSWORD "04578412"
//For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino
/* 2. Define the API Key */
/* 2. Define the API Key */
#define API_KEY "AIzaSyAnJnkPV4UfbYgjPiHVTANIHnedIAVgrX0"
/* 3. Define the RTDB URL */
#define DATABASE_URL "please-fee88-default-rtdb.firebaseio.com" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "nasuhaiskandar@sd.taylors.edu.my"
#define USER_PASSWORD "960710435112"
// temperature and humidity sensor -nasuha
#define DHTPIN 19
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
// co2 -nasuha
#define co2Input     34                        //analog feed from MQ135
#define co2Zero     55                        //calibrated CO2 0 level
// current and volatage

#include "EmonLib.h"             // Include Emon Library

//#define VOLT_CAL 148.7
#define CURRENT_CAL 62.6

EnergyMonitor emon1;             // Create an instance

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
//-- speed sensor

int hall_pin = 18;
int hall_thresh = 1;
int hall_count = 0;
float start = millis();
bool on_state = false;
int flag;
int printnow;
float end_time;
float  time_passed;
float rpm_val;

//-- for vibration
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
Adafruit_MPU6050 mpu;
//------------------------TEMPERATURE FOR COMPRESSOR
#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 5
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void setup()
{
  pinMode(co2Input, INPUT);                    //MQ135 analog feed set for input
  pinMode(hall_pin, INPUT);
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  //Or use legacy authenticate method
  //config.database_url = DATABASE_URL;
  //config.signer.tokens.legacy_token = "<database secret>";

  //////////////////////////////////////////////////////////////////////////////////////////////
  //Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  //otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////

  Firebase.begin(&config, &auth);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);
  //--------------------vibration
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }
  //
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }
  //------------------------------curent sensor pin
  Serial.println("");
  delay(100);
  //emon1.voltage(1, VOLT_CAL, 1.7);  // Voltage: input pin, calibration, phase_shift
  //  emon1.current(35, CURRENT_CAL);       // Current: input pin, calibration.
  //  flag=1;
  //  printnow=0;


  //Initializing timestamp code
  // Initialize Serial Monitor
 
  // Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(28800);
}


void loop()
{

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);

  
  //Retrieving date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);
  delay(1000);
  
  delay(500);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  //flag=1;


  if (digitalRead(hall_pin) == 0)
  {
    if (on_state == false)
    {
      on_state = true;
      hall_count += 1;
      //Serial.print(hall_count);
      Serial.println("Magnet");
    }
  }

  else
  {
    on_state = false;
  }

  if (hall_count >= hall_thresh)
  {
      end_time = millis();
      time_passed = ((end_time - start) / 1000000.0);
      rpm_val = (hall_count / time_passed) * 60.0;
  
      //Serial.print(rpm_val);
      //Serial.println(" RPM");
      //delay (2);
  
      hall_count = 0;
      start = millis();
  }




  int co2now[10];                               //int array for co2 readings
  int co2raw = 0;                               //int for raw value of co2
  int co2ppm = 0;                               //int for calculated ppm
  int zzz = 0;                                  //int for averaging

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  emon1.calcVI(20, 2000);        // Calculate all. No.of half wavelengths (crossings), time-out

  float currentDraw            = emon1.Irms;             //extract Irms into Variable
  float supplyVoltage   = emon1.Vrms;                    //extract Vrms into Variable

  //---- TEMP COMP
  sensors.requestTemperatures();
  float sen1 = sensors.getTempCByIndex(1);
  float sen2 = sensors.getTempCByIndex(2);
  float sen3 = sensors.getTempCByIndex(3);
  float sen0 = sensors.getTempCByIndex(0);

  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    if (isnan(h) || isnan(t) || isnan(f))
    {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);

    if (Firebase.RTDB.setFloat(&fbdo, "test/temperature", t)) //Print Temperature
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      Serial.print("DHT Temperature: ");
      Serial.println(t);
      Serial.print("DHT humidity: ");
      Serial.println(h);
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    for (int x = 0; x < 10; x++) //samplpe co2 10x over 2 seconds
    {
      co2now[x] = analogRead(co2Input);
      delay(200);
    }

    for (int x = 0; x < 10; x++) //add samples together
    {
      zzz = zzz + co2now[x];
    }

    co2raw = zzz / 10;                          //divide samples by 10
    co2ppm = co2raw - co2Zero;                 //get calculated ppm

    if (Firebase.RTDB.setFloat(&fbdo, "test/CO2", co2ppm)) //Print CO2
    {
      Serial.print("CO2=");
      Serial.print(co2ppm);  // prints the value read
      Serial.println(" PPM");
      delay(50);
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "test/rpm", rpm_val)) {
      Serial.print(rpm_val);
      Serial.println(" RPM");
      delay(2);        // delay in between reads for stability
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    //delay(1000);
    if (Firebase.RTDB.setString(&fbdo, "test/time", dayStamp)) //Print Temperature
    {
      Serial.print("Acceleration X: ");
      Serial.print(a.acceleration.x);
      Serial.println(" m/s^2");
      delay(50);


    }
   if (Firebase.RTDB.setFloat(&fbdo, "test/vibrationX", a.acceleration.x)) //Print Temperature
    {
      Serial.print("Acceleration X: ");
      Serial.print(a.acceleration.x);
      Serial.println(" m/s^2");
      delay(50);


    }

    if (Firebase.RTDB.setFloat(&fbdo, "test/vibrationX", a.acceleration.x)) //Print Temperature
    {
      Serial.print("Acceleration X: ");
      Serial.print(a.acceleration.x);
      Serial.println(" m/s^2");
      delay(50);


    }
    if (Firebase.RTDB.setFloat(&fbdo, "test/VibrationY", a.acceleration.y)) //Print Temperature
    {

      Serial.print(" Y: ");
      Serial.print(a.acceleration.y);
      Serial.println(" m/s^2");
      delay(50);


    }
    if (Firebase.RTDB.setFloat(&fbdo, "test/vibrationZ", a.acceleration.z)) //Print Temperature
    {

      Serial.print(" Z: ");
      Serial.print(a.acceleration.z);
      Serial.println(" m/s^2");
      delay(50);

    }
    if (Firebase.RTDB.setFloat(&fbdo, "test/Current", currentDraw)) //Print Temperature
    {
      Serial.print("Current: ");
      Serial.println(currentDraw);
    }
    if (Firebase.RTDB.setFloat(&fbdo, "test/tempComp", sen1)) //Print Temperature
    {
      Serial.print(sen1);
      Serial.println("°C");
    }
    if (Firebase.RTDB.setFloat(&fbdo, "test/Ind->Ac", sen2)) //Print Temperature
    {
      Serial.print(sen2);
      Serial.println("°C");
    }
    if (Firebase.RTDB.setFloat(&fbdo, "test/AC-EXP1", sen3)) //Print Temperature
    {
      Serial.print(sen3);
      Serial.println("°C");
    }
    if (Firebase.RTDB.setFloat(&fbdo, "test/AC-EXP2", sen0)) //Print Temperature
    {
      Serial.print(sen0);
      Serial.println("°C");
    }

    delay(1000);
    //to print every minute, delay(60000)
  }
}
