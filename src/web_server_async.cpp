/*
 * Copyright (c) 2025, Sandra Bati Furquim
 * All rights reserved.
 *
 * This file is released under the MIT License.
 * See LICENSE file for details.
 */
#include "globals.h"
#include "modulo.h"

AsyncWebServer server(80);

void handleSetupPage(AsyncWebServerRequest *request)
{
	String filePath = "/" + lang + "/setup.html";
	File file = LittleFS.open(filePath, "r"); // Abre em modo leitura

	if (!file)
	{
		request->send(404, "text/plain", "Arquivo " + filePath + " não encontrado no LittleFS");
		return;
	}

	String html = file.readString();
	file.close();

	html.replace("%SSID%", ssid);
	html.replace("%PASS%", password);
	html.replace("%BRILHO%", String(brilho_base));
	html.replace("%SENS%", String(sensibilidade));
	html.replace("%WS_SERVER%", ws_server);
	html.replace("%WS_PORT%", String(ws_port));
	html.replace("%SCHOOL_ID%", school_id);
	html.replace("%LANG%", lang);

	request->send(200, "text/html", html);
}

void handleSetupSubmit(AsyncWebServerRequest *request)
{
	if (request->method() == HTTP_POST)
	{
		if (request->hasParam("ssid", true)) ssid = request->getParam("ssid", true)->value();
		if (request->hasParam("pass", true)) password = request->getParam("pass", true)->value();
		if (request->hasParam("brilho", true)) brilho_base = request->getParam("brilho", true)->value().toInt();
		if (request->hasParam("sens", true)) sensibilidade = request->getParam("sens", true)->value().toInt();
		if (request->hasParam("ws_server", true)) ws_server = request->getParam("ws_server", true)->value();
		if (request->hasParam("ws_port", true)) ws_port = request->getParam("ws_port", true)->value().toInt();
		if (request->hasParam("school_id", true)) school_id = request->getParam("school_id", true)->value();
		if (request->hasParam("lang", true)) lang = request->getParam("lang", true)->value();

		Save_Config();
		String response = "<html><body><h1>Configura\u00e7\u00f5es salvas com sucesso!</h1><p><a href=\"/setup.html\">Voltar</a></p></body></html>";
		request->send(200, "text/html", response);
	}
	else
	{
		request->send(405, "text/plain", "M\u00e9todo n\u00e3o permitido");
	}
}

//------------------------------------------------------------------------------
void Setup_WebServer()
{
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		String filePath = "/" + lang + "/index.html";
		if (LittleFS.exists(filePath))
		{
			request->send(LittleFS, filePath, "text/html");
		}
		else
		{
			request->send(404, "text/plain", "Página não encontrada");
		}
	});

	events.onConnect([](AsyncEventSourceClient *client)
	{
		Serial.println("🛰️ Cliente SSE conectado");
		client->send("ready", "status", millis());
	});

	server.addHandler(&events); // Isso torna /events acessível

	server.on("/explorer.html", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		String filePath = "/" + lang + "/explorer.html";
		if (LittleFS.exists(filePath))
		{
			request->send(LittleFS, filePath, "text/html");
		}
		else
		{
			request->send(404, "text/plain", "Página explorer não encontrada");
		}
	});

	server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		if (!request->hasParam("file"))
		{
			request->send(400, "text/plain", "Parâmetro 'file' ausente");
			return;
		}
		String fileName = request->getParam("file")->value();
		if (!LittleFS.exists(fileName))
		{
			request->send(404, "text/plain", "Arquivo não encontrado");
			return;
		}
		request->send(LittleFS, fileName, "application/octet-stream");
	});

	// server.on("/listSD", HTTP_GET, [](AsyncWebServerRequest *request)
	// {
		// File dir = SD.open(dataDir);
		// String response = "<html><body><h1>Arquivos em /dados/</h1><ul>";
		// while (true)
		// {
			// File entry = dir.openNextFile();
			// if (!entry) break;
			// if (!entry.isDirectory())
			// {
				// response += "<li>" + String(entry.name()) + "</li>";
			// }
			// entry.close();
		// }
		// dir.close();
		// response += "</ul></body></html>";
		// request->send(200, "text/html", response);
	// });

	server.on("/list", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		File dir = LittleFS.open("/dados");
		if (!dir || !dir.isDirectory())
		{
			request->send(500, "text/plain", "Diretório /dados não encontrado");
			return;
		}

		String response = "<html><body><h1>Arquivos em /dados/</h1><ul>";
		File entry;
		while ((entry = dir.openNextFile()))
		{
			if (!entry.isDirectory())
			{
				response += "<li>" + String(entry.name()) + "</li>";
			}
			entry.close();
		}
		dir.close();
		response += "</ul></body></html>";
		request->send(200, "text/html", response);
	});

