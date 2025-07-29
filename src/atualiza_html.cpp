/*
 * Copyright (c) 2025, Sandra Bati Furquim
 * All rights reserved.
 *
 * This file is released under the MIT License.
 * See LICENSE file for details.
 */
#include "globals.h"

String versao_local = "0.0.0";

void Baixar_Arquivo_SD(String url, String caminho_local) {
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == 200) {
    // Extrai o diretório do caminho_local (ex.: "/pt-BR/" de "/pt-BR/setup.html")
    int lastSlash = caminho_local.lastIndexOf('/');
    String dir = caminho_local.substring(0, lastSlash);
    if (!SD.exists(dir)) {
      if (SD.mkdir(dir)) {
        Serial.println("Diretório criado: " + dir);
      } else {
        Serial.println("Erro ao criar diretório: " + dir);
        http.end();
        return;
      }
    }

    File file = SD.open(caminho_local, FILE_WRITE);
    if (file) {
      WiFiClient *stream = http.getStreamPtr();
      uint8_t buffer[128];
      int len;

      while (http.connected() && (len = stream->read(buffer, sizeof(buffer))) > 0) {
        file.write(buffer, len);
      }

      file.close();
      Serial.println("Arquivo salvo em " + caminho_local);
    } else {
      Serial.println("Erro ao abrir arquivo no SD: " + caminho_local);
    }
  } else {
    Serial.println("Erro HTTP ao baixar " + url + ": " + String(httpCode));
  }

  http.end();
}

bool Atualizar_Arquivos_Web_SD(String url_base)
{
	HTTPClient http;

	// Garante que url_base não termine com '/' para evitar duplicação
	if (url_base.endsWith("/"))
	{
		url_base = url_base.substring(0, url_base.length() - 1);
	}

	// Adiciona o idioma à URL base
	String url_com_idioma = url_base + "/" + lang;

	// Acessa o arquivo version.txt no diretório do idioma
	http.begin(url_com_idioma + "/version.txt");
	int httpCode = http.GET();

	if (httpCode == 200)
	{
		String versao_remota = http.getString();
		versao_remota.trim();

		String versao_local = "";
		File vfile = SD.open("/version.txt");
		if (vfile)
		{
			versao_local = vfile.readStringUntil('\n');
			versao_local.trim();
			vfile.close();
		}

		if (versao_local != versao_remota)
		{
			Serial.println("Atualizando arquivos web...");

			// Cria o diretório do idioma, se não existir
			String dir_idioma = "/" + lang;
			if (!SD.exists(dir_idioma))
			{
				if (SD.mkdir(dir_idioma))
				{
					Serial.println("Diretório criado: " + dir_idioma);
				}
				else
				{
					Serial.println("Erro ao criar diretório: " + dir_idioma);
					http.end();
					return false;
				}
			}

			// Baixa os arquivos e salva no diretório do idioma
			Baixar_Arquivo_SD(url_com_idioma + "/index.html", "/" + lang + "/index.html");
			Baixar_Arquivo_SD(url_com_idioma + "/setup.html", "/" + lang + "/setup.html");
			Baixar_Arquivo_SD(url_com_idioma + "/grafico.html", "/" + lang + "/grafico.html");
			Baixar_Arquivo_SD(url_com_idioma + "/status.html", "/" + lang + "/status.html");
			Baixar_Arquivo_SD(url_com_idioma + "/controle.html", "/" + lang + "/controle.html");
			Baixar_Arquivo_SD(url_com_idioma + "/favicon.ico", "/" + lang + "/favicon.ico");

			// Atualiza a versão local
			File vfile = SD.open("/version.txt", FILE_WRITE);
			if (vfile)
			{
				vfile.println(versao_remota);
				vfile.close();
				Serial.println("Versão atualizada em /version.txt");
			}
			else
			{
				Serial.println("Erro ao atualizar /version.txt");
			}

			Serial.println("Atualização concluída.");
			http.end();
			return true;
		}
		else
		{
			Serial.println("Arquivos web já estão atualizados.");
			http.end();
			return false;
		}
	}
	else
	{
		Serial.println("Erro ao obter versão remota: HTTP " + String(httpCode));
		http.end();
		return false;
	}
}
