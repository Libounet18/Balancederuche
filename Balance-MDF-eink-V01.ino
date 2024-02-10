/*
--  ███╗   ███╗██╗███████╗██╗         ██████╗ ███████╗    ███████╗██╗██╗      ██████╗ ████████╗
--  ████╗ ████║██║██╔════╝██║         ██╔══██╗██╔════╝    ██╔════╝██║██║     ██╔═══██╗╚══██╔══╝
--  ██╔████╔██║██║█████╗  ██║         ██║  ██║█████╗      █████╗  ██║██║     ██║   ██║   ██║   
--  ██║╚██╔╝██║██║██╔══╝  ██║         ██║  ██║██╔══╝      ██╔══╝  ██║██║     ██║   ██║   ██║   
--  ██║ ╚═╝ ██║██║███████╗███████╗    ██████╔╝███████╗    ██║     ██║███████╗╚██████╔╝   ██║   
--  ╚═╝     ╚═╝╚═╝╚══════╝╚══════╝    ╚═════╝ ╚══════╝    ╚═╝     ╚═╝╚══════╝ ╚═════╝    ╚═╝ 
Florent Libert                        Balance de ruche                       since 01-01-2023
.                                                               
_                         Software V0.0  //  PCB V0.1     05-03-2023                           _ */

#define LILYGO_T5_V213
#include <boards.h>
#include <GxEPD.h>
#include <GxDEPG0213BN/GxDEPG0213BN.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#include <HX711_ADC.h>
#include <EEPROM.h>

GxIO_Class io(SPI, EPD_CS, EPD_DC, EPD_RSET);
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);

const int HX711_dout = 14; //mcu > HX711 dout pin
const int HX711_sck = 19; //mcu > HX711 sck pin
HX711_ADC LoadCell(HX711_dout, HX711_sck);
const int calVal_eepromAdress = 0;

int buttonState = 0; 

int poids = 0;


const unsigned char MDF50[] = { 
0X00,0X01,0XE0,0X03,0XC0,0X00,0X00,0X00,0X01,0XF8,0X0F,0XC0,0X00,0X00,0X00,0X00,
0X3C,0X1E,0X00,0X00,0X00,0X00,0X00,0X0E,0X38,0X00,0X00,0X00,0X00,0X00,0X06,0X30,
0X00,0X00,0X00,0X00,0X00,0X03,0X60,0X00,0X00,0X00,0X00,0X00,0X03,0XE0,0X00,0X00,
0X00,0X00,0X00,0X0F,0XF8,0X00,0X00,0X00,0X00,0X00,0X1C,0X1C,0X00,0X00,0X00,0X00,
0X00,0X18,0X0C,0X00,0X00,0X00,0X00,0X00,0X30,0X06,0X00,0X00,0X00,0X00,0X00,0X30,
0X06,0X00,0X00,0X00,0X00,0X00,0X30,0X06,0X00,0X00,0X00,0X00,0X00,0X30,0X06,0X00,
0X00,0X00,0X00,0X00,0X30,0X06,0X00,0X00,0X00,0X00,0X00,0X3F,0X7E,0X00,0X00,0X00,
0X00,0X00,0XFF,0XFF,0X80,0X00,0X00,0X00,0X03,0XE1,0XC3,0XE0,0X00,0X00,0X00,0X0F,
0X00,0XC0,0X78,0X00,0X00,0X00,0X1C,0X00,0XC0,0X1C,0X00,0X00,0X00,0X38,0X71,0XC7,
0X0E,0X00,0X00,0X00,0XE1,0XE1,0XC3,0XC3,0X80,0X00,0X01,0XC7,0X81,0XC0,0XF1,0XC0,
0X00,0X03,0X8E,0X03,0XE0,0X38,0XE0,0X00,0X07,0X1C,0X03,0X60,0X1C,0X70,0X00,0X06,
0X38,0X03,0X70,0X0E,0X30,0X00,0X0C,0X10,0X06,0X30,0X02,0X18,0X00,0X0C,0X80,0X0F,
0XF8,0X00,0X9C,0X00,0X19,0X80,0X0F,0XF8,0X00,0XCC,0X00,0X18,0X00,0X18,0X0C,0X00,
0X0C,0X00,0X30,0X00,0X38,0X06,0X00,0X06,0X00,0X30,0X00,0X70,0X07,0X00,0X06,0X00,
0X30,0X00,0XE0,0X03,0X80,0X06,0X00,0X30,0X01,0XFF,0XFF,0XC0,0X06,0X00,0X18,0X03,
0XFF,0XFF,0XE0,0X0C,0X00,0X1C,0X07,0XC0,0X01,0XF0,0X1C,0X00,0X0E,0X3E,0XC0,0X01,
0XBE,0X38,0X00,0X07,0XF8,0XC0,0X01,0X8F,0XF0,0X00,0X00,0XC0,0X60,0X03,0X81,0XC0,
0X00,0X00,0X00,0X60,0X03,0X00,0X00,0X00,0X00,0X00,0X7F,0XFF,0X00,0X00,0X00,0X00,
0X00,0X3F,0XFE,0X00,0X00,0X00,0X00,0X00,0X30,0X06,0X00,0X00,0X00,0X00,0X00,0X18,
0X0C,0X00,0X00,0X00,0X00,0X00,0X1C,0X1C,0X00,0X00,0X00,0X00,0X00,0X0E,0X38,0X00,
0X00,0X00,0X00,0X00,0X07,0XF0,0X00,0X00,0X00,0X00,0X00,0X03,0XE0,0X00,0X00,0X00,
0X00,0X00,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X80,0X00,0X00,0X00,};


void setup(void) {
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);


  SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
  display.init();

  LoadCell.begin();
}


void loop() {
  buttonState = digitalRead(2);
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold12pt7b);
  display.setCursor(90, 14);
  display.print("10 C 1");  
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(60, 120);
  display.print("Miel de Filot");
  display.setCursor(0, 120);
    if (buttonState == HIGH) {
    Serial.println("bouton-non-enfoncé");
    display.print("0");
  } else {
    Serial.println("!! bouton poussé !!");
    display.print("1");
  }
  display.setFont(&FreeSans24pt7b);
  display.setCursor(72, 75);
  display.print("30.84");
  display.drawBitmap(MDF50,0,40,49,48,GxEPD_BLACK);
  display.drawBitmap(MDF50,200,40,49,48,GxEPD_BLACK);
  display.update();

   delay(5000);
}
