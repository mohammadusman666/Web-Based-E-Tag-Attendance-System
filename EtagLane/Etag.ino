#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <Servo.h>
#include <SPI.h>

#define SS_PIN 2 // D4
#define RST_PIN 0 // D3
MFRC522 mfrc522(SS_PIN, RST_PIN); // MFRC522 instance

const int servo = 15; // D8
Servo serv; // servo instance

const int btn = 4; // D2

const int buzzer = 5; // D1

const char *ssid = "exec-wifi";
const char *password = "Fccpak2018";
String host = "192.168.0.60"; // vmware
String host = "192.168.43.183"; // rpi

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(btn, INPUT_PULLUP);
  digitalWrite(btn, LOW);

  serv.attach(servo); // initiate servo
  lowerBarrier(); // lower barrier

  pinMode(buzzer, OUTPUT);

  SPI.begin(); // initiate SPI bus
  mfrc522.PCD_Init(); // initiate MFRC522

  WiFi.begin(ssid, password); // connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) // wait for connection
  {
    delay(500);
    Serial.print(".");
  }

  // if connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // IP address assigned to your ESP

  IPAddress ip (192, 168, 43, 183); // the remote ip to ping (ubuntu server's ip)
  bool ret = Ping.ping(ip);
  Serial.print("Pinging: ");
  Serial.println(ret);
  Serial.println();
}

void loop()
{
  if (digitalRead(btn) == HIGH) // if button is pressed
  {
    raiseBarrier();
  }
  else // if button is not pressed
  {
    lowerBarrier();
  }
  checkRFID(); // check for any card
}

void raiseBarrier()
{
  serv.write(90); // rotate the servo to the specified angle
}

void lowerBarrier()
{
  serv.write(0); // rotate the servo to the specified angle
}

void checkRFID()
{
  // look for any cards
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  // show UID on serial monitor
  Serial.print("UID tag: ");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    if (i == 0)
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
    }
    else
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    }
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();

  HTTPClient http; // HTTPClient object
  String postData;

  // post data
  postData = "uid=" + content;

  http.begin("http://" + host + "/insert.php"); // specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // specify content-type header

  int httpCode = http.POST(postData); // send the request
  String payload = http.getString(); // get the response payload

  Serial.print("HTTP Code: ");
  Serial.println(httpCode); // print HTTP return code
  Serial.print("Payload: ");
  Serial.println(payload); // print request response payload

  http.end(); // close connection

  if (payload == "OK") // payload returned by ubuntu server
  {
    Serial.println("Access granted!");
    Serial.println();

    raiseBarrier();
    delay(30 * 1000); // wait for 30 secs
    lowerBarrier(); // lower barrier
  }
  else
  {
    Serial.println("Access denied!");
    Serial.println();

    tone(buzzer, 1000); // send 1KHz sound signal...
    delay(1000); // ... for 1 sec
    noTone(buzzer); // stop sound...
    delay(1000); // ... for 1sec
  }
}
