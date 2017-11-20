/********************************************************
  * Reading temp to control analog PWM
 ********************************************************/


//DHT
 //Libraries DHT:https://github.com/adafruit/DHT-sensor-library
 // Conectar VDD/VCC a 3.3V o 5V de Arduino
 // GND a Ground
 // Out/data al pin 2
 #include <DHT.h>
 //Constants
 #define DHTPIN 2     // what pin we're connected to
 #define DHTTYPE DHT22   // DHT 22  (AM2302)
 DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
 //Variables
 int chk;
 float hum;  //Stores humidity value
 float temp; //Stores temperature value


//sensor DS18B20
 #include <OneWire.h>  //https://github.com/PaulStoffregen/OneWire
 #include <DallasTemperature.h>  //https://github.com/milesburton/Arduino-Temperature-Control-Library
  //Conexiones
  // (sensor pin3)Vdd a +5V arduino
  // (sensor pin2)Data  a digital 3 arduino
  // (sensor pin1)Gnd a gnd arduino

 // Data wire is plugged into pin 2 on the Arduino
 #define ONE_WIRE_BUS 3

 // Setup a oneWire instance to communicate with any OneWire devices
 // (not just Maxim/Dallas temperature ICs)
 OneWire oneWire(ONE_WIRE_BUS);

 // Pass our oneWire reference to Dallas Temperature.
 DallasTemperature sensors(&oneWire);
float tempPeltierH, tempPeltierC;

//PID parameters
#include <PID_v1.h> //https://github.com/br3ttb/Arduino-PID-Library/

//#define PIN_INPUT 0
#define PIN_OUTPUT 10 // PWM capable

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=5, Ki=1, Kd=0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup()
{
  Serial.begin(9600);
  dht.begin(); //Start DHT lib
  sensors.begin(); //Start DallasTemperature for DS18B20
  //initialize the variables we're linked to

  Setpoint = 30;

  //turn the PID on
  myPID.SetOutputLimits(0, 180);
  myPID.SetMode(AUTOMATIC);

}

void loop()
{
  //Read data from DHT and store it to variables hum and temp
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  //Print temp and humidity values to serial monitor
  Serial.print("DHT: Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");

  // call DS18B20 sensors.requestTemperatures() to issue a global temperature
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.print("Temperature is: ");
  tempPeltierH = sensors.getTempCByIndex(0);
  Serial.println(tempPeltierH); // Why "byIndex"?


  Input = temp;
  if (tempPeltierH>60) {
    myPID.SetMode(MANUAL);
    Output=0;
  } else {
    myPID.SetMode(AUTOMATIC);
  }


  myPID.Compute();
  Serial.print("PID Output: ");
  Serial.println(Output);
  analogWrite(PIN_OUTPUT, Output);
}
