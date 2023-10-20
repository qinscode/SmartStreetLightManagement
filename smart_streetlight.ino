#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>


const int lightSensorPin = A2;
const int motionSensorPin = 4;
const int ledPin = 3;

// TTN network session key, application session key, and device address
static const PROGMEM u1_t NWKSKEY[16] = { 0x8D, 0xA0, 0x97, 0x14, 0x66, 0x75, 0x6E, 0x3F, 0x5D, 0xC8, 0xDF, 0x6A, 0x56, 0x61, 0x81, 0x1B };
static const u1_t PROGMEM APPSKEY[16] = { 0x5D, 0x75, 0xAB, 0xBB, 0x16, 0x60, 0x3B, 0x5D, 0xE5, 0xEA, 0xF4, 0x50, 0xE5, 0x48, 0x3E, 0xC8 };
static const u4_t DEVADDR = 0x260D93B8;

void os_getArtEui(u1_t *buf) {}
void os_getDevEui(u1_t *buf) {}
void os_getDevKey(u1_t *buf) {}

static const char mydata[] = "{\"switch\": \"1\",\"brightness\": \"75%\",\"event\": \"Starting Light System!!!!!!!!!!!\",\"maintenance\": 0}";
byte payload[4];
bool sendDataEnabled = false;

// light
const int ANALOG_THRESHOLD = 500;  // minimum light level
const int maintenenceThreshold = 50;
int analogValue;  // storing light level
// motion
int motionStateCurrent = LOW;   // current state of motion sensor's pin
int motionStatePrevious = LOW;  // previous state of motion sensor's pin
int ledBrightness = 25;         // initial LED brightness



osjob_t sendjob;
unsigned int interval = 6;  // Interval for low brightness (6 seconds)

const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 9,
  .dio = { 2, 6, 7 },
};

void onEvent(ev_t ev) {
  switch (ev) {
    case EV_TXCOMPLETE:
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ACK"));
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
      }
      // Schedule the next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(interval), do_send);
      // Turn on LED at full brightness on motion detection
      if (digitalRead(motionSensorPin)) {
        analogWrite(ledPin, 255);
        delay(6000);
        Serial.println("Motion Detected!");
      } else {
        // Turn on LED at low brightness in darkness
        analogWrite(ledPin, 25);
        delay(6000);
      }
      break;
    default:
      break;
  }
}

void do_send(osjob_t *j) {
  // Read light sensor
  int lightValue = analogRead(lightSensorPin);

  // Read motion sensor
  int motionValue = digitalRead(motionSensorPin);

  // payload[0] = highByte(lightValue);
  // payload[1] = lowByte(lightValue);
  // payload[2] = motionValue;
  // payload[3] = 0; // Placeholder for future data
  // payload[0] = lightValue;
  // payload[1] = motionStateCurrent;
  // payload[2] = ledBrightness;
  // payload[3] = 0;  // Placeholder for future data
  Serial.print(sendDataEnabled);
  Serial.print(" ");
  Serial.print(mydata);
  Serial.print(" ");
  Serial.println(sizeof(mydata) - 1);
  // Send the payload
  if (sendDataEnabled) {
    LMIC_setTxData2(1, mydata, sizeof(mydata) - 1, 0);
    Serial.println("Data Sent!!");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(motionSensorPin, INPUT);  // set arduino pin to input mode
  pinMode(ledPin, OUTPUT);          // set arduino pin to output mode
  Serial.println("Start!");
  analogWrite(ledPin, ledBrightness);  // set initial LED brightness


  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
// Set static session parameters. Instead of dynamically establishing a session
// by joining the network, precomputed session parameters are be provided.
#ifdef PROGMEM
  // On AVR, these values are stored in flash and only copied to RAM
  // once. Copy them to a temporary buffer here, LMIC_setSession will
  // copy them into a buffer of its own again.
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NWKSKEY)];
  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  LMIC_setSession(0x13, DEVADDR, nwkskey, appskey);
#else
  // If not running an AVR with PROGMEM, just use the arrays directly
  LMIC_setSession(0x13, DEVADDR, NWKSKEY, APPSKEY);
