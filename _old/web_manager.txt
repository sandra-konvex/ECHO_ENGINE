#include <WebServer.h>
#include <SD.h>
#include "globals.h"
#include "modulo.h"

WebServer server(80);




// Manipulador da rota de reinicialização
void handleRestart()
{
	server.send(200, "text/plain", "Reiniciando ESP32...");
	delay(1000); // Aguarda um pouco para enviar a resposta
	ESP.restart(); // Reinicia o ESP32
}

void handleSetupPage()
{
	String filePath = "/" + lang + "/setup.html";
	File file = SD.open(filePath);

	if (!file)
	{
		server.send(404, "text/plain", "Arquivo " + filePath + " não encontrado no SD");
		server.client().stop();
		return;
	}

	String html = "";
	while (file.available())
	{
		html += (char)file.read();
	}
	file.close();

	// Substitui os placeholders pelos valores atuais
	html.replace("%SSID%", ssid);
	html.replace("%PASS%", password);
	html.replace("%BRILHO%", String(brilho_base));
	html.replace("%SENS%", String(sensibilidade));
	html.replace("%WS_SERVER%", ws_server);
	html.replace("%WS_PORT%", String(ws_port));
	html.replace("%SCHOOL_ID%", school_id);
	html.replace("%LANG%", lang);

	server.send(200, "text/html", html);
	server.client().stop(); // Fecha a conexão
}

void handleSetupSubmit()
{
	if (server.method() == HTTP_POST)
	{
		// Processa cada campo do formulário
		if (server.hasArg("ssid"))
		{
			ssid = server.arg("ssid");
			Serial.println("SSID atualizado: " + ssid);
		}
		if (server.hasArg("pass"))
		{
			password = server.arg("pass");
			Serial.println("Password atualizado: " + password);
		}
		if (server.hasArg("brilho"))
		{
			brilho_base = server.arg("brilho").toInt();
			Serial.println("Brilho atualizado: " + String(brilho_base));
		}
		if (server.hasArg("sens"))
		{
			sensibilidade = server.arg("sens").toInt();
			Serial.println("Sensibilidade atualizada: " + String(sensibilidade));
		}
		if (server.hasArg("ws_server"))
		{
			ws_server = server.arg("ws_server");
			Serial.println("WS Server atualizado: " + ws_server);
		}
		if (server.hasArg("ws_port"))
		{
			ws_port = server.arg("ws_port").toInt();
			Serial.println("WS Port atualizado: " + String(ws_port));
		}
		if (server.hasArg("school_id"))
		{
			school_id = server.arg("school_id");
			Serial.println("School ID atualizado: " + school_id);
		}
		if (server.hasArg("lang"))
		{
			lang = server.arg("lang");
			Serial.println("Lang atualizado: " + lang);
		}

		// Salva as configurações
		Save_Config();
		Serial.println("Configurações salvas");

		// Responde com uma mensagem de sucesso
		String response = "<html><body><h1>Configurações salvas com sucesso!</h1>";
		response += "<p><a href=\"/setup.html\">Voltar</a></p></body></html>";
		server.send(200, "text/html", response);
	}
	else
	{
		server.send(405, "text/plain", "Método não permitido");
	}
	server.client().stop(); // Fecha a conexão
}



// Página leve embutida na Flash
const char boot_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta http-equiv="refresh" content="0; url=/index.html">
</head>
<body>
    <p>Carregando interface...</p>
</body>
</html>
)rawliteral";

void handleListFiles() {
  File dir = SD.open(dataDir);
  String response = "<html><body><h1>Arquivos em /dados/</h1><ul>";
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;
    if (!entry.isDirectory()) {
      response += "<li>" + String(entry.name()) + "</li>";
    }
    entry.close();
  }
  dir.close();
  response += "</ul></body></html>";
  server.send(200, "text/html", response);
  server.client().stop(); // Fecha a conexão com o cliente
}

void handleFavicon(){
	File file = SD.open("/favicon.ico");
  if (file) {
    server.streamFile(file, "image/x-icon");
    file.close();
  } else {
    server.send(404, "text/plain", "Favicon não encontrado");
  }
  server.client().stop();
}

