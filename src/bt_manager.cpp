/*
 * Copyright (c) 2025, Sandra Bati Furquim
 * All rights reserved.
 *
 * This file is released under the MIT License.
 * See LICENSE file for details.
 */
#include "globals.h"
#include "modulo.h" // para Save_Config()

void Setup_Bluetooth() {
    Serial.begin(115200);
    SerialBT.begin("ESP32_AudioSystem");
    Serial.println("Bluetooth pronto para configuração.");
}

bool BT_Available() {
    return SerialBT.available();
}

String BT_Read() {
    return SerialBT.readStringUntil('}');
}

void Process_BT(String command) {
    command.trim();

    if (command.startsWith("{") && command.length() > 2) {
        command = command.substring(1); // remove '{'

        if (command.startsWith("SSID=")) {
            ssid = command.substring(5);
        } else if (command.startsWith("PASS=")) {
            password = command.substring(5);
        } else if (command.startsWith("BRILHO=")) {
            brilho_base = command.substring(7).toInt();
        } else if (command.startsWith("SENS=")) {
            sensibilidade = command.substring(5).toInt();
        } else if (command.startsWith("WS_SERVER=")) {
            ws_server = command.substring(10);
        } else if (command.startsWith("WS_PORT=")) {
            ws_port = command.substring(8).toInt();
        } else if (command.startsWith("SCHOOL_ID=")) {
            school_id = command.substring(10);
        } else if (command == "STATUS") {
            SerialBT.println("{STATUS}");
            SerialBT.println("{SSID=" + ssid + "}");
            SerialBT.println("{PASS=" + password + "}");
            SerialBT.println("{BRILHO=" + String(brilho_base) + "}");
            SerialBT.println("{SENS=" + String(sensibilidade) + "}");
            SerialBT.println("{WS_SERVER=" + ws_server + "}");
            SerialBT.println("{WS_PORT=" + String(ws_port) + "}");
            SerialBT.println("{SCHOOL_ID=" + school_id + "}");
        } else if (command == "SAVE") {
            Save_Config();
        } else {
            SerialBT.println("{UNKNOWN COMMAND}");
        }

        Serial.println("CMD Recebido: {" + command + "}");
    }
}

void check_bluetooth() {
    if (BT_Available()) {
        String cmd = BT_Read();
        Process_BT(cmd);
    }
}
