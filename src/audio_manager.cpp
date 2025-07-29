/*
 * Copyright (c) 2025, Sandra Bati Furquim
 * All rights reserved.
 *
 * This file is released under the MIT License.
 * See LICENSE file for details.
 */
#include "globals.h"
#include "modulo.h"
#include "hardware.h"

#define THRESH_YELLOW 500   // ~60 dB
#define THRESH_RED    1000  // ~70 dB
#define THRESH_BOTH   1500  // ~80 dB

const int ADC_OFFSET = 1549; // valor médio para 1.25V com Vcc = 3.3V
unsigned long yellow_timer = 0;
unsigned long red_timer = 0;

void Setup_ADC() {
    analogReadResolution(12); // 0–4095
}

uint16_t Read_MIC() {
    int raw = analogRead(MIC_PIN);
    int amplitude = abs(raw - ADC_OFFSET); // remove offset DC
    return amplitude;
}

void Process_Audio(uint16_t amplitude) {
    unsigned long now = millis();

    // Aciona ambos se ultrapassar THRESH_BOTH
    if (amplitude > THRESH_BOTH) {
        red_timer = now;
        yellow_timer = now;
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_YELLOW, HIGH);
        return;
    }

    // Aciona vermelho se ultrapassar THRESH_RED
    if (amplitude > THRESH_RED) {
        red_timer = now;
        digitalWrite(LED_RED, HIGH);
    } else if (now - red_timer > 1000) {
        digitalWrite(LED_RED, LOW);
    }

    // Aciona amarelo se ultrapassar THRESH_YELLOW
    if (amplitude > THRESH_YELLOW) {
        yellow_timer = now;
        digitalWrite(LED_YELLOW, HIGH);
    } else if (now - yellow_timer > 1000) {
        digitalWrite(LED_YELLOW, LOW);
    }
}

