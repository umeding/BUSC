/***********************************************************************************************/
// VLC Texting - Robust Non-Blocking Implementation for Arduino Leonardo
// Hardware: Arduino Leonardo (TX/RX on Pins 0/1 -> Serial1, USB Serial -> Serial)
//
// 2026-10-27 uwe <umeding@bu,edu> 
/***********************************************************************************************/

constexpr size_t BUFFER_SIZE = 128;
constexpr unsigned long RX_TIMEOUT_MS = 250; // Resets buffer if line breaks mid-frame

char rxBuffer[BUFFER_SIZE];
size_t rxIndex = 0;
unsigned long lastRxTime = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  // Wait for the USB CDC monitor connection
  while (!Serial) {
    ; // Leonardo requirement for USB CDC
  }

  Serial.println(F("--- VLC Optical Transceiver Ready ---"));
}

void loop() {
  handleUsbToOptical();
  handleOpticalToUsb();
}

// Read from USB Serial Monitor and transmit over optical link (Serial1)
void handleUsbToOptical() {
  static char txBuffer[BUFFER_SIZE];
  static size_t txIndex = 0;

  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\r') {
      continue; // Normalize CRLF: drop carriage returns
    }

    if (c == '\n') {
      if (txIndex > 0) {
        txBuffer[txIndex] = '\0';
        Serial.print(F("Sent: "));
        Serial.println(txBuffer);

        Serial1.write((const uint8_t*)txBuffer, txIndex);
        Serial1.write('\n');

        txIndex = 0;
      }
    } else {
      if (txIndex < BUFFER_SIZE - 1) {
        txBuffer[txIndex++] = c;
      } else {
        // Line exceeded buffer capacity; flush frame
        txBuffer[txIndex] = '\0';
        Serial.print(F("Sent (Buffer limit reached): "));
        Serial.println(txBuffer);

        Serial1.write((const uint8_t*)txBuffer, txIndex);
        Serial1.write('\n');

        txIndex = 0;
      }
    }
  }
}

// Non-blocking read from optical receiver (Serial1) and output to USB Serial
void handleOpticalToUsb() {
  // Check for line loss mid-message
  if (rxIndex > 0 && (millis() - lastRxTime > RX_TIMEOUT_MS)) {
    rxBuffer[rxIndex] = '\0';
    Serial.print(F("[Framing Error / Link Interrupted]: Partial = "));
    Serial.println(rxBuffer);
    rxIndex = 0;
  }

  while (Serial1.available() > 0) {
    char c = Serial1.read();
    lastRxTime = millis();

    // Filter out typical optical noise / low-voltage baseline drifts
    if (c == '\0' || (uint8_t)c > 127) {
      continue;
    }

    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      if (rxIndex > 0) {
        rxBuffer[rxIndex] = '\0';
        Serial.print(F("Received: "));
        Serial.println(rxBuffer);
        rxIndex = 0;
      }
    } else {
      if (rxIndex < BUFFER_SIZE - 1) {
        rxBuffer[rxIndex++] = c;
      } else {
        // Buffer saturated without finding newline delimiter
        rxBuffer[rxIndex] = '\0';
        Serial.print(F("Received (Overflow): "));
        Serial.println(rxBuffer);
        rxIndex = 0;
      }
    }
  }
}
