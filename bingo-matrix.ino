#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <Entropy.h>
#include <stdio.h>
#include <Button.h>
#include "kleuren.h"

// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

#define CLK 11
#define LAT A3
#define OE  9
#define A   A0
#define B   A1
#define C   A2
// Last parameter enables double-buffering, for flicker-free,
// buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
// until the first call to swapBuffers().  This is normal.
// RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true);
// Double-buffered mode consumes nearly all the RAM available on the
// Arduino Uno -- only a handful of free bytes remain.  In that case use PROGMEM:
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);
#define KLEUR_B         Red
#define KLEUR_I         Blue
#define KLEUR_N         Yellow
#define KLEUR_G         Orange
#define KLEUR_O         Purple
#define KLEUR_VRIJ      DarkGreen
#define KLEUR_LETTER    Yellow
#define KLEUR_BEZET     Red
#define KLEUR_RESET     DarkCyan
#define aantalGetallen 75
#define aantalToReset   5
int aantalResets = 0;

uint8_t random_byte;

int trekkingNummer = 0;
int bingoGetal = 0;
int vorigeBingoGetal = 0;
char bingoLetter;
char vorigeBingoLetter;

Button buttonTrekking = Button(3,BUTTON_PULLDOWN, false, 150);
Button buttonReset = Button(4,BUTTON_PULLDOWN, false, 150);

struct trekking {
  int x, y, done;
};
struct trekking trekkingen[aantalGetallen + 1];

void setup() {
  // Serial.begin(9600);
  Entropy.initialize();  
    
  matrix.begin();
  matrix.setTextWrap(false); // Allow text to run off right edge
  matrix.setTextSize(1);  
  matrix.setRotation(3);  // set in portrait
  // Clear background
  matrix.fillScreen(0);
  // matrix.swapBuffers(false);      
  
  aantalResets = 0;
  buttonTrekking.pressHandler(handleButtonPressEvents);
  buttonReset.pressHandler(handleButtonPressEvents);
 
  startBingo();
}

void loop() {
  buttonTrekking.isPressed(); //update internal state
  buttonReset.isPressed(); //update internal state
}

void handleButtonPressEvents(Button &btn) {
  if (btn == buttonTrekking) {
    trekkingKnopIngedrukt();
  }
  else if (btn == buttonReset) {    
    resetKnopIngedrukt();
  }
}

void resetKnopIngedrukt() {
  aantalResets++;
  if (aantalResets == 1) {
    drawB(KLEUR_RESET);
  } else if (aantalResets == 2) {
    drawI(KLEUR_RESET);
  } else if (aantalResets == 3) {
    drawN(KLEUR_RESET);
  } else if (aantalResets == 4) {
    drawG(KLEUR_RESET);
  } else if (aantalResets == 5) {
    drawO(KLEUR_RESET);
  }
  
  if (aantalResets >= aantalToReset) {
    resetBingo();
  }  
}

void resetBingo() {
    aantalResets = 0;
    stopBingo();
    startBingo();
}

void trekkingKnopIngedrukt() {
  aantalResets = 0;
  if (trekkingNummer == aantalGetallen) {
    resetBingo();
    return;
  }
  trekkingNummer++;
  vorigeBingoGetal = bingoGetal;
  vorigeBingoLetter = bingoLetter;
  do {
    bingoGetal = Entropy.random(1,aantalGetallen+1); // returns a value from 1 to 75 (76 excl)
  } while (trekkingen[bingoGetal].done == 1);    
  trekkingen[bingoGetal].done = 1; // markeer als geweest  
  doeTrekking(bingoGetal); 
}


void startBingo() {
    vorigeBingoGetal = 0;
    bingoGetal = 0;    
    bingoLetter = '-';
    vorigeBingoLetter = '-';     
    trekkingNummer = 0;  
    
    // trekking schoonmaken en grid berekenen
    for (int i = 1; i <= aantalGetallen; i++) {
      trekkingen[i].done = 0;
      int x = (i - 1 ) / 15;
      int y = (i - 1)  % 15;
      trekkingen[i].x = (x*3)+1;
      trekkingen[i].y = y+8;
      matrix.drawPixel(trekkingen[i].x, trekkingen[i].y, KLEUR_VRIJ);
    }
    matrix.drawChar(0,25, 'G', KLEUR_LETTER, Black, 1); 
    matrix.drawChar(6,25, 'O', KLEUR_LETTER, Black, 1); 
    matrix.drawChar(11,25, '!', Red, Black, 1); 
    drawBingo();
}

