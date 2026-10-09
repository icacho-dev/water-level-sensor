#include <Adafruit_LiquidCrystal.h>

Adafruit_LiquidCrystal lcd(0);

#define trigPin 2
#define echoPin 3

// LCD display variables
String row_1 = "";
String row_2 = "";

// Sensor distance metrics
long duration;
float distance;
int distance_fill;

// Tank physical properties (cm)
const long container_width = 200;
const long container_height = 200;
const long container_depth = 100; // Usable water depth limit
const long container_volume = container_width * container_height * container_depth; // 4,000,000 cm³

// Physical installation offsets
const int sensor_offset = 28;     // Air gap distance from sensor down to the 100% full line
const int max_distance = sensor_offset + container_depth; // 128 cm (Total distance to empty bottom)

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.begin(16, 2);
  
  row_1.reserve(16);
  row_2.reserve(16);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Print persistent label showing the maximum capacity in Liters (4000L)
  row_1 = "Max V: ";
  row_1 += (container_volume / 1000);
  row_1 += "L";

  lcd.setCursor(0, 0);
  lcd.print(row_1);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  
  // Trigger the ultrasonic ping
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance in centimeters
  distance = (float)duration / 58.2; 
  
  // Constrain sensor bounds to the real physical dimensions of your installation
  if (distance < sensor_offset) distance = sensor_offset;
  if (distance > max_distance) distance = max_distance;

  // Calculate actual water column height from the bottom up
  float water_height = max_distance - distance;

  // Calculate fill percentage relative to the water capacity limit
  float distance_percentage = (water_height / (float)container_depth) * 100.0;
  distance_fill = (int)distance_percentage;
  
  // Calculate volume directly in Liters
  float current_volume_liters = ((float)container_volume * (distance_percentage / 100.0)) / 1000.0;
  
  // Serial Monitor telemetry for alignment testing
  Serial.print("Dist: "); Serial.print(distance);
  Serial.print("cm | Height: "); Serial.print(water_height);
  Serial.print("cm | Fill: "); Serial.print(distance_fill);
  Serial.print("% | Vol: "); Serial.println(current_volume_liters);
  
  // Clear only Row 2 to prevent display flicker anomalies
  lcd.setCursor(0, 1);
  lcd.print("                "); 
  
  // Format layout display string: e.g., "2800L  70%"
  lcd.setCursor(0, 1);
  lcd.print((long)current_volume_liters);
  lcd.print("L ");
  lcd.setCursor(9, 1); // Positions percentage text evenly to the right side
  lcd.print(distance_fill);
  lcd.print("%");

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000); 
}