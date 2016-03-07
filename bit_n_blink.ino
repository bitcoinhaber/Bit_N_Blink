/*
 * BitcoinHaber Atolye
 *  
 * Nesnelerin Interneti
 *
 * Bitcoin ile LED kontrolü
 *
 * http://www.bitcoinhaber.net
 * http://twitter.com/bitcoinhaber
 * https://github.com/bitcoinhaber
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid     = "BITCOINHABER_SSID";
const char* password = "Sifre123456";

const char* host     = "blockchain.info"; 
String path          = "/q/addressbalance/";
String btcAdresi     = "1DFx4ePpwCFL7p4gkFSDUDPWoDzVwTptBT"; // Change bitcoin address with YOUR ADDRESS

long int eskiBakiye = 0;
long int yeniBakiye = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("WiFi Baglaniyor ");
  Serial.println("BITCOINHABER_SSID");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Baglandi");
  Serial.println("IP adresi: ");
  Serial.println(WiFi.localIP());
}

boolean isValidBalance(String str){
   for(byte i=0;i<str.length();i++)
   {
      if(isDigit(str.charAt(i))) return true;
        }
   return false;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  setup_wifi();
 
}

void loop() {
 
  Serial.print(host);
  Serial.println(" sunucusuna baglaniyor  ");
  WiFiClientSecure client;
  const int httpPort = 443;
  if (!client.connect(host, httpPort)) {
    Serial.println("... baglanti kesildi");
    return;
  }

  client.print(String("GET ") + path + btcAdresi + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: keep-alive\r\n\r\n");

  delay(5000); // Sunucudan yanit gelmesini bekliyoruz
  String response = "";
  String chunk = "";
  int limit = 1;
 
  // read response
   do {
    if (client.connected()) {
      yield();
      chunk = client.readStringUntil('\n');
      if(chunk.length() > 0) {
         response = chunk;
      }
    }
  } while (chunk.length() > 0 && ++limit < 100);  

  if(isValidBalance(response)) {  // sunucudan donen son satir bir sayi mi?
    yeniBakiye = response.toInt();
    Serial.print("Bakiye=");
    Serial.print(yeniBakiye);
    Serial.println(" satoshi");
    if (yeniBakiye > eskiBakiye) {
      Serial.print("Yeni BTC transferi! Tutar=");
      Serial.print(yeniBakiye-eskiBakiye);
      Serial.println(" satoshi");
      eskiBakiye = yeniBakiye;
      // Bakiye degistiginde LEDi 3 kez yakip sonduruyoruz
      digitalWrite(13, HIGH);
      delay(500);
      digitalWrite(13, LOW);
      delay(500);
      digitalWrite(13, HIGH);
      delay(500);
      digitalWrite(13, LOW);
      delay(500);
      digitalWrite(13, HIGH);
      delay(500);
      digitalWrite(13, LOW);
      delay(500);
    }
  }
  delay(50000);  // 5 saniye bekliyoruz
}

