/*
 * Copyright (c) 2025, Sandra Bati Furquim
 * All rights reserved.
 *
 * This file is released under the MIT License.
 * See LICENSE file for details.
 */
#include "globals.h"

std::vector<String> split(const String& str, char delimiter) {
  std::vector<String> result;
  int start = 0;
  int end = str.indexOf(delimiter);

  while (end != -1) {
    result.push_back(str.substring(start, end));
    start = end + 1;
    end = str.indexOf(delimiter, start);
  }

  // Adiciona o último pedaço
  result.push_back(str.substring(start));
  return result;
}


bool Atualizar_Arquivos_Web(String baseURL) {
  // String lang = "pt-BR"; // já lido do config.txt
  String localVersionPath = "/" + lang + "/version.txt";
  String remoteVersionURL = baseURL + "/" + lang + "/version.txt";

  // Lê versão local
  String localVersion = "0.0.0";
  if (LittleFS.exists(localVersionPath)) {
    File f = LittleFS.open(localVersionPath, "r");
    localVersion = f.readStringUntil('\n');
    f.close();
  }

  // Lê versão remota
  HTTPClient http;
  http.begin(remoteVersionURL);
  int httpCode = http.GET();
  if (httpCode != 200) {
    Serial.println("Erro ao acessar versão remota");
    http.end();
    return false;
  }

  String remoteData = http.getString();
  http.end();

  // Extrai versão e lista de arquivos
  String remoteVersion;
  std::vector<String> arquivos;
  int idx = remoteData.indexOf('\n');
  remoteVersion = remoteData.substring(0, idx);
  String fileList = remoteData.substring(idx + 1);
  fileList.trim();

  // Se versão diferente, atualiza
  if (remoteVersion != localVersion) {
    Serial.println("Atualizando arquivos...");

    // Cria pasta se necessário
    if (!LittleFS.exists("/" + lang)) {
      LittleFS.mkdir("/" + lang);
    }

    // Baixa arquivos
    arquivos = split(fileList, ','); // você pode implementar essa função
    for (String nome : arquivos) {
      String url = baseURL + "/" + lang + "/" + nome;
      http.begin(url);
      int code = http.GET();
      if (code == 200) {
        File f = LittleFS.open("/" + lang + "/" + nome, "w");
        f.print(http.getString());
        f.close();
        Serial.println("Atualizado: " + nome);
      }
      http.end();
    }

    // Atualiza version.txt local
    File f = LittleFS.open(localVersionPath, "w");
    f.println(remoteVersion);
    f.close();
  }

  return true;
}