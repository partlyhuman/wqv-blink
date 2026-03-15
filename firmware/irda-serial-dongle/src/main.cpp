/**
 * Use the QV1ckenIR as an IR dongle over USB serial, and log everything
 */

#include <Arduino.h>

#include "FFat.h"
#include "config.h"
#include "irda_hal.h"

const char* TAG = "Main";

void setup() {
    USBSerial.begin(115200);
    // Attempt to avoid the DTS/RTS reboot
    USBSerial.setRxBufferSize(1024);
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

    FFat.begin(true);
}

static const size_t SERIAL_BUFFER_SIZE = 4096;
static byte SERIAL_BUFFER[SERIAL_BUFFER_SIZE]{};

void loop() {
    size_t avail = USBSerial.available();
    if (avail > 0) {
        avail = USBSerial.readBytes(SERIAL_BUFFER, min(avail, SERIAL_BUFFER_SIZE));
        IRDA_tx(true);
        IRDA.write(SERIAL_BUFFER, avail);
        IRDA_tx(false);
    }

    avail = IRDA.available();
    if (avail > 0) {
        avail = IRDA.readBytes(SERIAL_BUFFER, min(avail, SERIAL_BUFFER_SIZE));
        USBSerial.write(SERIAL_BUFFER, avail);
    }
}