// Função do servidor web para o gráfico
// Manipulador para a rota /graph (dados JSON)
void handleGraph() {
  String jsonData = readLastHour(); // Ajuste conforme sua função
  server.send(200, "application/json", jsonData);
}
/* // Função para substituir placeholders no HTML
String replacePlaceholders(String content) {
  String placeholders[] = {
    "%LANG%", "%TITLE%", "%HEADING%", "%LABEL_MAX%", "%LABEL_MIN%", "%X_AXIS%", "%Y_AXIS%"
  };
  String values[] = {
    lang, // Idioma (ex.: "pt-BR")
    lang == "pt-BR" ? "Gráfico de Áudio" : lang == "en" ? "Audio Graph" : "Gráfico de Audio", // Título
    lang == "pt-BR" ? "Gráfico de Amplitude" : lang == "en" ? "Amplitude Graph" : "Gráfico de Amplitud", // Cabeçalho
    lang == "pt-BR" ? "Máximo" : lang == "en" ? "Max" : "Máximo", // Rótulo para máximo
    lang == "pt-BR" ? "Mínimo" : lang == "en" ? "Min" : "Mínimo", // Rótulo para mínimo
    lang == "pt-BR" ? "Hora" : lang == "en" ? "Time" : "Hora", // Eixo X
    lang == "pt-BR" ? "Amplitude" : lang == "en" ? "Amplitude" : "Amplitud" // Eixo Y
  };
  for (int i = 0; i < 7; i++) {
    content.replace(placeholders[i], values[i]);
  }
  return content;
}
 */
 
 
 

void handleGraphPage() {
  String filePath = "/" + lang + "/grafico.html";
  File file = SD.open(filePath);
  if (!file) {
    server.send(404, "text/plain", "Arquivo " + filePath + " não encontrado no SD");
    server.client().stop();
    return;
  }
  String content = file.readString();
  file.close();
  server.send(200, "text/html", content); // Sem replaceConfigPlaceholders
}

void handleIndexPage() {
  String filePath = "/" + lang + "/index.html";
  File file = SD.open(filePath);
  if (!file) {
    server.send(404, "text/plain", "Arquivo " + filePath + " não encontrado no SD");
    server.client().stop();
    return;
  }
  String content = file.readString();
  file.close();
  server.send(200, "text/html", content); // Sem replaceConfigPlaceholders
}

void Setup_WebServer() {
    server.on("/", []() {
        server.send(200, "text/html", boot_html);
    });

  // Registra os manipuladores
  server.on("/list", HTTP_GET, handleListFiles);
  server.on("/favicon.ico", HTTP_GET, handleFavicon);
  server.on("/setup.html", HTTP_GET, handleSetupPage);
  server.on("/setup", HTTP_POST, handleSetupSubmit);
  server.on("/restart", HTTP_POST, handleRestart); // Rota para reiniciar
  server.on("/graph", HTTP_GET, handleGraph);
  server.on("/grafico.html", HTTP_GET, handleGraphPage); // Página do gráfico
  server.on("/index.html", HTTP_GET, handleIndexPage);
	
	 // Rotas explícitas para arquivos HTML
   /*  server.on("/index.html", []() {
        File file = SD.open("/index.html");
        if (file && !file.isDirectory()) {
            Serial.println("Servindo /index.html");
            server.streamFile(file, "text/html");
            file.close();
        } else {
            Serial.println("Erro: /index.html não encontrado");
            server.send(404, "text/plain", "Arquivo não encontrado");
        }
    }); */

    server.on("/status.html", []() {
        File file = SD.open("/status.html");
        if (file && !file.isDirectory()) {
            Serial.println("Servindo /status.html");
            server.streamFile(file, "text/html");
            file.close();
        } else {
            Serial.println("Erro: /status.html não encontrado");
            server.send(404, "text/plain", "Arquivo não encontrado");
        }
    });

    server.on("/controle.html", []() {
        File file = SD.open("/controle.html");
        if (file && !file.isDirectory()) {
            Serial.println("Servindo /controle.html");
            server.streamFile(file, "text/html");
            file.close();
        } else {
            Serial.println("Erro: /controle.html não encontrado");
            server.send(404, "text/plain", "Arquivo não encontrado");
        }
    });
	
    
    
	// Manipulador genérico para outros arquivos (CSS, JS, etc.)
    server.onNotFound([]() {
        String path = server.uri();
        Serial.println("Requisição para: " + path);
        File file = SD.open(path);
        if (file && !file.isDirectory()) {
            Serial.println("Arquivo encontrado: " + path);
            String contentType = "text/plain";
            if (path.endsWith(".html")) contentType = "text/html";
            else if (path.endsWith(".css")) contentType = "text/css";
            else if (path.endsWith(".js")) contentType = "application/javascript";
            else if (path.endsWith(".csv")) contentType = "text/csv";

            Serial.println("Enviando arquivo com content-type: " + contentType);
            server.streamFile(file, contentType);
            file.close();
        } else {
            Serial.println("Arquivo NÃO encontrado: " + path);
            if (file) file.close();
            server.send(404, "text/plain", "Arquivo não encontrado");
        }
    });

    server.begin();
    Serial.println("Servidor Web iniciado na porta 80");
}

