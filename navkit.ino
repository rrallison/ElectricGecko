 /*
   Test program for the servo motor and ping sensor components in a
   take-home navigation kit for KNW2300 that is missing a keypad and LCD.
   Commands are issued and results returned through the serial port
   instead of a keypad and LCD.
   Standard Arduino libraries are used, not the KNW2300
   class library. Consult the wiring diagrams
   in NavKitWiringGuide.pdf. The serial commands are:
     1 = take a ping sensor reading
     2 = rotate servo motor 10 degrees clockwise
     3 = rotate servo motor 10 degrees counterclockwise
*/
#include <NewPing.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include "navSlippage.h"
#include <math.h> 
#include <Keypad.h>

#define TRIGGER_PIN  12  
#define ECHO_PIN     11 
#define MAX_DISTANCE 200 

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

Servo myservo; 

int pos = 0;   

double angleR;

const byte ROWS = 4; //four rows in 
const byte COLS = 4; //four columns in keypad

char keys[ROWS][COLS] = { 
  {'1','2','3','A'}, 
  {'4','5','6','B'},
  {'7','8','9','C'}, 
  {'*','0','#','D'}
}

;byte rowPins[ROWS] = {39,41,43,45};
;byte colPins[COLS] = {47,49,51,53}; 

Keypad keypad = Keypad(makeKeymap(keys),rowPins,colPins,ROWS,COLS);
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE); 
void setup() {
  myservo.attach(9); 
  Serial.begin(9600);
  seedSlippage();         
  delay(1000);
  lcd.begin(16,2);
  lcd.clear();
  lcd.blink();
}

void loop() {
  char key;
  unsigned int uS,cm;
  String outstr;

  int standVal= 8; //set standard value to f8cm or 3 inches
  int DirectA = 12;  
  int DirectB = 12;  
  int DirectC = 12;  

  int distOne = 0; // distance reading 1
  int distTwo = 0; // distance reading 2
  const int Dis = 8; // constant distance 8 cm
  
   key = keypad.getKey();
  
  if (key) { 
    lcd.clear();
    outstr = "Command:" + String(key);
    lcd.print(outstr);
    lcd.setCursor(0,1);
      if (key == '1') {
        uS = sonar.ping();
        if (uS > 0) {
          cm = uS/ US_ROUNDTRIP_CM; 
           outstr = "Ping: " + String(cm) + "cm";
           lcd.print(outstr);
           }     
           else {  
            lcd.print("Ping: --");
        }
      }
      else if (key == '5'){
        pos += 10;
        if (pos > 189) pos = 180;
        myservo.write(pos);
        outstr = "Servo: " + String(pos);
        lcd.print(outstr);
      
      }
      else if (key == '2') {
        while (DirectA >standVal || DirectB > standVal || DirectC > standVal)  
          {
            uS = sonar.ping();
            cm = uS / US_ROUNDTRIP_CM; 
            DirectA = cm; 
            outstr = "Ping: " + String(DirectA) + "cm"; //print value of direction A
            lcd.print(outstr); // Display the  value of Diretion A

            if (DirectA <= standVal)
            {
              pos -= 90;  
              if (pos > 180) pos = 180;
              myservo.write(pos);           

              uS = sonar.ping(); 
              cm = uS / US_ROUNDTRIP_CM; 
              DirectA = cm;         
              outstr = "Ping: " + String(DirectA) + "cm"; 
              lcd.print(outstr); // Display value of Direction B
              pos -= 180;  //turn servo with a ping on the top 180 degrees facing right
              if (pos > 180) pos = 180;
              myservo.write(pos);  

              uS = sonar.ping(); 
              cm = uS / US_ROUNDTRIP_CM; 
              DirectB = cm; //get R1 value from cm         
              outstr = "Ping: " + String(DirectB) + "cm"; 
              lcd.print(outstr); // Display value of direction C
              pos -= 90;  
              if (pos > 180) pos = 180;
              myservo.write(pos);

              if (DirectB>standVal && DirectC<standVal) 
              {
                angleR = navSlippage(90.0);    
                outstr = "TURN LEFT for" + String(angleR) + "degrees"; 
                
                pos += 90; 
                if (pos > 180) pos = 180;
                myservo.write(pos); 

                uS = sonar.ping(); 
                cm = uS / US_ROUNDTRIP_CM; 
                distOne = cm;      

                outstr = "FOWARD for 8 cm"; 
                lcd.print(outstr);  

                uS = sonar.ping(); 
                cm = uS / US_ROUNDTRIP_CM; 
                distTwo = cm;

                unsigned int triangle = distOne-distTwo;

                int ABSLUTE=abs(triangle); //assign triangle's absolute value to ABSOLUTE

                int alpha = round( atan2 (triangle, Dis) * 180/PI );

                if (triangle>0)  //turned not enough
                {
                  outstr = "Turn " + String(alpha) + "L"; 
                  lcd.print(outstr); 
                  delay(3000);
                }
                else if (triangle<0)  
                {
                  outstr = "Turn " + String(alpha) + "R"; 
                  lcd.print(outstr);
                  delay(3000);
                }
                else
                {
                  outstr = "Okay!";
                  lcd.print(outstr);
                  delay(3000);
                }
                
                pos -= 90;  
                if (pos > 180) pos = 180;
                myservo.write(pos);  
                
                
              }

              else if (DirectB<standVal && DirectC>standVal)//turn right
              {
                angleR = navSlippage(90.0);   
                outstr = "TURN RIGHT for" + String(angleR) + "degrees"; 
                
                pos -= 90; 
                if (pos > 180) pos = 180;
                myservo.write(pos); 

                uS = sonar.ping(); 
                cm = uS / US_ROUNDTRIP_CM; 
                distOne = cm;    

                outstr="F 8 cm"; 
                lcd.print(outstr);   
                uS = sonar.ping(); 
                cm = uS / US_ROUNDTRIP_CM; 
                distTwo = cm; 

                unsigned int triangle= distOne-distTwo;

                int ABSLUTE=abs(triangle); 
                int alpha = round( atan2 (triangle, Dis) * 180/PI); 

                if (triangle>0)  //turned not enough
                {
                  outstr = "Turn " + String(alpha) + "R      "; 
                  lcd.print(outstr); // Display on LCD
                  delay(4000);
                }
                else if (triangle<0)  //turned too much
                {
                  outstr = "Turn " + String(alpha) + "L      "; 
                  lcd.print(outstr);
                  delay(4000);
                }
                else
                {
                  outstr = "Okay         ";
                  lcd.print(outstr); 
                  delay(4000);
                }

                pos += 90;  
                if (pos > 180) pos = 180;
                myservo.write(pos);  
                 
              }

              else if (DirectB>standVal && DirectC>standVal)
              {
                outstr = "STOP FOR 2s        "; 
                lcd.print(outstr); 
                delay(4000);
              }
            }

            else
            {
               outstr = "go F 5 cm          "; 
               lcd.print(outstr);  
               delay(4000);
            }
         }

            
         }
       }
     }
      
