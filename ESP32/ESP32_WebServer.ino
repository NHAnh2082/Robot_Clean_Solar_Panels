/******************************************************************************
 * @file    main.cpp
 * @brief   Chương trình điều khiển robot làm sạch pin năng lượng mặt trời
 * @details Chương trình dùng ESP32 chạy WebServer bất đồng bộ, WebSocket,
 *          kết nối qua WiFi Access Point, nhận lệnh điều khiển robot,
 *          truyền dữ liệu tốc độ, trạng thái và nhận dữ liệu cảm biến từ STM32.
 * @author  Anh Nguyen
 * @date    2024-06-02
 * @version 1.0
 ******************************************************************************/
 
#include <WiFi.h>
#include <esp_wifi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include "Config_WiFi.h"

/*------------------------------SEVER---------------------------------*/
#include "htmlHomePage.h"           /**< Nội dung HTML trang chủ được nhúng sẵn */
#define serverPort 80               /**< Cổng HTTP mặc định */
AsyncWebServer server(serverPort);  /**< Tạo server bất đồng bộ cổng 80 */
AsyncWebSocket ws("/ws");           /**< Khai báo WebSocket endpoint */
String domain = "CSProbot";         /**< Tên miền mDNS */

/**
 * @brief   Xử lý sự kiện WebSocket
 * @param   server  Pointer đến server WebSocket
 * @param   client  Pointer đến client kết nối
 * @param   type    Loại sự kiện WebSocket
 * @param   arg     Thông tin bổ sung
 * @param   data    Dữ liệu nhận được
 * @param   len     Độ dài dữ liệu
 */
void onWebSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) 
{
  if (type == WS_EVT_CONNECT) 
  {
    // Khi một máy khách kết nối
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
  } 
  else if(type == WS_EVT_DISCONNECT) 
  {
    // Khi một máy khách ngắt kết nối
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
  } 
  else if(type == WS_EVT_DATA) 
  {
    // Khi máy khách gửi dữ liệu
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    
    if (info->final && info->index == 0 && info->len == len) 
    {
      // Xử lý dữ liệu nhận được
      if(info->opcode == WS_TEXT) 
      {
        data[len] = 0;
        Serial.printf("Received text: %s\n", data);
      }
    }
  }
}

/**
 * @brief Định nghĩa kiểu enum điều hướng robot
 */
enum class Navigation
{
  Forward,
  Backward,
  Left,
  Right,
  StopRobot
};

Navigation command = Navigation::StopRobot;

// Các chuỗi ký tự cho trạng thái động cơ, bơm, camera ...
const char *wheel_speed   = "wheelSpeed";
const char *brush_speed   = "brushSpeed";
const char *status_pump   = "statusPump";
const char *status_auto   = "statusAuto";
const char *status_brush  = "statusBrush";
const char *status_camera = "statusCamera";
const char *status_wheel  = "statusWheel";

// Các biến trạng thái toàn cục
int statusCamera, statusBrush, statusAuto;
int statusPump, statusWheel, wheelSpeed, brushSpeed;

// Cờ hiệu hiển thị trạng thái di chuyển
bool displayFW  = false;
bool displayBW  = false;
bool displayTL  = false;
bool displayTR  = false;
bool displayST  = false;

// Cờ hiệu cho cấu hình đã nhận
bool isSetup = false;

// Cấu hình UART (giao tiếp STM32)
#define RXD2 16
#define TXD2 17
#define BUFF_SIZE_RX 59
#define BUFF_SIZE_TX 24
 
String txbuff  = "";
String txbuff2 = "";

char data_navi  = 'S'; /* Variable contains characters 'S','F','B','R','L' */
char data_rasp1 = '0';
char data_rasp2 = '0';
String data_rcv = "";
bool rcv_flag = false;

String rxbuff = "";
String data_send = "";

// Cấu hình ngắt khẩn cấp (stop emergency)
#define SE 32  // SE: stop emergency
bool isSE = false;
void IRAM_ATTR ISR();
void handle_StopRobotEmergency(AsyncWebServerRequest *request);


