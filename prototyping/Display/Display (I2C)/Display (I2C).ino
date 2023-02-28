#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define I2C_SDA 4
#define I2C_SCL 5

void disp_ID(int id);

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
int i = 0;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x20, lcdColumns, lcdRows);  //0x20 is the I2c address of our LCD

void setup(){
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);

  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

  lcd.setCursor(0, 0);
  // print message
  lcd.print("ID : ");
}

void loop(){
  // set cursor to first column, first row
  
  //delay(1000);
  // clears the display to print new message
  if(i < 8)
  {
    disp_id(i);
    delay(1000);
  } 
}

void disp_id(int id)
{
  lcd.setCursor(0, i);
  lcd.print(id);
  i++;
}
