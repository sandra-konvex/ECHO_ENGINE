/*
 * Copyright (c) 2025, Sandra Bati Furquim
 * All rights reserved.
 *
 * This file is released under the MIT License.
 * See LICENSE file for details.
 */
#include "globals.h"
#include "modulo.h"

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_CONNECTED:
            Serial.println("WebSocket conectado!");
            break;
        case WStype_TEXT:
            Serial.printf("Recebido: %s\n", payload);
            break;
        case WStype_DISCONNECTED:
            Serial.println("WebSocket desconectado.");
            break;
    }
}

void Setup_WebSocket() {
    Serial.println("Conectando ao WebSocket...");
    webSocket.begin(ws_server.c_str(), ws_port, "/");
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
}

void check_websocket() {
    webSocket.loop();
}