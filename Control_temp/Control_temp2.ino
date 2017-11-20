#include <DHT.h>
 #define DHTPIN 2     // what pin we're connected to
 #define DHTTYPE DHT22   // DHT 22  (AM2302)
 DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
 #include <OneWire.h>  
 #include <DallasTemperature.h> 
 //Variables
 int pwm; 
 float error;
 int Setpoint;
 int Output;
 float hum;  //Stores humidity value
 float temp; //Stores temperature value

#define PIN_OUTPUT 10 // PWM capable
#define PIN_FAN 9 // PWM capable
#define ONE_WIRE_BUS 3
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float tempPeltierH1, tempPeltierC1, tempPeltierH2, tempPeltierC2, tempUsada;

void setup() {
  Serial.begin(9600);
  dht.begin(); //Start DHT lib
  sensors.begin(); //Start DallasTemperature for DS18B20
  //initialize the variables we're linked to
  Setpoint = 37;
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_OUTPUT, OUTPUT);
  digitalWrite(PIN_FAN, HIGH);
}

void loop() {
  //Read data from DHT and store it to variables hum and temp
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  
  //Print temp and humidity values to serial monitor
//  Serial.print("RH: ");
//  Serial.print(hum);
//  Serial.print(" %, Temp: ");
//  Serial.print(temp);
//  Serial.print(" C ");

  // call DS18B20 sensors.requestTemperatures() to issue a global temperature
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.print(" PeltierTemp1: ");
  tempPeltierH1 = sensors.getTempCByIndex(0);
  Serial.print(tempPeltierH1); // 
  Serial.print(" PeltierTemp2: ");
  tempPeltierH2 = sensors.getTempCByIndex(1);
  Serial.print(tempPeltierH2); // 
  
if (tempPeltierH1 != -127 && tempPeltierH2 != -127){
  tempUsada = (tempPeltierH1+tempPeltierH2)/2;}
else if (tempPeltierH1 != -127){
  tempUsada = tempPeltierH1;}
else if (tempPeltierH2 != -127){
  tempUsada = tempPeltierH2;}
  error = Setpoint - tempUsada;
  Serial.print(error);
    if (error > 2){
    Output=255;
    }else if (error >0.6){
      Output = 255*0.75;
      }else if (error>0.2){
        Output =255*0.5;}
        else if (error>0.1){
        Output =255*0.30;
        }
        else {
          Output =0;
          }
  Serial.print(" Temp: ");
  Serial.println(tempUsada);
  Serial.print(" Ouput: ");
  Serial.println(Output);
  analogWrite(PIN_OUTPUT, Output);
}