/*----------------------------FUNCTIONS------------------------------*/
void trans_wifi();
void handle_Root(AsyncWebServerRequest *request);
void handle_NotFound(AsyncWebServerRequest *request);
void handle_Setup(AsyncWebServerRequest *request);
void handle_Forward(AsyncWebServerRequest *request);
void handle_Backward(AsyncWebServerRequest *request);
void handle_TurnLeft(AsyncWebServerRequest *request);
void handle_TurnRight(AsyncWebServerRequest *request);
void handle_StopRobot(AsyncWebServerRequest *request);
void handle_StopBr(AsyncWebServerRequest *request);
void handle_RunBrush(AsyncWebServerRequest *request);
void handle_AutoMode(AsyncWebServerRequest *request);
void handle_Pump(AsyncWebServerRequest *request);
void check_wifi_status(void);
void get_device( WiFiEvent_t event, WiFiEventInfo_t info);
void lost_device(WiFiEvent_t event, WiFiEventInfo_t info);
void get_WheelSpeed(AsyncWebServerRequest *request);
void get_BrushSpeed(AsyncWebServerRequest *request);
void get_Devices(AsyncWebServerRequest *request);
void control_Robot(Navigation cmd);
void IntToStr(int length, int u, char *y);
void StrToInt(String str, int a);
void send_data_to_STM(void);
void read_data_from_STM(void);
void read_data_from_rasp(void);
void handleSendData(AsyncWebServerRequest *request);
void broadcastUpdate();

void setup() 
{
  Serial.begin(115200); 

  /* Configure UART2 parameters */
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  
  /* Declare an external interrupt */
  pinMode(SE, OUTPUT);
  digitalWrite(SE, LOW);
  attachInterrupt(SE, ISR, RISING);  

  /* Transmit WiFi and get IP address*/
  trans_wifi();     
  IP = WiFi.softAPIP();
  Serial.println("IP:" + IP.toString());
  
  /* Set up an mDNS domain */
  if (!MDNS.begin("CSProbot"))
  {
    Serial.println("Error set up MDNS responder!");
    while(1) delay(1000);
  }
  Serial.println("MDNS responder started");
  
  /* Get devices connected and disconnected */
  WiFi.onEvent(get_device, SYSTEM_EVENT_AP_STACONNECTED);
  WiFi.onEvent(lost_device,SYSTEM_EVENT_AP_STADISCONNECTED);
  delay(1);

  /* Initialize the server */
  server.on("/", HTTP_GET, handle_Root);        

  /* Get mac address of devices */
  server.on("/devices", HTTP_GET, get_Devices); 

  /* Stop robot emergency */
  server.on("/stopEmergency", HTTP_GET, handle_StopRobotEmergency);
  
  /* Receive initial parameters from server */
  server.on("/setup", HTTP_GET, handle_Setup);  

  /* Navigation for robot */
  server.on("/forward", HTTP_GET, handle_Forward);   /* Send char 'F' to notify that robot will move forward */
  server.on("/backward",HTTP_GET, handle_Backward);  /* Send char 'B' to notify that robot will move backward */
  server.on("/left",    HTTP_GET, handle_TurnLeft);  /* Send char 'L' to notify that robot will turn left */
  server.on("/right",   HTTP_GET, handle_TurnRight); /* Send char 'R' to notify that robot will turn right */
  server.on("/stop1",   HTTP_GET, handle_StopRobot); /* Send char 'S' to notify that robot will stop */

  /* Adjust speed */
  server.on("/changeSpeed1", HTTP_GET, get_WheelSpeed);
  server.on("/changeSpeed2", HTTP_GET, get_BrushSpeed);

  /** For other tasks such as turning on/off auto mode, brush, water pump and camera: send value '1' to run/open and '0' to stop/close **/
  /* Turn on/off brush motor */
  server.on("/brush", HTTP_GET, handle_RunBrush);

  /* Turn on/off water pump through relay */
  server.on("/pump", HTTP_GET, handle_Pump);
  
  /* Turn on auto mode for robot */
  server.on("/auto", HTTP_GET, handle_AutoMode);

  /* Send data to display on server */
  server.on("/data",  HTTP_GET, handleSendData); 
  
  /* Handle if can not found the server */
  server.onNotFound(handle_NotFound);

  /* Start server */
  server.begin(); 
  MDNS.addService("http", "tcp", 80);
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);
}

