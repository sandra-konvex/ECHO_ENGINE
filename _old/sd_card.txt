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

bool Setup_card() {
  if (!SD.begin(SD_CS)) {
    Serial.println("Erro ao inicializar cartão SD.");
    return false;
  }
  // Cria o diretório se não existir
  if (!SD.exists(dataDir)) {
    if (SD.mkdir(dataDir)) {
      Serial.println("Diretório " + String(dataDir) + " criado com sucesso.");
    } else {
      Serial.println("Erro ao criar diretório " + String(dataDir));
    }
  } else {
    Serial.println("Diretório " + String(dataDir) + " já existe.");
  }
  return true;
}

void Save_Config()
{
	Save_Config_LFS();
}

/* void Save_Config() {
    File file = SPIFFS.open("/config.txt", FILE_WRITE);
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
        Serial.println("✅ Configuração salva no SPIFFS.");
        SerialBT.println("{SAVE=OK}");
    } else {
        Serial.println("❌ Erro ao salvar configuração.");
        SerialBT.println("{SAVE=ERROR}");
    }
}
 */
/* void Load_Config() {
    File file = SPIFFS.open("/config.txt");
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
        Serial.println("✅ Configuração carregada do SPIFFS.");
    } else {
        ssid = "void"; // Marca para não tentar conectar
        Serial.println("⚠️ Arquivo de configuração não encontrado.");
    }
} */

void Load_Config_SD() {
    File file = SD.open("/config.txt");
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
		Serial.println("⚠️ Lendo Config SD.");
    } else {
        ssid = "void"; // Marca para não tentar conectar
    }
}

// Função para gerar nome do arquivo
String getFileName(unsigned long timestamp) {
  char buffer[50];
  snprintf(buffer, sizeof(buffer), "%s/dados_%lu.jsonL", dataDir, timestamp / 3600);
  String fileName = String(buffer);
  if (!fileName.startsWith("/")) {
    fileName = "/" + fileName;
    Serial.println("Corrigido caminho do arquivo: " + fileName);
  }
  return fileName;
}

// Função para contar arquivos no diretório
int countFiles() {
  int count = 0;
  File dir = SD.open(dataDir);
  if (!dir) {
    Serial.println("Erro ao abrir diretório: " + String(dataDir));
    return 0;
  }
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;
    if (!entry.isDirectory() && String(entry.name()).startsWith("dados_")) {
      count++;
    }
    entry.close();
  }
  dir.close();
  return count;
}

// Função para deletar o arquivo mais antigo
void deleteOldestFile() {
  File dir = SD.open(dataDir);
  if (!dir) {
    Serial.println("Erro ao abrir diretório: " + String(dataDir));
    return;
  }
  String oldestFile = "";
  unsigned long oldestTime = ULONG_MAX;

  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;
    if (!entry.isDirectory() && String(entry.name()).startsWith("dados_")) {
      String name = String(entry.name());
      unsigned long fileTime = name.substring(String("dados_").length(), name.indexOf(".jsonL")).toInt();
      if (fileTime < oldestTime) {
        oldestTime = fileTime;
        oldestFile = String(dataDir) + "/" + name;
      }
    }
    entry.close();
  }
  dir.close();

  if (oldestFile != "") {
    Serial.println("Deletando arquivo antigo: " + oldestFile);
    if (SD.remove(oldestFile)) {
      Serial.println("Arquivo deletado com sucesso.");
    } else {
      Serial.println("Erro ao deletar arquivo: " + oldestFile);
    }
  }
}

// Função para escrever dados no SD
// Função para escrever dados no SD
bool Write_SD(String data, unsigned long timestamp) {
  unsigned long currentHour = timestamp / 3600;

  // Verifica se é uma nova hora
  if (currentHour != lastHour || currentFileName == "") {
    lastHour = currentHour;
    currentFileName = getFileName(timestamp);
    Serial.println("Novo arquivo: " + currentFileName);

    // Verifica se o diretório existe
    if (!SD.exists(dataDir)) {
      Serial.println("Diretório " + String(dataDir) + " não existe, criando...");
      if (!SD.mkdir(dataDir)) {
        Serial.println("Erro ao criar diretório " + String(dataDir));
        return false;
      }
    }

    // Verifica se há mais de 72 arquivos
    if (countFiles() >= maxFiles) {
      deleteOldestFile();
    }
  }

  // Verifica se currentFileName é válido
  if (currentFileName == "" || !currentFileName.startsWith("/")) {
    Serial.println("Erro: currentFileName inválido: " + currentFileName);
    currentFileName = getFileName(timestamp);
    Serial.println("Corrigido para: " + currentFileName);
  }

  Serial.println("Escrevendo em: " + currentFileName);
  File file = SD.open(currentFileName, FILE_APPEND);
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

