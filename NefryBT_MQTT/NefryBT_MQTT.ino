#include <Nefry.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

WiFiClient httpsClient;
PubSubClient mqttClient(httpsClient);

// 入力の状態記録
int sensorValueState = LOW;
// 接続先のIP
IPAddress endpoint(192,168,1,50); // Node-REDの動いているPCのIPアドレス

const int port = 1883;

const char *deviceId = "NefryBT_123456789";  // MQTTに接続する固有のデバイス名
const char *pubTopic = "/pub/NefryBT_123456789";  // 送信するMQTTトピック

int PIN = D2;  // 今回センサーがつながったピン番号 D2

void setup() {
  pinMode(PIN,INPUT);
  Serial.begin(115200);
  // 接続先を設定
  mqttClient.setServer(endpoint, port);
  Serial.println(endpoint);
  // 接続開始
  connectMQTT();
}
 
void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print(".");
      if (mqttClient.connect(deviceId)) {
          Serial.println("Connected.");
      } else {
          Serial.print("Failed. Error state=");
          Serial.print(mqttClient.state());
          delay(5000);
      }
  }
}
 
void loop() {
  // MQTTループ
  if (!mqttClient.connected()) {
      connectMQTT();
  }
  mqttClient.loop();
  // センサー反応ON
  int sensorValueCurrent = digitalRead(PIN);
  if ( sensorValueCurrent != sensorValueState ) {
    if( sensorValueCurrent == HIGH ){
      Serial.println("sensor ON");
      mqttClient.publish(pubTopic, "{\"flag\":1}");
    } else {
      Serial.println("sensor OFF");
      mqttClient.publish(pubTopic, "{\"flag\":0}");
    }
  }
  // 入力の状態を次のループまで記憶
  sensorValueState = sensorValueCurrent;
}
