#include "U8glib.h"

//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI 
U8GLIB_SSD1306_128X64_2X u8g(U8G_I2C_OPT_NONE);

void draw(const char* what) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_osb35);
  u8g.drawStr( 0, 50, what);
}

void setup(void) {

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
    Serial.begin(115200);        // init serial
}

void loop(void) {
  // picture loop
  int x = 0;
  char buf[5];
  int initial = 0;
  int final = 0;
  
  initial = micros();
  for (x=0; x<=100; x++) {
  u8g.firstPage();  
  do {
    draw(itoa(x, buf, 10));
  } while( u8g.nextPage() );
  
  }
  final = micros();
  Serial.print(final-initial);
  Serial.println("");
 delay(4000); 

}
