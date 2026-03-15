/**
 * Use the QV1ckenIR as an IR dongle over USB serial, and log everything
 */

#include <Arduino.h>

#include "FFat.h"
#include "config.h"
#include "irda_hal.h"

const char* TAG = "Main";

void setup() {
    USBSerial.begin(BAUDRATE);
    // Attempt to avoid the DTS/RTS reboot
    USBSerial.enableReboot(false);
    USBSerial.setDebugOutput(false);

    // Doing this means it doesn't start until serial connected?
    // while (!USBSerial);

    pinMode(PIN_LED, OUTPUT);

#ifdef PIN_IRDA_SD
    // TFDU4101 Shutdown pin, powered by bus power, safe to tie to ground
    pinMode(PIN_IRDA_SD, OUTPUT);
    digitalWrite(PIN_IRDA_SD, LOW);
    delay(1);
#endif

    IRDA_setup(IRDA);
    while (!IRDA);

    digitalWrite(PIN_LED, LED_OFF);
}

static const size_t SERIAL_BUFFER_SIZE = 4096;
static uint8_t SERIAL_BUFFER[SERIAL_BUFFER_SIZE]{};

static bool b;

void loop() {
    size_t avail = USBSerial.available();
    if (avail > 0) {
        // digitalWrite(LED_BUILTIN, LED_ON);
        avail = USBSerial.readBytes(SERIAL_BUFFER, min(avail, SERIAL_BUFFER_SIZE));
        IRDA_tx(true);
        IRDA.write(SERIAL_BUFFER, avail);
        IRDA.flush();
        IRDA_tx(false);
    }

    avail = IRDA.available();
    if (avail > 0) {
        digitalWrite(LED_BUILTIN, LED_ON);
        avail = IRDA.readBytes(SERIAL_BUFFER, min(avail, SERIAL_BUFFER_SIZE));
        USBSerial.write(SERIAL_BUFFER, avail);
        USBSerial.flush();
    }

    digitalWrite(LED_BUILTIN, LED_OFF);
}
