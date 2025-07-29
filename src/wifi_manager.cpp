/*
 * Copyright (c) 2025, Sandra Bati Furquim
 * All rights reserved.
 *
 * This file is released under the MIT License.
 * See LICENSE file for details.
 */
#include <Arduino.h>
#include <WiFi.h>
#include "globals.h"
#include "modulo.h"

bool Init_Ap()
{
	Serial.println("\n❌ Falha na conexão. Entrando em modo AP...");

	IPAddress local_ip(192, 168, 0, 1);
	IPAddress gateway(192, 168, 0, 1);
	IPAddress subnet(255, 255, 255, 0);

	WiFi.softAPConfig(local_ip, gateway, subnet);
	WiFi.softAP("ECO_SETUP", "12345678");

	IPAddress IP = WiFi.softAPIP();
	Serial.print("📡 IP do AP: ");
	Serial.println(IP);

	return(true);
}


bool Setup_WiFi()
{
	Serial.println("Tentando conectar com os seguintes dados:");
	Serial.print("SSID: ");
	Serial.println(ssid);
	Serial.print("PASS: ");
	Serial.println(password);

	WiFi.begin(ssid, password);
	Serial.print("Conectando ao Wi-Fi");

	int retries = 0;
	while (WiFi.status() != WL_CONNECTED && retries < 20)
	{
		delay(500);
		Serial.print(".");
		retries++;
	}

	if (WiFi.status() == WL_CONNECTED)
	{
		Serial.println("\nConectado com sucesso!");
		Serial.print("IP: ");
		Serial.println(WiFi.localIP());
		return true;
	}
	else
	{
		Serial.println("\nFalha na conexão.");
		Init_Ap();
		return false;
	}
}

void check_wifi()
{
	if (WiFi.status() != WL_CONNECTED)
	{
		Serial.println("Reconectando WiFi...");
		Setup_WiFi();
	}
}
