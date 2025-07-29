/*
 * Copyright (c) 2025, Sandra Bati Furquim
 * All rights reserved.
 *
 * This file is released under the MIT License.
 * See LICENSE file for details.
 */
#include "globals.h"
#include "hardware.h"
#include "modulo.h"

bool Setup_SPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ Erro ao montar SPIFFS");
    return false;
  }

  Serial.println("✅ SPIFFS montado com sucesso");
  Serial.print("Tamanho total (bytes): ");
  Serial.println(SPIFFS.totalBytes());
  Serial.println("Espaço disponível: " + String(SPIFFS.totalBytes() - SPIFFS.usedBytes()));

  // Verifica se o diretório existe
  if (!SPIFFS.exists(dataDir)) {
    Serial.println("Diretório " + String(dataDir) + " não existe. Tentando criar...");
    if (SPIFFS.mkdir(dataDir)) {
      Serial.println("✅ Diretório " + String(dataDir) + " criado com sucesso.");
    } else {
      Serial.println("❌ Erro ao criar diretório " + String(dataDir));
      return false;
    }
  } else {
    Serial.println("📁 Diretório " + String(dataDir) + " já existe.");
  }
  
if (!SPIFFS.exists(dataDir)) {
  Serial.println("Diretório " + String(dataDir) + " não existe. Tentando criar...");
  File dummy = SPIFFS.open(String(dataDir) + "/.init", FILE_WRITE);
  if (!dummy) {
    Serial.println("❌ Erro ao criar arquivo dummy em " + String(dataDir));
    return false;
  }
  dummy.close();
  Serial.println("✅ Diretório " + String(dataDir) + " simulado com sucesso.");
}





  return true;
}

void list_SPIFFS(String path) {
  if (!SPIFFS.exists(path)) {
    Serial.println("📁 Diretório \"" + path + "\" não existe. Listando raiz \"/\":");
    path = "/";
  }

  File dir = SPIFFS.open(path);
  if (!dir || !dir.isDirectory()) {
    Serial.println("❌ Erro ao abrir diretório: " + path);
    return;
  }

  File file = dir.openNextFile();
  bool isEmpty = true;

  while (file) {
    isEmpty = false;
    String name = file.name();
    if (file.isDirectory()) {
      Serial.println("📂 " + name + " [DIR]");
    } else {
      Serial.println("📄 " + name + " [" + String(file.size()) + " bytes]");
    }
    file = dir.openNextFile();
  }

  if (isEmpty) {
    Serial.println("📁 Diretório \"" + path + "\" está vazio. Listando raiz \"/\":");
    dir = SPIFFS.open("/");
    file = dir.openNextFile();
    while (file) {
      String name = file.name();
      if (file.isDirectory()) {
        Serial.println("📂 " + name + " [DIR]");
      } else {
        Serial.println("📄 " + name + " [" + String(file.size()) + " bytes]");
      }
      file = dir.openNextFile();
    }
  }
}