void check_webserver() {
    server.handleClient();
}

#include <HTTPClient.h>
#include <SD.h>

String versao_local = "0.0.0";

bool Atualizar_Arquivos_Web(String url_base) {
  HTTPClient http;
  
  // Garante que url_base não termine com '/' para evitar duplicação
  if (url_base.endsWith("/")) {
    url_base = url_base.substring(0, url_base.length() - 1);
  }
  
  // Adiciona o idioma à URL base
  String url_com_idioma = url_base + "/" + lang;
  
  // Acessa o arquivo version.txt no diretório do idioma
  http.begin(url_com_idioma + "/version.txt");
  int httpCode = http.GET();

  if (httpCode == 200) {
    String versao_remota = http.getString();
    versao_remota.trim();

    String versao_local = "";
    File vfile = SD.open("/version.txt");
    if (vfile) {
      versao_local = vfile.readStringUntil('\n');
      versao_local.trim();
      vfile.close();
    }

    if (versao_local != versao_remota) {
      Serial.println("Atualizando arquivos web...");

      // Cria o diretório do idioma, se não existir
      String dir_idioma = "/" + lang;
      if (!SD.exists(dir_idioma)) {
        if (SD.mkdir(dir_idioma)) {
          Serial.println("Diretório criado: " + dir_idioma);
        } else {
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
      if (vfile) {
        vfile.println(versao_remota);
        vfile.close();
        Serial.println("Versão atualizada em /version.txt");
      } else {
        Serial.println("Erro ao atualizar /version.txt");
      }

      Serial.println("Atualização concluída.");
      http.end();
      return true;
    } else {
      Serial.println("Arquivos web já estão atualizados.");
      http.end();
      return false;
    }
  } else {
    Serial.println("Erro ao obter versão remota: HTTP " + String(httpCode));
    http.end();
    return false;
  }
}


/* bool Atualizar_Arquivos_Web(String url_base) {
    HTTPClient http;
    http.begin(url_base + "/version.txt");
    int httpCode = http.GET();

    if (httpCode == 200) {
        String versao_remota = http.getString();
        versao_remota.trim();

        File vfile = SD.open("/version.txt");
        if (vfile) {
            versao_local = vfile.readStringUntil('\n');
            vfile.close();
        }

        if (versao_local != versao_remota) {
            Serial.println("Atualizando arquivos web...");
            Baixar_Arquivo_SD(url_base + "/index.html", "/index.html");
            Baixar_Arquivo_SD(url_base + "/setup.html", "/setup.html");
            Baixar_Arquivo_SD(url_base + "/grafico.html", "/grafico.html");
            Baixar_Arquivo_SD(url_base + "/status.html", "/status.html");
			Baixar_Arquivo_SD(url_base + "/controle.html", "/controle.html");
			Baixar_Arquivo_SD(url_base + "/favicon.ico", "/favicon.ico");

            File vfile = SD.open("/version.txt", FILE_WRITE);
            if (vfile) {
                vfile.println(versao_remota);
                vfile.close();
            }

            Serial.println("Atualização concluída.");
            return true;
        } else {
            Serial.println("Arquivos web já estão atualizados.");
            return false;
        }
    } else {
        Serial.println("Erro ao obter versão remota.");
        return false;
    }

    http.end();
} */