void loop() 
{
  if (isSE)
  {
    Serial2.flush();
    txbuff2 = String("SE") + '\r' + '\n'; // SE\r\n
    Serial.println("Data send to STM32 - String: " + txbuff2);
    Serial2.write(txbuff2.c_str(), txbuff2.length());
    delay(100);
    
    while (Serial2.available())
    {
      char receivedChar = char(Serial2.read());
      Serial.printf("Received char: %c\n",receivedChar);
      
      if (receivedChar == 'R')
      {
        digitalWrite(SE, LOW);
        isSE = false;
      }
    }
  }
  
  if (!isSE)
  {
    DynamicJsonDocument doc(1024);
    doc["wheelSpeed"] = wheelSpeed;
    doc["brushSpeed"] = brushSpeed;
    doc["statusBrush"] = statusBrush;
    doc["statusPump"] = statusPump;
    doc["statusAuto"] = statusAuto;
    String json;
    serializeJson(doc, json);
    ws.textAll(json);
    control_Robot(command);
    
    if (!Serial.available())
    {
      data_rasp1 = 0x30;
      data_rasp2 = 0x30;
      send_data_to_STM();
    }
    else if (Serial.available())
    {
      if (!rcv_flag) 
      {
        read_data_from_rasp();
        
        if (rcv_flag) 
        {
          send_data_to_STM();
          data_rcv = "";
          rcv_flag = false; 
        }
      }
    }
    read_data_from_STM();
  }
  delay(200);
}

/**
 * @brief   Khởi tạo WiFi Access Point và cấu hình các tham số
 * @details
 *  - Chế độ WiFi: Access Point
 *  - Thiết lập tên SSID, mật khẩu, kênh, số kết nối tối đa
 *  - Cấu hình địa chỉ IP tĩnh cho AP
 *  - Cấu hình công suất phát, chế độ tiết kiệm năng lượng, tự kết nối lại
 *  - In thông tin cấu hình ra Serial Monitor
 */
void trans_wifi()
{
  String status_wifi = "";
  esp_err_t err;
  WiFi.mode(WIFI_AP);
  esp_wifi_set_protocol(WIFI_IF_AP, protocol1|protocol2|protocol3);
  WiFi.softAP(ssid_ap, pass_ap, WiFiChannel, 0, maxConnection);
  WiFi.softAPConfig(localip, gateway, subnet);
  status_wifi = WiFi.softAPConfig(localip, gateway, subnet) ? "Ready" : "Failed";

  if (status_wifi == "Falied")
  {
    WiFi.softAPConfig(localip, gateway, subnet);
  }

  WiFi.setTxPower(WIFI_POWER_19dBm);
  WiFi.setAutoReconnect(false);
  WiFi.setAutoConnect(false);
  WiFi.persistent(false);

  if (WiFi.getSleep() == true)
  {
    WiFi.setSleep(false);
  }

  /* Print on serial screen */
  Serial.println("ACCESS POINT");
  Serial.println("SSID:" + String(ssid_ap));
  Serial.println("PASS:" + String(pass_ap));
}

/**
 * @brief   Xử lý request HTTP khi truy cập trang chủ
 * @param   request  Con trỏ tới đối tượng request
 */
void handle_Root(AsyncWebServerRequest *request)
{
  request->send(200, "text/html", htmlHomePage);
}

/**
 * @brief   Xử lý request cấu hình tham số điều khiển robot
 * @details
 *  - Kiểm tra và lấy tham số truyền lên từ form: tốc độ bánh xe, bàn chải,
 *    trạng thái bàn chải, bánh xe, chế độ tự động, bơm nước.
 *  - Chuyển đổi chuỗi sang số nguyên và giới hạn giá trị hợp lệ.
 *  - Gửi phản hồi kết quả nhận tham số cho client.
 *  - Gửi dữ liệu xuống STM qua hàm send_data_to_STM() nếu nhận thành công.
 * @param   request  Con trỏ tới đối tượng request
 */
void handle_Setup(AsyncWebServerRequest *request)
{    
  /* Get value from WebServer */
  if (request->hasParam(wheel_speed) && request->hasParam(brush_speed) &&
      request->hasParam(status_brush)&& request->hasParam(status_wheel)&&
      request->hasParam(status_auto) && request->hasParam(status_pump))
  {

    String speed1 = request->getParam(wheel_speed)->value();   StrToInt(speed1, wheelSpeed);
    String speed2 = request->getParam(brush_speed)->value();   StrToInt(speed2, brushSpeed);
    String pump   = request->getParam(status_pump)->value();   StrToInt(pump,   statusPump);
    String brush  = request->getParam(status_brush)->value();  StrToInt(brush,  statusBrush);
    String wheel  = request->getParam(status_wheel)->value();  StrToInt(wheel,  statusWheel);
    String automode = request->getParam(status_auto)->value(); StrToInt(automode, statusAuto);

    if (wheelSpeed < 0)       wheelSpeed = 0;
    else if (wheelSpeed > 23) wheelSpeed = 23;

    if (brushSpeed < 0)       brushSpeed = 0;
    else if (brushSpeed > 53) brushSpeed = 53;

    Serial.println("Received parameters");
    Serial.printf("Wheel Speed: %d\n", wheelSpeed);
    Serial.printf("Brush Speed: %d\n", brushSpeed);
    Serial.printf("Status wheel: %d\n", statusWheel);
    Serial.printf("Status brush: %d\n", statusBrush);
    Serial.printf("Status auto: %d\n", statusAuto);
    Serial.printf("Status pump: %d\n", statusPump);
    isSetup = true;
    request->send(200, "text/plain", "Parameters received successful");
  }
  else
  {
    isSetup = false;
    request->send(404, "text/plain", "Missing parameters");
  }

  if (isSetup) send_data_to_STM();
  else return;
}