#endif
#if defined(CFG_eu868)


  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);   // g-band
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);  // g-band
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);   // g-band
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);   // g-band
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);   // g-band
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);   // g-band
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);   // g-band
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);   // g-band
  LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK, DR_FSK), BAND_MILLI);    // g2-band

#elif defined(CFG_us915) || defined(CFG_au915)

  LMIC_selectSubBand(1);
#elif defined(CFG_as923)

#elif defined(CFG_kr920)

#elif defined(CFG_in866)

#else
#error Region not supported
#endif
  // Disable link check validation
  LMIC_setLinkCheckMode(0);
  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;
  LMIC_setClockError(MAX_CLOCK_ERROR * 5 / 100);
  // Set data rate and transmit power for uplink
  LMIC_setDrTxpow(DR_SF7, 14);
  // Start job
  do_send(&sendjob);


  //   os_init();
  //   LMIC_reset();
  //   LMIC_setSession(0x1, DEVADDR, NWKSKEY, APPSKEY);
  //   os_setCallback(&sendjob, do_send);

  //   // Disable link check validation
  //   LMIC_setLinkCheckMode(0);

  //   // TTN uses SF9 for its RX2 window.
  //   LMIC.dn2Dr = DR_SF9;

  //   LMIC_setClockError(MAX_CLOCK_ERROR * 5 / 100);

  //   // Set data rate and transmit power for uplink
  //   LMIC_setDrTxpow(DR_SF7, 14);

  //   // Start job
  //   do_send(&sendjob);
}

void loop() {

  analogValue = analogRead(lightSensorPin);
  int voltage = analogRead(ledPin);

  motionStatePrevious = motionStateCurrent;           // store old state
  motionStateCurrent = digitalRead(motionSensorPin);  // read new state



  if (analogValue < ANALOG_THRESHOLD) {  // if ambient light level less than threshold
    // digitalWrite(LED_PIN, HIGH); // turn on LED
    // Serial.println("Turn on light");
    // analogWrite(LED_PIN, ledBrightness); // set initial LED brightness
    if (motionStatePrevious == LOW && motionStateCurrent == HIGH) {  // pin state change: LOW -> HIGH
      Serial.println("Motion detected!");

      static char newData[] = "{\"switch\": \"1\",\"brightness\": \"100%\",\"event\": \"Motion detected\",\"maintenance\": 0}";
      strcpy(mydata, newData);
      // Serial.println(mydata);
      digitalWrite(ledPin, HIGH);  // turn on
      sendDataEnabled = true;
      do_send(&sendjob);
      sendDataEnabled = false;
    } else if (motionStatePrevious == HIGH && motionStateCurrent == LOW) {  // pin state change: HIGH -> LOW
      Serial.println("Motion stopped!");
      // digitalWrite(LED_PIN, LOW);  // turn off
      // Decrease LED brightness gradually
      for (int i = 255; i >= 25; i--) {
        ledBrightness = i;
        analogWrite(ledPin, ledBrightness);
        delay(10);  // Adjust the delay to control the speed of brightness change
      }
      static char newData[] = "{\"switch\": \"1\",\"brightness\": \"75%\",\"event\": \"Motion Stopped!\",\"maintenance\": 0}";
      strcpy(mydata, newData);
      sendDataEnabled = true;
      do_send(&sendjob);
      sendDataEnabled = false;
    } else if (motionStateCurrent == LOW) {
      analogWrite(ledPin, 25);
    }

    if (analogValue < maintenenceThreshold) {
      Serial.print("LED broken");
      static char newData[] = "{\"switch\": \"0\",\"brightness\": \"0%\",\"event\": \"Maintenance Need!\",\"maintenance\": 1}";
      strcpy(mydata, newData);
      sendDataEnabled = true;
      do_send(&sendjob);
      sendDataEnabled = false;
    }

  } else {
    digitalWrite(ledPin, LOW);  // turn off LED

  }
  // os_runloop_once();
  os_runloop_once();
}
