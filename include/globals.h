/*
 * Copyright (c) 2025, Sandra Bati Furquim
 * All rights reserved.
 *
 * This file is released under the MIT License.
 * See LICENSE file for details.
 */
#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <SD.h>
#include <HTTPClient.h>
#include <BluetoothSerial.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <time.h>
#include <math.h>

extern int id_counter;
extern unsigned long lastSendTime;

// Objetos globais
extern BluetoothSerial SerialBT;
extern WebSocketsClient webSocket;

// Variáveis globais
extern String ssid;
extern String password;
extern String ws_server;
extern int ws_port;
extern String school_id;
extern int brilho_base;
extern int sensibilidade;
extern String lang;

#define MAX_SAMPLES 500

extern uint16_t audio_samples[MAX_SAMPLES];
extern int sample_index;
extern unsigned long audio_window_start;
extern const unsigned long audio_window_duration;


extern const char* dataDir; // Diretório para os arquivos
extern const int maxFiles;       // Máximo de 72 arquivos (72 horas)
extern String currentFileName;   // Nome do arquivo atual
extern unsigned long lastHour;    // Última hora registrada

extern AsyncEventSource events;

#endif
