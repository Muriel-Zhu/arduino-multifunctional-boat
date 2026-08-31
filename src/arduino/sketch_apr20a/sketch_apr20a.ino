#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =======================================================
// PROJECT NEREUS - 科考船操作系统 v3.0 (带雷达显示屏)
// =======================================================

// --- 屏幕配置 ---
#define SCREEN_WIDTH 128 // OLED 显示宽度，以像素为单位
#define SCREEN_HEIGHT 64 // OLED 显示高度，以像素为单位
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- 引脚分配 ---
const int JOY_Y_PIN = A0;   // 摇杆 Y 轴
const int JOY_X_PIN = A1;   // 摇杆 X 轴
const int BTN_MODE_PIN = 2; // 摇杆按键

const int SERVO_WINCH_PIN = 3;  // 起重机舵机
const int SERVO_STEER_PIN = 5;  // 转向舵机

// 根据你的驱动板更新了引脚名称 (D0/D1 对应 6和11)
const int DRIVER_D0_PIN = 6;    // ZK-5AD D0 (前进)
const int DRIVER_D1_PIN = 11;   // ZK-5AD D1 (后退)

const int TRIG_PIN = 9;         // HC-SR04 超声波 Trig
const int ECHO_PIN = 10;        // HC-SR04 超声波 Echo

// --- 核心控制对象 ---
Servo winchServo;
Servo steerServo;

enum SystemMode { SAIL, CRANE };
SystemMode currentMode = SAIL;

int winchPos = 90; 
int steerPos = 90; 
unsigned long lastButtonPress = 0;

// 屏幕刷新计时器 (防止屏幕刷新拖慢舵机)
unsigned long lastDisplayUpdate = 0;
float currentDistance = 0.0;
int currentEngineSpeed = 0;
String engineStatus = "STOP";

void setup() {
  Serial.begin(9600); 
  
  // 初始化引脚
  pinMode(BTN_MODE_PIN, INPUT_PULLUP); 
  pinMode(DRIVER_D0_PIN, OUTPUT);
  pinMode(DRIVER_D1_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // 初始化舵机
  winchServo.attach(SERVO_WINCH_PIN);
  steerServo.attach(SERVO_STEER_PIN);
  winchServo.write(winchPos);
  steerServo.write(steerPos);

  // 确保电机静止
  analogWrite(DRIVER_D0_PIN, 0);
  analogWrite(DRIVER_D1_PIN, 0);

  // 初始化 OLED 屏幕
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 一般地址为 0x3C
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // 屏幕初始化失败则卡住
  }
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println(F("NEREUS OS"));
  display.setTextSize(1);
  display.setCursor(10, 45);
  display.println(F("Booting..."));
  display.display();
  delay(1500); // 显示开机画面 1.5 秒
}

void loop() {
  // 1. 读取摇杆数据
  int joyX = analogRead(JOY_X_PIN); 
  int joyY = analogRead(JOY_Y_PIN); 

  // 2. 模式切换逻辑
  if (digitalRead(BTN_MODE_PIN) == LOW) {
    if (millis() - lastButtonPress > 500) { 
      currentMode = (currentMode == SAIL) ? CRANE : SAIL;
      
      // 切换模式安全急停
      analogWrite(DRIVER_D0_PIN, 0);
      analogWrite(DRIVER_D1_PIN, 0);
      engineStatus = "STOP";
      
      lastButtonPress = millis();
    }
  }

  // 3. 超声波探测 (只负责测距，不打印串口，避免卡顿)
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
  currentDistance = duration * 0.034 / 2;

  // 4. 转向系统
  if (joyX > 600) steerPos += 2;
  else if (joyX < 400) steerPos -= 2;
  steerPos = constrain(steerPos, 40, 140); 
  steerServo.write(steerPos);

  // 5. 动力核心分流
  if (currentMode == SAIL) {
    if (joyY > 600) {
      currentEngineSpeed = map(joyY, 600, 1023, 0, 255);
      analogWrite(DRIVER_D0_PIN, currentEngineSpeed);
      analogWrite(DRIVER_D1_PIN, 0);
      engineStatus = "FWD>>"; // 前进
    } else if (joyY < 400) {
      currentEngineSpeed = map(joyY, 400, 0, 0, 255);
      analogWrite(DRIVER_D0_PIN, 0);
      analogWrite(DRIVER_D1_PIN, currentEngineSpeed);
      engineStatus = "<<REV"; // 倒车
    } else {
      analogWrite(DRIVER_D0_PIN, 0);
      analogWrite(DRIVER_D1_PIN, 0);
      engineStatus = "STOP";
    }
  } else {
    // 起重机模式
    if (joyY > 600) winchPos += 2; 
    else if (joyY < 400) winchPos -= 2; 
    winchPos = constrain(winchPos, 0, 180);
    winchServo.write(winchPos);
    engineStatus = "LOCKED"; // 主引擎锁死
  }

  // 6. 异步刷新 OLED 屏幕 (每 150ms 刷新一次，保证舵机丝滑不卡)
  if (millis() - lastDisplayUpdate > 150) {
    updateScreen();
    lastDisplayUpdate = millis();
  }

  delay(20); 
}

// 屏幕绘制函数
void updateScreen() {
  display.clearDisplay();
  
  // 第一行：当前模式
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("MODE: "));
  display.setTextSize(2);
  display.setCursor(35, 0);
  if (currentMode == SAIL) {
    display.print(F("SAIL"));
  } else {
    display.print(F("CRANE"));
  }
  
  // 画一条分割线
  display.drawLine(0, 18, 128, 18, SSD1306_WHITE);

  // 第二行：雷达距离
  display.setTextSize(1);
  display.setCursor(0, 25);
  display.print(F("RADAR: "));
  if (currentDistance == 0 || currentDistance > 200) {
    display.print(F("CLEAR"));
  } else {
    display.print(currentDistance, 1);
    display.print(F(" cm"));
    // 危险警告框
    if (currentDistance < 15.0) {
      display.fillRect(90, 23, 38, 11, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setCursor(92, 25);
      display.print(F("WARN!"));
      display.setTextColor(SSD1306_WHITE); // 恢复白色字
    }
  }

  // 第三行：主引擎状态
  display.setCursor(0, 40);
  display.print(F("ENG: "));
  display.print(engineStatus);
  if (currentMode == SAIL && engineStatus != "STOP") {
    display.print(F(" PWR:"));
    int pwrPercent = map(currentEngineSpeed, 0, 255, 0, 100);
    display.print(pwrPercent);
    display.print(F("%"));
  }

  // 第四行：舵机状态
  display.setCursor(0, 55);
  display.print(F("STR:"));
  display.print(steerPos);
  display.print(F("   WINCH:"));
  display.print(winchPos);

  display.display();
}