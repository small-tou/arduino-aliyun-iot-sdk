# Arduino TopLevel Client for aliyun IoT Platform

 `AliyunIoTSDK` 可以帮助你快速连接阿里云 IoT 平台，通过和阿里云物联网开发平台配合，可快速实现各种硬件应用，包括了很上层的封装，无需自己解析数据体，绑定事件即可，在 esp8266 平台充分测试（NodeMCU 1.0）

## update

- v0.2 增加属性发送 buffer，5秒一次或者10条buffer满，才会一起发送数据，节省请求次数
- v0.1 上线

## Usage 使用示例

```c++
// 引入 wifi 模块，并实例化，不同的芯片这里的依赖可能不同
#include <ESP8266WiFi.h>
static WiFiClient espClient;

// 引入阿里云 IoT SDK
#include <AliyunIoTSDK.h>

// 设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY "xxx"
#define DEVICE_NAME "Device_D"
#define DEVICE_SECRET "xxxxxxxxxxxxxx"
#define REGION_ID "cn-shanghai"

// 设置 wifi 信息
#define WIFI_SSID "xxxxx"
#define WIFI_PASSWD "xxxxx"

void setup()
{
    Serial.begin(115200);
    
    // 初始化 wifi
    wifiInit(WIFI_SSID, WIFI_PASSWD);
    
    // 初始化 iot，需传入 wifi 的 client，和设备产品信息
    AliyunIoTSDK::begin(espClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);
    
    // 绑定一个设备属性回调，当远程修改此属性，会触发 powerCallback
    // PowerSwitch 是在设备产品中定义的物联网模型的 id
    AliyunIoTSDK::bindData("PowerSwitch", powerCallback);
    
    // 发送一个数据到云平台，LightLuminance 是在设备产品中定义的物联网模型的 id
    AliyunIoTSDK::send("LightLuminance", 100);
}

void loop()
{
    AliyunIoTSDK::loop();
}

// 初始化 wifi 连接
void wifiInit(const char *ssid, const char *passphrase)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, passphrase);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("WiFi not Connect");
    }
    Serial.println("Connected to AP");
}

// 电源属性修改的回调函数
void powerCallback(JsonVariant p)
{
    int PowerSwitch = p["PowerSwitch"];
    if (PowerSwitch == 1)
    {
        // 启动设备
    } 
}
```

## API 可用方法

```c++
// 在主程序 loop 中调用，检查连接和定时发送信息
  static void loop();

  /**
   * 初始化程序
   * @param ssid wifi名
   * @param passphrase wifi密码
   */
  static void begin(Client &espClient,
                    const char *_productKey,
                    const char *_deviceName,
                    const char *_deviceSecret,
                    const char *_region);

  /**
   * 发送数据
   * @param param 字符串形式的json 数据，例如 {"${key}":"${value}"}
   */
  static void send(const char *param);
  /**
   * 发送 float 格式数据
   * @param key 数据的 key
   * @param number 数据的值
   */
  static void send(char *key, float number);
  /**
   * 发送 int 格式数据
   * @param key 数据的 key
   * @param number 数据的值
   */
  static void send(char *key, int number);
  /**
   * 发送 double 格式数据
   * @param key 数据的 key
   * @param number 数据的值
   */
  static void send(char *key, double number);
  /**
   * 发送 string 格式数据
   * @param key 数据的 key
   * @param text 数据的值
   */
  static void send(char *key, char *text);

  /**
   * 发送事件到云平台（附带数据）
   * @param eventId 事件名，在阿里云物模型中定义好的
   * @param param 字符串形式的json 数据，例如 {"${key}":"${value}"}
   */
  static void sendEvent(const char *eventId, const char *param);
  /**
   * 发送事件到云平台（空数据）
   * @param eventId 事件名，在阿里云物模型中定义好的
   */
  static void sendEvent(const char *eventId);

  /**
   * 绑定回调，所有云服务下发的数据都会进回调
   */
  // static void bind(MQTT_CALLBACK_SIGNATURE);

  /**
   * 绑定事件回调，云服务下发的特定事件会进入回调
   * @param eventId 事件名
   */
  // static void bindEvent(const char * eventId, MQTT_CALLBACK_SIGNATURE);
  /**
   * 绑定属性回调，云服务下发的数据包含此 key 会进入回调，用于监听特定数据的下发
   * @param key 物模型的key
   */
  static int bindData(char *key, poniter_fun fp);
  /**
   * 卸载某个 key 的所有回调（慎用）
   * @param key 物模型的key
   */
  static int unbindData(char *key);
```

## Examples 示例

buiding...

## Limitations 使用限制和说明

 - 本库不包含 wifi 连接的代码，需先建立连接，然后将 client 传入
 - 依赖 PubSubClient ，在使用前，请务必修改 PubSubClient 的连接参数，否则无法使用
 - PubSubClient 中的 MQTT_MAX_PACKET_SIZE 修改为 1024
 - PubSubClient 中的 MQTT_KEEPALIVE 修改为 60
 - 掉线后会一直尝试重新连接，可能会触发阿里云的一些限流规则（已经做了规避），并且会导致挤掉其他同设备 ID 的设备
 - 默认 5000ms 检测一次连接状态，可以通过 CHECK_INTERVAL 修改此值


## Compatible Hardware 适用硬件

本 SDK 基于 PubSubClient 底层库开发，兼容列表与 PubSubClient 相同。

The library uses the Arduino Ethernet Client api for interacting with the underlying network hardware. This means it Just Works with a growing number of boards and shields, including:

 - Arduino Ethernet
- Arduino Ethernet Shield
- Arduino YUN – use the included YunClient in place of EthernetClient, and be sure to do a Bridge.begin() first
- Arduino WiFi Shield - if you want to send packets > 90 bytes with this shield, enable the MQTT_MAX_TRANSFER_SIZE define in PubSubClient.h.
- Sparkfun WiFly Shield – library
- TI CC3000 WiFi - library
- Intel Galileo/Edison
- ESP8266
- ESP32

The library cannot currently be used with hardware based on the ENC28J60 chip – such as the Nanode or the Nuelectronics Ethernet Shield. For those, there is an alternative library available.

## License

This code is released under the MIT License.
