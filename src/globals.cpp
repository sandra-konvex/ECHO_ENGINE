/*
 * Copyright (c) 2025, Sandra Bati Furquim
 * All rights reserved.
 *
 * This file is released under the MIT License.
 * See LICENSE file for details.
 */
#include "globals.h"

// Objetos globais
BluetoothSerial SerialBT;
WebSocketsClient webSocket;

// Variáveis globais
String ssid = "WIFI_EXT";
String password = "0123456789";
String ws_server = "127.0.0.1";
int ws_port = 8080;
String school_id = "";
int brilho_base = 50;
int sensibilidade = 60;
String lang = "pt-BR";

AsyncEventSource events("/events"); // Canal SSE

int id_counter = 1;
unsigned long lastSendTime = 0;

uint16_t audio_samples[MAX_SAMPLES];
int sample_index = 0;
unsigned long audio_window_start = 0;
const unsigned long audio_window_duration = 5000;

const char* dataDir = "/dados"; // Diretório para os arquivos
const int maxFiles = 24;       // Máximo de 24 arquivos (24 horas)
String currentFileName = "";   // Nome do arquivo atual
unsigned long lastHour = 0;    // Última hora registrada
