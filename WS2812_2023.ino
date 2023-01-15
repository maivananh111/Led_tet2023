
#include <FastLED.h>
#include "stdio.h"
#include "math.h"


#define NUM_LEDS 120
#define DATA_PIN 7
#define NUM_SPARKS 60 // max number (could be NUM_LEDS / 2);
#define NUM_LOOP 1

CRGB leds[NUM_LEDS]; 
float sparkPos[NUM_SPARKS];
float sparkVel[NUM_SPARKS];
float sparkCol[NUM_SPARKS];
float flarePos;
float gravity = -.004;

int firework_mode = 0;


void flare_start(void);

void flare1(int x, int y);
void flare_up(int x, int y, int pos, int len);
void show_flare_up(int color, int level, int len);
void clear_flare_up(int color, int top, int len);

void flare_down(int x, int y, int pos);
void show_flare_down(int color, int top, int len);

void flare_blink1(int num);
void flare_blink2(int num, int color);
void explodeLoop1(int x, int y);
void explodeLoop2_set(int x, int y, int pos, int top, int len);
void explodeLoop2(int x, int y, int top, int len);


void setup() {
    Serial.begin(9600);
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop() {   
    Serial.println(firework_mode);

    for(int num=0; num< NUM_LOOP; num++){
        int color = random(255);
        flarePos = random((int)(NUM_LEDS/2+20), (int)(NUM_LEDS-30));
        int len = random(10, 25);
        int exploder_color1 = random(0, 7) * 40;
        int exploder_color2 = random(0, 7) * 40;

        if(firework_mode == 0){
            flare_start();
            show_flare_up(color, flarePos, len); 
            clear_flare_up(color, flarePos, len);
            delay(50);
            show_flare_down(color, flarePos, 3);
        
            explodeLoop1(exploder_color1, exploder_color2);
            flare_blink2(100, random(255));
        }

        else if(firework_mode == 1){
            flare(exploder_color1, exploder_color2);
            
            explodeLoop2(exploder_color1, exploder_color2, flarePos, len);
            
            flare_blink1(50);
            flare_blink2(100, random(255));
        }

        else if(firework_mode == 2){
            flare(exploder_color1, exploder_color2);
            
            explodeLoop1(exploder_color1, exploder_color2);
            flare_blink2(100, random(255));
        }

        else if(firework_mode == 3){
            flare_start();
            show_flare_up(color, flarePos, len); 
            clear_flare_up(color, flarePos, len);
            delay(50);
            show_flare_down(color, flarePos, 3);
            
            explodeLoop2(exploder_color1, exploder_color2, flarePos, len);
            
            flare_blink1(50);
            flare_blink2(100, random(255));
        }

        FastLED.clear();
        FastLED.show();
        delay(random(500, 3000));
    }
    firework_mode++;
    if(firework_mode == 4) firework_mode = 0;
}

void flare_start(void){
    leds[0] = CRGB(255, 0, 0);
    for(int i=1; i<5; i++){
        leds[i] = CRGB(255, 255, 0);
        FastLED.show();
        delay(1);
    }
    for(int i=4; i>=0; i--){
        leds[i] = CRGB(0, 0, 0);
        FastLED.show();
        delay(1);
    }
}

void flare(int x, int y) {
    flarePos = 0;
    float flareVel = float(random16(80, 90)) / 100; 
    float brightness = 1;
  
    for (int i = 0; i < 5; i++) { 
        sparkPos[i] = 0;
        sparkVel[i] = (float(random8()) / 255) * (flareVel / 5);
        sparkCol[i] = sparkVel[i] * 1000;
        sparkCol[i] = constrain(sparkCol[i], 0, 255);
    } 
    
    FastLED.clear();
    while (flareVel >= -.2) {
        for (int i = 0; i < 5; i++) {
            sparkPos[i] += sparkVel[i];
            sparkPos[i] = constrain(sparkPos[i], 0, 120);
            sparkVel[i] += gravity;
            sparkCol[i] += -.8;
            sparkCol[i] = constrain(sparkCol[i], 0, 255);
            leds[int(sparkPos[i])] = HeatColor(sparkCol[i]);
            leds[int(sparkPos[i])] %= 50; // reduce brightness to 50/255
        }
        
        leds[int(flarePos)] = CHSV(x, y, int(brightness * 255));
        FastLED.show();
        FastLED.clear();
        flarePos += flareVel;
        flareVel += gravity;
        brightness *= .985;
    }
}

void flare_up(int x, int y, int pos, int len) {
    float brightness = 1;
    int start = (pos-len);
    
    if(start < 0) start = 0;
    for(int i=pos; i>=start; i--){
        int bn = (int)(brightness * 255);
        if(bn <= 0) bn = 1;
        leds[i] = CHSV(x, y, bn);
        brightness -= 0.2;
    }
    if(pos != 0){
        for(int i=0; i<start; i++)      leds[i] = CRGB(0, 0, 0);
        for(int i=pos; i<NUM_LEDS; i++) leds[i] = CRGB(0, 0, 0);
    }
    
    int z = pos - len - 5;
    if(z<0) z = 0; leds[z] = CHSV(x, y, 1);
    z = pos - len - 10;
    if(z<0) z = 0; leds[z] = CHSV(x, y, 1);    
    z = pos - len - 20;
    if(z<0) z = 0; leds[z] = CHSV(x, y, 1);
    
    FastLED.show();
    delay((int)(pos * 0.2));
}

void show_flare_up(int color, int level, int len){
    for(int i = 0; i < level; i++){ 
        flare_up(color, 255, i, len);
    }
}

void clear_flare_up(int color, int top, int len){
    int up_pos = top-1;
    for(int i = top - len - 21; i< up_pos; i++){
      leds[i] = CHSV(0, 0, 0);
      if(i%10 == 0) {
        leds[up_pos] = CHSV(color, 255, 255);
        up_pos++;
      }
      FastLED.show();
      delay(5);
    }
    flarePos = up_pos;
}

void flare_down(int x, int y, int pos){
    leds[pos] = CHSV(x, y, 0);
    leds[pos - 1] = CHSV(x, y, 255);
    FastLED.show();
    delay(100);
}

void show_flare_down(int color, int top, int len){
    for(int i=top; i>(top-len); i--){
        flare_down(color, 255, i);
    }
    flarePos -= len;
}

void explodeLoop1(int x, int y) {
    int nSparks = flarePos / 2; // works out to look about right
    for (int i = 0; i < nSparks; i++) { 
        sparkPos[i] = flarePos; sparkVel[i] = (float(random16(0, 20000)) / 10000.0) - 1.0; // from -1 to 1 
        sparkCol[i] = abs(sparkVel[i]) * 500; // set colors before scaling velocity to keep them bright 
        sparkCol[i] = constrain(sparkCol[i], 0, 255); 
        sparkVel[i] *= flarePos / NUM_LEDS; // proportional to height 
    } 
    sparkCol[0] = 255; // this will be our known spark 
    float dying_gravity = gravity; 
    float c1 = 120; 
    float c2 = 50; 
    while(sparkCol[0] > c2/128) { // as long as our known spark is lit, work with all the sparks
        FastLED.clear();
        for (int i = 0; i < nSparks; i++) { 
            sparkPos[i] += sparkVel[i]; 
            sparkPos[i] = constrain(sparkPos[i], 0, NUM_LEDS); 
            sparkVel[i] += dying_gravity; 
            sparkCol[i] *= .99; 
            sparkCol[i] = constrain(sparkCol[i], 0, 255); // red cross dissolve 
            if(sparkCol[i] > c1) { // fade white to yellow
                leds[int(sparkPos[i])] = CRGB(x, y, (255 * (sparkCol[i] - c1)) / (255 - c1));
            }
            else if (sparkCol[i] < c2) { // fade from red to black
                leds[int(sparkPos[i])] = CRGB((255 * sparkCol[i]) / c2, y, x);
            }
            else { // fade from yellow to red
                leds[int(sparkPos[i])] = CRGB(x, (255 * (sparkCol[i] - c2)) / (c1 - c2), y);
            }
        }
        dying_gravity *= .995; // as sparks burn out they fall slower
        FastLED.show();
    }
    FastLED.clear();
    FastLED.show();
}

void explodeLoop2_set(int x, int y, int pos, int top, int len){
    float brightness = 1;
    int End = (pos+len);
    
    if(End > top) End = top;
    for(int i=pos; i<=End; i++){
        int bn = (int)(brightness * 255);
        if(bn <= 0) bn = 1;
        leds[i] = CHSV(x, y, bn);
        
        int z = top + (top - i);
        if(z < NUM_LEDS) leds[z] = CHSV(x, y, bn);
        
        brightness -= 0.15;
    }
    for(int i=0; i<pos; i++) {
        leds[i] = CRGB(0, 0, 0);
        
        int z = top + (top - i);
        if(z < NUM_LEDS) leds[z] = CHSV(x, y, 0);
    }
    for(int i=End; i<=top; i++) {
        leds[i] = CRGB(0, 0, 0);
        
        int z = top + (top - i);
        if(z < NUM_LEDS) leds[z] = CHSV(x, y, 0);
    }

    int z = pos + len + 5;
    int v = top + (top - z);
    if(z < top) leds[z] = CHSV(x, y, 1);
    if(v < NUM_LEDS) leds[v] = CHSV(x, y, 1);

    z = pos + len + 10;
    v = top + (top - z);
    if(z < top) leds[z] = CHSV(x, y, 1);   
    if(v < NUM_LEDS) leds[v] = CHSV(x, y, 1);
     
    z = pos + len + 20;
    v = top + (top - z);
    if(z < top) leds[z] = CHSV(x, y, 1);
    if(v < NUM_LEDS) leds[v] = CHSV(x, y, 1);
    
    FastLED.show();
    delay((int)(pos * -0.25 + 26));
}

void explodeLoop2(int x, int y, int top, int len){
    for(int i=top; i>=0; i--){
        explodeLoop2_set(x, y, i, top, len);
    }
}

void flare_blink1(int num){
    int i = 0;
    while(i++ < num){
        int color = random(255);
        leds[random(NUM_LEDS)] = CHSV(color, 255, 255);
        FastLED.show();
        FastLED.clear();
    }
}

void flare_blink2(int num, int color){
    int i = 0;
    while(i++ < num){
        leds[random(NUM_LEDS)] = CHSV(color, 255, 255);
        FastLED.show();
        FastLED.clear();
    }
}
