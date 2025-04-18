#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include <Servo.h>

#define NUM_LEDS 64  // Total number of LEDs in the WS2812B-64 matrix
#define DATA_PIN 8   // Initializing pin connected to the LED matrix
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN 6   //PIN for larger LED
#define LED2_PIN 7  // smaller LED
#define LED_COUNT 24
#define LED_COUNT2 24
Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);  //define LED ring
Adafruit_NeoPixel ringsml(LED_COUNT, LED2_PIN, NEO_GRB + NEO_KHZ800);
CRGB leds[NUM_LEDS];

float light = 0;
float sound = 0;
int l_state = 0;  //state check for light
int s_state = 0;  //state check for sound

//Matrix variables
int brightM = 5;

//Popup varaibles
int popPin = 13;  //Ppopup Pin is 13
Servo Popup1;

void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);  // light sensor input IS A0
  pinMode(A1, INPUT);  // sound sensor input IS A1
  Serial.begin(9600);

  //LED rings setup
  ring.begin();
  ring.show();
  ring.setBrightness(5);

  ringsml.begin();
  ringsml.show();
  ringsml.setBrightness(5);

  //LED Matrix setup
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(3);

  //Servol popup setup
  Popup1.attach(popPin);
}

void loop() {
  // Serial.println(millis());
  if (millis() % 100 < 40) {
    sensor();
  }
  if (millis() % 100 < 40) {
    decideLED();
  }
  ring1();
  ring2();

  if (millis() % 50 < 20) {
    drawMLED();
    FastLED.show();
    clearMatrix();
  }

  popup();
}

//Light and Sound Sensor
void sensor() {
  //Two sensors
  //By default (not touching anything,) light =1, sound =0.
  //Part 1 : Light sensing
  // Convert value to 0 - 100.

  light = analogRead(A0) / 10;
  if (light >= 50) {  //stage 0: too dark, no points
    l_state = 0;
  } else if (light >= 40) {  // stage 1: normal light / room light. Change depending on room light value
    l_state = 1;
  } else if (light >= 20) {  // stage 2: brighter, but not bright enough
    l_state = 2;
  } else {  // stage 3: Exceeding cap for boom!
    l_state = 3;
  }
  //Higher value = darker, Lower value = Brighter
  //To design: FOUR stages:
  //0: too dark, no points awarded
  //1: little light, room light.
  //2: more light
  //3: Exceeding a limit

  //Part 2 : Sound sensing
  sound = analogRead(A1);

  if (sound >= 200 && sound <= 400) {
    s_state = 0;  //Stage 0: quiet
  } else if (sound >= 400 && sound <= 800) {
    s_state = 1;  //stage 1: little sound
  } else {
    s_state = 2;  //stage 2: loud!
  }
  //Lower value = louder, Higher value = softer

  Serial.print("light =");
  Serial.println(l_state);
  Serial.print("sound =");
  Serial.println(s_state);
}

//LED Rings
float bright = 20;  //
float bcap = 50;    //dafault cap value for brightness
float step = 5;
float t = 0;
float tspeed = 0.02;

float bright2 = 20;  //
float bcap2 = 50;    //maximum value for brightness
float step2 = 5;
float t2 = 0;
float tspeed2 = 0.02;

//larger ring
void ring1() {
  colorWipe(ring.Color(255, 255, 255));
  ring.setBrightness(bright);

  bright += step * sin(t);  //sound decides how quick it breathes
  if (sin(t) >= 0.999 || sin(t) <= -1) {
    step *= -1;
  }
  bright = constrain(bright, 0, bcap);  //light decides how bright it can get (larger constrain)

  t += tspeed;
}

void colorWipe(uint32_t color) {
  for (int i = 0; i < ring.numPixels(); i++) {  // For each pixel in strip...
    ring.setPixelColor(i, color);               //  Set pixel's color (in RAM)
    ring.show();                                //  Update strip to match
    // delay(wait);                           //  Pause for a moment
  }
}

//smaller ring
void ring2() {
  colorWipe2(ringsml.Color(255, 255, 255)); 
  ringsml.setBrightness(bright2);

  bright2 += step2 * sin(t2);  //sound decides how quick it breathes
  if (sin(t2) >= 0.999 || sin(t2) <= -1) {
    step2 *= -1;
  }
  bright2 = constrain(bright2, 0, bcap2);  //light decides how bright it can get (larger constrain)

  t2 += tspeed2;
}

