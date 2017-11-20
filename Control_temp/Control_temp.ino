
 #include <DHT.h>
 #define DHTPIN 2     // what pin we're connected to
 #define DHTTYPE DHT22   // DHT 22  (AM2302)
 DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
 #include <OneWire.h>  
 #include <DallasTemperature.h> 
 //Variables
 int pwm; 
 int error;
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
float tempPeltierH, tempPeltierC;

void setup() {
  Serial.begin(9600);
  dht.begin(); //Start DHT lib
  sensors.begin(); //Start DallasTemperature for DS18B20
  //initialize the variables we're linked to
  Setpoint = 30;
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
  Serial.print("RH: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.print(" C ");

  // call DS18B20 sensors.requestTemperatures() to issue a global temperature
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.print(" PeltierTemp: ");
  tempPeltierH = sensors.getTempCByIndex(0);
  Serial.print(tempPeltierH); // 
if (tempPeltierH != -127){
  error = Setpoint-tempPeltierH;
  if (error <0){
    Output=0;}
    else if (error > 3){
    Output=255;
    }else if (error >0.5){
      Output = 255*0.75;
      }else {
        Output =255*0.3;
        }
  }
  Serial.println(Output);
  analogWrite(PIN_OUTPUT, Output);
}

