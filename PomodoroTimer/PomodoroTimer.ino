// ============================================
// Pomodoro Timer mit Abstandskontrolle
// HC-SR04 Ultraschall-Sensor + I2C LCD 20x4
// ============================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Pin-Definitionen ---
const int TRIG_PIN = 7;
const int ECHO_PIN = 6;

// --- LCD (I2C Adresse 0x27, 20 Zeichen, 4 Zeilen) ---
LiquidCrystal_I2C lcd(0x27, 20, 4);

// --- Pomodoro Einstellungen ---
const int ARBEITSZEIT_SEK = 25 * 60;  // 25 Minuten
const int PAUSENZEIT_SEK  =  5 * 60;  //  5 Minuten
const int ANZAHL_RUNDEN   = 2;

// --- Abstandsgrenzen (cm) ---
const int ABSTAND_MIN = 45;
const int ABSTAND_MAX = 75;

// ============================================
// checkDistance()
// Steuert den HC-SR04 an und prueft ob der
// gemessene Abstand zwischen 45 und 75 cm liegt.
// Gibt true zurueck wenn ja, sonst false.
// ============================================
bool checkDistance() {
  // Trigger-Impuls senden
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Echo messen (Timeout 30ms ~ ca. 5m)
  long dauer = pulseIn(ECHO_PIN, HIGH, 30000);

  // Entfernung berechnen (Schallgeschwindigkeit 343 m/s)
  float entfernung_cm = dauer * 0.0343 / 2.0;

  // Pruefen ob im gueltigen Bereich
  if (entfernung_cm >= ABSTAND_MIN && entfernung_cm <= ABSTAND_MAX) {
    return true;
  } else {
    return false;
  }
}

// ============================================
// Zeigt die Zeit auf dem LCD an (Zeile 2)
// Format: MM:SS
// ============================================
void zeigeZeit(int sekunden, const char* label) {
  int minuten = sekunden / 60;
  int sek = sekunden % 60;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(label);

  lcd.setCursor(0, 1);
  char zeitStr[9];
  sprintf(zeitStr, "  %02d:%02d", minuten, sek);
  lcd.print(zeitStr);
}

// ============================================
// Timer-Countdown mit Abstandspruefung
// Gibt true zurueck wenn komplett durchgelaufen,
// false bei Abbruch wegen Abwesenheit.
// ============================================
bool timerMitPruefung(int dauerSek, const char* label) {
  int verbleibend = dauerSek;

  while (verbleibend >= 0) {
    zeigeZeit(verbleibend, label);

    // Abstandspruefung
    if (!checkDistance()) {
      // Erste Messung fehlgeschlagen -> 9 Messungen ueber 3 Sek
      // Timer laeuft weiter: alle 3 Messungen = 1 Sekunde
      lcd.setCursor(0, 2);
      lcd.print("Bist du noch da?");

      int fehlschlaege = 1;  // erste Messung war schon falsch

      for (int i = 1; i < 9; i++) {
        delay(333);  // 3 Messungen pro Sekunde

        if (!checkDistance()) {
          fehlschlaege++;
        }

        // Alle 3 Messungen: 1 Sekunde vergangen -> Timer aktualisieren
        if (i % 3 == 2) {
          verbleibend--;
          if (verbleibend >= 0) {
            zeigeZeit(verbleibend, label);
            lcd.setCursor(0, 2);
            lcd.print("Bist du noch da?");
          }
        }
      }

      if (fehlschlaege >= 9) {
        // Alle 9 Messungen fehlgeschlagen -> Abbruch
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Abbruch wegen");
        lcd.setCursor(0, 1);
        lcd.print("Abwesenheit");
        return false;
      }
      // Mindestens eine Messung war ok -> weiter
      lcd.setCursor(0, 2);
      lcd.print("                    ");
      // Die 3 Sekunden sind schon vergangen, naechste Iteration
      verbleibend--;
      continue;
    }

    // 1 Sekunde warten
    delay(1000);
    verbleibend--;
  }
  return true;
}

// ============================================
// Pause-Countdown (ohne Abstandspruefung)
// ============================================
void pauseTimer(int dauerSek) {
  for (int verbleibend = dauerSek; verbleibend >= 0; verbleibend--) {
    zeigeZeit(verbleibend, "-- PAUSE --");
    lcd.setCursor(0, 2);
    lcd.print("Entspann dich!");
    delay(1000);
  }
}

// ============================================
// SETUP
// ============================================
void setup() {
  // Pins konfigurieren
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // LCD starten
  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pomodoro Timer");
  lcd.setCursor(0, 1);
  lcd.print("Starte in 5 Sek...");
  delay(5000);
}

// ============================================
// LOOP
// ============================================
void loop() {
  for (int runde = 1; runde <= ANZAHL_RUNDEN; runde++) {
    // Arbeitsphase
    char label[21];
    sprintf(label, "Runde %d/%d - Arbeit", runde, ANZAHL_RUNDEN);

    bool erfolg = timerMitPruefung(ARBEITSZEIT_SEK, label);

    if (!erfolg) {
      // Abbruch -> 10 Sek anzeigen, dann neustart
      delay(10000);
      return;  // loop() startet von vorne
    }

    // Pause (nach letzter Runde keine Pause)
    if (runde < ANZAHL_RUNDEN) {
      pauseTimer(PAUSENZEIT_SEK);
    }
  }

  // Alle Runden geschafft!
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("***  Geschafft!  ***");
  lcd.setCursor(0, 1);
  lcd.print("Gut gemacht!");
  lcd.setCursor(0, 3);
  lcd.print("Neustart in 30 Sek");
  delay(30000);
}
