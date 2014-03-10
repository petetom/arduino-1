#include <Wire.h>
#include <OneWire.h>
#include <Adafruit_NeoPixel.h>

#define PIN 9
#define Sensor 2    //int0 signal for opto sensor

OneWire  m1(5);    //motor 1 temperature sensor
OneWire  m2(7);    //motor 2 temperature sensor

Adafruit_NeoPixel strip = Adafruit_NeoPixel(15, PIN, NEO_GRB + NEO_KHZ800);

int  low_c = 1;      // low counter byte
int  hgh_c = 0;      // high counter byte
unsigned int rpm_i;
float rpm_f;
// One wire stuff
byte i;
byte present = 0;
byte dev1[8];
byte dev2[8];
byte data[12];
byte addr[8];
float c1,c2;
float f1,f2;

char sr;      // serial reab byte
byte c;
byte d;
int  p_send;  

byte t_ary[2] = {5,6};     //i2c temp data send
byte rpm_b = 3;            //i2c rpm data

void setup()
{
  int d = 0;
  
  pinMode(Sensor,INPUT);    //RPM sensor
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  digitalWrite(12,LOW);
   
  Wire.begin(2);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // receive data
  Wire.onRequest(requestEvent); // send data
  Serial.begin(9600);           // start serial for output
  Serial.println("start");
  strip.begin();                // begin the LED ring 
  strip.show();
  
  TCCR1A = 0x0;    // set mode
  TCCR1B = 0x04;   //set mode and counter prescale to 256
  
  m1.reset();
  m1.reset_search();
  
   if ( m1.search(dev1))
   {  d++;
      Serial.println("device 1 found");
   }else
      dev1[0]=0;
 
    m2.reset();
   m2.reset_search();
   
   if ( m2.search(dev2))
   {  d++;
      Serial.println("device 2 found");
   }else 
      dev2[0]=0;
  
    attachInterrupt(0, rpm, FALLING);
    interrupts();
}//end setup

void loop()
{
   //delay(100);
   if (Serial.available())
   { sr = Serial.read();
     Serial.println(sr);
     if (sr == 'r')
     {  Serial.println("red");
        colorWipe(strip.Color(255, 0, 0), 1); // Red
     }
     if (sr == 'g')
     {  Serial.println("green");
        colorWipe(strip.Color(0, 255, 0), 1); // Green
     }
   }
   
   if(dev1[0])    //if device is present get temperature
   {  digitalWrite(12,HIGH);    //flag high - use to see how long conversion takes
      m1.reset();
      m1.select(dev1);
      m1.write(0x44);         // start conversion, with parasite power on at the end

      delay(80);     // maybe 750ms is enough, maybe not
      do{
        data[0] = m1.read();
      }while(!data[0]);  //wait for convesion to complete
  
      present = m1.reset();
      m1.select(dev1);    
      m1.write(0xBE);         // Read Scratchpad

      for ( i = 0; i < 9; i++) {           // we need 9 bytes
        data[i] = m1.read();
      }
      int16_t raw = (data[1] << 8) | data[0];
      raw = raw << 3;
      c1 = (float)raw/16.0;
      f1 = c2 * 1.8 + 32.0;
      digitalWrite(12,LOW);
    }
    if(dev2[0])    // if device is present get temperature
    {
      m2.reset();
      m2.select(dev2);
      m2.write(0x44);         // start conversion, with parasite power on at the end
    
      delay(80);     // maybe 750ms is enough, maybe not
      do{
        data[0] = m2.read();
      }while(data[0]!=0xFF);
  
      present = m2.reset();
      m2.select(dev2);    
      m2.write(0xBE);         // Read Scratchpad

      for ( i = 0; i < 9; i++) {           // we need 9 bytes
        data[i] = m2.read();
      }
      int16_t raw = (data[1] << 8) | data[0];
      raw = raw << 3;
      c2 = (float)raw/16.0;
      f2 = c2 * 1.8 + 32.0;
    }
    
    interrupts();
    delay(20);
    noInterrupts();
  
    rpm_i = (hgh_c * 256 + low_c);
    Serial.print(float(rpm_i)*0.016);
    Serial.print("  ");
    rpm_f = (1/(float(rpm_i)*0.000016))*60;
    Serial.print(rpm_f);
    Serial.print("  ");
    Serial.print(f1);
    Serial.print("  ");
    Serial.println(f2);
  
    rpm_b = byte(rpm_f/100);    //6456 becomes 64, value in hundreds of RPM
       
}

void rpm(){
// grab the coounter values on interrupt
  //noInterrupts();
  low_c = TCNT1L;
  hgh_c = TCNT1H;
  digitalWrite(13,HIGH);
  digitalWrite(13,LOW);
  TCNT1H = 0;    // reset high and low count to 0
  TCNT1L = 1; 
  //interrupts();
}

float conv_to_f(int c)
{  
   return (c*9/5)+32;
}

void requestEvent()  // function to return data
{
  Serial.println("send");
  
  if (p_send == 1) // send 2 bytes (temperature data)
  {  Wire.write(t_ary,2);
  }
  else            // send RPM byte
  { Wire.write(rpm_b);
  }
  
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int numBytes)  //function to receive and act on data
{
   Serial.println("receive"); 
   c = Wire.read();  
   //Serial.println(c,HEX);         // print the integer
   d = Wire.read();
   Serial.println(d,HEX);
   switch(char(d))
   {
     case 'r':
       colorWipe(strip.Color(255, 0, 0), 1); // Red
       break;
     case 'g':
       colorWipe(strip.Color(0, 255, 0), 1); // Green
       break;
     case 't':  //if 't' send data back
       p_send = 1;  // flag to send back 2 temperature bytes
       break;
     case 's':
       p_send = 2;  //flag to send RPM
       break;
   }  
   
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
