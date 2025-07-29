/*
 * Copyright (c) 2025, Sandra Bati Furquim
 * All rights reserved.
 *
 * This file is released under the MIT License.
 * See LICENSE file for details.
 */
#include <LittleFS.h>
#include "globals.h"
#include "modulo.h"

bool Setup_LittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("❌ Erro ao montar LittleFS");
    return false;
  }

  Serial.println("✅ LittleFS montado com sucesso");
  Serial.print("Tamanho total (bytes): ");
  Serial.println(LittleFS.totalBytes());
  Serial.print("Espaço disponível: ");
  Serial.println(LittleFS.totalBytes() - LittleFS.usedBytes());

  // Cria o diretório se não existir
  if (!LittleFS.exists(dataDir)) {
    if (LittleFS.mkdir(dataDir)) {
      Serial.println("📁 Diretório " + String(dataDir) + " criado com sucesso.");
    } else {
      Serial.println("❌ Erro ao criar diretório " + String(dataDir));
      return false;
    }
  } else {
    Serial.println("📁 Diretório " + String(dataDir) + " já existe.");
  }

  return true;
}

void Load_Config_LFS() {
  File file = LittleFS.open("/config.txt", "r");
  if (file) {
    while (file.available()) {
      String line = file.readStringUntil('\n');
      line.trim();

      if (line.startsWith("SSID=")) ssid = line.substring(5);
      else if (line.startsWith("PASS=")) password = line.substring(5);
      else if (line.startsWith("WS_SERVER=")) ws_server = line.substring(10);
      else if (line.startsWith("WS_PORT=")) ws_port = line.substring(8).toInt();
      else if (line.startsWith("SCHOOL_ID=")) school_id = line.substring(10);
      else if (line.startsWith("BRILHO=")) brilho_base = line.substring(7).toInt();
      else if (line.startsWith("SENS=")) sensibilidade = line.substring(5).toInt();
      else if (line.startsWith("LANG=")) lang = line.substring(5);
    }
    file.close();
    Serial.println("⚠️ Lendo Config LittleFS.");
  } else {
    ssid = "void"; // Marca para não tentar conectar
    Serial.println("❌ Arquivo de configuração não encontrado no LittleFS.");
  }
}

void Save_Config_LFS() {
  File file = LittleFS.open("/config.txt", FILE_WRITE);
  if (file) {
    file.println("SSID=" + ssid);
    file.println("PASS=" + password);
    file.println("WS_SERVER=" + ws_server);
    file.println("WS_PORT=" + String(ws_port));
    file.println("SCHOOL_ID=" + school_id);
    file.println("BRILHO=" + String(brilho_base));
    file.println("SENS=" + String(sensibilidade));
    file.println("LANG=" + lang);
    file.close();

    Serial.println("✅ Configuração salva no LittleFS.");
    SerialBT.println("{SAVE=OK}");
  } else {
    Serial.println("❌ Erro ao salvar configuração no LittleFS.");
    SerialBT.println("{SAVE=ERROR}");
  }
}


String getFileNameFS(unsigned long timestamp) {
  char buffer[50];
  snprintf(buffer, sizeof(buffer), "/dados/dados_%lu.jsonL", timestamp / 3600);
  return String(buffer);
}


String getFallbackFileName() {
  File dir = LittleFS.open("/dados");
  if (!dir || !dir.isDirectory()) {
    LittleFS.mkdir("/dados"); // Garante que o diretório exista
    return "/dados/dados_000001.jsonL";
  }

  String lastName = "";
  File file = dir.openNextFile();
  while (file) {
    String name = String(file.name());
    if (name.endsWith(".jsonL") && name.startsWith("/dados/dados_")) {
      if (name > lastName) lastName = name;
    }
    file = dir.openNextFile();
  }

  if (lastName == "") {
    return "/dados/dados_000001.jsonL";
  } else {
    int underscore = lastName.lastIndexOf('_');
    int dot = lastName.lastIndexOf('.');
    String numStr = lastName.substring(underscore + 1, dot);
    int num = numStr.toInt() + 1;
    char buffer[32];
    sprintf(buffer, "/dados/dados_%06d.jsonL", num);
    return String(buffer);
  }
}

void deleteOldestFile_LFS() {
  File dir = LittleFS.open("/dados");
  if (!dir || !dir.isDirectory()) {
    Serial.println("Erro ao abrir diretório /dados");
    return;
  }

  String oldestFile = "";
  unsigned long oldestTime = ULONG_MAX;
  File entry;

  while ((entry = dir.openNextFile())) {
    String name = String(entry.name());
    if (!entry.isDirectory() && name.startsWith("/dados/dados_")) {
      unsigned long fileTime = name.substring(name.indexOf("dados_") + 6, name.indexOf(".jsonL")).toInt();
      if (fileTime < oldestTime) {
        oldestTime = fileTime;
        oldestFile = name;
      }
    }
    entry.close();
  }
  dir.close();

  if (oldestFile != "") {
    Serial.println("Deletando arquivo antigo: " + oldestFile);
    if (LittleFS.remove(oldestFile)) {
      Serial.println("Arquivo deletado com sucesso.");
    } else {
      Serial.println("Erro ao deletar arquivo: " + oldestFile);
    }
  }
}

int countFiles_LFS() {
  int count = 0;
  File dir = LittleFS.open("/dados");
  if (!dir || !dir.isDirectory()) {
    Serial.println("Erro ao abrir diretório /dados");
    return 0;
  }
  File entry;
  while ((entry = dir.openNextFile())) {
    if (!entry.isDirectory() && String(entry.name()).startsWith("/dados/dados_")) {
      count++;
    }
    entry.close();
  }
  dir.close();
  return count;
}

// const int maxFiles = 72; // Limite máximo de arquivos

bool Write_LFS(String data, unsigned long timestamp) {
  unsigned long currentHour = timestamp / 3600;

  // Verifica se é uma nova hora
  if (currentHour != lastHour || currentFileName == "") {
    lastHour = currentHour;
    currentFileName = getFileNameFS(timestamp);
    Serial.println("Novo arquivo: " + currentFileName);

    // Verifica se o diretório existe
    if (!LittleFS.exists("/dados")) {
      Serial.println("Diretório /dados não existe, criando...");
      if (!LittleFS.mkdir("/dados")) {
        Serial.println("Erro ao criar diretório /dados");
        return false;
      }
    }

    // Verifica se há mais de 72 arquivos
    if (countFiles_LFS() >= maxFiles) {
      deleteOldestFile_LFS();
    }
  }

  // Verifica se currentFileName é válido
  if (currentFileName == "" || !currentFileName.startsWith("/")) {
    Serial.println("Erro: currentFileName inválido: " + currentFileName);
    currentFileName = getFileNameFS(timestamp);
    Serial.println("Corrigido para: " + currentFileName);
  }

  Serial.println("Escrevendo em: " + currentFileName);
  File file = LittleFS.open(currentFileName, FILE_APPEND);
  if (!file) {
    Serial.println("Erro ao abrir arquivo: " + currentFileName);
    return false;
  }
  if (file.println(data)) {
    Serial.println("Dados escritos com sucesso: " + data);
    file.flush();
  } else {
    Serial.println("Erro ao escrever dados no arquivo.");
  }
  file.close();
  return true;
}



