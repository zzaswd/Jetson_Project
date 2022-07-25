#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <string.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
WiFiClient client;


const char * host = "10.10.141.188";
const uint16_t port = 5001;

int trig = 13;
int echo = 12;
int LED = 4;
int button = 5;
bool led_status = false;
unsigned long past = 0;
int pre_charging = 0;

int init_conn = 1; //입차및출차시 주소, 포트 연결하여 In,Out으로 로그인
int msg_RevData; //입차시 1, 출차시 2 출력

void ClientConn();
void MsgSend();
void ledcontrol(char *data);

void setup() {
  Serial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(14, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(button, INPUT);

  WiFi.begin("iot24kcci", "iot40000");  //WiFi 연결
  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
  }
  Serial.println("Connected to wifi successful!");
}
int flag = 0;
int Hyojun = 0;
int in_out = -1;

char * car_info[4];
char ch[30] = {0};

void loop() {
  memset(ch, 0, sizeof(ch));

  int str_len;
  if ((WiFiMulti.run() == WL_CONNECTED))
  {
    if (flag == 0) {
      if (init_conn == 1) {
        ClientConn();
        client.print("[SPOT1:PASSWD]");
        init_conn = 0;
      }
      msg_RevData = 1;
      //MsgSend();
    }
    int index = 0;

    while (client.available()) {
      // ch = static_cast<char>(client.read());
      ch[index++] = static_cast<char>(client.read());
      if (ch[index - 1] == 'L')
      {
        // 0 9315 3L
        char *pToken;
        char *pArray[10] = {0};
        ch[index - 1] = '\0';
        ch[0] = '@';
        ch[6] = '@';
        //str_len = strlen(ch);
        Serial.println(ch);

        pArray[0] = strtok(ch, "@");
        //Serial.print("첫번째 : "); Serial.println(pToken); // block

        pArray[1] = strtok(NULL, "@");
        //Serial.print("두번째 : "); Serial.println(pToken); // 띄어쓰기 하고 1

        pArray[2] = strtok(NULL, "@");
        //Serial.print("세번째 : "); Serial.println(pToken); // 9315

        pArray[3] = strtok(NULL, "@");
        //Serial.print("네번째 : "); Serial.println(pToken); // 1

        /*
          int i = 0;
          while (pToken != NULL)
          {
          pArray[i] = pToken;
          if (++i > 5)
            break;
          pToken = strtok(NULL, "@");
          }
        */

        //Serial.println(pArray[1]);

        if (strcmp(pArray[0], "admin") == 0)
          ledcontrol(pArray[1]);
        else if (strcmp(pArray[0], "block") == 0) {
          car_info[1] = pArray[1]; // In/Out
          car_info[2] = pArray[2]; // Car Number
          car_info[3] = pArray[3]; // Type

          if (atoi(car_info[1]) == 1) {
            Serial.println("차단기에서 왔대!");
            while (1) {
              digitalWrite(trig, HIGH);
              delayMicroseconds(10);
              digitalWrite(trig, LOW);
              int dis = pulseIn(echo, HIGH) * 340 / 2 / 10000;
              if (dis > 15) {
                if (led_status == false) {
                  digitalWrite(LED, HIGH);
                }
                else {
                  digitalWrite(LED, LOW);
                }
                led_status = !led_status;
                continue;
              }
              else if (dis <= 15 && dis != 0) {
                Serial.println("들어왔다!");
                digitalWrite(LED, HIGH);
                Hyojun = 1;
                /*
                  if (digitalRead(button) == 1) {
                  break;
                  }
                */
                break;

              }
              delay(200);
            }
            if (Hyojun == 1) break;
          }
          else {
            // 나가는 신호
            Serial.println("Bye");
            digitalWrite(LED, LOW);
            Hyojun = 2;
          }
          break;
        }
        fflush(stdout);

      }

    }

    if (Hyojun == 1) {
      //while (client.available()) {
      Serial.println("Moving Start");
      delay(1000);
      //Moving Start
      char send_msg[30] = "";
      sprintf(send_msg, "[Jetson1]%s@%s@%sL\n", car_info[1], car_info[2], car_info[3]);
      client.print(send_msg);
      delay(1000);

      int ori_val, cur_charging;
      char bat_msg[30] = "";
      char Bat_Stop[30] = "";
      index = 0;
      while (1) {
        ori_val = analogRead(A0);
        cur_charging = map(ori_val, 0, 1023, 0, 100);
        if (cur_charging != pre_charging)
        {
          memset(bat_msg, 0, sizeof(bat_msg));
          sprintf(bat_msg, "[admin] %d@\n", cur_charging);
          client.print(bat_msg);
          Serial.println(bat_msg);
          pre_charging = cur_charging;
          delay(500);
        }
        // [Jetson1]break;

        if (client.available()) {
          Serial.println("Avail???");
          Bat_Stop[index++] = static_cast<char>(client.read());
          Serial.print("Bat = ");
          Serial.println(Bat_Stop);
          if (Bat_Stop[index - 1] == 'L')
          {
           // Serial.print(" 끄래! = ");
           // Serial.println(Bat_Stop);
            char *pToken2;
            char *pArray2[10] = {0};
            //memset(Bat_Stop, 0, sizeof(Bat_Stop));
            //Bat_Stop[index - 1] = '\0';
            str_len = strlen(ch);
            //Bat_Stop[0] = '@';
            //Bat_Stop[6] = '@';
            //Serial.print(ch);
            pArray2[0] = strtok(Bat_Stop, "]");
            pArray2[1] = strtok(NULL, "L");
            Serial.print("pArray[0] = " );
            Serial.println(pArray2[0]);
            Serial.print("pArray[1] = " );
            Serial.println(pArray2[1]);
            

            if ((strcmp(pArray2[0], "admin") == 0 && strcmp(pArray2[1], " OFF") == 0) || (strcmp(pArray2[0], "Jetson1") == 0 && strcmp(pArray2[1], "OFF") == 0)) {
              digitalWrite(LED, LOW);
              break;
            }
          }
          else if(Bat_Stop[index -1] == '\0' || index >20){
            memset(Bat_Stop,0,sizeof(Bat_Stop));
            index = 0;
          }
        }
        if (cur_charging == 100) {
          break;
        }
      }
      Serial.println("충전완료");
      Hyojun = 2;
      //Finish();
    }
    //}
    //ch[index] = '\0';

    //flag = 1;

  }

}


void ClientConn()
{
  if (!client.connect(host, port)) {
    Serial.println("Connection to host failed");
    delay(1000);
    return;
  }
  else
    Serial.println("Connected to CarTower server successful!");
}

/*
  void MsgSend(char *To, char *send_msg)
  {
  sprintf(msg, "[Jetson1] %d\n", msg_RevData);
  sprintf(msg, "[admin] %d\n", msg_RevData);
  //Serial.println(msg);
  client.print(msg);
  }

*/

void ledcontrol(char *data)
{
  if (strcmp(data, "ON") == 0)
  {
    digitalWrite(LED, HIGH);
  }
  else if (strcmp(data, "OFF") == 0)
  {
    digitalWrite(LED, LOW);
  }

}
