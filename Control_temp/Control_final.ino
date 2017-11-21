#include <DHT.h>
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h> //https://github.com/br3ttb/Arduino-PID-Library/

int pwm; 
char inByte ;
int value;
boolean overTemp, bangBang, manualOverride; 
 //Variables
 float error;
 float hum;  //Stores humidity value
 float temp; //Stores temperature value
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 1000;   
#define PIN_OUTPUT 10 // PWM capable
#define PIN_FAN 9 // PWM capable
#define ONE_WIRE_BUS 3
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float tempPeltierH1, tempPeltierC1, tempPeltierH2, tempPeltierC2, tempUsada;
//Define Variables we'll be connecting to
double Setpoint, Input, Output;



//Specify the links and initial tuning parameters
double Kp=5, Ki=1, Kd=0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

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
//  hum = dht.readHumidity();
//  temp= dht.readTemperature();

//Print temp and humidity values to serial monitor
//  Serial.print("RH: ");
//  Serial.print(hum);
//  Serial.print(" %, Temp: ");
//  Serial.print(temp);
//  Serial.print(" C ");
// call DS18B20 sensors.requestTemperatures() to issue a global temperature
  sensors.requestTemperatures(); // Send the command to get temperatures

  tempPeltierH1 = sensors.getTempCByIndex(0);
  tempPeltierH2 = sensors.getTempCByIndex(1);


  revisaSerial();
  controlBacan();
  //controlPID();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
  Serial.print("Setpoint: ");
  Serial.print(Setpoint);
  Serial.print(" S1: ");
  Serial.print(tempPeltierH1); //
  Serial.print(" S2: ");
  Serial.print(tempPeltierH2); //
  Serial.print(" e: ");
  Serial.print(error);
  Serial.print(" Temp: ");
  Serial.print(tempUsada);
  Serial.print(" Ouput: ");
  Serial.println(Output);
  }
  analogWrite(PIN_OUTPUT, Output);

}

void controlPID(){
  if (Setpoint- tempPeltierH1 >3) {
    myPID.SetMode(MANUAL);

    bangBang=true;
//  } else if (Setpoint- temp > -10){
//    myPID.SetMode(MANUAL);
//    Output=0;
//    bangBang=true;
  } else if (bangBang && !manualOverride) {
    myPID.SetMode(AUTOMATIC);
    bangBang=false;
    Output=0;
  }
  Input = tempPeltierH1;
  myPID.Compute();


}
void controlBacan() {
  if (tempPeltierH1 != -127 && tempPeltierH2 != -127){
    tempUsada = (tempPeltierH1+tempPeltierH2)/2;}
  else if (tempPeltierH1 != -127){
    tempUsada = tempPeltierH1;}
  else if (tempPeltierH2 != -127){
    tempUsada = tempPeltierH2;}
    error = Setpoint - tempUsada;

  if (Setpoint < 35){
      if (error > 3){
      Output=255;
      }else if (error >0.6){
        Output = 255*0.75;
        }else if (error>0.1){
          Output =255*0.5;}
          else if (error>0.1){
          Output =255*0.30;
          }
          else {
            Output =0;
            }
  }
  else if (Setpoint == 37){
      if (error > -0.15){
      Output=255;
        }
          else {
            Output =0;
            }
  }
}

void revisaOverTemp(){
  if (tempPeltierH1>80) {
    myPID.SetMode(MANUAL);
    Output=0;
    overTemp=true;
  } else if (overTemp && tempPeltierH1 < 60 ){
    myPID.SetMode(AUTOMATIC);
    overTemp=false;
    Output=0;
  }
}


void revisaSerial(){
  if (Serial.available()) {   //recibe comando por serial a,m,s,p,i,d,? seguido de un número ej a0, m100, s30
    //a: control en auto
    //mX: control manual, output= X
    //sX: fija el setpoint=X
    //pX: hace Kp=X
    //iX: hace Ki=X
    //dX: hace Kd=X
    //?: control en auto
    inByte = Serial.read();
    value=Serial.parseInt();
    constrain(value,0,255);
    Serial.print("Read! ");

    Serial.print(inByte);
    Serial.print(": ");
    Serial.println(value);

  switch (inByte) {
    case 'a': //pone el controlador en automático
      myPID.SetMode(AUTOMATIC);
      manualOverride=false;
      break;
    case 'm':  //pone el control en modo manual (control apagado), y setea el output
      myPID.SetMode(MANUAL);
      Output=value;
      manualOverride = true;
      break;
    case 's':   //Cambia el setpoint del controlador
      Setpoint=value;
      break;
    case 'p':   //Cambia el setpoint del controlador
      Kp= value;
      myPID.SetTunings(Kp, Ki, Kd) ;
      break;
    case 'i':   //Cambia el setpoint del controlador
      Ki= value;
      myPID.SetTunings(Kp, Ki, Kd) ;
      break;
    case 'd':   //Cambia el setpoint del controlador
      Kd= value;
      myPID.SetTunings(Kp, Ki, Kd) ;
      break;
    case '?':   //Cambia el setpoint del controlador
      Serial.println();
      Serial.print(" Kp ");
      Serial.print(myPID.GetKp());
      Serial.print(" Ki ");
      Serial.print(myPID.GetKi());
      Serial.print(" Kd ");
      Serial.println(myPID.GetKd());
      break;
    default: ;
    }
  }
}
