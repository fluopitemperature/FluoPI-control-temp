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
 float aaa;

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
#define PIN_FAN 9 // PWM capable


//Define Variables we'll be connecting to
double Setpoint, Input, Output;

int pwm; 
char inByte ;
int value;
boolean overTemp, bangBang, manualOverride; 

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

  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_OUTPUT, OUTPUT);
  //turn the PID on
  myPID.SetOutputLimits(0, 255);
  myPID.SetMode(AUTOMATIC);
  
  digitalWrite(PIN_FAN, HIGH);
  aaa=0;
}

void loop()
{
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

  
    
  if (tempPeltierH>80) {
    myPID.SetMode(MANUAL);
    Output=0;
    overTemp=true;
  } else if (overTemp && tempPeltierH < 60 ){
    myPID.SetMode(AUTOMATIC);
    overTemp=false;
    Output=0;
  }

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

  if (Setpoint- tempPeltierH >3) {
    myPID.SetMode(MANUAL);
    if (tempPeltierH != -127) {
      Output=255;
      } else { 
        Output=aaa;
        }
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
     
  Input = tempPeltierH;
  myPID.Compute();
  aaa=Output;
  Serial.print(" Pid On?: ");
  Serial.print(myPID.GetMode());
  Serial.print(" Out: ");
  Serial.println(Output);
  analogWrite(PIN_OUTPUT, Output);
  
}
