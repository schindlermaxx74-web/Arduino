# Pomodoro Timer mit Abstandskontrolle - Bauanleitung

## Bauteile

| Bauteil              | Anzahl |
|----------------------|--------|
| Arduino Uno/Nano     | 1      |
| HC-SR04 Ultraschall  | 1      |
| I2C LCD 20x4         | 1      |
| Jumper-Kabel         | 8      |

## Verkabelung

### HC-SR04 Ultraschall-Sensor

| HC-SR04 Pin | Arduino Pin |
|-------------|-------------|
| VCC         | 5V          |
| Trig        | Digital 7   |
| Echo        | Digital 6   |
| GND         | GND         |

### I2C LCD Display (20x4)

| LCD Pin | Arduino Pin |
|---------|-------------|
| VCC     | 5V          |
| GND     | GND         |
| SDA     | A4          |
| SCL     | A5          |

## Schaltplan (Textform)

```
Arduino Uno
  ├── 5V ──────┬──── HC-SR04 VCC
  │            └──── LCD VCC
  ├── GND ─────┬──── HC-SR04 GND
  │            └──── LCD GND
  ├── D7 ──────────── HC-SR04 Trig
  ├── D6 ──────────── HC-SR04 Echo
  ├── A4 (SDA) ────── LCD SDA
  └── A5 (SCL) ────── LCD SCL
```

## Benoetigte Arduino-Bibliothek

- **LiquidCrystal_I2C** (in der Arduino IDE unter Bibliotheken verwalten installieren)

## LCD I2C-Adresse

Der Code verwendet `0x27` als Standard-Adresse. Falls das Display nichts anzeigt,
probiere `0x3F` (zweithaeufigste Adresse). Mit dem I2C-Scanner-Sketch
laesst sich die richtige Adresse herausfinden.

## Funktionsweise

1. **Start**: Display zeigt "Pomodoro Timer" und startet nach 5 Sekunden
2. **Arbeitsphase**: 25-Minuten-Countdown laeuft auf dem Display (MM:SS)
3. **Abstandspruefung**: Jede Sekunde wird geprueft ob du im Bereich 45-75 cm sitzt
4. **Bei Abwesenheit**: "Bist du noch da?" erscheint, 3 Sekunden Wartezeit,
   dann erneute Pruefung. Bei erneutem Fehlschlag -> "Abbruch wegen Abwesenheit"
5. **Pause**: Nach erfolgreichen 25 Min laeuft ein 5-Min-Pause-Timer
6. **2 Runden**: Nach 2 abgeschlossenen Arbeitsrunden zeigt das Display "Geschafft!"