/**
 * @brief   Xử lý dừng robot khẩn cấp
 * @param   request  Con trỏ tới đối tượng request
 */
void handle_StopRobotEmergency(AsyncWebServerRequest *request)
{
  digitalWrite(SE, HIGH);
}

//------------------------------------------------------------------
void IRAM_ATTR ISR()
{
  isSE = true;
}

/**
 * @brief   Xử lý lệnh di chuyển robot đi thẳng
 * @param   request  Con trỏ tới đối tượng request
 */
void handle_Forward(AsyncWebServerRequest *request)
{
  displayFW = true;
  command = Navigation::Forward;
  request->send(200, "text/plain", "OK");
}

/**
 * @brief   Xử lý lệnh di chuyển robot đi lùi
 * @param   request  Con trỏ tới đối tượng request
 */
void handle_Backward(AsyncWebServerRequest *request)
{
  displayBW = true;
  command = Navigation::Backward;
  request->send(200, "text/plain", "OK");
}

/**
 * @brief   Xử lý lệnh di chuyển robot rẽ trái
 * @param   request  Con trỏ tới đối tượng request
 */
void handle_TurnLeft(AsyncWebServerRequest *request)
{
  displayTL = true;
  command = Navigation::Left;
  request->send(200, "text/plain", "OK");
}

/**
 * @brief   Xử lý lệnh di chuyển robot rẽ phải
 * @param   request  Con trỏ tới đối tượng request
 */
void handle_TurnRight(AsyncWebServerRequest *request)
{
  displayTR = true;
  command = Navigation::Right;
  request->send(200, "text/plain", "OK");
}

/**
 * @brief   Xử lý lệnh dừng robot
 * @param   request  Con trỏ tới đối tượng request
 */
void handle_StopRobot(AsyncWebServerRequest *request)
{
  displayST = true;
  command = Navigation::StopRobot;
  request->send(200, "text/plain", "OK");
}

/**
 * @brief   Xử lý lệnh điều khiển robot, gửi lệnh tới STM32
 * @param   cmd     Lệnh điều khiển robot (enum Navigation)
 */
void control_Robot(Navigation cmd)
{
  switch(cmd)
  {
    case Navigation::StopRobot:
      if (displayST)
      {
        Serial.println("Stop Robot");
        displayST = false;
      }
      data_navi = 'S';
      send_data_to_STM();
      break;
      
    case Navigation::Forward:
      if (displayFW)
      {
        Serial.printf("Move Forward - Speed: %d\n", wheelSpeed);
        displayFW = false;
      }
      data_navi = 'F';
      send_data_to_STM();
      break;
      
    case Navigation::Backward:
      if (displayBW)
      {
        Serial.printf("Move Backward - Speed: %d\n", wheelSpeed);
        displayBW = false;
      }
      data_navi = 'B';
      send_data_to_STM();
      break;
      
    case Navigation::Left:
      if (displayTL)
      {
        Serial.printf("Turn Left - Speed: %d\n", wheelSpeed);
        displayTL = false;
      }
      data_navi = 'L';
      send_data_to_STM();
      break;
      
    case Navigation::Right:
      if (displayTR)
      {
        Serial.printf("Turn Right - Speed: %d\n", wheelSpeed);
        displayTR = false;
      }
      data_navi = 'R';
      send_data_to_STM();
      break;
      
    default:  
      break;
  }
}

/**
 * @brief   Xử lý nhận tốc độ bánh xe từ request HTTP GET
 * @param   request  Con trỏ tới đối tượng request
 */
