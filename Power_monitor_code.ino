#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Servo myServo;

// ---------------- Pins ----------------
#define POT_PIN A0
#define CURRENT_PIN A1

#define GREEN_LED 2
#define YELLOW_LED 3
#define RED_LED 4
#define BUZZER 5
#define SERVO_PIN 6

// ------------ Servo Variables ----------
int angle = 0;
bool forward = true;

void setup()
{
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  myServo.attach(SERVO_PIN);
}

void loop()
{
  // ---------------- Servo Movement ----------------
  myServo.write(angle);

  if (forward)
    angle++;
  else
    angle--;

  if (angle >= 180)
    forward = false;

  if (angle <= 0)
    forward = true;

  // ---------------- Battery Voltage ----------------
  int sensor = analogRead(POT_PIN);

  float voltage = map(sensor, 0, 1023, 1400, 1680) / 100.0;

  int percent = map(sensor, 0, 1023, 0, 100);

  // ---------------- Current Averaging ----------------
  long sum = 0;

  for (int i = 0; i < 50; i++)
  {
    sum += analogRead(CURRENT_PIN);
  }

  int raw = sum / 50;

  float current = abs(raw - 519) * (5.0 / 1023.0) / 0.066;

  float power = voltage * current;
    // ---------------- LEDs & Buzzer ----------------
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  String status;

  if (percent > 40)
  {
    digitalWrite(GREEN_LED, HIGH);
    status = "NORMAL";
  }
  else if (percent > 20)
  {
    digitalWrite(YELLOW_LED, HIGH);
    status = "LOW";
  }
  else
  {
    status = "CRITICAL";

    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);
  }

  // ---------------- OLED ----------------
  display.clearDisplay();

  display.setCursor(20, 0);
  display.println("XO11 UAV");

  display.setCursor(0, 12);
  display.print("Voltage : ");
  display.print(voltage, 2);
  display.println("V");

  display.setCursor(0, 24);
  display.print("Battery : ");
  display.print(percent);
  display.println("%");

  display.setCursor(0, 36);
  display.print("Current : ");
  display.print(current, 2);
  display.println("A");

  display.setCursor(0, 48);
  display.print("Power   : ");
  display.print(power, 1);
  display.println("W");

  // Battery Icon
  display.drawRect(95, 10, 28, 10, WHITE);
  display.drawRect(123, 13, 3, 4, WHITE);

  int fill = map(percent, 0, 100, 0, 24);
  display.fillRect(97, 12, fill, 6, WHITE);

  display.display();

  // ---------------- Serial Monitor ----------------
  Serial.print("Voltage: ");
  Serial.print(voltage, 2);

  Serial.print("V  Battery: ");
  Serial.print(percent);

  Serial.print("%  Current: ");
  Serial.print(current, 2);

  Serial.print("A  Power: ");
  Serial.print(power, 1);
  Serial.println("W");

  delay(20);
}