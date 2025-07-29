/*
 * Copyright (c) 2025, Sandra Bati Furquim
 * All rights reserved.
 *
 * This file is released under the MIT License.
 * See LICENSE file for details.
 */
#include "globals.h"
#include "modulo.h"
#include "hardware.h"

void setup()
{
	Serial.begin(115200);
	Serial.setDebugOutput(true);

	if (!Setup_LittleFS())
	{
		Serial.println("⚠️ Prosseguindo sem sistema de arquivos.");
	}
	else
	{
		Load_Config_LFS();
	}
	

	if (!Setup_card())
	{
		Serial.println("SD não disponível. Prosseguindo sem salvar dados.");
	}

	// if (SD.exists("/config.txt"))
	// {
		// Load_Config_SD(); // SD
	// }

	// Setup_Bluetooth();


	if (Setup_WiFi())
	{
		Atualizar_Arquivos_Web("http://192.168.1.129/esp32");
		delay(1000);
		Setup_NTP();
		// String resposta = readLastHour();
		// Serial.println(resposta);
		Setup_WebSocket();
	}
	else
	{
		Serial.println("Erro ao conectar Wi-Fi. WebSocket não será iniciado.");
	}
	
	Setup_WebServer();
	Setup_LEDs();
	Setup_ADC();
}

void loop()
{
	// check_webserver();
	// check_bluetooth();
	check_wifi();
	// check_websocket();
	Respira_LED();
	// webSocket.loop();

	uint16_t amplitude = Read_MIC();
	Process_Audio(amplitude);

	// Armazena amostras
	if (sample_index < MAX_SAMPLES)
	{
		audio_samples[sample_index++] = amplitude;
	}

	// Verifica se passou 5 segundos
	unsigned long now = millis();
	if (now - audio_window_start >= audio_window_duration)
	{
		audio_window_start = now;

		// Calcula min e max
		uint16_t min_val = 4095;
		uint16_t max_val = 0;

		for (int i = 0; i < sample_index; i++)
		{
			if (audio_samples[i] < min_val) min_val = audio_samples[i];
			if (audio_samples[i] > max_val) max_val = audio_samples[i];
		}

		// Monta JSONL
		unsigned long epoch = Get_Epoch();
		String json = "{\"max\":\"" + String(max_val) +
		              "\",\"min\":\"" + String(min_val) +
		              "\",\"time\":\"" + String(epoch) + "\"}";

		events.send(json, "dado");



		// Envia via WebSocket
		// webSocket.sendTXT(json);
		// Serial.println("Enviado: " + json);
		// events.send(json.c_str(), "dado_audio"); // Evento específico para áudio

		// Serial.println("SSE enviado: " + json);
		
		Write_LFS(json, epoch);

		/* // Grava no SD
		if (!Write_SD(json, epoch))
		{
			Serial.println("Falha ao escrever no SD.");
		} */

		// Reinicia janela
		sample_index = 0;
		Serial.printf("Memória livre: %u bytes\n", ESP.getFreeHeap());
	}

// Serial.printf("Memória livre: %u bytes\n", ESP.getFreeHeap());
	delay(100); // pequeno delay para estabilidade
}
