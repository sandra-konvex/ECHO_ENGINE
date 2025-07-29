#ifndef MODULO_H
#define MODULO_H


bool Setup_WiFi();
void check_wifi();

void Setup_WebSocket();
void check_websocket();

bool Setup_card();
bool Write_SD(String data, unsigned long timestamp) ;
String getFileName(unsigned long timestamp);

bool Setup_LittleFS();
void Load_Config_LFS();
void Save_Config_LFS();
bool Write_LFS(String data, unsigned long timestamp);
int countFiles_LFS();



void Save_Config();
void Load_Config();
void Load_Config_SD();

void Setup_Bluetooth();
bool BT_Available();
String BT_Read();
void Process_BT(String command);
void check_bluetooth();

void Setup_LEDs();
void Respira_LED();

void Setup_ADC();
uint16_t Read_MIC();
void Process_Audio(uint16_t amplitude);

void Setup_NTP();
unsigned long Get_Epoch();

void Setup_WebServer();
void check_webserver();

void Baixar_Arquivo_SD(String url, String caminho_local);
// bool Atualizar_Arquivos_Web(String url_base);
bool Atualizar_Arquivos_Web(String baseURL);

String readLastHour();


#endif