void get_WheelSpeed(AsyncWebServerRequest *request)
{
  if (request->hasParam("value1")){
    int speed1 = request->getParam("value1")->value().toInt();
    if (speed1 >= 0 && speed1 <= 23){
      wheelSpeed = speed1;
      //Serial.printf("Wheel speed: %d\n", wheelSpeed);
    }
    request->send(200, "text/plain", "Speed value received successful");
  }
  else{
    request->send(404, "text/plain", "Missing speed value");
  }
  send_data_to_STM();
}

/**
 * @brief   Xử lý nhận tốc độ bàn chải từ request HTTP GET
 * @param   request  Con trỏ tới đối tượng request
 */
void get_BrushSpeed(AsyncWebServerRequest *request)
{
  if (request->hasParam("value2"))
  {
    int speed2 = request->getParam("value2")->value().toInt();
    if (speed2 >= 0 && speed2 <= 53)
    {
      brushSpeed = speed2;
      //Serial.printf("Brush speed: %d\n", brushSpeed);
    }
    request->send(200, "text/plain", "Speed value received successful");
  }
  else
  {
    request->send(404, "text/plain", "Missing speed value");
  }
  send_data_to_STM();
}

/**
 * @brief   Xử lý bật/tắt động cơ chổi theo request HTTP POST
 * @param   request  Con trỏ tới đối tượng request
 */
void handle_RunBrush(AsyncWebServerRequest *request)
{
  String brush;
  if (request->hasParam(status_brush))
  {
    statusBrush = request->getParam(status_brush)->value().toInt();
  }
  request->send(200, "text/plain", "OK");

  //StrToInt(brush, statusBrush);

  if (statusBrush == 1)
  {
    Serial.println("Run Brush");
  }
  else if (statusBrush == 0)
  {
    Serial.println("Stop Brush");
  }
  send_data_to_STM();
}

/**
 * @brief   Xử lý bật/tắt chế độ tự động robot
 * @param   request  Con trỏ tới đối tượng request
 */
void handle_AutoMode(AsyncWebServerRequest *request)
{
  String automode;

  if (request->hasParam(status_auto))
  {
    statusAuto = request->getParam(status_auto)->value().toInt();
  }
  request->send(200, "text/plain", "OK");

  //StrToInt(automode, statusAuto);

  if (statusAuto == 1)
  {
    Serial.println("Run Auto Mode");
  }
  else if (statusAuto == 0)
  {
    Serial.println("Stop Auto Mode");
  }
  send_data_to_STM();
}

/**
 * @brief   Xử lý bật/tắt máy bơm nước theo request HTTP POST
 * @param   request  Con trỏ tới đối tượng request
 */
void handle_Pump(AsyncWebServerRequest *request)
{
  String pump;

  if (request->hasParam(status_pump))
  {
    statusPump = request->getParam(status_pump)->value().toInt();
  }
  request->send(200, "text/plain", "OK");

  //StrToInt(pump, statusPump);

  if (statusPump == 1)
  {
    Serial.println("Open Pump");
  }
  else if (statusPump == 0)
  {
    Serial.println("Close Pump");
  }
  send_data_to_STM();
}

/**
 * @brief   Xử lý trang lỗi khi không tìm thấy URL phù hợp
 * @param   request  Con trỏ tới đối tượng request
 */
void handle_NotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Page Not Found");  //404 or 500
}

/**
 * @brief   Sự kiện có thiết bị kết nối tới AP
 * @param   event   Loại sự kiện WiFi
 * @param   info    Thông tin thiết bị kết nối
 */
void get_device(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println();
  deviceCount = WiFi.softAPgetStationNum();
  Serial.printf("Number of connected stations: %d\n",deviceCount);
  Serial.println("Station connected");
  for (int i=0; i<6; i++)

  {
    Serial.printf("%02X", info.sta_connected.mac[i]);  
    if (i<5) Srial.print(":");
  }
  Serial.println("\n------------");
}

/**
 * @brief   Sự kiện thiết bị ngắt kết nối khỏi AP
 * @param   event   Loại sự kiện WiFi
 * @param   info    Thông tin thiết bị ngắt kết nối
 */
void lost_device(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println();
  deviceCount = WiFi.softAPgetStationNum();
  Serial.printf("Number of connected stations: %d\n",deviceCount);
  Serial.println("Station disconnected");

  for (int i=0; i<6; i++)
  {
    Serial.printf("%02X", info.sta_disconnected.mac[i]);  
    if (i<5) Serial.print(":");
  }
  Serial.println("\n------------");
}

