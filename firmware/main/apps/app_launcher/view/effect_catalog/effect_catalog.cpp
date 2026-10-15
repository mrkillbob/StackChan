#include "raster.hpp"
#include "../screensaver_filter.hpp"
namespace effect_catalog {
namespace {
Raster raster; Raster* gfx=&raster; float animationTime=0; bool pendingClear=true;
void drawGear(int x, int y, int radius, int teeth, float rotation, uint16_t color) {
  // Draw gear with specified parameters
  float toothAngle = 2 * PI / teeth;
  
  // Draw gear body
  gfx->fillCircle(x, y, radius - 5, color);
  gfx->drawCircle(x, y, radius - 5, gfx->color565(100, 100, 100));
  
  // Draw gear teeth
  for (int tooth = 0; tooth < teeth; tooth++) {
    float angle1 = tooth * toothAngle + rotation;
    float angle2 = (tooth + 0.3) * toothAngle + rotation;
    float angle3 = (tooth + 0.7) * toothAngle + rotation;
    float angle4 = (tooth + 1) * toothAngle + rotation;
    
    // Inner tooth base
    int x1 = x + cos(angle1) * (radius - 5);
    int y1 = y + sin(angle1) * (radius - 5);
    int x2 = x + cos(angle2) * (radius - 5);
    int y2 = y + sin(angle2) * (radius - 5);
    
    // Outer tooth tip
    int x3 = x + cos(angle2) * radius;
    int y3 = y + sin(angle2) * radius;
    int x4 = x + cos(angle3) * radius;
    int y4 = y + sin(angle3) * radius;
    
    // Inner tooth base (other side)
    int x5 = x + cos(angle3) * (radius - 5);
    int y5 = y + sin(angle3) * (radius - 5);
    int x6 = x + cos(angle4) * (radius - 5);
    int y6 = y + sin(angle4) * (radius - 5);
    
    // Draw tooth outline
    gfx->drawLine(x1, y1, x2, y2, gfx->color565(200, 200, 200));
    gfx->drawLine(x2, y2, x3, y3, gfx->color565(200, 200, 200));
    gfx->drawLine(x3, y3, x4, y4, gfx->color565(200, 200, 200));
    gfx->drawLine(x4, y4, x5, y5, gfx->color565(200, 200, 200));
    gfx->drawLine(x5, y5, x6, y6, gfx->color565(200, 200, 200));
  }
  
  // Central hub
  gfx->fillCircle(x, y, 6, gfx->color565(80, 80, 80));
  gfx->drawCircle(x, y, 6, gfx->color565(150, 150, 150));
  gfx->fillCircle(x, y, 3, BLACK);
}
namespace e0 {

void reset(){}
void drawBoxed() {
  // Boxed effect - rotating 3D-like boxes
  gfx->fillScreen(BLACK);
  for (int i = 0; i < 8; i++) {
    int x = (int)(sin(animationTime * 2 + i * 0.8) * 120 + gfx->width() / 2);
    int y = (int)(cos(animationTime * 1.5 + i * 0.6) * 100 + gfx->height() / 2);
    int size = (int)(sin(animationTime * 3 + i) * 8 + 15);
    
    float phase = animationTime + i * 0.7;
    int red = (int)(sin(phase) * 127 + 128);
    int green = (int)(cos(phase + 1) * 127 + 128);  
    int blue = (int)(sin(phase + 2) * 127 + 128);
    
    if (x > size && y > size && x < gfx->width()-size && y < gfx->height()-size) {
      gfx->fillRect(x-size, y-size, size*2, size*2, gfx->color565(red, green, blue));
      gfx->drawRect(x-size, y-size, size*2, size*2, WHITE);
    }
  }
}
}
namespace e1 {

void reset(){}
void drawBoxfit() {
  // Boxfit effect - fitting rectangles
  gfx->fillScreen(BLACK);
  for (int i = 0; i < 29; i++) {
    int x = (int)(sin(animationTime + i * 0.5) * 100 + gfx->width() / 2);
    int y = (int)(cos(animationTime * 1.3 + i * 0.3) * 80 + gfx->height() / 2);
    int size = (int)(sin(animationTime * 2 + i) * 15 + 20);
    
    float phase = animationTime + i * 0.2;
    int r = (int)(sin(phase) * 127 + 128);
    int g = (int)(sin(phase + 2) * 127 + 128);
    int b = (int)(sin(phase + 4) * 127 + 128);
    
    if (x > 0 && y > 0 && x < gfx->width() && y < gfx->height() && size > 0) {
      gfx->fillRect(x, y, size, size, gfx->color565(r, g, b));
    }
  }
}
}
namespace e2 {

void reset(){}
void drawBraid() {
  // Braid effect - interwoven lines
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 50; j++) {
      float t = animationTime + j * 0.1;
      float angle1 = t + i * PI / 3;
      float angle2 = t + i * PI / 3 + PI;
      
      int radius = 60 + sin(t + i) * 20;
      int x1 = centerX + cos(angle1) * radius;
      int y1 = centerY + sin(angle1) * radius * 0.5;
      int x2 = centerX + cos(angle2) * radius;
      int y2 = centerY + sin(angle2) * radius * 0.5;
      
      uint16_t color = gfx->color565(128 + sin(t + i) * 127, 
                                     128 + cos(t + i + 1) * 127,
                                     128 + sin(t + i + 2) * 127);
      gfx->drawLine(x1, y1, x2, y2, color);
    }
  }
}
}
// Stable slide 4: removed by screensaver policy.
// Stable slide 5: removed by screensaver policy.
// Stable slide 6: removed by screensaver policy.
namespace e6 {

void reset(){}
void drawBuildlwo() {
  // Buildlwo effect - 3D object construction
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw wireframe-style 3D construction
  for (int i = 0; i < 8; i++) {
    float t = animationTime + i * PI / 4;
    int radius = 60 + sin(animationTime + i) * 20;
    
    // Draw rotating wireframe cube
    for (int j = 0; j < 4; j++) {
      float angle1 = t + j * PI / 2;
      float angle2 = t + (j + 1) * PI / 2;
      
      int x1 = centerX + cos(angle1) * radius;
      int y1 = centerY + sin(angle1) * radius;
      int x2 = centerX + cos(angle2) * radius;
      int y2 = centerY + sin(angle2) * radius;
      
      // Inner and outer connections
      uint16_t color = gfx->color565(128 + sin(t) * 127, 255, 128 + cos(t) * 127);
      gfx->drawLine(x1, y1, x2, y2, color);
      gfx->drawLine(x1, y1, centerX, centerY, color);
    }
  }
}
}
namespace e7 {

void reset(){}
void drawBumps() {
  // Bumps effect - bump mapping simulation
  gfx->fillScreen(BLACK);
  
  // Create bump map effect with light source
  int lightX = (int)(sin(animationTime * 0.5) * 100 + gfx->width() / 2);
  int lightY = (int)(cos(animationTime * 0.3) * 80 + gfx->height() / 2);
  
  for (int y = 0; y < gfx->height(); y += 4) {
    for (int x = 0; x < gfx->width(); x += 4) {
      // Calculate distance from light source
      float dx = x - lightX;
      float dy = y - lightY;
      float distance = sqrt(dx*dx + dy*dy);
      
      // Height map based on sine waves
      float height = sin(x * 0.1 + animationTime) * sin(y * 0.1 + animationTime * 0.7);
      
      // Lighting calculation
      float intensity = 1.0 / (1.0 + distance * 0.01) * (height + 1.0) * 0.5;
      intensity = constrain(intensity, 0, 1);
      
      int brightness = (int)(intensity * 255);
      uint16_t color = gfx->color565(brightness, brightness/2, brightness);
      gfx->fillRect(x, y, 4, 4, color);
    }
  }
  
  // Draw light source
  gfx->fillCircle(lightX, lightY, 8, YELLOW);
}
}
namespace e8 {

void reset(){}
void drawCage() {
  // Cage effect - Escher-style impossible cage
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw impossible cage structure
  for (int layer = 0; layer < 3; layer++) {
    float layerOffset = layer * PI / 3 + animationTime * 0.2;
    int radius = 40 + layer * 25;
    
    for (int i = 0; i < 6; i++) {
      float angle1 = layerOffset + i * PI / 3;
      float angle2 = layerOffset + (i + 1) * PI / 3;
      
      // Outer hexagon
      int x1 = centerX + cos(angle1) * radius;
      int y1 = centerY + sin(angle1) * radius;
      int x2 = centerX + cos(angle2) * radius;
      int y2 = centerY + sin(angle2) * radius;
      
      uint16_t color = gfx->color565(255 - layer * 50, 200 - layer * 30, 255);
      gfx->drawLine(x1, y1, x2, y2, color);
      
      // Connect to inner layers for impossible effect
      if (layer > 0) {
        int innerRadius = radius - 25;
        int ix = centerX + cos(angle1 + PI) * innerRadius;
        int iy = centerY + sin(angle1 + PI) * innerRadius;
        gfx->drawLine(x1, y1, ix, iy, color);
      }
    }
  }
}
}
namespace e9 {

void reset(){}
void drawCarousel() {
  // Carousel effect - rotating objects in 3D
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int i = 0; i < 8; i++) {
    float angle = (animationTime + i * PI / 4) * 0.5;
    float z = sin(angle * 2) * 50 + 100; // Z depth
    float scale = 100.0 / z; // Perspective scaling
    
    int x = centerX + cos(angle) * 80 * scale;
    int y = centerY + sin(angle + animationTime) * 20 * scale;
    int size = (int)(15 * scale);
    
    if (size > 2) {
      uint16_t color = gfx->color565(
        (int)(sin(angle + animationTime) * 127 + 128),
        (int)(cos(angle + animationTime + 1) * 127 + 128),
        (int)(sin(angle + animationTime + 2) * 127 + 128)
      );
      gfx->fillRect(x - size/2, y - size/2, size, size, color);
    }
  }
}
}
namespace e10 {

void reset(){}
void drawCCurve() {
  // C Curve fractal
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  float scale = 60 + sin(animationTime * 0.5) * 20;
  
  // Draw C-curve iterations
  for (int depth = 1; depth <= 6; depth++) {
    int segments = 1 << depth; // 2^depth segments
    
    for (int i = 0; i < segments; i++) {
      float t = (float)i / segments;
      float angle = 0;
      
      // Calculate C-curve angle
      int temp = i;
      for (int j = 0; j < depth; j++) {
        if (temp & 1) angle += PI / 4;
        else angle -= PI / 4;
        temp >>= 1;
      }
      
      angle += animationTime * 0.2;
      
      int x = centerX + cos(angle) * scale * t;
      int y = centerY + sin(angle) * scale * t;
      
      uint16_t color = gfx->color565(255 - depth * 30, 100 + depth * 20, 255);
      gfx->fillCircle(x, y, 2, color);
    }
  }
}
}
namespace e11 {

void reset(){}
void drawCeltic() {
  // Celtic knot pattern
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw interwoven Celtic pattern
  for (int ring = 0; ring < 3; ring++) {
    float radius = 30 + ring * 25;
    int segments = 8 + ring * 4;
    
    for (int i = 0; i < segments; i++) {
      float angle1 = (float)i / segments * 2 * PI + animationTime * (0.1 + ring * 0.05);
      float angle2 = (float)(i + 1) / segments * 2 * PI + animationTime * (0.1 + ring * 0.05);
      
      // Create interwoven effect
      float wave = sin(angle1 * 4 + animationTime) * 10;
      
      int x1 = centerX + cos(angle1) * (radius + wave);
      int y1 = centerY + sin(angle1) * (radius + wave);
      int x2 = centerX + cos(angle2) * (radius + wave);
      int y2 = centerY + sin(angle2) * (radius + wave);
      
      uint16_t color = gfx->color565(
        100 + ring * 50,
        200 - ring * 30,
        150 + ring * 35
      );
      
      gfx->drawLine(x1, y1, x2, y2, color);
      
      // Draw crossing effect
      if (i % 2 == ring % 2) {
        gfx->fillCircle(x1, y1, 3, WHITE);
      }
    }
  }
}
}
// Stable slide 13: removed by screensaver policy.
// Stable slide 14: removed by screensaver policy.
namespace e14 {

void reset(){}
void drawCircuit() {
  // Circuit board effect
  gfx->fillScreen(gfx->color565(0, 40, 0)); // Dark green PCB
  
  // Draw circuit traces
  for (int i = 0; i < 20; i++) {
    float traceTime = animationTime * 2 + i * 0.3;
    int startX = (int)(sin(traceTime) * 100 + gfx->width() / 2);
    int startY = (int)(cos(traceTime) * 80 + gfx->height() / 2);
    
    // Horizontal traces
    int traceLength = 40 + sin(traceTime) * 20;
    uint16_t traceColor = gfx->color565(0, 200, 100);
    gfx->drawFastHLine(startX, startY, traceLength, traceColor);
    
    // Vertical connections
    if (i % 3 == 0) {
      gfx->drawFastVLine(startX + traceLength, startY - 10, 20, traceColor);
    }
    
    // Components (resistors, capacitors)
    if (i % 4 == 0) {
      // Resistor
      gfx->fillRect(startX + 10, startY - 3, 15, 6, gfx->color565(139, 69, 19));
      // Color bands
      gfx->drawFastVLine(startX + 13, startY - 3, 6, RED);
      gfx->drawFastVLine(startX + 16, startY - 3, 6, YELLOW);
      gfx->drawFastVLine(startX + 19, startY - 3, 6, BLUE);
    } else if (i % 4 == 1) {
      // Capacitor
      gfx->fillRect(startX + 15, startY - 4, 3, 8, gfx->color565(100, 100, 100));
      gfx->fillRect(startX + 19, startY - 4, 3, 8, gfx->color565(100, 100, 100));
    }
    
    // IC chips
    if (i % 6 == 0) {
      gfx->fillRect(startX + 5, startY - 8, 20, 16, BLACK);
      // Pins
      for (int pin = 0; pin < 4; pin++) {
        gfx->drawFastVLine(startX + 7 + pin * 4, startY - 10, 4, gfx->color565(200, 200, 200));
        gfx->drawFastVLine(startX + 7 + pin * 4, startY + 8, 4, gfx->color565(200, 200, 200));
      }
    }
  }
  
  // Power flowing through traces
  for (int i = 0; i < 5; i++) {
    int pulseX = (int)(animationTime * 100 + i * 60) % gfx->width();
    int pulseY = 40 + i * 20;
    gfx->fillCircle(pulseX, pulseY, 3, CYAN);
  }
}
}
namespace e15 {

void reset(){}
void drawCityFlow() {
  // City flow effect - traffic and data streams
  gfx->fillScreen(BLACK);
  
  // Draw city grid
  for (int i = 0; i < 8; i++) {
    int x = i * (gfx->width() / 7);
    gfx->drawFastVLine(x, 0, gfx->height(), gfx->color565(40, 40, 40));
  }
  for (int i = 0; i < 6; i++) {
    int y = i * (gfx->height() / 5);
    gfx->drawFastHLine(0, y, gfx->width(), gfx->color565(40, 40, 40));
  }
  
  // Moving traffic lights
  for (int i = 0; i < 15; i++) {
    float flowTime = animationTime * 3 + i * 0.4;
    int x = (int)(flowTime * 20) % gfx->width();
    int y = 20 + (i % 4) * 60;
    
    // Traffic stream
    uint16_t color = (i % 3 == 0) ? RED : (i % 3 == 1) ? GREEN : BLUE;
    gfx->fillRect(x, y, 8, 4, color);
    
    // Data packets
    if (i % 2 == 0) {
      int dataX = (int)(flowTime * 15 + 50) % gfx->width();
      gfx->fillCircle(dataX, y + 20, 2, YELLOW);
    }
  }
}
}
// Stable slide 17: removed by screensaver policy.
// Stable slide 18: removed by screensaver policy.
// Stable slide 19: removed by screensaver policy.
namespace e19 {

void reset(){}
void drawColors() {
  // Colors effect - cycling through color space
  gfx->fillScreen(BLACK);
  
  // HSV color wheel effect
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  int radius = min(gfx->width(), gfx->height()) / 3;
  
  for (int angle = 0; angle < 360; angle += 5) {
    float rad = (angle + animationTime * 50) * PI / 180.0;
    int x1 = centerX;
    int y1 = centerY;
    int x2 = centerX + cos(rad) * radius;
    int y2 = centerY + sin(rad) * radius;
    
    // Convert HSV to RGB approximation
    float hue = (angle + animationTime * 100) / 360.0;
    while (hue > 1.0) hue -= 1.0;
    
    int r, g, b;
    if (hue < 0.167) {
      r = 255; g = hue * 6 * 255; b = 0;
    } else if (hue < 0.333) {
      r = (0.333 - hue) * 6 * 255; g = 255; b = 0;
    } else if (hue < 0.5) {
      r = 0; g = 255; b = (hue - 0.333) * 6 * 255;
    } else if (hue < 0.667) {
      r = 0; g = (0.667 - hue) * 6 * 255; b = 255;
    } else if (hue < 0.833) {
      r = (hue - 0.667) * 6 * 255; g = 0; b = 255;
    } else {
      r = 255; g = 0; b = (1.0 - hue) * 6 * 255;
    }
    
    gfx->drawLine(x1, y1, x2, y2, gfx->color565(r, g, b));
  }
  
  // Color gradient bands
  for (int y = 0; y < gfx->height(); y += 4) {
    int colorShift = (int)(animationTime * 50 + y) % 768;
    int r = 0, g = 0, b = 0;
    
    if (colorShift < 256) {
      r = colorShift; g = 255 - colorShift; b = 0;
    } else if (colorShift < 512) {
      r = 0; g = colorShift - 256; b = 255 - (colorShift - 256);
    } else {
      r = colorShift - 512; g = 0; b = 255 - (colorShift - 512);
    }
    
    gfx->drawFastHLine(0, y, gfx->width(), gfx->color565(r, g, b));
  }
}
}
// Stable slide 21: removed by screensaver policy.
// Stable slide 22: removed by screensaver policy.
// Stable slide 23: removed by screensaver policy.
// Stable slide 24: removed by screensaver policy.
// Stable slide 25: removed by screensaver policy.
// Stable slide 26: removed by screensaver policy.
// Stable slide 27: removed by screensaver policy.
// Stable slide 28: removed by screensaver policy.
// Stable slide 29: removed by screensaver policy.
// Stable slide 30: removed by screensaver policy.
// Stable slide 31: removed by screensaver policy.
// Stable slide 32: removed by screensaver policy.
// Stable slide 33: removed by screensaver policy.
namespace e33 {
bool cracks[60][40];
bool initialized = false;
int generation = 0;
void reset(){std::memset(cracks,0,sizeof(cracks));initialized=false;generation=0;}
void drawCrackberg() {
  // Crackberg effect - cracked surface with growing fractures
  gfx->fillScreen(gfx->color565(80, 80, 80)); // Gray surface
  
  
  
  
  
  if (!initialized || generation > 200) {
    // Initialize with some initial cracks
    for (int x = 0; x < 60; x++) {
      for (int y = 0; y < 40; y++) {
        cracks[x][y] = false;
      }
    }
    // Create initial crack seeds
    for (int i = 0; i < 5; i++) {
      int x = random(10, 50);
      int y = random(10, 30);
      cracks[x][y] = true;
    }
    initialized = true;
    generation = 0;
  }
  
  // Crack propagation (every 30 frames)
  if ((int)(animationTime * 10) % 30 == 0) {
    bool newCracks[60][40];
    for (int x = 0; x < 60; x++) {
      for (int y = 0; y < 40; y++) {
        newCracks[x][y] = cracks[x][y];
      }
    }
    
    for (int x = 1; x < 59; x++) {
      for (int y = 1; y < 39; y++) {
        if (!cracks[x][y]) {
          // Check for adjacent cracks
          int adjacent = 0;
          if (cracks[x-1][y] || cracks[x+1][y] || cracks[x][y-1] || cracks[x][y+1]) adjacent++;
          
          // Propagate crack based on stress
          if (adjacent > 0 && random(0, 8) == 0) {
            newCracks[x][y] = true;
          }
        }
      }
    }
    
    for (int x = 0; x < 60; x++) {
      for (int y = 0; y < 40; y++) {
        cracks[x][y] = newCracks[x][y];
      }
    }
    generation++;
  }
  
  // Draw cracks
  for (int x = 0; x < 60; x++) {
    for (int y = 0; y < 40; y++) {
      if (cracks[x][y]) {
        int screenX = x * (gfx->width() / 60);
        int screenY = y * (gfx->height() / 40);
        
        // Crack depth based on age
        uint16_t crackColor = BLACK;
        gfx->fillRect(screenX, screenY, 
                      gfx->width() / 60 + 1, gfx->height() / 40 + 1, 
                      crackColor);
        
        // Add some red glow for deep cracks
        if (random(0, 4) == 0) {
          gfx->fillRect(screenX, screenY, 
                        gfx->width() / 60 + 1, gfx->height() / 40 + 1, 
                        gfx->color565(100, 0, 0));
        }
      }
    }
  }
  
  // Stress lines radiating from crack centers
  for (int i = 0; i < 8; i++) {
    float stressAngle = i * PI / 4 + animationTime * 0.5;
    int centerX = gfx->width() / 2;
    int centerY = gfx->height() / 2;
    
    int x1 = centerX + cos(stressAngle) * 20;
    int y1 = centerY + sin(stressAngle) * 20;
    int x2 = centerX + cos(stressAngle) * 80;
    int y2 = centerY + sin(stressAngle) * 80;
    
    gfx->drawLine(x1, y1, x2, y2, gfx->color565(60, 60, 60));
  }
}
}
// Stable slide 35: removed by screensaver policy.
// Stable slide 36: removed by screensaver policy.
// Stable slide 37: removed by screensaver policy.
namespace e37 {

void reset(){}
void drawCube21() {
  // Cube21 effect - 3D rotating cubes in formation
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw 21 cubes in a pattern
  for (int cube = 0; cube < 21; cube++) {
    float cubeAngle = cube * 2 * PI / 21 + animationTime;
    float radius = 60 + sin(animationTime * 2 + cube * 0.3) * 20;
    
    int cubeX = centerX + cos(cubeAngle) * radius;
    int cubeY = centerY + sin(cubeAngle) * radius;
    
    // 3D cube rotation
    float rotX = animationTime * 1.5 + cube * 0.2;
    float rotY = animationTime * 1.2 + cube * 0.15;
    
    // Cube size varies
    int cubeSize = 8 + sin(animationTime * 3 + cube) * 4;
    
    // Cube color based on position
    uint16_t cubeColor = gfx->color565(
      128 + sin(cubeAngle) * 127,
      128 + cos(cubeAngle + 1) * 127,
      128 + sin(cubeAngle + 2) * 127
    );
    
    // Draw 3D cube wireframe
    // Front face
    gfx->drawRect(cubeX - cubeSize, cubeY - cubeSize, cubeSize * 2, cubeSize * 2, cubeColor);
    
    // Back face offset for 3D effect
    int offsetX = (int)(cos(rotX) * 6);
    int offsetY = (int)(sin(rotY) * 6);
    gfx->drawRect(cubeX - cubeSize + offsetX, cubeY - cubeSize + offsetY, 
                  cubeSize * 2, cubeSize * 2, cubeColor);
    
    // Connect front and back faces
    gfx->drawLine(cubeX - cubeSize, cubeY - cubeSize, 
                  cubeX - cubeSize + offsetX, cubeY - cubeSize + offsetY, cubeColor);
    gfx->drawLine(cubeX + cubeSize, cubeY - cubeSize, 
                  cubeX + cubeSize + offsetX, cubeY - cubeSize + offsetY, cubeColor);
    gfx->drawLine(cubeX - cubeSize, cubeY + cubeSize, 
                  cubeX - cubeSize + offsetX, cubeY + cubeSize + offsetY, cubeColor);
    gfx->drawLine(cubeX + cubeSize, cubeY + cubeSize, 
                  cubeX + cubeSize + offsetX, cubeY + cubeSize + offsetY, cubeColor);
  }
  
  // Central connecting lines
  for (int line = 0; line < 21; line++) {
    float lineAngle = line * 2 * PI / 21 + animationTime;
    float radius = 60 + sin(animationTime * 2 + line * 0.3) * 20;
    
    int lineX = centerX + cos(lineAngle) * radius;
    int lineY = centerY + sin(lineAngle) * radius;
    
    if (line % 3 == 0) {
      gfx->drawLine(centerX, centerY, lineX, lineY, 
                    gfx->color565(100, 100, 200));
    }
  }
}
}
namespace e38 {
int population[16][4];
bool initialized = false;
int generation = 0;
void reset(){std::memset(population,0,sizeof(population));initialized=false;generation=0;}
void drawCubenetic() {
  // Cubenetic effect - genetic algorithm visualization with cubes
  gfx->fillScreen(gfx->color565(20, 40, 20)); // Dark green background
  
   // x, y, fitness, generation
  
  
  
  if (!initialized) {
    // Initialize random population
    for (int i = 0; i < 16; i++) {
      population[i][0] = random(20, gfx->width() - 20);
      population[i][1] = random(20, gfx->height() - 20);
      population[i][2] = random(0, 100); // fitness
      population[i][3] = 0; // generation
    }
    initialized = true;
  }
  
  // Evolution step every 60 frames
  if ((int)(animationTime * 10) % 60 == 0) {
    // Selection and mutation
    for (int i = 0; i < 16; i++) {
      // Calculate fitness based on position (closer to center = higher fitness)
      int dx = population[i][0] - gfx->width() / 2;
      int dy = population[i][1] - gfx->height() / 2;
      population[i][2] = 100 - (int)(sqrt(dx*dx + dy*dy) * 0.3);
      
      // Mutate position towards center for high fitness individuals
      if (population[i][2] > 50) {
        if (population[i][0] > gfx->width() / 2) population[i][0] -= random(1, 4);
        else population[i][0] += random(1, 4);
        
        if (population[i][1] > gfx->height() / 2) population[i][1] -= random(1, 4);
        else population[i][1] += random(1, 4);
      } else {
        // Random mutation for low fitness
        population[i][0] += random(-5, 6);
        population[i][1] += random(-5, 6);
        
        // Keep in bounds
        population[i][0] = constrain(population[i][0], 10, gfx->width() - 10);
        population[i][1] = constrain(population[i][1], 10, gfx->height() - 10);
      }
      
      population[i][3] = generation; // Update generation
    }
    generation++;
  }
  
  // Draw population as evolving cubes
  for (int i = 0; i < 16; i++) {
    int cubeX = population[i][0];
    int cubeY = population[i][1];
    int fitness = population[i][2];
    
    // Cube size based on fitness
    int cubeSize = 4 + fitness / 10;
    
    // Color based on fitness (green = good, red = bad)
    uint16_t cubeColor;
    if (fitness > 70) {
      cubeColor = gfx->color565(0, 255, 0); // Green - high fitness
    } else if (fitness > 40) {
      cubeColor = gfx->color565(255, 255, 0); // Yellow - medium fitness
    } else {
      cubeColor = gfx->color565(255, 0, 0); // Red - low fitness
    }
    
    // Draw cube
    gfx->fillRect(cubeX - cubeSize, cubeY - cubeSize, 
                  cubeSize * 2, cubeSize * 2, cubeColor);
    gfx->drawRect(cubeX - cubeSize, cubeY - cubeSize, 
                  cubeSize * 2, cubeSize * 2, WHITE);
    
    // Fitness indicator
    gfx->fillCircle(cubeX, cubeY, fitness / 20, cubeColor);
    
    // DNA connections to nearby individuals
    for (int j = i + 1; j < 16; j++) {
      int dx = abs(population[i][0] - population[j][0]);
      int dy = abs(population[i][1] - population[j][1]);
      int distance = dx + dy;
      
      if (distance < 50) {
        uint16_t connectionColor = gfx->color565(0, 100, 255);
        gfx->drawLine(population[i][0], population[i][1],
                      population[j][0], population[j][1], connectionColor);
      }
    }
  }
  
  // Target area (evolution goal)
  int targetX = gfx->width() / 2;
  int targetY = gfx->height() / 2;
  gfx->drawCircle(targetX, targetY, 30, WHITE);
  gfx->drawCircle(targetX, targetY, 25, WHITE);
  
  // Generation counter
  gfx->setTextColor(WHITE);
  gfx->setTextSize(1);
  gfx->setCursor(5, 5);
  gfx->printf("Gen: %d", generation);
  
  // Best fitness
  int bestFitness = 0;
  for (int i = 0; i < 16; i++) {
    if (population[i][2] > bestFitness) bestFitness = population[i][2];
  }
  gfx->setCursor(5, 15);
  gfx->printf("Best: %d", bestFitness);
}
}
// Stable slide 40: removed by screensaver policy.
// Stable slide 41: removed by screensaver policy.
namespace e41 {

void reset(){}
void drawCubeTwist() {
  // Cubetwist effect - cubes twisting in spiral pattern
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw twisted cube formation
  for (int ring = 0; ring < 5; ring++) {
    float ringRadius = 20 + ring * 20;
    int cubesInRing = 6 + ring * 2;
    
    for (int cube = 0; cube < cubesInRing; cube++) {
      // Base angle for this cube
      float baseAngle = (cube * 2 * PI) / cubesInRing;
      
      // Twist effect - inner rings rotate faster
      float twistRate = (5 - ring) * 0.3;
      float twistAngle = animationTime * twistRate + baseAngle;
      
      // Spiral twist - each ring offset vertically
      float spiralOffset = sin(animationTime * 2 + ring * 0.5) * 15;
      
      // Calculate position
      int cubeX = centerX + cos(twistAngle) * ringRadius;
      int cubeY = centerY + sin(twistAngle) * ringRadius + spiralOffset;
      
      // Cube size decreases with distance from center
      int cubeSize = 12 - ring * 2;
      if (cubeSize < 4) cubeSize = 4;
      
      // Color based on twist angle and ring
      uint16_t cubeColor = gfx->color565(
        (int)(sin(twistAngle) * 127 + 128),
        (int)(cos(twistAngle + ring) * 127 + 128),
        255 - ring * 40
      );
      
      // 3D rotation based on twist
      float rotX = twistAngle * 2;
      float rotY = animationTime + ring * 0.3;
      
      // Draw 3D cube with perspective
      int depth = (int)(cos(rotY) * 8);
      
      // Front face
      gfx->fillRect(cubeX - cubeSize/2, cubeY - cubeSize/2, 
                    cubeSize, cubeSize, cubeColor);
      gfx->drawRect(cubeX - cubeSize/2, cubeY - cubeSize/2, 
                    cubeSize, cubeSize, WHITE);
      
      // Back face (offset for 3D effect)
      int backX = cubeX + depth;
      int backY = cubeY + depth;
      gfx->drawRect(backX - cubeSize/2, backY - cubeSize/2, 
                    cubeSize, cubeSize, 
                    gfx->color565(100, 100, 150));
      
      // Connect corners for 3D wireframe
      gfx->drawLine(cubeX - cubeSize/2, cubeY - cubeSize/2,
                    backX - cubeSize/2, backY - cubeSize/2, WHITE);
      gfx->drawLine(cubeX + cubeSize/2, cubeY - cubeSize/2,
                    backX + cubeSize/2, backY - cubeSize/2, WHITE);
      gfx->drawLine(cubeX - cubeSize/2, cubeY + cubeSize/2,
                    backX - cubeSize/2, backY + cubeSize/2, WHITE);
      gfx->drawLine(cubeX + cubeSize/2, cubeY + cubeSize/2,
                    backX + cubeSize/2, backY + cubeSize/2, WHITE);
    }
  }
  
  // Central twist vortex
  for (int i = 0; i < 8; i++) {
    float vortexAngle = animationTime * 4 + i * PI / 4;
    int vortexRadius = (int)(sin(animationTime * 3 + i) * 15 + 25);
    
    int vortexX = centerX + cos(vortexAngle) * vortexRadius;
    int vortexY = centerY + sin(vortexAngle) * vortexRadius;
    
    gfx->drawLine(centerX, centerY, vortexX, vortexY, 
                  gfx->color565(255, 200 - i * 20, 100 + i * 15));
  }
  
  // Center point
  gfx->fillCircle(centerX, centerY, 5, WHITE);
}
}
// Stable slide 43: removed by screensaver policy.
namespace e43 {

void reset(){}
void drawCuboctaversion() {
  // Cuboctaversion effect - morphing between cube and octahedron
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Morphing parameter between 0 and 1
  float morphParam = sin(animationTime * 0.8) * 0.5 + 0.5;
  
  // Draw multiple cuboctahedra at different scales
  for (int scale = 1; scale <= 3; scale++) {
    float radius = 30 * scale + sin(animationTime * 1.5 + scale) * 10;
    
    // 8 vertices of cuboctahedron
    float vertices[8][2];
    
    for (int i = 0; i < 8; i++) {
      float angle = i * PI / 4 + animationTime * (0.5 + scale * 0.2);
      
      // Interpolate between octagon (cube projection) and diamond (octahedron)
      float cubeRadius = radius;
      float octRadius = radius * 0.7;
      float currentRadius = cubeRadius * (1 - morphParam) + octRadius * morphParam;
      
      // Vertex distortion based on morph parameter
      float distortion = 1 + sin(angle * 4 + animationTime * 3) * morphParam * 0.3;
      currentRadius *= distortion;
      
      vertices[i][0] = centerX + cos(angle) * currentRadius;
      vertices[i][1] = centerY + sin(angle) * currentRadius;
    }
    
    // Color based on scale and morph
    uint16_t shapeColor = gfx->color565(
      (int)(100 + morphParam * 155),
      (int)(150 - morphParam * 50 + scale * 20),
      (int)(200 - morphParam * 100 + scale * 15)
    );
    
    // Draw edges
    for (int i = 0; i < 8; i++) {
      int next = (i + 1) % 8;
      gfx->drawLine((int)vertices[i][0], (int)vertices[i][1],
                    (int)vertices[next][0], (int)vertices[next][1], 
                    shapeColor);
      
      // Cross connections for cuboctahedral structure
      if (i % 2 == 0) {
        int cross = (i + 4) % 8;
        gfx->drawLine((int)vertices[i][0], (int)vertices[i][1],
                      (int)vertices[cross][0], (int)vertices[cross][1], 
                      gfx->color565(50 + scale * 30, 50 + scale * 30, 100 + scale * 30));
      }
    }
    
    // Draw vertices as points
    for (int i = 0; i < 8; i++) {
      gfx->fillCircle((int)vertices[i][0], (int)vertices[i][1], 
                      2 + morphParam * 2, WHITE);
    }
    
    // Central morph indicator
    int centerSize = (int)(morphParam * 8 + 2);
    gfx->fillCircle(centerX, centerY, centerSize, 
                    gfx->color565((int)(morphParam * 255), 
                                  (int)((1 - morphParam) * 255), 100));
  }
  
  // Rotation trails
  for (int trail = 0; trail < 12; trail++) {
    float trailAngle = animationTime * 2 + trail * PI / 6;
    float trailRadius = 80 + sin(animationTime * 3 + trail) * 20;
    
    int trailX = centerX + cos(trailAngle) * trailRadius;
    int trailY = centerY + sin(trailAngle) * trailRadius;
    
    gfx->fillCircle(trailX, trailY, 1 + trail % 3, 
                    gfx->color565(trail * 20, 100, 200 - trail * 10));
  }
}
}
// Stable slide 45: removed by screensaver policy.
// Stable slide 46: removed by screensaver policy.
namespace e46 {
float ballX = 120;
float ballY = 120;
float ballVX = 3;
float ballVY = 2.5;
bool initialized = false;
void reset(){ballX=120;ballY=120;ballVX=3;ballVY=2.5;initialized=false;}
void drawDangerball() {
  // Dangerball effect - hazardous bouncing ball with warnings
  gfx->fillScreen(BLACK);
  
  
  
  
  
  
  
  if (!initialized) {
    ballX = gfx->width() / 2;
    ballY = gfx->height() / 2;
    ballVX = random(-4, 5);
    ballVY = random(-4, 5);
    initialized = true;
  }
  
  // Update ball position
  ballX += ballVX;
  ballY += ballVY;
  
  int ballRadius = 12 + sin(animationTime * 6) * 3;
  
  // Bounce off walls with danger effects
  bool bounced = false;
  if (ballX - ballRadius < 0 || ballX + ballRadius > gfx->width()) {
    ballVX = -ballVX;
    bounced = true;
  }
  if (ballY - ballRadius < 0 || ballY + ballRadius > gfx->height()) {
    ballVY = -ballVY;
    bounced = true;
  }
  
  // Keep ball in bounds
  ballX = constrain(ballX, ballRadius, gfx->width() - ballRadius);
  ballY = constrain(ballY, ballRadius, gfx->height() - ballRadius);
  
  // Draw danger ball
  uint16_t dangerColor = gfx->color565(255, (int)(sin(animationTime * 8) * 127 + 128), 0);
  gfx->fillCircle((int)ballX, (int)ballY, ballRadius, dangerColor);
  
  // Hazard symbols on ball
  gfx->fillTriangle((int)ballX, (int)ballY - 6,
                    (int)ballX - 4, (int)ballY + 2,
                    (int)ballX + 4, (int)ballY + 2, BLACK);
  gfx->fillCircle((int)ballX, (int)ballY + 4, 1, BLACK);
  gfx->drawFastVLine((int)ballX, (int)ballY - 3, 4, BLACK);
  
  // Danger aura
  for (int aura = 1; aura <= 3; aura++) {
    int auraRadius = ballRadius + aura * 8;
    uint16_t auraColor = gfx->color565(200 - aura * 50, 50 - aura * 15, 0);
    
    if ((int)(animationTime * 10 + aura) % 4 == 0) {
      gfx->drawCircle((int)ballX, (int)ballY, auraRadius, auraColor);
    }
  }
  
  // Bounce explosion effect
  if (bounced) {
    for (int spark = 0; spark < 8; spark++) {
      float sparkAngle = spark * PI / 4;
      int sparkX = (int)ballX + cos(sparkAngle) * 20;
      int sparkY = (int)ballY + sin(sparkAngle) * 20;
      gfx->fillCircle(sparkX, sparkY, 3, YELLOW);
    }
  }
  
  // Warning indicators at screen edges
  if (ballX < 60) {
    gfx->fillTriangle(5, (int)ballY, 15, (int)ballY - 8, 15, (int)ballY + 8, RED);
  }
  if (ballX > gfx->width() - 60) {
    gfx->fillTriangle(gfx->width() - 5, (int)ballY, 
                      gfx->width() - 15, (int)ballY - 8, 
                      gfx->width() - 15, (int)ballY + 8, RED);
  }
  if (ballY < 60) {
    gfx->fillTriangle((int)ballX, 5, (int)ballX - 8, 15, (int)ballX + 8, 15, RED);
  }
  if (ballY > gfx->height() - 60) {
    gfx->fillTriangle((int)ballX, gfx->height() - 5, 
                      (int)ballX - 8, gfx->height() - 15, 
                      (int)ballX + 8, gfx->height() - 15, RED);
  }
  
  // Speed indicator
  float speed = sqrt(ballVX*ballVX + ballVY*ballVY);
  int speedBars = (int)(speed);
  
  gfx->setTextColor(WHITE);
  gfx->setTextSize(1);
  gfx->setCursor(5, 5);
  gfx->printf("DANGER");
  
  for (int bar = 0; bar < speedBars && bar < 8; bar++) {
    uint16_t barColor = (bar < 4) ? YELLOW : RED;
    gfx->fillRect(5 + bar * 8, 15, 6, 8, barColor);
  }
}
}
// Stable slide 48: removed by screensaver policy.
// Stable slide 49: removed by screensaver policy.
namespace e49 {
float stars[100][4];
bool initialized = false;
void reset(){std::memset(stars,0,sizeof(stars));initialized=false;}
void drawDeepStars() {
  // Deep stars effect - deep space starfield with parallax
  gfx->fillScreen(BLACK);
  
   // x, y, z, brightness
  
  
  if (!initialized) {
    for (int i = 0; i < 100; i++) {
      stars[i][0] = random(-200, 200); // x
      stars[i][1] = random(-150, 150); // y
      stars[i][2] = random(1, 100);    // z depth
      stars[i][3] = random(50, 255);   // brightness
    }
    initialized = true;
  }
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Move stars toward viewer
  for (int i = 0; i < 100; i++) {
    stars[i][2] -= 0.5 + sin(animationTime * 0.1) * 0.3; // Variable speed
    
    // Reset star when it gets too close
    if (stars[i][2] < 1) {
      stars[i][0] = random(-200, 200);
      stars[i][1] = random(-150, 150);
      stars[i][2] = random(80, 100);
      stars[i][3] = random(50, 255);
    }
    
    // 3D projection
    float projectedX = centerX + (stars[i][0] / stars[i][2]) * 100;
    float projectedY = centerY + (stars[i][1] / stars[i][2]) * 100;
    
    // Only draw if on screen
    if (projectedX >= 0 && projectedX < gfx->width() && 
        projectedY >= 0 && projectedY < gfx->height()) {
      
      // Star size based on depth and brightness
      int starSize = (int)((100.0 / stars[i][2]) * 2);
      starSize = constrain(starSize, 1, 8);
      
      // Star brightness decreases with distance
      int brightness = (int)(stars[i][3] / stars[i][2] * 20);
      brightness = constrain(brightness, 0, 255);
      
      // Different star colors based on temperature/type
      uint16_t starColor;
      if (brightness > 200) {
        starColor = gfx->color565(255, 255, brightness); // Blue-white (hot)
      } else if (brightness > 150) {
        starColor = gfx->color565(255, brightness, brightness); // White
      } else if (brightness > 100) {
        starColor = gfx->color565(255, brightness, brightness/2); // Yellow
      } else {
        starColor = gfx->color565(brightness, brightness/2, brightness/4); // Red (cool)
      }
      
      gfx->fillCircle((int)projectedX, (int)projectedY, starSize, starColor);
      
      // Twinkling effect for close stars
      if (stars[i][2] < 20 && (int)(animationTime * 10 + i) % 3 == 0) {
        gfx->drawCircle((int)projectedX, (int)projectedY, starSize + 2, 
                        gfx->color565(brightness, brightness, 255));
      }
    }
  }
  
  // Nebula background effect
  for (int x = 0; x < gfx->width(); x += 16) {
    for (int y = 0; y < gfx->height(); y += 16) {
      float nebulaDensity = sin((x + animationTime * 10) * 0.02) * 
                           cos((y + animationTime * 8) * 0.03);
      if (nebulaDensity > 0.7) {
        int nebulaIntensity = (int)((nebulaDensity - 0.7) * 400);
        nebulaIntensity = constrain(nebulaIntensity, 0, 100);
        
        uint16_t nebulaColor = gfx->color565(nebulaIntensity/4, 0, nebulaIntensity);
        gfx->fillRect(x, y, 16, 16, nebulaColor);
      }
    }
  }
  
  // Shooting stars occasionally
  if (random(0, 200) == 0) {
    int shootX = random(0, gfx->width());
    int shootY = random(0, gfx->height()/2);
    
    for (int trail = 0; trail < 20; trail++) {
      int trailX = shootX + trail * 3;
      int trailY = shootY + trail * 2;
      
      if (trailX < gfx->width() && trailY < gfx->height()) {
        uint16_t trailColor = gfx->color565(255 - trail * 10, 255 - trail * 8, 255 - trail * 12);
        gfx->fillCircle(trailX, trailY, 2 - trail/10, trailColor);
      }
    }
  }
}
}
// Stable slide 51: removed by screensaver policy.
// Stable slide 52: removed by screensaver policy.
// Stable slide 53: removed by screensaver policy.
// Stable slide 54: removed by screensaver policy.
namespace e54 {

void reset(){}
void drawDemon() {
  // Demon effect - dark supernatural entity with glowing eyes
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Dark aura spreading from center
  for (int aura = 1; aura <= 8; aura++) {
    int auraRadius = aura * 15 + sin(animationTime * 3 + aura) * 8;
    uint16_t auraColor = gfx->color565(aura * 8, 0, aura * 4);
    
    if ((int)(animationTime * 6 + aura) % 4 == 0) {
      gfx->drawCircle(centerX, centerY, auraRadius, auraColor);
    }
  }
  
  // Demon's head silhouette
  int headRadius = 35 + sin(animationTime * 2) * 5;
  gfx->fillCircle(centerX, centerY - 20, headRadius, gfx->color565(20, 5, 5));
  
  // Horns
  for (int horn = 0; horn < 2; horn++) {
    int hornX = centerX + (horn == 0 ? -25 : 25);
    int hornY = centerY - 45;
    
    // Horn shape - triangular
    for (int segment = 0; segment < 6; segment++) {
      int segY = hornY - segment * 5;
      int segWidth = 8 - segment;
      
      gfx->fillRect(hornX - segWidth/2, segY, segWidth, 4, 
                    gfx->color565(30 + segment * 5, 0, 0));
    }
    
    // Horn tip glow
    if ((int)(animationTime * 10) % 3 == 0) {
      gfx->fillCircle(hornX, hornY - 30, 3, RED);
    }
  }
  
  // Glowing demon eyes
  for (int eye = 0; eye < 2; eye++) {
    int eyeX = centerX + (eye == 0 ? -15 : 15);
    int eyeY = centerY - 25;
    
    // Eye glow effect
    int glowIntensity = (int)(sin(animationTime * 8 + eye * PI) * 127 + 128);
    
    for (int glow = 3; glow >= 0; glow--) {
      uint16_t glowColor = gfx->color565(
        glowIntensity - glow * 30,
        (glowIntensity - glow * 40) / 4,
        0
      );
      gfx->fillCircle(eyeX, eyeY, 5 + glow * 2, glowColor);
    }
    
    // Pupil
    gfx->fillCircle(eyeX, eyeY, 2, WHITE);
  }
  
  // Demon mouth/sneer
  int mouthY = centerY - 5;
  for (int tooth = 0; tooth < 5; tooth++) {
    int toothX = centerX - 15 + tooth * 8;
    int toothHeight = (tooth == 1 || tooth == 3) ? 8 : 5;
    
    gfx->fillTriangle(toothX, mouthY, 
                      toothX - 2, mouthY + toothHeight,
                      toothX + 2, mouthY + toothHeight, WHITE);
  }
  
  // Flowing demonic energy
  for (int energy = 0; energy < 12; energy++) {
    float energyAngle = energy * PI / 6 + animationTime * 2;
    float energyRadius = 60 + sin(animationTime * 4 + energy) * 20;
    
    int energyX = centerX + cos(energyAngle) * energyRadius;
    int energyY = centerY + sin(energyAngle) * energyRadius;
    
    // Energy orb
    int orbSize = 3 + sin(animationTime * 6 + energy) * 2;
    uint16_t energyColor = gfx->color565(
      200 + sin(energyAngle + animationTime) * 55,
      50,
      100 + cos(energyAngle + animationTime) * 100
    );
    
    gfx->fillCircle(energyX, energyY, orbSize, energyColor);
    
    // Energy trail back to center
    if (energy % 3 == 0) {
      gfx->drawLine(centerX, centerY, energyX, energyY, 
                    gfx->color565(100, 0, 50));
    }
  }
  
  // Hell flames at bottom
  for (int flame = 0; flame < 8; flame++) {
    int flameX = flame * (gfx->width() / 8) + random(-5, 6);
    int flameHeight = 15 + sin(animationTime * 6 + flame) * 8;
    
    // Flame segments
    for (int segment = 0; segment < flameHeight; segment++) {
      int segY = gfx->height() - segment - 1;
      int segWidth = 6 - (segment / 3);
      
      uint16_t flameColor;
      if (segment < flameHeight / 3) {
        flameColor = WHITE; // Hot center
      } else if (segment < flameHeight * 2 / 3) {
        flameColor = YELLOW; // Middle
      } else {
        flameColor = RED; // Outer flame
      }
      
      gfx->fillRect(flameX - segWidth/2, segY, segWidth, 1, flameColor);
    }
  }
  
  // Floating evil symbols
  for (int symbol = 0; symbol < 6; symbol++) {
    float symAngle = symbol * PI / 3 + animationTime * 0.5;
    int symX = centerX + cos(symAngle) * 80;
    int symY = centerY + sin(symAngle) * 60;
    
    // Pentagram-like symbol
    for (int line = 0; line < 5; line++) {
      float lineAngle = line * 2 * PI / 5 + animationTime;
      int x1 = symX + cos(lineAngle) * 8;
      int y1 = symY + sin(lineAngle) * 8;
      int x2 = symX + cos(lineAngle + 2 * PI / 5) * 8;
      int y2 = symY + sin(lineAngle + 2 * PI / 5) * 8;
      
      gfx->drawLine(x1, y1, x2, y2, gfx->color565(150, 0, 100));
    }
  }
  
  // Demon name in fiery text
  gfx->setTextColor(gfx->color565(255, 100, 0));
  gfx->setTextSize(1);
  gfx->setCursor(centerX - 24, gfx->height() - 15);
  gfx->printf("DAEMON");
}
}
// Stable slide 56: removed by screensaver policy.
// Stable slide 57: removed by screensaver policy.
// Stable slide 58: removed by screensaver policy.
// Stable slide 59: removed by screensaver policy.
// Stable slide 60: removed by screensaver policy.
// Stable slide 61: removed by screensaver policy.
namespace e61 {

void reset(){}
void drawDnalogo() {
  // DNA Logo effect - double helix structure with scientific styling
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // DNA helix parameters
  float helixHeight = gfx->height() * 0.8;
  float helixRadius = 30;
  float helixTwist = animationTime * 2;
  
  // Draw double helix
  for (float y = -helixHeight/2; y < helixHeight/2; y += 2) {
    float normalizedY = y / (helixHeight/2);
    
    // Left helix strand
    float angle1 = normalizedY * 4 * PI + helixTwist;
    int x1 = centerX + cos(angle1) * helixRadius;
    int screenY = centerY + y;
    
    // Right helix strand (180 degrees out of phase)
    float angle2 = angle1 + PI;
    int x2 = centerX + cos(angle2) * helixRadius;
    
    // Color gradient along helix
    uint16_t helixColor1 = gfx->color565(
      (int)(sin(normalizedY * 2 + animationTime) * 127 + 128),
      100,
      255
    );
    
    uint16_t helixColor2 = gfx->color565(
      255,
      (int)(cos(normalizedY * 2 + animationTime) * 127 + 128),
      100
    );
    
    // Draw helix strands
    if (screenY >= 0 && screenY < gfx->height()) {
      gfx->fillCircle(x1, screenY, 3, helixColor1);
      gfx->fillCircle(x2, screenY, 3, helixColor2);
      
      // Connect base pairs when strands are close
      if (abs(x1 - x2) < helixRadius) {
        gfx->drawLine(x1, screenY, x2, screenY, 
                      gfx->color565(200, 200, 100));
        
        // Base pair nucleotides
        int midX = (x1 + x2) / 2;
        
        // Different base pairs (A-T, G-C)
        char basePair = ((int)(normalizedY * 10) % 4);
        uint16_t baseColor = BLACK;
        
        switch(basePair) {
          case 0: baseColor = RED; break;    // A
          case 1: baseColor = BLUE; break;   // T
          case 2: baseColor = GREEN; break;  // G
          case 3: baseColor = YELLOW; break; // C
        }
        
        gfx->fillCircle(midX - 8, screenY, 2, baseColor);
        gfx->fillCircle(midX + 8, screenY, 2, baseColor);
      }
    }
  }
  
  // DNA title and scientific info
  gfx->setTextColor(gfx->color565(0, 255, 100));
  gfx->setTextSize(1);
  gfx->setCursor(centerX - 12, 10);
  gfx->printf("DNA");
  
  gfx->setTextColor(WHITE);
  gfx->setCursor(10, 30);
  gfx->printf("Double Helix");
  
  // Molecular structure indicators
  gfx->setCursor(10, gfx->height() - 40);
  gfx->printf("A-T G-C");
  
  gfx->setCursor(10, gfx->height() - 30);
  gfx->printf("Base Pairs");
  
  // Rotating DNA molecule symbol
  int logoX = gfx->width() - 40;
  int logoY = 40;
  
  for (int i = 0; i < 6; i++) {
    float logoAngle = i * PI / 3 + animationTime * 3;
    int logoRadius = 15;
    
    int atomX = logoX + cos(logoAngle) * logoRadius;
    int atomY = logoY + sin(logoAngle) * logoRadius * 0.3; // Flattened for perspective
    
    uint16_t atomColor = (i % 2 == 0) ? 
      gfx->color565(255, 100, 100) : gfx->color565(100, 100, 255);
    
    gfx->fillCircle(atomX, atomY, 4, atomColor);
    
    // Bonds between atoms
    if (i < 5) {
      float nextAngle = (i + 1) * PI / 3 + animationTime * 3;
      int nextX = logoX + cos(nextAngle) * logoRadius;
      int nextY = logoY + sin(nextAngle) * logoRadius * 0.3;
      
      gfx->drawLine(atomX, atomY, nextX, nextY, WHITE);
    }
  }
  
  // Genetic code scrolling
  int codeY = centerY + 80;
  
  gfx->setTextColor(gfx->color565(100, 255, 100));
  
  char geneticCode[] = "ATCGATCGTAGCTAGCTACGATCG";
  int codeLength = strlen(geneticCode);
  int scrollOffset = (int)(animationTime * 20) % (codeLength * 6);
  
  for (int i = 0; i < 20; i++) {
    int charIndex = (i + scrollOffset / 6) % codeLength;
    char nucleotide = geneticCode[charIndex];
    
    // Color code nucleotides
    uint16_t nucleotideColor;
    switch(nucleotide) {
      case 'A': nucleotideColor = RED; break;
      case 'T': nucleotideColor = BLUE; break;
      case 'G': nucleotideColor = GREEN; break;
      case 'C': nucleotideColor = YELLOW; break;
      default: nucleotideColor = WHITE; break;
    }
    
    gfx->setTextColor(nucleotideColor);
    gfx->setCursor(i * 12 - scrollOffset % 6, codeY);
    gfx->printf("%c", nucleotide);
  }
  
  // Scientific data display
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("3.4A pitch");
  
  gfx->setCursor(80, gfx->height() - 20);
  gfx->printf("36deg/bp");
}
}
// Stable slide 63: removed by screensaver policy.
namespace e63 {

void reset(){}
void drawDoubletime() {
  // Doubletime effect - dual timepieces showing 12-hour vs 24-hour formats
  gfx->fillScreen(BLACK);
  
  // Left clock - 12-hour format
  int leftCenterX = gfx->width() / 4;
  int leftCenterY = gfx->height() / 2;
  int clockRadius = 50;
  
  // Clock face border
  gfx->drawCircle(leftCenterX, leftCenterY, clockRadius, gfx->color565(200, 200, 200));
  gfx->drawCircle(leftCenterX, leftCenterY, clockRadius - 2, gfx->color565(150, 150, 150));
  
  // 12-hour markers
  for (int h = 0; h < 12; h++) {
    float angle = h * PI / 6 - PI / 2;
    int x1 = leftCenterX + cos(angle) * (clockRadius - 8);
    int y1 = leftCenterY + sin(angle) * (clockRadius - 8);
    int x2 = leftCenterX + cos(angle) * (clockRadius - 15);
    int y2 = leftCenterY + sin(angle) * (clockRadius - 15);
    gfx->drawLine(x1, y1, x2, y2, WHITE);
  }
  
  // Right clock - 24-hour format
  int rightCenterX = 3 * gfx->width() / 4;
  int rightCenterY = gfx->height() / 2;
  
  gfx->drawCircle(rightCenterX, rightCenterY, clockRadius, gfx->color565(200, 100, 100));
  gfx->drawCircle(rightCenterX, rightCenterY, clockRadius - 2, gfx->color565(150, 75, 75));
  
  // 24-hour markers
  for (int h = 0; h < 24; h++) {
    float angle = h * PI / 12 - PI / 2;
    int x1 = rightCenterX + cos(angle) * (clockRadius - 8);
    int y1 = rightCenterY + sin(angle) * (clockRadius - 8);
    int x2 = rightCenterX + cos(angle) * (clockRadius - 15);
    int y2 = rightCenterY + sin(angle) * (clockRadius - 15);
    
    uint16_t color = (h % 2 == 0) ? WHITE : gfx->color565(200, 200, 200);
    gfx->drawLine(x1, y1, x2, y2, color);
  }
  
  // Animated hands based on animationTime
  float timeOffset = animationTime * 2; // Speed up for demo
  
  // Left clock hands (12-hour)
  float hour12 = fmod(timeOffset / 12.0, 1.0);
  float min12 = fmod(timeOffset, 1.0);
  
  float hourAngle12 = hour12 * 2 * PI - PI / 2;
  float minAngle12 = min12 * 2 * PI - PI / 2;
  
  // Hour hand
  int hx12 = leftCenterX + cos(hourAngle12) * 25;
  int hy12 = leftCenterY + sin(hourAngle12) * 25;
  gfx->drawLine(leftCenterX, leftCenterY, hx12, hy12, gfx->color565(255, 200, 0));
  
  // Minute hand
  int mx12 = leftCenterX + cos(minAngle12) * 35;
  int my12 = leftCenterY + sin(minAngle12) * 35;
  gfx->drawLine(leftCenterX, leftCenterY, mx12, my12, gfx->color565(0, 255, 200));
  
  // Right clock hands (24-hour)
  float hour24 = fmod(timeOffset / 24.0, 1.0);
  float min24 = fmod(timeOffset, 1.0);
  
  float hourAngle24 = hour24 * 2 * PI - PI / 2;
  float minAngle24 = min24 * 2 * PI - PI / 2;
  
  // Hour hand
  int hx24 = rightCenterX + cos(hourAngle24) * 25;
  int hy24 = rightCenterY + sin(hourAngle24) * 25;
  gfx->drawLine(rightCenterX, rightCenterY, hx24, hy24, gfx->color565(255, 100, 100));
  
  // Minute hand
  int mx24 = rightCenterX + cos(minAngle24) * 35;
  int my24 = rightCenterY + sin(minAngle24) * 35;
  gfx->drawLine(rightCenterX, rightCenterY, mx24, my24, gfx->color565(100, 255, 100));
  
  // Center dots
  gfx->fillCircle(leftCenterX, leftCenterY, 3, WHITE);
  gfx->fillCircle(rightCenterX, rightCenterY, 3, WHITE);
  
  // Labels
  gfx->setTextColor(gfx->color565(200, 200, 200));
  gfx->setTextSize(1);
  gfx->setCursor(leftCenterX - 15, leftCenterY + 60);
  gfx->printf("12-HR");
  gfx->setCursor(rightCenterX - 15, rightCenterY + 60);
  gfx->printf("24-HR");
}
}
// Stable slide 65: removed by screensaver policy.
namespace e65 {

void reset(){}
void drawDpmsProtocol() {
  // DPMS Protocol effect - technical visualization of DPMS communication
  gfx->fillScreen(BLACK);
  
  // Protocol state machine visualization
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw state boxes
  const char* states[] = {"ON", "STBY", "SUSP", "OFF"};
  int statePositions[][2] = {{-60, -30}, {60, -30}, {60, 30}, {-60, 30}};
  uint16_t stateColors[] = {
    gfx->color565(0, 255, 0),
    gfx->color565(255, 255, 0),
    gfx->color565(255, 128, 0),
    gfx->color565(255, 0, 0)
  };
  
  int currentState = (int)(animationTime * 0.8) % 4;
  
  for (int i = 0; i < 4; i++) {
    int x = centerX + statePositions[i][0];
    int y = centerY + statePositions[i][1];
    
    uint16_t color = (i == currentState) ? stateColors[i] : gfx->color565(50, 50, 50);
    gfx->drawRect(x - 20, y - 10, 40, 20, color);
    
    if (i == currentState) {
      gfx->fillRect(x - 18, y - 8, 36, 16, gfx->color565(20, 20, 20));
    }
    
    gfx->setTextColor(color);
    gfx->setTextSize(1);
    gfx->setCursor(x - strlen(states[i]) * 3, y - 4);
    gfx->printf("%s", states[i]);
  }
  
  // Draw transition arrows
  uint16_t arrowColor = gfx->color565(100, 100, 255);
  
  // Horizontal arrows
  gfx->drawLine(centerX - 40, centerY - 30, centerX + 40, centerY - 30, arrowColor);
  gfx->drawLine(centerX + 40, centerY + 30, centerX - 40, centerY + 30, arrowColor);
  
  // Vertical arrows
  gfx->drawLine(centerX + 60, centerY - 10, centerX + 60, centerY + 10, arrowColor);
  gfx->drawLine(centerX - 60, centerY + 10, centerX - 60, centerY - 10, arrowColor);
  
  // Signal timing diagram at bottom
  int timelineY = gfx->height() - 30;
  gfx->drawFastHLine(10, timelineY, gfx->width() - 20, WHITE);
  
  // HSYNC and VSYNC signals
  for (int x = 10; x < gfx->width() - 10; x += 4) {
    float phase = (x - 10) * 0.1 + animationTime * 3;
    
    // HSYNC signal
    int hsyncY = timelineY - 15;
    if ((int)phase % 8 < 4) {
      gfx->drawPixel(x, hsyncY, gfx->color565(255, 100, 100));
    }
    
    // VSYNC signal
    int vsyncY = timelineY - 25;
    if ((int)(phase * 0.1) % 2 == 0) {
      gfx->drawPixel(x, vsyncY, gfx->color565(100, 255, 100));
    }
  }
  
  // Labels
  gfx->setTextColor(gfx->color565(255, 100, 100));
  gfx->setTextSize(1);
  gfx->setCursor(10, timelineY - 10);
  gfx->printf("HSYNC");
  
  gfx->setTextColor(gfx->color565(100, 255, 100));
  gfx->setCursor(10, timelineY - 20);
  gfx->printf("VSYNC");
  
  // Protocol header
  gfx->setTextColor(WHITE);
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("DPMS Protocol State Machine");
}
}
namespace e66 {
float particles[30][4];
bool initialized = false;
void reset(){std::memset(particles,0,sizeof(particles));initialized=false;}
void drawDrift() {
  // Drift effect - slowly drifting particles and colors
  gfx->fillScreen(BLACK);
  
   // x, y, vx, vy for each particle
  
  
  if (!initialized) {
    for (int i = 0; i < 30; i++) {
      particles[i][0] = random(0, gfx->width());   // x
      particles[i][1] = random(0, gfx->height());  // y
      particles[i][2] = (random(-50, 51) / 100.0); // vx (slow drift)
      particles[i][3] = (random(-50, 51) / 100.0); // vy (slow drift)
    }
    initialized = true;
  }
  
  // Update and draw particles
  for (int i = 0; i < 30; i++) {
    // Add drift forces
    particles[i][2] += sin(animationTime * 0.3 + i) * 0.01;
    particles[i][3] += cos(animationTime * 0.4 + i * 0.7) * 0.01;
    
    // Apply velocity damping
    particles[i][2] *= 0.99;
    particles[i][3] *= 0.99;
    
    // Update positions
    particles[i][0] += particles[i][2];
    particles[i][1] += particles[i][3];
    
    // Wrap around edges with smooth transition
    if (particles[i][0] < 0) particles[i][0] = gfx->width();
    if (particles[i][0] > gfx->width()) particles[i][0] = 0;
    if (particles[i][1] < 0) particles[i][1] = gfx->height();
    if (particles[i][1] > gfx->height()) particles[i][1] = 0;
    
    // Draw particle with trail
    int x = (int)particles[i][0];
    int y = (int)particles[i][1];
    
    // Color based on position and time
    int red = (int)(sin(animationTime * 0.2 + i * 0.3) * 127 + 128);
    int green = (int)(cos(animationTime * 0.15 + i * 0.5) * 127 + 128);
    int blue = (int)(sin(animationTime * 0.25 + i * 0.8) * 127 + 128);
    
    uint16_t particleColor = gfx->color565(red, green, blue);
    
    // Draw particle with size variation
    int size = 2 + sin(animationTime + i) * 2;
    gfx->fillCircle(x, y, size, particleColor);
    
    // Draw fading trail
    for (int trail = 1; trail <= 3; trail++) {
      int trailX = x - particles[i][2] * trail * 10;
      int trailY = y - particles[i][3] * trail * 10;
      
      if (trailX >= 0 && trailX < gfx->width() && trailY >= 0 && trailY < gfx->height()) {
        uint16_t trailColor = gfx->color565(red/(trail*2), green/(trail*2), blue/(trail*2));
        gfx->fillCircle(trailX, trailY, 1, trailColor);
      }
    }
  }
  
  // Background drift effect - slowly changing gradient
  int gradientOffset = (int)(animationTime * 5) % (gfx->height() * 2);
  
  for (int y = 0; y < gfx->height(); y++) {
    float phase = (y + gradientOffset) * 0.01;
    int bgRed = (int)(sin(phase) * 20 + 25);
    int bgGreen = (int)(cos(phase * 1.3) * 15 + 20);
    int bgBlue = (int)(sin(phase * 0.7) * 25 + 30);
    
    uint16_t bgColor = gfx->color565(bgRed, bgGreen, bgBlue);
    
    // Draw subtle background lines
    if (y % 4 == 0) {
      gfx->drawFastHLine(0, y, gfx->width(), bgColor);
    }
  }
  
  // Floating text that drifts
  gfx->setTextColor(gfx->color565(150, 150, 200));
  gfx->setTextSize(1);
  int textX = gfx->width()/2 + sin(animationTime * 0.1) * 50;
  int textY = gfx->height()/2 + cos(animationTime * 0.08) * 20;
  gfx->setCursor(textX - 15, textY);
  gfx->printf("DRIFT");
}
}
// Stable slide 68: removed by screensaver policy.
namespace e68 {

void reset(){}
void drawDroste() {
  // Droste effect - recursive/infinite loop visual effect
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Recursive rectangles creating infinite zoom effect
  float scale = 1.0;
  float rotation = animationTime * 0.5;
  
  for (int level = 0; level < 8; level++) {
    // Calculate current transformation
    float currentRotation = rotation * (level + 1);
    float currentScale = scale * pow(0.7, level); // Each level is 70% the size
    
    // Rectangle dimensions
    int rectWidth = (int)(80 * currentScale);
    int rectHeight = (int)(60 * currentScale);
    
    // Color shifts through levels
    int red = (int)(sin(currentRotation + level) * 127 + 128);
    int green = (int)(cos(currentRotation + level + 2) * 127 + 128);
    int blue = (int)(sin(currentRotation + level + 4) * 127 + 128);
    
    uint16_t levelColor = gfx->color565(red, green, blue);
    
    // Draw rotated rectangle (approximated with lines)
    float cos_r = cos(currentRotation);
    float sin_r = sin(currentRotation);
    
    // Rectangle corners before rotation
    int corners[4][2] = {
      {-rectWidth/2, -rectHeight/2},
      {rectWidth/2, -rectHeight/2},
      {rectWidth/2, rectHeight/2},
      {-rectWidth/2, rectHeight/2}
    };
    
    // Rotate and translate corners
    int rotatedCorners[4][2];
    for (int i = 0; i < 4; i++) {
      rotatedCorners[i][0] = centerX + corners[i][0] * cos_r - corners[i][1] * sin_r;
      rotatedCorners[i][1] = centerY + corners[i][0] * sin_r + corners[i][1] * cos_r;
    }
    
    // Draw rectangle edges
    for (int i = 0; i < 4; i++) {
      int next = (i + 1) % 4;
      gfx->drawLine(rotatedCorners[i][0], rotatedCorners[i][1],
                    rotatedCorners[next][0], rotatedCorners[next][1], levelColor);
    }
    
    // Draw some fill pattern for inner rectangles
    if (level < 5) {
      // Draw cross pattern inside
      gfx->drawLine(rotatedCorners[0][0], rotatedCorners[0][1],
                    rotatedCorners[2][0], rotatedCorners[2][1], levelColor);
      gfx->drawLine(rotatedCorners[1][0], rotatedCorners[1][1],
                    rotatedCorners[3][0], rotatedCorners[3][1], levelColor);
    }
  }
  
  // Spiral elements connecting the rectangles
  for (int spiral = 0; spiral < 3; spiral++) {
    float spiralPhase = animationTime * (spiral + 1) * 0.3;
    
    for (int point = 0; point < 50; point++) {
      float t = point * 0.1 + spiralPhase;
      float spiralRadius = t * 3;
      
      if (spiralRadius > 120) continue; // Don't go beyond screen
      
      int spiralX = centerX + cos(t) * spiralRadius;
      int spiralY = centerY + sin(t) * spiralRadius;
      
      // Color fades as spiral expands
      int intensity = (int)(255 * (1.0 - spiralRadius / 120.0));
      uint16_t spiralColor = gfx->color565(intensity, intensity/2, intensity/2);
      
      gfx->fillCircle(spiralX, spiralY, 2, spiralColor);
    }
  }
  
  // Central focal point that pulsates
  int pulseRadius = 5 + sin(animationTime * 4) * 3;
  gfx->fillCircle(centerX, centerY, pulseRadius, WHITE);
  gfx->drawCircle(centerX, centerY, pulseRadius + 2, gfx->color565(255, 200, 0));
  
  // Infinity symbol overlay
  float infinityScale = 0.5;
  for (int t = 0; t < 360; t += 3) {
    float rad = t * PI / 180.0;
    float x = cos(rad) / (1 + sin(rad) * sin(rad)) * 40 * infinityScale;
    float y = sin(rad) * cos(rad) / (1 + sin(rad) * sin(rad)) * 40 * infinityScale;
    
    int plotX = centerX + x;
    int plotY = centerY + y;
    
    if (plotX >= 0 && plotX < gfx->width() && plotY >= 0 && plotY < gfx->height()) {
      gfx->fillCircle(plotX, plotY, 1, gfx->color565(255, 255, 100));
    }
  }
  
  // Title with recursive text effect
  gfx->setTextColor(gfx->color565(200, 200, 200));
  gfx->setTextSize(1);
  for (int textLevel = 0; textLevel < 3; textLevel++) {
    int textScale = textLevel + 1;
    int textOffset = textLevel * 2;
    gfx->setCursor(10 + textOffset, 10 + textOffset);
    uint16_t textColor = gfx->color565(200/(textScale), 200/(textScale), 200/(textScale));
    gfx->setTextColor(textColor);
    gfx->printf("DROSTE");
  }
}
}
namespace e69 {
float rotation = 0;
void reset(){rotation=0;}
void drawDymaxionmapCoords() {
  // Dymaxion map coordinate system effect
  gfx->fillScreen(BLACK);
  
  
  rotation += 0.02;
  
  // Draw geodesic grid pattern
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int i = 0; i < 12; i++) {
    float angle = i * PI / 6 + rotation;
    float radius = 80 + sin(animationTime * 2 + i) * 20;
    
    int x1 = centerX + cos(angle) * radius;
    int y1 = centerY + sin(angle) * radius;
    
    // Connect to center
    gfx->drawLine(centerX, centerY, x1, y1, gfx->color565(0, 150, 255));
    
    // Connect adjacent points
    float nextAngle = (i + 1) * PI / 6 + rotation;
    int x2 = centerX + cos(nextAngle) * radius;
    int y2 = centerY + sin(nextAngle) * radius;
    gfx->drawLine(x1, y1, x2, y2, gfx->color565(100, 255, 100));
    
    // Draw coordinate markers
    gfx->fillCircle(x1, y1, 3, gfx->color565(255, 200, 0));
  }
  
  gfx->setTextColor(WHITE);
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("DYMAXION COORDS");
}
}
namespace e70 {
float phase = 0;
void reset(){phase=0;}
void drawEasing() {
  // Easing function demonstration effect
  gfx->fillScreen(BLACK);
  
  
  phase += 0.03;
  
  // Show different easing curves
  for (int curve = 0; curve < 5; curve++) {
    float t = fmod(phase + curve * 0.5, 2.0);
    if (t > 1.0) t = 2.0 - t; // Bounce back
    
    float easedValue;
    switch(curve) {
      case 0: easedValue = t; break; // Linear
      case 1: easedValue = t * t; break; // Quad
      case 2: easedValue = t * t * t; break; // Cubic
      case 3: easedValue = sin(t * PI / 2); break; // Sine
      case 4: easedValue = 1 - cos(t * PI / 2); break; // Cosine
    }
    
    int x = 20 + curve * 50;
    int y = gfx->height() - 20 - easedValue * (gfx->height() - 40);
    
    // Draw easing curve path
    for (int i = 0; i < 100; i++) {
      float plotT = i / 100.0;
      float plotEased;
      switch(curve) {
        case 0: plotEased = plotT; break;
        case 1: plotEased = plotT * plotT; break; 
        case 2: plotEased = plotT * plotT * plotT; break;
        case 3: plotEased = sin(plotT * PI / 2); break;
        case 4: plotEased = 1 - cos(plotT * PI / 2); break;
      }
      int plotX = x;
      int plotY = gfx->height() - 20 - plotEased * (gfx->height() - 40);
      gfx->fillCircle(plotX + i/5, plotY, 1, gfx->color565(100, 100, 255));
    }
    
    // Moving dot
    gfx->fillCircle(x, y, 4, gfx->color565(255, 100 + curve * 30, 100));
  }
  
  gfx->setTextColor(WHITE);
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EASING");
}
}
namespace e71 {
float battlePhase = 0;
void reset(){battlePhase=0;}
void drawEndgame() {
  // Endgame effect - final battle visualization
  gfx->fillScreen(BLACK);
  
  
  battlePhase += 0.05;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Central explosion pattern
  for (int ray = 0; ray < 16; ray++) {
    float angle = ray * PI / 8 + battlePhase;
    float intensity = sin(battlePhase * 3 + ray) * 0.5 + 0.5;
    int rayLength = 60 + intensity * 40;
    
    int x1 = centerX + cos(angle) * 20;
    int y1 = centerY + sin(angle) * 20;
    int x2 = centerX + cos(angle) * rayLength;
    int y2 = centerY + sin(angle) * rayLength;
    
    uint16_t rayColor = gfx->color565(255 * intensity, 100, 100 + intensity * 155);
    gfx->drawLine(x1, y1, x2, y2, rayColor);
    
    // Particles at ray ends
    if (random(100) < 30) {
      gfx->fillCircle(x2 + random(10) - 5, y2 + random(10) - 5, 2, rayColor);
    }
  }
  
  // Debris field
  for (int debris = 0; debris < 20; debris++) {
    float debrisAngle = debris * PI / 10 + battlePhase * 2;
    float debrisRadius = 80 + sin(battlePhase + debris) * 30;
    
    int debrisX = centerX + cos(debrisAngle) * debrisRadius;
    int debrisY = centerY + sin(debrisAngle) * debrisRadius;
    
    gfx->fillRect(debrisX - 2, debrisY - 2, 4, 4, gfx->color565(150, 150, 150));
  }
  
  // Central core pulsing
  int coreRadius = 8 + sin(battlePhase * 6) * 4;
  gfx->fillCircle(centerX, centerY, coreRadius, WHITE);
  gfx->drawCircle(centerX, centerY, coreRadius + 3, gfx->color565(255, 0, 0));
  
  gfx->setTextColor(gfx->color565(255, 200, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("ENDGAME");
}
}
namespace e72 {
float streamPhase = 0;
void reset(){streamPhase=0;}
void drawEnergystream() {
  // Energy stream effect - flowing energy particles
  gfx->fillScreen(BLACK);
  
  
  streamPhase += 0.08;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple energy streams flowing from center
  for (int stream = 0; stream < 6; stream++) {
    float baseAngle = stream * PI / 3;
    
    // Stream particles
    for (int particle = 0; particle < 20; particle++) {
      float particlePhase = streamPhase + particle * 0.3;
      float distance = fmod(particlePhase * 30, 120);
      
      if (distance > 100) continue; // Fade out at edge
      
      float angle = baseAngle + sin(particlePhase) * 0.2; // Slight wobble
      
      int x = centerX + cos(angle) * distance;
      int y = centerY + sin(angle) * distance;
      
      // Energy color - bright core with glow
      float intensity = 1.0 - distance / 100.0;
      int r = 255 * intensity;
      int g = 200 * intensity;
      int b = 100 + 155 * intensity;
      
      gfx->fillCircle(x, y, 2 + intensity * 2, gfx->color565(r, g, b));
      
      // Glow effect
      if (intensity > 0.5) {
        gfx->drawCircle(x, y, 4, gfx->color565(r/3, g/3, b/3));
      }
    }
  }
  
  // Central energy core
  int coreSize = 6 + sin(streamPhase * 4) * 2;
  gfx->fillCircle(centerX, centerY, coreSize, WHITE);
  gfx->drawCircle(centerX, centerY, coreSize + 2, gfx->color565(255, 255, 0));
  
  gfx->setTextColor(gfx->color565(200, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("ENERGYSTREAM");
}
}
namespace e73 {
float engineTime = 0;
void reset(){engineTime=0;}
void drawEngine() {
  // Engine effect - mechanical parts in motion
  gfx->fillScreen(BLACK);
  
  
  engineTime += 0.1;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Main engine cylinder
  int cylinderRadius = 40;
  gfx->drawCircle(centerX, centerY, cylinderRadius, gfx->color565(150, 150, 150));
  gfx->drawCircle(centerX, centerY, cylinderRadius - 5, gfx->color565(100, 100, 100));
  
  // Piston movement
  float pistonOffset = sin(engineTime * 2) * 15;
  
  // Piston rod
  gfx->drawLine(centerX, centerY - cylinderRadius, 
                centerX, centerY - cylinderRadius - 30 + pistonOffset, 
                gfx->color565(180, 180, 180));
  
  // Piston head
  gfx->fillRect(centerX - 8, centerY - cylinderRadius - 35 + pistonOffset, 16, 8, 
                gfx->color565(200, 200, 200));
  
  // Connecting rods (4 directions)
  for (int i = 0; i < 4; i++) {
    float angle = i * PI / 2 + engineTime;
    float rodLength = 25 + sin(engineTime * 3 + i) * 5;
    
    int x1 = centerX + cos(angle) * cylinderRadius;
    int y1 = centerY + sin(angle) * cylinderRadius;
    int x2 = centerX + cos(angle) * (cylinderRadius + rodLength);
    int y2 = centerY + sin(angle) * (cylinderRadius + rodLength);
    
    gfx->drawLine(x1, y1, x2, y2, gfx->color565(120, 120, 120));
    gfx->fillCircle(x2, y2, 3, gfx->color565(160, 160, 160));
  }
  
  // Flywheel
  int flywheelRadius = 20;
  gfx->drawCircle(centerX - 60, centerY, flywheelRadius, gfx->color565(100, 100, 100));
  
  // Flywheel spokes (rotating)
  for (int spoke = 0; spoke < 6; spoke++) {
    float spokeAngle = spoke * PI / 3 + engineTime * 4;
    int sx1 = centerX - 60 + cos(spokeAngle) * 5;
    int sy1 = centerY + sin(spokeAngle) * 5;
    int sx2 = centerX - 60 + cos(spokeAngle) * 18;
    int sy2 = centerY + sin(spokeAngle) * 18;
    
    gfx->drawLine(sx1, sy1, sx2, sy2, gfx->color565(140, 140, 140));
  }
  
  // Steam/exhaust particles
  for (int steam = 0; steam < 8; steam++) {
    float steamY = centerY - cylinderRadius - 40 + pistonOffset - steam * 8;
    if (steamY < 0) continue;
    
    int steamX = centerX + random(20) - 10;
    int steamAlpha = 255 - steam * 30;
    if (steamAlpha > 0) {
      gfx->fillCircle(steamX, steamY, 1 + random(2), 
                      gfx->color565(steamAlpha, steamAlpha, steamAlpha));
    }
  }
  
  gfx->setTextColor(gfx->color565(180, 180, 180));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("ENGINE");
}
}
namespace e74 {
float epicycleTime = 0;
void reset(){epicycleTime=0;}
void drawEpicycle() {
  // Epicycle effect - circles rolling around circles
  gfx->fillScreen(BLACK);
  
  
  epicycleTime += 0.03;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Main circle (deferent)
  int mainRadius = 60;
  gfx->drawCircle(centerX, centerY, mainRadius, gfx->color565(100, 100, 100));
  
  // Multiple epicycles
  for (int epic = 0; epic < 3; epic++) {
    float baseAngle = epicycleTime * (epic + 1) * 0.7;
    int epicycleRadius = 20 + epic * 10;
    
    // Center of epicycle on main circle
    int epicX = centerX + cos(baseAngle) * mainRadius;
    int epicY = centerY + sin(baseAngle) * mainRadius;
    
    // Draw epicycle circle
    gfx->drawCircle(epicX, epicY, epicycleRadius, 
                    gfx->color565(150 - epic * 30, 150 + epic * 30, 255 - epic * 50));
    
    // Point on epicycle
    float epicAngle = epicycleTime * (5 - epic) * 2; // Different speeds
    int pointX = epicX + cos(epicAngle) * epicycleRadius;
    int pointY = epicY + sin(epicAngle) * epicycleRadius;
    
    // Draw the traced path (simplified - just current points)
    for (int trail = 0; trail < 50; trail++) {
      float trailTime = epicycleTime - trail * 0.02;
      float trailBaseAngle = trailTime * (epic + 1) * 0.7;
      float trailEpicAngle = trailTime * (5 - epic) * 2;
      
      int trailEpicX = centerX + cos(trailBaseAngle) * mainRadius;
      int trailEpicY = centerY + sin(trailBaseAngle) * mainRadius;
      int trailPointX = trailEpicX + cos(trailEpicAngle) * epicycleRadius;
      int trailPointY = trailEpicY + sin(trailEpicAngle) * epicycleRadius;
      
      int alpha = 255 - trail * 5;
      if (alpha > 0) {
        uint16_t trailColor = gfx->color565(alpha/4 + epic * 20, alpha/4, alpha/2);
        gfx->fillCircle(trailPointX, trailPointY, 1, trailColor);
      }
    }
    
    // Current point (brighter)
    gfx->fillCircle(pointX, pointY, 3, 
                    gfx->color565(255, 200 - epic * 50, 100 + epic * 50));
  }
  
  // Center point
  gfx->fillCircle(centerX, centerY, 2, WHITE);
  
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EPICYCLE");
}
}
// Stable slide 76: removed by screensaver policy.
namespace e76 {
float fluidTime = 0;
void reset(){fluidTime=0;}
void drawEuler2d() {
  // 2D Euler fluid dynamics simulation
  gfx->fillScreen(BLACK);
  
  
  fluidTime += 0.05;
  
  // Fluid grid simulation (simplified)
  for (int x = 0; x < gfx->width(); x += 8) {
    for (int y = 0; y < gfx->height(); y += 8) {
      // Velocity field
      float vx = sin(x * 0.02 + fluidTime) * cos(y * 0.03);
      float vy = cos(x * 0.03 - fluidTime) * sin(y * 0.02);
      
      // Pressure field
      float pressure = sin(x * 0.05 + fluidTime * 2) * cos(y * 0.05 + fluidTime);
      
      // Color based on fluid properties
      int r = (int)(abs(vx) * 255);
      int g = (int)(abs(vy) * 255);
      int b = (int)((pressure + 1) * 127);
      
      gfx->fillRect(x, y, 6, 6, gfx->color565(r, g, b));
      
      // Velocity vectors
      if (abs(vx) > 0.3 || abs(vy) > 0.3) {
        int endX = x + vx * 10;
        int endY = y + vy * 10;
        gfx->drawLine(x + 3, y + 3, endX, endY, WHITE);
      }
    }
  }
  
  // Fluid streamlines
  for (int stream = 0; stream < 5; stream++) {
    float startX = 10 + stream * 60;
    float currentX = startX;
    float currentY = 50;
    
    for (int step = 0; step < 30; step++) {
      float vx = sin(currentX * 0.02 + fluidTime) * cos(currentY * 0.03);
      float vy = cos(currentX * 0.03 - fluidTime) * sin(currentY * 0.02);
      
      float nextX = currentX + vx * 3;
      float nextY = currentY + vy * 3;
      
      if (nextX > 0 && nextX < gfx->width() && nextY > 0 && nextY < gfx->height()) {
        gfx->drawLine(currentX, currentY, nextX, nextY, 
                      gfx->color565(255 - step * 8, 200, 255 - step * 5));
        currentX = nextX;
        currentY = nextY;
      } else break;
    }
  }
  
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EULER2D");
}
}
namespace e77 {
float plotTime = 0;
int plotHistory[320];
void reset(){plotTime=0;std::memset(plotHistory,0,sizeof(plotHistory));}
void drawFadeplot() {
  // Fading plot visualization
  gfx->fillScreen(BLACK);
  
  
   // Store previous values
  plotTime += 0.1;
  
  // Generate new plot value
  float newValue = sin(plotTime) * cos(plotTime * 0.7) * sin(plotTime * 0.3);
  int plotY = gfx->height() / 2 + newValue * (gfx->height() / 3);
  
  // Shift history and add new point
  for (int i = 0; i < gfx->width() - 1; i++) {
    plotHistory[i] = plotHistory[i + 1];
  }
  plotHistory[gfx->width() - 1] = plotY;
  
  // Draw fading plot lines
  for (int x = 1; x < gfx->width(); x++) {
    if (plotHistory[x] != 0 && plotHistory[x-1] != 0) {
      // Fade based on age
      float fade = (float)x / gfx->width();
      int alpha = fade * 255;
      
      // Multiple colored plots
      gfx->drawLine(x-1, plotHistory[x-1], x, plotHistory[x], 
                    gfx->color565(alpha, alpha/2, 255 - alpha/2));
      
      // Secondary plot (phase shifted)
      int plotY2 = gfx->height() / 2 + sin(plotTime - x * 0.01) * (gfx->height() / 4);
      if (x > 1) {
        gfx->drawLine(x-1, gfx->height() / 2 + sin(plotTime - (x-1) * 0.01) * (gfx->height() / 4), 
                      x, plotY2, gfx->color565(255 - alpha, alpha, alpha/2));
      }
    }
  }
  
  // Grid lines
  for (int y = 0; y < gfx->height(); y += 20) {
    gfx->drawFastHLine(0, y, gfx->width(), gfx->color565(30, 30, 30));
  }
  for (int x = 0; x < gfx->width(); x += 40) {
    gfx->drawFastVLine(x, 0, gfx->height(), gfx->color565(30, 30, 30));
  }
  
  // Current value indicator
  gfx->fillCircle(gfx->width() - 5, plotY, 3, gfx->color565(255, 255, 0));
  
  gfx->setTextColor(gfx->color565(200, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FADEPLOT");
}
}
namespace e78 {
float esperTime = 0;
void reset(){esperTime=0;}
void drawEsper() {
  // ESP-based psychic visualization
  gfx->fillScreen(BLACK);
  
  
  esperTime += 0.08;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Psychic energy rings
  for (int ring = 0; ring < 6; ring++) {
    float ringPhase = esperTime + ring * 0.5;
    int radius = 20 + ring * 15 + sin(ringPhase) * 10;
    
    // Pulsing energy rings
    for (int segment = 0; segment < 24; segment++) {
      float angle = segment * PI / 12 + ringPhase * 0.3;
      float intensity = sin(ringPhase + segment * 0.2) * 0.5 + 0.5;
      
      if (intensity > 0.3) {
        int x = centerX + cos(angle) * radius;
        int y = centerY + sin(angle) * radius;
        
        int r = 100 + intensity * 155;
        int g = 50 + intensity * 100;
        int b = 200 + intensity * 55;
        
        gfx->fillCircle(x, y, 2 + intensity * 2, gfx->color565(r, g, b));
      }
    }
  }
  
  // Mind waves
  for (int wave = 0; wave < 8; wave++) {
    float wavePhase = esperTime * 2 + wave;
    int waveY = centerY + sin(wavePhase) * 60;
    
    for (int x = 0; x < gfx->width(); x += 4) {
      float waveValue = sin(x * 0.05 + wavePhase) * sin(wavePhase + wave);
      if (abs(waveValue) > 0.5) {
        int alpha = abs(waveValue) * 255;
        gfx->fillRect(x, waveY - 1, 2, 3, gfx->color565(alpha, alpha/3, alpha));
      }
    }
  }
  
  // Central consciousness
  int coreSize = 8 + sin(esperTime * 3) * 4;
  gfx->fillCircle(centerX, centerY, coreSize, gfx->color565(255, 255, 255));
  gfx->drawCircle(centerX, centerY, coreSize + 3, gfx->color565(150, 100, 255));
  
  gfx->setTextColor(gfx->color565(200, 150, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("ESPER");
}
}
namespace e79 {
float venusTime = 0;
void reset(){venusTime=0;}
void drawEtruscanvenus() {
  // Etruscan Venus classical art effect
  gfx->fillScreen(BLACK);
  
  
  venusTime += 0.04;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Classical marble base
  gfx->fillRect(0, gfx->height() - 40, gfx->width(), 40, gfx->color565(200, 190, 180));
  
  // Venus silhouette (simplified)
  int venusH = 100;
  int venusW = 40;
  
  // Body outline
  for (int y = 0; y < venusH; y++) {
    float bodyWidth = sin((float)y / venusH * PI) * venusW;
    int x1 = centerX - bodyWidth / 2;
    int x2 = centerX + bodyWidth / 2;
    
    // Marble texture
    float marble = sin(venusTime + y * 0.1) * 0.2 + 0.8;
    int marbleColor = marble * 220;
    
    gfx->drawLine(x1, centerY - venusH/2 + y, x2, centerY - venusH/2 + y, 
                  gfx->color565(marbleColor, marbleColor - 20, marbleColor - 40));
  }
  
  // Etruscan decorative patterns
  for (int pattern = 0; pattern < 4; pattern++) {
    float patternAngle = pattern * PI / 2 + venusTime * 0.5;
    int patternRadius = 80 + sin(venusTime + pattern) * 20;
    
    for (int segment = 0; segment < 16; segment++) {
      float segmentAngle = patternAngle + segment * PI / 8;
      int x1 = centerX + cos(segmentAngle) * patternRadius;
      int y1 = centerY + sin(segmentAngle) * patternRadius;
      int x2 = centerX + cos(segmentAngle + 0.2) * (patternRadius + 10);
      int y2 = centerY + sin(segmentAngle + 0.2) * (patternRadius + 10);
      
      gfx->drawLine(x1, y1, x2, y2, gfx->color565(180, 140, 100));
    }
  }
  
  // Golden ratio spiral
  float goldenAngle = venusTime;
  for (int spiral = 0; spiral < 50; spiral++) {
    float radius = spiral * 2;
    float angle = goldenAngle + spiral * 0.618 * 2;
    
    int x = centerX + cos(angle) * radius;
    int y = centerY + sin(angle) * radius;
    
    if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
      gfx->fillCircle(x, y, 1, gfx->color565(220, 180, 120));
    }
  }
  
  gfx->setTextColor(gfx->color565(200, 160, 120));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("ETRUSCANVENUS");
}
}
// Stable slide 81: removed by screensaver policy.
namespace e81 {
float idleTime = 0;
void reset(){idleTime=0;}
void drawExtIdleNotify() {
  // External idle notification protocol visualization
  gfx->fillScreen(BLACK);
  
  
  idleTime += 0.06;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Idle state indicator
  float idleLevel = sin(idleTime * 0.5) * 0.5 + 0.5;
  
  // Central idle monitor
  int monitorSize = 40 + idleLevel * 20;
  gfx->drawCircle(centerX, centerY, monitorSize, gfx->color565(100, 200, 255));
  gfx->fillCircle(centerX, centerY, monitorSize - 5, gfx->color565(20, 50, 100));
  
  // Protocol messages
  for (int msg = 0; msg < 6; msg++) {
    float msgAngle = msg * PI / 3 + idleTime * 0.3;
    int msgRadius = 80 + sin(idleTime + msg) * 20;
    
    int x = centerX + cos(msgAngle) * msgRadius;
    int y = centerY + sin(msgAngle) * msgRadius;
    
    // Message packets
    if (sin(idleTime * 2 + msg) > 0) {
      gfx->fillRect(x - 8, y - 4, 16, 8, gfx->color565(255, 200, 0));
      gfx->setTextColor(BLACK);
      gfx->setTextSize(1);
      gfx->setCursor(x - 6, y - 3);
      gfx->printf("ID%d", msg);
    }
    
    // Connection lines
    gfx->drawLine(centerX, centerY, x, y, gfx->color565(80, 80, 255));
  }
  
  // Idle percentage display
  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(centerX - 20, centerY - 8);
  gfx->printf("%d%%", (int)(idleLevel * 100));
  
  // Status text
  gfx->setTextColor(gfx->color565(100, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, gfx->height() - 20);
  if (idleLevel > 0.7) {
    gfx->printf("SYSTEM IDLE");
  } else if (idleLevel > 0.3) {
    gfx->printf("MODERATE LOAD");
  } else {
    gfx->printf("HIGH ACTIVITY");
  }
  
  gfx->setTextColor(gfx->color565(100, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EXT-IDLE-NOTIFY");
}
}
namespace e82 {
float extrudeTime = 0;
void reset(){extrudeTime=0;}
void drawExtrusion() {
  // 3D extrusion effect
  gfx->fillScreen(BLACK);
  
  
  extrudeTime += 0.08;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Base shape (hexagon)
  int baseRadius = 40;
  int extrudeDepth = 30 + sin(extrudeTime) * 20;
  
  // Draw extruded hexagon
  for (int depth = 0; depth < extrudeDepth; depth++) {
    float depthRatio = (float)depth / extrudeDepth;
    int currentRadius = baseRadius + depth * 0.5;
    
    // Color fades with depth
    int r = 100 + depthRatio * 155;
    int g = 50 + depthRatio * 100;
    int b = 200 - depthRatio * 100;
    
    // Draw hexagon at this depth level
    for (int side = 0; side < 6; side++) {
      float angle1 = side * PI / 3 + extrudeTime * 0.2;
      float angle2 = (side + 1) * PI / 3 + extrudeTime * 0.2;
      
      int x1 = centerX + cos(angle1) * currentRadius - depth * 0.3;
      int y1 = centerY + sin(angle1) * currentRadius - depth * 0.2;
      int x2 = centerX + cos(angle2) * currentRadius - depth * 0.3;
      int y2 = centerY + sin(angle2) * currentRadius - depth * 0.2;
      
      gfx->drawLine(x1, y1, x2, y2, gfx->color565(r, g, b));
      
      // Connect to previous depth level
      if (depth > 0) {
        int prevRadius = baseRadius + (depth - 1) * 0.5;
        int px1 = centerX + cos(angle1) * prevRadius - (depth - 1) * 0.3;
        int py1 = centerY + sin(angle1) * prevRadius - (depth - 1) * 0.2;
        
        gfx->drawLine(px1, py1, x1, y1, gfx->color565(r - 30, g - 30, b));
      }
    }
  }
  
  // Surface normal vectors
  for (int normal = 0; normal < 6; normal++) {
    float normalAngle = normal * PI / 3 + extrudeTime * 0.2;
    int surfaceX = centerX + cos(normalAngle) * (baseRadius + extrudeDepth * 0.5);
    int surfaceY = centerY + sin(normalAngle) * (baseRadius + extrudeDepth * 0.5);
    
    int normalX = surfaceX + cos(normalAngle) * 15;
    int normalY = surfaceY + sin(normalAngle) * 15;
    
    gfx->drawLine(surfaceX, surfaceY, normalX, normalY, gfx->color565(255, 255, 100));
    gfx->fillCircle(normalX, normalY, 2, gfx->color565(255, 200, 0));
  }
  
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EXTRUSION");
}
}
namespace e83 {
float helix2Time = 0;
void reset(){helix2Time=0;}
void drawExtrusionHelix2() {
  // Double helix extrusion
  gfx->fillScreen(BLACK);
  
  
  helix2Time += 0.1;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Double helix parameters
  float helixHeight = gfx->height() - 40;
  float helixRadius = 30;
  
  // Draw two intertwining helixes
  for (int helix = 0; helix < 2; helix++) {
    float helixOffset = helix * PI;
    
    for (int segment = 0; segment < 60; segment++) {
      float t = segment / 60.0;
      float y = 20 + t * helixHeight;
      float angle = t * 4 * PI + helix2Time + helixOffset;
      
      int x = centerX + cos(angle) * helixRadius;
      int z = sin(angle) * helixRadius; // Depth simulation
      
      // Color based on depth and helix
      int r = helix == 0 ? 255 : 100;
      int g = 100 + abs(z) * 3;
      int b = helix == 1 ? 255 : 100;
      
      // Size based on depth
      int size = 2 + (z + helixRadius) / (helixRadius * 2) * 3;
      
      gfx->fillCircle(x, y, size, gfx->color565(r, g, b));
      
      // Connect segments
      if (segment > 0) {
        float prevT = (segment - 1) / 60.0;
        float prevY = 20 + prevT * helixHeight;
        float prevAngle = prevT * 4 * PI + helix2Time + helixOffset;
        int prevX = centerX + cos(prevAngle) * helixRadius;
        
        gfx->drawLine(prevX, prevY, x, y, gfx->color565(r/2, g/2, b/2));
      }
    }
  }
  
  gfx->setTextColor(gfx->color565(255, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EXTRUSION-HELIX2");
}
}
namespace e84 {
float helix3Time = 0;
void reset(){helix3Time=0;}
void drawExtrusionHelix3() {
  // Triple helix extrusion
  gfx->fillScreen(BLACK);
  
  
  helix3Time += 0.08;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Triple helix parameters
  float helixHeight = gfx->height() - 40;
  float helixRadius = 25;
  
  // Draw three intertwining helixes
  for (int helix = 0; helix < 3; helix++) {
    float helixOffset = helix * 2 * PI / 3;
    
    // Helix colors
    int colors[3][3] = {{255, 100, 100}, {100, 255, 100}, {100, 100, 255}};
    
    for (int segment = 0; segment < 50; segment++) {
      float t = segment / 50.0;
      float y = 20 + t * helixHeight;
      float angle = t * 3 * PI + helix3Time + helixOffset;
      
      int x = centerX + cos(angle) * helixRadius;
      int z = sin(angle) * helixRadius;
      
      // Size and color based on position and depth
      int size = 2 + (z + helixRadius) / (helixRadius * 2) * 2;
      int intensity = 150 + (z + helixRadius) / (helixRadius * 2) * 105;
      
      int r = colors[helix][0] * intensity / 255;
      int g = colors[helix][1] * intensity / 255;
      int b = colors[helix][2] * intensity / 255;
      
      gfx->fillCircle(x, y, size, gfx->color565(r, g, b));
      
      // Connecting strands
      if (segment > 0 && segment % 3 == 0) {
        // Connect to next helix
        int nextHelix = (helix + 1) % 3;
        float nextAngle = t * 3 * PI + helix3Time + nextHelix * 2 * PI / 3;
        int nextX = centerX + cos(nextAngle) * helixRadius;
        
        gfx->drawLine(x, y, nextX, y, gfx->color565(100, 100, 100));
      }
    }
  }
  
  gfx->setTextColor(gfx->color565(200, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EXTRUSION-HELIX3");
}
}
namespace e85 {
float helix4Time = 0;
void reset(){helix4Time=0;}
void drawExtrusionHelix4() {
  // Quadruple helix extrusion
  gfx->fillScreen(BLACK);
  
  
  helix4Time += 0.06;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Quadruple helix parameters
  float helixHeight = gfx->height() - 40;
  float helixRadius = 20;
  
  // Draw four intertwining helixes
  for (int helix = 0; helix < 4; helix++) {
    float helixOffset = helix * PI / 2;
    
    // Different radius for each helix to create complex pattern
    float currentRadius = helixRadius + helix * 3;
    
    for (int segment = 0; segment < 40; segment++) {
      float t = segment / 40.0;
      float y = 20 + t * helixHeight;
      float angle = t * 2.5 * PI + helix4Time + helixOffset;
      
      int x = centerX + cos(angle) * currentRadius;
      int z = sin(angle) * currentRadius;
      
      // Unique color for each helix
      int r = (helix == 0 || helix == 2) ? 255 : 80;
      int g = (helix == 1 || helix == 2) ? 255 : 80;
      int b = (helix == 0 || helix == 1) ? 255 : 80;
      
      // Intensity based on depth
      float intensity = (z + currentRadius) / (currentRadius * 2);
      r = r * (0.5 + intensity * 0.5);
      g = g * (0.5 + intensity * 0.5);
      b = b * (0.5 + intensity * 0.5);
      
      int size = 1 + intensity * 3;
      gfx->fillCircle(x, y, size, gfx->color565(r, g, b));
      
      // Cross-connections between helixes
      if (segment % 5 == 0) {
        for (int otherHelix = helix + 1; otherHelix < 4; otherHelix++) {
          float otherOffset = otherHelix * PI / 2;
          float otherRadius = helixRadius + otherHelix * 3;
          float otherAngle = t * 2.5 * PI + helix4Time + otherOffset;
          int otherX = centerX + cos(otherAngle) * otherRadius;
          
          // Only draw connection if helixes are close
          if (abs(otherX - x) < 15) {
            gfx->drawLine(x, y, otherX, y, gfx->color565(60, 60, 60));
          }
        }
      }
    }
  }
  
  gfx->setTextColor(gfx->color565(255, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EXTRUSION-HELIX4");
}
}
namespace e86 {
float joinTime = 0;
void reset(){joinTime=0;}
void drawExtrusionJoinoffset() {
  // Extrusion with offset joins
  gfx->fillScreen(BLACK);
  
  
  joinTime += 0.09;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple extrusion paths that join with offsets
  for (int path = 0; path < 4; path++) {
    float pathAngle = path * PI / 2 + joinTime * 0.3;
    int pathLength = 80;
    
    // Main extrusion line
    int startX = centerX + cos(pathAngle) * 20;
    int startY = centerY + sin(pathAngle) * 20;
    int endX = centerX + cos(pathAngle) * pathLength;
    int endY = centerY + sin(pathAngle) * pathLength;
    
    // Draw main path segments
    for (int segment = 0; segment < 20; segment++) {
      float t = segment / 20.0;
      int x = startX + (endX - startX) * t;
      int y = startY + (endY - startY) * t;
      
      // Thickness varies along path
      int thickness = 3 + sin(joinTime + segment * 0.3) * 2;
      
      // Color changes along path
      int r = 150 + t * 105;
      int g = 100 + sin(joinTime + t * PI) * 100;
      int b = 200 - t * 100;
      
      gfx->fillCircle(x, y, thickness, gfx->color565(r, g, b));
      
      // Offset joins at specific points
      if (segment % 5 == 0) {
        float offsetAngle = pathAngle + PI / 2;
        int offsetDist = 15 + sin(joinTime + segment) * 10;
        int offsetX = x + cos(offsetAngle) * offsetDist;
        int offsetY = y + sin(offsetAngle) * offsetDist;
        
        // Draw join connector
        gfx->drawLine(x, y, offsetX, offsetY, gfx->color565(255, 200, 100));
        gfx->fillCircle(offsetX, offsetY, 2, gfx->color565(255, 255, 0));
      }
    }
  }
  
  // Central junction with animated offset
  int junctionOffset = sin(joinTime * 2) * 8;
  gfx->fillCircle(centerX + junctionOffset, centerY, 8, gfx->color565(255, 255, 255));
  gfx->drawCircle(centerX + junctionOffset, centerY, 12, gfx->color565(200, 200, 200));
  
  gfx->setTextColor(gfx->color565(255, 200, 100));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EXTRUSION-JOINOFFSET");
}
}
namespace e87 {
float screwTime = 0;
void reset(){screwTime=0;}
void drawExtrusionScrew() {
  // Screw thread extrusion effect
  gfx->fillScreen(BLACK);
  
  
  screwTime += 0.12;
  
  int centerX = gfx->width() / 2;
  int screwHeight = gfx->height() - 40;
  
  // Screw shaft
  gfx->fillRect(centerX - 8, 20, 16, screwHeight, gfx->color565(150, 150, 150));
  
  // Screw threads (helical pattern)
  for (int thread = 0; thread < screwHeight; thread += 2) {
    float threadAngle = thread * 0.2 + screwTime;
    int threadRadius = 12 + sin(screwTime + thread * 0.1) * 3;
    
    // Left thread
    int leftX = centerX + cos(threadAngle) * threadRadius;
    int leftY = 20 + thread;
    
    // Right thread  
    int rightX = centerX + cos(threadAngle + PI) * threadRadius;
    int rightY = 20 + thread;
    
    // Thread depth shading
    float depth = cos(threadAngle) * 0.5 + 0.5;
    int threadBrightness = 100 + depth * 155;
    
    gfx->fillCircle(leftX, leftY, 2, gfx->color565(threadBrightness, threadBrightness, threadBrightness));
    gfx->fillCircle(rightX, rightY, 2, gfx->color565(threadBrightness, threadBrightness, threadBrightness));
    
    // Connect thread segments
    if (thread > 0) {
      float prevAngle = (thread - 2) * 0.2 + screwTime;
      int prevLeftX = centerX + cos(prevAngle) * threadRadius;
      int prevRightX = centerX + cos(prevAngle + PI) * threadRadius;
      
      gfx->drawLine(prevLeftX, leftY - 2, leftX, leftY, gfx->color565(180, 180, 180));
      gfx->drawLine(prevRightX, rightY - 2, rightX, rightY, gfx->color565(180, 180, 180));
    }
  }
  
  // Screw head
  gfx->fillCircle(centerX, 15, 15, gfx->color565(200, 200, 200));
  gfx->drawCircle(centerX, 15, 15, gfx->color565(100, 100, 100));
  
  // Phillips head pattern
  gfx->drawLine(centerX - 8, 15, centerX + 8, 15, gfx->color565(80, 80, 80));
  gfx->drawLine(centerX, 7, centerX, 23, gfx->color565(80, 80, 80));
  
  // Rotation indication
  float rotAngle = screwTime * 2;
  for (int mark = 0; mark < 8; mark++) {
    float markAngle = mark * PI / 4 + rotAngle;
    int markX = centerX + cos(markAngle) * 18;
    int markY = 15 + sin(markAngle) * 18;
    gfx->fillCircle(markX, markY, 1, gfx->color565(255, 200, 0));
  }
  
  gfx->setTextColor(gfx->color565(200, 200, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EXTRUSION-SCREW");
}
}
namespace e88 {
float taperTime = 0;
void reset(){taperTime=0;}
void drawExtrusionTaper() {
  // Tapered extrusion effect
  gfx->fillScreen(BLACK);
  
  
  taperTime += 0.07;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple tapered extrusions radiating from center
  for (int taper = 0; taper < 6; taper++) {
    float taperAngle = taper * PI / 3 + taperTime * 0.2;
    int maxLength = 100;
    
    // Draw tapered extrusion segments
    for (int segment = 0; segment < 30; segment++) {
      float t = segment / 30.0;
      int distance = t * maxLength;
      
      int x = centerX + cos(taperAngle) * distance;
      int y = centerY + sin(taperAngle) * distance;
      
      // Taper: size decreases with distance, pulsates
      float baseTaper = (1.0 - t * 0.8);
      float pulseTaper = 1.0 + sin(taperTime * 3 + segment * 0.2) * 0.3;
      int size = (baseTaper * pulseTaper * 8) + 1;
      
      // Color shifts along taper
      int r = 255 * (1.0 - t * 0.5);
      int g = 100 + t * 155;
      int b = 150 + sin(taperTime + t * PI) * 105;
      
      gfx->fillCircle(x, y, size, gfx->color565(r, g, b));
      
      // Taper outline for depth
      if (size > 2) {
        gfx->drawCircle(x, y, size + 1, gfx->color565(r/3, g/3, b/3));
      }
    }
    
    // Branch tapers at midpoints
    if (taper % 2 == 0) {
      float branchAngle = taperAngle + PI / 6;
      for (int branch = 10; branch < 20; branch++) {
        float bt = (branch - 10) / 10.0;
        int branchDist = bt * 50;
        
        int startX = centerX + cos(taperAngle) * (branch * maxLength / 30);
        int startY = centerY + sin(taperAngle) * (branch * maxLength / 30);
        
        int branchX = startX + cos(branchAngle) * branchDist;
        int branchY = startY + sin(branchAngle) * branchDist;
        
        int branchSize = (1.0 - bt) * 4 + 1;
        gfx->fillCircle(branchX, branchY, branchSize, gfx->color565(255, 200 - bt * 100, 100));
      }
    }
  }
  
  // Central core
  int coreSize = 6 + sin(taperTime * 4) * 2;
  gfx->fillCircle(centerX, centerY, coreSize, gfx->color565(255, 255, 255));
  gfx->drawCircle(centerX, centerY, coreSize + 2, gfx->color565(200, 200, 0));
  
  gfx->setTextColor(gfx->color565(255, 200, 150));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EXTRUSION-TAPER");
}
}
namespace e89 {
float twistTime = 0;
void reset(){twistTime=0;}
void drawExtrusionTwistoid() {
  // Twisting toroidal extrusion effect
  gfx->fillScreen(BLACK);
  
  
  twistTime += 0.08;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Twistoid parameters
  float majorRadius = 60;
  float minorRadius = 20;
  float twistRate = 3.0;
  
  // Draw twisted torus
  for (int u = 0; u < 36; u++) {
    for (int v = 0; v < 24; v++) {
      float uAngle = u * PI / 18 + twistTime * 0.5;
      float vAngle = v * PI / 12 + twistTime * twistRate;
      
      // Twist factor varies along major radius
      float twistFactor = sin(uAngle * 2 + twistTime) * 0.5;
      float actualVAngle = vAngle + twistFactor;
      
      // Parametric torus equations with twist
      float x = (majorRadius + minorRadius * cos(actualVAngle)) * cos(uAngle);
      float y = (majorRadius + minorRadius * cos(actualVAngle)) * sin(uAngle);
      float z = minorRadius * sin(actualVAngle);
      
      // Project to 2D
      int screenX = centerX + x * 0.8;
      int screenY = centerY + y * 0.6 + z * 0.3;
      
      // Color based on twist and depth
      float depthFactor = (z + minorRadius) / (2 * minorRadius);
      int r = 100 + depthFactor * 155 + twistFactor * 100;
      int g = 150 + sin(uAngle + twistTime) * 105;
      int b = 200 + cos(actualVAngle) * 55;
      
      // Size based on depth
      int size = 1 + depthFactor * 3;
      
      if (screenX >= 0 && screenX < gfx->width() && screenY >= 0 && screenY < gfx->height()) {
        gfx->fillCircle(screenX, screenY, size, gfx->color565(r, g, b));
      }
      
      // Connect adjacent points for wireframe effect
      if (u > 0 && v > 0) {
        float prevUAngle = (u-1) * PI / 18 + twistTime * 0.5;
        float prevVAngle = (v-1) * PI / 12 + twistTime * twistRate;
        float prevTwist = sin(prevUAngle * 2 + twistTime) * 0.5;
        float prevActualVAngle = prevVAngle + prevTwist;
        
        float prevX = (majorRadius + minorRadius * cos(prevActualVAngle)) * cos(prevUAngle);
        float prevY = (majorRadius + minorRadius * cos(prevActualVAngle)) * sin(prevUAngle);
        float prevZ = minorRadius * sin(prevActualVAngle);
        
        int prevScreenX = centerX + prevX * 0.8;
        int prevScreenY = centerY + prevY * 0.6 + prevZ * 0.3;
        
        if (abs(screenX - prevScreenX) < 20 && abs(screenY - prevScreenY) < 20) {
          gfx->drawLine(screenX, screenY, prevScreenX, prevScreenY, gfx->color565(r/4, g/4, b/4));
        }
      }
    }
  }
  
  gfx->setTextColor(gfx->color565(255, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EXTRUSION-TWISTOID");
}
}
namespace e90 {
float extTime = 0;
void reset(){extTime=0;}
void drawExts() {
  // Extensions system visualization
  gfx->fillScreen(BLACK);
  
  
  extTime += 0.1;
  
  // Extension modules arranged in grid
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 4; col++) {
      int moduleX = 20 + col * 70;
      int moduleY = 30 + row * 60;
      
      // Module activity
      float activity = sin(extTime + row + col) * 0.5 + 0.5;
      
      // Module box
      int moduleSize = 25 + activity * 10;
      gfx->drawRect(moduleX - moduleSize/2, moduleY - moduleSize/2, moduleSize, moduleSize, 
                    gfx->color565(100, 150, 200));
      
      // Active modules have colored fill
      if (activity > 0.6) {
        gfx->fillRect(moduleX - moduleSize/2 + 2, moduleY - moduleSize/2 + 2, 
                     moduleSize - 4, moduleSize - 4, 
                     gfx->color565(0, 255 * activity, 100));
      }
      
      // Extension connections
      if (col < 3) {
        // Horizontal connections
        gfx->drawLine(moduleX + moduleSize/2, moduleY, 
                     moduleX + 70 - moduleSize/2, moduleY,
                     gfx->color565(150, 150, 150));
        
        // Data flow indicators
        if (sin(extTime * 3 - col) > 0) {
          int flowX = moduleX + moduleSize/2 + (70 - moduleSize) * ((sin(extTime * 2) + 1) / 2);
          gfx->fillCircle(flowX, moduleY, 2, gfx->color565(255, 255, 0));
        }
      }
      
      if (row < 2) {
        // Vertical connections
        gfx->drawLine(moduleX, moduleY + moduleSize/2,
                     moduleX, moduleY + 60 - moduleSize/2,
                     gfx->color565(150, 150, 150));
      }
      
      // Extension ID
      gfx->setTextColor(WHITE);
      gfx->setTextSize(1);
      gfx->setCursor(moduleX - 6, moduleY - 3);
      gfx->printf("E%d", row * 4 + col);
    }
  }
  
  // Status bar
  gfx->drawRect(10, gfx->height() - 25, gfx->width() - 20, 20, gfx->color565(100, 100, 100));
  
  // Active extension count
  int activeCount = (sin(extTime) * 6 + 6);
  gfx->setTextColor(gfx->color565(0, 255, 0));
  gfx->setCursor(15, gfx->height() - 20);
  gfx->printf("ACTIVE: %d/12", activeCount);
  
  gfx->setTextColor(gfx->color565(200, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EXTS");
}
}
namespace e91 {
float lockTime = 0;
void reset(){lockTime=0;}
void drawExtSessionLock() {
  // External session lock protocol
  gfx->fillScreen(BLACK);
  
  
  lockTime += 0.06;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Lock state (cycles between locked/unlocked)
  bool isLocked = sin(lockTime * 0.8) > 0;
  
  // Lock mechanism visualization
  if (isLocked) {
    // Locked state - closed padlock
    gfx->drawRect(centerX - 20, centerY - 5, 40, 25, gfx->color565(200, 100, 100));
    gfx->fillRect(centerX - 18, centerY - 3, 36, 21, gfx->color565(150, 50, 50));
    
    // Shackle (closed)
    gfx->drawCircle(centerX, centerY - 15, 12, gfx->color565(180, 180, 180));
    gfx->fillCircle(centerX, centerY - 15, 8, BLACK);
    gfx->fillRect(centerX - 8, centerY - 15, 16, 10, BLACK);
    
    // Lock status
    gfx->setTextColor(gfx->color565(255, 100, 100));
    gfx->setCursor(centerX - 30, centerY + 40);
    gfx->printf("LOCKED");
  } else {
    // Unlocked state - open padlock
    gfx->drawRect(centerX - 20, centerY - 5, 40, 25, gfx->color565(100, 200, 100));
    gfx->fillRect(centerX - 18, centerY - 3, 36, 21, gfx->color565(50, 150, 50));
    
    // Shackle (open)
    gfx->drawCircle(centerX - 5, centerY - 15, 12, gfx->color565(180, 180, 180));
    gfx->fillCircle(centerX - 5, centerY - 15, 8, BLACK);
    gfx->fillRect(centerX - 13, centerY - 15, 16, 8, BLACK);
    
    // Lock status
    gfx->setTextColor(gfx->color565(100, 255, 100));
    gfx->setCursor(centerX - 35, centerY + 40);
    gfx->printf("UNLOCKED");
  }
  
  // Session indicators around the lock
  for (int session = 0; session < 8; session++) {
    float sessionAngle = session * PI / 4 + lockTime;
    int sessionRadius = 50;
    
    int sessionX = centerX + cos(sessionAngle) * sessionRadius;
    int sessionY = centerY + sin(sessionAngle) * sessionRadius;
    
    // Session activity
    bool sessionActive = sin(lockTime * 2 + session) > 0;
    
    if (sessionActive) {
      gfx->fillCircle(sessionX, sessionY, 4, gfx->color565(255, 255, 0));
      gfx->drawLine(centerX, centerY, sessionX, sessionY, gfx->color565(100, 100, 100));
    } else {
      gfx->drawCircle(sessionX, sessionY, 4, gfx->color565(100, 100, 100));
    }
    
    // Session ID
    gfx->setTextColor(WHITE);
    gfx->setTextSize(1);
    gfx->setCursor(sessionX - 3, sessionY - 3);
    gfx->printf("%d", session);
  }
  
  // Protocol status
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("EXT-SESSION-LOCK-V1");
  
  gfx->setTextColor(gfx->color565(150, 150, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("EXT-SESSION-LOCK");
}
}
namespace e92 {
float fadeTime = 0;
void reset(){fadeTime=0;}
void drawFade() {
  // Fade in/out effect with multiple layers
  gfx->fillScreen(BLACK);
  
  
  fadeTime += 0.05;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple fading layers with different rates
  for (int layer = 0; layer < 5; layer++) {
    float layerPhase = fadeTime + layer * 0.8;
    float fadeLevel = (sin(layerPhase) + 1) / 2; // 0 to 1
    
    // Layer properties
    int layerRadius = 20 + layer * 15;
    int layerSize = fadeLevel * layerRadius;
    
    // Fade color based on layer and time
    int r = fadeLevel * (200 + layer * 10);
    int g = fadeLevel * (100 + sin(layerPhase * 2) * 100);
    int b = fadeLevel * (255 - layer * 30);
    
    // Draw fading circles
    for (int ring = 0; ring < 8; ring++) {
      float ringAngle = ring * PI / 4 + fadeTime * 0.3;
      int ringX = centerX + cos(ringAngle) * layerSize;
      int ringY = centerY + sin(ringAngle) * layerSize;
      
      int ringSize = fadeLevel * (8 - ring);
      if (ringSize > 0) {
        gfx->fillCircle(ringX, ringY, ringSize, gfx->color565(r, g, b));
      }
    }
    
    // Fading lines radiating outward
    for (int line = 0; line < 12; line++) {
      float lineAngle = line * PI / 6 + fadeTime * 0.2;
      int lineLength = fadeLevel * 60;
      
      int x1 = centerX + cos(lineAngle) * 10;
      int y1 = centerY + sin(lineAngle) * 10;
      int x2 = centerX + cos(lineAngle) * lineLength;
      int y2 = centerY + sin(lineAngle) * lineLength;
      
      gfx->drawLine(x1, y1, x2, y2, gfx->color565(r/2, g/2, b/2));
    }
  }
  
  // Central fade indicator
  float centralFade = (sin(fadeTime * 2) + 1) / 2;
  int centralSize = centralFade * 12;
  gfx->fillCircle(centerX, centerY, centralSize, gfx->color565(255, 255, 255));
  
  // Fade percentage text
  gfx->setTextColor(gfx->color565(255, 255, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("FADE: %d%%", (int)(centralFade * 100));
  
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FADE");
}
}
namespace e93 {
float streamTime = 0;
void reset(){streamTime=0;}
void drawFfmpegOut() {
  // FFmpeg output stream visualization
  gfx->fillScreen(BLACK);
  
  
  streamTime += 0.1;
  
  // Video stream header
  gfx->drawRect(5, 5, gfx->width() - 10, 20, gfx->color565(100, 100, 100));
  gfx->fillRect(6, 6, gfx->width() - 12, 18, gfx->color565(20, 20, 20));
  
  gfx->setTextColor(gfx->color565(0, 255, 0));
  gfx->setTextSize(1);
  gfx->setCursor(8, 10);
  gfx->printf("FFmpeg Output Stream");
  
  // Encoding progress bar
  float progress = (sin(streamTime * 0.5) + 1) / 2;
  int progressWidth = progress * (gfx->width() - 20);
  
  gfx->drawRect(10, 30, gfx->width() - 20, 8, gfx->color565(100, 100, 100));
  gfx->fillRect(11, 31, progressWidth, 6, gfx->color565(0, 200, 0));
  
  // Frame data visualization
  int frameHeight = 100;
  int frameY = 45;
  
  // Video frames as colored rectangles
  for (int frame = 0; frame < 20; frame++) {
    int frameX = 10 + frame * 14;
    float frameIntensity = sin(streamTime * 2 + frame * 0.5) * 0.5 + 0.5;
    
    // Frame compression indicator
    int frameSize = frameHeight * (0.3 + frameIntensity * 0.7);
    
    // Color based on frame type (I, P, B frames)
    int frameType = frame % 3;
    int r, g, b;
    switch (frameType) {
      case 0: // I-frame (keyframe)
        r = 255 * frameIntensity;
        g = 100;
        b = 100;
        break;
      case 1: // P-frame 
        r = 100;
        g = 255 * frameIntensity;
        b = 100;
        break;
      case 2: // B-frame
        r = 100;
        g = 100;
        b = 255 * frameIntensity;
        break;
    }
    
    gfx->fillRect(frameX, frameY + frameHeight - frameSize, 12, frameSize, 
                  gfx->color565(r, g, b));
    gfx->drawRect(frameX, frameY, 12, frameHeight, gfx->color565(80, 80, 80));
  }
  
  // Codec information
  gfx->setTextColor(gfx->color565(200, 200, 200));
  gfx->setCursor(10, frameY + frameHeight + 10);
  gfx->printf("Codec: H.264");
  
  gfx->setCursor(10, frameY + frameHeight + 20);
  gfx->printf("Bitrate: %d kbps", (int)(1000 + progress * 2000));
  
  gfx->setCursor(10, frameY + frameHeight + 30);
  gfx->printf("FPS: %.1f", 30.0 + sin(streamTime) * 5);
  
  // Output stream indicators
  for (int stream = 0; stream < 3; stream++) {
    int streamY = frameY + frameHeight + 50 + stream * 15;
    bool streamActive = sin(streamTime * 3 + stream) > 0;
    
    uint16_t streamColor = streamActive ? gfx->color565(0, 255, 0) : gfx->color565(100, 100, 100);
    gfx->fillCircle(15, streamY, 3, streamColor);
    
    gfx->setTextColor(streamColor);
    gfx->setCursor(25, streamY - 3);
    switch (stream) {
      case 0: gfx->printf("Video Out"); break;
      case 1: gfx->printf("Audio Out"); break;
      case 2: gfx->printf("Meta Out"); break;
    }
  }
  
  gfx->setTextColor(gfx->color565(255, 200, 100));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FFMPEG-OUT");
}
}
// Stable slide 95: removed by screensaver policy.
namespace e95 {
float filmTime = 0;
void reset(){filmTime=0;}
void drawFilmleader() {
  // Film leader countdown effect
  gfx->fillScreen(BLACK);
  
  
  filmTime += 0.08;
  
  // Film strip background
  gfx->fillRect(0, 0, gfx->width(), gfx->height(), BLACK);
  
  // Film perforations on sides
  for (int perf = 0; perf < gfx->height(); perf += 12) {
    // Left perforations
    gfx->fillRect(2, perf + 2, 6, 8, gfx->color565(40, 40, 40));
    // Right perforations  
    gfx->fillRect(gfx->width() - 8, perf + 2, 6, 8, gfx->color565(40, 40, 40));
  }
  
  // Film edges
  gfx->drawFastVLine(12, 0, gfx->height(), gfx->color565(100, 100, 100));
  gfx->drawFastVLine(gfx->width() - 13, 0, gfx->height(), gfx->color565(100, 100, 100));
  
  // Countdown circle
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  int circleRadius = 60;
  
  gfx->drawCircle(centerX, centerY, circleRadius, gfx->color565(200, 200, 200));
  gfx->drawCircle(centerX, centerY, circleRadius - 5, gfx->color565(150, 150, 150));
  
  // Countdown number
  int countdown = (int)(8 - fmod(filmTime * 0.8, 8));
  if (countdown <= 0) countdown = 8;
  
  gfx->setTextColor(gfx->color565(255, 255, 255));
  gfx->setTextSize(4);
  gfx->setCursor(centerX - 12, centerY - 16);
  gfx->printf("%d", countdown);
  
  // Rotating sweep hand
  float sweepAngle = fmod(filmTime * 2, 2 * PI);
  int sweepX = centerX + cos(sweepAngle - PI/2) * (circleRadius - 10);
  int sweepY = centerY + sin(sweepAngle - PI/2) * (circleRadius - 10);
  gfx->drawLine(centerX, centerY, sweepX, sweepY, gfx->color565(255, 200, 0));
  
  // Film scratches and artifacts
  for (int scratch = 0; scratch < 5; scratch++) {
    int scratchX = 20 + scratch * 50;
    int scratchY = (int)(sin(filmTime + scratch) * 30) + gfx->height() / 2;
    int scratchLen = 10 + random(20);
    
    gfx->drawFastVLine(scratchX, scratchY, scratchLen, gfx->color565(80, 80, 80));
  }
  
  // Dust spots
  for (int dust = 0; dust < 8; dust++) {
    int dustX = 15 + random(gfx->width() - 30);
    int dustY = random(gfx->height());
    
    if (sin(filmTime * 3 + dust) > 0.5) {
      gfx->fillCircle(dustX, dustY, 1, gfx->color565(120, 120, 120));
    }
  }
  
  // Academy leader markings
  for (int mark = 0; mark < 4; mark++) {
    float markAngle = mark * PI / 2;
    int markX = centerX + cos(markAngle) * (circleRadius + 15);
    int markY = centerY + sin(markAngle) * (circleRadius + 15);
    gfx->drawLine(centerX + cos(markAngle) * circleRadius, 
                  centerY + sin(markAngle) * circleRadius,
                  markX, markY, gfx->color565(180, 180, 180));
  }
  
  gfx->setTextColor(gfx->color565(200, 200, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FILMLEADER");
}
}
// Stable slide 97: removed by screensaver policy.
namespace e97 {
float flagTime = 0;
void reset(){flagTime=0;}
void drawFlag() {
  // Waving flag effect
  gfx->fillScreen(BLACK);
  
  
  flagTime += 0.1;
  
  int flagWidth = gfx->width() - 40;
  int flagHeight = 80;
  int flagX = 20;
  int flagY = 40;
  
  // Flag pole
  gfx->drawFastVLine(flagX - 5, 0, gfx->height(), gfx->color565(139, 69, 19));
  gfx->fillRect(flagX - 7, 0, 4, gfx->height(), gfx->color565(101, 67, 33));
  
  // Waving flag mesh
  for (int y = 0; y < flagHeight; y += 4) {
    for (int x = 0; x < flagWidth; x += 4) {
      // Wave calculation
      float waveX = x / 20.0;
      float waveY = y / 40.0;
      float wave = sin(waveX + flagTime * 2) * cos(waveY + flagTime) * 8;
      
      int pixelX = flagX + x + wave;
      int pixelY = flagY + y;
      
      // Flag colors - simple tricolor
      int colorBand = (y * 3) / flagHeight;
      uint16_t bandColor;
      switch (colorBand) {
        case 0: bandColor = gfx->color565(255, 0, 0); break; // Red
        case 1: bandColor = gfx->color565(255, 255, 255); break; // White  
        case 2: bandColor = gfx->color565(0, 0, 255); break; // Blue
        default: bandColor = gfx->color565(255, 255, 255); break;
      }
      
      // Shading based on wave position for 3D effect
      float shading = 1.0 + sin(waveX + flagTime * 2) * 0.3;
      int r = ((bandColor >> 11) & 0x1F) * shading;
      int g = ((bandColor >> 5) & 0x3F) * shading;
      int b = (bandColor & 0x1F) * shading;
      
      // Clamp values
      r = r > 31 ? 31 : r;
      g = g > 63 ? 63 : g;
      b = b > 31 ? 31 : b;
      
      gfx->fillRect(pixelX, pixelY, 3, 3, (r << 11) | (g << 5) | b);
    }
  }
  
  // Wind lines
  for (int wind = 0; wind < 5; wind++) {
    int windY = 20 + wind * 30;
    int windX = flagX + flagWidth + 10 + sin(flagTime * 3 + wind) * 15;
    gfx->drawLine(windX, windY, windX + 20, windY, gfx->color565(150, 150, 150));
  }
  
  gfx->setTextColor(gfx->color565(255, 255, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FLAG");
}
}
namespace e98 {
float flameTime = 0;
void reset(){flameTime=0;}
void drawFlame() {
  // Realistic flame effect
  gfx->fillScreen(BLACK);
  
  
  flameTime += 0.15;
  
  int baseY = gfx->height() - 20;
  int centerX = gfx->width() / 2;
  
  // Flame base (candle)
  gfx->fillRect(centerX - 6, baseY, 12, 20, gfx->color565(139, 69, 19));
  
  // Flame particles rising
  for (int particle = 0; particle < 50; particle++) {
    float particleAge = fmod(flameTime * 2 + particle * 0.1, 3.0);
    float particleHeight = particleAge * 40;
    
    // Flame sway
    float sway = sin(flameTime * 4 + particle * 0.3) * 15;
    
    int x = centerX + sway + random(10) - 5;
    int y = baseY - particleHeight;
    
    if (y < 0 || particleAge > 2.5) continue;
    
    // Flame color transition: white -> yellow -> orange -> red
    float intensity = 1.0 - particleAge / 2.5;
    int r, g, b;
    
    if (particleAge < 0.5) {
      // Hot core - white to yellow
      float t = particleAge / 0.5;
      r = 255;
      g = 255;
      b = (1.0 - t) * 255;
    } else if (particleAge < 1.5) {
      // Yellow to orange
      float t = (particleAge - 0.5) / 1.0;
      r = 255;
      g = 255 - t * 100;
      b = 0;
    } else {
      // Orange to red
      float t = (particleAge - 1.5) / 1.0;
      r = 255;
      g = 155 - t * 155;
      b = 0;
    }
    
    // Apply intensity
    r *= intensity;
    g *= intensity;
    b *= intensity;
    
    // Particle size decreases with age
    int size = (1.0 - particleAge / 2.5) * 4 + 1;
    
    gfx->fillCircle(x, y, size, gfx->color565(r, g, b));
  }
  
  // Flame core
  for (int core = 0; core < 15; core++) {
    float coreHeight = core * 8;
    float coreWidth = (15 - core) * 2;
    
    int coreX = centerX + sin(flameTime * 5 + core * 0.5) * 5;
    int coreY = baseY - coreHeight;
    
    // Core color
    int coreIntensity = 255 - core * 15;
    gfx->fillCircle(coreX, coreY, coreWidth/2, gfx->color565(coreIntensity, coreIntensity, coreIntensity/3));
  }
  
  // Heat distortion effect
  for (int distort = 0; distort < 8; distort++) {
    int distortX = centerX - 40 + distort * 10;
    int distortY = baseY - 60 + sin(flameTime * 3 + distort) * 20;
    
    gfx->drawLine(distortX, distortY, distortX + 2, distortY + 4, gfx->color565(50, 50, 50));
  }
  
  gfx->setTextColor(gfx->color565(255, 200, 100));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FLAME");
}
}
namespace e99 {
float flipTime = 0;
bool flipState = false;
float lastFlip = 0;
void reset(){flipTime=0;flipState=false;lastFlip=0;}
void drawFlipflop() {
  // Digital flip-flop circuit visualization
  gfx->fillScreen(BLACK);
  
  
  flipTime += 0.08;
  
  
  
  
  // Flip the state periodically
  if (flipTime - lastFlip > 1.0) {
    flipState = !flipState;
    lastFlip = flipTime;
  }
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Main flip-flop box
  gfx->drawRect(centerX - 40, centerY - 25, 80, 50, gfx->color565(100, 100, 100));
  gfx->fillRect(centerX - 38, centerY - 23, 76, 46, gfx->color565(20, 20, 20));
  
  // FF label
  gfx->setTextColor(gfx->color565(200, 200, 200));
  gfx->setTextSize(2);
  gfx->setCursor(centerX - 12, centerY - 8);
  gfx->printf("FF");
  
  // Input pins
  gfx->drawLine(centerX - 60, centerY - 10, centerX - 40, centerY - 10, gfx->color565(150, 150, 150));
  gfx->drawLine(centerX - 60, centerY + 10, centerX - 40, centerY + 10, gfx->color565(150, 150, 150));
  
  // Clock input with pulse
  bool clockPulse = sin(flipTime * 6) > 0;
  uint16_t clockColor = clockPulse ? gfx->color565(0, 255, 0) : gfx->color565(100, 100, 100);
  gfx->drawLine(centerX - 60, centerY, centerX - 40, centerY, clockColor);
  gfx->fillTriangle(centerX - 45, centerY - 3, centerX - 45, centerY + 3, centerX - 40, centerY, clockColor);
  
  // Output pins
  uint16_t qColor = flipState ? gfx->color565(255, 0, 0) : gfx->color565(100, 100, 100);
  uint16_t qBarColor = !flipState ? gfx->color565(255, 0, 0) : gfx->color565(100, 100, 100);
  
  gfx->drawLine(centerX + 40, centerY - 10, centerX + 60, centerY - 10, qColor);
  gfx->drawLine(centerX + 40, centerY + 10, centerX + 60, centerY + 10, qBarColor);
  
  // Pin labels
  gfx->setTextSize(1);
  gfx->setTextColor(gfx->color565(200, 200, 200));
  
  gfx->setCursor(centerX - 75, centerY - 15);
  gfx->printf("D");
  gfx->setCursor(centerX - 75, centerY - 4);
  gfx->printf("CLK");
  gfx->setCursor(centerX - 75, centerY + 6);
  gfx->printf("R");
  
  gfx->setCursor(centerX + 65, centerY - 15);
  gfx->printf("Q");
  gfx->setCursor(centerX + 65, centerY + 6);
  gfx->printf("/Q");
  
  // Logic gates feeding the flip-flop
  for (int gate = 0; gate < 2; gate++) {
    int gateX = centerX - 120;
    int gateY = centerY - 30 + gate * 60;
    
    // NAND gate symbol
    gfx->drawRect(gateX, gateY, 30, 20, gfx->color565(150, 150, 150));
    gfx->drawCircle(gateX + 32, gateY + 10, 2, gfx->color565(150, 150, 150));
    
    gfx->setTextSize(1);
    gfx->setCursor(gateX + 8, gateY + 7);
    gfx->printf("&");
    
    // Gate inputs
    bool input1 = sin(flipTime * 4 + gate) > 0;
    bool input2 = sin(flipTime * 3 - gate) > 0.5;
    
    uint16_t in1Color = input1 ? gfx->color565(0, 255, 0) : gfx->color565(100, 100, 100);
    uint16_t in2Color = input2 ? gfx->color565(0, 255, 0) : gfx->color565(100, 100, 100);
    
    gfx->drawLine(gateX - 15, gateY + 5, gateX, gateY + 5, in1Color);
    gfx->drawLine(gateX - 15, gateY + 15, gateX, gateY + 15, in2Color);
    
    // Gate output
    bool gateOut = !(input1 && input2);
    uint16_t outColor = gateOut ? gfx->color565(255, 0, 0) : gfx->color565(100, 100, 100);
    gfx->drawLine(gateX + 34, gateY + 10, gateX + 50, gateY + 10, outColor);
  }
  
  // Truth table display
  gfx->setTextSize(1);
  gfx->setTextColor(gfx->color565(200, 200, 200));
  gfx->setCursor(10, gfx->height() - 40);
  gfx->printf("CLK D | Q /Q");
  gfx->setCursor(10, gfx->height() - 30);
  gfx->printf(" %c  %c | %c  %c", 
              clockPulse ? '1' : '0',
              '1',
              flipState ? '1' : '0', 
              flipState ? '0' : '1');
  
  gfx->setTextColor(gfx->color565(100, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FLIPFLOP");
}
}
namespace e100 {
float flipTime = 0;
void reset(){flipTime=0;}
void drawFlipscreen3d() {
  // 3D screen flip transition effect
  gfx->fillScreen(BLACK);
  
  
  flipTime += 0.1;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Screen flip angle
  float flipAngle = sin(flipTime) * PI;
  
  // Two screens - front and back
  for (int screen = 0; screen < 2; screen++) {
    float screenAngle = flipAngle + screen * PI;
    float cosAngle = cos(screenAngle);
    
    // Only draw if screen is facing forward
    if (cosAngle > 0) {
      // Screen dimensions affected by rotation
      int screenWidth = abs(cosAngle * 120);
      int screenHeight = 80;
      
      // Screen position
      int screenX = centerX - screenWidth / 2;
      int screenY = centerY - screenHeight / 2;
      
      // Screen colors
      uint16_t screenColor, contentColor;
      if (screen == 0) {
        screenColor = gfx->color565(100, 150, 255);
        contentColor = gfx->color565(255, 255, 255);
      } else {
        screenColor = gfx->color565(255, 150, 100);
        contentColor = gfx->color565(0, 0, 0);
      }
      
      // Draw screen frame
      gfx->fillRect(screenX - 5, screenY - 5, screenWidth + 10, screenHeight + 10, 
                   gfx->color565(60, 60, 60));
      gfx->fillRect(screenX, screenY, screenWidth, screenHeight, screenColor);
      
      // Screen content based on 3D depth
      float depth = cosAngle;
      
      // Content grid
      for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 8; col++) {
          if (screenWidth > 20 && screenHeight > 20) {
            int cellW = screenWidth / 8;
            int cellH = screenHeight / 6;
            int cellX = screenX + col * cellW + cellW / 4;
            int cellY = screenY + row * cellH + cellH / 4;
            
            // Content varies by screen
            if (screen == 0) {
              // Screen 1: Grid pattern
              if ((row + col) % 2 == 0) {
                gfx->fillRect(cellX, cellY, cellW/2, cellH/2, contentColor);
              }
            } else {
              // Screen 2: Dot pattern  
              gfx->fillCircle(cellX + cellW/4, cellY + cellH/4, 
                            max(1, (int)(cellW/4 * depth)), contentColor);
            }
          }
        }
      }
      
      // 3D perspective lines
      if (screenWidth > 10) {
        // Top perspective
        gfx->drawLine(screenX, screenY, screenX + screenWidth/4, screenY - 10, 
                     gfx->color565(80, 80, 80));
        gfx->drawLine(screenX + screenWidth, screenY, 
                     screenX + screenWidth - screenWidth/4, screenY - 10,
                     gfx->color565(80, 80, 80));
        
        // Side perspective
        gfx->drawLine(screenX + screenWidth, screenY, 
                     screenX + screenWidth + 8, screenY + screenHeight/4,
                     gfx->color565(40, 40, 40));
        gfx->drawLine(screenX + screenWidth, screenY + screenHeight,
                     screenX + screenWidth + 8, screenY + screenHeight - screenHeight/4,
                     gfx->color565(40, 40, 40));
      }
    }
  }
  
  // Flip axis indicator
  gfx->drawFastVLine(centerX, 20, gfx->height() - 40, gfx->color565(100, 100, 100));
  
  // Motion blur effect
  for (int blur = 0; blur < 3; blur++) {
    float blurAngle = flipAngle + blur * 0.2;
    if (abs(sin(blurAngle)) > 0.8) {
      int blurWidth = abs(cos(blurAngle) * 40);
      gfx->drawRect(centerX - blurWidth/2, centerY - 30, blurWidth, 60, 
                   gfx->color565(50, 50, 50));
    }
  }
  
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FLIPSCREEN3D");
}
}
namespace e101 {
float textTime = 0;
void reset(){textTime=0;}
void drawFliptext() {
  // Text flipping and morphing effect
  gfx->fillScreen(BLACK);
  
  
  textTime += 0.08;
  
  // Text strings to cycle through
  const char* texts[] = {"FLIP", "TEXT", "MORPH", "CYCLE", "MAGIC"};
  int numTexts = 5;
  
  int currentText = (int)(textTime * 0.3) % numTexts;
  int nextText = (currentText + 1) % numTexts;
  
  // Flip progress
  float flipProgress = fmod(textTime * 0.3, 1.0);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Text flip effect
  for (int phase = 0; phase < 2; phase++) {
    const char* displayText;
    float textPhase;
    
    if (phase == 0) {
      displayText = texts[currentText];
      textPhase = flipProgress;
    } else {
      displayText = texts[nextText];
      textPhase = flipProgress - 0.5;
    }
    
    // Only draw if in correct phase
    if ((phase == 0 && flipProgress < 0.5) || (phase == 1 && flipProgress >= 0.5)) {
      
      // Calculate text flip transformation
      float scaleY = abs(cos(textPhase * PI));
      float skew = sin(textPhase * PI) * 0.3;
      
      // Text color changes during flip
      int intensity = 150 + scaleY * 105;
      uint16_t textColor = gfx->color565(intensity, intensity, intensity);
      
      // Character-by-character flip
      int textLen = strlen(displayText);
      int totalWidth = textLen * 12; // Approximate character width
      int startX = centerX - totalWidth / 2;
      
      for (int i = 0; i < textLen; i++) {
        char c = displayText[i];
        int charX = startX + i * 12;
        int charY = centerY - 4;
        
        // Individual character flip timing
        float charDelay = i * 0.1;
        float charFlip = textPhase - charDelay;
        
        if (charFlip > 0) {
          float charScale = abs(cos(charFlip * PI * 2));
          int adjustedY = charY + (1.0 - charScale) * 10;
          
          // Character flip colors
          int r = intensity;
          int g = intensity - abs(sin(charFlip * PI)) * 100;
          int b = intensity + abs(sin(charFlip * PI)) * 50;
          
          gfx->setTextColor(gfx->color565(r, g, b));
          gfx->setTextSize(2);
          gfx->setCursor(charX, adjustedY);
          gfx->printf("%c", c);
          
          // Character shadow/reflection
          if (charScale > 0.3) {
            gfx->setTextColor(gfx->color565(r/4, g/4, b/4));
            gfx->setCursor(charX + 1, adjustedY + 20);
            gfx->printf("%c", c);
          }
        }
      }
    }
  }
  
  // Flip transition particles
  if (flipProgress > 0.4 && flipProgress < 0.6) {
    for (int particle = 0; particle < 10; particle++) {
      int pX = centerX + random(80) - 40;
      int pY = centerY + random(40) - 20;
      
      gfx->fillCircle(pX, pY, 1, gfx->color565(255, 255, 0));
    }
  }
  
  // Text flip indicator
  gfx->drawRect(10, gfx->height() - 25, gfx->width() - 20, 20, gfx->color565(100, 100, 100));
  int progressWidth = flipProgress * (gfx->width() - 24);
  gfx->fillRect(12, gfx->height() - 23, progressWidth, 16, gfx->color565(0, 255, 100));
  
  gfx->setTextColor(gfx->color565(255, 200, 100));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FLIPTEXT");
}
}
namespace e102 {
float flowTime = 0;
int particleX[50];
int particleY[50];
float particleAge[50];
bool initialized = false;
void reset(){flowTime=0;std::memset(particleX,0,sizeof(particleX));std::memset(particleY,0,sizeof(particleY));std::memset(particleAge,0,sizeof(particleAge));initialized=false;}
void drawFlow() {
  // Fluid flow field visualization
  gfx->fillScreen(BLACK);
  
  
  flowTime += 0.06;
  
  // Flow field parameters
  float fieldScale = 0.02;
  
  // Flow particles
  
  
  
  
  
  if (!initialized) {
    for (int i = 0; i < 50; i++) {
      particleX[i] = random(gfx->width());
      particleY[i] = random(gfx->height());
      particleAge[i] = random(100) / 100.0;
    }
    initialized = true;
  }
  
  // Update and draw particles
  for (int i = 0; i < 50; i++) {
    // Calculate flow field at particle position
    float fieldX = particleX[i] * fieldScale;
    float fieldY = particleY[i] * fieldScale;
    
    // Flow field equations (Perlin-like noise)
    float flowAngle = sin(fieldX + flowTime) * cos(fieldY + flowTime) * PI;
    float flowStrength = (sin(fieldX * 2 + flowTime * 0.5) + 1) * 0.5;
    
    // Move particle
    float vx = cos(flowAngle) * flowStrength * 2;
    float vy = sin(flowAngle) * flowStrength * 2;
    
    particleX[i] += vx;
    particleY[i] += vy;
    particleAge[i] += 0.01;
    
    // Wrap or reset particles
    if (particleX[i] < 0 || particleX[i] >= gfx->width() || 
        particleY[i] < 0 || particleY[i] >= gfx->height() || 
        particleAge[i] > 2.0) {
      particleX[i] = random(gfx->width());
      particleY[i] = random(gfx->height());
      particleAge[i] = 0;
    }
    
    // Draw particle with trail
    float alpha = 1.0 - particleAge[i] / 2.0;
    if (alpha > 0) {
      int r = alpha * 255;
      int g = alpha * 150;
      int b = alpha * 100;
      
      gfx->fillCircle(particleX[i], particleY[i], 2, gfx->color565(r, g, b));
      
      // Particle trail
      int trailX = particleX[i] - vx * 3;
      int trailY = particleY[i] - vy * 3;
      if (trailX >= 0 && trailX < gfx->width() && trailY >= 0 && trailY < gfx->height()) {
        gfx->fillCircle(trailX, trailY, 1, gfx->color565(r/2, g/2, b/2));
      }
    }
  }
  
  // Flow field visualization (grid)
  for (int x = 0; x < gfx->width(); x += 20) {
    for (int y = 0; y < gfx->height(); y += 20) {
      float fieldX = x * fieldScale;
      float fieldY = y * fieldScale;
      
      float flowAngle = sin(fieldX + flowTime) * cos(fieldY + flowTime) * PI;
      float flowStrength = (sin(fieldX * 2 + flowTime * 0.5) + 1) * 0.5;
      
      if (flowStrength > 0.3) {
        int endX = x + cos(flowAngle) * 10;
        int endY = y + sin(flowAngle) * 10;
        
        int intensity = flowStrength * 150;
        gfx->drawLine(x, y, endX, endY, gfx->color565(0, intensity, intensity));
        
        // Arrow head
        gfx->fillCircle(endX, endY, 1, gfx->color565(0, intensity + 50, intensity + 50));
      }
    }
  }
  
  // Flow source indicators
  for (int source = 0; source < 3; source++) {
    float sourceAngle = source * 2 * PI / 3 + flowTime * 0.2;
    int sourceX = gfx->width() / 2 + cos(sourceAngle) * 60;
    int sourceY = gfx->height() / 2 + sin(sourceAngle) * 40;
    
    int sourceSize = 4 + sin(flowTime * 3 + source) * 2;
    gfx->fillCircle(sourceX, sourceY, sourceSize, gfx->color565(255, 255, 255));
    gfx->drawCircle(sourceX, sourceY, sourceSize + 3, gfx->color565(100, 200, 255));
  }
  
  gfx->setTextColor(gfx->color565(100, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FLOW");
}
}
namespace e103 {
float fluidTime = 0;
float ballX[8] = {40, 80, 120, 160, 200, 240, 60, 140};
const float initial_ballX[8] = {40, 80, 120, 160, 200, 240, 60, 140};
float ballY[8] = {60, 100, 40, 80, 120, 60, 140, 180};
const float initial_ballY[8] = {60, 100, 40, 80, 120, 60, 140, 180};
float ballVX[8] = {1.5, -1.2, 0.8, -2.0, 1.3, -0.9, 1.7, -1.6};
const float initial_ballVX[8] = {1.5, -1.2, 0.8, -2.0, 1.3, -0.9, 1.7, -1.6};
float ballVY[8] = {-1.8, 1.1, -1.5, 0.7, -1.0, 1.9, -1.3, 0.9};
const float initial_ballVY[8] = {-1.8, 1.1, -1.5, 0.7, -1.0, 1.9, -1.3, 0.9};
float ballRadius[8] = {8, 12, 6, 10, 14, 7, 9, 11};
const float initial_ballRadius[8] = {8, 12, 6, 10, 14, 7, 9, 11};
void reset(){fluidTime=0;std::memcpy(ballX,initial_ballX,sizeof(ballX));std::memcpy(ballY,initial_ballY,sizeof(ballY));std::memcpy(ballVX,initial_ballVX,sizeof(ballVX));std::memcpy(ballVY,initial_ballVY,sizeof(ballVY));std::memcpy(ballRadius,initial_ballRadius,sizeof(ballRadius));}
void drawFluidballs() {
  // Fluid dynamics with bouncing balls
  gfx->fillScreen(BLACK);
  
  
  fluidTime += 0.1;
  
  // Fluid ball parameters
  
  
  
  
  
  
  // Update ball physics
  for (int i = 0; i < 8; i++) {
    // Apply gravity
    ballVY[i] += 0.1;
    
    // Update position
    ballX[i] += ballVX[i];
    ballY[i] += ballVY[i];
    
    // Bounce off walls
    if (ballX[i] <= ballRadius[i] || ballX[i] >= gfx->width() - ballRadius[i]) {
      ballVX[i] *= -0.8; // Energy loss
      ballX[i] = constrain(ballX[i], ballRadius[i], gfx->width() - ballRadius[i]);
    }
    
    if (ballY[i] <= ballRadius[i] || ballY[i] >= gfx->height() - ballRadius[i]) {
      ballVY[i] *= -0.8;
      ballY[i] = constrain(ballY[i], ballRadius[i], gfx->height() - ballRadius[i]);
    }
    
    // Ball-to-ball interactions
    for (int j = i + 1; j < 8; j++) {
      float dx = ballX[j] - ballX[i];
      float dy = ballY[j] - ballY[i];
      float dist = sqrt(dx*dx + dy*dy);
      float minDist = ballRadius[i] + ballRadius[j];
      
      if (dist < minDist && dist > 0) {
        // Collision response
        float overlap = minDist - dist;
        float nx = dx / dist;
        float ny = dy / dist;
        
        // Separate balls
        ballX[i] -= nx * overlap * 0.5;
        ballY[i] -= ny * overlap * 0.5;
        ballX[j] += nx * overlap * 0.5;
        ballY[j] += ny * overlap * 0.5;
        
        // Exchange velocities (simplified)
        float tempVX = ballVX[i];
        float tempVY = ballVY[i];
        ballVX[i] = ballVX[j] * 0.9;
        ballVY[i] = ballVY[j] * 0.9;
        ballVX[j] = tempVX * 0.9;
        ballVY[j] = tempVY * 0.9;
      }
    }
  }
  
  // Draw fluid connections
  for (int i = 0; i < 8; i++) {
    for (int j = i + 1; j < 8; j++) {
      float dx = ballX[j] - ballX[i];
      float dy = ballY[j] - ballY[i];
      float dist = sqrt(dx*dx + dy*dy);
      
      if (dist < 80) {
        float connection = 1.0 - dist / 80.0;
        int alpha = connection * 150;
        gfx->drawLine(ballX[i], ballY[i], ballX[j], ballY[j], 
                     gfx->color565(0, alpha, alpha));
      }
    }
  }
  
  // Draw balls with fluid effect
  for (int i = 0; i < 8; i++) {
    // Ball color based on velocity
    float speed = sqrt(ballVX[i]*ballVX[i] + ballVY[i]*ballVY[i]);
    int r = constrain(speed * 50, 50, 255);
    int g = constrain(255 - speed * 30, 100, 255);
    int b = 200;
    
    // Main ball
    gfx->fillCircle(ballX[i], ballY[i], ballRadius[i], gfx->color565(r, g, b));
    
    // Highlight
    gfx->fillCircle(ballX[i] - 2, ballY[i] - 2, ballRadius[i]/3, 
                   gfx->color565(255, 255, 255));
    
    // Motion trail
    int trailX = ballX[i] - ballVX[i] * 5;
    int trailY = ballY[i] - ballVY[i] * 5;
    gfx->fillCircle(trailX, trailY, ballRadius[i]/2, gfx->color565(r/3, g/3, b/3));
  }
  
  gfx->setTextColor(gfx->color565(100, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FLUIDBALLS");
}
}
namespace e104 {
float flurryTime = 0;
float snowX[60];
float snowY[60];
float snowVX[60];
float snowVY[60];
float snowSize[60];
float snowSpin[60];
bool initialized = false;
void reset(){flurryTime=0;std::memset(snowX,0,sizeof(snowX));std::memset(snowY,0,sizeof(snowY));std::memset(snowVX,0,sizeof(snowVX));std::memset(snowVY,0,sizeof(snowVY));std::memset(snowSize,0,sizeof(snowSize));std::memset(snowSpin,0,sizeof(snowSpin));initialized=false;}
void drawFlurry() {
  // Snow flurry effect
  gfx->fillScreen(BLACK);
  
  
  flurryTime += 0.08;
  
  // Wind effect
  float windX = sin(flurryTime * 0.5) * 2;
  float windY = cos(flurryTime * 0.3) * 0.5;
  
  // Snowflakes
  
  
  
  
  
  
  
  
  if (!initialized) {
    for (int i = 0; i < 60; i++) {
      snowX[i] = random(gfx->width());
      snowY[i] = random(gfx->height());
      snowVX[i] = (random(200) - 100) / 100.0;
      snowVY[i] = random(100) / 50.0 + 0.5;
      snowSize[i] = random(30) / 10.0 + 1;
      snowSpin[i] = random(628) / 100.0;
    }
    initialized = true;
  }
  
  // Update and draw snowflakes
  for (int i = 0; i < 60; i++) {
    // Update position with wind
    snowVX[i] += (windX - snowVX[i]) * 0.02;
    snowX[i] += snowVX[i];
    snowY[i] += snowVY[i];
    snowSpin[i] += 0.1;
    
    // Wrap around screen
    if (snowX[i] < -10) snowX[i] = gfx->width() + 10;
    if (snowX[i] > gfx->width() + 10) snowX[i] = -10;
    if (snowY[i] > gfx->height() + 10) {
      snowY[i] = -10;
      snowX[i] = random(gfx->width());
    }
    
    // Draw snowflake
    int x = snowX[i];
    int y = snowY[i];
    float size = snowSize[i];
    
    // Snowflake brightness based on size and position
    float brightness = size / 4.0;
    int alpha = brightness * 255;
    uint16_t snowColor = gfx->color565(alpha, alpha, alpha);
    
    // Draw snowflake pattern
    if (size > 2) {
      // Complex snowflake
      for (int arm = 0; arm < 6; arm++) {
        float armAngle = arm * PI / 3 + snowSpin[i];
        int armLength = size * 2;
        
        int endX = x + cos(armAngle) * armLength;
        int endY = y + sin(armAngle) * armLength;
        
        gfx->drawLine(x, y, endX, endY, snowColor);
        
        // Branch details
        int branchX = x + cos(armAngle) * armLength * 0.6;
        int branchY = y + sin(armAngle) * armLength * 0.6;
        int branch1X = branchX + cos(armAngle + 0.5) * size;
        int branch1Y = branchY + sin(armAngle + 0.5) * size;
        int branch2X = branchX + cos(armAngle - 0.5) * size;
        int branch2Y = branchY + sin(armAngle - 0.5) * size;
        
        gfx->drawLine(branchX, branchY, branch1X, branch1Y, snowColor);
        gfx->drawLine(branchX, branchY, branch2X, branch2Y, snowColor);
      }
    } else {
      // Simple snowflake
      gfx->fillCircle(x, y, size, snowColor);
    }
  }
  
  // Ground accumulation
  for (int x = 0; x < gfx->width(); x += 4) {
    float accumulation = sin(x * 0.1 + flurryTime * 0.2) * 3 + 5;
    gfx->fillRect(x, gfx->height() - accumulation, 4, accumulation, 
                 gfx->color565(200, 200, 255));
  }
  
  // Wind indicator
  int windIndicatorX = gfx->width() - 30;
  int windIndicatorY = 20;
  gfx->drawLine(windIndicatorX, windIndicatorY, 
               windIndicatorX + windX * 10, windIndicatorY + windY * 10,
               gfx->color565(100, 100, 255));
  
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FLURRY");
}
}
namespace e105 {
float smokeTime = 0;
float smokeX[40];
float smokeY[40];
float smokeVX[40];
float smokeVY[40];
float smokeAge[40];
float smokeSize[40];
bool smokeInit = false;
float flurryX[30];
float flurryY[30];
bool flurryInit = false;
void reset(){smokeTime=0;std::memset(smokeX,0,sizeof(smokeX));std::memset(smokeY,0,sizeof(smokeY));std::memset(smokeVX,0,sizeof(smokeVX));std::memset(smokeVY,0,sizeof(smokeVY));std::memset(smokeAge,0,sizeof(smokeAge));std::memset(smokeSize,0,sizeof(smokeSize));smokeInit=false;std::memset(flurryX,0,sizeof(flurryX));std::memset(flurryY,0,sizeof(flurryY));flurryInit=false;}
void drawFlurrySmoke() {
  // Flurry combined with smoke effect
  gfx->fillScreen(BLACK);
  
  
  smokeTime += 0.12;
  
  // Smoke sources
  int smokeSourceX = gfx->width() / 2;
  int smokeSourceY = gfx->height() - 10;
  
  // Smoke particles
  
  
  
  
  
  
  
  
  if (!smokeInit) {
    for (int i = 0; i < 40; i++) {
      smokeX[i] = smokeSourceX + random(20) - 10;
      smokeY[i] = smokeSourceY;
      smokeVX[i] = (random(100) - 50) / 50.0;
      smokeVY[i] = -random(100) / 50.0 - 1;
      smokeAge[i] = random(100) / 100.0 * 3;
      smokeSize[i] = random(50) / 10.0 + 2;
    }
    smokeInit = true;
  }
  
  // Update smoke
  for (int i = 0; i < 40; i++) {
    smokeAge[i] += 0.02;
    
    // Smoke physics
    smokeVY[i] -= 0.01; // Rise
    smokeVX[i] += sin(smokeTime + i) * 0.02; // Turbulence
    
    smokeX[i] += smokeVX[i];
    smokeY[i] += smokeVY[i];
    smokeSize[i] += 0.1; // Expand
    
    // Reset old smoke
    if (smokeAge[i] > 3 || smokeY[i] < -20) {
      smokeX[i] = smokeSourceX + random(20) - 10;
      smokeY[i] = smokeSourceY;
      smokeVX[i] = (random(100) - 50) / 50.0;
      smokeVY[i] = -random(100) / 50.0 - 1;
      smokeAge[i] = 0;
      smokeSize[i] = random(50) / 10.0 + 2;
    }
    
    // Draw smoke
    float alpha = 1.0 - smokeAge[i] / 3.0;
    if (alpha > 0) {
      int smokeAlpha = alpha * 100;
      gfx->fillCircle(smokeX[i], smokeY[i], smokeSize[i], 
                     gfx->color565(smokeAlpha, smokeAlpha, smokeAlpha));
    }
  }
  
  // Snow flurry over smoke
  
  
  
  
  if (!flurryInit) {
    for (int i = 0; i < 30; i++) {
      flurryX[i] = random(gfx->width());
      flurryY[i] = random(gfx->height());
    }
    flurryInit = true;
  }
  
  // Update and draw snow
  for (int i = 0; i < 30; i++) {
    flurryY[i] += 1 + sin(smokeTime + i) * 0.5;
    flurryX[i] += sin(smokeTime * 2 + i) * 0.3;
    
    if (flurryY[i] > gfx->height()) {
      flurryY[i] = -5;
      flurryX[i] = random(gfx->width());
    }
    
    // Snow interacting with smoke heat
    float distToSmoke = sqrt(pow(flurryX[i] - smokeSourceX, 2) + 
                           pow(flurryY[i] - smokeSourceY, 2));
    
    if (distToSmoke < 50) {
      // Melt effect near smoke
      float melt = 1.0 - distToSmoke / 50.0;
      int meltAlpha = (1.0 - melt) * 255;
      gfx->fillCircle(flurryX[i], flurryY[i], 2, 
                     gfx->color565(meltAlpha, meltAlpha, 255));
    } else {
      // Normal snow
      gfx->fillCircle(flurryX[i], flurryY[i], 2, gfx->color565(255, 255, 255));
      
      // Snowflake arms
      gfx->drawLine(flurryX[i]-2, flurryY[i], flurryX[i]+2, flurryY[i], 
                   gfx->color565(200, 200, 200));
      gfx->drawLine(flurryX[i], flurryY[i]-2, flurryX[i], flurryY[i]+2, 
                   gfx->color565(200, 200, 200));
    }
  }
  
  // Smoke source (fire/chimney)
  gfx->fillRect(smokeSourceX - 8, smokeSourceY - 5, 16, 10, 
               gfx->color565(139, 69, 19));
  int flameSize = 3 + sin(smokeTime * 6) * 2;
  gfx->fillCircle(smokeSourceX, smokeSourceY - 8, flameSize, 
                 gfx->color565(255, 100, 0));
  
  gfx->setTextColor(gfx->color565(255, 200, 150));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FLURRY-SMOKE");
}
}
// Stable slide 107: removed by screensaver policy.
namespace e107 {
float starTime = 0;
float starX[30];
float starY[30];
float starVY[30];
float starSpin[30];
float starSize[30];
bool starInit = false;
float shootingStarX = 0;
float shootingStarY = 0;
float shootingTimer = 0;
void reset(){starTime=0;std::memset(starX,0,sizeof(starX));std::memset(starY,0,sizeof(starY));std::memset(starVY,0,sizeof(starVY));std::memset(starSpin,0,sizeof(starSpin));std::memset(starSize,0,sizeof(starSize));starInit=false;shootingStarX=0;shootingStarY=0;shootingTimer=0;}
void drawFlurryStar() {
  // Snow flurry with star patterns
  gfx->fillScreen(BLACK);
  
  
  starTime += 0.1;
  
  // Star-shaped snowflakes
  
  
  
  
  
  
  
  if (!starInit) {
    for (int i = 0; i < 30; i++) {
      starX[i] = random(gfx->width());
      starY[i] = random(gfx->height());
      starVY[i] = random(100) / 100.0 + 0.3;
      starSpin[i] = random(628) / 100.0;
      starSize[i] = random(40) / 10.0 + 2;
    }
    starInit = true;
  }
  
  // Update and draw star snowflakes
  for (int i = 0; i < 30; i++) {
    starY[i] += starVY[i];
    starSpin[i] += 0.05;
    
    if (starY[i] > gfx->height() + 10) {
      starY[i] = -10;
      starX[i] = random(gfx->width());
    }
    
    // Draw star snowflake
    int x = starX[i];
    int y = starY[i];
    float size = starSize[i];
    
    // Star brightness
    float twinkle = sin(starTime * 4 + i) * 0.3 + 0.7;
    int brightness = twinkle * 255;
    
    // Draw 5-pointed star
    for (int point = 0; point < 5; point++) {
      float outerAngle = point * 2 * PI / 5 + starSpin[i];
      float innerAngle = outerAngle + PI / 5;
      
      // Outer point
      int outerX = x + cos(outerAngle) * size;
      int outerY = y + sin(outerAngle) * size;
      
      // Inner point
      int innerX = x + cos(innerAngle) * size * 0.4;
      int innerY = y + sin(innerAngle) * size * 0.4;
      
      // Next outer point
      float nextOuterAngle = (point + 1) * 2 * PI / 5 + starSpin[i];
      int nextOuterX = x + cos(nextOuterAngle) * size;
      int nextOuterY = y + sin(nextOuterAngle) * size;
      
      // Draw star segments
      gfx->drawLine(x, y, outerX, outerY, gfx->color565(brightness, brightness, 255));
      gfx->drawLine(outerX, outerY, innerX, innerY, gfx->color565(brightness, brightness, 255));
      gfx->drawLine(innerX, innerY, nextOuterX, nextOuterY, gfx->color565(brightness, brightness, 255));
    }
    
    // Central star core
    gfx->fillCircle(x, y, size / 4, gfx->color565(255, 255, brightness));
  }
  
  // Background constellation stars
  for (int bg = 0; bg < 50; bg++) {
    float bgAngle = bg * 0.5 + starTime * 0.1;
    int bgX = 20 + bg * 6;
    int bgY = 30 + sin(bgAngle) * 20;
    
    if (bgX < gfx->width() && bgY > 0 && bgY < gfx->height()) {
      float bgTwinkle = sin(starTime * 3 + bg) * 0.5 + 0.5;
      int bgBright = bgTwinkle * 150 + 50;
      
      gfx->fillCircle(bgX, bgY, 1, gfx->color565(bgBright, bgBright, bgBright));
      
      // Occasional bright twinkle
      if (bgTwinkle > 0.8) {
        gfx->drawLine(bgX - 2, bgY, bgX + 2, bgY, gfx->color565(200, 200, 255));
        gfx->drawLine(bgX, bgY - 2, bgX, bgY + 2, gfx->color565(200, 200, 255));
      }
    }
  }
  
  // Shooting stars
  
  
  
  
  shootingTimer += 0.1;
  if (shootingTimer > 5) {
    shootingStarX = gfx->width() + 20;
    shootingStarY = random(gfx->height() / 2);
    shootingTimer = 0;
  }
  
  if (shootingStarX > -20) {
    shootingStarX -= 4;
    shootingStarY += 1;
    
    // Draw shooting star
    gfx->fillCircle(shootingStarX, shootingStarY, 2, gfx->color565(255, 255, 255));
    
    // Tail
    for (int tail = 1; tail < 8; tail++) {
      int tailX = shootingStarX + tail * 4;
      int tailY = shootingStarY - tail;
      float tailAlpha = 1.0 - tail / 8.0;
      int tailBright = tailAlpha * 200;
      
      gfx->fillCircle(tailX, tailY, 1, gfx->color565(tailBright, tailBright, 255));
    }
  }
  
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FLURRY-STAR");
}
}
namespace e108 {
float textureTime = 0;
float texSnowX[50];
float texSnowY[50];
float texSnowVX[50];
float texSnowVY[50];
int texSnowType[50];
float texSnowSpin[50];
bool texInit = false;
void reset(){textureTime=0;std::memset(texSnowX,0,sizeof(texSnowX));std::memset(texSnowY,0,sizeof(texSnowY));std::memset(texSnowVX,0,sizeof(texSnowVX));std::memset(texSnowVY,0,sizeof(texSnowVY));std::memset(texSnowType,0,sizeof(texSnowType));std::memset(texSnowSpin,0,sizeof(texSnowSpin));texInit=false;}
void drawFlurryTexture() {
  // Textured snow flurry effect
  gfx->fillScreen(BLACK);
  
  
  textureTime += 0.06;
  
  // Textured snow particles
  
  
  
  
  
  
  
  
  if (!texInit) {
    for (int i = 0; i < 50; i++) {
      texSnowX[i] = random(gfx->width());
      texSnowY[i] = random(gfx->height());
      texSnowVX[i] = (random(100) - 50) / 200.0;
      texSnowVY[i] = random(100) / 100.0 + 0.5;
      texSnowType[i] = random(4); // Different texture types
      texSnowSpin[i] = random(628) / 100.0;
    }
    texInit = true;
  }
  
  // Update textured snow
  for (int i = 0; i < 50; i++) {
    texSnowX[i] += texSnowVX[i] + sin(textureTime + i) * 0.3;
    texSnowY[i] += texSnowVY[i];
    texSnowSpin[i] += 0.08;
    
    if (texSnowY[i] > gfx->height() + 5) {
      texSnowY[i] = -5;
      texSnowX[i] = random(gfx->width());
    }
    
    int x = texSnowX[i];
    int y = texSnowY[i];
    
    // Draw different textured snowflakes
    switch (texSnowType[i]) {
      case 0: // Crystalline
        {
          uint16_t crystalColor = gfx->color565(200, 220, 255);
          // Hexagonal crystal
          for (int side = 0; side < 6; side++) {
            float angle = side * PI / 3 + texSnowSpin[i];
            int x1 = x + cos(angle) * 4;
            int y1 = y + sin(angle) * 4;
            int x2 = x + cos(angle + PI/3) * 4;
            int y2 = y + sin(angle + PI/3) * 4;
            gfx->drawLine(x1, y1, x2, y2, crystalColor);
          }
          gfx->fillCircle(x, y, 1, gfx->color565(255, 255, 255));
        }
        break;
        
      case 1: // Feathery
        {
          uint16_t featherColor = gfx->color565(180, 200, 255);
          // Feather-like branches
          for (int branch = 0; branch < 4; branch++) {
            float branchAngle = branch * PI / 2 + texSnowSpin[i];
            int branchLen = 5;
            
            for (int seg = 0; seg < branchLen; seg++) {
              int segX = x + cos(branchAngle) * seg;
              int segY = y + sin(branchAngle) * seg;
              
              // Side feathers
              int featherX1 = segX + cos(branchAngle + PI/4) * 2;
              int featherY1 = segY + sin(branchAngle + PI/4) * 2;
              int featherX2 = segX + cos(branchAngle - PI/4) * 2;
              int featherY2 = segY + sin(branchAngle - PI/4) * 2;
              
              gfx->drawLine(segX, segY, featherX1, featherY1, featherColor);
              gfx->drawLine(segX, segY, featherX2, featherY2, featherColor);
            }
          }
        }
        break;
        
      case 2: // Dendrite
        {
          uint16_t dendriteColor = gfx->color565(220, 230, 255);
          // Tree-like dendrite pattern
          float mainAngle = texSnowSpin[i];
          
          for (int main = 0; main < 3; main++) {
            float angle = mainAngle + main * 2 * PI / 3;
            int mainX = x + cos(angle) * 6;
            int mainY = y + sin(angle) * 6;
            
            gfx->drawLine(x, y, mainX, mainY, dendriteColor);
            
            // Secondary branches
            for (int sec = 1; sec < 4; sec++) {
              int secBaseX = x + cos(angle) * sec * 2;
              int secBaseY = y + sin(angle) * sec * 2;
              
              int sec1X = secBaseX + cos(angle + PI/3) * 2;
              int sec1Y = secBaseY + sin(angle + PI/3) * 2;
              int sec2X = secBaseX + cos(angle - PI/3) * 2;
              int sec2Y = secBaseY + sin(angle - PI/3) * 2;
              
              gfx->drawLine(secBaseX, secBaseY, sec1X, sec1Y, dendriteColor);
              gfx->drawLine(secBaseX, secBaseY, sec2X, sec2Y, dendriteColor);
            }
          }
        }
        break;
        
      case 3: // Plate crystal
        {
          uint16_t plateColor = gfx->color565(240, 240, 255);
          // Flat plate with internal structure
          
          // Outer hexagon
          for (int side = 0; side < 6; side++) {
            float angle1 = side * PI / 3 + texSnowSpin[i];
            float angle2 = (side + 1) * PI / 3 + texSnowSpin[i];
            
            int x1 = x + cos(angle1) * 5;
            int y1 = y + sin(angle1) * 5;
            int x2 = x + cos(angle2) * 5;
            int y2 = y + sin(angle2) * 5;
            
            gfx->drawLine(x1, y1, x2, y2, plateColor);
            
            // Internal lines
            gfx->drawLine(x, y, x1, y1, plateColor);
          }
          
          // Inner pattern
          gfx->drawCircle(x, y, 2, plateColor);
        }
        break;
    }
  }
  
  // Texture overlay effect
  for (int tx = 0; tx < gfx->width(); tx += 8) {
    for (int ty = 0; ty < gfx->height(); ty += 8) {
      float texNoise = sin(tx * 0.1 + textureTime) * cos(ty * 0.1 + textureTime);
      if (texNoise > 0.3) {
        int texAlpha = (texNoise - 0.3) * 100;
        gfx->fillRect(tx, ty, 2, 2, gfx->color565(texAlpha, texAlpha, texAlpha + 50));
      }
    }
  }
  
  gfx->setTextColor(gfx->color565(220, 230, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FLURRY-TEXTURE");
}
}
namespace e109 {
float toasterTime = 0;
float toasterX[6] = {320, 380, 440, 500, 360, 420};
const float initial_toasterX[6] = {320, 380, 440, 500, 360, 420};
float toasterY[6] = {40, 80, 120, 160, 200, 60};
const float initial_toasterY[6] = {40, 80, 120, 160, 200, 60};
float toasterSpeed[6] = {1.2, 0.8, 1.5, 1.0, 0.9, 1.3};
const float initial_toasterSpeed[6] = {1.2, 0.8, 1.5, 1.0, 0.9, 1.3};
float toasterBob[6] = {0, 1, 2, 3, 4, 5};
const float initial_toasterBob[6] = {0, 1, 2, 3, 4, 5};
void reset(){toasterTime=0;std::memcpy(toasterX,initial_toasterX,sizeof(toasterX));std::memcpy(toasterY,initial_toasterY,sizeof(toasterY));std::memcpy(toasterSpeed,initial_toasterSpeed,sizeof(toasterSpeed));std::memcpy(toasterBob,initial_toasterBob,sizeof(toasterBob));}
void drawFlyingtoasters() {
  // Classic flying toasters screensaver
  gfx->fillScreen(BLACK);
  
  
  toasterTime += 0.08;
  
  // Flying toasters
  
  
  
  
  
  for (int i = 0; i < 6; i++) {
    // Move toaster
    toasterX[i] -= toasterSpeed[i];
    
    // Reset when off screen
    if (toasterX[i] < -40) {
      toasterX[i] = gfx->width() + 40;
      toasterY[i] = random(gfx->height() - 60) + 20;
    }
    
    // Bobbing motion
    float bobOffset = sin(toasterTime * 2 + toasterBob[i]) * 8;
    int drawY = toasterY[i] + bobOffset;
    
    // Draw toaster body
    gfx->fillRect(toasterX[i], drawY, 28, 18, gfx->color565(180, 180, 180));
    gfx->drawRect(toasterX[i], drawY, 28, 18, gfx->color565(100, 100, 100));
    
    // Toaster slots
    gfx->fillRect(toasterX[i] + 3, drawY + 2, 6, 14, gfx->color565(50, 50, 50));
    gfx->fillRect(toasterX[i] + 11, drawY + 2, 6, 14, gfx->color565(50, 50, 50));
    gfx->fillRect(toasterX[i] + 19, drawY + 2, 6, 14, gfx->color565(50, 50, 50));
    
    // Toaster knobs
    gfx->fillCircle(toasterX[i] + 26, drawY + 5, 2, gfx->color565(120, 120, 120));
    gfx->fillCircle(toasterX[i] + 26, drawY + 13, 2, gfx->color565(120, 120, 120));
    
    // Wings
    float wingFlap = sin(toasterTime * 8 + i) * 0.5 + 0.5;
    int wingSpread = 8 + wingFlap * 6;
    
    // Left wing
    gfx->drawLine(toasterX[i] - 2, drawY + 4, toasterX[i] - wingSpread, drawY - 2, 
                 gfx->color565(255, 255, 255));
    gfx->drawLine(toasterX[i] - 2, drawY + 8, toasterX[i] - wingSpread, drawY + 2, 
                 gfx->color565(255, 255, 255));
    gfx->drawLine(toasterX[i] - wingSpread, drawY - 2, toasterX[i] - wingSpread, drawY + 2, 
                 gfx->color565(255, 255, 255));
    
    // Right wing  
    gfx->drawLine(toasterX[i] + 30, drawY + 4, toasterX[i] + 30 + wingSpread, drawY - 2, 
                 gfx->color565(255, 255, 255));
    gfx->drawLine(toasterX[i] + 30, drawY + 8, toasterX[i] + 30 + wingSpread, drawY + 2, 
                 gfx->color565(255, 255, 255));
    gfx->drawLine(toasterX[i] + 30 + wingSpread, drawY - 2, toasterX[i] + 30 + wingSpread, drawY + 2, 
                 gfx->color565(255, 255, 255));
    
    // Toast pieces flying out
    if (sin(toasterTime * 4 + i) > 0.7) {
      int toastX = toasterX[i] + 14;
      int toastY = drawY - 8;
      
      // Flying toast
      gfx->fillRect(toastX - 3, toastY, 6, 4, gfx->color565(200, 150, 100));
      gfx->drawRect(toastX - 3, toastY, 6, 4, gfx->color565(139, 69, 19));
    }
  }
  
  // Background clouds
  for (int cloud = 0; cloud < 8; cloud++) {
    float cloudX = cloud * 50 + sin(toasterTime * 0.3 + cloud) * 20;
    float cloudY = 30 + cloud * 20 + cos(toasterTime * 0.2 + cloud) * 10;
    
    if (cloudX > -30 && cloudX < gfx->width() + 30) {
      // Cloud puffs
      gfx->fillCircle(cloudX, cloudY, 8, gfx->color565(80, 80, 100));
      gfx->fillCircle(cloudX + 6, cloudY, 6, gfx->color565(80, 80, 100));
      gfx->fillCircle(cloudX - 6, cloudY, 6, gfx->color565(80, 80, 100));
      gfx->fillCircle(cloudX, cloudY - 4, 5, gfx->color565(80, 80, 100));
    }
  }
  
  gfx->setTextColor(gfx->color565(255, 255, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FLYINGTOASTERS");
}
}
namespace e110 {
float glideTime = 0;
float particleX[20];
float particleY[20];
float particleVX[20];
float particleVY[20];
bool particleInit = false;
void reset(){glideTime=0;std::memset(particleX,0,sizeof(particleX));std::memset(particleY,0,sizeof(particleY));std::memset(particleVX,0,sizeof(particleVX));std::memset(particleVY,0,sizeof(particleVY));particleInit=false;}
void drawFontglide() {
  // Font gliding and morphing effect
  gfx->fillScreen(BLACK);
  
  
  glideTime += 0.06;
  
  // Text to display
  const char* glideText = "GLIDE";
  int textLen = strlen(glideText);
  
  // Character gliding
  for (int i = 0; i < textLen; i++) {
    char c = glideText[i];
    
    // Character position with wave motion
    float charPhase = glideTime + i * 0.8;
    int baseX = 40 + i * 35;
    int baseY = gfx->height() / 2;
    
    // Gliding motion - sine wave path
    int glideX = baseX + sin(charPhase * 0.8) * 60;
    int glideY = baseY + cos(charPhase * 0.6) * 40;
    
    // Character size variation
    int charSize = 2 + sin(charPhase * 1.2) * 1;
    
    // Character color cycling
    float colorPhase = charPhase * 2;
    int r = 128 + sin(colorPhase) * 127;
    int g = 128 + sin(colorPhase + PI/3) * 127;
    int b = 128 + sin(colorPhase + 2*PI/3) * 127;
    
    // Draw character with trail effect
    for (int trail = 0; trail < 5; trail++) {
      float trailPhase = charPhase - trail * 0.1;
      int trailX = baseX + sin(trailPhase * 0.8) * 60;
      int trailY = baseY + cos(trailPhase * 0.6) * 40;
      
      float trailAlpha = 1.0 - trail / 5.0;
      int trailR = r * trailAlpha;
      int trailG = g * trailAlpha;
      int trailB = b * trailAlpha;
      
      gfx->setTextColor(gfx->color565(trailR, trailG, trailB));
      gfx->setTextSize(charSize);
      gfx->setCursor(trailX - 6, trailY - 8);
      gfx->printf("%c", c);
    }
    
    // Main character
    gfx->setTextColor(gfx->color565(r, g, b));
    gfx->setTextSize(charSize);
    gfx->setCursor(glideX - 6, glideY - 8);
    gfx->printf("%c", c);
    
    // Character glow effect
    gfx->setTextColor(gfx->color565(255, 255, 255));
    gfx->setTextSize(1);
    gfx->setCursor(glideX - 3, glideY - 4);
    gfx->printf("%c", c);
  }
  
  // Background particle system
  
  
  
  
  
  
  if (!particleInit) {
    for (int i = 0; i < 20; i++) {
      particleX[i] = random(gfx->width());
      particleY[i] = random(gfx->height());
      particleVX[i] = (random(100) - 50) / 100.0;
      particleVY[i] = (random(100) - 50) / 100.0;
    }
    particleInit = true;
  }
  
  // Update and draw particles
  for (int i = 0; i < 20; i++) {
    particleX[i] += particleVX[i];
    particleY[i] += particleVY[i];
    
    // Wrap particles
    if (particleX[i] < 0) particleX[i] = gfx->width();
    if (particleX[i] > gfx->width()) particleX[i] = 0;
    if (particleY[i] < 0) particleY[i] = gfx->height();
    if (particleY[i] > gfx->height()) particleY[i] = 0;
    
    // Particle attraction to text
    float attraction = sin(glideTime * 3 + i) * 0.5 + 0.5;
    int particleBright = attraction * 150 + 50;
    
    gfx->fillCircle(particleX[i], particleY[i], 1, gfx->color565(particleBright, particleBright, 255));
  }
  
  // Font style indicator
  gfx->setTextColor(gfx->color565(200, 200, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Style: %d", (int)(glideTime) % 4 + 1);
  
  gfx->setTextColor(gfx->color565(100, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FONTGLIDE");
}
}
// Stable slide 112: removed by screensaver policy.
// Stable slide 113: removed by screensaver policy.
namespace e113 {
float fpsTime = 0;
int frameCount = 0;
float lastFpsUpdate = 0;
float currentFps = 60;
float fpsHistory[60];
int historyIndex = 0;
bool fpsInit = false;
void reset(){fpsTime=0;frameCount=0;lastFpsUpdate=0;currentFps=60;std::memset(fpsHistory,0,sizeof(fpsHistory));historyIndex=0;fpsInit=false;}
void drawFps() {
  // FPS counter and performance monitor
  gfx->fillScreen(BLACK);
  
  
  
  
  
  
  
  
  
  fpsTime += 0.016; // Simulate 60fps timing
  frameCount++;
  
  if (!fpsInit) {
    for (int i = 0; i < 60; i++) {
      fpsHistory[i] = 60;
    }
    fpsInit = true;
  }
  
  // Update FPS calculation
  if (fpsTime - lastFpsUpdate >= 1.0) {
    currentFps = frameCount / (fpsTime - lastFpsUpdate);
    fpsHistory[historyIndex] = currentFps;
    historyIndex = (historyIndex + 1) % 60;
    frameCount = 0;
    lastFpsUpdate = fpsTime;
  }
  
  // Simulate varying performance
  float performanceVariation = sin(fpsTime * 0.5) * 20 + sin(fpsTime * 2) * 10;
  currentFps = 60 + performanceVariation;
  currentFps = constrain(currentFps, 10, 120);
  
  // Main FPS display
  gfx->setTextColor(gfx->color565(255, 255, 255));
  gfx->setTextSize(3);
  gfx->setCursor(20, 40);
  gfx->printf("FPS");
  
  // Current FPS number
  uint16_t fpsColor;
  if (currentFps >= 50) fpsColor = gfx->color565(0, 255, 0);      // Green
  else if (currentFps >= 30) fpsColor = gfx->color565(255, 255, 0); // Yellow
  else fpsColor = gfx->color565(255, 0, 0);                        // Red
  
  gfx->setTextColor(fpsColor);
  gfx->setTextSize(4);
  gfx->setCursor(20, 80);
  gfx->printf("%d", (int)currentFps);
  
  // FPS history graph
  int graphX = 150;
  int graphY = 50;
  int graphWidth = gfx->width() - graphX - 10;
  int graphHeight = 60;
  
  // Graph background
  gfx->drawRect(graphX, graphY, graphWidth, graphHeight, gfx->color565(100, 100, 100));
  
  // Draw FPS history line
  for (int i = 0; i < graphWidth - 1; i++) {
    int histIdx1 = (historyIndex + i * 60 / graphWidth) % 60;
    int histIdx2 = (historyIndex + (i + 1) * 60 / graphWidth) % 60;
    
    int y1 = graphY + graphHeight - (fpsHistory[histIdx1] / 120.0) * graphHeight;
    int y2 = graphY + graphHeight - (fpsHistory[histIdx2] / 120.0) * graphHeight;
    
    uint16_t lineColor;
    if (fpsHistory[histIdx1] >= 50) lineColor = gfx->color565(0, 255, 0);
    else if (fpsHistory[histIdx1] >= 30) lineColor = gfx->color565(255, 255, 0);
    else lineColor = gfx->color565(255, 0, 0);
    
    gfx->drawLine(graphX + i, y1, graphX + i + 1, y2, lineColor);
  }
  
  // Graph labels
  gfx->setTextColor(gfx->color565(200, 200, 200));
  gfx->setTextSize(1);
  gfx->setCursor(graphX, graphY - 10);
  gfx->printf("120");
  gfx->setCursor(graphX, graphY + graphHeight / 2 - 4);
  gfx->printf("60");
  gfx->setCursor(graphX, graphY + graphHeight - 8);
  gfx->printf("0");
  
  // Performance metrics
  gfx->setTextColor(gfx->color565(150, 150, 255));
  gfx->setTextSize(1);
  gfx->setCursor(20, 140);
  gfx->printf("Frame Time: %.1fms", 1000.0 / currentFps);
  
  gfx->setCursor(20, 155);
  float avgFps = 0;
  for (int i = 0; i < 60; i++) avgFps += fpsHistory[i];
  avgFps /= 60;
  gfx->printf("Avg FPS: %.1f", avgFps);
  
  gfx->setCursor(20, 170);
  gfx->printf("Min FPS: %.0f", *std::min_element(fpsHistory, fpsHistory + 60));
  
  gfx->setCursor(20, 185);
  gfx->printf("Max FPS: %.0f", *std::max_element(fpsHistory, fpsHistory + 60));
  
  // Performance indicators
  for (int i = 0; i < 10; i++) {
    float intensity = (sin(fpsTime * 8 + i) + 1) / 2;
    int barHeight = intensity * 20;
    uint16_t barColor = gfx->color565(intensity * 255, (1 - intensity) * 255, 0);
    
    gfx->fillRect(20 + i * 12, 210 - barHeight, 8, barHeight, barColor);
  }
  
  gfx->setTextColor(gfx->color565(100, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FPS");
}
}
namespace e114 {
float glTime = 0;
float gpuLoad = 0;
float cpuLoad = 0;
float memoryUsage = 0;
float temperature = 0;
void reset(){glTime=0;gpuLoad=0;cpuLoad=0;memoryUsage=0;temperature=0;}
void drawFpsGl() {
  // OpenGL-style FPS monitor with 3D elements
  gfx->fillScreen(BLACK);
  
  
  glTime += 0.02;
  
  // Simulated 3D performance data
  
  
  
  
  
  // Update performance metrics
  gpuLoad = 50 + sin(glTime * 1.5) * 30 + sin(glTime * 4) * 10;
  cpuLoad = 40 + cos(glTime * 1.2) * 25 + cos(glTime * 6) * 8;
  memoryUsage = 60 + sin(glTime * 0.8) * 20;
  temperature = 65 + cos(glTime * 0.5) * 15;
  
  gpuLoad = constrain(gpuLoad, 0, 100);
  cpuLoad = constrain(cpuLoad, 0, 100);
  memoryUsage = constrain(memoryUsage, 0, 100);
  temperature = constrain(temperature, 40, 90);
  
  // 3D-style frame counter
  gfx->setTextColor(gfx->color565(0, 255, 255));
  gfx->setTextSize(2);
  gfx->setCursor(20, 20);
  gfx->printf("OpenGL FPS");
  
  // Rotating 3D cube indicator
  int cubeX = 200;
  int cubeY = 50;
  float rotation = glTime * 2;
  
  // Cube vertices (simplified 2D projection)
  for (int face = 0; face < 3; face++) {
    float faceRotation = rotation + face * PI / 3;
    int size = 15 + sin(glTime * 3 + face) * 5;
    
    // Draw cube face
    int x1 = cubeX + cos(faceRotation) * size;
    int y1 = cubeY + sin(faceRotation) * size;
    int x2 = cubeX + cos(faceRotation + PI/2) * size;
    int y2 = cubeY + sin(faceRotation + PI/2) * size;
    int x3 = cubeX + cos(faceRotation + PI) * size;
    int y3 = cubeY + sin(faceRotation + PI) * size;
    int x4 = cubeX + cos(faceRotation + 3*PI/2) * size;
    int y4 = cubeY + sin(faceRotation + 3*PI/2) * size;
    
    uint16_t faceColor = gfx->color565(100 + face * 50, 100, 255 - face * 50);
    gfx->drawLine(x1, y1, x2, y2, faceColor);
    gfx->drawLine(x2, y2, x3, y3, faceColor);
    gfx->drawLine(x3, y3, x4, y4, faceColor);
    gfx->drawLine(x4, y4, x1, y1, faceColor);
  }
  
  // GPU Load bar
  int barY = 80;
  gfx->setTextColor(gfx->color565(255, 100, 100));
  gfx->setTextSize(1);
  gfx->setCursor(20, barY);
  gfx->printf("GPU: %d%%", (int)gpuLoad);
  
  int barWidth = (gpuLoad / 100.0) * 120;
  gfx->fillRect(80, barY, barWidth, 8, gfx->color565(255, 100, 100));
  gfx->drawRect(80, barY, 120, 8, gfx->color565(150, 150, 150));
  
  // CPU Load bar
  barY += 20;
  gfx->setTextColor(gfx->color565(100, 255, 100));
  gfx->setCursor(20, barY);
  gfx->printf("CPU: %d%%", (int)cpuLoad);
  
  barWidth = (cpuLoad / 100.0) * 120;
  gfx->fillRect(80, barY, barWidth, 8, gfx->color565(100, 255, 100));
  gfx->drawRect(80, barY, 120, 8, gfx->color565(150, 150, 150));
  
  // Memory usage bar
  barY += 20;
  gfx->setTextColor(gfx->color565(100, 100, 255));
  gfx->setCursor(20, barY);
  gfx->printf("MEM: %d%%", (int)memoryUsage);
  
  barWidth = (memoryUsage / 100.0) * 120;
  gfx->fillRect(80, barY, barWidth, 8, gfx->color565(100, 100, 255));
  gfx->drawRect(80, barY, 120, 8, gfx->color565(150, 150, 150));
  
  // Temperature gauge
  barY += 20;
  gfx->setTextColor(gfx->color565(255, 255, 100));
  gfx->setCursor(20, barY);
  gfx->printf("TEMP: %d°C", (int)temperature);
  
  float tempRatio = (temperature - 40) / 50.0;
  barWidth = tempRatio * 120;
  uint16_t tempColor = gfx->color565(255 * tempRatio, 255 * (1 - tempRatio), 0);
  gfx->fillRect(80, barY, barWidth, 8, tempColor);
  gfx->drawRect(80, barY, 120, 8, gfx->color565(150, 150, 150));
  
  // Render pipeline visualization
  gfx->setTextColor(gfx->color565(200, 200, 200));
  gfx->setTextSize(1);
  gfx->setCursor(20, 170);
  gfx->printf("Render Pipeline:");
  
  // Pipeline stages
  const char* stages[] = {"VERT", "GEOM", "RAST", "FRAG", "BLEND"};
  for (int i = 0; i < 5; i++) {
    int stageX = 20 + i * 50;
    int stageY = 185;
    
    // Stage activity
    float activity = sin(glTime * 6 + i * 1.2) * 0.5 + 0.5;
    uint16_t stageColor = gfx->color565(activity * 255, activity * 100, 255);
    
    gfx->fillRect(stageX, stageY, 40, 12, stageColor);
    gfx->drawRect(stageX, stageY, 40, 12, gfx->color565(200, 200, 200));
    
    gfx->setTextColor(BLACK);
    gfx->setCursor(stageX + 5, stageY + 3);
    gfx->printf("%s", stages[i]);
    
    // Connection lines
    if (i < 4) {
      gfx->drawLine(stageX + 40, stageY + 6, stageX + 50, stageY + 6, 
                   gfx->color565(150, 150, 150));
    }
  }
  
  gfx->setTextColor(gfx->color565(100, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FPS-GL");
}
}
namespace e115 {
float fuzzyTime = 0;
float flakeX[40];
float flakeY[40];
float flakeVY[40];
float flakeSize[40];
float flakeFuzz[40];
float flakeSpin[40];
bool fuzzyInit = false;
void reset(){fuzzyTime=0;std::memset(flakeX,0,sizeof(flakeX));std::memset(flakeY,0,sizeof(flakeY));std::memset(flakeVY,0,sizeof(flakeVY));std::memset(flakeSize,0,sizeof(flakeSize));std::memset(flakeFuzz,0,sizeof(flakeFuzz));std::memset(flakeSpin,0,sizeof(flakeSpin));fuzzyInit=false;}
void drawFuzzyflakes() {
  // Fuzzy snowflake effect with soft edges
  gfx->fillScreen(BLACK);
  
  
  fuzzyTime += 0.04;
  
  // Fuzzy snowflakes
  
  
  
  
  
  
  
  
  if (!fuzzyInit) {
    for (int i = 0; i < 40; i++) {
      flakeX[i] = random(gfx->width());
      flakeY[i] = random(gfx->height());
      flakeVY[i] = random(50) / 50.0 + 0.3;
      flakeSize[i] = random(60) / 10.0 + 2;
      flakeFuzz[i] = random(30) / 10.0 + 1;
      flakeSpin[i] = random(628) / 100.0;
    }
    fuzzyInit = true;
  }
  
  // Update and draw fuzzy flakes
  for (int i = 0; i < 40; i++) {
    flakeY[i] += flakeVY[i];
    flakeSpin[i] += 0.03;
    
    if (flakeY[i] > gfx->height() + 10) {
      flakeY[i] = -10;
      flakeX[i] = random(gfx->width());
    }
    
    int x = flakeX[i];
    int y = flakeY[i];
    float size = flakeSize[i];
    float fuzz = flakeFuzz[i];
    
    // Fuzzy glow layers
    for (int layer = 0; layer < 5; layer++) {
      float layerSize = size + layer * fuzz;
      float layerAlpha = 1.0 - layer / 5.0;
      int alpha = layerAlpha * 150;
      
      uint16_t layerColor = gfx->color565(alpha, alpha, alpha + 50);
      
      // Draw fuzzy circle
      for (int angle = 0; angle < 360; angle += 30) {
        float rad = angle * PI / 180 + flakeSpin[i];
        int fx = x + cos(rad) * layerSize;
        int fy = y + sin(rad) * layerSize;
        
        gfx->fillCircle(fx, fy, 1 + layer, layerColor);
      }
    }
    
    // Core snowflake structure
    uint16_t coreColor = gfx->color565(255, 255, 255);
    for (int arm = 0; arm < 6; arm++) {
      float armAngle = arm * PI / 3 + flakeSpin[i];
      
      // Main arm
      int armLength = size;
      int armEndX = x + cos(armAngle) * armLength;
      int armEndY = y + sin(armAngle) * armLength;
      
      // Fuzzy line drawing
      for (int seg = 0; seg <= armLength; seg++) {
        float segT = seg / (float)armLength;
        int segX = x + cos(armAngle) * seg;
        int segY = y + sin(armAngle) * seg;
        
        // Add fuzziness
        int fuzzX = segX + sin(fuzzyTime * 4 + i + arm) * fuzz;
        int fuzzY = segY + cos(fuzzyTime * 3 + i + arm) * fuzz;
        
        gfx->fillCircle(fuzzX, fuzzY, 1, coreColor);
        
        // Branch details
        if (seg > armLength / 3) {
          int branchX1 = segX + cos(armAngle + PI/4) * (armLength - seg) / 3;
          int branchY1 = segY + sin(armAngle + PI/4) * (armLength - seg) / 3;
          int branchX2 = segX + cos(armAngle - PI/4) * (armLength - seg) / 3;
          int branchY2 = segY + sin(armAngle - PI/4) * (armLength - seg) / 3;
          
          gfx->fillCircle(branchX1, branchY1, 1, coreColor);
          gfx->fillCircle(branchX2, branchY2, 1, coreColor);
        }
      }
    }
  }
  
  // Fuzzy wind effect
  for (int wind = 0; wind < gfx->width(); wind += 20) {
    float windStrength = sin(fuzzyTime * 2 + wind * 0.1) * 0.5 + 0.5;
    int windY = 50 + cos(fuzzyTime + wind * 0.05) * 30;
    
    if (windStrength > 0.3) {
      for (int particle = 0; particle < 5; particle++) {
        int px = wind + particle * 4;
        int py = windY + particle * 2;
        int alpha = windStrength * 100;
        
        gfx->fillCircle(px, py, 1, gfx->color565(alpha, alpha, alpha + 100));
      }
    }
  }
  
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("FUZZYFLAKES");
}
}
namespace e116 {
float galaxyTime = 0;
void reset(){galaxyTime=0;}
void drawGalaxy() {
  // Spiral galaxy visualization
  gfx->fillScreen(BLACK);
  
  
  galaxyTime += 0.02;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Galaxy spiral arms
  for (int arm = 0; arm < 4; arm++) {
    float armAngle = arm * PI / 2 + galaxyTime * 0.3;
    
    // Draw spiral arm
    for (int r = 10; r < 120; r += 2) {
      float spiralAngle = armAngle + r * 0.05;
      
      // Spiral position
      int spiralX = centerX + cos(spiralAngle) * r;
      int spiralY = centerY + sin(spiralAngle) * r * 0.7; // Elliptical
      
      if (spiralX >= 0 && spiralX < gfx->width() && spiralY >= 0 && spiralY < gfx->height()) {
        // Star density variation
        float density = sin(r * 0.1 + galaxyTime) * 0.5 + 0.5;
        
        if (random(100) < density * 30) {
          // Star color based on distance from center
          float distFromCenter = sqrt((spiralX - centerX) * (spiralX - centerX) + 
                                    (spiralY - centerY) * (spiralY - centerY));
          
          // Younger stars (blue) in outer regions, older (red) in center
          float colorRatio = distFromCenter / 120.0;
          int r_color = 100 + (1 - colorRatio) * 155;
          int g_color = 100 + colorRatio * 100;
          int b_color = 150 + colorRatio * 105;
          
          gfx->fillCircle(spiralX, spiralY, 1, gfx->color565(r_color, g_color, b_color));
          
          // Bright stars
          if (random(100) < 5) {
            gfx->fillCircle(spiralX, spiralY, 2, gfx->color565(255, 255, 255));
            // Star glow
            gfx->drawCircle(spiralX, spiralY, 3, gfx->color565(200, 200, 255));
          }
        }
      }
    }
  }
  
  // Galactic core
  for (int core = 0; core < 20; core++) {
    float coreAngle = core * PI / 10 + galaxyTime * 2;
    float coreRadius = 8 + sin(galaxyTime * 4 + core) * 4;
    
    int coreX = centerX + cos(coreAngle) * coreRadius;
    int coreY = centerY + sin(coreAngle) * coreRadius;
    
    float brightness = sin(galaxyTime * 3 + core) * 0.5 + 0.5;
    int bright = brightness * 255;
    
    gfx->fillCircle(coreX, coreY, 2, gfx->color565(255, bright, 100));
  }
  
  // Central black hole
  gfx->fillCircle(centerX, centerY, 4, BLACK);
  gfx->drawCircle(centerX, centerY, 6, gfx->color565(100, 50, 0));
  gfx->drawCircle(centerX, centerY, 8, gfx->color565(150, 100, 50));
  
  // Dust lanes
  for (int dust = 0; dust < 3; dust++) {
    float dustAngle = dust * 2 * PI / 3 + galaxyTime * 0.1;
    
    for (int d = 20; d < 80; d += 3) {
      int dustX = centerX + cos(dustAngle + d * 0.03) * d;
      int dustY = centerY + sin(dustAngle + d * 0.03) * d * 0.5;
      
      if (dustX >= 0 && dustX < gfx->width() && dustY >= 0 && dustY < gfx->height()) {
        float dustDensity = sin(d * 0.2 + galaxyTime * 2) * 0.3 + 0.3;
        int dustAlpha = dustDensity * 100;
        
        gfx->fillCircle(dustX, dustY, 1, gfx->color565(dustAlpha, dustAlpha / 2, 0));
      }
    }
  }
  
  // Background stars
  for (int bg = 0; bg < 50; bg++) {
    int bgX = (bg * 37) % gfx->width();
    int bgY = (bg * 73) % gfx->height();
    
    float twinkle = sin(galaxyTime * 5 + bg) * 0.5 + 0.5;
    int twinkleBright = twinkle * 150 + 50;
    
    gfx->fillCircle(bgX, bgY, 1, gfx->color565(twinkleBright, twinkleBright, twinkleBright));
  }
  
  // Galaxy info
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, gfx->height() - 30);
  gfx->printf("Type: Spiral");
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Arms: 4");
  
  gfx->setTextColor(gfx->color565(100, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("GALAXY");
}
}
namespace e117 {
float fontTime = 0;
void reset(){fontTime=0;}
void drawGallant12x22Bsd() {
  // BSD-style font rendering demonstration
  gfx->fillScreen(BLACK);
  
  
  fontTime += 0.05;
  
  // Font size demonstration
  const char* sampleText = "BSD FONT";
  const char* charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  
  // Title
  gfx->setTextColor(gfx->color565(255, 200, 100));
  gfx->setTextSize(2);
  gfx->setCursor(20, 20);
  gfx->printf("Gallant 12x22");
  
  // Scrolling text demo
  int scrollX = gfx->width() - (int)(fontTime * 60) % (gfx->width() + 150);
  gfx->setTextColor(gfx->color565(100, 255, 100));
  gfx->setTextSize(1);
  gfx->setCursor(scrollX, 50);
  gfx->printf("BSD Font Rendering System");
  
  // Character showcase
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(20, 80);
  gfx->printf("Character Set:");
  
  // Display charset in grid
  for (int i = 0; i < strlen(charset); i++) {
    int col = i % 13;
    int row = i / 13;
    
    int charX = 20 + col * 20;
    int charY = 100 + row * 25;
    
    // Character cycling color
    float colorPhase = fontTime * 2 + i * 0.1;
    int r = 128 + sin(colorPhase) * 127;
    int g = 128 + sin(colorPhase + PI/3) * 127;
    int b = 128 + sin(colorPhase + 2*PI/3) * 127;
    
    gfx->setTextColor(gfx->color565(r, g, b));
    gfx->setTextSize(1);
    gfx->setCursor(charX, charY);
    gfx->printf("%c", charset[i]);
    
    // Character highlight
    if (((int)(fontTime * 5)) % strlen(charset) == i) {
      gfx->drawRect(charX - 2, charY - 2, 16, 12, gfx->color565(255, 255, 255));
    }
  }
  
  // Font metrics display
  gfx->setTextColor(gfx->color565(255, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(20, 170);
  gfx->printf("Font: Fixed-width");
  gfx->setCursor(20, 185);
  gfx->printf("Size: 12x22 pixels");
  gfx->setCursor(20, 200);
  gfx->printf("License: BSD");
  
  // Animated text effects
  for (int effect = 0; effect < 8; effect++) {
    float effectPhase = fontTime * 3 + effect;
    int effectX = 200 + effect * 15;
    int effectY = 170 + sin(effectPhase) * 10;
    
    char effectChar = sampleText[effect % strlen(sampleText)];
    
    // Text shadow
    gfx->setTextColor(gfx->color565(50, 50, 50));
    gfx->setTextSize(2);
    gfx->setCursor(effectX + 2, effectY + 2);
    gfx->printf("%c", effectChar);
    
    // Main character
    float brightness = sin(effectPhase * 2) * 0.5 + 0.5;
    int bright = brightness * 255;
    gfx->setTextColor(gfx->color565(255, bright, 100));
    gfx->setTextSize(2);
    gfx->setCursor(effectX, effectY);
    gfx->printf("%c", effectChar);
  }
  
  // Font rendering stats
  gfx->setTextColor(gfx->color565(150, 150, 150));
  gfx->setTextSize(1);
  gfx->setCursor(200, 100);
  gfx->printf("Rendering Stats:");
  gfx->setCursor(200, 115);
  gfx->printf("Chars/sec: %d", (int)(fontTime * 10) % 1000 + 500);
  gfx->setCursor(200, 130);
  gfx->printf("Memory: %dKB", (int)(sin(fontTime) * 50) + 100);
  gfx->setCursor(200, 145);
  gfx->printf("Quality: High");
  
  // Copyright notice
  gfx->setTextColor(gfx->color565(100, 100, 100));
  gfx->setTextSize(1);
  gfx->setCursor(10, gfx->height() - 10);
  gfx->printf("(c) BSD Licensed Font System");
  
  gfx->setTextColor(gfx->color565(100, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("GALLANT12X22-BSD");
}
}
namespace e118 {
float gearTime = 0;
void reset(){gearTime=0;}
void drawGears() {
  // Mechanical gears animation - simplified version
  gfx->fillScreen(BLACK);
  
  
  gearTime += 0.05;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw multiple rotating gear circles
  for (int gear = 0; gear < 8; gear++) {
    float angle = gear * PI / 4 + gearTime * (0.5 + gear * 0.1);
    float distance = 60 + sin(gearTime + gear) * 20;
    
    int gearX = centerX + cos(angle) * distance;
    int gearY = centerY + sin(angle) * distance;
    
    int gearRadius = 15 + gear * 2;
    float gearRotation = gearTime * (1 + gear * 0.2);
    
    // Gear color based on position
    int r = 100 + gear * 20;
    int g = 150 + sin(gearTime + gear) * 50;
    int b = 200 - gear * 15;
    uint16_t gearColor = gfx->color565(r, g, b);
    
    // Draw gear body
    gfx->fillCircle(gearX, gearY, gearRadius, gearColor);
    gfx->drawCircle(gearX, gearY, gearRadius, gfx->color565(200, 200, 200));
    
    // Draw gear teeth as small rectangles
    int numTeeth = 8 + gear;
    for (int tooth = 0; tooth < numTeeth; tooth++) {
      float toothAngle = tooth * 2 * PI / numTeeth + gearRotation;
      int toothX = gearX + cos(toothAngle) * gearRadius;
      int toothY = gearY + sin(toothAngle) * gearRadius;
      
      gfx->fillRect(toothX - 1, toothY - 1, 3, 3, gfx->color565(255, 255, 255));
    }
    
    // Center hub
    gfx->fillCircle(gearX, gearY, 5, gfx->color565(80, 80, 80));
  }
  
  // Central master gear
  gfx->fillCircle(centerX, centerY, 20, gfx->color565(255, 200, 100));
  gfx->drawCircle(centerX, centerY, 20, gfx->color565(200, 200, 200));
  
  // Master gear teeth
  for (int tooth = 0; tooth < 12; tooth++) {
    float toothAngle = tooth * PI / 6 + gearTime;
    int toothX = centerX + cos(toothAngle) * 20;
    int toothY = centerY + sin(toothAngle) * 20;
    
    gfx->fillRect(toothX - 2, toothY - 2, 4, 4, gfx->color565(255, 255, 255));
  }
  
  gfx->fillCircle(centerX, centerY, 8, gfx->color565(100, 100, 100));
  
  // Connection lines
  for (int connection = 0; connection < 8; connection++) {
    float connAngle = connection * PI / 4 + gearTime * 0.1;
    int connX = centerX + cos(connAngle) * 40;
    int connY = centerY + sin(connAngle) * 40;
    
    gfx->drawLine(centerX, centerY, connX, connY, gfx->color565(100, 100, 150));
  }
  
  // Stats
  gfx->setTextColor(gfx->color565(255, 255, 100));
  gfx->setTextSize(1);
  gfx->setCursor(10, gfx->height() - 30);
  gfx->printf("RPM: %.0f", fabs(sin(gearTime * 3)) * 100 + 50);
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Torque: %.1fNm", cos(gearTime * 2) * 25 + 30);
  
  gfx->setTextColor(gfx->color565(100, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("GEARS");
}
}
namespace e119 {
float geodesicTime = 0;
void reset(){geodesicTime=0;}
void drawGeodesic() {
  // Geodesic dome wireframe
  gfx->fillScreen(BLACK);
  
  
  geodesicTime += 0.03;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  int radius = 80;
  
  // Icosahedron vertices (simplified geodesic)
  float phi = (1.0 + sqrt(5.0)) / 2.0; // Golden ratio
  
  // 12 vertices of icosahedron
  float vertices[12][3] = {
    {-1,  phi,  0}, { 1,  phi,  0}, {-1, -phi,  0}, { 1, -phi,  0},
    { 0, -1,  phi}, { 0,  1,  phi}, { 0, -1, -phi}, { 0,  1, -phi},
    { phi,  0, -1}, { phi,  0,  1}, {-phi,  0, -1}, {-phi,  0,  1}
  };
  
  // Normalize vertices
  for (int i = 0; i < 12; i++) {
    float len = sqrt(vertices[i][0]*vertices[i][0] + 
                    vertices[i][1]*vertices[i][1] + 
                    vertices[i][2]*vertices[i][2]);
    vertices[i][0] /= len;
    vertices[i][1] /= len;
    vertices[i][2] /= len;
  }
  
  // Rotation matrices
  float rotY = geodesicTime * 0.5;
  float rotX = geodesicTime * 0.3;
  
  // Project and draw vertices
  int projectedX[12], projectedY[12];
  float projectedZ[12];
  
  for (int i = 0; i < 12; i++) {
    // Apply rotation
    float x = vertices[i][0];
    float y = vertices[i][1];
    float z = vertices[i][2];
    
    // Rotate around Y axis
    float newX = x * cos(rotY) + z * sin(rotY);
    float newZ = -x * sin(rotY) + z * cos(rotY);
    x = newX;
    z = newZ;
    
    // Rotate around X axis
    float newY = y * cos(rotX) - z * sin(rotX);
    newZ = y * sin(rotX) + z * cos(rotX);
    y = newY;
    z = newZ;
    
    // Project to 2D
    float perspective = 200.0 / (200.0 + z * 100);
    projectedX[i] = centerX + x * radius * perspective;
    projectedY[i] = centerY + y * radius * perspective;
    projectedZ[i] = z;
  }
  
  // Draw edges (simplified icosahedron connections)
  int edges[][2] = {
    {0,1}, {0,5}, {0,7}, {0,10}, {0,11},
    {1,5}, {1,7}, {1,8}, {1,9},
    {2,3}, {2,4}, {2,6}, {2,10}, {2,11},
    {3,4}, {3,6}, {3,8}, {3,9},
    {4,5}, {4,9}, {4,11},
    {5,9}, {5,11},
    {6,7}, {6,8}, {6,10},
    {7,8}, {7,10},
    {8,9}
  };
  
  for (int i = 0; i < 29; i++) {
    int v1 = edges[i][0];
    int v2 = edges[i][1];
    
    // Color based on depth
    float avgZ = (projectedZ[v1] + projectedZ[v2]) / 2;
    int brightness = 100 + (avgZ + 1) * 77;
    brightness = constrain(brightness, 50, 255);
    
    uint16_t edgeColor = gfx->color565(brightness, brightness/2, brightness);
    
    gfx->drawLine(projectedX[v1], projectedY[v1], 
                 projectedX[v2], projectedY[v2], edgeColor);
  }
  
  // Draw vertices
  for (int i = 0; i < 12; i++) {
    float brightness = (projectedZ[i] + 1) * 127 + 50;
    uint16_t vertexColor = gfx->color565(255, brightness, brightness);
    
    gfx->fillCircle(projectedX[i], projectedY[i], 3, vertexColor);
    gfx->drawCircle(projectedX[i], projectedY[i], 4, gfx->color565(255, 255, 255));
  }
  
  // Geodesic info
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->setTextSize(1);
  gfx->setCursor(10, gfx->height() - 40);
  gfx->printf("Vertices: 12");
  gfx->setCursor(10, gfx->height() - 30);
  gfx->printf("Edges: 30");
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Faces: 20");
  
  gfx->setTextColor(gfx->color565(100, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("GEODESIC");
}
}
namespace e120 {
float hybridTime = 0;
void reset(){hybridTime=0;}
void drawGeodesicgears() {
  // Combination of geodesic structure with rotating gears
  gfx->fillScreen(BLACK);
  
  
  hybridTime += 0.04;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Geodesic framework (simplified)
  float radius = 60;
  int numNodes = 8;
  
  // Draw geodesic nodes with gears
  for (int node = 0; node < numNodes; node++) {
    float angle = node * 2 * PI / numNodes + hybridTime * 0.2;
    float nodeRadius = radius + sin(hybridTime * 2 + node) * 15;
    
    int nodeX = centerX + cos(angle) * nodeRadius;
    int nodeY = centerY + sin(angle) * nodeRadius * 0.7;
    
    // Gear at each node
    float gearRotation = hybridTime * (1 + node * 0.1);
    int gearSize = 12 + sin(hybridTime + node) * 4;
    uint16_t gearColor = gfx->color565(100 + node * 20, 150, 200 - node * 15);
    
    // Simple node gear
    gfx->fillCircle(nodeX, nodeY, gearSize, gearColor);
    gfx->drawCircle(nodeX, nodeY, gearSize, gfx->color565(200, 200, 200));
    
    // Node gear teeth
    int nodeTeeth = 6 + node % 4;
    for (int tooth = 0; tooth < nodeTeeth; tooth++) {
      float toothAngle = tooth * 2 * PI / nodeTeeth + gearRotation;
      int toothX = nodeX + cos(toothAngle) * gearSize;
      int toothY = nodeY + sin(toothAngle) * gearSize;
      gfx->fillCircle(toothX, toothY, 1, gfx->color565(255, 255, 255));
    }
    gfx->fillCircle(nodeX, nodeY, 4, gfx->color565(80, 80, 80));
    
    // Connect to center
    uint16_t connectionColor = gfx->color565(80, 80, 120);
    gfx->drawLine(centerX, centerY, nodeX, nodeY, connectionColor);
    
    // Connect to adjacent nodes
    if (node < numNodes - 1) {
      float nextAngle = (node + 1) * 2 * PI / numNodes + hybridTime * 0.2;
      float nextNodeRadius = radius + sin(hybridTime * 2 + node + 1) * 15;
      int nextX = centerX + cos(nextAngle) * nextNodeRadius;
      int nextY = centerY + sin(nextAngle) * nextNodeRadius * 0.7;
      
      gfx->drawLine(nodeX, nodeY, nextX, nextY, connectionColor);
    } else {
      // Connect last to first
      float firstAngle = hybridTime * 0.2;
      float firstNodeRadius = radius + sin(hybridTime * 2) * 15;
      int firstX = centerX + cos(firstAngle) * firstNodeRadius;
      int firstY = centerY + sin(firstAngle) * firstNodeRadius * 0.7;
      
      gfx->drawLine(nodeX, nodeY, firstX, firstY, connectionColor);
    }
  }
  
  // Central master gear
  float centralRotation = -hybridTime * 0.8;
  gfx->fillCircle(centerX, centerY, 25, gfx->color565(255, 200, 100));
  gfx->drawCircle(centerX, centerY, 25, gfx->color565(200, 200, 200));
  
  // Central gear teeth
  for (int tooth = 0; tooth < 12; tooth++) {
    float toothAngle = tooth * PI / 6 + centralRotation;
    int toothX = centerX + cos(toothAngle) * 25;
    int toothY = centerY + sin(toothAngle) * 25;
    gfx->fillRect(toothX - 1, toothY - 1, 3, 3, gfx->color565(255, 255, 255));
  }
  gfx->fillCircle(centerX, centerY, 8, gfx->color565(100, 100, 100));
  
  // Orbital gears
  for (int orbit = 0; orbit < 3; orbit++) {
    float orbitAngle = hybridTime * (0.5 + orbit * 0.3) + orbit * 2;
    float orbitRadius = 90 + orbit * 20;
    
    int orbitX = centerX + cos(orbitAngle) * orbitRadius;
    int orbitY = centerY + sin(orbitAngle) * orbitRadius;
    
    float orbitGearRotation = hybridTime * (2 - orbit * 0.4);
    uint16_t orbitColor = gfx->color565(200 - orbit * 50, 100 + orbit * 50, 150);
    
    // Simple orbital gear (circle with teeth marks)
    int gearRadius = 8 + orbit * 2;
    gfx->fillCircle(orbitX, orbitY, gearRadius, orbitColor);
    gfx->drawCircle(orbitX, orbitY, gearRadius, gfx->color565(200, 200, 200));
    
    // Simple teeth marks
    for (int tooth = 0; tooth < 5; tooth++) {
      float toothAngle = tooth * 2 * PI / 5 + orbitGearRotation;
      int toothX = orbitX + cos(toothAngle) * gearRadius;
      int toothY = orbitY + sin(toothAngle) * gearRadius;
      gfx->fillCircle(toothX, toothY, 1, gfx->color565(255, 255, 255));
    }
  }
  
  // Energy flow visualization
  for (int flow = 0; flow < 12; flow++) {
    float flowAngle = flow * PI / 6 + hybridTime * 3;
    float flowDistance = 40 + sin(hybridTime * 4 + flow) * 20;
    
    int flowX = centerX + cos(flowAngle) * flowDistance;
    int flowY = centerY + sin(flowAngle) * flowDistance;
    
    float pulseIntensity = sin(hybridTime * 6 + flow) * 0.5 + 0.5;
    int pulseAlpha = pulseIntensity * 200 + 55;
    
    gfx->fillCircle(flowX, flowY, 2, gfx->color565(pulseAlpha, pulseAlpha/2, 255));
  }
  
  // System stats
  gfx->setTextColor(gfx->color565(255, 255, 100));
  gfx->setTextSize(1);
  gfx->setCursor(10, gfx->height() - 40);
  gfx->printf("Nodes: %d", numNodes);
  gfx->setCursor(10, gfx->height() - 30);
  gfx->printf("Gears: %d", numNodes + 4);
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Efficiency: %.0f%%", 85 + sin(hybridTime * 2) * 10);
  
  gfx->setTextColor(gfx->color565(100, 255, 200));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("GEODESICGEARS");
}
}
// Stable slide 122: removed by screensaver policy.
namespace e122 {
char matrix[40][20];
bool initialized = false;
void reset(){std::memset(matrix,0,sizeof(matrix));initialized=false;}
void drawGlmatrix() {
  
  
  
  if (!initialized) {
    for (int y = 0; y < 20; y++) {
      for (int x = 0; x < 40; x++) {
        matrix[x][y] = random(33, 127); // ASCII printable
      }
    }
    initialized = true;
  }
  
  gfx->fillScreen(gfx->color565(0, 0, 0));
  
  // Matrix rain effect
  for (int col = 0; col < 40; col++) {
    // Column parameters
    float dropSpeed = 0.5 + (col % 3) * 0.3;
    int dropPos = (int)(animationTime * dropSpeed * 10) % 25;
    
    for (int row = 0; row < 20; row++) {
      // Calculate intensity based on distance from drop
      int distance = abs(row - dropPos);
      float intensity = 0;
      
      if (distance < 8) {
        intensity = 1.0 - (distance / 8.0);
        if (distance == 0) intensity = 1.0; // Bright head
      }
      
      if (intensity > 0) {
        // Occasionally change character
        if (random(100) < 5) {
          matrix[col][row] = random(33, 127);
        }
        
        // Color based on intensity
        int green = intensity * 255;
        int blue = intensity * 100;
        uint16_t charColor = gfx->color565(0, green, blue);
        
        // Draw character
        gfx->setTextColor(charColor);
        gfx->setTextSize(1);
        gfx->setCursor(col * 8, row * 12);
        gfx->write(matrix[col][row]);
      }
    }
  }
  
  // Digital rain streaks
  for (int streak = 0; streak < 15; streak++) {
    int streakXInt = (int)(streak * 25 + animationTime * 30) % gfx->width();
    float streakX = streakXInt;
    float streakSpeed = 2 + streak % 4;
    
    for (int particle = 0; particle < 8; particle++) {
      int particleYInt = (int)(animationTime * streakSpeed * 50 + particle * 20) % (gfx->height() + 100);
      float particleY = particleYInt - 50;
      
      if (particleY >= 0 && particleY < gfx->height()) {
        float alpha = (8 - particle) / 8.0;
        int intensity = alpha * 150;
        uint16_t streakColor = gfx->color565(0, intensity, intensity/2);
        
        gfx->fillCircle(streakX, particleY, 1, streakColor);
      }
    }
  }
  
  gfx->setTextColor(gfx->color565(0, 255, 100));
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->printf("GLMATRIX");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Streams: %d", 40);
}
}
// Stable slide 124: removed by screensaver policy.
// Stable slide 125: removed by screensaver policy.
// Stable slide 126: removed by screensaver policy.
// Stable slide 127: removed by screensaver policy.
namespace e127 {

void reset(){}
void drawGlslUtils() {
  gfx->fillScreen(BLACK);
  
  // GLSL utility shader effects - vertex and fragment processing
  int numParticles = 20;
  float shaderTime = animationTime * 2.0;
  
  for (int i = 0; i < numParticles; i++) {
    float x = sin(shaderTime + i * 0.3) * 80 + gfx->width() / 2;
    float y = cos(shaderTime * 0.7 + i * 0.5) * 60 + gfx->height() / 2;
    
    // Vertex shader simulation
    float vertexIntensity = (sin(shaderTime + i) + 1.0) / 2.0;
    
    // Fragment shader color processing
    int r = (int)(255 * vertexIntensity);
    int g = (int)(128 + 127 * sin(shaderTime * 2 + i));
    int b = (int)(64 + 191 * cos(shaderTime * 1.5 + i));
    
    gfx->fillCircle(x, y, 3 + vertexIntensity * 2, gfx->color565(r, g, b));
  }
  
  gfx->setTextColor(WHITE);
  gfx->setCursor(10, 10);
  gfx->printf("GLSLutils");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Shader: %.1f", shaderTime);
}
}
namespace e128 {
int snakeX[50], snakeY[50];
int snakeLength = 10;
float snakeAngle = 0;
bool initialized = false;
void reset(){std::memset(snakeX,0,sizeof(snakeX));snakeLength=10;snakeAngle=0;initialized=false;}
void drawGlsnake() {
  gfx->fillScreen(BLACK);
  
  // 3D snake game simulation
  
  
  
  
  
  if (!initialized) {
    for (int i = 0; i < snakeLength; i++) {
      snakeX[i] = gfx->width() / 2 - i * 8;
      snakeY[i] = gfx->height() / 2;
    }
    initialized = true;
  }
  
  // Update snake movement
  snakeAngle += 0.1;
  
  // Move snake head
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i-1];
    snakeY[i] = snakeY[i-1];
  }
  
  snakeX[0] += cos(snakeAngle) * 2;
  snakeY[0] += sin(snakeAngle * 0.7) * 2;
  
  // Wrap around screen
  if (snakeX[0] < 0) snakeX[0] = gfx->width();
  if (snakeX[0] >= gfx->width()) snakeX[0] = 0;
  if (snakeY[0] < 0) snakeY[0] = gfx->height();
  if (snakeY[0] >= gfx->height()) snakeY[0] = 0;
  
  // Draw snake segments with 3D effect
  for (int i = 0; i < snakeLength; i++) {
    float segmentDepth = 1.0 - (float)i / snakeLength;
    int brightness = (int)(255 * segmentDepth);
    int segmentSize = 3 + segmentDepth * 2;
    
    gfx->fillCircle(snakeX[i], snakeY[i], segmentSize, 
                   gfx->color565(brightness, brightness/2, 0));
  }
  
  // Draw food items
  for (int i = 0; i < 3; i++) {
    int foodX = 40 + i * 80;
    int foodY = 80 + sin(animationTime + i) * 20;
    gfx->fillRect(foodX, foodY, 6, 6, GREEN);
  }
  
  gfx->setTextColor(WHITE);
  gfx->setCursor(10, 10);
  gfx->printf("GLsnake");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Length: %d", snakeLength);
}
}
// Stable slide 130: removed by screensaver policy.
namespace e130 {
float goopTime = 0;
void reset(){goopTime=0;}
void drawGoop() {
  
  goopTime += 0.02;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(GREEN);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple goop blobs with organic movement
  for (int blob = 0; blob < 8; blob++) {
    float angle = blob * PI / 4 + goopTime * 0.5;
    float radius = 60 + sin(goopTime * 2 + blob) * 20;
    
    int x = centerX + cos(angle) * radius;
    int y = centerY + sin(angle) * radius;
    
    // Goop blob size varies organically
    int blobSize = 15 + sin(goopTime * 3 + blob * 0.7) * 8;
    
    // Color shifts through spectrum
    uint16_t color = gfx->color565(
      128 + sin(goopTime + blob * 0.5) * 127,
      128 + sin(goopTime * 1.3 + blob * 0.7) * 127,
      128 + sin(goopTime * 1.7 + blob * 0.9) * 127
    );
    
    gfx->fillCircle(x, y, blobSize, color);
    
    // Connecting tentacles between nearby blobs
    if (blob < 7) {
      float nextAngle = (blob + 1) * PI / 4 + goopTime * 0.5;
      float nextRadius = 60 + sin(goopTime * 2 + (blob + 1)) * 20;
      int nextX = centerX + cos(nextAngle) * nextRadius;
      int nextY = centerY + sin(nextAngle) * nextRadius;
      
      gfx->drawLine(x, y, nextX, nextY, color);
    }
  }
  
  // Title
  gfx->setCursor(10, 10);
  gfx->setTextSize(2);
  gfx->printf("GOOP");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->setTextSize(1);
  gfx->printf("Organic blobs: %d", (int)(goopTime * 10) % 100);
}
}
namespace e131 {
float trackTime = 0;
void reset(){trackTime=0;}
void drawGltrackball() {
  
  trackTime += 0.03;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(CYAN);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Rotating trackball sphere simulation
  for (int i = 0; i < 20; i++) {
    float lat = (i - 10) * PI / 20;
    for (int j = 0; j < 40; j++) {
      float lon = j * PI / 20 + trackTime;
      
      // 3D to 2D projection
      float x3d = cos(lat) * cos(lon);
      float y3d = cos(lat) * sin(lon);
      float z3d = sin(lat);
      
      // Simple rotation around Y axis
      float rotX = x3d * cos(trackTime * 0.7) - z3d * sin(trackTime * 0.7);
      float rotZ = x3d * sin(trackTime * 0.7) + z3d * cos(trackTime * 0.7);
      
      if (rotZ > 0) { // Only draw front-facing points
        int screenX = centerX + rotX * 80;
        int screenY = centerY - y3d * 80;
        
        // Color based on depth
        uint8_t intensity = static_cast<uint8_t>(constrain(double((rotZ + 1) * 127),0.0,255.0));
        uint16_t color = gfx->color565(intensity, intensity, 255);
        
        gfx->drawPixel(screenX, screenY, color);
      }
    }
  }
  
  // Title
  gfx->setCursor(10, 10);
  gfx->setTextSize(2);
  gfx->printf("GLTrackball");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->setTextSize(1);
  gfx->printf("Rotation: %.0f°", trackTime * 180 / PI);
}
}
namespace e132 {
float strokeTime = 0;
void reset(){strokeTime=0;}
void drawGlutStroke() {
  
  strokeTime += 0.04;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(MAGENTA);
  
  // Simulated stroke font rendering with vector-like text
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw animated stroke letters
  for (int letter = 0; letter < 5; letter++) {
    float angle = letter * PI / 2.5 + strokeTime;
    int x = centerX + cos(angle) * 60;
    int y = centerY + sin(angle) * 40;
    
    // Different stroke patterns for each letter
    switch (letter) {
      case 0: // Letter A
        gfx->drawLine(x-8, y+10, x, y-10, WHITE);
        gfx->drawLine(x, y-10, x+8, y+10, WHITE);
        gfx->drawLine(x-4, y, x+4, y, WHITE);
        break;
      case 1: // Letter B
        gfx->drawLine(x-6, y-10, x-6, y+10, WHITE);
        gfx->drawLine(x-6, y-10, x+4, y-10, WHITE);
        gfx->drawLine(x-6, y, x+2, y, WHITE);
        gfx->drawLine(x-6, y+10, x+4, y+10, WHITE);
        break;
      case 2: // Letter C
        gfx->drawCircle(x, y, 8, WHITE);
        gfx->drawLine(x+6, y-6, x+8, y-8, BLACK);
        gfx->drawLine(x+6, y+6, x+8, y+8, BLACK);
        break;
      case 3: // Letter D
        gfx->drawLine(x-6, y-10, x-6, y+10, WHITE);
        gfx->drawCircle(x-2, y, 8, WHITE);
        gfx->drawLine(x-10, y-8, x-6, y-10, BLACK);
        gfx->drawLine(x-10, y+8, x-6, y+10, BLACK);
        break;
      case 4: // Letter E
        gfx->drawLine(x-6, y-10, x-6, y+10, WHITE);
        gfx->drawLine(x-6, y-10, x+6, y-10, WHITE);
        gfx->drawLine(x-6, y, x+2, y, WHITE);
        gfx->drawLine(x-6, y+10, x+6, y+10, WHITE);
        break;
    }
  }
  
  // Title
  gfx->setCursor(10, 10);
  gfx->setTextSize(2);
  gfx->printf("GLUT Stroke");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->setTextSize(1);
  gfx->printf("Vector fonts");
}
}
namespace e133 {
float swidthTime = 0;
void reset(){swidthTime=0;}
void drawGlutSwidth() {
  
  swidthTime += 0.05;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(YELLOW);
  
  // String width calculation visualization
  int centerY = gfx->height() / 2;
  
  // Animated text with varying widths
  const char* texts[] = {"A", "AB", "ABC", "ABCD", "ABCDE"};
  
  for (int i = 0; i < 5; i++) {
    float wave = sin(swidthTime + i * 0.5) * 30;
    int x = 20 + i * 50 + wave;
    int y = centerY + sin(swidthTime * 2 + i) * 20;
    
    // Calculate text width (simulated)
    int textWidth = strlen(texts[i]) * 12; // 12 pixels per character
    
    // Draw text with width indicator
    gfx->setCursor(x, y);
    gfx->setTextSize(2);
    gfx->printf("%s", texts[i]);
    
    // Draw width measurement
    gfx->drawLine(x, y + 25, x + textWidth, y + 25, RED);
    gfx->drawLine(x, y + 23, x, y + 27, RED);
    gfx->drawLine(x + textWidth, y + 23, x + textWidth, y + 27, RED);
    
    // Width number
    gfx->setCursor(x, y + 30);
    gfx->setTextSize(1);
    gfx->printf("%dpx", textWidth);
  }
  
  // Title
  gfx->setCursor(10, 10);
  gfx->setTextSize(2);
  gfx->printf("GLUT SWidth");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->setTextSize(1);
  gfx->printf("String width calc");
}
}
// Stable slide 135: removed by screensaver policy.
namespace e135 {
float ximageTime = 0;
void reset(){ximageTime=0;}
void drawGrabXimage() {
  
  ximageTime += 0.04;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(CYAN);
  
  // X11 image capture simulation with pixel-by-pixel effect
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Progressive image "capture" effect
  int captureRadius = (int)(sin(ximageTime) * 50 + 60);
  
  for (int angle = 0; angle < 360; angle += 10) {
    float rad = angle * PI / 180;
    int x = centerX + cos(rad) * captureRadius;
    int y = centerY + sin(rad) * captureRadius;
    
    if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
      // Simulated captured pixel data
      uint16_t color = gfx->color565(
        128 + sin(ximageTime + angle * 0.01) * 127,
        128 + sin(ximageTime * 1.3 + angle * 0.02) * 127,
        128 + sin(ximageTime * 1.7 + angle * 0.03) * 127
      );
      
      // Draw captured "pixel" with size based on distance
      int pixelSize = 3 + sin(ximageTime + angle * 0.1) * 2;
      gfx->fillRect(x-pixelSize/2, y-pixelSize/2, pixelSize, pixelSize, color);
    }
  }
  
  // X11 window frame simulation
  gfx->drawRect(centerX-80, centerY-60, 160, 120, WHITE);
  gfx->drawRect(centerX-78, centerY-58, 156, 116, BLUE);
  
  // Title bar
  gfx->fillRect(centerX-76, centerY-56, 152, 20, BLUE);
  gfx->setCursor(centerX-70, centerY-52);
  gfx->setTextSize(1);
  gfx->printf("XImage Capture");
  
  // Main title
  gfx->setCursor(10, 10);
  gfx->setTextSize(2);
  gfx->printf("Grab XImage");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->setTextSize(1);
  gfx->printf("X11 capture: %d px", captureRadius);
}
}
namespace e136 {
float particles[20][4];
bool initialized = false;
void reset(){std::memset(particles,0,sizeof(particles));initialized=false;}
void drawGrav() {
  gfx->fillScreen(BLACK);
  
  // Gravity simulation with particles
   // x, y, vx, vy for each particle
  
  
  if (!initialized) {
    for (int i = 0; i < 20; i++) {
      particles[i][0] = random(gfx->width());
      particles[i][1] = random(gfx->height());
      particles[i][2] = (random(200) - 100) / 100.0;
      particles[i][3] = (random(200) - 100) / 100.0;
    }
    initialized = true;
  }
  
  float centerX = gfx->width() / 2 + sin(animationTime) * 40;
  float centerY = gfx->height() / 2 + cos(animationTime * 0.7) * 30;
  
  // Draw gravitational center
  gfx->fillCircle(centerX, centerY, 8, YELLOW);
  
  for (int i = 0; i < 20; i++) {
    float dx = centerX - particles[i][0];
    float dy = centerY - particles[i][1];
    float dist = sqrt(dx*dx + dy*dy);
    
    if (dist > 5) {
      float force = 200.0 / (dist * dist);
      particles[i][2] += (dx / dist) * force * 0.01;
      particles[i][3] += (dy / dist) * force * 0.01;
    }
    
    particles[i][0] += particles[i][2];
    particles[i][1] += particles[i][3];
    
    // Wrap around screen
    if (particles[i][0] < 0) particles[i][0] = gfx->width();
    if (particles[i][0] >= gfx->width()) particles[i][0] = 0;
    if (particles[i][1] < 0) particles[i][1] = gfx->height();
    if (particles[i][1] >= gfx->height()) particles[i][1] = 0;
    
    uint16_t color = gfx->color565(255 - i*10, 100 + i*5, i*12);
    gfx->fillCircle(particles[i][0], particles[i][1], 2, color);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextSize(2);
  gfx->printf("GRAV");
}
}
namespace e137 {

void reset(){}
void drawGravitywell() {
  gfx->fillScreen(BLACK);
  
  // Multiple gravity wells
  int wells = 3;
  float wellX[3], wellY[3];
  
  for (int w = 0; w < wells; w++) {
    wellX[w] = gfx->width()/2 + sin(animationTime + w*2) * 60;
    wellY[w] = gfx->height()/2 + cos(animationTime*0.8 + w*2) * 40;
  }
  
  // Draw field visualization
  for (int x = 0; x < gfx->width(); x += 8) {
    for (int y = 0; y < gfx->height(); y += 8) {
      float totalForceX = 0, totalForceY = 0;
      
      for (int w = 0; w < wells; w++) {
        float dx = wellX[w] - x;
        float dy = wellY[w] - y;
        float dist = sqrt(dx*dx + dy*dy) + 1;
        float force = 100.0 / dist;
        
        totalForceX += (dx / dist) * force;
        totalForceY += (dy / dist) * force;
      }
      
      float intensity = sqrt(totalForceX*totalForceX + totalForceY*totalForceY);
      uint8_t brightness = static_cast<uint8_t>(constrain(double(min(255, (int)(intensity * 20))),0.0,255.0));
      
      if (brightness > 20) {
        uint16_t color = gfx->color565(brightness/2, brightness, brightness/3);
        gfx->fillRect(x, y, 4, 4, color);
      }
    }
  }
  
  // Draw gravity wells
  for (int w = 0; w < wells; w++) {
    uint16_t wellColor = gfx->color565(255, 150 + w*30, 100 + w*50);
    gfx->fillCircle(wellX[w], wellY[w], 10, wellColor);
    
    // Draw gravitational rings
    for (int r = 15; r < 50; r += 10) {
      uint8_t alpha = static_cast<uint8_t>(constrain(double(100 - (r-15)*3),0.0,255.0));
      gfx->drawCircle(wellX[w], wellY[w], r, gfx->color565(alpha, alpha/2, alpha/4));
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextSize(2);
  gfx->printf("GravityWELL");
}
}
// Stable slide 139: removed by screensaver policy.
namespace e139 {
float halftoneTime = 0;
void reset(){halftoneTime=0;}
void drawHalftone() {
  
  halftoneTime += 0.03;
  
  gfx->fillScreen(BLACK);
  
  int spacing = 12;
  int maxDotSize = 8;
  
  // Create gravity points
  int numPoints = 4;
  float gravityX[4], gravityY[4];
  
  for (int i = 0; i < numPoints; i++) {
    gravityX[i] = gfx->width()/2 + sin(halftoneTime * (0.7 + i * 0.3)) * (gfx->width() * 0.3);
    gravityY[i] = gfx->height()/2 + cos(halftoneTime * (0.5 + i * 0.2)) * (gfx->height() * 0.3);
  }
  
  // Draw halftone dots based on gravitational field
  for (int y = spacing/2; y < gfx->height(); y += spacing) {
    for (int x = spacing/2; x < gfx->width(); x += spacing) {
      
      float totalForce = 0;
      for (int i = 0; i < numPoints; i++) {
        float dx = x - gravityX[i];
        float dy = y - gravityY[i];
        float distance = sqrt(dx*dx + dy*dy) + 1;
        totalForce += 1000.0 / (distance * distance);
      }
      
      int dotSize = constrain(totalForce * 0.1, 0, maxDotSize);
      
      if (dotSize > 1) {
        uint8_t intensity = static_cast<uint8_t>(constrain(double(map(dotSize, 0, maxDotSize, 0, 255)),0.0,255.0));
        uint16_t dotColor = gfx->color565(intensity/3, intensity/2, intensity);
        gfx->fillCircle(x, y, dotSize/2, dotColor);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextSize(2);
  gfx->printf("HALFTONE");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->setTextSize(1);
  gfx->printf("Gravity Field Dots");
}
}
namespace e140 {
float haloTime = 0;
int haloPhase = 0;
void reset(){haloTime=0;haloPhase=0;}
void drawHalo() {
  
  
  haloTime += 0.05;
  
  if (haloTime > 6.28) {
    haloTime = 0;
    haloPhase = (haloPhase + 1) % 3;
  }
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  int maxRadius = min(gfx->width(), gfx->height()) / 2;
  
  // Draw multiple concentric halos
  for (int h = 0; h < 4; h++) {
    float phaseOffset = h * 1.57; // π/2
    float radius = maxRadius * (0.3 + 0.2 * h) * (1 + sin(haloTime * 2 + phaseOffset) * 0.3);
    
    for (int i = 0; i < 36; i++) {
      float angle = i * 0.174533; // 10 degrees in radians
      float x = centerX + cos(angle + haloTime) * radius;
      float y = centerY + sin(angle + haloTime) * radius;
      
      uint8_t brightness = static_cast<uint8_t>(constrain(double(128 + sin(haloTime * 3 + i * 0.2) * 127),0.0,255.0));
      uint16_t color = BLACK;
      
      switch (haloPhase) {
        case 0: color = gfx->color565(brightness, brightness/2, 0); break;
        case 1: color = gfx->color565(0, brightness, brightness/2); break;
        case 2: color = gfx->color565(brightness/2, 0, brightness); break;
      }
      
      gfx->fillCircle(x, y, 3, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextSize(2);
  gfx->printf("HALO");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->setTextSize(1);
  gfx->printf("Phase: %d", haloPhase + 1);
}
}
namespace e141 {
float handsyTime = 0;
void reset(){handsyTime=0;}
void drawHandsy() {
  
  handsyTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  // Draw animated hand silhouettes
  int numHands = 3;
  
  for (int h = 0; h < numHands; h++) {
    float handPhase = handsyTime + h * 2.094; // 120 degrees apart
    
    int handX = gfx->width() * (0.2 + h * 0.3);
    int handY = gfx->height() * (0.5 + sin(handPhase) * 0.2);
    
    // Hand palm
    uint16_t handColor = gfx->color565(180 + h * 20, 140 + h * 30, 100 + h * 40);
    gfx->fillRect(handX, handY, 20, 35, handColor);
    
    // Animated fingers
    for (int f = 0; f < 5; f++) {
      float fingerPhase = handsyTime * (1 + f * 0.2) + h;
      int fingerLength = 15 + sin(fingerPhase) * 8;
      int fingerX = handX + f * 4;
      int fingerY = handY - fingerLength;
      
      gfx->fillRect(fingerX, fingerY, 3, fingerLength, handColor);
      
      // Finger joints
      int joint1Y = fingerY + fingerLength * 0.3;
      int joint2Y = fingerY + fingerLength * 0.7;
      
      uint16_t jointColor = gfx->color565(200, 180, 160);
      gfx->fillCircle(fingerX + 1, joint1Y, 1, jointColor);
      gfx->fillCircle(fingerX + 1, joint2Y, 1, jointColor);
    }
    
    // Thumb
    float thumbPhase = handsyTime * 1.5 + h;
    int thumbX = handX - 5;
    int thumbY = handY + 10 + sin(thumbPhase) * 5;
    gfx->fillRect(thumbX, thumbY, 12, 4, handColor);
  }
  
  // Draw gesture trails
  for (int t = 0; t < 20; t++) {
    float trailPhase = handsyTime - t * 0.1;
    int trailX = gfx->width()/2 + sin(trailPhase * 2) * 80;
    int trailY = gfx->height()/2 + cos(trailPhase * 1.5) * 60;
    
    uint8_t alpha = static_cast<uint8_t>(constrain(double(255 - t * 12),0.0,255.0));
    uint16_t trailColor = gfx->color565(alpha/4, alpha/6, alpha/3);
    gfx->drawPixel(trailX, trailY, trailColor);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextSize(2);
  gfx->printf("HANDSY");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->setTextSize(1);
  gfx->printf("Hand Gestures");
}
}
namespace e142 {
float modelTime = 0;
void reset(){modelTime=0;}
void drawHandsyModel() {
  
  modelTime += 0.06;
  
  gfx->fillScreen(BLACK);
  
  // Draw 3D hand model wireframe
  int centerX = gfx->width()/2;
  int centerY = gfx->height()/2;
  
  // Rotating 3D hand vertices
  float vertices[12][3] = {
    {-20, -30, 0}, {20, -30, 0}, {20, 30, 0}, {-20, 30, 0},  // Palm
    {-15, -45, 5}, {-7, -50, 8}, {2, -48, 6}, {10, -52, 4}, {18, -45, 2},  // Fingers
    {-25, -10, 3}, {-30, 0, 5}, {-25, 10, 3}  // Thumb
  };
  
  // Rotate and project vertices
  int projected[12][2];
  for (int i = 0; i < 12; i++) {
    float x = vertices[i][0];
    float y = vertices[i][1] * cos(modelTime) - vertices[i][2] * sin(modelTime);
    float z = vertices[i][1] * sin(modelTime) + vertices[i][2] * cos(modelTime);
    
    // Simple perspective projection
    float scale = 100.0 / (100 + z);
    projected[i][0] = centerX + x * scale;
    projected[i][1] = centerY + y * scale;
  }
  
  // Draw wireframe connections
  uint16_t wireColor = gfx->color565(150, 200, 255);
  
  // Palm connections
  for (int i = 0; i < 3; i++) {
    gfx->drawLine(projected[i][0], projected[i][1], 
                  projected[i+1][0], projected[i+1][1], wireColor);
  }
  gfx->drawLine(projected[3][0], projected[3][1], 
                projected[0][0], projected[0][1], wireColor);
  
  // Finger connections
  for (int i = 4; i < 9; i++) {
    gfx->drawLine(projected[0][0], projected[0][1], 
                  projected[i][0], projected[i][1], wireColor);
  }
  
  // Thumb connections  
  for (int i = 9; i < 12; i++) {
    gfx->drawLine(projected[0][0], projected[0][1], 
                  projected[i][0], projected[i][1], wireColor);
  }
  
  // Draw vertices as dots
  for (int i = 0; i < 12; i++) {
    uint16_t dotColor = gfx->color565(255, 180 + i * 5, 100);
    gfx->fillCircle(projected[i][0], projected[i][1], 2, dotColor);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextSize(2);
  gfx->printf("HANDSY 3D");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->setTextSize(1);
  gfx->printf("3D Hand Model");
}
}
// Stable slide 144: removed by screensaver policy.
namespace e144 {
float modelTime = 0;
void reset(){modelTime=0;}
void drawHeadroomModel() {
  
  modelTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width()/2;
  int centerY = gfx->height()/2;
  
  // 3D head wireframe model
  float rotX = modelTime * 0.7;
  float rotY = modelTime * 0.5;
  
  // Define 3D head vertices (simplified)
  float vertices[16][3] = {
    // Head outline (8 points)
    {0, -40, 20}, {20, -30, 15}, {30, 0, 10}, {20, 30, 15},
    {0, 40, 20}, {-20, 30, 15}, {-30, 0, 10}, {-20, -30, 15},
    // Inner features (8 points)
    {-10, -15, 25}, {10, -15, 25}, // Eyes
    {0, 0, 30}, // Nose tip
    {-8, 15, 25}, {8, 15, 25}, // Mouth corners
    {0, 20, 25}, // Mouth center
    {0, -45, 15}, {0, 45, 15} // Top/bottom
  };
  
  // Rotate and project vertices
  int projected[16][2];
  for (int i = 0; i < 16; i++) {
    // Rotate around Y axis
    float x1 = vertices[i][0] * cos(rotY) + vertices[i][2] * sin(rotY);
    float z1 = -vertices[i][0] * sin(rotY) + vertices[i][2] * cos(rotY);
    
    // Rotate around X axis  
    float y2 = vertices[i][1] * cos(rotX) - z1 * sin(rotX);
    float z2 = vertices[i][1] * sin(rotX) + z1 * cos(rotX);
    
    // Project to 2D
    float scale = 200.0 / (200 + z2);
    projected[i][0] = centerX + x1 * scale;
    projected[i][1] = centerY + y2 * scale;
  }
  
  // Draw head outline connections
  uint16_t outlineColor = gfx->color565(100, 255, 200);
  for (int i = 0; i < 7; i++) {
    gfx->drawLine(projected[i][0], projected[i][1], 
                  projected[i+1][0], projected[i+1][1], outlineColor);
  }
  gfx->drawLine(projected[7][0], projected[7][1], 
                projected[0][0], projected[0][1], outlineColor);
  
  // Draw facial features
  uint16_t featureColor = gfx->color565(255, 200, 100);
  
  // Eyes
  gfx->drawLine(projected[8][0], projected[8][1], 
                projected[9][0], projected[9][1], featureColor);
  
  // Nose
  gfx->drawLine(projected[9][0], projected[9][1], 
                projected[10][0], projected[10][1], featureColor);
  gfx->drawLine(projected[8][0], projected[8][1], 
                projected[10][0], projected[10][1], featureColor);
  
  // Mouth
  gfx->drawLine(projected[11][0], projected[11][1], 
                projected[13][0], projected[13][1], featureColor);
  gfx->drawLine(projected[11][0], projected[11][1], 
                projected[12][0], projected[12][1], featureColor);
  gfx->drawLine(projected[12][0], projected[12][1], 
                projected[13][0], projected[13][1], featureColor);
  
  // Vertical guides
  gfx->drawLine(projected[14][0], projected[14][1], 
                projected[15][0], projected[15][1], 
                gfx->color565(80, 80, 255));
  
  // Draw all vertices as points
  for (int i = 0; i < 16; i++) {
    uint16_t pointColor = (i < 8) ? outlineColor : featureColor;
    gfx->fillCircle(projected[i][0], projected[i][1], 1, pointColor);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextSize(2);
  gfx->printf("HEAD 3D");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->setTextSize(1);
  gfx->printf("Rotation: %.0f°", rotY * 57.3);
}
}
// Stable slide 146: removed by screensaver policy.
// Stable slide 147: removed by screensaver policy.
// Stable slide 148: removed by screensaver policy.
namespace e148 {
float time = 0;
void reset(){time=0;}
void drawHextrail() {
  
  time += 0.03;
  
  gfx->fillScreen(0x0000);
  
  // Hexagonal trail effect - flowing hex patterns
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int i = 0; i < 8; i++) {
    float angle = time + i * 0.8;
    int x = centerX + cos(angle) * (30 + i * 15);
    int y = centerY + sin(angle) * (20 + i * 10);
    
    // Draw hexagon
    uint16_t color = gfx->color565(
      (int)(128 + 127 * sin(time + i * 0.5)),
      (int)(128 + 127 * cos(time + i * 0.7)),
      (int)(128 + 127 * sin(time + i * 0.9))
    );
    
    // Simple hexagon approximation
    for (int j = 0; j < 6; j++) {
      float a1 = j * PI / 3;
      float a2 = (j + 1) * PI / 3;
      int x1 = x + cos(a1) * 8;
      int y1 = y + sin(a1) * 8;
      int x2 = x + cos(a2) * 8;
      int y2 = y + sin(a2) * 8;
      gfx->drawLine(x1, y1, x2, y2, color);
    }
  }
  
  gfx->setTextColor(0xFFFF);
  gfx->setCursor(10, 10);
  gfx->printf("HEXTRAIL");
}
}
// Stable slide 150: removed by screensaver policy.
// Stable slide 151: removed by screensaver policy.
namespace e151 {
int order = 4;
float curveTime = 0;
void reset(){order=4;curveTime=0;}
void drawHilbert() {
  // Hilbert space-filling curve
  gfx->fillScreen(BLACK);
  
  
  
  curveTime += 0.02;
  
  // Simple Hilbert curve approximation with animated colors
  int size = min(gfx->width(), gfx->height()) / (1 << order);
  int offsetX = (gfx->width() - (size << order)) / 2;
  int offsetY = (gfx->height() - (size << order)) / 2;
  
  for (int i = 0; i < (1 << (order * 2)); i++) {
    int x, y;
    // Simplified Hilbert coordinate calculation
    int t = i;
    int s = 1;
    x = y = 0;
    while (s < (1 << order)) {
      int rx = 1 & (t / 2);
      int ry = 1 & (t ^ rx);
      if (ry == 0) {
        if (rx == 1) {
          x = s - 1 - x;
          y = s - 1 - y;
        }
        int temp = x;
        x = y;
        y = temp;
      }
      x += s * rx;
      y += s * ry;
      t /= 4;
      s *= 2;
    }
    
    float phase = curveTime + i * 0.01;
    uint16_t color = gfx->color565(
      (int)(sin(phase) * 127 + 128),
      (int)(cos(phase + 1) * 127 + 128),
      (int)(sin(phase + 2) * 127 + 128)
    );
    
    gfx->fillRect(offsetX + x * size, offsetY + y * size, size, size, color);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("HILBERT");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Order: %d", order);
}
}
namespace e152 {
float x = 0, y = 0;
float a = 7.7, b = 0.4, c = 1.0;
float paramTime = 0;
void reset(){x=0, y = 0;a=7.7, b = 0.4, c = 1.0;paramTime=0;}
void drawHopalong() {
  // Hopalong attractor
  gfx->fillScreen(BLACK);
  
  
  
  
  paramTime += 0.005;
  
  // Slowly vary parameters
  a = 7.7 + sin(paramTime) * 2.0;
  b = 0.4 + cos(paramTime * 1.3) * 0.2;
  
  for (int i = 0; i < 500; i++) {
    float newX = y - (x < 0 ? -1 : 1) * sqrt(abs(b * x - c));
    float newY = a - x;
    
    x = newX;
    y = newY;
    
    int px = (int)(x * 15 + gfx->width() / 2);
    int py = (int)(y * 15 + gfx->height() / 2);
    
    if (px >= 0 && px < gfx->width() && py >= 0 && py < gfx->height()) {
      float phase = i * 0.02 + paramTime * 5;
      uint16_t color = gfx->color565(
        (int)(sin(phase) * 127 + 128),
        (int)(cos(phase + 1) * 127 + 128),
        (int)(sin(phase + 2) * 127 + 128)
      );
      gfx->drawPixel(px, py, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("HOPALONG");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("a=%.1f b=%.1f", a, b);
}
}
namespace e153 {
float hopfTime = 0;
void reset(){hopfTime=0;}
void drawHopfanimations() {
  // Hopf fibration visualization
  gfx->fillScreen(BLACK);
  
  
  hopfTime += 0.03;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  int radius = min(centerX, centerY) - 20;
  
  // Draw Hopf circles
  for (int fiber = 0; fiber < 12; fiber++) {
    float fiberAngle = fiber * PI * 2 / 12;
    float phase = hopfTime + fiberAngle;
    
    // Hopf fibration parameters
    float u = cos(phase);
    float v = sin(phase);
    
    for (int t = 0; t < 36; t++) {
      float theta = t * PI * 2 / 36;
      
      // Stereographic projection from 3-sphere to 2-sphere
      float x = 2 * u * cos(theta) / (1 + u*u + v*v);
      float y = 2 * v * sin(theta) / (1 + u*u + v*v);
      
      int px = centerX + (int)(x * radius);
      int py = centerY + (int)(y * radius);
      
      if (px >= 0 && px < gfx->width() && py >= 0 && py < gfx->height()) {
        uint16_t color = gfx->color565(
          (int)(sin(fiberAngle + hopfTime) * 127 + 128),
          (int)(cos(fiberAngle + hopfTime + 1) * 127 + 128),
          (int)(sin(fiberAngle + hopfTime + 2) * 127 + 128)
        );
        gfx->drawPixel(px, py, color);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("HOPF");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Fibration");
}
}
namespace e154 {
float hopfTime = 0;
void reset(){hopfTime=0;}
void drawHopffibration() {
  
  hopfTime += 0.03;
  
  gfx->fillScreen(BLACK);
  
  // Hopf fibration visualization with mathematical mapping
  for (int i = 0; i < 120; i++) {
    float theta = (i / 120.0) * 2 * PI + hopfTime;
    float phi = sin(hopfTime * 0.7 + i * 0.1) * PI;
    
    // Stereographic projection from S3 to R3
    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);
    
    // Project to screen coordinates
    int screenX = gfx->width()/2 + (int)(x * 80);
    int screenY = gfx->height()/2 + (int)(y * 80);
    
    uint16_t color = gfx->color565(
      (sin(theta + hopfTime) * 127 + 128),
      (cos(phi + hopfTime * 0.8) * 127 + 128),
      (sin(theta * 2 + phi) * 127 + 128)
    );
    
    if (screenX >= 0 && screenX < gfx->width() && screenY >= 0 && screenY < gfx->height()) {
      gfx->fillCircle(screenX, screenY, 2, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("HOPF Fibration");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("S3 -> S2 Projection");
}
}
// Stable slide 156: removed by screensaver policy.
namespace e156 {
float hydroTime = 0;
void reset(){hydroTime=0;}
void drawHydrostat() {
  
  hydroTime += 0.025;
  
  gfx->fillScreen(BLACK);
  
  // Hydrostatic pressure visualization with fluid dynamics
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int y = 0; y < gfx->height(); y += 2) {
    for (int x = 0; x < gfx->width(); x += 2) {
      float dx = x - centerX;
      float dy = y - centerY;
      float dist = sqrt(dx*dx + dy*dy);
      
      // Fluid pressure field
      float pressure = sin(hydroTime * 2) * cos(dist * 0.1 + hydroTime);
      pressure += sin(hydroTime * 1.5 + dx * 0.05) * cos(hydroTime * 0.8 + dy * 0.05);
      
      // Depth-based coloring (blue to green gradient)
      float depth = (y / (float)gfx->height()) + pressure * 0.3;
      depth = constrain(depth, 0, 1);
      
      uint8_t blue = static_cast<uint8_t>(constrain(double(255 * (1 - depth * 0.7)),0.0,255.0));
      uint8_t green = static_cast<uint8_t>(constrain(double(180 * depth),0.0,255.0));
      uint8_t red = static_cast<uint8_t>(constrain(double(50 * sin(hydroTime + dist * 0.1)),0.0,255.0));
      
      // Add bubbles effect
      if (sin(hydroTime * 3 + x * 0.1 + y * 0.15) > 0.95) {
        red = green = blue = 255;
      }
      
      gfx->drawPixel(x, y, gfx->color565(red, green, blue));
    }
  }
  
  // Draw surface waves
  for (int x = 0; x < gfx->width(); x += 3) {
    int waveY = 20 + sin(hydroTime * 2 + x * 0.1) * 10;
    gfx->drawPixel(x, waveY, WHITE);
    gfx->drawPixel(x, waveY + 1, WHITE);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("HYDROSTAT");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Pressure: %.1f", sin(hydroTime * 2) * 50 + 50);
}
}
namespace e157 {
float hyperTime = 0;
void reset(){hyperTime=0;}
void drawHyperball() {
  
  hyperTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  // 4D hypersphere projected to 3D then to 2D
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int i = 0; i < 60; i++) {
    float theta = i * PI / 30 + hyperTime;
    float phi = sin(hyperTime * 0.7 + i * 0.1) * PI;
    float psi = hyperTime * 0.5 + i * 0.15;
    
    // 4D coordinates
    float x4 = sin(phi) * cos(theta) * cos(psi);
    float y4 = sin(phi) * sin(theta) * cos(psi);
    float z4 = cos(phi) * cos(psi);
    float w4 = sin(psi);
    
    // Project from 4D to 3D (stereographic)
    float x3 = x4 / (1 - w4 + 0.1);
    float y3 = y4 / (1 - w4 + 0.1);
    float z3 = z4 / (1 - w4 + 0.1);
    
    // Project from 3D to 2D
    int screenX = centerX + (int)(x3 * 80);
    int screenY = centerY + (int)(y3 * 80);
    
    uint16_t color = gfx->color565(
      (sin(theta + hyperTime) * 127 + 128),
      (cos(phi + hyperTime * 0.8) * 127 + 128),
      (sin(psi + hyperTime * 1.2) * 127 + 128)
    );
    
    if (screenX >= 0 && screenX < gfx->width() && screenY >= 0 && screenY < gfx->height()) {
      gfx->fillCircle(screenX, screenY, 3 + sin(hyperTime + i) * 2, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("HYPERBALL");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("4D Hypersphere");
}
}
namespace e158 {
float cubeTime = 0;
void reset(){cubeTime=0;}
void drawHypercube() {
  
  cubeTime += 0.03;
  
  gfx->fillScreen(BLACK);
  
  // 4D hypercube (tesseract) visualization
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // 16 vertices of tesseract
  float vertices[16][4] = {
    {-1,-1,-1,-1}, {1,-1,-1,-1}, {-1,1,-1,-1}, {1,1,-1,-1},
    {-1,-1,1,-1}, {1,-1,1,-1}, {-1,1,1,-1}, {1,1,1,-1},
    {-1,-1,-1,1}, {1,-1,-1,1}, {-1,1,-1,1}, {1,1,-1,1},
    {-1,-1,1,1}, {1,-1,1,1}, {-1,1,1,1}, {1,1,1,1}
  };
  
  int projected[16][2];
  
  // Rotate and project vertices
  for (int i = 0; i < 16; i++) {
    // 4D rotation in XW and YZ planes
    float x = vertices[i][0] * cos(cubeTime) - vertices[i][3] * sin(cubeTime);
    float y = vertices[i][1] * cos(cubeTime * 0.7) - vertices[i][2] * sin(cubeTime * 0.7);
    float z = vertices[i][1] * sin(cubeTime * 0.7) + vertices[i][2] * cos(cubeTime * 0.7);
    float w = vertices[i][0] * sin(cubeTime) + vertices[i][3] * cos(cubeTime);
    
    // Project 4D to 3D
    float scale = 1.0 / (2 - w);
    float x3 = x * scale;
    float y3 = y * scale;
    float z3 = z * scale;
    
    // Project 3D to 2D
    projected[i][0] = centerX + (int)(x3 * 60);
    projected[i][1] = centerY + (int)(y3 * 60);
  }
  
  // Draw tesseract edges
  uint16_t edgeColor = gfx->color565(100, 200, 255);
  
  // Connect vertices that differ by one coordinate
  for (int i = 0; i < 16; i++) {
    for (int j = i + 1; j < 16; j++) {
      int diff = 0;
      for (int k = 0; k < 4; k++) {
        if (vertices[i][k] != vertices[j][k]) diff++;
      }
      
      if (diff == 1) {
        gfx->drawLine(projected[i][0], projected[i][1], 
                      projected[j][0], projected[j][1], edgeColor);
      }
    }
  }
  
  // Draw vertices
  for (int i = 0; i < 16; i++) {
    uint16_t vertexColor = gfx->color565(255, 150 + i * 6, 100);
    gfx->fillCircle(projected[i][0], projected[i][1], 2, vertexColor);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("HYPERCUBE");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("4D Tesseract");
}
}
namespace e159 {
float torusTime = 0;
void reset(){torusTime=0;}
void drawHypertorus() {
  
  torusTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  // 4D hypertorus visualization
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  float R = 30; // Major radius
  float r = 15; // Minor radius
  
  for (int u = 0; u < 24; u++) {
    for (int v = 0; v < 16; v++) {
      float theta = u * 2 * PI / 24 + torusTime;
      float phi = v * 2 * PI / 16 + torusTime * 0.7;
      
      // 4D torus parametrization
      float x = (R + r * cos(phi)) * cos(theta);
      float y = (R + r * cos(phi)) * sin(theta);
      float z = r * sin(phi);
      float w = r * sin(phi + torusTime * 0.5);
      
      // 4D to 3D projection
      float scale = 1.0 / (2 - w * 0.1);
      float x3 = x * scale;
      float y3 = y * scale;
      float z3 = z * scale;
      
      // 3D rotation for better view
      float rotY = torusTime * 0.3;
      float xr = x3 * cos(rotY) + z3 * sin(rotY);
      float yr = y3;
      
      int screenX = centerX + (int)xr;
      int screenY = centerY + (int)yr;
      
      if (screenX >= 0 && screenX < gfx->width() && screenY >= 0 && screenY < gfx->height()) {
        uint16_t color = gfx->color565(
          (sin(theta + torusTime) * 127 + 128),
          (cos(phi + torusTime * 0.8) * 127 + 128),
          (sin(theta + phi + torusTime * 1.3) * 127 + 128)
        );
        gfx->drawPixel(screenX, screenY, color);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("HYPERTORUS");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("4D Torus");
}
}
namespace e160 {
float hypnoTime = 0;
void reset(){hypnoTime=0;}
void drawHypnowheel() {
  
  hypnoTime += 0.03;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Hypnotic rotating wheel patterns
  for (int ring = 0; ring < 8; ring++) {
    int radius = 20 + ring * 15;
    int spokes = 8 + ring * 2;
    
    for (int i = 0; i < spokes; i++) {
      float angle = (float)i / spokes * 2 * PI + hypnoTime * (1 + ring * 0.2);
      
      int x1 = centerX + cos(angle) * (radius - 10);
      int y1 = centerY + sin(angle) * (radius - 10);
      int x2 = centerX + cos(angle) * radius;
      int y2 = centerY + sin(angle) * radius;
      
      uint16_t color = gfx->color565(
        (sin(hypnoTime * 2 + ring * 0.5) * 0.5 + 0.5) * 255,
        (cos(hypnoTime * 1.7 + i * 0.3) * 0.5 + 0.5) * 255,
        (sin(hypnoTime * 1.3 + ring * 0.7) * 0.5 + 0.5) * 255
      );
      
      gfx->drawLine(x1, y1, x2, y2, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(CYAN);
  gfx->printf("HYPNOWHEEL");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Hypnotic: %.1f", hypnoTime);
}
}
namespace e161 {
float idleTime = 0;
void reset(){idleTime=0;}
void drawIdleProtocol() {
  
  idleTime += 0.01;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(GREEN);
  
  // Simulate system idle activity monitoring
  for (int i = 0; i < gfx->width(); i += 4) {
    float activity = sin(idleTime * 3 + i * 0.02) * 0.5 + 0.5;
    int barHeight = activity * (gfx->height() / 3);
    
    uint16_t color = gfx->color565(
      activity * 100,
      255 - activity * 155,
      activity * 200
    );
    
    gfx->fillRect(i, gfx->height() - barHeight, 3, barHeight, color);
  }
  
  // Overlay text
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("IDLE PROTOCOL");
  
  gfx->setCursor(10, 30);
  gfx->printf("System Monitor");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Activity: %.1f%%", (sin(idleTime * 2) * 0.5 + 0.5) * 100);
}
}
namespace e162 {
float ifsTime = 0;
void reset(){ifsTime=0;}
void drawIfs() {
  
  ifsTime += 0.02;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(MAGENTA);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Iterated Function System fractal
  float x = 0.5, y = 0.5;
  
  for (int i = 0; i < 1000; i++) {
    // Simple IFS transformation
    int choice = random(3);
    float newX, newY;
    
    if (choice == 0) {
      newX = 0.5 * x;
      newY = 0.5 * y;
    } else if (choice == 1) {
      newX = 0.5 * x + 0.5;
      newY = 0.5 * y + 0.5;
    } else {
      newX = 0.5 * x + 0.25;
      newY = 0.5 * y + sin(ifsTime) * 0.1;
    }
    
    x = newX;
    y = newY;
    
    int screenX = centerX + (x - 0.5) * gfx->width() * 0.8;
    int screenY = centerY + (y - 0.5) * gfx->height() * 0.8;
    
    if (screenX >= 0 && screenX < gfx->width() && screenY >= 0 && screenY < gfx->height()) {
      uint16_t color = gfx->color565(
        (sin(ifsTime + x * 5) * 0.5 + 0.5) * 255,
        (cos(ifsTime + y * 5) * 0.5 + 0.5) * 255,
        (sin(ifsTime * 1.3 + (x + y) * 3) * 0.5 + 0.5) * 255
      );
      gfx->drawPixel(screenX, screenY, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("IFS");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Fractal System");
}
}
namespace e163 {
float mapTime = 0;
void reset(){mapTime=0;}
void drawImsmap() {
  
  mapTime += 0.02;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(YELLOW);
  
  // Image mapping simulation with distortion
  for (int y = 0; y < gfx->height(); y += 2) {
    for (int x = 0; x < gfx->width(); x += 2) {
      float u = (float)x / gfx->width();
      float v = (float)y / gfx->height();
      
      // Apply mapping distortion
      float distortX = u + sin(mapTime + v * 8) * 0.1;
      float distortY = v + cos(mapTime + u * 6) * 0.1;
      
      uint16_t color = gfx->color565(
        (sin(distortX * 10 + mapTime) * 0.5 + 0.5) * 255,
        (cos(distortY * 8 + mapTime * 1.2) * 0.5 + 0.5) * 255,
        (sin((distortX + distortY) * 6 + mapTime * 0.8) * 0.5 + 0.5) * 255
      );
      
      gfx->drawPixel(x, y, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("IMSMAP");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Image Mapping");
}
}
namespace e164 {
float aggTime = 0;
void reset(){aggTime=0;}
void drawInteraggregate() {
  
  aggTime += 0.03;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(CYAN);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Particle aggregation simulation
  for (int cluster = 0; cluster < 5; cluster++) {
    float clusterX = centerX + cos(aggTime * 0.5 + cluster * 1.3) * 60;
    float clusterY = centerY + sin(aggTime * 0.7 + cluster * 1.1) * 40;
    
    for (int particle = 0; particle < 20; particle++) {
      float angle = (float)particle / 20 * 2 * PI + aggTime * (1 + cluster * 0.2);
      float radius = 5 + sin(aggTime * 2 + particle * 0.3) * 15;
      
      int x = clusterX + cos(angle) * radius;
      int y = clusterY + sin(angle) * radius;
      
      if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
        uint16_t color = gfx->color565(
          (sin(aggTime + cluster * 0.8) * 0.5 + 0.5) * 255,
          (cos(aggTime * 1.2 + particle * 0.2) * 0.5 + 0.5) * 255,
          (sin(aggTime * 0.9 + cluster + particle * 0.1) * 0.5 + 0.5) * 255
        );
        gfx->fillCircle(x, y, 2, color);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("INTERAGGREGATE");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Particles: %.1f", aggTime);
}
}
namespace e165 {
float intTime = 0;
void reset(){intTime=0;}
void drawInterference() {
  
  intTime += 0.04;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Wave interference patterns
  for (int y = 0; y < gfx->height(); y += 2) {
    for (int x = 0; x < gfx->width(); x += 2) {
      float dist1 = sqrt(pow(x - centerX + 40, 2) + pow(y - centerY, 2));
      float dist2 = sqrt(pow(x - centerX - 40, 2) + pow(y - centerY, 2));
      
      float wave1 = sin(dist1 * 0.2 - intTime * 3);
      float wave2 = sin(dist2 * 0.2 - intTime * 3);
      float interference = (wave1 + wave2) * 0.5;
      
      uint16_t intensity = (interference * 0.5 + 0.5) * 255;
      uint16_t color = gfx->color565(
        intensity,
        intensity * 0.7,
        intensity * 0.9
      );
      
      gfx->drawPixel(x, y, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(YELLOW);
  gfx->printf("INTERFERENCE");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Wave Pattern");
}
}
namespace e166 {
float interTime = 0;
void reset(){interTime=0;}
void drawIntermomentary() {
  
  interTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  // Momentary interference patterns
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int i = 0; i < 200; i++) {
    float phase = interTime + i * 0.1;
    float x = centerX + sin(phase) * (50 + cos(phase * 2) * 30);
    float y = centerY + cos(phase) * (40 + sin(phase * 1.5) * 25);
    
    if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
      int intensity = 128 + sin(phase * 3) * 127;
      uint16_t color = gfx->color565(intensity, intensity / 2, intensity / 4);
      gfx->drawPixel(x, y, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(ORANGE);
  gfx->printf("INTERMOMENTARY");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Moment Flash");
}
}
namespace e167 {
float invTime = 0;
void reset(){invTime=0;}
void drawInvolute() {
  
  invTime += 0.03;
  
  gfx->fillScreen(BLACK);
  
  // Mathematical involute curves
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (float t = 0; t < 6.28 * 4; t += 0.05) {
    float radius = t + invTime;
    float x = centerX + (cos(t) + t * sin(t)) * 3;
    float y = centerY + (sin(t) - t * cos(t)) * 3;
    
    if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
      int hue = (int)(t * 50 + invTime * 100) % 360;
      uint16_t color = gfx->color565(
        128 + sin(hue * PI / 180) * 127,
        128 + sin((hue + 120) * PI / 180) * 127,
        128 + sin((hue + 240) * PI / 180) * 127
      );
      gfx->drawPixel(x, y, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(CYAN);
  gfx->printf("INVOLUTE");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Math Curves");
}
}
namespace e168 {
float jiggleTime = 0;
void reset(){jiggleTime=0;}
void drawJigglypuff() {
  
  jiggleTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  // Bouncing pink spheres
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int puff = 0; puff < 5; puff++) {
    float angle = puff * 2.0 * PI / 5 + jiggleTime;
    float bounce = sin(jiggleTime * 3 + puff) * 20;
    float x = centerX + cos(angle) * (60 + bounce);
    float y = centerY + sin(angle) * (40 + bounce);
    
    // Draw jiggly sphere
    for (int r = 15; r > 0; r--) {
      float wobble = sin(jiggleTime * 4 + r * 0.3) * 2;
      int intensity = 255 - r * 8;
      uint16_t color = gfx->color565(intensity, intensity / 3, intensity / 2);
      
      for (int i = 0; i < 20; i++) {
        float a = i * PI / 10;
        int px = x + (cos(a) * (r + wobble));
        int py = y + (sin(a) * (r + wobble));
        if (px >= 0 && px < gfx->width() && py >= 0 && py < gfx->height()) {
          gfx->drawPixel(px, py, color);
        }
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(MAGENTA);
  gfx->printf("JIGGLYPUFF");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Bounce: %.0f%%", sin(jiggleTime * 2) * 50 + 50);
}
}
namespace e169 {
float jigsawTime = 0;
void reset(){jigsawTime=0;}
void drawJigsaw() {
  
  jigsawTime += 0.03;
  
  gfx->fillScreen(BLACK);
  
  // Create jigsaw puzzle pieces
  int pieceW = gfx->width() / 8;
  int pieceH = gfx->height() / 6;
  
  for (int px = 0; px < 8; px++) {
    for (int py = 0; py < 6; py++) {
      int baseX = px * pieceW;
      int baseY = py * pieceH;
      
      // Animated offset for pieces
      float offsetX = sin(jigsawTime + px * 0.5) * 3;
      float offsetY = cos(jigsawTime + py * 0.5) * 3;
      
      int x = baseX + offsetX;
      int y = baseY + offsetY;
      
      // Piece color based on position
      uint8_t r = static_cast<uint8_t>(constrain(double(((px + py) * 40) % 255),0.0,255.0));
      uint8_t g = static_cast<uint8_t>(constrain(double((px * 60) % 255),0.0,255.0));
      uint8_t b = static_cast<uint8_t>(constrain(double((py * 80) % 255),0.0,255.0));
      uint16_t color = gfx->color565(r, g, b);
      
      // Draw puzzle piece shape
      gfx->fillRect(x + 2, y + 2, pieceW - 4, pieceH - 4, color);
      
      // Add connecting tabs
      if (px > 0) {
        int tabY = y + pieceH / 2 - 3;
        gfx->fillRect(x - 2, tabY, 4, 6, color);
      }
      if (py > 0) {
        int tabX = x + pieceW / 2 - 3;
        gfx->fillRect(tabX, y - 2, 6, 4, color);
      }
      
      // Draw piece outline
      gfx->drawRect(x + 2, y + 2, pieceW - 4, pieceH - 4, WHITE);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(CYAN);
  gfx->printf("JIGSAW");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Pieces: %d", 8 * 6);
}
}
namespace e170 {
float juggleTime = 0;
void reset(){juggleTime=0;}
void drawJuggle() {
  
  juggleTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  // Juggler position
  int jugX = gfx->width() / 2;
  int jugY = gfx->height() - 30;
  
  // Draw juggler (simple stick figure)
  gfx->drawLine(jugX, jugY, jugX, jugY - 20, WHITE);
  gfx->fillCircle(jugX, jugY - 25, 3, WHITE);
  
  // Arms
  int armOffset = sin(juggleTime * 4) * 10;
  gfx->drawLine(jugX, jugY - 15, jugX - 15 + armOffset, jugY - 10, WHITE);
  gfx->drawLine(jugX, jugY - 15, jugX + 15 - armOffset, jugY - 10, WHITE);
  
  // Juggling balls
  for (int ball = 0; ball < 3; ball++) {
    float ballTime = juggleTime + ball * (2 * PI / 3);
    
    // Parabolic trajectory
    float t = fmod(ballTime, PI);
    float ballX = jugX + sin(ballTime * 2) * 40;
    float ballY = jugY - 40 - abs(sin(t) * 50);
    
    uint16_t ballColor = (ball == 0) ? RED : (ball == 1) ? GREEN : BLUE;
    gfx->fillCircle(ballX, ballY, 3, ballColor);
    
    // Ball trail
    for (int i = 1; i < 5; i++) {
      float prevT = t - i * 0.1;
      if (prevT > 0) {
        float prevX = jugX + sin((ballTime - i * 0.1) * 2) * 40;
        float prevY = jugY - 40 - abs(sin(prevT) * 50);
        gfx->drawPixel(prevX, prevY, gfx->color565(
          (ballColor >> 11) << 3,
          ((ballColor >> 5) & 0x3F) << 2,
          (ballColor & 0x1F) << 3
        ) >> (i * 2));
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(YELLOW);
  gfx->printf("JUGGLE");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Balls: 3");
}
}
namespace e171 {
float juggler3dTime = 0;
void reset(){juggler3dTime=0;}
void drawJuggler3d() {
  
  juggler3dTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // 3D juggling pattern - figure-8 in 3D space
  for (int ball = 0; ball < 4; ball++) {
    float ballTime = juggler3dTime + ball * (PI / 2);
    
    // 3D coordinates
    float x3d = sin(ballTime) * 60;
    float y3d = sin(ballTime * 2) * 40;
    float z3d = cos(ballTime) * 30;
    
    // Project to 2D
    float perspective = 200 / (200 + z3d);
    int screenX = centerX + (x3d * perspective);
    int screenY = centerY + (y3d * perspective);
    
    // Ball size based on depth
    int ballSize = 2 + (perspective * 3);
    
    // Color based on depth
    uint8_t intensity = static_cast<uint8_t>(constrain(double(100 + (z3d + 30) * 2),0.0,255.0));
    uint16_t ballColor = gfx->color565(
      (ball == 0) ? intensity : intensity / 3,
      (ball == 1) ? intensity : intensity / 3,
      (ball == 2) ? intensity : intensity / 3
    );
    
    gfx->fillCircle(screenX, screenY, ballSize, ballColor);
    
    // Draw 3D trajectory lines
    for (int i = 1; i < 10; i++) {
      float prevTime = ballTime - i * 0.1;
      float prevX3d = sin(prevTime) * 60;
      float prevY3d = sin(prevTime * 2) * 40;
      float prevZ3d = cos(prevTime) * 30;
      
      float prevPerspective = 200 / (200 + prevZ3d);
      int prevScreenX = centerX + (prevX3d * prevPerspective);
      int prevScreenY = centerY + (prevY3d * prevPerspective);
      
      if (i == 1) {
        gfx->drawLine(screenX, screenY, prevScreenX, prevScreenY, 
                     gfx->color565(intensity/4, intensity/4, intensity/4));
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(MAGENTA);
  gfx->printf("JUGGLER3D");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("3D Pattern");
}
}
namespace e172 {
float rotorTime = 0;
void reset(){rotorTime=0;}
void drawRotor() {
  
  rotorTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Rotating rotor blades
  for (int blade = 0; blade < 4; blade++) {
    float angle = rotorTime * 3 + blade * PI / 2;
    
    // Main rotor blade
    int x1 = centerX + cos(angle) * 80;
    int y1 = centerY + sin(angle) * 80;
    int x2 = centerX + cos(angle + PI) * 20;
    int y2 = centerY + sin(angle + PI) * 20;
    
    gfx->drawLine(x1, y1, x2, y2, WHITE);
    gfx->drawLine(x1+1, y1, x2+1, y2, WHITE);
    
    // Blade tip circles
    gfx->fillCircle(x1, y1, 3, RED);
  }
  
  // Central hub
  gfx->fillCircle(centerX, centerY, 8, YELLOW);
  gfx->drawCircle(centerX, centerY, 12, WHITE);
  
  // Motion blur effect
  for (int i = 1; i < 5; i++) {
    float prevAngle = rotorTime * 3 - i * 0.1;
    for (int blade = 0; blade < 4; blade++) {
      float angle = prevAngle + blade * PI / 2;
      int x = centerX + cos(angle) * 80;
      int y = centerY + sin(angle) * 80;
      gfx->drawPixel(x, y, gfx->color565(60 >> i, 60 >> i, 60 >> i));
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("ROTOR");
}
}
namespace e173 {
float rotzoomTime = 0;
void reset(){rotzoomTime=0;}
void drawRotzoomer() {
  
  rotzoomTime += 0.06;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Rotating and zooming pattern
  float zoom = 1.0 + sin(rotzoomTime * 0.8) * 0.5;
  float rotation = rotzoomTime * 1.5;
  
  for (int y = 0; y < gfx->height(); y += 4) {
    for (int x = 0; x < gfx->width(); x += 4) {
      // Transform coordinates
      float dx = (x - centerX) / zoom;
      float dy = (y - centerY) / zoom;
      
      // Rotate
      float rx = dx * cos(rotation) - dy * sin(rotation);
      float ry = dx * sin(rotation) + dy * cos(rotation);
      
      // Create pattern
      int pattern = (int)(sin(rx * 0.1) * sin(ry * 0.1) * 128) + 128;
      
      if (pattern > 100) {
        uint16_t color = gfx->color565(pattern, pattern/2, 255-pattern);
        gfx->drawPixel(x, y, color);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(CYAN);
  gfx->printf("ROTZOOMER");
}
}
// Stable slide 175: removed by screensaver policy.
namespace e175 {
float rubikCubeTime = 0;
void reset(){rubikCubeTime=0;}
void drawRubik() {
  
  rubikCubeTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Full Rubik's cube rotation
  float rotX = rubikCubeTime * 0.8;
  float rotY = rubikCubeTime * 1.2;
  
  // Draw 3x3x3 cube structure
  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      for (int z = -1; z <= 1; z++) {
        // 3D rotation
        float px = x * cos(rotY) - z * sin(rotY);
        float pz = x * sin(rotY) + z * cos(rotY);
        float py = y * cos(rotX) - pz * sin(rotX);
        pz = y * sin(rotX) + pz * cos(rotX);
        
        // Perspective projection
        if (pz > -2) {
          int screenX = centerX + (px * 40) / (pz + 3);
          int screenY = centerY + (py * 40) / (pz + 3);
          
          // Color based on position
          uint16_t color = gfx->color565(
            (x + 1) * 127,
            (y + 1) * 127,
            (z + 1) * 127
          );
          
          int cubeSize = 15 / (pz + 3);
          gfx->fillRect(screenX - cubeSize/2, screenY - cubeSize/2, 
                       cubeSize, cubeSize, color);
          gfx->drawRect(screenX - cubeSize/2, screenY - cubeSize/2, 
                       cubeSize, cubeSize, WHITE);
        }
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(RED);
  gfx->printf("RUBIK");
}
}
namespace e176 {
float sballsTime = 0;
void reset(){sballsTime=0;}
void drawSballs() {
  
  sballsTime += 0.07;
  
  gfx->fillScreen(BLACK);
  
  // Spinning balls in formation
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int ball = 0; ball < 8; ball++) {
    float angle = sballsTime + ball * PI / 4;
    float radius = 60 + sin(sballsTime * 2 + ball) * 20;
    
    int ballX = centerX + cos(angle) * radius;
    int ballY = centerY + sin(angle) * radius;
    
    // Ball with spin effect
    float spinAngle = sballsTime * 5 + ball;
    int ballSize = 8 + sin(spinAngle) * 3;
    
    uint16_t ballColor = gfx->color565(
      128 + sin(angle + sballsTime) * 127,
      128 + cos(angle + sballsTime * 1.3) * 127,
      128 + sin(angle * 2 + sballsTime * 0.7) * 127
    );
    
    gfx->fillCircle(ballX, ballY, ballSize, ballColor);
    
    // Highlight
    gfx->fillCircle(ballX - 2, ballY - 2, 2, WHITE);
    
    // Trail
    for (int t = 1; t < 6; t++) {
      float trailAngle = sballsTime - t * 0.1 + ball * PI / 4;
      float trailRadius = 60 + sin((sballsTime - t * 0.1) * 2 + ball) * 20;
      int trailX = centerX + cos(trailAngle) * trailRadius;
      int trailY = centerY + sin(trailAngle) * trailRadius;
      
      gfx->drawPixel(trailX, trailY, ballColor >> (t * 2));
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(YELLOW);
  gfx->printf("SBALLS");
}
}
namespace e177 {
float scooterTime = 0;
void reset(){scooterTime=0;}
void drawScooter() {
  
  scooterTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  int scooterX = 50 + sin(scooterTime * 0.5) * 100;
  int scooterY = gfx->height() / 2;
  
  // Scooter body
  gfx->fillRect(scooterX - 20, scooterY - 5, 40, 10, BLUE);
  
  // Wheels
  int wheelRadius = 8;
  float wheelSpin = scooterTime * 3;
  
  // Front wheel
  gfx->fillCircle(scooterX + 15, scooterY + 10, wheelRadius, WHITE);
  gfx->drawCircle(scooterX + 15, scooterY + 10, wheelRadius, BLACK);
  // Spokes
  for (int spoke = 0; spoke < 6; spoke++) {
    float spokeAngle = wheelSpin + spoke * PI / 3;
    int spokeX = scooterX + 15 + cos(spokeAngle) * (wheelRadius - 2);
    int spokeY = scooterY + 10 + sin(spokeAngle) * (wheelRadius - 2);
    gfx->drawLine(scooterX + 15, scooterY + 10, spokeX, spokeY, BLACK);
  }
  
  // Rear wheel
  gfx->fillCircle(scooterX - 15, scooterY + 10, wheelRadius, WHITE);
  gfx->drawCircle(scooterX - 15, scooterY + 10, wheelRadius, BLACK);
  for (int spoke = 0; spoke < 6; spoke++) {
    float spokeAngle = wheelSpin + spoke * PI / 3;
    int spokeX = scooterX - 15 + cos(spokeAngle) * (wheelRadius - 2);
    int spokeY = scooterY + 10 + sin(spokeAngle) * (wheelRadius - 2);
    gfx->drawLine(scooterX - 15, scooterY + 10, spokeX, spokeY, BLACK);
  }
  
  // Handle bars
  gfx->drawLine(scooterX + 10, scooterY - 5, scooterX + 10, scooterY - 20, WHITE);
  gfx->drawLine(scooterX + 5, scooterY - 20, scooterX + 15, scooterY - 20, WHITE);
  
  // Rider (stick figure)
  int riderX = scooterX - 5;
  int riderY = scooterY - 25;
  gfx->fillCircle(riderX, riderY, 4, YELLOW); // Head
  gfx->drawLine(riderX, riderY + 4, riderX, riderY + 20, WHITE); // Body
  gfx->drawLine(riderX, riderY + 10, riderX + 8, riderY - 15, WHITE); // Arms to handlebars
  gfx->drawLine(riderX, riderY + 20, riderX - 5, riderY + 35, WHITE); // Legs
  gfx->drawLine(riderX, riderY + 20, riderX + 10, riderY + 15, WHITE);
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(GREEN);
  gfx->printf("SCOOTER");
}
}
// Stable slide 179: removed by screensaver policy.
namespace e179 {
float shadeBobTime = 0;
void reset(){shadeBobTime=0;}
void drawShadebobs() {
  
  shadeBobTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  // Multiple shadebobs (metaballs with shading)
  for (int bob = 0; bob < 6; bob++) {
    float bobAngle = shadeBobTime + bob * PI / 3;
    int bobX = gfx->width()/2 + cos(bobAngle) * 80;
    int bobY = gfx->height()/2 + sin(bobAngle * 1.3) * 60;
    
    int bobRadius = 25 + sin(shadeBobTime * 2 + bob) * 10;
    
    // Draw metaball with gradient shading
    for (int r = bobRadius; r > 0; r -= 2) {
      int intensity = (r * 255) / bobRadius;
      uint16_t color = gfx->color565(
        intensity * (bob % 3 == 0 ? 1 : 0),
        intensity * (bob % 3 == 1 ? 1 : 0),
        intensity * (bob % 3 == 2 ? 1 : 0)
      );
      
      gfx->drawCircle(bobX, bobY, r, color);
    }
  }
  
  // Connecting lines between bobs
  for (int i = 0; i < 6; i++) {
    for (int j = i + 1; j < 6; j++) {
      float angle1 = shadeBobTime + i * PI / 3;
      float angle2 = shadeBobTime + j * PI / 3;
      
      int x1 = gfx->width()/2 + cos(angle1) * 80;
      int y1 = gfx->height()/2 + sin(angle1 * 1.3) * 60;
      int x2 = gfx->width()/2 + cos(angle2) * 80;
      int y2 = gfx->height()/2 + sin(angle2 * 1.3) * 60;
      
      float distance = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
      if (distance < 100) {
        uint16_t lineColor = gfx->color565(100, 100, 255 - distance * 2);
        gfx->drawLine(x1, y1, x2, y2, lineColor);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(CYAN);
  gfx->printf("SHADEBOBS");
}
}
// Stable slide 181: removed by screensaver policy.
namespace e181 {
float slideTime = 0;
void reset(){slideTime=0;}
void drawSlidescreen() {
  
  slideTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  // Sliding panels effect
  int numPanels = 8;
  int panelWidth = gfx->width() / numPanels;
  
  for (int panel = 0; panel < numPanels; panel++) {
    float panelOffset = slideTime + panel * 0.5;
    int slideY = sin(panelOffset) * 50;
    
    uint16_t panelColor = gfx->color565(
      (panel * 32) % 255,
      (panel * 64) % 255,
      (panel * 96) % 255
    );
    
    // Main panel
    gfx->fillRect(panel * panelWidth, gfx->height()/2 + slideY - 25, 
                  panelWidth - 2, 50, panelColor);
    
    // Panel border
    gfx->drawRect(panel * panelWidth, gfx->height()/2 + slideY - 25, 
                  panelWidth - 2, 50, WHITE);
    
    // Panel number
    gfx->setCursor(panel * panelWidth + 5, gfx->height()/2 + slideY - 5);
    gfx->setTextColor(BLACK);
    gfx->printf("%d", panel + 1);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(YELLOW);
  gfx->printf("SLIDESCREEN");
}
}
namespace e182 {
float slipTime = 0;
void reset(){slipTime=0;}
void drawSlip() {
  
  slipTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Slipping/sliding geometric shapes
  for (int shape = 0; shape < 6; shape++) {
    float shapeAngle = slipTime + shape * PI / 3;
    
    // Slip motion - elliptical with phase
    int shapeX = centerX + cos(shapeAngle) * 80 + sin(slipTime * 2 + shape) * 30;
    int shapeY = centerY + sin(shapeAngle * 1.3) * 60;
    
    uint16_t shapeColor = gfx->color565(
      128 + sin(slipTime + shape) * 127,
      128 + cos(slipTime * 1.5 + shape) * 127,
      128 + sin(slipTime * 2 + shape) * 127
    );
    
    int shapeSize = 15 + sin(slipTime * 3 + shape) * 8;
    
    if (shape % 2 == 0) {
      // Rectangles
      gfx->fillRect(shapeX - shapeSize/2, shapeY - shapeSize/2, 
                   shapeSize, shapeSize, shapeColor);
    } else {
      // Circles
      gfx->fillCircle(shapeX, shapeY, shapeSize/2, shapeColor);
    }
    
    // Motion trail
    for (int trail = 1; trail < 5; trail++) {
      float trailAngle = slipTime - trail * 0.1 + shape * PI / 3;
      int trailX = centerX + cos(trailAngle) * 80 + sin((slipTime - trail * 0.1) * 2 + shape) * 30;
      int trailY = centerY + sin(trailAngle * 1.3) * 60;
      
      gfx->drawPixel(trailX, trailY, shapeColor >> (trail * 2));
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("SLIP");
}
}
namespace e183 {
float sonarTime = 0;
void reset(){sonarTime=0;}
void drawSonar() {
  
  sonarTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Sonar sweep
  float sweepAngle = sonarTime * 2;
  
  // Sonar circles
  for (int ring = 1; ring <= 5; ring++) {
    int radius = ring * 30;
    gfx->drawCircle(centerX, centerY, radius, gfx->color565(0, 100, 0));
  }
  
  // Sweep line
  int sweepX = centerX + cos(sweepAngle) * 150;
  int sweepY = centerY + sin(sweepAngle) * 150;
  gfx->drawLine(centerX, centerY, sweepX, sweepY, gfx->color565(0, 255, 0));
  
  // Fade trail behind sweep
  for (int trail = 1; trail < 20; trail++) {
    float trailAngle = sweepAngle - trail * 0.1;
    int trailX = centerX + cos(trailAngle) * 150;
    int trailY = centerY + sin(trailAngle) * 150;
    
    uint16_t trailColor = gfx->color565(0, 255 - trail * 10, 0);
    gfx->drawLine(centerX, centerY, trailX, trailY, trailColor);
  }
  
  // Detected objects
  for (int obj = 0; obj < 4; obj++) {
    float objAngle = sonarTime * 0.3 + obj * PI / 2;
    int objDistance = 60 + sin(sonarTime + obj) * 40;
    int objX = centerX + cos(objAngle) * objDistance;
    int objY = centerY + sin(objAngle) * objDistance;
    
    // Pulse when sweep passes over
    float angleDiff = abs(sweepAngle - objAngle);
    if (angleDiff > PI) angleDiff = 2*PI - angleDiff;
    
    if (angleDiff < 0.3) {
      gfx->fillCircle(objX, objY, 5, gfx->color565(255, 255, 0));
      // Ping rings
      gfx->drawCircle(objX, objY, 8, gfx->color565(200, 200, 0));
      gfx->drawCircle(objX, objY, 12, gfx->color565(150, 150, 0));
    } else {
      gfx->fillCircle(objX, objY, 3, gfx->color565(100, 100, 100));
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(0, 255, 0));
  gfx->printf("SONAR");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Range: 150m");
}
}
namespace e184 {
float speedmineTime = 0;
void reset(){speedmineTime=0;}
void drawSpeedmine() {
  
  speedmineTime += 0.1;
  
  gfx->fillScreen(BLACK);
  
  // High-speed mining operation
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Drilling motion
  float drillSpeed = speedmineTime * 8;
  int drillX = centerX + sin(speedmineTime * 0.5) * 50;
  int drillY = centerY;
  
  // Drill bit
  for (int bit = 0; bit < 8; bit++) {
    float bitAngle = drillSpeed + bit * PI / 4;
    int bitX = drillX + cos(bitAngle) * 15;
    int bitY = drillY + sin(bitAngle) * 15;
    
    gfx->drawLine(drillX, drillY, bitX, bitY, YELLOW);
  }
  
  gfx->fillCircle(drillX, drillY, 8, gfx->color565(200, 200, 0));
  
  // Mining particles
  for (int particle = 0; particle < 20; particle++) {
    float pTime = speedmineTime + particle * 0.2;
    int pX = drillX + cos(pTime * 5) * (20 + particle * 3);
    int pY = drillY + sin(pTime * 3) * (10 + particle * 2);
    
    uint16_t pColor = gfx->color565(
      255 - particle * 10,
      200 - particle * 8,
      100 - particle * 4
    );
    
    gfx->fillCircle(pX, pY, 2, pColor);
  }
  
  // Ore deposits
  for (int ore = 0; ore < 6; ore++) {
    int oreX = 30 + ore * 40;
    int oreY = centerY + 50 + sin(speedmineTime + ore) * 20;
    
    uint16_t oreColor = (ore % 3 == 0) ? gfx->color565(200, 100, 0) :
                        (ore % 3 == 1) ? gfx->color565(100, 100, 200) :
                        gfx->color565(100, 200, 100);
    
    gfx->fillRect(oreX - 8, oreY - 5, 16, 10, oreColor);
    gfx->drawRect(oreX - 8, oreY - 5, 16, 10, WHITE);
  }
  
  // Speed indicator
  gfx->setCursor(10, 10);
  gfx->setTextColor(RED);
  gfx->printf("SPEEDMINE");
  
  gfx->setCursor(10, 25);
  gfx->printf("RPM: %d", (int)(drillSpeed * 100) % 9999);
}
}
namespace e185 {
float sphereTime = 0;
void reset(){sphereTime=0;}
void drawSphere() {
  
  sphereTime += 0.06;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // 3D sphere with latitude/longitude lines
  float rotX = sphereTime * 0.8;
  float rotY = sphereTime * 1.2;
  
  int sphereRadius = 80;
  
  // Draw latitude lines
  for (int lat = -4; lat <= 4; lat++) {
    float latAngle = lat * PI / 8;
    float latRadius = cos(latAngle) * sphereRadius;
    float latY = sin(latAngle) * sphereRadius;
    
    // Draw longitude segments
    for (int segment = 0; segment < 32; segment++) {
      float lon1 = segment * 2 * PI / 32;
      float lon2 = (segment + 1) * 2 * PI / 32;
      
      // Rotate points
      float x1 = cos(lon1) * latRadius;
      float z1 = sin(lon1) * latRadius;
      float x2 = cos(lon2) * latRadius;
      float z2 = sin(lon2) * latRadius;
      
      // Apply rotation
      float rx1 = x1 * cos(rotY) - z1 * sin(rotY);
      float rz1 = x1 * sin(rotY) + z1 * cos(rotY);
      float ry1 = latY * cos(rotX) - rz1 * sin(rotX);
      
      float rx2 = x2 * cos(rotY) - z2 * sin(rotY);
      float rz2 = x2 * sin(rotY) + z2 * cos(rotY);
      float ry2 = latY * cos(rotX) - rz2 * sin(rotX);
      
      // Project to screen
      if (rz1 > -sphereRadius && rz2 > -sphereRadius) {
        int sx1 = centerX + rx1;
        int sy1 = centerY + ry1;
        int sx2 = centerX + rx2;
        int sy2 = centerY + ry2;
        
        uint16_t lineColor = gfx->color565(100 + rz1, 100, 200 - rz1);
        gfx->drawLine(sx1, sy1, sx2, sy2, lineColor);
      }
    }
  }
  
  // Draw longitude lines
  for (int lon = 0; lon < 8; lon++) {
    float lonAngle = lon * PI / 4;
    
    for (int segment = 0; segment < 16; segment++) {
      float lat1 = (segment - 8) * PI / 16;
      float lat2 = (segment - 7) * PI / 16;
      
      float x1 = cos(lonAngle) * cos(lat1) * sphereRadius;
      float y1 = sin(lat1) * sphereRadius;
      float z1 = sin(lonAngle) * cos(lat1) * sphereRadius;
      
      float x2 = cos(lonAngle) * cos(lat2) * sphereRadius;
      float y2 = sin(lat2) * sphereRadius;
      float z2 = sin(lonAngle) * cos(lat2) * sphereRadius;
      
      // Rotate
      float rx1 = x1 * cos(rotY) - z1 * sin(rotY);
      float rz1 = x1 * sin(rotY) + z1 * cos(rotY);
      float ry1 = y1 * cos(rotX) - rz1 * sin(rotX);
      
      float rx2 = x2 * cos(rotY) - z2 * sin(rotY);
      float rz2 = x2 * sin(rotY) + z2 * cos(rotY);
      float ry2 = y2 * cos(rotX) - rz2 * sin(rotX);
      
      if (rz1 > -sphereRadius && rz2 > -sphereRadius) {
        int sx1 = centerX + rx1;
        int sy1 = centerY + ry1;
        int sx2 = centerX + rx2;
        int sy2 = centerY + ry2;
        
        uint16_t lineColor = gfx->color565(200, 100 + rz1, 100);
        gfx->drawLine(sx1, sy1, sx2, sy2, lineColor);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("SPHERE");
}
}
namespace e186 {
float spheremonicsTime = 0;
void reset(){spheremonicsTime=0;}
void drawSpheremonics() {
  
  spheremonicsTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Spherical harmonics visualization
  float rotation = spheremonicsTime * 0.5;
  
  // Draw spherical harmonic surface
  for (int theta = 0; theta < 32; theta++) {
    for (int phi = 0; phi < 16; phi++) {
      float thetaAngle = theta * 2 * PI / 32;
      float phiAngle = phi * PI / 16;
      
      // Spherical harmonic function (simplified)
      float harmonic = sin(3 * thetaAngle) * cos(2 * phiAngle) * sin(spheremonicsTime);
      float radius = 60 + harmonic * 20;
      
      // Convert to Cartesian
      float x = radius * sin(phiAngle) * cos(thetaAngle);
      float y = radius * sin(phiAngle) * sin(thetaAngle);
      float z = radius * cos(phiAngle);
      
      // Rotate
      float rx = x * cos(rotation) - z * sin(rotation);
      float rz = x * sin(rotation) + z * cos(rotation);
      
      // Project
      if (rz > -100) {
        int sx = centerX + rx * 0.8;
        int sy = centerY + y * 0.8;
        
        uint16_t color = gfx->color565(
          128 + harmonic * 127,
          128 + sin(thetaAngle + spheremonicsTime) * 127,
          128 + cos(phiAngle + spheremonicsTime) * 127
        );
        
        gfx->drawPixel(sx, sy, color);
        
        // Add some thickness
        if (abs(harmonic) > 0.3) {
          gfx->drawPixel(sx + 1, sy, color);
          gfx->drawPixel(sx, sy + 1, color);
        }
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(MAGENTA);
  gfx->printf("SPHEREMONICS");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Harmonic: Y3,2");
}
}
namespace e187 {
float spiralTime = 0;
void reset(){spiralTime=0;}
void drawSpiral() {
  
  spiralTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple spirals
  for (int spiral = 0; spiral < 3; spiral++) {
    float spiralOffset = spiral * 2 * PI / 3;
    
    // Draw spiral arms
    for (int point = 0; point < 200; point++) {
      float t = point * 0.1;
      float angle = t + spiralTime + spiralOffset;
      float radius = t * 2;
      
      if (radius < 120) {
        int x = centerX + cos(angle) * radius;
        int y = centerY + sin(angle) * radius;
        
        // Color changes along spiral
        uint16_t color = gfx->color565(
          128 + sin(t + spiralTime) * 127,
          128 + cos(t + spiralTime + spiralOffset) * 127,
          128 + sin(t * 2 + spiralTime) * 127
        );
        
        int dotSize = 1 + (int)(sin(t + spiralTime) * 2);
        gfx->fillCircle(x, y, dotSize, color);
      }
    }
  }
  
  // Central core
  gfx->fillCircle(centerX, centerY, 8, WHITE);
  
  // Pulsing ring
  int ringRadius = 15 + sin(spiralTime * 3) * 5;
  gfx->drawCircle(centerX, centerY, ringRadius, YELLOW);
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(CYAN);
  gfx->printf("SPIRAL");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Arms: 3");
}
}
namespace e188 {
float spotTime = 0;
void reset(){spotTime=0;}
void drawSpotlight() {
  
  spotTime += 0.06;
  
  gfx->fillScreen(BLACK);
  
  // Moving spotlight effect
  int spotX = gfx->width()/2 + sin(spotTime * 0.8) * 100;
  int spotY = gfx->height()/2 + cos(spotTime * 1.2) * 60;
  
  // Spotlight cone
  int spotRadius = 60;
  
  // Draw illuminated area with gradient
  for (int r = spotRadius; r > 0; r -= 3) {
    float intensity = (float)(spotRadius - r) / spotRadius;
    uint16_t color = gfx->color565(
      intensity * 255,
      intensity * 255,
      intensity * 200
    );
    
    gfx->drawCircle(spotX, spotY, r, color);
  }
  
  // Objects in scene (only visible when lit)
  struct Object {
    int x, y, size;
    uint16_t color = BLACK;
  };
  
  Object objects[] = {
    {80, 100, 15, RED},
    {200, 150, 20, GREEN},
    {120, 200, 12, BLUE},
    {250, 80, 18, YELLOW},
    {50, 180, 16, MAGENTA}
  };
  
  for (int i = 0; i < 5; i++) {
    float distance = sqrt((objects[i].x - spotX) * (objects[i].x - spotX) + 
                         (objects[i].y - spotY) * (objects[i].y - spotY));
    
    if (distance < spotRadius) {
      // Object is lit - draw with full brightness
      float brightness = (spotRadius - distance) / spotRadius;
      uint16_t litColor = gfx->color565(
        (((objects[i].color >> 11) & 0x1F) << 3) * brightness,
        (((objects[i].color >> 5) & 0x3F) << 2) * brightness,
        ((objects[i].color & 0x1F) << 3) * brightness
      );
      
      gfx->fillCircle(objects[i].x, objects[i].y, objects[i].size, litColor);
    } else {
      // Object in shadow - barely visible
      gfx->drawCircle(objects[i].x, objects[i].y, objects[i].size, 
                     gfx->color565(20, 20, 20));
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("SPOTLIGHT");
}
}
namespace e189 {
float sproingTime = 0;
void reset(){sproingTime=0;}
void drawSproingies() {
  
  sproingTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  // Bouncing spring creatures
  for (int sproing = 0; sproing < 5; sproing++) {
    float bouncePhase = sproingTime * 2 + sproing * PI / 2.5;
    
    int baseX = 30 + sproing * 50;
    int bounceHeight = abs(sin(bouncePhase)) * 80;
    int sproingY = gfx->height() - 30 - bounceHeight;
    
    // Spring body
    int springSegments = 12;
    for (int seg = 0; seg < springSegments; seg++) {
      float segmentRatio = (float)seg / springSegments;
      int segY = sproingY + segmentRatio * bounceHeight * 0.3;
      
      // Compression effect - spring gets squished when bouncing
      float compression = (1.0 - abs(sin(bouncePhase))) * 0.5 + 0.5;
      int segX = baseX + sin(segmentRatio * PI * 4 + sproingTime) * 8 * compression;
      
      uint16_t springColor = gfx->color565(100, 200, 100);
      if (seg > 0) {
        int prevX = baseX + sin((segmentRatio - 1.0/springSegments) * PI * 4 + sproingTime) * 8 * compression;
        int prevY = sproingY + (segmentRatio - 1.0/springSegments) * bounceHeight * 0.3;
        gfx->drawLine(prevX, prevY, segX, segY, springColor);
      }
      
      gfx->fillCircle(segX, segY, 2, springColor);
    }
    
    // Sproingies head
    int headX = baseX + sin(sproingTime + sproing) * 5;
    int headY = sproingY - 15;
    
    gfx->fillCircle(headX, headY, 8, YELLOW);
    
    // Eyes
    gfx->fillCircle(headX - 3, headY - 2, 2, BLACK);
    gfx->fillCircle(headX + 3, headY - 2, 2, BLACK);
    gfx->drawPixel(headX - 3, headY - 3, WHITE);
    gfx->drawPixel(headX + 3, headY - 3, WHITE);
    
    // Mouth
    if (bounceHeight > 40) { // Open mouth when bouncing high
      gfx->fillCircle(headX, headY + 2, 2, BLACK);
    } else {
      gfx->drawLine(headX - 2, headY + 2, headX + 2, headY + 2, BLACK);
    }
    
    // Ground impact effect
    if (bounceHeight < 5) {
      for (int particle = 0; particle < 6; particle++) {
        int pX = baseX + (particle - 3) * 8;
        int pY = gfx->height() - 25;
        gfx->drawPixel(pX, pY, gfx->color565(200, 100, 50));
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(GREEN);
  gfx->printf("SPROINGIES");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Boing! Boing!");
}
}
namespace e190 {
float stairsTime = 0;
void reset(){stairsTime=0;}
void drawStairs() {
  
  stairsTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  // 3D staircase with perspective
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  float rotation = stairsTime * 0.3;
  
  // Draw staircase steps
  for (int step = 0; step < 12; step++) {
    float stepZ = step * 15;
    float stepY = -step * 12;
    
    // Step corners in 3D space
    float corners[4][3] = {
      {-40, stepY, stepZ},
      {40, stepY, stepZ},
      {40, stepY - 12, stepZ},
      {-40, stepY - 12, stepZ}
    };
    
    // Project to 2D with rotation
    int projectedCorners[4][2];
    bool allVisible = true;
    
    for (int corner = 0; corner < 4; corner++) {
      float x = corners[corner][0];
      float y = corners[corner][1];
      float z = corners[corner][2];
      
      // Rotate around Y axis
      float rx = x * cos(rotation) - z * sin(rotation);
      float rz = x * sin(rotation) + z * cos(rotation);
      
      // Perspective projection
      if (rz > -200) {
        projectedCorners[corner][0] = centerX + (rx * 200) / (rz + 250);
        projectedCorners[corner][1] = centerY + (y * 200) / (rz + 250);
      } else {
        allVisible = false;
        break;
      }
    }
    
    if (allVisible) {
      // Step top face
      uint16_t stepColor = gfx->color565(
        100 + step * 10,
        150 - step * 5,
        200 - step * 8
      );
      
      // Draw step as quadrilateral (simplified as lines)
      gfx->drawLine(projectedCorners[0][0], projectedCorners[0][1],
                   projectedCorners[1][0], projectedCorners[1][1], stepColor);
      gfx->drawLine(projectedCorners[1][0], projectedCorners[1][1],
                   projectedCorners[2][0], projectedCorners[2][1], stepColor);
      gfx->drawLine(projectedCorners[2][0], projectedCorners[2][1],
                   projectedCorners[3][0], projectedCorners[3][1], stepColor);
      gfx->drawLine(projectedCorners[3][0], projectedCorners[3][1],
                   projectedCorners[0][0], projectedCorners[0][1], stepColor);
      
      // Step side face (darker)
      uint16_t sideColor = gfx->color565(
        (100 + step * 10) >> 1,
        (150 - step * 5) >> 1,
        (200 - step * 8) >> 1
      );
      
      // Side face bottom corners
      int sideBottom[2][2] = {
        {projectedCorners[1][0], projectedCorners[1][1] + 12},
        {projectedCorners[2][0], projectedCorners[2][1] + 12}
      };
      
      gfx->drawLine(projectedCorners[1][0], projectedCorners[1][1],
                   sideBottom[0][0], sideBottom[0][1], sideColor);
      gfx->drawLine(projectedCorners[2][0], projectedCorners[2][1],
                   sideBottom[1][0], sideBottom[1][1], sideColor);
      gfx->drawLine(sideBottom[0][0], sideBottom[0][1],
                   sideBottom[1][0], sideBottom[1][1], sideColor);
    }
  }
  
  // Person climbing stairs (stick figure)
  int personStep = (int)(stairsTime * 2) % 12;
  float personZ = personStep * 15;
  float personY = -personStep * 12 - 20;
  
  float px = 0;
  float pz = personZ;
  
  // Rotate person position
  float rpx = px * cos(rotation) - pz * sin(rotation);
  float rpz = px * sin(rotation) + pz * cos(rotation);
  
  if (rpz > -200) {
    int screenX = centerX + (rpx * 200) / (rpz + 250);
    int screenY = centerY + (personY * 200) / (rpz + 250);
    
    // Draw stick figure
    gfx->fillCircle(screenX, screenY, 3, YELLOW); // Head
    gfx->drawLine(screenX, screenY + 3, screenX, screenY + 15, WHITE); // Body
    gfx->drawLine(screenX, screenY + 8, screenX - 5, screenY + 5, WHITE); // Arms
    gfx->drawLine(screenX, screenY + 8, screenX + 5, screenY + 5, WHITE);
    gfx->drawLine(screenX, screenY + 15, screenX - 4, screenY + 25, WHITE); // Legs
    gfx->drawLine(screenX, screenY + 15, screenX + 4, screenY + 25, WHITE);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("STAIRS");
  
  gfx->setCursor(10, gfx->height() - 20);
  gfx->printf("Step: %d/12", personStep + 1);
}
}
// Stable slide 192: removed by screensaver policy.
namespace e192 {
float petriTime = 0;
void reset(){petriTime=0;}
void drawPetri() {
  
  petriTime += 0.02;
  gfx->fillScreen(BLACK);
  
  for (int i = 0; i < 25; i++) {
    float cellAge = petriTime + i * 0.3;
    int x = (int)(gfx->width()/2 + 80 * sin(cellAge + i));
    int y = (int)(gfx->height()/2 + 60 * cos(cellAge * 1.2 + i));
    int radius = (int)(8 + 6 * sin(cellAge * 2));
    uint16_t color = gfx->color565((int)(128 + 100 * sin(cellAge)), 
                                   (int)(150 + 80 * cos(cellAge * 1.5)), 
                                   100);
    gfx->fillCircle(x, y, radius, color);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(GREEN);
  gfx->printf("PETRI");
}
}
namespace e193 {
int phosphorLines[240];
bool phosphorInit = false;
void reset(){std::memset(phosphorLines,0,sizeof(phosphorLines));phosphorInit=false;}
void drawPhosphor() {
  
  
  if (!phosphorInit) {
    for (int i = 0; i < 240; i++) phosphorLines[i] = random(320);
    phosphorInit = true;
  }
  
  gfx->fillScreen(BLACK);
  for (int y = 0; y < 240; y++) {
    phosphorLines[y] = (phosphorLines[y] + random(-2, 3)) % 320;
    if (phosphorLines[y] < 0) phosphorLines[y] += 320;
    
    uint16_t color = gfx->color565(0, (int)(200 + 55 * sin(y * 0.1)), 0);
    gfx->drawPixel(phosphorLines[y], y, color);
    if (phosphorLines[y] > 0) gfx->drawPixel(phosphorLines[y]-1, y, gfx->color565(0, 100, 0));
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(GREEN);
  gfx->printf("PHOSPHOR");
}
}
// Stable slide 195: removed by screensaver policy.
namespace e195 {
float pieceTime = 0;
void reset(){pieceTime=0;}
void drawPiecewise() {
  
  pieceTime += 0.04;
  gfx->fillScreen(BLACK);
  
  for (int segment = 0; segment < 6; segment++) {
    float angle = segment * PI / 3 + pieceTime;
    int x1 = gfx->width()/2;
    int y1 = gfx->height()/2;
    int x2 = (int)(x1 + 80 * cos(angle));
    int y2 = (int)(y1 + 80 * sin(angle));
    
    for (int piece = 0; piece < 10; piece++) {
      float t = piece / 9.0;
      int px = (int)(x1 + t * (x2 - x1) + 15 * sin(pieceTime * 3 + piece));
      int py = (int)(y1 + t * (y2 - y1) + 10 * cos(pieceTime * 4 + piece));
      
      uint16_t color = gfx->color565((int)(255 * t), (int)(200 * (1-t)), 128);
      gfx->fillCircle(px, py, 3, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(YELLOW);
  gfx->printf("PIECEWISE");
}
}
// Stable slide 197: removed by screensaver policy.
namespace e197 {
float pipeTime = 0;
void reset(){pipeTime=0;}
void drawPipeobjs() {
  
  pipeTime += 0.05;
  gfx->fillScreen(BLACK);
  
  for (int pipe = 0; pipe < 5; pipe++) {
    float pipePhase = pipeTime + pipe * 1.2;
    int x = (int)(50 + pipe * 50 + 30 * sin(pipePhase));
    int y = (int)(gfx->height()/2 + 40 * cos(pipePhase * 1.5));
    int length = (int)(60 + 20 * sin(pipePhase * 2));
    
    uint16_t color = gfx->color565(100 + pipe * 30, 150, 200 - pipe * 20);
    
    // Vertical pipe
    gfx->fillRect(x-5, y-length/2, 10, length, color);
    // Horizontal connector
    gfx->fillRect(x-15, y-5, 30, 10, color);
    
    // Joint circles
    gfx->fillCircle(x, y-length/2, 8, gfx->color565(180, 180, 180));
    gfx->fillCircle(x, y+length/2, 8, gfx->color565(180, 180, 180));
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(CYAN);
  gfx->printf("PIPEOBJS");
}
}
// Stable slide 199: removed by screensaver policy.
namespace e199 {
float foldTime = 0;
void reset(){foldTime=0;}
void drawPlatonicfolding() {
  
  foldTime += 0.03;
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width()/2;
  int centerY = gfx->height()/2;
  
  for (int face = 0; face < 6; face++) {
    float faceAngle = face * PI / 3 + foldTime;
    float foldAmount = sin(foldTime * 2 + face) * 0.5 + 0.5;
    
    int x1 = (int)(centerX + 60 * cos(faceAngle) * foldAmount);
    int y1 = (int)(centerY + 60 * sin(faceAngle) * foldAmount);
    int x2 = (int)(centerX + 80 * cos(faceAngle + PI/3));
    int y2 = (int)(centerY + 80 * sin(faceAngle + PI/3));
    int x3 = (int)(centerX + 80 * cos(faceAngle - PI/3));
    int y3 = (int)(centerY + 80 * sin(faceAngle - PI/3));
    
    uint16_t color = gfx->color565((int)(255 * foldAmount), 
                                   (int)(200 * (1-foldAmount)), 
                                   (int)(150 + 100 * foldAmount));
    
    gfx->drawTriangle(x1, y1, x2, y2, x3, y3, color);
    gfx->drawLine(centerX, centerY, x1, y1, WHITE);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(MAGENTA);
  gfx->printf("PLATONICFOLDING");
}
}
// Stable slide 201: removed by screensaver policy.
namespace e201 {
float glTime = 0;
void reset(){glTime=0;}
void drawPolyhedraGl() {
  
  glTime += 0.06;
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width()/2;
  int centerY = gfx->height()/2;
  
  // Simulate 3D GL-style polyhedron
  for (int vertex = 0; vertex < 20; vertex++) {
    float vAngle = vertex * PI / 10 + glTime;
    float depth = sin(glTime * 2 + vertex) + 2;
    
    int x = (int)(centerX + 80 * cos(vAngle) / depth);
    int y = (int)(centerY + 60 * sin(vAngle) / depth);
    int size = (int)(8 / depth);
    
    uint16_t color = gfx->color565((int)(255 / depth), 
                                   (int)(200 / depth), 
                                   (int)(150 + 100 / depth));
    
    gfx->fillCircle(x, y, size, color);
    
    // Connect to center with depth-based alpha
    if (vertex % 3 == 0) {
      gfx->drawLine(centerX, centerY, x, y, gfx->color565((int)(100 / depth), (int)(150 / depth), (int)(200 / depth)));
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(GREEN);
  gfx->printf("POLYHEDRA-GL");
}
}
// Stable slide 203: removed by screensaver policy.
// Stable slide 204: removed by screensaver policy.
namespace e204 {
float ballX = 160, ballY = 120;
float ballVX = 2, ballVY = 1.5;
float paddle1Y = 100, paddle2Y = 100;
int score1 = 0, score2 = 0;
void reset(){ballX=160, ballY = 120;ballVX=2, ballVY = 1.5;paddle1Y=100, paddle2Y = 100;score1=0, score2 = 0;}
void drawPong() {
  
  
  
  
  
  gfx->fillScreen(BLACK);
  
  // Update ball
  ballX += ballVX;
  ballY += ballVY;
  
  // Ball collision with top/bottom
  if (ballY <= 5 || ballY >= gfx->height() - 5) ballVY = -ballVY;
  
  // Ball collision with paddles
  if (ballX <= 15 && ballY >= paddle1Y && ballY <= paddle1Y + 40) {
    ballVX = abs(ballVX);
  }
  if (ballX >= gfx->width() - 15 && ballY >= paddle2Y && ballY <= paddle2Y + 40) {
    ballVX = -abs(ballVX);
  }
  
  // Reset ball if out of bounds
  if (ballX < 0) { score2++; ballX = 160; ballVX = 2; }
  if (ballX > gfx->width()) { score1++; ballX = 160; ballVX = -2; }
  
  // AI paddles
  paddle1Y += (ballY - paddle1Y - 20) * 0.1;
  paddle2Y += (ballY - paddle2Y - 20) * 0.1;
  
  // Draw game
  gfx->fillRect(10, paddle1Y, 5, 40, WHITE);
  gfx->fillRect(gfx->width()-15, paddle2Y, 5, 40, WHITE);
  gfx->fillCircle(ballX, ballY, 3, WHITE);
  
  // Center line
  for (int i = 0; i < gfx->height(); i += 10) {
    gfx->fillRect(gfx->width()/2 - 1, i, 2, 5, WHITE);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("PONG %d-%d", score1, score2);
}
}
// Stable slide 206: removed by screensaver policy.
namespace e206 {
float powTime = 0;
void reset(){powTime=0;}
void drawPow2() {
  
  powTime += 0.07;
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width()/2;
  int centerY = gfx->height()/2;
  
  // Draw powers of 2 visualization
  for (int power = 0; power < 8; power++) {
    int value = 1 << power; // 2^power
    float angle = power * PI / 4 + powTime;
    float radius = 30 + power * 8;
    
    int x = (int)(centerX + radius * cos(angle));
    int y = (int)(centerY + radius * sin(angle));
    
    // Size based on power
    int nodeSize = 4 + power;
    
    uint16_t color = gfx->color565((power * 30) % 255, 
                                   (power * 50) % 255, 
                                   255 - (power * 25));
    
    gfx->fillCircle(x, y, nodeSize, color);
    
    // Draw value
    if (power < 5) { // Only show smaller values
      gfx->setCursor(x - 10, y - 4);
      gfx->setTextColor(WHITE);
      gfx->printf("%d", value);
    }
    
    // Connect to center
    gfx->drawLine(centerX, centerY, x, y, gfx->color565(100, 100, 150));
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(CYAN);
  gfx->printf("POW2");
}
}
// Stable slide 208: removed by screensaver policy.
// Stable slide 209: removed by screensaver policy.
// Stable slide 210: removed by screensaver policy.
namespace e210 {
float provTime = 0;
void reset(){provTime=0;}
void drawProvidence() {
  
  provTime += 0.04;
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width()/2;
  int centerY = gfx->height()/2;
  
  // All-seeing eye effect
  // Eye outline
  for (int i = 0; i < 360; i += 5) {
    float angle = i * PI / 180;
    float radius = 60 + 10 * sin(provTime * 3 + angle * 4);
    int x = (int)(centerX + radius * cos(angle));
    int y = (int)(centerY + radius * sin(angle) * 0.6);
    
    uint16_t color = gfx->color565((int)(200 + 55 * sin(provTime + angle)), 
                                   (int)(150 + 100 * cos(provTime * 1.5 + angle)), 
                                   100);
    gfx->drawPixel(x, y, color);
  }
  
  // Pupil
  int pupilSize = (int)(15 + 5 * sin(provTime * 4));
  gfx->fillCircle(centerX, centerY, pupilSize, BLACK);
  gfx->drawCircle(centerX, centerY, pupilSize, YELLOW);
  
  // Iris
  for (int ring = pupilSize; ring < 40; ring += 3) {
    uint16_t irisColor = gfx->color565((int)(100 + ring * 2), 
                                       (int)(200 - ring), 
                                       (int)(150 + ring));
    gfx->drawCircle(centerX, centerY, ring, irisColor);
  }
  
  // Radiating lines
  for (int ray = 0; ray < 12; ray++) {
    float rayAngle = ray * PI / 6 + provTime;
    int x1 = (int)(centerX + 80 * cos(rayAngle));
    int y1 = (int)(centerY + 48 * sin(rayAngle));
    int x2 = (int)(centerX + 120 * cos(rayAngle));
    int y2 = (int)(centerY + 72 * sin(rayAngle));
    
    gfx->drawLine(x1, y1, x2, y2, gfx->color565(255, 215, 0));
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(YELLOW);
  gfx->printf("PROVIDENCE");
}
}
namespace e211 {
float cpuTime = 0;
void reset(){cpuTime=0;}
void drawM6502() {
  
  cpuTime += 0.03;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(GREEN);
  
  // Simulate 6502 processor display
  for (int y = 0; y < gfx->height(); y += 12) {
    for (int x = 0; x < gfx->width(); x += 48) {
      if (random(100) < 30) {
        gfx->setCursor(x, y);
        gfx->printf("%04X", (int)(random(0xFFFF) + cpuTime * 100) & 0xFFFF);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(CYAN);
  gfx->printf("M6502");
}
}
// Stable slide 213: removed by screensaver policy.
// Stable slide 214: removed by screensaver policy.
namespace e214 {
float marchTime = 0;
void reset(){marchTime=0;}
void drawMarching() {
  
  marchTime += 0.05;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(RED);
  
  // Marching squares algorithm visualization
  for (int y = 0; y < gfx->height() - 16; y += 16) {
    for (int x = 0; x < gfx->width() - 16; x += 16) {
      float field = sin((x + marchTime * 20) * 0.05) * cos((y + marchTime * 15) * 0.05);
      
      if (field > 0) {
        gfx->fillRect(x, y, 16, 16, gfx->color565(255, (int)(field * 255), 0));
      } else {
        gfx->drawRect(x, y, 16, 16, gfx->color565(0, 0, abs((int)(field * 255))));
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(YELLOW);
  gfx->printf("MARCHING");
}
}
namespace e215 {
float mazeTime = 0;
void reset(){mazeTime=0;}
void drawMaze3d() {
  
  mazeTime += 0.02;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // 3D maze corridor effect
  for (int depth = 10; depth > 0; depth--) {
    float scale = depth * 0.1;
    int wallWidth = (int)(gfx->width() * scale);
    int wallHeight = (int)(gfx->height() * scale);
    
    uint16_t wallColor = gfx->color565(depth * 25, depth * 20, depth * 15);
    
    // Left wall
    gfx->drawLine(centerX - wallWidth/2, centerY - wallHeight/2, 
                  centerX - wallWidth/2, centerY + wallHeight/2, wallColor);
    // Right wall
    gfx->drawLine(centerX + wallWidth/2, centerY - wallHeight/2,
                  centerX + wallWidth/2, centerY + wallHeight/2, wallColor);
    // Top wall
    gfx->drawLine(centerX - wallWidth/2, centerY - wallHeight/2,
                  centerX + wallWidth/2, centerY - wallHeight/2, wallColor);
    // Bottom wall
    gfx->drawLine(centerX - wallWidth/2, centerY + wallHeight/2,
                  centerX + wallWidth/2, centerY + wallHeight/2, wallColor);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("MAZE3D");
}
}
// Stable slide 217: removed by screensaver policy.
namespace e217 {
float memTime = 0;
void reset(){memTime=0;}
void drawMemscroller() {
  
  memTime += 0.05;
  
  gfx->fillScreen(BLACK);
  gfx->setTextColor(GREEN);
  
  // Scrolling memory dump
  for (int line = 0; line < 20; line++) {
    int y = (line * 12 - (int)(memTime * 30)) % (gfx->height() + 24);
    if (y > -12 && y < gfx->height()) {
      gfx->setCursor(0, y);
      gfx->printf("%04X: ", ((int)memTime * 16 + line * 16) & 0xFFFF);
      
      for (int i = 0; i < 8; i++) {
        gfx->printf("%02X ", random(256));
      }
    }
  }
  
  gfx->setCursor(10, 200);
  gfx->setTextColor(CYAN);
  gfx->printf("MEMSCROLLER");
}
}
namespace e218 {
float mengerTime = 0;
void reset(){mengerTime=0;}
void drawMenger() {
  
  mengerTime += 0.02;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Menger sponge fractal approximation
  for (int level = 0; level < 4; level++) {
    int size = 120 - level * 25;
    float angle = mengerTime + level * PI/4;
    
    for (int face = 0; face < 6; face++) {
      float faceAngle = face * PI/3 + angle;
      int x = centerX + (int)(cos(faceAngle) * (size/2));
      int y = centerY + (int)(sin(faceAngle) * (size/2));
      
      uint16_t color = gfx->color565(255 - level * 50, level * 60, 128 + level * 30);
      
      if (level % 3 != 1) { // Menger sponge holes
        gfx->fillRect(x - size/6, y - size/6, size/3, size/3, color);
      }
      gfx->drawRect(x - size/2, y - size/2, size, size, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("MENGER");
}
}
namespace e219 {
float ballTime = 0;
void reset(){ballTime=0;}
void drawMetaballs() {
  
  ballTime += 0.03;
  
  gfx->fillScreen(BLACK);
  
  // Multiple metaballs
  for (int y = 0; y < gfx->height(); y += 2) {
    for (int x = 0; x < gfx->width(); x += 2) {
      float field = 0;
      
      // Calculate field from 4 moving balls
      for (int ball = 0; ball < 4; ball++) {
        float ballX = gfx->width()/2 + cos(ballTime + ball * PI/2) * 80;
        float ballY = gfx->height()/2 + sin(ballTime * 0.7 + ball * PI/2) * 60;
        
        float dist = sqrt((x - ballX) * (x - ballX) + (y - ballY) * (y - ballY));
        if (dist > 0) field += 1000 / (dist * dist);
      }
      
      if (field > 1) {
        uint16_t color = gfx->color565(
          min(255, (int)(field * 50)),
          min(255, (int)(field * 30)),
          min(255, (int)(field * 80))
        );
        gfx->drawPixel(x, y, color);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("METABALLS");
}
}
// Stable slide 221: removed by screensaver policy.
namespace e221 {
float blobTime = 0;
void reset(){blobTime=0;}
void drawMirrorblob() {
  
  blobTime += 0.03;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Mirrored blob across 4 quadrants
  for (int quadrant = 0; quadrant < 4; quadrant++) {
    float qAngle = quadrant * PI/2;
    int mirrorX = (quadrant % 2) ? -1 : 1;
    int mirrorY = (quadrant > 1) ? -1 : 1;
    
    for (int i = 0; i < 20; i++) {
      float angle = i * PI/10 + blobTime;
      float radius = 50 + sin(blobTime * 2 + i * 0.5) * 30;
      
      int x = centerX + mirrorX * (int)(cos(angle) * radius);
      int y = centerY + mirrorY * (int)(sin(angle) * radius);
      
      uint16_t color = gfx->color565(
        128 + (int)(sin(angle) * 127),
        128 + (int)(cos(angle * 1.3) * 127),
        128 + (int)(sin(angle * 0.7) * 127)
      );
      
      gfx->fillCircle(x, y, 8, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("MIRRORBLOB");
}
}
namespace e222 {
float moebiusTime = 0;
void reset(){moebiusTime=0;}
void drawMoebius() {
  
  moebiusTime += 0.02;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Möbius strip visualization
  for (int t = 0; t < 100; t++) {
    float u = t * 2 * PI / 100;
    float v = sin(moebiusTime + u * 3) * 0.3;
    
    // Möbius strip parametric equations (simplified for 2D)
    float x = (2 + v * cos(u/2)) * cos(u);
    float y = (2 + v * cos(u/2)) * sin(u);
    
    int screenX = centerX + (int)(x * 30);
    int screenY = centerY + (int)(y * 30);
    
    uint16_t color = gfx->color565(
      128 + (int)(sin(u) * 127),
      128 + (int)(cos(u * 1.3) * 127),
      255
    );
    
    if (screenX >= 0 && screenX < gfx->width() && screenY >= 0 && screenY < gfx->height()) {
      gfx->drawPixel(screenX, screenY, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("MOEBIUS");
}
}
namespace e223 {
float gearTime = 0;
void reset(){gearTime=0;}
void drawMoebiusgears() {
  
  gearTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Two interlocking gears in Möbius configuration
  for (int gear = 0; gear < 2; gear++) {
    float gearAngle = gearTime * (gear ? -1 : 1);
    int gearX = centerX + (gear ? 40 : -40);
    int gearY = centerY;
    
    // Draw gear teeth
    for (int tooth = 0; tooth < 12; tooth++) {
      float toothAngle = tooth * PI/6 + gearAngle;
      int innerRadius = 30;
      int outerRadius = 45;
      
      int x1 = gearX + (int)(cos(toothAngle) * innerRadius);
      int y1 = gearY + (int)(sin(toothAngle) * innerRadius);
      int x2 = gearX + (int)(cos(toothAngle) * outerRadius);
      int y2 = gearY + (int)(sin(toothAngle) * outerRadius);
      
      uint16_t color = gear ? CYAN : YELLOW;
      gfx->drawLine(x1, y1, x2, y2, color);
    }
    
    gfx->drawCircle(gearX, gearY, 30, gear ? CYAN : YELLOW);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("MOEBIUSGEARS");
}
}
namespace e224 {
float moireTime = 0;
void reset(){moireTime=0;}
void drawMoire2() {
  
  moireTime += 0.02;
  
  gfx->fillScreen(BLACK);
  
  // Second moiré pattern with different frequencies
  for (int y = 0; y < gfx->height(); y += 2) {
    for (int x = 0; x < gfx->width(); x += 2) {
      float pattern1 = sin((x + moireTime * 30) * 0.08) * sin((y + moireTime * 20) * 0.08);
      float pattern2 = sin((x - moireTime * 25) * 0.12) * sin((y - moireTime * 35) * 0.06);
      
      float interference = pattern1 * pattern2;
      
      if (interference > 0.3) {
        uint16_t intensity = (int)(interference * 255);
        gfx->drawPixel(x, y, gfx->color565(intensity, intensity/2, intensity));
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("MOIRE2");
}
}
namespace e225 {
float moireTime = 0;
void reset(){moireTime=0;}
void drawMoire() {
  
  moireTime += 0.03;
  
  gfx->fillScreen(BLACK);
  
  // Classic moiré interference pattern
  for (int y = 0; y < gfx->height(); y += 1) {
    for (int x = 0; x < gfx->width(); x += 1) {
      float pattern1 = sin((x + moireTime * 20) * 0.1);
      float pattern2 = sin((x + moireTime * 15) * 0.11) * sin((y + moireTime * 10) * 0.1);
      
      float interference = pattern1 * pattern2;
      
      if (interference > 0.5) {
        gfx->drawPixel(x, y, WHITE);
      } else if (interference < -0.5) {
        gfx->drawPixel(x, y, gfx->color565(100, 100, 255));
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(YELLOW);
  gfx->printf("MOIRE");
}
}
namespace e226 {
float morphTime = 0;
void reset(){morphTime=0;}
void drawMorph3d() {
  
  morphTime += 0.03;
  
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Morphing 3D object
  for (int vertex = 0; vertex < 12; vertex++) {
    float angle = vertex * PI/6 + morphTime;
    
    // Morph between cube and sphere
    float morphFactor = (sin(morphTime * 2) + 1) / 2;
    float cubeRadius = 60;
    float sphereRadius = 50 + sin(morphTime * 3) * 20;
    
    float radius = cubeRadius * (1 - morphFactor) + sphereRadius * morphFactor;
    
    // Add 3D rotation
    float x3d = cos(angle) * radius;
    float y3d = sin(angle) * radius;
    float z3d = sin(morphTime * 1.5 + vertex * 0.5) * 30;
    
    // Project to 2D
    int x = centerX + (int)(x3d * (200 / (200 + z3d)));
    int y = centerY + (int)(y3d * (200 / (200 + z3d)));
    
    uint16_t color = gfx->color565(
      128 + (int)(sin(angle) * 127),
      128 + (int)(cos(angle * 1.3) * 127),
      128 + (int)(z3d * 2)
    );
    
    gfx->fillCircle(x, y, 6, color);
    
    // Connect to next vertex
    if (vertex < 11) {
      float nextAngle = (vertex + 1) * PI/6 + morphTime;
      float nextRadius = cubeRadius * (1 - morphFactor) + sphereRadius * morphFactor;
      float nextX3d = cos(nextAngle) * nextRadius;
      float nextY3d = sin(nextAngle) * nextRadius;
      float nextZ3d = sin(morphTime * 1.5 + (vertex + 1) * 0.5) * 30;
      
      int nextX = centerX + (int)(nextX3d * (200 / (200 + nextZ3d)));
      int nextY = centerY + (int)(nextY3d * (200 / (200 + nextZ3d)));
      
      gfx->drawLine(x, y, nextX, nextY, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("MORPH3D");
}
}
namespace e227 {
float munchTime = 0;
void reset(){munchTime=0;}
void drawMunch() {
  
  munchTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  // Munching squares (classic bit manipulation pattern)
  for (int y = 0; y < gfx->height(); y += 4) {
    for (int x = 0; x < gfx->width(); x += 4) {
      int value = (x + (int)(munchTime * 10)) ^ (y + (int)(munchTime * 8));
      
      uint16_t color = gfx->color565(
        value & 255,
        (value >> 1) & 255,
        (value >> 2) & 255
      );
      
      gfx->fillRect(x, y, 4, 4, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("MUNCH");
}
}
// Stable slide 229: removed by screensaver policy.
namespace e229 {
int neurons[20][3];
bool initialized = false;
void reset(){std::memset(neurons,0,sizeof(neurons));initialized=false;}
void drawNerverot() {
  // Nerve rot effect - chaotic neural network
  gfx->fillScreen(BLACK);
  
   // x, y, charge
  
  
  if (!initialized) {
    for (int i = 0; i < 20; i++) {
      neurons[i][0] = random(20, gfx->width()-20);
      neurons[i][1] = random(20, gfx->height()-20);
      neurons[i][2] = random(0, 255);
    }
    initialized = true;
  }
  
  // Update neuron charges
  for (int i = 0; i < 20; i++) {
    neurons[i][2] = (neurons[i][2] + random(-30, 31)) % 256;
    if (neurons[i][2] < 0) neurons[i][2] = 0;
  }
  
  // Draw connections
  for (int i = 0; i < 20; i++) {
    for (int j = i+1; j < 20; j++) {
      int dx = neurons[i][0] - neurons[j][0];
      int dy = neurons[i][1] - neurons[j][1];
      int dist = sqrt(dx*dx + dy*dy);
      
      if (dist < 80 && (neurons[i][2] > 128 || neurons[j][2] > 128)) {
        int intensity = 255 - (dist * 3);
        if (intensity > 0) {
          gfx->drawLine(neurons[i][0], neurons[i][1], neurons[j][0], neurons[j][1], 
                       gfx->color565(intensity, 0, intensity/2));
        }
      }
    }
  }
  
  // Draw neurons
  for (int i = 0; i < 20; i++) {
    int size = 2 + neurons[i][2] / 64;
    uint16_t color = gfx->color565(neurons[i][2], neurons[i][2]/4, neurons[i][2]/2);
    gfx->fillCircle(neurons[i][0], neurons[i][1], size, color);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("NERVEROT");
}
}
// Stable slide 231: removed by screensaver policy.
namespace e231 {

void reset(){}
void drawNormals() {
  // Normals effect - 3D surface normal visualization
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 12; j++) {
      float x = (i - 7.5) * 15;
      float y = (j - 6) * 15;
      float z = sin(animationTime + x*0.1) * cos(animationTime*1.2 + y*0.1) * 20;
      
      // Calculate normal vector (simplified)
      float nx = cos(animationTime + x*0.1) * 0.1 * cos(animationTime*1.2 + y*0.1);
      float ny = sin(animationTime + x*0.1) * (-sin(animationTime*1.2 + y*0.1)) * 0.1;
      float nz = 1.0;
      
      // Normalize
      float len = sqrt(nx*nx + ny*ny + nz*nz);
      nx /= len; ny /= len; nz /= len;
      
      int px = centerX + x;
      int py = centerY + y + z*0.5;
      
      // Draw point
      gfx->drawPixel(px, py, WHITE);
      
      // Draw normal vector
      int ex = px + nx * 15;
      int ey = py + ny * 15;
      
      uint16_t color = gfx->color565((nz + 1) * 127, abs(nx) * 255, abs(ny) * 255);
      gfx->drawLine(px, py, ex, ey, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("NORMALS");
}
}
// Stable slide 233: removed by screensaver policy.
namespace e233 {

void reset(){}
void drawOverlay() {
  // Overlay effect - multiple layered patterns
  gfx->fillScreen(BLACK);
  
  // Layer 1: Background grid
  for (int x = 0; x < gfx->width(); x += 20) {
    for (int y = 0; y < gfx->height(); y += 20) {
      if ((x/20 + y/20) % 2 == 0) {
        gfx->fillRect(x, y, 20, 20, gfx->color565(20, 20, 40));
      }
    }
  }
  
  // Layer 2: Rotating circles
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int i = 0; i < 6; i++) {
    float angle = animationTime * 0.5 + i * PI / 3;
    int x = centerX + cos(angle) * 40;
    int y = centerY + sin(angle) * 30;
    
    gfx->drawCircle(x, y, 15, gfx->color565(100 + i*25, 150, 200 - i*20));
  }
  
  // Layer 3: Scanning lines
  for (int i = 0; i < 5; i++) {
    int lineY = (int)(sin(animationTime * 2 + i) * gfx->height()/2 + gfx->height()/2);
    for (int x = 0; x < gfx->width(); x += 3) {
      gfx->drawPixel(x, lineY, gfx->color565(255, 255, 0));
    }
  }
  
  // Layer 4: Crosshair overlay
  gfx->drawLine(centerX - 30, centerY, centerX + 30, centerY, RED);
  gfx->drawLine(centerX, centerY - 30, centerX, centerY + 30, RED);
  gfx->drawCircle(centerX, centerY, 25, RED);
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("OVERLAY");
}
}
namespace e234 {
int pacX = 50, pacY = 100;
int ghostX = 200, ghostY = 150;
float aiTimer = 0;
void reset(){pacX=50, pacY = 100;ghostX=200, ghostY = 150;aiTimer=0;}
void drawPacmanAI() {
  // Pacman AI effect - intelligent ghost movement
  gfx->fillScreen(BLACK);
  
  
  
  
  
  aiTimer += 0.1;
  
  // AI movement for ghost (chase Pacman)
  if (aiTimer > 1.0) {
    if (ghostX < pacX) ghostX += 2;
    else if (ghostX > pacX) ghostX -= 2;
    
    if (ghostY < pacY) ghostY += 2;
    else if (ghostY > pacY) ghostY -= 2;
    
    aiTimer = 0;
  }
  
  // Pacman movement (random walk)
  pacX += random(-3, 4);
  pacY += random(-3, 4);
  
  // Keep in bounds
  pacX = constrain(pacX, 20, gfx->width()-20);
  pacY = constrain(pacY, 20, gfx->height()-20);
  ghostX = constrain(ghostX, 20, gfx->width()-20);
  ghostY = constrain(ghostY, 20, gfx->height()-20);
  
  // Draw maze background
  for (int x = 0; x < gfx->width(); x += 40) {
    gfx->drawLine(x, 0, x, gfx->height(), gfx->color565(0, 0, 100));
  }
  for (int y = 0; y < gfx->height(); y += 40) {
    gfx->drawLine(0, y, gfx->width(), y, gfx->color565(0, 0, 100));
  }
  
  // Draw dots
  for (int x = 20; x < gfx->width(); x += 20) {
    for (int y = 20; y < gfx->height(); y += 20) {
      gfx->fillCircle(x, y, 2, WHITE);
    }
  }
  
  // Draw Pacman
  gfx->fillCircle(pacX, pacY, 12, YELLOW);
  int mouthX = pacX + cos(animationTime * 5) * 8;
  int mouthY = pacY + sin(animationTime * 5) * 8;
  gfx->drawLine(pacX, pacY, mouthX, mouthY, BLACK);
  
  // Draw Ghost with AI indicator
  gfx->fillCircle(ghostX, ghostY, 12, RED);
  gfx->fillCircle(ghostX - 4, ghostY - 4, 3, WHITE); // Eyes
  gfx->fillCircle(ghostX + 4, ghostY - 4, 3, WHITE);
  gfx->fillCircle(ghostX - 4, ghostY - 4, 1, BLACK);
  gfx->fillCircle(ghostX + 4, ghostY - 4, 1, BLACK);
  
  // AI decision visualization
  gfx->drawLine(ghostX, ghostY, pacX, pacY, gfx->color565(255, 100, 100));
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("PACMAN AI");
}
}
namespace e235 {
float pacmanX = 0;
float pacmanY = 120;
float mouthAngle = 0;
float ghost1X = -50, ghost2X = -80, ghost3X = -110;
void reset(){pacmanX=0;pacmanY=120;mouthAngle=0;ghost1X=-50, ghost2X = -80, ghost3X = -110;}
void drawPacman() {
  // Classic Pacman effect
  gfx->fillScreen(BLACK);
  
  
  
  
  
  pacmanX += 2;
  if (pacmanX > gfx->width() + 20) {
    pacmanX = -20;
    pacmanY = random(50, gfx->height() - 50);
  }
  
  mouthAngle += 0.3;
  
  // Draw dots in path
  for (int x = 0; x < gfx->width(); x += 15) {
    if (x > pacmanX + 20) {
      gfx->fillCircle(x, pacmanY, 2, WHITE);
    }
  }
  
  // Draw Pacman
  gfx->fillCircle(pacmanX, pacmanY, 15, YELLOW);
  
  // Mouth animation
  float mouthOpen = abs(sin(mouthAngle)) * 10 + 5;
  int mouthX1 = pacmanX + cos(PI/6) * mouthOpen;
  int mouthY1 = pacmanY - sin(PI/6) * mouthOpen;
  int mouthX2 = pacmanX + cos(-PI/6) * mouthOpen;
  int mouthY2 = pacmanY + sin(PI/6) * mouthOpen;
  
  gfx->drawLine(pacmanX, pacmanY, mouthX1, mouthY1, BLACK);
  gfx->drawLine(pacmanX, pacmanY, mouthX2, mouthY2, BLACK);
  gfx->drawLine(pacmanX, pacmanY, pacmanX + 15, pacmanY, BLACK);
  
  // Ghosts following
  
  ghost1X += 1.5;
  ghost2X += 1.3;
  ghost3X += 1.1;
  
  if (ghost1X > gfx->width() + 20) ghost1X = -50;
  if (ghost2X > gfx->width() + 20) ghost2X = -80;
  if (ghost3X > gfx->width() + 20) ghost3X = -110;
  
  // Draw ghosts
  uint16_t ghostColors[] = {RED, gfx->color565(255, 100, 255), gfx->color565(0, 255, 255)};
  float ghostXs[] = {ghost1X, ghost2X, ghost3X};
  
  for (int i = 0; i < 3; i++) {
    if (ghostXs[i] > -20) {
      gfx->fillCircle(ghostXs[i], pacmanY, 12, ghostColors[i]);
      gfx->fillCircle(ghostXs[i] - 4, pacmanY - 4, 2, WHITE);
      gfx->fillCircle(ghostXs[i] + 4, pacmanY - 4, 2, WHITE);
      gfx->fillCircle(ghostXs[i] - 4, pacmanY - 4, 1, BLACK);
      gfx->fillCircle(ghostXs[i] + 4, pacmanY - 4, 1, BLACK);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("PACMAN");
}
}
namespace e236 {
float levelPacX = 20, levelPacY = 20;
int direction = 0;
void reset(){levelPacX=20, levelPacY = 20;direction=0;}
void drawPacmanLevel() {
  // Pacman level effect - full maze view
  gfx->fillScreen(BLACK);
  
  // Draw maze walls
  uint16_t wallColor = gfx->color565(0, 0, 255);
  
  // Outer walls
  gfx->drawRect(10, 10, gfx->width()-20, gfx->height()-20, wallColor);
  
  // Internal maze structure
  for (int i = 0; i < 8; i++) {
    int x = 30 + i * 30;
    int y = 40 + (i % 3) * 40;
    
    if (i % 2 == 0) {
      gfx->fillRect(x, y, 15, 30, wallColor);
    } else {
      gfx->fillRect(x, y, 30, 15, wallColor);
    }
  }
  
  // Draw dots
  for (int x = 20; x < gfx->width()-20; x += 20) {
    for (int y = 20; y < gfx->height()-20; y += 20) {
      bool isWall = false;
      
      // Check if position is on a wall (simplified)
      for (int i = 0; i < 8; i++) {
        int wx = 30 + i * 30;
        int wy = 40 + (i % 3) * 40;
        
        if ((x >= wx && x <= wx+30 && y >= wy && y <= wy+30)) {
          isWall = true;
          break;
        }
      }
      
      if (!isWall) {
        if ((x + y) % 40 == 0) {
          gfx->fillCircle(x, y, 3, YELLOW); // Power pellet
        } else {
          gfx->fillCircle(x, y, 1, WHITE); // Regular dot
        }
      }
    }
  }
  
  // Moving Pacman
  
   // 0=right, 1=down, 2=left, 3=up
  
  switch (direction) {
    case 0: levelPacX += 1; break;
    case 1: levelPacY += 1; break;
    case 2: levelPacX -= 1; break;
    case 3: levelPacY -= 1; break;
  }
  
  // Change direction randomly
  if (random(0, 100) < 5) {
    direction = random(0, 4);
  }
  
  // Keep in bounds
  if (levelPacX < 20) { levelPacX = 20; direction = 0; }
  if (levelPacX > gfx->width()-20) { levelPacX = gfx->width()-20; direction = 2; }
  if (levelPacY < 20) { levelPacY = 20; direction = 1; }
  if (levelPacY > gfx->height()-20) { levelPacY = gfx->height()-20; direction = 3; }
  
  gfx->fillCircle(levelPacX, levelPacY, 8, YELLOW);
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("PACMAN LEVEL");
}
}
// Stable slide 238: removed by screensaver policy.
// Stable slide 239: removed by screensaver policy.
namespace e239 {

void reset(){}
void drawPasswdKerberos() {
  // Kerberos authentication effect
  gfx->fillScreen(BLACK);
  
  // Kerberos 3-way authentication visualization
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw authentication triangle
  int points[3][2] = {
    {centerX, centerY - 50},        // KDC (top)
    {centerX - 60, centerY + 30},   // Client (bottom left)
    {centerX + 60, centerY + 30}    // Server (bottom right)
  };
  
  // Draw connection lines with data flow animation
  for (int i = 0; i < 3; i++) {
    int next = (i + 1) % 3;
    
    // Animated data packets
    float progress = fmod(animationTime * 2 + i * 2, 6.0) / 6.0;
    int packetX = points[i][0] + (points[next][0] - points[i][0]) * progress;
    int packetY = points[i][1] + (points[next][1] - points[i][1]) * progress;
    
    gfx->drawLine(points[i][0], points[i][1], points[next][0], points[next][1], YELLOW);
    gfx->fillCircle(packetX, packetY, 3, RED);
  }
  
  // Draw nodes
  gfx->fillCircle(points[0][0], points[0][1], 15, gfx->color565(100, 100, 255)); // KDC
  gfx->fillCircle(points[1][0], points[1][1], 12, GREEN);   // Client
  gfx->fillCircle(points[2][0], points[2][1], 12, gfx->color565(255, 100, 100)); // Server
  
  // Labels
  gfx->setCursor(points[0][0] - 15, points[0][1] - 25);
  gfx->setTextColor(WHITE);
  gfx->printf("KDC");
  
  gfx->setCursor(points[1][0] - 25, points[1][1] + 20);
  gfx->printf("CLIENT");
  
  gfx->setCursor(points[2][0] - 10, points[2][1] + 20);
  gfx->printf("SERVER");
  
  // Authentication steps
  gfx->setCursor(10, gfx->height() - 40);
  int step = ((int)(animationTime * 2)) % 4;
  switch (step) {
    case 0:
      gfx->setTextColor(YELLOW);
      gfx->printf("1. Client -> KDC: Request TGT");
      break;
    case 1:
      gfx->setTextColor(GREEN);
      gfx->printf("2. KDC -> Client: TGT + Session Key");
      break;
    case 2:
      gfx->setTextColor(gfx->color565(255, 100, 255));
      gfx->printf("3. Client -> Server: Service Request");
      break;
    case 3:
      gfx->setTextColor(gfx->color565(100, 255, 100));
      gfx->printf("4. Server -> Client: Service Access");
      break;
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("KERBEROS");
}
}
// Stable slide 241: removed by screensaver policy.
namespace e241 {
float scrollOffset = 0;
void reset(){scrollOffset=0;}
void drawPasswdPwent() {
  // Password entry effect - /etc/passwd visualization
  gfx->fillScreen(BLACK);
  
  // Simulated /etc/passwd entries
  const char* users[] = {"root", "daemon", "bin", "sys", "admin", "user", "guest"};
  int userCount = 7;
  
  
  scrollOffset += 0.5;
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(GREEN);
  gfx->printf("/etc/passwd");
  
  for (int i = 0; i < userCount; i++) {
    int y = 40 + i * 25 - (int)scrollOffset % 175;
    
    if (y > -20 && y < gfx->height()) {
      // Username
      gfx->setCursor(10, y);
      gfx->setTextColor(YELLOW);
      gfx->printf("%s", users[i]);
      
      // UID/GID
      gfx->setCursor(80, y);
      gfx->setTextColor(WHITE);
      gfx->printf(":%d:%d:", 1000 + i, 1000 + i);
      
      // Home directory
      gfx->setCursor(140, y);
      gfx->setTextColor(gfx->color565(100, 200, 255));
      gfx->printf("/home/%s", users[i]);
      
      // Password hash visualization (animated)
      if (sin(animationTime + i) > 0) {
        gfx->setCursor(220, y);
        gfx->setTextColor(RED);
        gfx->printf("$6$...");
      }
    }
  }
  
  // Highlight current entry
  int highlightY = 40 + ((int)(animationTime * 3) % userCount) * 25 - (int)scrollOffset % 175;
  if (highlightY > 20 && highlightY < gfx->height() - 20) {
    gfx->drawRect(5, highlightY - 2, gfx->width() - 10, 20, GREEN);
  }
  
  // Security warning
  gfx->setCursor(10, gfx->height() - 20);
  gfx->setTextColor(RED);
  if (sin(animationTime * 4) > 0) {
    gfx->printf("WARNING: Password file access detected!");
  }
  
  gfx->setCursor(10, 25);
  gfx->setTextColor(WHITE);
  gfx->printf("PWENT");
}
}
// Stable slide 243: removed by screensaver policy.
// Stable slide 244: removed by screensaver policy.
namespace e244 {
float objects[8][3];
bool initialized = false;
void reset(){std::memset(objects,0,sizeof(objects));initialized=false;}
void drawPenetrate() {
  // Penetrate effect - objects passing through barriers
  gfx->fillScreen(BLACK);
  
  // Draw barriers
  for (int i = 0; i < 5; i++) {
    int x = 50 + i * 50;
    gfx->fillRect(x, 50, 10, gfx->height() - 100, gfx->color565(100, 100, 255));
  }
  
  // Penetrating objects
   // x, y, phase
  
  
  if (!initialized) {
    for (int i = 0; i < 8; i++) {
      objects[i][0] = random(-20, 20);
      objects[i][1] = random(60, gfx->height() - 60);
      objects[i][2] = i * 0.5;
    }
    initialized = true;
  }
  
  for (int i = 0; i < 8; i++) {
    objects[i][0] += 2;
    if (objects[i][0] > gfx->width() + 20) {
      objects[i][0] = -20;
      objects[i][1] = random(60, gfx->height() - 60);
    }
    
    int x = (int)objects[i][0];
    int y = (int)objects[i][1];
    
    // Check if object is intersecting with barrier
    bool penetrating = false;
    for (int b = 0; b < 5; b++) {
      int barrierX = 50 + b * 50;
      if (x >= barrierX && x <= barrierX + 10) {
        penetrating = true;
        break;
      }
    }
    
    // Draw object with penetration effect
    if (penetrating) {
      // Penetration visualization - object becomes translucent and distorted
      for (int px = -6; px <= 6; px++) {
        for (int py = -6; py <= 6; py++) {
          if (px*px + py*py <= 36) {
            int alpha = 128 + sin(animationTime * 8 + px + py) * 64;
            uint16_t color = gfx->color565(alpha, alpha/2, 0);
            gfx->drawPixel(x + px + sin(animationTime * 10) * 2, 
                          y + py + cos(animationTime * 12) * 2, color);
          }
        }
      }
      
      // Particle effect during penetration
      for (int p = 0; p < 5; p++) {
        int px = x + random(-15, 16);
        int py = y + random(-15, 16);
        gfx->drawPixel(px, py, gfx->color565(255, 255, 0));
      }
    } else {
      // Normal solid object
      gfx->fillCircle(x, y, 6, RED);
      gfx->drawCircle(x, y, 6, WHITE);
    }
    
    // Trail effect
    for (int t = 1; t < 5; t++) {
      int trailX = x - t * 8;
      int trailAlpha = 255 - t * 50;
      if (trailAlpha > 0 && trailX > 0) {
        gfx->fillCircle(trailX, y, 6 - t, gfx->color565(trailAlpha, 0, 0));
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("PENETRATE");
}
}
// Stable slide 246: removed by screensaver policy.
namespace e246 {

void reset(){}
void drawPulsar() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  float pulse = sin(animationTime * 2) * 0.5 + 0.5;
  int radius = pulse * 80 + 20;
  gfx->fillCircle(centerX, centerY, radius, gfx->color565(255 * pulse, 100, 255 - 255 * pulse));
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("PULSAR");
}
}
namespace e247 {

void reset(){}
void drawPyro() {
  gfx->fillScreen(BLACK);
  for (int i = 0; i < 15; i++) {
    int x = (sin(animationTime + i) * 100) + gfx->width() / 2;
    int y = (cos(animationTime * 1.5 + i) * 80) + gfx->height() / 2;
    int size = abs(sin(animationTime * 2 + i)) * 15;
    gfx->fillCircle(x, y, size, gfx->color565(255, 200 - i * 10, 0));
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("PYRO");
}
}
namespace e248 {
int lastX = 160, lastY = 120;
void reset(){lastX=160, lastY = 120;}
void drawQix() {
  gfx->fillScreen(BLACK);
  
  int x = sin(animationTime * 1.5) * 120 + gfx->width() / 2;
  int y = cos(animationTime * 2.3) * 90 + gfx->height() / 2;
  gfx->drawLine(lastX, lastY, x, y, gfx->color565(255, 255 * sin(animationTime), 255 * cos(animationTime)));
  lastX = x; lastY = y;
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("QIX");
}
}
// Stable slide 250: removed by screensaver policy.
// Stable slide 251: removed by screensaver policy.
namespace e251 {

void reset(){}
void drawQuickhull() {
  gfx->fillScreen(BLACK);
  int points[][2] = {{50, 50}, {270, 80}, {200, 200}, {80, 180}, {160, 120}};
  for (int i = 0; i < 5; i++) {
    int x = points[i][0] + sin(animationTime + i) * 20;
    int y = points[i][1] + cos(animationTime + i) * 15;
    gfx->fillCircle(x, y, 5, gfx->color565(255, 100, 100));
    if (i > 0) gfx->drawLine(points[i-1][0], points[i-1][1], x, y, WHITE);
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("QUICKHULL");
}
}
namespace e252 {

void reset(){}
void drawRaverhoop() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  for (int i = 0; i < 20; i++) {
    float angle = (i / 20.0) * 2 * PI + animationTime;
    int x = centerX + cos(angle) * (60 + sin(animationTime * 3 + i) * 20);
    int y = centerY + sin(angle) * (60 + sin(animationTime * 3 + i) * 20);
    gfx->fillCircle(x, y, 3, gfx->color565(255 * sin(animationTime + i), 255, 255 * cos(animationTime + i)));
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("RAVERHOOP");
}
}
namespace e253 {

void reset(){}
void drawRazzledazzle() {
  gfx->fillScreen(BLACK);
  for (int i = 0; i < 10; i++) {
    int x1 = sin(animationTime * (i + 1)) * 120 + gfx->width() / 2;
    int y1 = cos(animationTime * (i + 1)) * 90 + gfx->height() / 2;
    int x2 = sin(animationTime * (i + 2)) * 100 + gfx->width() / 2;
    int y2 = cos(animationTime * (i + 2)) * 70 + gfx->height() / 2;
    gfx->drawLine(x1, y1, x2, y2, gfx->color565(255 * (i % 2), 255 * ((i + 1) % 2), 255));
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("RAZZLEDAZZLE");
}
}
namespace e254 {

void reset(){}
void drawRdbomb() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  float explosion = sin(animationTime * 3) * 0.5 + 0.5;
  for (int i = 0; i < 12; i++) {
    float angle = (i / 12.0) * 2 * PI;
    int x = centerX + cos(angle) * explosion * 80;
    int y = centerY + sin(angle) * explosion * 80;
    gfx->fillCircle(x, y, 5 + explosion * 10, gfx->color565(255, 255 * (1 - explosion), 0));
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("RDBOMB");
}
}
// Stable slide 256: removed by screensaver policy.
namespace e256 {

void reset(){}
void drawRemote() {
  gfx->fillScreen(BLACK);
  gfx->fillRect(100, 80, 120, 80, gfx->color565(50, 50, 50));
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      int x = 110 + j * 25;
      int y = 90 + i * 20;
      if ((i + j + (int)animationTime) % 2 == 0) {
        gfx->fillRect(x, y, 15, 15, gfx->color565(255, 100, 100));
      }
    }
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("REMOTE");
}
}
namespace e257 {

void reset(){}
void drawResources() {
  gfx->fillScreen(BLACK);
  for (int i = 0; i < 8; i++) {
    int height = sin(animationTime + i) * 50 + 100;
    int x = i * 40;
    gfx->fillRect(x, gfx->height() - height, 35, height, gfx->color565(255, 255 - i * 30, i * 30));
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("RESOURCES");
}
}
namespace e258 {

void reset(){}
void drawRipples() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  for (int i = 0; i < 5; i++) {
    int radius = ((int)(animationTime * 50) + i * 30) % 150;
    int intensity = 255 - (radius * 2);
    if (intensity > 0) {
      gfx->drawCircle(centerX, centerY, radius, gfx->color565(0, intensity, 255));
    }
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("RIPPLES");
}
}
namespace e259 {

void reset(){}
void drawRobot() {
  gfx->fillScreen(BLACK);
  int x = gfx->width() / 2 + sin(animationTime) * 50;
  int y = gfx->height() / 2;
  gfx->fillRect(x - 20, y - 30, 40, 60, gfx->color565(150, 150, 150));
  gfx->fillRect(x - 15, y - 25, 10, 10, gfx->color565(255, 0, 0));
  gfx->fillRect(x + 5, y - 25, 10, 10, gfx->color565(255, 0, 0));
  gfx->fillRect(x - 5, y - 10, 10, 5, gfx->color565(255, 255, 255));
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("ROBOT");
}
}
namespace e260 {

void reset(){}
void drawRobotWireframe() {
  gfx->fillScreen(BLACK);
  int x = gfx->width() / 2;
  int y = gfx->height() / 2;
  gfx->drawRect(x - 20, y - 30, 40, 60, WHITE);
  gfx->drawRect(x - 15, y - 25, 10, 10, gfx->color565(255, 0, 0));
  gfx->drawRect(x + 5, y - 25, 10, 10, gfx->color565(255, 0, 0));
  gfx->drawLine(x - 20, y, x + 20, y, WHITE);
  gfx->drawLine(x, y - 30, x, y + 30, WHITE);
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("ROBOT_WIREFRAME");
}
}
namespace e261 {

void reset(){}
void drawRocks() {
  gfx->fillScreen(BLACK);
  for (int i = 0; i < 8; i++) {
    int x = (sin(animationTime * 0.5 + i) * 120) + gfx->width() / 2;
    int y = (cos(animationTime * 0.7 + i) * 90) + gfx->height() / 2;
    int size = 10 + i * 3;
    gfx->fillCircle(x, y, size, gfx->color565(100 + i * 15, 80 + i * 10, 60));
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("ROCKS");
}
}
namespace e262 {

void reset(){}
void drawRomanboy() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  for (int i = 0; i < 20; i++) {
    float t = i / 20.0 * 2 * PI + animationTime;
    int x = centerX + cos(t) * (50 + sin(t * 3) * 20);
    int y = centerY + sin(t) * (30 + cos(t * 2) * 15);
    gfx->fillCircle(x, y, 3, gfx->color565(255, 100 + sin(t) * 100, 200));
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("ROMANBOY");
}
}
namespace e263 {

void reset(){}
void drawRorschach() {
  gfx->fillScreen(BLACK);
  for (int x = 0; x < gfx->width() / 2; x += 5) {
    for (int y = 0; y < gfx->height(); y += 5) {
      if (sin((x + y) * 0.1 + animationTime) > 0.3) {
        int color = sin(animationTime + x + y) * 255;
        gfx->fillRect(x, y, 5, 5, gfx->color565(color, color, color));
        gfx->fillRect(gfx->width() - x - 5, y, 5, 5, gfx->color565(color, color, color));
      }
    }
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("RORSCHACH");
}
}
namespace e264 {

void reset(){}
void drawRotator() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  for (int i = 0; i < 8; i++) {
    float angle = animationTime + (i * PI / 4);
    int x1 = centerX + cos(angle) * 60;
    int y1 = centerY + sin(angle) * 60;
    int x2 = centerX + cos(angle) * 30;
    int y2 = centerY + sin(angle) * 30;
    gfx->drawLine(x1, y1, x2, y2, gfx->color565(255, 255 * sin(angle), 255 * cos(angle)));
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("ROTATOR");
}
}
namespace e265 {

void reset(){}
void drawShips() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  for (int i = 0; i < 5; i++) {
    float t = animationTime + i * 0.5;
    int x = centerX + sin(t) * 80;
    int y = centerY + cos(t * 1.3) * 40;
    gfx->fillTriangle(x, y, x-10, y+15, x+10, y+15, gfx->color565(100+i*30, 200, 255));
    gfx->drawLine(x, y-5, x, y-20, WHITE);
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("SHIPS");
}
}
namespace e266 {

void reset(){}
void drawSierpinski3d() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  for (int level = 0; level < 4; level++) {
    int size = 60 - level * 15;
    float offset = animationTime * (level + 1) * 0.2;
    for (int i = 0; i < 3; i++) {
      float angle = i * 2 * PI / 3 + offset;
      int x = centerX + cos(angle) * size;
      int y = centerY + sin(angle) * size;
      gfx->fillCircle(x, y, 4 - level, gfx->color565(255-level*40, 100+level*50, 200));
    }
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("SIERPINSKI3D");
}
}
namespace e267 {

void reset(){}
void drawSierpinski() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  float scale = 50 + sin(animationTime) * 20;
  for (int i = 0; i < 100; i++) {
    float x = random(-100, 100) / 100.0 * scale;
    float y = random(-100, 100) / 100.0 * scale;
    if ((int(x) ^ int(y)) & 1) {
      gfx->drawPixel(centerX + x, centerY + y, gfx->color565(255, 200, 100));
    }
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("SIERPINSKI");
}
}
namespace e268 {

void reset(){}
void drawSkullModel() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  float rotation = animationTime * 0.5;
  gfx->fillCircle(centerX, centerY-20, 25, WHITE);
  gfx->fillCircle(centerX-8, centerY-25, 4, BLACK);
  gfx->fillCircle(centerX+8, centerY-25, 4, BLACK);
  gfx->fillTriangle(centerX, centerY-15, centerX-3, centerY-8, centerX+3, centerY-8, BLACK);
  for (int i = 0; i < 6; i++) {
    int x = centerX - 10 + i * 4;
    gfx->drawLine(x, centerY-5, x, centerY, BLACK);
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("SKULL_MODEL");
}
}
namespace e269 {

void reset(){}
void drawSkulloop() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  for (int i = 0; i < 8; i++) {
    float angle = animationTime + i * PI / 4;
    int x = centerX + cos(angle) * (30 + sin(animationTime * 2) * 10);
    int y = centerY + sin(angle) * (30 + cos(animationTime * 2) * 10);
    gfx->fillCircle(x, y, 3, gfx->color565(255, 150, 50 + i * 20));
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("SKULLOOP");
}
}
// Stable slide 271: removed by screensaver policy.
namespace e271 {

void reset(){}
void drawSphereversionAnalytic() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  float phase = animationTime * 0.3;
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 20; j++) {
      float u = i / 20.0 * PI;
      float v = j / 20.0 * 2 * PI + phase;
      float x = sin(u) * cos(v) * 40;
      float y = sin(u) * sin(v) * 40;
      int px = centerX + x;
      int py = centerY + y;
      if (px >= 0 && px < gfx->width() && py >= 0 && py < gfx->height()) {
        gfx->drawPixel(px, py, gfx->color565(255 * sin(u), 255 * cos(v), 200));
      }
    }
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("SPHEREEVERSION_A");
}
}
namespace e272 {

void reset(){}
void drawSphereversion() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  float t = animationTime * 0.5;
  for (int i = 0; i < 15; i++) {
    float angle = i * 2 * PI / 15 + t;
    float radius = 30 + sin(t + i * 0.2) * 15;
    int x = centerX + cos(angle) * radius;
    int y = centerY + sin(angle) * radius;
    gfx->fillCircle(x, y, 4, gfx->color565(255, 100 + sin(angle) * 100, 150));
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("SPHEREVERSION");
}
}
namespace e273 {

void reset(){}
void drawSphereversionCorrugations() {
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  for (int x = 0; x < gfx->width(); x += 4) {
    for (int y = 0; y < gfx->height(); y += 4) {
      float dx = x - centerX;
      float dy = y - centerY;
      float dist = sqrt(dx*dx + dy*dy);
      float wave = sin(dist * 0.1 + animationTime) * sin(dx * 0.05 + animationTime * 0.3);
      if (wave > 0.3) {
        gfx->fillRect(x, y, 3, 3, gfx->color565(255 * wave, 100, 200 * wave));
      }
    }
  }
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("SPHEREVERSION_C");
}
}
namespace e274 {
float flapTime = 0;
void reset(){flapTime=0;}
void drawSplitflap() {
  
  flapTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  // Draw split-flap display board
  int boardX = 20, boardY = 30;
  int cellW = 25, cellH = 35;
  int cols = 10, rows = 6;
  
  // Board background
  gfx->fillRect(boardX-5, boardY-5, cols*cellW+10, rows*cellH+10, gfx->color565(40, 40, 40));
  
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      int x = boardX + col * cellW;
      int y = boardY + row * cellH;
      
      // Flap animation
      float flapPhase = flapTime + row * 0.2 + col * 0.1;
      int flipState = ((int)(flapPhase * 2)) % 4;
      
      // Draw flap cell
      gfx->drawRect(x, y, cellW-1, cellH-1, gfx->color565(80, 80, 80));
      
      // Animated character flip
      char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
      char currentChar = letters[((int)(flapPhase) + row + col) % 36];
      
      if (flipState < 2) {
        // Top half visible
        gfx->fillRect(x+1, y+1, cellW-3, cellH/2-2, gfx->color565(200, 180, 50));
        gfx->setCursor(x+6, y+8);
        gfx->setTextColor(BLACK);
        gfx->printf("%c", currentChar);
      } else {
        // Bottom half visible  
        gfx->fillRect(x+1, y+cellH/2+1, cellW-3, cellH/2-2, gfx->color565(160, 140, 40));
        gfx->setCursor(x+6, y+20);
        gfx->setTextColor(BLACK);
        gfx->printf("%c", currentChar);
      }
      
      // Split line
      gfx->drawLine(x+1, y+cellH/2, x+cellW-2, y+cellH/2, gfx->color565(20, 20, 20));
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(200, 180, 50));
  gfx->printf("SPLITFLAP");
}
}
namespace e275 {
float splodeTime = 0;
void reset(){splodeTime=0;}
void drawSplodesic() {
  
  splodeTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  // Geodesic explosion patterns
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int layer = 0; layer < 5; layer++) {
    float layerTime = splodeTime + layer * 0.3;
    float radius = 20 + layer * 25 + sin(layerTime) * 15;
    int segments = 6 + layer * 2;
    
    for (int seg = 0; seg < segments; seg++) {
      float angle = (seg * 2 * PI / segments) + layerTime * 0.5;
      float x = centerX + cos(angle) * radius;
      float y = centerY + sin(angle) * radius;
      
      // Exploding triangular facets
      float facetSize = 8 + sin(layerTime * 2 + seg) * 4;
      uint16_t color = gfx->color565(
        255 - layer * 40,
        100 + layer * 30,
        layer * 50 + sin(layerTime + seg) * 50
      );
      
      // Draw triangular facet
      for (int i = 0; i < 3; i++) {
        float triAngle = angle + i * 2 * PI / 3;
        int tx = x + cos(triAngle) * facetSize;
        int ty = y + sin(triAngle) * facetSize;
        gfx->drawLine(x, y, tx, ty, color);
      }
      
      // Center dot
      gfx->fillCircle(x, y, 2, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(255, 150, 100));
  gfx->printf("SPLODESIC");
}
}
namespace e276 {
float squiralTime = 0;
void reset(){squiralTime=0;}
void drawSquiral() {
  
  squiralTime += 0.03;
  
  gfx->fillScreen(BLACK);
  
  // Squiral (square spiral) pattern
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  float angle = 0;
  float radius = 2;
  int x = centerX, y = centerY;
  int lastX = x, lastY = y;
  
  for (int i = 0; i < 150; i++) {
    // Square spiral increment
    angle += 0.1 + sin(squiralTime + i * 0.01) * 0.05;
    radius += 0.8;
    
    // Calculate position with square-like motion
    float squareAngle = floor(angle * 2 / PI) * PI / 2;
    float blend = (angle * 2 / PI) - floor(angle * 2 / PI);
    blend = blend * blend * (3 - 2 * blend); // smooth step
    
    float finalAngle = squareAngle + blend * PI / 2;
    
    x = centerX + cos(finalAngle) * radius;
    y = centerY + sin(finalAngle) * radius;
    
    if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
      uint16_t color = gfx->color565(
        (int)(128 + 127 * sin(squiralTime + i * 0.1)),
        (int)(128 + 127 * sin(squiralTime + i * 0.15 + PI/3)),
        (int)(128 + 127 * sin(squiralTime + i * 0.2 + 2*PI/3))
      );
      
      gfx->drawLine(lastX, lastY, x, y, color);
      gfx->fillCircle(x, y, 2, color);
    }
    
    lastX = x;
    lastY = y;
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(150, 255, 150));
  gfx->printf("SQUIRAL");
}
}
namespace e277 {
float torusTime = 0;
void reset(){torusTime=0;}
void drawSquirtorus() {
  
  torusTime += 0.06;
  
  gfx->fillScreen(BLACK);
  
  // Squirting torus effect
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int particle = 0; particle < 40; particle++) {
    float pTime = torusTime + particle * 0.15;
    
    // Torus parameters
    float majorR = 60;
    float minorR = 20 + sin(pTime * 3) * 15;
    float torusAngle = pTime * 2;
    float tubeAngle = particle * PI / 20 + pTime * 5;
    
    // Torus position
    float torusX = majorR * cos(torusAngle);
    float torusY = majorR * sin(torusAngle);
    
    // Tube position
    float tubeX = minorR * cos(tubeAngle);
    float tubeY = minorR * sin(tubeAngle);
    
    // Final position
    int x = centerX + torusX + tubeX;
    int y = centerY + torusY + tubeY;
    
    // Squirting jets
    if (sin(pTime * 4) > 0.7) {
      float jetLength = 30 + sin(pTime * 6) * 20;
      float jetAngle = tubeAngle;
      
      for (int jet = 0; jet < jetLength; jet += 2) {
        int jetX = x + cos(jetAngle) * jet;
        int jetY = y + sin(jetAngle) * jet;
        
        if (jetX >= 0 && jetX < gfx->width() && jetY >= 0 && jetY < gfx->height()) {
          uint16_t jetColor = gfx->color565(
            50 + jet * 4,
            100 + jet * 3,
            200 - jet * 2
          );
          gfx->drawPixel(jetX, jetY, jetColor);
        }
      }
    }
    
    // Torus particles
    if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
      uint16_t color = gfx->color565(
        (int)(150 + 105 * sin(pTime + particle)),
        (int)(100 + 80 * cos(pTime * 1.3 + particle)),
        (int)(200 + 55 * sin(pTime * 1.7 + particle))
      );
      gfx->fillCircle(x, y, 3, color);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(100, 200, 255));
  gfx->printf("SQUIRTORUS");
}
}
namespace e278 {
float starwarsTime = 0;
int textLine = 0;
void reset(){starwarsTime=0;textLine=0;}
void drawStarwars() {
  
  
  starwarsTime += 0.02;
  
  gfx->fillScreen(BLACK);
  
  // Star field
  for (int star = 0; star < 50; star++) {
    int x = (star * 37) % gfx->width();
    int y = (int)(star * 23 + starwarsTime * 10) % gfx->height();
    gfx->drawPixel(x, y, WHITE);
  }
  
  // Scrolling text effect
  const char* crawlText[] = {
    "A long time ago in a",
    "galaxy far, far away...",
    "",
    "Episode IV",
    "A NEW HOPE",
    "",
    "It is a period of civil war.",
    "Rebel spaceships, striking",
    "from a hidden base, have won",
    "their first victory against",
    "the evil Galactic Empire.",
    "",
    "During the battle, Rebel",
    "spies managed to steal secret",
    "plans to the Empire's ultimate",
    "weapon, the DEATH STAR..."
  };
  
  int numLines = sizeof(crawlText) / sizeof(crawlText[0]);
  
  for (int i = 0; i < 8; i++) {
    int lineIndex = (textLine + i) % numLines;
    int yPos = gfx->height() - 20 + i * 25 - (int)(starwarsTime * 30) % (25 * numLines);
    
    if (yPos > -15 && yPos < gfx->height() + 15) {
      // Perspective scaling
      float scale = (float)(gfx->height() - yPos) / gfx->height();
      scale = scale * scale; // More dramatic perspective
      
      if (scale > 0.1 && scale < 2.0) {
        gfx->setCursor(gfx->width()/2 - strlen(crawlText[lineIndex]) * 3, yPos);
        gfx->setTextSize(1);
        gfx->setTextColor(gfx->color565(255, 255, 100 + scale * 100));
        gfx->printf("%s", crawlText[lineIndex]);
      }
    }
  }
  
  // Update text line periodically
  if ((int)(starwarsTime * 10) % 100 == 0) {
    textLine = (textLine + 1) % numLines;
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(255, 255, 100));
  gfx->printf("STAR WARS");
}
}
// Stable slide 280: removed by screensaver policy.
namespace e280 {
float strangeTime = 0;
float x = 0.1, y = 0.1, z = 0.1;
void reset(){strangeTime=0;x=0.1, y = 0.1, z = 0.1;}
void drawStrange() {
  
  strangeTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  // Strange attractor visualization
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  
  
  for (int i = 0; i < 200; i++) {
    // Lorenz attractor equations
    float dt = 0.01;
    float sigma = 10.0;
    float rho = 28.0;
    float beta = 8.0/3.0;
    
    float dx = sigma * (y - x);
    float dy = x * (rho - z) - y;
    float dz = x * y - beta * z;
    
    x += dx * dt;
    y += dy * dt;  
    z += dz * dt;
    
    // Project 3D to 2D
    int plotX = centerX + (int)(x * 3 + sin(strangeTime) * 10);
    int plotY = centerY + (int)(y * 3 + cos(strangeTime) * 10);
    
    if (plotX >= 0 && plotX < gfx->width() && plotY >= 0 && plotY < gfx->height()) {
      uint16_t color = gfx->color565(
        (int)(128 + 127 * sin(strangeTime + i * 0.05)),
        (int)(128 + 127 * sin(strangeTime + i * 0.07 + PI/2)),
        (int)(128 + 127 * sin(strangeTime + i * 0.03 + PI))
      );
      gfx->drawPixel(plotX, plotY, color);
    }
  }
  
  // Reset periodically to prevent overflow
  if ((int)(strangeTime * 10) % 1000 == 0) {
    x = 0.1 + sin(strangeTime) * 0.05;
    y = 0.1 + cos(strangeTime) * 0.05;
    z = 0.1;
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(255, 100, 200));
  gfx->printf("STRANGE ATTRACTOR");
}
}
namespace e281 {
float substrateTime = 0;
void reset(){substrateTime=0;}
void drawSubstrate() {
  
  substrateTime += 0.03;
  
  gfx->fillScreen(gfx->color565(10, 10, 10));
  
  // Organic growth patterns like substrate art
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int crack = 0; crack < 8; crack++) {
    float crackTime = substrateTime + crack * 0.5;
    float crackAngle = crack * PI / 4 + crackTime * 0.1;
    
    // Main crack line
    float startRadius = 10;
    float endRadius = 80 + sin(crackTime) * 20;
    
    for (int step = 0; step < 50; step++) {
      float stepRatio = (float)step / 50;
      float radius = startRadius + stepRatio * endRadius;
      
      // Organic deviation
      float deviation = sin(crackTime + step * 0.2) * 15;
      float currentAngle = crackAngle + deviation * 0.1;
      
      int x = centerX + cos(currentAngle) * radius;
      int y = centerY + sin(currentAngle) * radius;
      
      if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
        // Main crack color
        uint16_t crackColor = gfx->color565(
          100 + crack * 15,
          80 + sin(crackTime + step * 0.1) * 50,
          60 + crack * 10
        );
        gfx->fillCircle(x, y, 2, crackColor);
        
        // Branch cracks
        if (step % 8 == 0) {
          for (int branch = 0; branch < 2; branch++) {
            float branchAngle = currentAngle + (branch - 0.5) * PI / 3;
            float branchLength = 20 + sin(crackTime + step + branch) * 10;
            
            for (int bStep = 0; bStep < 10; bStep++) {
              float bx = x + cos(branchAngle) * bStep * 2;
              float by = y + sin(branchAngle) * bStep * 2;
              
              if (bx >= 0 && bx < gfx->width() && by >= 0 && by < gfx->height()) {
                uint16_t branchColor = gfx->color565(
                  80 + crack * 10,
                  60 + bStep * 8,
                  40 + branch * 20
                );
                gfx->drawPixel(bx, by, branchColor);
              }
            }
          }
        }
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(150, 120, 80));
  gfx->printf("SUBSTRATE");
}
}
namespace e282 {
float sqTime = 0;
void reset(){sqTime=0;}
void drawSuperquadrics() {
  
  sqTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  // 2D projection of superquadric shapes
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple superquadrics with different parameters
  for (int shape = 0; shape < 3; shape++) {
    float shapeTime = sqTime + shape * PI / 1.5;
    float n1 = 2.0 + sin(shapeTime) * 1.5;      // Shape parameter 1
    float n2 = 2.0 + cos(shapeTime * 1.3) * 1.5; // Shape parameter 2
    float scale = 40 + shape * 20;
    
    int steps = 60;
    for (int i = 0; i < steps; i++) {
      float u = (i * 2.0 * PI) / steps;
      
      // Superquadric equations (2D cross-section)
      float cosU = cos(u);
      float sinU = sin(u);
      
      // Sign-preserving power function
      auto signPow = [](float val, float exp) {
        return (val >= 0) ? pow(val, exp) : -pow(-val, exp);
      };
      
      float x = signPow(abs(cosU), 2.0/n1) * ((cosU >= 0) ? 1 : -1);
      float y = signPow(abs(sinU), 2.0/n2) * ((sinU >= 0) ? 1 : -1);
      
      // 3D rotation projection
      float rotX = shapeTime * 0.5;
      float rotY = shapeTime * 0.7;
      
      float x3d = x * cos(rotY) - y * sin(rotX) * sin(rotY);
      float y3d = y * cos(rotX);
      
      int plotX = centerX + x3d * scale;
      int plotY = centerY + y3d * scale;
      
      if (plotX >= 0 && plotX < gfx->width() && plotY >= 0 && plotY < gfx->height()) {
        uint16_t color = gfx->color565(
          100 + shape * 50 + sin(shapeTime + i * 0.1) * 50,
          150 + cos(shapeTime + i * 0.15) * 80,
          200 - shape * 40 + sin(shapeTime + i * 0.08) * 40
        );
        gfx->fillCircle(plotX, plotY, 3, color);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(200, 150, 255));
  gfx->printf("SUPERQUADRICS");
}
}
namespace e283 {
float surfaceTime = 0;
void reset(){surfaceTime=0;}
void drawSurfaces() {
  
  surfaceTime += 0.03;
  
  gfx->fillScreen(BLACK);
  
  // Mathematical surface visualization
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Klein bottle projection
  for (int u = 0; u < 32; u++) {
    for (int v = 0; v < 16; v++) {
      float uf = (u * 2.0 * PI) / 32;
      float vf = (v * 2.0 * PI) / 16;
      
      float time_uf = uf + surfaceTime * 0.5;
      float time_vf = vf + surfaceTime * 0.3;
      
      // Klein bottle parametric equations (simplified)
      float x = (2 + cos(time_vf/2) * sin(time_uf) - sin(time_vf/2) * sin(2*time_uf)) * cos(time_vf/2);
      float y = (2 + cos(time_vf/2) * sin(time_uf) - sin(time_vf/2) * sin(2*time_uf)) * sin(time_vf/2);
      float z = sin(time_vf/2) * sin(time_uf) + cos(time_vf/2) * sin(2*time_uf);
      
      // 3D to 2D projection
      float scale = 15;
      int plotX = centerX + x * scale;
      int plotY = centerY + y * scale;
      
      if (plotX >= 0 && plotX < gfx->width() && plotY >= 0 && plotY < gfx->height()) {
        uint16_t color = gfx->color565(
          (int)(128 + z * 60),
          (int)(100 + sin(surfaceTime + uf) * 80),
          (int)(150 + cos(surfaceTime + vf) * 70)
        );
        gfx->fillCircle(plotX, plotY, 2, color);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(150, 200, 255));
  gfx->printf("SURFACES");
}
}
// Stable slide 285: removed by screensaver policy.
namespace e285 {
float swirlTime = 0;
void reset(){swirlTime=0;}
void drawSwirl() {
  
  swirlTime += 0.02;
  
  gfx->fillScreen(BLACK);
  
  // Color swirl patterns
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int layer = 0; layer < 5; layer++) {
    int numPoints = 30 + layer * 10;
    float layerRadius = 20 + layer * 25;
    
    for (int point = 0; point < numPoints; point++) {
      float baseAngle = (point * 2.0 * PI) / numPoints;
      
      // Swirl effect - angle increases with radius and time
      float swirlFactor = (swirlTime * 2) + (layerRadius * 0.05);
      float angle = baseAngle + swirlFactor;
      
      float radius = layerRadius + sin(swirlTime * 3 + point * 0.2) * 8;
      
      int x = centerX + cos(angle) * radius;
      int y = centerY + sin(angle) * radius;
      
      if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
        // Color cycling
        float hue = fmod(swirlTime * 50 + angle * 180/PI + layer * 60, 360);
        
        uint16_t color = BLACK;
        if (hue < 120) {
          color = gfx->color565(255 * (120 - hue) / 120, 255 * hue / 120, 0);
        } else if (hue < 240) {
          color = gfx->color565(0, 255 * (240 - hue) / 120, 255 * (hue - 120) / 120);
        } else {
          color = gfx->color565(255 * (hue - 240) / 120, 0, 255 * (360 - hue) / 120);
        }
        
        gfx->fillCircle(x, y, 3 - layer/2, color);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("SWIRL");
}
}
namespace e286 {
float t3dTime = 0;
void reset(){t3dTime=0;}
void drawT3d() {
  
  t3dTime += 0.06;
  
  gfx->fillScreen(BLACK);
  
  // 3D rotating objects
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw rotating wireframe cube
  float cubeSize = 40;
  
  // Cube vertices
  float vertices[8][3] = {
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
    {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}
  };
  
  // Rotation matrices
  float rotX = t3dTime * 0.7;
  float rotY = t3dTime * 0.5;
  float rotZ = t3dTime * 0.3;
  
  int projectedVerts[8][2];
  
  for (int i = 0; i < 8; i++) {
    float x = vertices[i][0] * cubeSize;
    float y = vertices[i][1] * cubeSize;
    float z = vertices[i][2] * cubeSize;
    
    // Rotate around X
    float y1 = y * cos(rotX) - z * sin(rotX);
    float z1 = y * sin(rotX) + z * cos(rotX);
    
    // Rotate around Y  
    float x2 = x * cos(rotY) + z1 * sin(rotY);
    float z2 = -x * sin(rotY) + z1 * cos(rotY);
    
    // Rotate around Z
    float x3 = x2 * cos(rotZ) - y1 * sin(rotZ);
    float y3 = x2 * sin(rotZ) + y1 * cos(rotZ);
    
    // Project to 2D
    projectedVerts[i][0] = centerX + x3;
    projectedVerts[i][1] = centerY + y3;
  }
  
  // Draw cube edges
  int edges[12][2] = {
    {0,1}, {1,2}, {2,3}, {3,0}, // bottom face
    {4,5}, {5,6}, {6,7}, {7,4}, // top face
    {0,4}, {1,5}, {2,6}, {3,7}  // vertical edges
  };
  
  for (int i = 0; i < 12; i++) {
    int v1 = edges[i][0];
    int v2 = edges[i][1];
    
    uint16_t edgeColor = gfx->color565(
      150 + sin(t3dTime + i) * 100,
      100 + cos(t3dTime + i * 1.2) * 80,
      200 + sin(t3dTime + i * 0.8) * 50
    );
    
    gfx->drawLine(
      projectedVerts[v1][0], projectedVerts[v1][1],
      projectedVerts[v2][0], projectedVerts[v2][1],
      edgeColor
    );
  }
  
  // Draw vertices
  for (int i = 0; i < 8; i++) {
    gfx->fillCircle(projectedVerts[i][0], projectedVerts[i][1], 3, 
      gfx->color565(255, 200, 100));
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(200, 150, 255));
  gfx->printf("T3D");
}
}
// Stable slide 288: removed by screensaver policy.
namespace e288 {
float teapotTime = 0;
void reset(){teapotTime=0;}
void drawTeapot() {
  
  teapotTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  // Classic Utah teapot wireframe
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Simplified teapot outline
  float rotation = teapotTime * 0.5;
  
  // Teapot body (ellipse)
  int bodyRadiusX = 50;
  int bodyRadiusY = 35;
  
  for (int angle = 0; angle < 360; angle += 10) {
    float rad = angle * PI / 180 + rotation;
    int x1 = centerX + cos(rad) * bodyRadiusX;
    int y1 = centerY + sin(rad) * bodyRadiusY;
    
    float rad2 = (angle + 10) * PI / 180 + rotation;
    int x2 = centerX + cos(rad2) * bodyRadiusX;
    int y2 = centerY + sin(rad2) * bodyRadiusY;
    
    uint16_t bodyColor = gfx->color565(150, 200, 255);
    gfx->drawLine(x1, y1, x2, y2, bodyColor);
  }
  
  // Teapot lid
  int lidY = centerY - 35;
  for (int angle = 0; angle < 360; angle += 15) {
    float rad = angle * PI / 180 + rotation;
    int x = centerX + cos(rad) * 35;
    int y = lidY + sin(rad) * 10;
    gfx->drawPixel(x, y, gfx->color565(200, 150, 100));
  }
  
  // Teapot handle
  float handleCenterX = centerX + 40;
  float handleCenterY = centerY;
  for (int angle = 30; angle < 330; angle += 10) {
    float rad = angle * PI / 180 + rotation * 0.3;
    int x = handleCenterX + cos(rad) * 25;
    int y = handleCenterY + sin(rad) * 30;
    gfx->fillCircle(x, y, 2, gfx->color565(100, 150, 200));
  }
  
  // Teapot spout
  float spoutStartX = centerX - 45;
  float spoutStartY = centerY - 10;
  for (int i = 0; i < 20; i++) {
    float t = i / 20.0;
    float spoutX = spoutStartX - t * 30;
    float spoutY = spoutStartY - t * t * 15; // Curved spout
    
    uint16_t spoutColor = gfx->color565(180, 120, 200);
    gfx->fillCircle(spoutX, spoutY, 3 - i/7, spoutColor);
  }
  
  // Teapot feet
  for (int foot = 0; foot < 3; foot++) {
    float footAngle = foot * 2 * PI / 3 + rotation;
    int footX = centerX + cos(footAngle) * 30;
    int footY = centerY + 35;
    gfx->fillCircle(footX, footY, 4, gfx->color565(100, 100, 150));
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(200, 150, 255));
  gfx->printf("TEAPOT");
}
}
// Stable slide 290: removed by screensaver policy.
namespace e290 {
float thornTime = 0;
float x = 0.1, y = 0.1;
void reset(){thornTime=0;x=0.1, y = 0.1;}
void drawThornbird() {
  
  thornTime += 0.01;
  
  gfx->fillScreen(BLACK);
  
  // Thornbird fractal (Bird in a Thornbush)
  // Parameters that vary over time
  float a = 1.99 + sin(thornTime * 0.3) * 0.1;
  float b = -1.0 + cos(thornTime * 0.2) * 0.2;
  float c = -0.6 + sin(thornTime * 0.4) * 0.1;
  
  
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Iterate the Thornbird equations
  for (int i = 0; i < 200; i++) {
    float newX = sin(y * b) + c * sin(x * b);
    float newY = sin(x * a) + c * sin(y * a);
    
    x = newX;
    y = newY;
    
    // Map to screen coordinates
    int plotX = centerX + x * 80;
    int plotY = centerY + y * 80;
    
    if (plotX >= 0 && plotX < gfx->width() && plotY >= 0 && plotY < gfx->height()) {
      // Color based on iteration and position
      uint16_t color = gfx->color565(
        (int)(128 + 127 * sin(thornTime + i * 0.05 + x)),
        (int)(128 + 127 * sin(thornTime + i * 0.07 + y)),
        (int)(128 + 127 * sin(thornTime + i * 0.03 + x + y))
      );
      
      gfx->fillCircle(plotX, plotY, 2, color);
      
      // Fade trail
      if (i > 100) {
        gfx->drawPixel(plotX, plotY, gfx->color565(50, 30, 70));
      }
    }
  }
  
  // Reset position periodically to prevent overflow
  if ((int)(thornTime * 100) % 500 == 0) {
    x = 0.1 + sin(thornTime) * 0.1;
    y = 0.1 + cos(thornTime) * 0.1;
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(255, 150, 100));
  gfx->printf("THORNBIRD");
}
}
namespace e291 {
float tunnelTime = 0;
void reset(){tunnelTime=0;}
void drawTimetunnel() {
  
  tunnelTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  // Time tunnel effect
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple concentric rings moving toward viewer
  for (int ring = 0; ring < 15; ring++) {
    float ringTime = tunnelTime + ring * 0.3;
    
    // Ring expands over time
    float baseRadius = 10 + fmod(ringTime * 40, 120);
    
    // Ring segments
    int segments = 8 + ring % 4;
    for (int seg = 0; seg < segments; seg++) {
      float angle = (seg * 2.0 * PI / segments) + ringTime * 0.5;
      
      // Pulsing radius
      float radius = baseRadius + sin(ringTime * 3 + seg) * 8;
      
      // Tunnel perspective - closer rings are brighter/larger
      float perspective = 1.0 / (1.0 + baseRadius * 0.01);
      
      int x = centerX + cos(angle) * radius;
      int y = centerY + sin(angle) * radius;
      
      if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
        // Color shifts through time spectrum
        float hue = fmod(tunnelTime * 100 + ring * 30 + seg * 45, 360);
        uint16_t color = BLACK;
        
        int brightness = 255 * perspective;
        
        if (hue < 120) {
          color = gfx->color565(
            brightness * (120 - hue) / 120, 
            brightness * hue / 120, 
            0
          );
        } else if (hue < 240) {
          color = gfx->color565(
            0,
            brightness * (240 - hue) / 120, 
            brightness * (hue - 120) / 120
          );
        } else {
          color = gfx->color565(
            brightness * (hue - 240) / 120, 
            0, 
            brightness * (360 - hue) / 120
          );
        }
        
        int dotSize = 1 + perspective * 4;
        gfx->fillCircle(x, y, dotSize, color);
        
        // Connect segments with lines for tunnel effect
        if (seg > 0) {
          float prevAngle = ((seg-1) * 2.0 * PI / segments) + ringTime * 0.5;
          int prevX = centerX + cos(prevAngle) * radius;
          int prevY = centerY + sin(prevAngle) * radius;
          gfx->drawLine(x, y, prevX, prevY, color);
        }
      }
    }
  }
  
  // Central vortex
  for (int vortex = 0; vortex < 20; vortex++) {
    float vortexTime = tunnelTime * 5 + vortex * 0.2;
    float vortexRadius = vortex * 2 + sin(vortexTime) * 3;
    float vortexAngle = vortexTime + vortex * 0.3;
    
    int vx = centerX + cos(vortexAngle) * vortexRadius;
    int vy = centerY + sin(vortexAngle) * vortexRadius;
    
    if (vx >= 0 && vx < gfx->width() && vy >= 0 && vy < gfx->height()) {
      gfx->fillCircle(vx, vy, 2, gfx->color565(255, 255, 200));
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(200, 255, 255));
  gfx->printf("TIMETUNNEL");
}
}
// Stable slide 293: removed by screensaver policy.
// Stable slide 294: removed by screensaver policy.
namespace e294 {
float blockTime = 0;
void reset(){blockTime=0;}
void drawTopblock() {
  
  blockTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  // Falling blocks puzzle game style
  int centerX = gfx->width() / 2;
  
  // Game grid
  int gridW = 12, gridH = 15;
  int blockSize = 12;
  int gridStartX = centerX - (gridW * blockSize) / 2;
  int gridStartY = 20;
  
  // Draw grid outline
  gfx->drawRect(gridStartX - 2, gridStartY - 2, gridW * blockSize + 4, gridH * blockSize + 4, WHITE);
  
  // Static blocks at bottom
  for (int x = 0; x < gridW; x++) {
    for (int y = gridH - 4; y < gridH; y++) {
      if ((x + y) % 3 != 0) {
        int blockX = gridStartX + x * blockSize;
        int blockY = gridStartY + y * blockSize;
        
        uint16_t blockColor = gfx->color565(
          100 + (x * 20) % 155,
          100 + (y * 30) % 155, 
          100 + ((x + y) * 15) % 155
        );
        gfx->fillRect(blockX, blockY, blockSize - 1, blockSize - 1, blockColor);
        gfx->drawRect(blockX, blockY, blockSize - 1, blockSize - 1, WHITE);
      }
    }
  }
  
  // Falling pieces
  for (int piece = 0; piece < 3; piece++) {
    float pieceTime = blockTime + piece * 2.0;
    int fallY = ((int)(pieceTime * 50)) % (gridH * blockSize + 60) - 60;
    int pieceX = gridStartX + (piece * 4 + 2) * blockSize;
    
    // Different piece shapes
    switch (piece) {
      case 0: // L-piece
        gfx->fillRect(pieceX, gridStartY + fallY, blockSize - 1, blockSize * 3 - 1, gfx->color565(255, 100, 100));
        gfx->fillRect(pieceX + blockSize, gridStartY + fallY + blockSize * 2, blockSize - 1, blockSize - 1, gfx->color565(255, 100, 100));
        break;
        
      case 1: // T-piece  
        gfx->fillRect(pieceX, gridStartY + fallY, blockSize * 3 - 1, blockSize - 1, gfx->color565(100, 255, 100));
        gfx->fillRect(pieceX + blockSize, gridStartY + fallY + blockSize, blockSize - 1, blockSize - 1, gfx->color565(100, 255, 100));
        break;
        
      case 2: // Square
        gfx->fillRect(pieceX, gridStartY + fallY, blockSize * 2 - 1, blockSize * 2 - 1, gfx->color565(100, 100, 255));
        break;
    }
  }
  
  // Score display
  gfx->setCursor(10, gfx->height() - 40);
  gfx->setTextColor(WHITE);
  gfx->printf("SCORE: %d", (int)(blockTime * 10) % 9999);
  
  gfx->setCursor(10, gfx->height() - 25);
  gfx->printf("LEVEL: %d", 1 + (int)(blockTime * 0.1) % 10);
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(255, 255, 100));
  gfx->printf("TOPBLOCK");
}
}
namespace e295 {
float ballTime = 0;
void reset(){ballTime=0;}
void drawTrackball() {
  
  ballTime += 0.06;
  
  gfx->fillScreen(BLACK);
  
  // 3D trackball visualization
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  int ballRadius = 60;
  
  // Ball rotation
  float rotX = ballTime * 0.7;
  float rotY = ballTime * 0.5;
  
  // Draw sphere with latitude/longitude lines
  for (int lat = -90; lat <= 90; lat += 30) {
    for (int lon = 0; lon < 360; lon += 10) {
      float latRad = lat * PI / 180;
      float lonRad = lon * PI / 180;
      
      // 3D sphere coordinates
      float x = cos(latRad) * cos(lonRad + rotY);
      float y = cos(latRad) * sin(lonRad + rotY);
      float z = sin(latRad);
      
      // Rotate around X
      float y2 = y * cos(rotX) - z * sin(rotX);
      float z2 = y * sin(rotX) + z * cos(rotX);
      
      // Project to 2D
      if (z2 > 0) { // Only draw front-facing points
        int plotX = centerX + x * ballRadius;
        int plotY = centerY + y2 * ballRadius;
        
        if (plotX >= 0 && plotX < gfx->width() && plotY >= 0 && plotY < gfx->height()) {
          // Brightness based on Z depth
          int brightness = 128 + z2 * 127;
          uint16_t color = BLACK;
          
          if (lat == 0) {
            color = gfx->color565(brightness, brightness/2, brightness/4); // Equator - red
          } else if (lon % 90 == 0) {
            color = gfx->color565(brightness/4, brightness, brightness/2); // Meridians - green
          } else {
            color = gfx->color565(brightness/2, brightness/2, brightness); // Other lines - blue
          }
          
          gfx->fillCircle(plotX, plotY, 2, color);
        }
      }
    }
  }
  
  // Trackball cursor
  float cursorX = centerX + sin(ballTime * 2) * 80;
  float cursorY = centerY + cos(ballTime * 1.3) * 60;
  gfx->fillCircle(cursorX, cursorY, 5, gfx->color565(255, 255, 100));
  gfx->drawCircle(cursorX, cursorY, 8, WHITE);
  
  // Axis indicators
  gfx->drawLine(centerX - 80, centerY, centerX + 80, centerY, gfx->color565(100, 100, 100));
  gfx->drawLine(centerX, centerY - 80, centerX, centerY + 80, gfx->color565(100, 100, 100));
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(200, 200, 255));
  gfx->printf("TRACKBALL");
}
}
namespace e296 {
float triTime = 0;
void reset(){triTime=0;}
void drawTriangle() {
  
  triTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  // Animated triangular patterns
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple triangle layers
  for (int layer = 0; layer < 6; layer++) {
    float layerTime = triTime + layer * 0.5;
    float radius = 20 + layer * 20;
    float rotation = layerTime * (0.5 + layer * 0.1);
    
    // Triangle vertices
    for (int tri = 0; tri < 3; tri++) {
      float angle1 = rotation + tri * 2 * PI / 3;
      float angle2 = rotation + (tri + 1) * 2 * PI / 3;
      
      int x1 = centerX + cos(angle1) * radius;
      int y1 = centerY + sin(angle1) * radius;
      int x2 = centerX + cos(angle2) * radius;
      int y2 = centerY + sin(angle2) * radius;
      
      uint16_t triColor = gfx->color565(
        128 + 127 * sin(layerTime + tri * 0.8),
        128 + 127 * sin(layerTime + tri * 0.8 + PI/2),
        128 + 127 * sin(layerTime + tri * 0.8 + PI)
      );
      
      // Draw triangle
      gfx->fillTriangle(centerX, centerY, x1, y1, x2, y2, triColor);
      gfx->drawTriangle(centerX, centerY, x1, y1, x2, y2, WHITE);
    }
  }
  
  // Sierpinski triangle elements
  for (int iter = 0; iter < 100; iter++) {
    float iterTime = triTime + iter * 0.1;
    
    // Sierpinski point calculation (simplified)
    float x = 160 + 50 * sin(iterTime * 3 + iter * 0.2);
    float y = 120 + 50 * cos(iterTime * 2.5 + iter * 0.15);
    
    uint16_t dotColor = gfx->color565(
      255 - iter,
      100 + iter,
      150 + sin(iterTime) * 50
    );
    
    if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
      gfx->drawPixel(x, y, dotColor);
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(255, 200, 100));
  gfx->printf("TRIANGLE");
}
}
namespace e297 {
float tronTime = 0;
void reset(){tronTime=0;}
void drawTronbit() {
  
  tronTime += 0.06;
  
  gfx->fillScreen(BLACK);
  
  // TRON-style digital world
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Grid lines
  for (int x = 0; x < gfx->width(); x += 20) {
    gfx->drawLine(x, 0, x, gfx->height(), gfx->color565(0, 50, 100));
  }
  for (int y = 0; y < gfx->height(); y += 20) {
    gfx->drawLine(0, y, gfx->width(), y, gfx->color565(0, 50, 100));
  }
  
  // Digital entities/bits
  for (int bit = 0; bit < 8; bit++) {
    float bitTime = tronTime + bit * 0.8;
    
    // Bit position
    float bitX = centerX + sin(bitTime * 2) * (40 + bit * 8);
    float bitY = centerY + cos(bitTime * 1.5) * (30 + bit * 6);
    
    // Bit state animation
    bool bitState = (sin(bitTime * 4) > 0);
    
    if (bitState) {
      // "1" bit - bright cyan cube
      gfx->fillRect(bitX - 4, bitY - 4, 8, 8, gfx->color565(0, 255, 255));
      gfx->drawRect(bitX - 4, bitY - 4, 8, 8, WHITE);
      
      // Data trails
      for (int trail = 1; trail < 6; trail++) {
        int trailX = bitX - cos(bitTime * 2) * trail * 8;
        int trailY = bitY - sin(bitTime * 1.5) * trail * 6;
        int brightness = 255 - trail * 40;
        gfx->fillRect(trailX - 2, trailY - 2, 4, 4, 
          gfx->color565(0, brightness/4, brightness/2));
      }
    } else {
      // "0" bit - dimmer orange
      gfx->fillRect(bitX - 3, bitY - 3, 6, 6, gfx->color565(100, 50, 0));
      gfx->drawRect(bitX - 3, bitY - 3, 6, 6, gfx->color565(150, 100, 50));
    }
  }
  
  // Central processing unit
  int cpuSize = 20 + sin(tronTime * 3) * 5;
  gfx->fillRect(centerX - cpuSize/2, centerY - cpuSize/2, cpuSize, cpuSize, 
    gfx->color565(255, 100, 0));
  gfx->drawRect(centerX - cpuSize/2, centerY - cpuSize/2, cpuSize, cpuSize, 
    gfx->color565(255, 200, 100));
    
  // CPU activity lines
  for (int line = 0; line < 4; line++) {
    float angle = tronTime * 2 + line * PI/2;
    int x1 = centerX + cos(angle) * cpuSize/2;
    int y1 = centerY + sin(angle) * cpuSize/2;
    int x2 = centerX + cos(angle) * (cpuSize/2 + 15);
    int y2 = centerY + sin(angle) * (cpuSize/2 + 15);
    
    gfx->drawLine(x1, y1, x2, y2, gfx->color565(255, 255, 0));
  }
  
  // Binary status display
  gfx->setCursor(10, gfx->height() - 30);
  gfx->setTextColor(gfx->color565(0, 255, 255));
  gfx->printf("01101001");
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(0, 255, 255));
  gfx->printf("TRONBIT");
}
}
namespace e298 {
float truchetTime = 0;
void reset(){truchetTime=0;}
void drawTruchet() {
  
  truchetTime += 0.02;
  
  gfx->fillScreen(BLACK);
  
  // Truchet tile patterns
  int tileSize = 20;
  int tilesX = gfx->width() / tileSize + 1;
  int tilesY = gfx->height() / tileSize + 1;
  
  for (int tileX = 0; tileX < tilesX; tileX++) {
    for (int tileY = 0; tileY < tilesY; tileY++) {
      int x = tileX * tileSize;
      int y = tileY * tileSize;
      
      // Tile animation phase
      float tileTime = truchetTime + tileX * 0.1 + tileY * 0.15;
      
      // Tile type based on position and time
      int tileType = ((int)(tileTime * 2) + tileX + tileY) % 4;
      
      uint16_t tileColor = gfx->color565(
        100 + 100 * sin(tileTime + tileX * 0.3),
        100 + 100 * sin(tileTime + tileY * 0.4), 
        100 + 100 * sin(tileTime + (tileX + tileY) * 0.2)
      );
      
      switch (tileType) {
        case 0: // Quarter circles - top-left to bottom-right
          for (int r = 2; r < tileSize/2; r += 2) {
            gfx->drawCircle(x, y, r, tileColor);
            gfx->drawCircle(x + tileSize, y + tileSize, r, tileColor);
          }
          break;
          
        case 1: // Quarter circles - top-right to bottom-left  
          for (int r = 2; r < tileSize/2; r += 2) {
            gfx->drawCircle(x + tileSize, y, r, tileColor);
            gfx->drawCircle(x, y + tileSize, r, tileColor);
          }
          break;
          
        case 2: // Diagonal lines
          gfx->drawLine(x, y, x + tileSize, y + tileSize, tileColor);
          gfx->drawLine(x + 2, y, x + tileSize, y + tileSize - 2, tileColor);
          gfx->drawLine(x, y + 2, x + tileSize - 2, y + tileSize, tileColor);
          break;
          
        case 3: // Anti-diagonal lines
          gfx->drawLine(x + tileSize, y, x, y + tileSize, tileColor);
          gfx->drawLine(x + tileSize - 2, y, x, y + tileSize - 2, tileColor);
          gfx->drawLine(x + tileSize, y + 2, x + 2, y + tileSize, tileColor);
          break;
      }
      
      // Highlight active tiles
      if (sin(tileTime * 4) > 0.8) {
        gfx->drawRect(x, y, tileSize, tileSize, WHITE);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("TRUCHET");
}
}
namespace e299 {
float tubeTime = 0;
void reset(){tubeTime=0;}
void drawTube() {
  
  tubeTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  // 3D tube/tunnel effect
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple concentric tube rings
  for (int ring = 0; ring < 12; ring++) {
    float ringTime = tubeTime + ring * 0.2;
    float ringZ = fmod(ringTime * 60, 300) - 100; // Z depth
    
    if (ringZ > 0) { // Only draw rings in front
      float scale = 200.0 / ringZ; // Perspective scaling
      int radius = 20 * scale;
      
      if (radius > 2 && radius < 200) {
        // Ring segments
        int segments = 16;
        for (int seg = 0; seg < segments; seg++) {
          float angle = (seg * 2.0 * PI / segments) + ringTime * 0.3;
          
          int x = centerX + cos(angle) * radius;
          int y = centerY + sin(angle) * radius;
          
          // Brightness based on distance
          int brightness = 255 * scale;
          if (brightness > 255) brightness = 255;
          
          uint16_t color = gfx->color565(brightness/2, brightness/3, brightness);
          gfx->fillCircle(x, y, 2 + scale, color);
          
          // Connect to next segment
          if (seg < segments - 1) {
            float nextAngle = ((seg + 1) * 2.0 * PI / segments) + ringTime * 0.3;
            int nextX = centerX + cos(nextAngle) * radius;
            int nextY = centerY + sin(nextAngle) * radius;
            gfx->drawLine(x, y, nextX, nextY, color);
          }
        }
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(100, 150, 255));
  gfx->printf("TUBE");
}
}
namespace e300 {
float twangTime = 0;
void reset(){twangTime=0;}
void drawTwang() {
  
  twangTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  // Guitar string vibration effect
  int stringY = gfx->height() / 2;
  
  // Multiple strings
  for (int string = 0; string < 6; string++) {
    float stringTime = twangTime + string * 0.3;
    int yOffset = (string - 2.5) * 25;
    
    // String amplitude and frequency
    float amplitude = 15 * exp(-stringTime * 0.5); // Decay over time
    float frequency = 0.2 + string * 0.05;
    
    // Draw vibrating string
    for (int x = 0; x < gfx->width(); x += 2) {
      float vibration = amplitude * sin(stringTime * 8 + x * frequency);
      int y = stringY + yOffset + vibration;
      
      uint16_t stringColor = gfx->color565(
        200 + string * 10,
        150 - string * 20,
        100 + sin(stringTime) * 50
      );
      
      if (y >= 0 && y < gfx->height()) {
        gfx->drawPixel(x, y, stringColor);
        
        // Harmonic overtones
        int harmonic2Y = y + vibration * 0.3 * sin(stringTime * 16 + x * frequency * 2);
        int harmonic3Y = y + vibration * 0.1 * sin(stringTime * 24 + x * frequency * 3);
        
        if (harmonic2Y >= 0 && harmonic2Y < gfx->height()) {
          uint16_t dimColor = gfx->color565((200 + string * 10) / 2, (150 - string * 20) / 2, (int)(100 + sin(stringTime) * 50) / 2);
          gfx->drawPixel(x, harmonic2Y, dimColor);
        }
        if (harmonic3Y >= 0 && harmonic3Y < gfx->height()) {
          uint16_t dimColor2 = gfx->color565((200 + string * 10) / 4, (150 - string * 20) / 4, (int)(100 + sin(stringTime) * 50) / 4);
          gfx->drawPixel(x, harmonic3Y, dimColor2);
        }
      }
    }
    
    // String endpoints (tuning pegs and bridge)
    gfx->fillCircle(5, stringY + yOffset, 3, gfx->color565(150, 150, 100));
    gfx->fillCircle(gfx->width() - 5, stringY + yOffset, 2, gfx->color565(200, 200, 150));
  }
  
  // Sound waves visualization
  for (int wave = 0; wave < 20; wave++) {
    float waveTime = twangTime * 3 + wave * 0.1;
    int waveX = (int)(waveTime * 50) % gfx->width();
    int waveAmplitude = 30 * exp(-waveTime * 0.2);
    
    for (int y = 0; y < gfx->height(); y += 3) {
      int intensity = waveAmplitude * sin(y * 0.1 + waveTime);
      if (intensity > 5) {
        gfx->drawPixel(waveX, y, gfx->color565(intensity, intensity/2, 100));
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(255, 200, 100));
  gfx->printf("TWANG");
}
}
// Stable slide 302: removed by screensaver policy.
// Stable slide 303: removed by screensaver policy.
// Stable slide 304: removed by screensaver policy.
namespace e304 {
float feedbackTime = 0;
void reset(){feedbackTime=0;}
void drawVfeedback() {
  
  feedbackTime += 0.06;
  
  gfx->fillScreen(BLACK);
  
  // Video feedback effect
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Recursive feedback layers
  for (int layer = 0; layer < 8; layer++) {
    float layerTime = feedbackTime + layer * 0.2;
    float scale = 0.9 - layer * 0.08;
    float rotation = layerTime * 0.1 + layer * 0.3;
    
    // Feedback rectangle
    int rectW = gfx->width() * scale;
    int rectH = gfx->height() * scale;
    
    // Rotate around center
    for (int edge = 0; edge < 4; edge++) {
      float angle1 = rotation + edge * PI/2;
      float angle2 = rotation + (edge + 1) * PI/2;
      
      int x1 = centerX + cos(angle1) * rectW/2;
      int y1 = centerY + sin(angle1) * rectH/2;
      int x2 = centerX + cos(angle2) * rectW/2;
      int y2 = centerY + sin(angle2) * rectH/2;
      
      uint16_t edgeColor = gfx->color565(
        255 - layer * 25,
        100 + layer * 15 + sin(layerTime) * 50,
        layer * 30 + cos(layerTime * 1.5) * 40
      );
      
      gfx->drawLine(x1, y1, x2, y2, edgeColor);
    }
    
    // Feedback pixels
    if (layer % 2 == 0) {
      for (int i = 0; i < 20; i++) {
        float pixelAngle = layerTime * 3 + i * 0.5;
        int pixelX = centerX + cos(pixelAngle) * rectW/3;
        int pixelY = centerY + sin(pixelAngle) * rectH/3;
        
        uint16_t pixelColor = gfx->color565(
          255 - layer * 20,
          255 - layer * 30,
          100 + layer * 20
        );
        
        gfx->fillCircle(pixelX, pixelY, 2, pixelColor);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(255, 200, 100));
  gfx->printf("VFEEDBACK");
}
}
// Stable slide 306: removed by screensaver policy.
namespace e306 {
float vineTime = 0;
void reset(){vineTime=0;}
void drawVines() {
  
  vineTime += 0.02;
  
  gfx->fillScreen(gfx->color565(20, 40, 20));
  
  // Growing vines
  for (int vine = 0; vine < 5; vine++) {
    float vineStartTime = vineTime + vine * 2.0;
    
    // Vine growth from bottom
    int startX = 30 + vine * 50;
    int startY = gfx->height() - 10;
    
    int segments = (int)(vineStartTime * 10) % 25;
    
    float currentX = startX;
    float currentY = startY;
    float currentAngle = -PI/2; // Start growing upward
    
    for (int seg = 0; seg < segments; seg++) {
      float segTime = vineStartTime - seg * 0.1;
      
      // Vine growth direction (mostly up, some random)
      currentAngle += (sin(segTime * 3 + vine) * 0.3);
      
      float segLength = 8 + sin(segTime * 2) * 3;
      float newX = currentX + cos(currentAngle) * segLength;
      float newY = currentY + sin(currentAngle) * segLength;
      
      // Draw vine segment
      uint16_t vineColor = gfx->color565(
        50 + seg * 2,
        100 + seg * 3 + sin(segTime) * 20,
        50 + seg * 1
      );
      
      gfx->drawLine(currentX, currentY, newX, newY, vineColor);
      
      // Leaves occasionally
      if (seg % 4 == 0 && seg > 0) {
        float leafAngle1 = currentAngle + PI/3;
        float leafAngle2 = currentAngle - PI/3;
        
        int leafX1 = currentX + cos(leafAngle1) * 6;
        int leafY1 = currentY + sin(leafAngle1) * 6;
        int leafX2 = currentX + cos(leafAngle2) * 6;
        int leafY2 = currentY + sin(leafAngle2) * 6;
        
        uint16_t leafColor = gfx->color565(30, 150, 30);
        gfx->fillTriangle(currentX, currentY, leafX1, leafY1, leafX2, leafY2, leafColor);
      }
      
      currentX = newX;
      currentY = newY;
      
      // Stop if out of bounds
      if (currentY < 0 || currentX < 0 || currentX >= gfx->width()) break;
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(100, 255, 100));
  gfx->printf("VINES");
}
}
// Stable slide 308: removed by screensaver policy.
// Stable slide 309: removed by screensaver policy.
// Stable slide 310: removed by screensaver policy.
namespace e310 {
float warpTime = 0;
void reset(){warpTime=0;}
void drawWhirlwindwarp() {
  
  warpTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  // Whirlwind warp effect
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int layer = 0; layer < 6; layer++) {
    float layerTime = warpTime + layer * 0.4;
    float rotation = layerTime * (1 + layer * 0.2);
    
    int numSpirals = 8 + layer * 2;
    for (int spiral = 0; spiral < numSpirals; spiral++) {
      float spiralAngle = (spiral * 2.0 * PI / numSpirals) + rotation;
      
      // Spiral positions with increasing radius
      for (int point = 0; point < 20; point++) {
        float radius = point * 3 + layer * 10;
        float warpRadius = radius + sin(layerTime * 3 + point * 0.2) * 8;
        
        int x = centerX + cos(spiralAngle + point * 0.3) * warpRadius;
        int y = centerY + sin(spiralAngle + point * 0.3) * warpRadius;
        
        if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
          uint16_t warpColor = gfx->color565(
            255 - layer * 30 - point * 8,
            100 + layer * 20 + sin(layerTime + point) * 60,
            layer * 40 + cos(layerTime + spiral) * 80
          );
          
          gfx->fillCircle(x, y, 2, warpColor);
          
          // Connect points for spiral effect
          if (point > 0) {
            float prevRadius = (point - 1) * 3 + layer * 10;
            float prevWarpRadius = prevRadius + sin(layerTime * 3 + (point - 1) * 0.2) * 8;
            int prevX = centerX + cos(spiralAngle + (point - 1) * 0.3) * prevWarpRadius;
            int prevY = centerY + sin(spiralAngle + (point - 1) * 0.3) * prevWarpRadius;
            
            gfx->drawLine(prevX, prevY, x, y, warpColor);
          }
        }
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(255, 150, 200));
  gfx->printf("WHIRLWINDWARP");
}
}
// Stable slide 312: removed by screensaver policy.
// Stable slide 313: removed by screensaver policy.
// Stable slide 314: removed by screensaver policy.
namespace e314 {
float holeTime = 0;
void reset(){holeTime=0;}
void drawWormhole() {
  
  holeTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  // Wormhole space-time distortion
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple concentric rings moving toward center
  for (int ring = 0; ring < 15; ring++) {
    float ringTime = holeTime + ring * 0.3;
    
    // Ring contracts over time
    float baseRadius = 120 - fmod(ringTime * 40, 140);
    if (baseRadius <= 0) continue;
    
    // Ring distortion
    int segments = 16;
    for (int seg = 0; seg < segments; seg++) {
      float angle = (seg * 2.0 * PI / segments) + ringTime * 0.5;
      
      // Wormhole distortion
      float distortion = sin(ringTime * 2 + seg * 0.5) * 10;
      float radius = baseRadius + distortion;
      
      int x = centerX + cos(angle) * radius;
      int y = centerY + sin(angle) * radius;
      
      if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
        // Color based on distance from center
        float perspective = baseRadius / 120.0;
        uint16_t ringColor = gfx->color565(
          (int)(255 * perspective + sin(ringTime + seg) * 50),
          (int)(150 * perspective + cos(ringTime + seg) * 40),
          (int)(100 * perspective + sin(ringTime * 1.5 + seg) * 60)
        );
        
        gfx->fillCircle(x, y, 3, ringColor);
        
        // Connect to next segment
        if (seg < segments - 1) {
          float nextAngle = ((seg + 1) * 2.0 * PI / segments) + ringTime * 0.5;
          float nextRadius = baseRadius + sin(ringTime * 2 + (seg + 1) * 0.5) * 10;
          int nextX = centerX + cos(nextAngle) * nextRadius;
          int nextY = centerY + sin(nextAngle) * nextRadius;
          
          gfx->drawLine(x, y, nextX, nextY, ringColor);
        }
      }
    }
  }
  
  // Central vortex
  for (int vortex = 0; vortex < 8; vortex++) {
    float vortexAngle = holeTime * 4 + vortex * PI/4;
    int vortexRadius = 5 + vortex * 3;
    
    int vortexX = centerX + cos(vortexAngle) * vortexRadius;
    int vortexY = centerY + sin(vortexAngle) * vortexRadius;
    
    gfx->fillCircle(vortexX, vortexY, 2, gfx->color565(255, 255, 255));
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(200, 150, 255));
  gfx->printf("WORMHOLE");
}
}
// Stable slide 316: removed by screensaver policy.
// Stable slide 317: removed by screensaver policy.
// Stable slide 318: removed by screensaver policy.
// Stable slide 319: removed by screensaver policy.
namespace e319 {
float matrixTime = 0;
int columnState[40];
float columnSpeed[40];
bool initialized = false;
void reset(){matrixTime=0;std::memset(columnState,0,sizeof(columnState));std::memset(columnSpeed,0,sizeof(columnSpeed));initialized=false;}
void drawXmatrix() {
  
  matrixTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  // Classic Matrix digital rain effect
   // State for each column
   // Speed for each column
  
  
  if (!initialized) {
    for (int i = 0; i < 40; i++) {
      columnState[i] = random(gfx->height());
      columnSpeed[i] = 1 + (random(100) / 100.0);
    }
    initialized = true;
  }
  
  int charWidth = 8;
  int charHeight = 12;
  int columns = gfx->width() / charWidth;
  
  // Matrix characters (simplified to ASCII)
  const char matrixChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()";
  int numChars = strlen(matrixChars);
  
  for (int col = 0; col < min(columns, 40); col++) {
    // Update column position
    columnState[col] += columnSpeed[col];
    if (columnState[col] > gfx->height() + 20) {
      columnState[col] = -20;
      columnSpeed[col] = 1 + (random(200) / 100.0);
    }
    
    // Draw falling characters
    for (int row = 0; row < 15; row++) {
      int charY = columnState[col] - row * charHeight;
      
      if (charY >= 0 && charY < gfx->height()) {
        int charX = col * charWidth;
        
        // Character selection (changes over time)
        char displayChar = matrixChars[(int)(matrixTime * 10 + col + row) % numChars];
        
        // Color intensity based on position in stream
        int intensity;
        if (row == 0) {
          intensity = 255; // Bright white head
        } else if (row < 4) {
          intensity = 255 - row * 40; // Bright green fade
        } else {
          intensity = 150 - row * 10; // Dark green fade
        }
        
        if (intensity > 0) {
          uint16_t charColor;
          if (row == 0) {
            charColor = gfx->color565(255, 255, 255); // White head
          } else {
            charColor = gfx->color565(0, intensity, 0); // Green trail
          }
          
          gfx->setCursor(charX, charY);
          gfx->setTextColor(charColor);
          gfx->setTextSize(1);
          gfx->printf("%c", displayChar);
        }
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(0, 255, 0));
  gfx->printf("XMATRIX - DIGITAL RAIN");
}
}
namespace e320 {
float swarmTime = 0;
void reset(){swarmTime=0;}
void drawXrayswarm() {
  
  swarmTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  // X-ray style particle swarm
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple swarm entities
  for (int swarm = 0; swarm < 8; swarm++) {
    float swarmPhase = swarmTime + swarm * 0.8;
    
    // Swarm center motion
    float swarmCenterX = centerX + sin(swarmPhase * 0.6) * 60;
    float swarmCenterY = centerY + cos(swarmPhase * 0.4) * 40;
    
    // Individual particles in swarm
    for (int particle = 0; particle < 20; particle++) {
      float particlePhase = swarmPhase + particle * 0.3;
      
      // Particle position relative to swarm center
      float radius = 20 + sin(particlePhase * 2) * 15;
      float angle = particlePhase * 3 + particle * 0.5;
      
      float particleX = swarmCenterX + cos(angle) * radius;
      float particleY = swarmCenterY + sin(angle) * radius;
      
      if (particleX >= 0 && particleX < gfx->width() && 
          particleY >= 0 && particleY < gfx->height()) {
        
        // X-ray effect - particles glow through each other
        uint16_t particleColor = gfx->color565(
          200 + sin(particlePhase + swarm) * 55,
          150 + cos(particlePhase + particle) * 80,
          255 - swarm * 20 - particle * 5
        );
        
        int particleSize = 2 + sin(particlePhase * 4) * 2;
        gfx->fillCircle(particleX, particleY, particleSize, particleColor);
        
        // X-ray trails
        for (int trail = 1; trail < 5; trail++) {
          float trailX = particleX - cos(angle) * trail * 3;
          float trailY = particleY - sin(angle) * trail * 3;
          
          int trailIntensity = 255 - trail * 50;
          if (trailIntensity > 0 && trailX >= 0 && trailX < gfx->width() && 
              trailY >= 0 && trailY < gfx->height()) {
            
            uint16_t trailColor = gfx->color565(
              trailIntensity * (200 + sin(particlePhase + swarm) * 55) / 255,
              trailIntensity * (150 + cos(particlePhase + particle) * 80) / 255,
              trailIntensity * (255 - swarm * 20 - particle * 5) / 255
            );
            
            gfx->drawPixel(trailX, trailY, trailColor);
          }
        }
      }
    }
  }
  
  // X-ray scanner lines
  float scanY = fmod(swarmTime * 50, gfx->height());
  gfx->drawLine(0, scanY, gfx->width(), scanY, gfx->color565(100, 255, 100));
  gfx->drawLine(0, scanY + 1, gfx->width(), scanY + 1, gfx->color565(50, 200, 50));
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(200, 255, 200));
  gfx->printf("XRAYSWARM");
}
}
namespace e321 {
float spiroTime = 0;
void reset(){spiroTime=0;}
void drawXspirograph() {
  
  spiroTime += 0.03;
  
  gfx->fillScreen(BLACK);
  
  // Spirograph mathematical art
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple spirograph patterns
  for (int pattern = 0; pattern < 3; pattern++) {
    float patternTime = spiroTime + pattern * 2.0;
    
    // Spirograph parameters that change over time
    float R = 40 + pattern * 15; // Fixed circle radius
    float r = 10 + sin(patternTime * 0.2) * 5; // Rolling circle radius  
    float d = 15 + cos(patternTime * 0.3) * 8; // Distance from rolling circle center
    
    // Draw spirograph curve
    for (int step = 0; step < 200; step++) {
      float t = step * 0.1 + patternTime;
      
      // Spirograph parametric equations
      float x = (R - r) * cos(t) + d * cos((R - r) * t / r);
      float y = (R - r) * sin(t) - d * sin((R - r) * t / r);
      
      int plotX = centerX + x;
      int plotY = centerY + y;
      
      if (plotX >= 0 && plotX < gfx->width() && plotY >= 0 && plotY < gfx->height()) {
        // Color based on pattern and position
        uint16_t spiroColor = gfx->color565(
          150 + pattern * 40 + sin(t + patternTime) * 60,
          100 + cos(t + patternTime * 1.2) * 80,
          200 - pattern * 30 + sin(t * 2 + patternTime) * 50
        );
        
        gfx->fillCircle(plotX, plotY, 2, spiroColor);
        
        // Connect to previous point
        if (step > 0) {
          float prevT = (step - 1) * 0.1 + patternTime;
          float prevX = (R - r) * cos(prevT) + d * cos((R - r) * prevT / r);
          float prevY = (R - r) * sin(prevT) - d * sin((R - r) * prevT / r);
          
          int prevPlotX = centerX + prevX;
          int prevPlotY = centerY + prevY;
          
          gfx->drawLine(prevPlotX, prevPlotY, plotX, plotY, spiroColor);
        }
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(255, 200, 150));
  gfx->printf("XSPIROGRAPH");
}
}
// Stable slide 323: removed by screensaver policy.
namespace e323 {
float zoomTime = 0;
void reset(){zoomTime=0;}
void drawZoom() {
  
  zoomTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  // Zoom/tunnel effect
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple zoom layers
  for (int layer = 0; layer < 8; layer++) {
    float layerTime = zoomTime + layer * 0.5;
    
    // Zoom scale (starts small, grows large)
    float scale = fmod(layerTime * 2, 6) + 0.1;
    
    // Pattern type rotates through different shapes
    int patternType = (layer + (int)(zoomTime * 0.5)) % 4;
    
    switch (patternType) {
      case 0: { // Concentric circles
        int radius = 10 * scale;
        if (radius > 3 && radius < 150) {
          uint16_t circleColor = gfx->color565(
            255 - layer * 20,
            100 + layer * 15 + sin(layerTime) * 80,
            layer * 30 + cos(layerTime * 1.5) * 50
          );
          gfx->drawCircle(centerX, centerY, radius, circleColor);
        }
        break;
      }
      
      case 1: { // Square spirals
        int size = 15 * scale;
        if (size > 5 && size < 120) {
          uint16_t squareColor = gfx->color565(
            layer * 30 + sin(layerTime) * 60,
            255 - layer * 25,
            100 + cos(layerTime * 1.2) * 70
          );
          gfx->drawRect(centerX - size/2, centerY - size/2, size, size, squareColor);
        }
        break;
      }
      
      case 2: { // Star patterns
        int starRadius = 8 * scale;
        if (starRadius > 2 && starRadius < 100) {
          uint16_t starColor = gfx->color565(
            200 + sin(layerTime + layer) * 55,
            200 + cos(layerTime + layer) * 55,
            100 + layer * 20
          );
          
          // Draw simple star
          for (int point = 0; point < 5; point++) {
            float angle = point * 2 * PI / 5 + layerTime;
            int x = centerX + cos(angle) * starRadius;
            int y = centerY + sin(angle) * starRadius;
            gfx->drawLine(centerX, centerY, x, y, starColor);
          }
        }
        break;
      }
      
      case 3: { // Cross patterns
        int crossSize = 12 * scale;
        if (crossSize > 4 && crossSize < 80) {
          uint16_t crossColor = gfx->color565(
            100 + layer * 25,
            layer * 35 + sin(layerTime * 2) * 40,
            255 - layer * 30
          );
          
          gfx->drawLine(centerX - crossSize, centerY, centerX + crossSize, centerY, crossColor);
          gfx->drawLine(centerX, centerY - crossSize, centerX, centerY + crossSize, crossColor);
        }
        break;
      }
    }
  }
  
  // Central vortex
  for (int vortex = 0; vortex < 12; vortex++) {
    float vortexAngle = zoomTime * 4 + vortex * PI/6;
    float vortexRadius = 3 + vortex + sin(zoomTime * 3 + vortex) * 2;
    
    int vortexX = centerX + cos(vortexAngle) * vortexRadius;
    int vortexY = centerY + sin(vortexAngle) * vortexRadius;
    
    gfx->fillCircle(vortexX, vortexY, 2, gfx->color565(255, 255, 200));
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("ZOOM - INFINITE TUNNEL");
}
}
namespace e324 {
float abstractTime = 0;
void reset(){abstractTime=0;}
void drawAbstractile() {
  
  abstractTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  // Abstract tile patterns
  int tileSize = 20;
  int tilesX = gfx->width() / tileSize + 1;
  int tilesY = gfx->height() / tileSize + 1;
  
  for (int tx = 0; tx < tilesX; tx++) {
    for (int ty = 0; ty < tilesY; ty++) {
      float tileTime = abstractTime + tx * 0.1 + ty * 0.15;
      
      int x = tx * tileSize;
      int y = ty * tileSize;
      
      // Abstract pattern based on position and time
      float pattern = sin(tileTime + tx * 0.3) * cos(tileTime + ty * 0.4);
      
      // Tile type selection
      int tileType = (int)(pattern * 4 + tileTime * 2) % 6;
      
      uint16_t tileColor = gfx->color565(
        128 + 127 * sin(tileTime + tx * 0.2),
        128 + 127 * cos(tileTime + ty * 0.3),
        128 + 127 * sin(tileTime + (tx + ty) * 0.1)
      );
      
      switch (tileType) {
        case 0: // Solid fill
          gfx->fillRect(x, y, tileSize - 1, tileSize - 1, tileColor);
          break;
          
        case 1: // Diagonal lines
          for (int line = 0; line < tileSize; line += 3) {
            gfx->drawLine(x + line, y, x, y + line, tileColor);
            gfx->drawLine(x + tileSize, y + line, x + line, y + tileSize, tileColor);
          }
          break;
          
        case 2: // Concentric squares
          for (int square = 2; square < tileSize / 2; square += 3) {
            gfx->drawRect(x + square, y + square, 
                         tileSize - 2 * square, tileSize - 2 * square, tileColor);
          }
          break;
          
        case 3: // Cross pattern
          gfx->drawLine(x + tileSize/2, y, x + tileSize/2, y + tileSize, tileColor);
          gfx->drawLine(x, y + tileSize/2, x + tileSize, y + tileSize/2, tileColor);
          break;
          
        case 4: // Dots
          for (int dot = 0; dot < 4; dot++) {
            int dotX = x + 4 + (dot % 2) * 10;
            int dotY = y + 4 + (dot / 2) * 10;
            gfx->fillCircle(dotX, dotY, 2, tileColor);
          }
          break;
          
        case 5: // Triangle
          gfx->fillTriangle(
            x + tileSize/2, y + 2,
            x + 2, y + tileSize - 2,
            x + tileSize - 2, y + tileSize - 2,
            tileColor
          );
          break;
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("ABSTRACTILE");
}
}
// Stable slide 326: removed by screensaver policy.
// Stable slide 327: removed by screensaver policy.
// Stable slide 328: removed by screensaver policy.
namespace e328 {
float apolloTime = 0;
void reset(){apolloTime=0;}
void drawApollonian() {
  
  apolloTime += 0.02;
  
  gfx->fillScreen(BLACK);
  
  // Apollonian gasket - circle packing
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Base circles that change over time
  float baseRadius = 60 + sin(apolloTime * 0.5) * 20;
  
  // Main containing circle
  gfx->drawCircle(centerX, centerY, baseRadius, WHITE);
  
  // Three mutually tangent circles inside
  float innerRadius = baseRadius / 3;
  float angle1 = apolloTime * 0.3;
  float angle2 = angle1 + 2 * PI / 3;
  float angle3 = angle2 + 2 * PI / 3;
  
  int circle1X = centerX + cos(angle1) * (baseRadius - innerRadius) * 0.5;
  int circle1Y = centerY + sin(angle1) * (baseRadius - innerRadius) * 0.5;
  
  int circle2X = centerX + cos(angle2) * (baseRadius - innerRadius) * 0.5;
  int circle2Y = centerY + sin(angle2) * (baseRadius - innerRadius) * 0.5;
  
  int circle3X = centerX + cos(angle3) * (baseRadius - innerRadius) * 0.5;
  int circle3Y = centerY + sin(angle3) * (baseRadius - innerRadius) * 0.5;
  
  uint16_t color1 = gfx->color565(255, 100, 100);
  uint16_t color2 = gfx->color565(100, 255, 100);
  uint16_t color3 = gfx->color565(100, 100, 255);
  
  gfx->drawCircle(circle1X, circle1Y, innerRadius, color1);
  gfx->drawCircle(circle2X, circle2Y, innerRadius, color2);
  gfx->drawCircle(circle3X, circle3Y, innerRadius, color3);
  
  // Smaller circles in the gaps (simplified Apollonian construction)
  for (int layer = 0; layer < 3; layer++) {
    float layerTime = apolloTime + layer * 0.7;
    int numSmallCircles = 6 + layer * 2;
    
    for (int i = 0; i < numSmallCircles; i++) {
      float smallAngle = layerTime + i * 2 * PI / numSmallCircles;
      float smallRadius = (baseRadius * 0.8) / (2 + layer);
      
      int smallX = centerX + cos(smallAngle) * smallRadius;
      int smallY = centerY + sin(smallAngle) * smallRadius;
      int smallSize = max(2, 15 - layer * 4);
      
      uint16_t smallColor = gfx->color565(
        128 + 127 * sin(layerTime + i * 0.5),
        128 + 127 * cos(layerTime + i * 0.7),
        128 + 127 * sin(layerTime + i * 0.3 + layer)
      );
      
      gfx->fillCircle(smallX, smallY, smallSize, smallColor);
    }
  }
  
  // Fractal detail circles
  for (int detail = 0; detail < 20; detail++) {
    float detailTime = apolloTime * 2 + detail * 0.4;
    float detailAngle = detailTime;
    float detailDist = 20 + sin(detailTime) * 15;
    
    int detailX = centerX + cos(detailAngle) * detailDist;
    int detailY = centerY + sin(detailAngle) * detailDist;
    
    if (detailX >= 0 && detailX < gfx->width() && detailY >= 0 && detailY < gfx->height()) {
      gfx->fillCircle(detailX, detailY, 1 + sin(detailTime * 3), 
        gfx->color565(255, 255, 200));
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("APOLLONIAN");
}
}
// Stable slide 330: removed by screensaver policy.
// Stable slide 331: removed by screensaver policy.
namespace e331 {
float attractTime = 0;
float particleX[20];
float particleY[20];
float particleVX[20];
float particleVY[20];
bool initialized = false;
void reset(){attractTime=0;std::memset(particleX,0,sizeof(particleX));std::memset(particleY,0,sizeof(particleY));std::memset(particleVX,0,sizeof(particleVX));std::memset(particleVY,0,sizeof(particleVY));initialized=false;}
void drawAttraction() {
  
  attractTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  // Particle attraction simulation
  
  
  
  
  
  
  if (!initialized) {
    for (int i = 0; i < 20; i++) {
      particleX[i] = random(gfx->width());
      particleY[i] = random(gfx->height());
      particleVX[i] = (random(100) - 50) / 25.0;
      particleVY[i] = (random(100) - 50) / 25.0;
    }
    initialized = true;
  }
  
  // Attractor points
  int attractorX = gfx->width() / 2 + sin(attractTime * 0.3) * 60;
  int attractorY = gfx->height() / 2 + cos(attractTime * 0.4) * 40;
  
  int attractor2X = gfx->width() * 0.3 + sin(attractTime * 0.5) * 30;
  int attractor2Y = gfx->height() * 0.7 + cos(attractTime * 0.6) * 25;
  
  // Draw attractors
  gfx->fillCircle(attractorX, attractorY, 8, gfx->color565(255, 200, 0));
  gfx->fillCircle(attractor2X, attractor2Y, 6, gfx->color565(0, 255, 200));
  
  // Update particles
  for (int i = 0; i < 20; i++) {
    // Calculate forces from attractors
    float dx1 = attractorX - particleX[i];
    float dy1 = attractorY - particleY[i];
    float dist1 = sqrt(dx1 * dx1 + dy1 * dy1) + 1;
    
    float dx2 = attractor2X - particleX[i];
    float dy2 = attractor2Y - particleY[i];
    float dist2 = sqrt(dx2 * dx2 + dy2 * dy2) + 1;
    
    // Attraction forces
    float forceStrength1 = 500.0 / (dist1 * dist1);
    float forceStrength2 = 300.0 / (dist2 * dist2);
    
    particleVX[i] += (dx1 / dist1) * forceStrength1 * 0.01;
    particleVY[i] += (dy1 / dist1) * forceStrength1 * 0.01;
    
    particleVX[i] += (dx2 / dist2) * forceStrength2 * 0.01;
    particleVY[i] += (dy2 / dist2) * forceStrength2 * 0.01;
    
    // Damping
    particleVX[i] *= 0.99;
    particleVY[i] *= 0.99;
    
    // Update position
    particleX[i] += particleVX[i];
    particleY[i] += particleVY[i];
    
    // Boundary wrapping
    if (particleX[i] < 0) particleX[i] = gfx->width();
    if (particleX[i] > gfx->width()) particleX[i] = 0;
    if (particleY[i] < 0) particleY[i] = gfx->height();
    if (particleY[i] > gfx->height()) particleY[i] = 0;
    
    // Draw particle
    uint16_t particleColor = gfx->color565(
      150 + sin(attractTime + i * 0.5) * 100,
      100 + cos(attractTime + i * 0.7) * 80,
      200 - i * 8
    );
    
    gfx->fillCircle(particleX[i], particleY[i], 2, particleColor);
    
    // Draw trails
    for (int trail = 1; trail < 4; trail++) {
      int trailX = particleX[i] - particleVX[i] * trail * 3;
      int trailY = particleY[i] - particleVY[i] * trail * 3;
      
      if (trailX >= 0 && trailX < gfx->width() && trailY >= 0 && trailY < gfx->height()) {
        int trailBrightness = 255 - trail * 60;
        uint16_t trailColor = gfx->color565(trailBrightness/3, trailBrightness/4, trailBrightness/2);
        gfx->drawPixel(trailX, trailY, trailColor);
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("ATTRACTION");
}
}
namespace e332 {
float tunnelTime = 0;
void reset(){tunnelTime=0;}
void drawAtunnel() {
  
  tunnelTime += 0.06;
  
  gfx->fillScreen(BLACK);
  
  // 3D tunnel effect
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple tunnel rings
  for (int ring = 0; ring < 12; ring++) {
    float ringTime = tunnelTime + ring * 0.2;
    
    // Ring moves toward viewer
    float z = fmod(ringTime * 50, 100) + 10;
    float scale = 100.0 / z;
    
    if (scale > 0.1 && scale < 5) {
      int radius = 50 * scale;
      
      // Ring segments
      int segments = 16;
      for (int seg = 0; seg < segments; seg++) {
        float angle = (seg * 2.0 * PI / segments) + ringTime * 2;
        
        int x1 = centerX + cos(angle) * radius;
        int y1 = centerY + sin(angle) * radius;
        
        float nextAngle = ((seg + 1) * 2.0 * PI / segments) + ringTime * 2;
        int x2 = centerX + cos(nextAngle) * radius;
        int y2 = centerY + sin(nextAngle) * radius;
        
        // Color based on distance
        uint16_t tunnelColor = gfx->color565(
          255 * scale + sin(ringTime + seg) * 100,
          150 * scale + cos(ringTime + seg) * 80,
          200 * scale
        );
        
        gfx->drawLine(x1, y1, x2, y2, tunnelColor);
        
        // Connect to next ring
        if (ring > 0) {
          float prevZ = fmod((ringTime - 0.2) * 50, 100) + 10;
          float prevScale = 100.0 / prevZ;
          int prevRadius = 50 * prevScale;
          
          int prevX = centerX + cos(angle) * prevRadius;
          int prevY = centerY + sin(angle) * prevRadius;
          
          gfx->drawLine(x1, y1, prevX, prevY, tunnelColor);
        }
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("ATUNNEL");
}
}
// Stable slide 334: removed by screensaver policy.
namespace e334 {
float horizonTime = 0;
void reset(){horizonTime=0;}
void drawBinaryhorizon() {
  
  horizonTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  // Binary data horizon
  int horizonY = gfx->height() / 2 + sin(horizonTime * 0.5) * 20;
  
  // Draw horizon line
  gfx->drawLine(0, horizonY, gfx->width(), horizonY, gfx->color565(0, 255, 0));
  
  // Binary data streams above horizon
  for (int stream = 0; stream < 15; stream++) {
    float streamTime = horizonTime * 2 + stream * 0.4;
    int streamX = 20 + stream * 20;
    
    // Data blocks moving up from horizon
    for (int block = 0; block < 8; block++) {
      float blockTime = streamTime + block * 0.3;
      int blockY = horizonY - block * 15 - (int)(streamTime * 20) % 60;
      
      if (blockY > 10 && blockY < horizonY) {
        // Binary digit (0 or 1)
        bool isOne = sin(blockTime * 5 + stream) > 0;
        
        uint16_t digitColor = gfx->color565(
          isOne ? 255 : 100,
          255 - block * 20,
          isOne ? 100 : 255
        );
        
        gfx->setTextSize(1);
        gfx->setTextColor(digitColor);
        gfx->setCursor(streamX, blockY);
        gfx->printf("%d", isOne ? 1 : 0);
        
        // Connecting lines
        if (block > 0) {
          int prevBlockY = horizonY - (block - 1) * 15 - (int)(streamTime * 20) % 60;
          if (prevBlockY < horizonY) {
            gfx->drawLine(streamX + 3, blockY, streamX + 3, prevBlockY + 8, digitColor);
          }
        }
      }
    }
  }
  
  // Binary data streams below horizon (inverted)
  for (int stream = 0; stream < 12; stream++) {
    float streamTime = horizonTime * 1.5 + stream * 0.5;
    int streamX = 25 + stream * 25;
    
    for (int block = 0; block < 6; block++) {
      float blockTime = streamTime + block * 0.4;
      int blockY = horizonY + 20 + block * 18 + (int)(streamTime * 15) % 50;
      
      if (blockY < gfx->height() - 10 && blockY > horizonY) {
        bool isOne = cos(blockTime * 4 + stream * 1.5) > 0;
        
        uint16_t digitColor = gfx->color565(
          isOne ? 200 : 80,
          100 + block * 15,
          isOne ? 80 : 200
        );
        
        gfx->setTextSize(1);
        gfx->setTextColor(digitColor);
        gfx->setCursor(streamX, blockY);
        gfx->printf("%d", isOne ? 1 : 0);
      }
    }
  }
  
  // Grid overlay
  for (int x = 0; x < gfx->width(); x += 40) {
    gfx->drawLine(x, 0, x, gfx->height(), gfx->color565(0, 50, 0));
  }
  for (int y = 0; y < gfx->height(); y += 30) {
    gfx->drawLine(0, y, gfx->width(), y, gfx->color565(0, 50, 0));
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(gfx->color565(0, 255, 0));
  gfx->printf("BINARYHORIZON");
}
}
namespace e335 {
float ringTime = 0;
void reset(){ringTime=0;}
void drawBinaryring() {
  
  ringTime += 0.06;
  
  gfx->fillScreen(BLACK);
  
  // Binary data arranged in rings
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple concentric rings
  for (int ring = 0; ring < 6; ring++) {
    float ringPhase = ringTime + ring * 0.8;
    int radius = 20 + ring * 18;
    
    // Number of binary digits per ring
    int numDigits = 8 + ring * 4;
    
    for (int digit = 0; digit < numDigits; digit++) {
      float digitAngle = (digit * 2.0 * PI / numDigits) + ringPhase * (1 + ring * 0.2);
      
      int digitX = centerX + cos(digitAngle) * radius;
      int digitY = centerY + sin(digitAngle) * radius;
      
      if (digitX >= 0 && digitX < gfx->width() - 8 && 
          digitY >= 0 && digitY < gfx->height() - 8) {
        
        // Binary value based on position and time
        bool isOne = sin(ringPhase * 3 + digit * 0.5 + ring) > 0;
        
        // Color coding by ring
        uint16_t digitColor = gfx->color565(
          255 - ring * 30,
          100 + ring * 25 + sin(ringPhase + digit) * 60,
          ring * 40 + cos(ringPhase + digit) * 80
        );
        
        gfx->setTextSize(1);
        gfx->setTextColor(digitColor);
        gfx->setCursor(digitX, digitY);
        gfx->printf("%d", isOne ? 1 : 0);
        
        // Connecting lines to center
        if (sin(ringPhase * 2 + digit) > 0.7) {
          gfx->drawLine(centerX, centerY, digitX + 3, digitY + 4, digitColor);
        }
        
        // Ring connections
        if (digit > 0) {
          float prevAngle = ((digit - 1) * 2.0 * PI / numDigits) + ringPhase * (1 + ring * 0.2);
          int prevX = centerX + cos(prevAngle) * radius;
          int prevY = centerY + sin(prevAngle) * radius;
          
          gfx->drawLine(prevX + 3, prevY + 4, digitX + 3, digitY + 4, digitColor);
        }
      }
    }
    
    // Close the ring
    if (numDigits > 0) {
      float firstAngle = ringPhase * (1 + ring * 0.2);
      float lastAngle = ((numDigits - 1) * 2.0 * PI / numDigits) + ringPhase * (1 + ring * 0.2);
      
      int firstX = centerX + cos(firstAngle) * radius;
      int firstY = centerY + sin(firstAngle) * radius;
      int lastX = centerX + cos(lastAngle) * radius;
      int lastY = centerY + sin(lastAngle) * radius;
      
      uint16_t ringColor = gfx->color565(100 + ring * 20, 150, 200 - ring * 25);
      gfx->drawLine(firstX + 3, firstY + 4, lastX + 3, lastY + 4, ringColor);
    }
  }
  
  // Central data core
  gfx->fillCircle(centerX, centerY, 8, gfx->color565(255, 255, 0));
  gfx->setTextSize(1);
  gfx->setTextColor(BLACK);
  gfx->setCursor(centerX - 6, centerY - 4);
  gfx->printf("BIN");
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("BINARYRING");
}
}
namespace e336 {
float blasterTime = 0;
float blasterX = 160;
float blasterY = 200;
float shots[10][4];
float enemies[8][3];
bool initialized = false;
void reset(){blasterTime=0;blasterX=160;blasterY=200;std::memset(shots,0,sizeof(shots));std::memset(enemies,0,sizeof(enemies));initialized=false;}
void drawBlaster() {
  
  blasterTime += 0.07;
  
  gfx->fillScreen(BLACK);
  
  // Space blaster shooting game
  
  
   // x, y, vx, vy
   // x, y, type
  
  
  if (!initialized) {
    blasterX = gfx->width() / 2;
    blasterY = gfx->height() - 30;
    
    // Initialize shots
    for (int i = 0; i < 10; i++) {
      shots[i][0] = -1; // inactive
    }
    
    // Initialize enemies
    for (int i = 0; i < 8; i++) {
      enemies[i][0] = random(gfx->width());
      enemies[i][1] = random(gfx->height() / 2);
      enemies[i][2] = random(3); // enemy type
    }
    
    initialized = true;
  }
  
  // Move blaster (auto-control)
  blasterX += sin(blasterTime * 2) * 30;
  if (blasterX < 10) blasterX = 10;
  if (blasterX > gfx->width() - 10) blasterX = gfx->width() - 10;
  
  // Draw blaster ship
  gfx->fillTriangle(
    blasterX, blasterY - 10,
    blasterX - 8, blasterY + 5,
    blasterX + 8, blasterY + 5,
    gfx->color565(0, 255, 0)
  );
  
  // Engine glow
  if (sin(blasterTime * 20) > 0) {
    gfx->fillCircle(blasterX, blasterY + 8, 3, gfx->color565(255, 100, 0));
  }
  
  // Auto-fire shots
  if ((int)(blasterTime * 10) % 8 == 0) {
    for (int i = 0; i < 10; i++) {
      if (shots[i][0] < 0) { // Find inactive shot
        shots[i][0] = blasterX;
        shots[i][1] = blasterY - 10;
        shots[i][2] = sin(blasterTime) * 2; // slight angle
        shots[i][3] = -8; // upward velocity
        break;
      }
    }
  }
  
  // Update and draw shots
  for (int i = 0; i < 10; i++) {
    if (shots[i][0] >= 0) { // active shot
      shots[i][0] += shots[i][2];
      shots[i][1] += shots[i][3];
      
      // Remove if off screen
      if (shots[i][1] < 0 || shots[i][0] < 0 || shots[i][0] > gfx->width()) {
        shots[i][0] = -1; // deactivate
      } else {
        gfx->fillCircle(shots[i][0], shots[i][1], 2, gfx->color565(255, 255, 0));
        
        // Shot trail
        gfx->drawPixel(shots[i][0], shots[i][1] + 3, gfx->color565(255, 200, 0));
        gfx->drawPixel(shots[i][0], shots[i][1] + 6, gfx->color565(255, 150, 0));
      }
    }
  }
  
  // Update and draw enemies
  for (int i = 0; i < 8; i++) {
    // Move enemies
    enemies[i][1] += 1 + sin(blasterTime + i) * 0.5;
    enemies[i][0] += sin(blasterTime * 2 + i * 0.7) * 2;
    
    // Reset if off screen
    if (enemies[i][1] > gfx->height()) {
      enemies[i][0] = random(gfx->width());
      enemies[i][1] = -20;
      enemies[i][2] = random(3);
    }
    
    // Boundary check
    if (enemies[i][0] < 0) enemies[i][0] = gfx->width();
    if (enemies[i][0] > gfx->width()) enemies[i][0] = 0;
    
    // Draw enemy based on type
    uint16_t enemyColor;
    switch ((int)enemies[i][2]) {
      case 0: // Red fighter
        enemyColor = gfx->color565(255, 0, 0);
        gfx->fillRect(enemies[i][0] - 4, enemies[i][1] - 2, 8, 4, enemyColor);
        break;
      case 1: // Blue bomber
        enemyColor = gfx->color565(0, 100, 255);
        gfx->fillCircle(enemies[i][0], enemies[i][1], 5, enemyColor);
        break;
      case 2: // Green scout
        enemyColor = gfx->color565(0, 255, 100);
        gfx->fillTriangle(
          enemies[i][0], enemies[i][1] - 4,
          enemies[i][0] - 3, enemies[i][1] + 2,
          enemies[i][0] + 3, enemies[i][1] + 2,
          enemyColor
        );
        break;
    }
    
    // Check collisions with shots
    for (int shot = 0; shot < 10; shot++) {
      if (shots[shot][0] >= 0) {
        float dx = enemies[i][0] - shots[shot][0];
        float dy = enemies[i][1] - shots[shot][1];
        if (sqrt(dx*dx + dy*dy) < 8) {
          // Hit! Create explosion effect
          for (int exp = 0; exp < 5; exp++) {
            int expX = enemies[i][0] + random(10) - 5;
            int expY = enemies[i][1] + random(10) - 5;
            gfx->fillCircle(expX, expY, 2, gfx->color565(255, 255, 0));
          }
          
          // Reset enemy and shot
          enemies[i][0] = random(gfx->width());
          enemies[i][1] = -20;
          shots[shot][0] = -1;
        }
      }
    }
  }
  
  // Starfield
  for (int star = 0; star < 30; star++) {
    int starX = (star * 37) % gfx->width();
    int starY = (int)(blasterTime * 100 + star * 23) % gfx->height();
    gfx->drawPixel(starX, starY, gfx->color565(200, 200, 255));
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("BLASTER");
}
}
namespace e337 {
float spinTime = 0;
void reset(){spinTime=0;}
void drawBlitspin() {
  
  spinTime += 0.08;
  
  gfx->fillScreen(BLACK);
  
  // Rotating bitmap-like patterns
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple spinning elements
  for (int spinner = 0; spinner < 4; spinner++) {
    float spinPhase = spinTime * (1 + spinner * 0.3) + spinner * PI/2;
    
    int spinnerX = centerX + cos(spinTime * 0.4 + spinner) * 40;
    int spinnerY = centerY + sin(spinTime * 0.3 + spinner) * 30;
    
    // Create bitmap-like pattern
    int patternSize = 16;
    for (int px = 0; px < patternSize; px++) {
      for (int py = 0; py < patternSize; py++) {
        // Create pattern based on position
        bool isSet = ((px ^ py) & 3) == 0 || (px & py & 1);
        
        if (isSet) {
          // Rotate the pattern
          float relX = px - patternSize/2;
          float relY = py - patternSize/2;
          
          float rotX = relX * cos(spinPhase) - relY * sin(spinPhase);
          float rotY = relX * sin(spinPhase) + relY * cos(spinPhase);
          
          int finalX = spinnerX + rotX * 2;
          int finalY = spinnerY + rotY * 2;
          
          if (finalX >= 0 && finalX < gfx->width() && 
              finalY >= 0 && finalY < gfx->height()) {
            
            uint16_t pixelColor = gfx->color565(
              150 + spinner * 25 + sin(spinPhase + px) * 60,
              100 + cos(spinPhase + py) * 80,
              200 - spinner * 30 + sin(spinPhase + px + py) * 50
            );
            
            gfx->fillRect(finalX, finalY, 2, 2, pixelColor);
          }
        }
      }
    }
  }
  
  // Additional spinning lines
  for (int line = 0; line < 8; line++) {
    float lineAngle = spinTime * 2 + line * PI/4;
    
    int startRadius = 20;
    int endRadius = 60;
    
    int startX = centerX + cos(lineAngle) * startRadius;
    int startY = centerY + sin(lineAngle) * startRadius;
    int endX = centerX + cos(lineAngle) * endRadius;
    int endY = centerY + sin(lineAngle) * endRadius;
    
    uint16_t lineColor = gfx->color565(
      255 * abs(sin(spinTime + line)),
      255 * abs(cos(spinTime + line * 1.3)),
      255 * abs(sin(spinTime * 1.7 + line))
    );
    
    gfx->drawLine(startX, startY, endX, endY, lineColor);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("BLITSPIN");
}
}
namespace e338 {
float tubeTime = 0;
void reset(){tubeTime=0;}
void drawBlocktube() {
  
  tubeTime += 0.05;
  
  gfx->fillScreen(BLACK);
  
  // 3D block tube tunnel
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Multiple tube segments
  for (int segment = 0; segment < 15; segment++) {
    float segmentTime = tubeTime + segment * 0.3;
    
    // Segment moves toward viewer
    float z = fmod(segmentTime * 30, 80) + 5;
    float scale = 50.0 / z;
    
    if (scale > 0.05 && scale < 3) {
      // Tube made of blocks
      int blockSize = max(1, (int)(8 * scale));
      int tubeRadius = 40 * scale;
      
      // Blocks around the tube
      int numBlocks = 12;
      for (int block = 0; block < numBlocks; block++) {
        float blockAngle = (block * 2.0 * PI / numBlocks) + segmentTime;
        
        int blockX = centerX + cos(blockAngle) * tubeRadius;
        int blockY = centerY + sin(blockAngle) * tubeRadius;
        
        if (blockX >= 0 && blockX < gfx->width() && 
            blockY >= 0 && blockY < gfx->height()) {
          
          // Block color based on distance and position
          uint16_t blockColor = gfx->color565(
            255 * scale + sin(segmentTime + block) * 100,
            150 * scale + cos(segmentTime + block) * 60,
            200 * scale + sin(segmentTime * 2 + block) * 80
          );
          
          // 3D block effect
          gfx->fillRect(blockX, blockY, blockSize, blockSize, blockColor);
          
          // Block highlights
          uint16_t highlightColor = gfx->color565(
            min(255, (int)(255 * scale * 1.5)),
            min(255, (int)(200 * scale * 1.5)),
            min(255, (int)(255 * scale * 1.5))
          );
          
          gfx->drawLine(blockX, blockY, blockX + blockSize - 1, blockY, highlightColor);
          gfx->drawLine(blockX, blockY, blockX, blockY + blockSize - 1, highlightColor);
          
          // Block shadows
          uint16_t shadowColor = gfx->color565(
            (int)(100 * scale),
            (int)(50 * scale),
            (int)(80 * scale)
          );
          
          gfx->drawLine(blockX + blockSize - 1, blockY + 1, 
                       blockX + blockSize - 1, blockY + blockSize - 1, shadowColor);
          gfx->drawLine(blockX + 1, blockY + blockSize - 1, 
                       blockX + blockSize - 1, blockY + blockSize - 1, shadowColor);
        }
      }
    }
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("BLOCKTUBE");
}
}
namespace e339 {
float boubouleTime = 0;
float ballX[12];
float ballY[12];
float ballVX[12];
float ballVY[12];
float ballSize[12];
bool initialized = false;
void reset(){boubouleTime=0;std::memset(ballX,0,sizeof(ballX));std::memset(ballY,0,sizeof(ballY));std::memset(ballVX,0,sizeof(ballVX));std::memset(ballVY,0,sizeof(ballVY));std::memset(ballSize,0,sizeof(ballSize));initialized=false;}
void drawBouboule() {
  
  boubouleTime += 0.04;
  
  gfx->fillScreen(BLACK);
  
  // French "Bouboule" effect - bouncing balls/bubbles
  
  
  
  
  
  
  
  if (!initialized) {
    for (int i = 0; i < 12; i++) {
      ballX[i] = random(gfx->width());
      ballY[i] = random(gfx->height());
      ballVX[i] = (random(100) - 50) / 10.0;
      ballVY[i] = (random(100) - 50) / 10.0;
      ballSize[i] = 5 + random(10);
    }
    initialized = true;
  }
  
  // Update and draw balls
  for (int i = 0; i < 12; i++) {
    // Update position
    ballX[i] += ballVX[i];
    ballY[i] += ballVY[i];
    
    // Bounce off walls
    if (ballX[i] <= ballSize[i] || ballX[i] >= gfx->width() - ballSize[i]) {
      ballVX[i] = -ballVX[i];
      ballX[i] = max(ballSize[i], min(gfx->width() - ballSize[i], ballX[i]));
    }
    
    if (ballY[i] <= ballSize[i] || ballY[i] >= gfx->height() - ballSize[i]) {
      ballVY[i] = -ballVY[i];
      ballY[i] = max(ballSize[i], min(gfx->height() - ballSize[i], ballY[i]));
    }
    
    // Ball-to-ball collisions (simplified)
    for (int j = i + 1; j < 12; j++) {
      float dx = ballX[j] - ballX[i];
      float dy = ballY[j] - ballY[i];
      float distance = sqrt(dx * dx + dy * dy);
      float minDistance = ballSize[i] + ballSize[j];
      
      if (distance < minDistance && distance > 0) {
        // Simple collision response
        float nx = dx / distance;
        float ny = dy / distance;
        
        // Separate balls
        float overlap = minDistance - distance;
        ballX[i] -= nx * overlap * 0.5;
        ballY[i] -= ny * overlap * 0.5;
        ballX[j] += nx * overlap * 0.5;
        ballY[j] += ny * overlap * 0.5;
        
        // Exchange velocities (simplified)
        float tempVX = ballVX[i];
        float tempVY = ballVY[i];
        ballVX[i] = ballVX[j];
        ballVY[i] = ballVY[j];
        ballVX[j] = tempVX;
        ballVY[j] = tempVY;
      }
    }
    
    // Draw ball with gradient effect
    int ballRadius = ballSize[i];
    
    // Main ball body
    uint16_t ballColor = gfx->color565(
      150 + sin(boubouleTime + i * 0.7) * 100,
      100 + cos(boubouleTime + i * 1.1) * 80,
      200 + sin(boubouleTime * 1.3 + i) * 55
    );
    
    gfx->fillCircle(ballX[i], ballY[i], ballRadius, ballColor);
    
    // Highlight
    uint16_t highlightColor = gfx->color565(255, 255, 255);
    gfx->fillCircle(ballX[i] - ballRadius/3, ballY[i] - ballRadius/3, ballRadius/3, highlightColor);
    
    // Shadow/reflection
    uint16_t shadowColor = gfx->color565(
      (150 + sin(boubouleTime + i * 0.7) * 100) / 3,
      (100 + cos(boubouleTime + i * 1.1) * 80) / 3,
      (200 + sin(boubouleTime * 1.3 + i) * 55) / 3
    );
    gfx->fillCircle(ballX[i] + ballRadius/4, ballY[i] + ballRadius/4, ballRadius/4, shadowColor);
  }
  
  gfx->setCursor(10, 10);
  gfx->setTextColor(WHITE);
  gfx->printf("BOUBOULE");
}
}
// Stable slide 341: removed by screensaver policy.
using Fn=void(*)();
const Fn draws[]={e0::drawBoxed,e1::drawBoxfit,e2::drawBraid,nullptr,nullptr,nullptr,e6::drawBuildlwo,e7::drawBumps,e8::drawCage,e9::drawCarousel,e10::drawCCurve,e11::drawCeltic,nullptr,nullptr,e14::drawCircuit,e15::drawCityFlow,nullptr,nullptr,nullptr,e19::drawColors,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,e33::drawCrackberg,nullptr,nullptr,nullptr,e37::drawCube21,e38::drawCubenetic,nullptr,nullptr,e41::drawCubeTwist,nullptr,e43::drawCuboctaversion,nullptr,nullptr,e46::drawDangerball,nullptr,nullptr,e49::drawDeepStars,nullptr,nullptr,nullptr,nullptr,e54::drawDemon,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,e61::drawDnalogo,nullptr,e63::drawDoubletime,nullptr,e65::drawDpmsProtocol,e66::drawDrift,nullptr,e68::drawDroste,e69::drawDymaxionmapCoords,e70::drawEasing,e71::drawEndgame,e72::drawEnergystream,e73::drawEngine,e74::drawEpicycle,nullptr,e76::drawEuler2d,e77::drawFadeplot,e78::drawEsper,e79::drawEtruscanvenus,nullptr,e81::drawExtIdleNotify,e82::drawExtrusion,e83::drawExtrusionHelix2,e84::drawExtrusionHelix3,e85::drawExtrusionHelix4,e86::drawExtrusionJoinoffset,e87::drawExtrusionScrew,e88::drawExtrusionTaper,e89::drawExtrusionTwistoid,e90::drawExts,e91::drawExtSessionLock,e92::drawFade,e93::drawFfmpegOut,nullptr,e95::drawFilmleader,nullptr,e97::drawFlag,e98::drawFlame,e99::drawFlipflop,e100::drawFlipscreen3d,e101::drawFliptext,e102::drawFlow,e103::drawFluidballs,e104::drawFlurry,e105::drawFlurrySmoke,nullptr,e107::drawFlurryStar,e108::drawFlurryTexture,e109::drawFlyingtoasters,e110::drawFontglide,nullptr,nullptr,e113::drawFps,e114::drawFpsGl,e115::drawFuzzyflakes,e116::drawGalaxy,e117::drawGallant12x22Bsd,e118::drawGears,e119::drawGeodesic,e120::drawGeodesicgears,nullptr,e122::drawGlmatrix,nullptr,nullptr,nullptr,nullptr,e127::drawGlslUtils,e128::drawGlsnake,nullptr,e130::drawGoop,e131::drawGltrackball,e132::drawGlutStroke,e133::drawGlutSwidth,nullptr,e135::drawGrabXimage,e136::drawGrav,e137::drawGravitywell,nullptr,e139::drawHalftone,e140::drawHalo,e141::drawHandsy,e142::drawHandsyModel,nullptr,e144::drawHeadroomModel,nullptr,nullptr,nullptr,e148::drawHextrail,nullptr,nullptr,e151::drawHilbert,e152::drawHopalong,e153::drawHopfanimations,e154::drawHopffibration,nullptr,e156::drawHydrostat,e157::drawHyperball,e158::drawHypercube,e159::drawHypertorus,e160::drawHypnowheel,e161::drawIdleProtocol,e162::drawIfs,e163::drawImsmap,e164::drawInteraggregate,e165::drawInterference,e166::drawIntermomentary,e167::drawInvolute,e168::drawJigglypuff,e169::drawJigsaw,e170::drawJuggle,e171::drawJuggler3d,e172::drawRotor,e173::drawRotzoomer,nullptr,e175::drawRubik,e176::drawSballs,e177::drawScooter,nullptr,e179::drawShadebobs,nullptr,e181::drawSlidescreen,e182::drawSlip,e183::drawSonar,e184::drawSpeedmine,e185::drawSphere,e186::drawSpheremonics,e187::drawSpiral,e188::drawSpotlight,e189::drawSproingies,e190::drawStairs,nullptr,e192::drawPetri,e193::drawPhosphor,nullptr,e195::drawPiecewise,nullptr,e197::drawPipeobjs,nullptr,e199::drawPlatonicfolding,nullptr,e201::drawPolyhedraGl,nullptr,nullptr,e204::drawPong,nullptr,e206::drawPow2,nullptr,nullptr,nullptr,e210::drawProvidence,e211::drawM6502,nullptr,nullptr,e214::drawMarching,e215::drawMaze3d,nullptr,e217::drawMemscroller,e218::drawMenger,e219::drawMetaballs,nullptr,e221::drawMirrorblob,e222::drawMoebius,e223::drawMoebiusgears,e224::drawMoire2,e225::drawMoire,e226::drawMorph3d,e227::drawMunch,nullptr,e229::drawNerverot,nullptr,e231::drawNormals,nullptr,e233::drawOverlay,e234::drawPacmanAI,e235::drawPacman,e236::drawPacmanLevel,nullptr,nullptr,e239::drawPasswdKerberos,nullptr,e241::drawPasswdPwent,nullptr,nullptr,e244::drawPenetrate,nullptr,e246::drawPulsar,e247::drawPyro,e248::drawQix,nullptr,nullptr,e251::drawQuickhull,e252::drawRaverhoop,e253::drawRazzledazzle,e254::drawRdbomb,nullptr,e256::drawRemote,e257::drawResources,e258::drawRipples,e259::drawRobot,e260::drawRobotWireframe,e261::drawRocks,e262::drawRomanboy,e263::drawRorschach,e264::drawRotator,e265::drawShips,e266::drawSierpinski3d,e267::drawSierpinski,e268::drawSkullModel,e269::drawSkulloop,nullptr,e271::drawSphereversionAnalytic,e272::drawSphereversion,e273::drawSphereversionCorrugations,e274::drawSplitflap,e275::drawSplodesic,e276::drawSquiral,e277::drawSquirtorus,e278::drawStarwars,nullptr,e280::drawStrange,e281::drawSubstrate,e282::drawSuperquadrics,e283::drawSurfaces,nullptr,e285::drawSwirl,e286::drawT3d,nullptr,e288::drawTeapot,nullptr,e290::drawThornbird,e291::drawTimetunnel,nullptr,nullptr,e294::drawTopblock,e295::drawTrackball,e296::drawTriangle,e297::drawTronbit,e298::drawTruchet,e299::drawTube,e300::drawTwang,nullptr,nullptr,nullptr,e304::drawVfeedback,nullptr,e306::drawVines,nullptr,nullptr,nullptr,e310::drawWhirlwindwarp,nullptr,nullptr,nullptr,e314::drawWormhole,nullptr,nullptr,nullptr,nullptr,e319::drawXmatrix,e320::drawXrayswarm,e321::drawXspirograph,nullptr,e323::drawZoom,e324::drawAbstractile,nullptr,nullptr,nullptr,e328::drawApollonian,nullptr,nullptr,e331::drawAttraction,e332::drawAtunnel,nullptr,e334::drawBinaryhorizon,e335::drawBinaryring,e336::drawBlaster,e337::drawBlitspin,e338::drawBlocktube,e339::drawBouboule,nullptr};
const Fn resets[]={e0::reset,e1::reset,e2::reset,nullptr,nullptr,nullptr,e6::reset,e7::reset,e8::reset,e9::reset,e10::reset,e11::reset,nullptr,nullptr,e14::reset,e15::reset,nullptr,nullptr,nullptr,e19::reset,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,e33::reset,nullptr,nullptr,nullptr,e37::reset,e38::reset,nullptr,nullptr,e41::reset,nullptr,e43::reset,nullptr,nullptr,e46::reset,nullptr,nullptr,e49::reset,nullptr,nullptr,nullptr,nullptr,e54::reset,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,e61::reset,nullptr,e63::reset,nullptr,e65::reset,e66::reset,nullptr,e68::reset,e69::reset,e70::reset,e71::reset,e72::reset,e73::reset,e74::reset,nullptr,e76::reset,e77::reset,e78::reset,e79::reset,nullptr,e81::reset,e82::reset,e83::reset,e84::reset,e85::reset,e86::reset,e87::reset,e88::reset,e89::reset,e90::reset,e91::reset,e92::reset,e93::reset,nullptr,e95::reset,nullptr,e97::reset,e98::reset,e99::reset,e100::reset,e101::reset,e102::reset,e103::reset,e104::reset,e105::reset,nullptr,e107::reset,e108::reset,e109::reset,e110::reset,nullptr,nullptr,e113::reset,e114::reset,e115::reset,e116::reset,e117::reset,e118::reset,e119::reset,e120::reset,nullptr,e122::reset,nullptr,nullptr,nullptr,nullptr,e127::reset,e128::reset,nullptr,e130::reset,e131::reset,e132::reset,e133::reset,nullptr,e135::reset,e136::reset,e137::reset,nullptr,e139::reset,e140::reset,e141::reset,e142::reset,nullptr,e144::reset,nullptr,nullptr,nullptr,e148::reset,nullptr,nullptr,e151::reset,e152::reset,e153::reset,e154::reset,nullptr,e156::reset,e157::reset,e158::reset,e159::reset,e160::reset,e161::reset,e162::reset,e163::reset,e164::reset,e165::reset,e166::reset,e167::reset,e168::reset,e169::reset,e170::reset,e171::reset,e172::reset,e173::reset,nullptr,e175::reset,e176::reset,e177::reset,nullptr,e179::reset,nullptr,e181::reset,e182::reset,e183::reset,e184::reset,e185::reset,e186::reset,e187::reset,e188::reset,e189::reset,e190::reset,nullptr,e192::reset,e193::reset,nullptr,e195::reset,nullptr,e197::reset,nullptr,e199::reset,nullptr,e201::reset,nullptr,nullptr,e204::reset,nullptr,e206::reset,nullptr,nullptr,nullptr,e210::reset,e211::reset,nullptr,nullptr,e214::reset,e215::reset,nullptr,e217::reset,e218::reset,e219::reset,nullptr,e221::reset,e222::reset,e223::reset,e224::reset,e225::reset,e226::reset,e227::reset,nullptr,e229::reset,nullptr,e231::reset,nullptr,e233::reset,e234::reset,e235::reset,e236::reset,nullptr,nullptr,e239::reset,nullptr,e241::reset,nullptr,nullptr,e244::reset,nullptr,e246::reset,e247::reset,e248::reset,nullptr,nullptr,e251::reset,e252::reset,e253::reset,e254::reset,nullptr,e256::reset,e257::reset,e258::reset,e259::reset,e260::reset,e261::reset,e262::reset,e263::reset,e264::reset,e265::reset,e266::reset,e267::reset,e268::reset,e269::reset,nullptr,e271::reset,e272::reset,e273::reset,e274::reset,e275::reset,e276::reset,e277::reset,e278::reset,nullptr,e280::reset,e281::reset,e282::reset,e283::reset,nullptr,e285::reset,e286::reset,nullptr,e288::reset,nullptr,e290::reset,e291::reset,nullptr,nullptr,e294::reset,e295::reset,e296::reset,e297::reset,e298::reset,e299::reset,e300::reset,nullptr,nullptr,nullptr,e304::reset,nullptr,e306::reset,nullptr,nullptr,nullptr,e310::reset,nullptr,nullptr,nullptr,e314::reset,nullptr,nullptr,nullptr,nullptr,e319::reset,e320::reset,e321::reset,nullptr,e323::reset,e324::reset,nullptr,nullptr,nullptr,e328::reset,nullptr,nullptr,e331::reset,e332::reset,nullptr,e334::reset,e335::reset,e336::reset,e337::reset,e338::reset,e339::reset,nullptr};
const char* names[]={"Boxed","Boxfit","Braid","Unavailable","Unavailable","Unavailable","Buildlwo","Bumps","Cage","Carousel","CCurve","Celtic","Unavailable","Unavailable","Circuit","CityFlow","Unavailable","Unavailable","Unavailable","Colors","Unavailable","Unavailable","Unavailable","Unavailable","Unavailable","Unavailable","Unavailable","Unavailable","Unavailable","Unavailable","Unavailable","Unavailable","Unavailable","Crackberg","Unavailable","Unavailable","Unavailable","Cube21","Cubenetic","Unavailable","Unavailable","CubeTwist","Unavailable","Cuboctaversion","Unavailable","Unavailable","Dangerball","Unavailable","Unavailable","DeepStars","Unavailable","Unavailable","Unavailable","Unavailable","Demon","Unavailable","Unavailable","Unavailable","Unavailable","Unavailable","Unavailable","Dnalogo","Unavailable","Doubletime","Unavailable","DpmsProtocol","Drift","Unavailable","Droste","DymaxionmapCoords","Easing","Endgame","Energystream","Engine","Epicycle","Unavailable","Euler2d","Fadeplot","Esper","Etruscanvenus","Unavailable","ExtIdleNotify","Extrusion","ExtrusionHelix2","ExtrusionHelix3","ExtrusionHelix4","ExtrusionJoinoffset","ExtrusionScrew","ExtrusionTaper","ExtrusionTwistoid","Exts","ExtSessionLock","Fade","FfmpegOut","Unavailable","Filmleader","Unavailable","Flag","Flame","Flipflop","Flipscreen3d","Fliptext","Flow","Fluidballs","Flurry","FlurrySmoke","Unavailable","FlurryStar","FlurryTexture","Flyingtoasters","Fontglide","Unavailable","Unavailable","Fps","FpsGl","Fuzzyflakes","Galaxy","Gallant12x22Bsd","Gears","Geodesic","Geodesicgears","Unavailable","Glmatrix","Unavailable","Unavailable","Unavailable","Unavailable","GlslUtils","Glsnake","Unavailable","Goop","Gltrackball","GlutStroke","GlutSwidth","Unavailable","GrabXimage","Grav","Gravitywell","Unavailable","Halftone","Halo","Handsy","HandsyModel","Unavailable","HeadroomModel","Unavailable","Unavailable","Unavailable","Hextrail","Unavailable","Unavailable","Hilbert","Hopalong","Hopfanimations","Hopffibration","Unavailable","Hydrostat","Hyperball","Hypercube","Hypertorus","Hypnowheel","IdleProtocol","Ifs","Imsmap","Interaggregate","Interference","Intermomentary","Involute","Jigglypuff","Jigsaw","Juggle","Juggler3d","Rotor","Rotzoomer","Unavailable","Rubik","Sballs","Scooter","Unavailable","Shadebobs","Unavailable","Slidescreen","Slip","Sonar","Speedmine","Sphere","Spheremonics","Spiral","Spotlight","Sproingies","Stairs","Unavailable","Petri","Phosphor","Unavailable","Piecewise","Unavailable","Pipeobjs","Unavailable","Platonicfolding","Unavailable","PolyhedraGl","Unavailable","Unavailable","Pong","Unavailable","Pow2","Unavailable","Unavailable","Unavailable","Providence","M6502","Unavailable","Unavailable","Marching","Maze3d","Unavailable","Memscroller","Menger","Metaballs","Unavailable","Mirrorblob","Moebius","Moebiusgears","Moire2","Moire","Morph3d","Munch","Unavailable","Nerverot","Unavailable","Normals","Unavailable","Overlay","PacmanAI","Pacman","PacmanLevel","Unavailable","Unavailable","PasswdKerberos","Unavailable","PasswdPwent","Unavailable","Unavailable","Penetrate","Unavailable","Pulsar","Pyro","Qix","Unavailable","Unavailable","Quickhull","Raverhoop","Razzledazzle","Rdbomb","Unavailable","Remote","Resources","Ripples","Robot","RobotWireframe","Rocks","Romanboy","Rorschach","Rotator","Ships","Sierpinski3d","Sierpinski","SkullModel","Skulloop","Unavailable","SphereversionAnalytic","Sphereversion","SphereversionCorrugations","Splitflap","Splodesic","Squiral","Squirtorus","Starwars","Unavailable","Strange","Substrate","Superquadrics","Surfaces","Unavailable","Swirl","T3d","Unavailable","Teapot","Unavailable","Thornbird","Timetunnel","Unavailable","Unavailable","Topblock","Trackball","Triangle","Tronbit","Truchet","Tube","Twang","Unavailable","Unavailable","Unavailable","Vfeedback","Unavailable","Vines","Unavailable","Unavailable","Unavailable","Whirlwindwarp","Unavailable","Unavailable","Unavailable","Wormhole","Unavailable","Unavailable","Unavailable","Unavailable","Xmatrix","Xrayswarm","Xspirograph","Unavailable","Zoom","Abstractile","Unavailable","Unavailable","Unavailable","Apollonian","Unavailable","Unavailable","Attraction","Atunnel","Unavailable","Binaryhorizon","Binaryring","Blaster","Blitspin","Blocktube","Bouboule","Unavailable"};
}
uint16_t Count(){return sizeof(draws)/sizeof(draws[0]);}
const char* Name(uint16_t i){return i<Count()?names[i]:"Unknown";}
bool Usable(uint16_t i){return i<Count()&&view::saver::Allowed(unsigned(i)+1)&&draws[i]&&resets[i];}
void Reset(uint16_t i){if(Usable(i)){resetRandom(0x71352u+i);resets[i]();pendingClear=true;}}
bool Render(uint16_t i,uint32_t ms,uint16_t* p,uint16_t w,uint16_t h){
 if(!Usable(i)||!p||w!=320||h!=240)return false;
 raster.bind(p,w,h); if(pendingClear){raster.fillScreen(BLACK);pendingClear=false;} animationTime=(ms%3600000)/1000.0f;
 draws[i](); raster.unbind();return true;
}
}
