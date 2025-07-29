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

#define PWM_CHANNEL_BLUE 0
#define PWM_FREQ 5000
#define PWM_RESOLUTION 12 // bits

void Setup_LEDs() {
    ledcSetup(PWM_CHANNEL_BLUE, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(LED_BLUE, PWM_CHANNEL_BLUE);

    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
}

void Respira_LED() {
    static unsigned long startTime = millis();
    static const float ciclo_duracao = 5000.0; // 5 segundos por ciclo

    float tempo = (millis() - startTime) / ciclo_duracao; // tempo normalizado
    float ciclo = sin(tempo * 2 * PI); // varia entre -1 e 1
    ciclo = (ciclo + 1.0) / 2.0;       // ajusta para 0 a 1

    uint16_t brilho = ciclo * 4095;   // respira entre 0 e 4095
    ledcWrite(PWM_CHANNEL_BLUE, brilho);
}