/* 	server.on("/last", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		AsyncResponseStream *response = request->beginResponseStream("application/json");
		response->print("[");

		File atual = SD.open("/dados/dados_486984.jsonL");
		bool first = true;

		const size_t bufSize = 256;  // ou maior se suas linhas forem longas
		char buffer[bufSize];

		while (atual.available())
		{
			size_t len = atual.readBytesUntil('\n', buffer, bufSize - 1);
			buffer[len] = '\0';  // termina a string

			// pular linhas muito curtas (vazias, lixo, etc)
			if (len < 5) continue;

			if (!first) response->print(",");
			response->print(buffer);
			first = false;
		}

		atual.close();
		response->print("]");
		request->send(response);
	}); */

	server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		if (request->hasParam("file"))
		{
			String path = request->getParam("file")->value();

			if (LittleFS.exists(path))
			{
				if (LittleFS.remove(path))
				{
					request->send(200, "text/plain", "Arquivo deletado com sucesso.");
				}
				else
				{
					request->send(500, "text/plain", "Erro ao deletar o arquivo.");
				}
			}
			else
			{
				request->send(404, "text/plain", "Arquivo não encontrado.");
			}
		}
		else
		{
			request->send(400, "text/plain", "Parâmetro 'file' ausente.");
		}
	});

	server.on("/dados", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		if (!request->hasParam("file"))
		{
			request->send(400, "text/plain", "Faltando parâmetro 'file'");
			return;
		}
		String nome = request->getParam("file")->value();
		String caminho = "/dados/" + nome;
		if (!LittleFS.exists(caminho))
		{
			request->send(404, "text/plain", "Arquivo não encontrado");
			return;
		}
		request->send(LittleFS, caminho, "text/plain");
	});

	server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		String filePath = "/" + lang + "/favicon.ico";
		request->send(LittleFS, filePath, "image/x-icon");
		// if (SD.exists("/favicon.ico"))
		// {
			// request->send(SD, "/favicon.ico", "image/x-icon");
		// }
		// else
		// {
			// request->send(404, "text/plain", "Favicon n\u00e3o encontrado");
		// }
	});

	server.on("/setup.html", HTTP_GET, handleSetupPage);
	server.on("/setup", HTTP_POST, handleSetupSubmit);

	server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request)
	{
		request->send(200, "text/plain", "Reiniciando ESP32...");
		delay(1000);
		ESP.restart();
	});

	server.on("/grafico.html", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		String filePath = "/" + lang + "/grafico.html";
		request->send(LittleFS, filePath, "text/plain");
	});

	server.on("/status.html", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		String filePath = "/" + lang + "/status.html";
		request->send(LittleFS, filePath, "text/plain");
		// if (SD.exists("/status.html"))
		// {
			// request->send(SD, "/status.html", "text/html");
		// }
		// else
		// {
			// request->send(404, "text/plain", "Arquivo n\u00e3o encontrado");
		// }
	});

	server.on("/list.json", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		File dir = LittleFS.open("/dados");
		if (!dir || !dir.isDirectory())
		{
			request->send(500, "application/json", "[]");
			return;
		}

		String json = "[";
		bool primeiro = true;
		File entry;
		while ((entry = dir.openNextFile()))
		{
			if (!entry.isDirectory())
			{
				String nomeCompleto = String(entry.name()); // Ex: /dados/dados_487150.jsonL
				String nomeArquivo = nomeCompleto.substring(nomeCompleto.lastIndexOf("/") + 1); // Só o nome

				if (nomeArquivo.startsWith("dados_") && nomeArquivo.endsWith(".jsonL"))
				{
					if (!primeiro) json += ",";
					json += "\"" + nomeArquivo + "\"";
					primeiro = false;
				}
			}
			entry.close();
		}
		dir.close();
		json += "]";
		request->send(200, "application/json", json);
	});

	server.on("/controle.html", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		String filePath = "/" + lang + "/controle.html";  // Ex: /pt-BR/controle.html
		if (LittleFS.exists(filePath))
		{
			request->send(LittleFS, filePath, "text/html");
		}
		else
		{
			request->send(404, "text/plain", "Página explorer não encontrada");
		}
	});

	server.onNotFound([](AsyncWebServerRequest *request)
	{
		String path = request->url();

		if (LittleFS.exists(path))
		{
			String contentType = "text/plain";
			if (path.endsWith(".html")) contentType = "text/html";
			else if (path.endsWith(".css")) contentType = "text/css";
			else if (path.endsWith(".js")) contentType = "application/javascript";
			else if (path.endsWith(".csv")) contentType = "text/csv";

			request->send(LittleFS, path, contentType);
		}
		else
		{
			request->send(404, "text/plain", "Arquivo não encontrado");
		}
	});

	server.begin();
	Serial.println("Servidor Web Async iniciado na porta 80");
}












