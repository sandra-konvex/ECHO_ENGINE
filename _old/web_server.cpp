// #include <ESPAsyncWebServer.h>
// #include <SD.h>
// #include "globals.h"
// #include "modulo.h"

// AsyncWebServer server(80);
// AsyncEventSource events("/events");

// String currentFileName = "";

// Função auxiliar para contar linhas em um arquivo
// size_t contarLinhas(File file) {
  // size_t count = 0;
  // while (file.available()) {
    // String line = file.readStringUntil('\n');
    // if (line.length() > 5) count++;
  // }
  // file.seek(0); // volta ao início para reutilização
  // return count;
// }

// Função auxiliar para ler últimas N linhas de um arquivo
/* std::vector<String> lerUltimasLinhas(const char* filename, size_t n) {
  std::vector<String> linhas;
  File file = SD.open(filename);
  if (!file) return linhas;

  std::deque<String> buffer;
  while (file.available()) {
    String line = file.readStringUntil('\n');
    if (line.length() > 5) {
      buffer.push_back(line);
      if (buffer.size() > n) buffer.pop_front();
    }
  }
  file.close();

  for (auto& l : buffer) linhas.push_back(l);
  return linhas;
} */

/* // SSE para enviar dados do gráfico
server.on("/grafico_stream", HTTP_GET, [](AsyncWebServerRequest *request){
  AsyncResponseStream *response = request->beginResponseStream("text/plain");
  request->send(response); // precisa de resposta válida, mesmo que não use
}); */
/* 
void enviarDadosGraficoViaSSE() {
  if (!SD.exists(currentFileName)) return;

  File atual = SD.open(currentFileName);
  if (!atual) return;

  size_t linhas_atuais = contarLinhas(atual);

  std::vector<String> todas_linhas;

  if (linhas_atuais < 340) {
    // Precisamos buscar no arquivo anterior
    // Extrai número do epoch do nome do arquivo atual
    int index = currentFileName.indexOf("dados_") + 6;
    int end = currentFileName.indexOf(".jsonL");
    String num = currentFileName.substring(index, end);
    unsigned long epoch = num.toInt();

    // Gera nome do arquivo anterior
    String anteriorFile = "/dados/dados_" + String(epoch - 1) + ".jsonL";
    if (SD.exists(anteriorFile)) {
      auto ultimas = lerUltimasLinhas(anteriorFile.c_str(), 340 - linhas_atuais);
      todas_linhas.insert(todas_linhas.end(), ultimas.begin(), ultimas.end());
    }
  }

  // Adiciona linhas do arquivo atual
  while (atual.available()) {
    String line = atual.readStringUntil('\n');
    if (line.length() > 5) todas_linhas.push_back(line);
  }
  atual.close();

  // Envia uma a uma via SSE
  for (const auto& linha : todas_linhas) {
    events.send(linha.c_str(), "grafico", millis());
    delay(5); // controla velocidade de envio
  }
} */

/* void Setup_WebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", boot_html);
  });

  // outras rotas omitidas...

  // SSE setup
  events.onConnect([](AsyncEventSourceClient *client){
    Serial.println("Cliente SSE conectado");
    client->send("ready", "status", millis());
    enviarDadosGraficoViaSSE(); // envia dados acumulados ao conectar
  });
  server.addHandler(&events);

  server.begin();
  Serial.println("Servidor Web Async iniciado na porta 80");
}
 */