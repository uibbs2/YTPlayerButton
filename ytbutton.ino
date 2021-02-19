/*
 * Counter degli iscritti/play button digitale
 * Su progetto di Becky Stern, qui:
 *  https://www.instructables.com/YouTube-Subscriber-Counter-With-ESP8266-V2/
 * Modificato da Grizzly in febbraio 2021 per: 
 *  - utilizzare un display TM1637
 *  - fornire sul display stato dell'esecuzione
 *  - correggere l'errore "unexpected https status 0" delle api
 * 
 * TODO:
 *  - predisposizione per andare con il display 7 segmenti doppio
 *  - gestione debug
 */

/*
 * Display sette segmenti per il counter
 *  (collegare alimentazione a 3.3V e GND)
 */
#include <TM1637Display.h>       // https://github.com/avishorp/TM1637
const int CLK = D6; // il pin CLK del display è collegato al pin D6 di NodeMCU
const int DIO = D5; // come sopra: DIO su D5
TM1637Display display(CLK, DIO); // inizializza l'oggetto display

/*
 * Rete wifi
 * 
 */
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
char ssid[] =     "LA_MIA_RETE_WIRELESS";       // SSID (nome) della rete wifi
char password[] = "abcdefgh12345678ijkl1234";   // password per la connessione
WiFiClientSecure client;    // inizializza l'oggetto client per il wifi

/*
 * Youtube API
 *  una volta attivata una chiave, abilita l'api Youtube v3,
 *  poi limita quella chiave alla sola youtube api v3
 *
 */
#include <YoutubeApi.h>            // https://github.com/witnessmenow/arduino-youtube-api
#include <ArduinoJson.h>           // https://github.com/bblanchon/ArduinoJson
#define API_KEY     "ABcdEFgh12345678ijKL1234mNOpQrStU5678vZ" // Token autenticazione
#define CHANNEL_ID  "UCns9zEpyUFH3uRZGrT3an8Q"                // id del canale
YoutubeApi api(API_KEY, client);

uint16_t subscriberCount; // variabile per contenere il numero di iscritti

/*
 * Altre variabili di sistema
 * 
 */
uint32_t api_mtbs = 1000;  // tempo medio di attesa fra le richieste (ms), default 1 secondo
uint32_t api_lasttime;     // indicatore dell'ultima richiesta

// Messaggio in attesa di connessione alla wifi (nt e cerchietto che gira)
byte m_NET[4][4]={        // in attesa di connessione di rete, animazione di rotazione
  {0x00,0x18,0x03,0x00},  //sb+b,da+a
  {0x00,0x30,0x06,0x00},  //sa+sb,db+da
  {0x00,0x21,0x0C,0x00},  //sa+a,b+db
  {0x00,0x09,0x09,0x00},  //a+b,a+b
};

short i;  // variabili di servizio per cicli vari

/*
 * INIZIO
 */
void setup(){
  Serial.begin(115200);     // Inizializza la seriale, per il debug

  display.setBrightness(6); // Luminosità (0..7)

  // Inizializza la rete
  i=0;  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){   // animazione in attesa di connessione wireless
    display.setSegments(m_NET[i]);  delay(40);
    i++;  if(i==sizeof(m_NET)/sizeof(m_NET[0])){i=0;}
  }

  // soluzione trovata su https://github.com/witnessmenow/arduino-youtube-api/issues/25
  client.setInsecure();   // risolve "Unexpected HTTP Status Code: 0"
}

/*
 * CICLO PRINCIPALE
 */
void loop(){
  if (millis()>api_lasttime+api_mtbs){
    if(api.getChannelStatistics(CHANNEL_ID)){ // prova la lettura
/*      Serial.println("===== Statistiche YT =====");   // DEBUG
      Serial.print("Iscritti.......: ");  Serial.println(api.channelStats.subscriberCount);
      Serial.print("Visualizzazioni: ");  Serial.println(api.channelStats.viewCount);
      Serial.print("Video totali...: ");  Serial.println(api.channelStats.videoCount);
      Serial.print("Commenti.......: ");  Serial.println(api.channelStats.commentCount);
      Serial.println("===========================");  Serial.println();
*/
      subscriberCount = api.channelStats.subscriberCount;
      display.showNumberDec(subscriberCount);
      api_lasttime=millis();
    }//.if_api
  }//.if_millis
}//.loop()