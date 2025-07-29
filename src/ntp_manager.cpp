/*
 * Copyright (c) 2025, Sandra Bati Furquim
 * All rights reserved.
 *
 * This file is released under the MIT License.
 * See LICENSE file for details.
 */
#include "globals.h"
#include "modulo.h"

void Setup_NTP() {
    configTime(0, 0, "pool.ntp.org"); // UTC

    Serial.println("Sincronizando com NTP...");
    struct tm timeinfo;
    int retries = 0;

    while (!getLocalTime(&timeinfo) && retries < 10) {
        Serial.print(".");
        delay(500);
        retries++;
    }

    if (retries < 10) {
        Serial.println("\nHora sincronizada com sucesso.");
    } else {
        Serial.println("\nFalha ao sincronizar com NTP.");
    }
}

unsigned long Get_Epoch() {
    time_t now;
    time(&now);
    return now;
}