void colorWipe2(uint32_t color) {
  for (int h = 0; h < ringsml.numPixels(); h++) {  // For each pixel in strip...
    ringsml.setPixelColor(h, color);               //  Set pixel's color (in RAM)
    ringsml.show();                                //  Update strip to match                       
  }
}

void decideLED() {
  switch (l_state) {  //decide how bright LED can get
    case 0:
      bcap -= 5;  // default brightness;
      bcap2 -= 5;
      bcap = constrain(bcap, 2, 8);
      bcap2 = constrain(bcap2, 2, 8);
      brightM = 2;
      break;
    case 1:
      bcap -= 5;  // little light;
      bcap2 -= 5;
      bcap = constrain(bcap, 25, 40);
      bcap2 = constrain(bcap2, 25, 40);
      brightM = 30;
      break;
    case 2:
      bcap += 6;  // more light;
      bcap2 += 6;
      bcap = constrain(bcap, 75, 100);
      bcap2 = constrain(bcap2, 75, 100);
      brightM = 100;
      break;
    case 3:
      bcap += 20;  //cap brightness increase at brightest
      bcap2 += 20;
      bcap = constrain(bcap, 150, 250);
      bcap2 = constrain(bcap2, 150, 250);
      brightM = 200;
      break;
  }

  switch (s_state) {
    case 0:
      step -= 0.5;  // default breathing speed = 2;
      step2 -= 0.5;
      step = constrain(step, 0.5, 2);
      step2 = constrain(step2, 0.5, 2);
      tspeed = 0.01;
      tspeed2 = 0.01;
      break;
    case 1:
      step -= 1;  // little sound = faster step
      step2 -= 1;
      step = constrain(step, 5, 10);
      step2 = constrain(step2, 5, 10);
      tspeed = 0.06;
      tspeed2 = 0.06;
      break;
    case 2:
      step += 2;  //cap loudness = very fast
      step2 += 2;
      step = constrain(step, 10, 40);
      step2 = constrain(step2, 10, 40);
      tspeed = 0.10;
      tspeed2 = 0.10;
      break;
  }
}

//LED Matrix

#define NUM_LEDS 64  // Total number of LEDs in the WS2812B-64 matrix
#define DATA_PIN 9   // Data pin connected to the LED matrix

int starty = 3;
int startx = 3;
int endy = 4;

int drawx = 3;
int drawy = 3;

int sqsize = 2;
int initialSize = 2;
int maxSize = sqrt(NUM_LEDS);

void drawMLED() {
  FastLED.setBrightness(brightM);
  // Fill row
  for (int x = drawx; x < drawx + sqsize; x++) {
    setPixel(x, drawy, CRGB::White);
    setPixel(x, drawy + sqsize - 1, CRGB::White);
  }
  // Fill column
  for (int y = drawy + 1; y < drawy + sqsize - 1; y++) {
    setPixel(drawx, y, CRGB::White);
    setPixel(drawx + sqsize - 1, y, CRGB::White);
  }
  // Shift starting position and size for next iteration
  drawx--;
  drawy--;
  if (drawx < 0) drawx = startx;
  if (drawy < 0) drawy = starty;
  sqsize += 2;
  if (sqsize > maxSize) sqsize = initialSize;
}

void setPixel(int x, int y, CRGB color) {
  if (x >= 0 && x < 8 && y >= 0 && y < 8) {
    int index = x + (y * 8);  // Calculate the index for the 1D array
    leds[index] = color;      // Set the color of the LED
  }
}

void clearMatrix() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);  // Clear the matrix
}

//Servol (Popups)

float finv = 0;
float pstep = 20;
unsigned long lastPopupTime = 0;
int popupDelay = 100;
bool popupActive = false;

void popup() {
  if (s_state == 2 && !popupActive) { //If sound state is 2, move
    finv = 0; 
    popupActive = true;
    lastPopupTime = millis();
  }

  if (popupActive && millis() - lastPopupTime >= popupDelay) {
    if (finv <= 180) {
      Popup1.write(finv);
      finv += 5;
      lastPopupTime = millis(); // Reset timing for next step
    } else {
      popupActive = false; // Reset when done
    }
  }
}