void stopBingo() {
  drawB(Black);        
  delay(400);    
  drawI(Black);
  delay(400);    
  drawN(Black);
  delay(400);    
  drawG(Black);
  delay(400);    
  drawO(Black);
  delay(400);    
}

void doeTrekking(int bingoGetal) {
  bingoLetter = bepaalBingoLetter(bingoGetal);
  // Serial.println("Trekking " + String(bingoLetter) + ": " + String(bingoGetal));
   
  char b[3];   //declaring character array
 
  String str=String(bingoGetal); //converting integer into a string
  str.toCharArray(b,3); //passing the value of the string to the character array
  
  matrix.fillRect(0, 25, 16, 7, Black); 
  matrix.drawChar(0,25, bingoLetter, KLEUR_LETTER, Black, 1); 
  matrix.drawChar(6,25,b[0], KLEUR_BEZET, Black, 1);  
  matrix.drawChar(11,25,b[1], KLEUR_BEZET, Black, 1);
  
  drawGetalLed(bingoGetal);
  drawLines();
  drawBingo();
  // matrix.swapBuffers(false);      
}



char bepaalBingoLetter(int getal) {
  if (getal <= 15) {
    return 'B';
  } else if (getal <= 30) {
    return 'I';
  } else if (getal <= 45) {
    return 'N';
  }  else if (getal <= 60) {
    return 'G';
  }  else if (getal <= 75) {
    return 'O';
  }  
  return '-';
}

void drawBingo() {
  // maak schoon
  matrix.fillRect(0, 0, 16, 6, Black); 
  // schrijf BINGO
  drawB(KLEUR_B);
  drawI(KLEUR_I);
  drawN(KLEUR_N);
  drawG(KLEUR_G);
  drawO(KLEUR_O);  
}  

void drawB(uint16_t kleur) {
  matrix.drawFastVLine(0, 0, 6, kleur );
  matrix.drawFastVLine(2, 2, 4, kleur );
  matrix.drawPixel(1, 2, kleur);
  matrix.drawPixel(1, 5, kleur);
}  

void drawI(uint16_t kleur) {
  matrix.drawFastVLine(4, 2, 4, kleur );
  matrix.drawPixel(4, 0, kleur);
}

void drawN(uint16_t kleur) {
  matrix.drawFastVLine(6, 2, 4, kleur );
  matrix.drawPixel(7, 2, kleur);
  matrix.drawFastVLine(8, 2, 4, kleur );
}

void drawG(uint16_t kleur) {
  matrix.drawFastVLine(10, 2, 3, kleur );
  matrix.drawFastVLine(12, 2, 5, kleur );
  matrix.drawPixel(11, 2, kleur);
  matrix.drawPixel(11, 4, kleur);
  matrix.drawFastHLine(10, 6 , 3, kleur);
}

void drawO(uint16_t kleur) {
  matrix.drawFastVLine(14, 2, 4, kleur );
  matrix.drawFastVLine(15, 2, 4, kleur );
}

void drawGetalLed(int bingoGetal) {
  for (int i=1; i < 5; i++) {
    bezet(bingoGetal);
    delay(400);
    vrij(bingoGetal);
    delay(400);
  }
  bezet(bingoGetal);
}
 
void drawLines() {
  matrix.fillRect(0, 8, 16, 15, Black); 
  for (int i = 1; i <= aantalGetallen; i++) {
    if (trekkingen[i].done == 1) {
       bezet(i);
    } else {
       vrij(i);
    }
  }      
}

void bezet(int bingoGetal) {
  matrix.drawPixel(trekkingen[bingoGetal].x, trekkingen[bingoGetal].y, KLEUR_BEZET);
}

void vrij(int bingoGetal) {
  matrix.drawPixel(trekkingen[bingoGetal].x, trekkingen[bingoGetal].y, KLEUR_VRIJ);
}

void doHourglassOn () {
  //knipper de i
  matrix.drawPixel(4,0, White);
}

void doHourglassOff () {
  matrix.drawPixel(4,0, KLEUR_I);
}