/**
 * @brief   Xử lý request lấy danh sách thiết bị đang kết nối
 * @param   request  Con trỏ tới đối tượng request
 */
void get_Devices(AsyncWebServerRequest *request)
{
  deviceCount = WiFi.softAPgetStationNum();

  for (int i=0; i<deviceCount; i++)
  {
    String mac_address = WiFi.softAPmacAddress();
    deviceList += mac_address + "<br>";
  }
  request->send(200, "text/html", deviceList);
}

/**
 * @brief   Gửi dữ liệu điều khiển xuống STM32 qua UART
 */
void send_data_to_STM(void)
{
  char data_wheelspeed[4], data_brushspeed[4];
  IntToStr(3, wheelSpeed, data_wheelspeed);
  IntToStr(3, brushSpeed, data_brushspeed);
  txbuff = "M_" + String(data_navi) + '_' + String(data_wheelspeed) + '_' 
                     + statusBrush + '_' + String(data_brushspeed) + '_' 
                     + statusAuto  + '_' + statusPump + '_' 
                     + String(data_rasp1) + '_' + String(data_rasp2) + '_' + '\r' + '\n';
  Serial.println(txbuff);
  
  if (txbuff.length() == 24)
  {
    Serial2.write(txbuff.c_str(),txbuff.length());
  }            
  else
  {
    Serial2.flush();
  }
  
  delay(200);
}

/**
 * @brief   Chuyển đổi số nguyên sang chuỗi ký tự ASCII
 * @param   length  Độ dài chuỗi kết quả
 * @param   u       Số nguyên cần chuyển đổi
 * @param   y       Bộ đệm chứa chuỗi kết quả (cần đủ kích thước)
 */
void IntToStr(int length, int u, char *y)
{
  int a = 0;
  a = u;
  for (int i=length-1; i>=0; i--)
  {
    y[i] = (a % 10) + 0x30;
    a /= 10;
  }
  y[length] = '\0';
}

/**
 * @brief   Chuyển chuỗi số (String) sang số nguyên int
 * @param   str     Chuỗi ký tự số
 * @param   a       Biến int nhận kết quả
 * @note    Hàm gán giá trị vào a, nên truyền tham chiếu mới đúng.
 */
void StrToInt(String str, int a)
{
  int value = 0;

  switch (str.length())
  {
    case 4:
      value = (str[3] - 0x30) + (str[2] - 0x30)*10 + (str[1] - 0x30)*100 + (str[0] - 0x30)*1000;
      break;

    case 3:
      value = (str[2] - 0x30) + (str[1] - 0x30)*10 + (str[0] - 0x30)*100;
      break;

    case 2:
      value = (str[1] - 0x30) + (str[0] - 0x30)*10;
      break;

    case 1:
      value = str[0] - 0x30;
      break;

    default:
      break;
  }

  a = value;
}

/**
 * @brief   Đọc dữ liệu từ module Raspberry Pi qua Serial
 */
void read_data_from_rasp(void)
{
  while (Serial.available())
  {
    char x = Serial.read();
    if (x != '\n') 
    {
      data_rcv += x;
    }
    else if (x == '\n')
    {
      Serial.println(data_rcv);
      data_rasp1 = data_rcv[0];
      data_rasp2 = data_rcv[1];
//      Serial.print("Char 1: ");
//      Serial.println(data_rasp1); 
//      Serial.print("Char 2: ");
//      Serial.println(data_rasp2); 
      rcv_flag = true;
    }
  }
}

/**
 * @brief   Đọc dữ liệu từ STM32 qua Serial2
 */
void read_data_from_STM(void)
{
  while (Serial2.available())
  {
      char x = Serial2.read();
      if (x != '\n') 
      {
        rxbuff += x;
      }
      else if (x == '\n')
      {
//        Serial.println("String received: " + String(rxbuff));
//        Serial.printf("Length: %d\n", rxbuff.length()); 
        data_send = rxbuff;
        rxbuff = "";
      }
    }
  delay(10);
}

/**
 * @brief   Xử lý gửi dữ liệu đã nhận qua web server
 * @param   request Con trỏ tới đối tượng request
 */
void handleSendData(AsyncWebServerRequest *request)
{
   Serial.println("String send: " + String(data_send));
   Serial.printf("Length: %d\n", data_send.length()); 
   request->send(200, "text/plain", data_send);
   data_send = "";
}
