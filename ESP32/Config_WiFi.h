/**********************************************************************************************
 * @file Config_WiFi.h
 * @brief   Cấu hình tham số WiFi cho ESP32   
 * @details Chứa khai báo địa chỉ IP, SSID, mật khẩu cho chế độ Access Point
 *          và Station. Các tham số như số kết nối tối đa, kênh WiFi, protocol
 * @author Anh Nguyen
 * @date 2024-06-02
 * @version 1.0
 *********************************************************************************************/

#ifndef CONFIG_WIFI_H
#define CONFIG_WIFI_H

/*===== Access Point (AP) Mode Configuration =====*/
IPAddress localip(192,168,222,171);   // Địa chỉ IP tĩnh cho ESP32 khi chạy ở chế độ AP
IPAddress gateway(192,168,222,1);     // Địa chỉ Gateway cho mạng AP
IPAddress subnet(255,255,255,0);      // Subnet mask cho mạng AP
IPAddress IP;                         // Địa chỉ IP động

/* Setup SSID and Password */
const char *ssid_ap = "your_username";   // Tên mạng WiFi AP
const char *pass_ap = "your_password";   // Mật khẩu mạng WiFi AP
const char *camera_ip = "192.168.222.172";  // Địa chỉ IP của camera kết nối trong mạng

// Số lượng thiết bị tối đa có thể kết nối vào ESP32 AP
const int maxConnection = 5;

// Kênh WiFi sử dụng cho Access Point
const int WiFiChannel = 10;

// Các giao thức WiFi hỗ trợ (802.11b/g/n)
const uint8_t protocol1 = WIFI_PROTOCOL_11B;
const uint8_t protocol2 = WIFI_PROTOCOL_11G;
const uint8_t protocol3 = WIFI_PROTOCOL_11N;

// Chuỗi danh sách thiết bị đã kết nối (dùng để hiển thị MAC)
String deviceList = "";

// Số lượng thiết bị hiện tại đã kết nối
int deviceCount = 0;

/*===== Mode Station =====*/
const char *ssid_sta = "YOUR_SSID";
const char *pass_sta = "YOUR_PASS";

#endif  // CONFIG_WIFI_H
