#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Pin definitions for ESP32
#define MODEM_RX 16
#define MODEM_TX 17
#define BUTTON_ADD 33
#define BUTTON_DELETE 32

// Setup serial communication
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD I2C address and dimensions

const uint8_t MIN_ID = 1;
const uint8_t MAX_ID = 127;
uint8_t id;

void setup() {
  Serial.begin(9600);  // Debugging
  Serial2.begin(57600, SERIAL_8N1, MODEM_RX, MODEM_TX);
  
  lcd.init();
  lcd.backlight(); 
  lcd.setCursor(0, 0);
  lcd.print("Nut: Them / Xoa");

  pinMode(BUTTON_ADD, INPUT_PULLUP);
  pinMode(BUTTON_DELETE, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BUTTON_ADD) == LOW) {
    handleAddFingerprint();
  }

  if (digitalRead(BUTTON_DELETE) == LOW) {
    handleDeleteFingerprint();
  }
}

void handleAddFingerprint() {
  Serial.println("Nut Them Van Tay Duoc Nhan!");
  lcd.clear();
  lcd.print("Them Van Tay");
  
  id = promptForID("Vui long nhap ID # (tu " + String(MIN_ID) + " den " + String(MAX_ID) + "):");
  if (isValidID(id)) {
    enrollFingerprint();
  }
  lcd.clear();
    lcd.setCursor(0, 0);
  lcd.print("Nut: Them / Xoa");

}

void handleDeleteFingerprint() {
  Serial.println("Nut Xoa Van Tay Duoc Nhan!");
  lcd.clear();
  lcd.print("Xoa Van Tay");

  id = promptForID("Vui long nhap ID # de xoa (tu " + String(MIN_ID) + " den " + String(MAX_ID) + "):");
  if (isValidID(id)) {
    deleteFingerprint();
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nut: Them / Xoa");
}

uint8_t promptForID(String prompt) {
  Serial.println(prompt);
  return readNumber();
}

bool isValidID(uint8_t id) {
  if (id < MIN_ID || id > MAX_ID) {
    Serial.println("ID khong hop le, vui long thu lai.");
    return false;
  }
  return true;
}

int readNumber() {
  while (!Serial.available()) {
    delay(10);
  }
  return Serial.parseInt();
}

void waitForFinger() {
  int p = finger.getImage();
  while (p != FINGERPRINT_OK) {
    Serial.print("Trang thai cam bien: "); Serial.println(p);
    if (p == FINGERPRINT_NOFINGER) {
      Serial.print(".");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Loi giao tiep");
    } else if (p == FINGERPRINT_IMAGEFAIL) {
      Serial.println("Loi chup hinh");
    }
    delay(500);
    p = finger.getImage();
  }
  Serial.println("\nHinh da duoc chup");
}

uint8_t enrollFingerprint() {
  Serial.print("Chuan bi cho ngon tay hop le de ghi danh la #");
  Serial.println(id);
  
  waitForFinger();
  
  if (finger.image2Tz(1) != FINGERPRINT_OK) {
    Serial.println("Khong the chuyen doi hinh anh");
    return -1;
  }
  
  Serial.println("Lay ngon tay ra");
  delay(2000);
  Serial.println("Dat ngon tay giong lai");
  
  waitForFinger();
  if (finger.image2Tz(2) != FINGERPRINT_OK) {
    Serial.println("Khong the chuyen doi hinh anh thu hai");
    return -1;
  }

  if (finger.createModel() == FINGERPRINT_OK) {
    if (finger.storeModel(id) == FINGERPRINT_OK) {
      Serial.println("Van tay da duoc luu thanh cong!");
    } else {
      Serial.println("Khong the luu van tay");
    }
  } else {
    Serial.println("Van tay khong khop");
  }

  return 0;
}

uint8_t deleteFingerprint() {
  Serial.print("Xoa van tay ID #");
  Serial.println(id);
  
  if (finger.deleteModel(id) == FINGERPRINT_OK) {
    Serial.println("Xoa van tay thanh cong!");
  } else {
    Serial.println("Khong the xoa van tay");
    return -1;
  }

  return 0;
}
