# Arduino Toplevel Client for aliyun MQTT

> `AliyunIoTSDK` 库可以帮助你快速连接阿里云 IoT 平台，通过和阿里云物联网开发平台配合，可快速实现各种硬件应用，包括了很上层的封装，无需自己解析数据体，绑定事件即可，在 esp8266 平台充分测试（NodeMCU 1.0）

## Usage

```c++
#include <ESP8266WiFi.h>
static WiFiClient espClient;

#include <AliyunIoTSDK.h>

#define PRODUCT_KEY "xxx"
#define DEVICE_NAME "Device_D"
#define DEVICE_SECRET "xxxxxxxxxxxxxx"
#define REGION_ID "cn-shanghai"

#define WIFI_SSID "xxxxx"
#define WIFI_PASSWD "xxxxx"

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

## Examples

buiding...

## Limitations

 - 本库不包含 wifi 连接的代码
 - 依赖 PubSubClient ，在使用前，请务必修改 PubSubClient 的连接参数，否则无法使用。
 - PubSubClient 中的 MQTT_MAX_PACKET_SIZE 修改为 1024
 - PubSubClient 中的 MQTT_KEEPALIVE 修改为 60
 - 掉线后会一直尝试重新连接，可能会触发阿里云的一些限流规则（已经做了规避），并且会导致挤掉其他同设备 ID 的设备。
 - 默认 5000ms 检测一次连接状态，可以通过 CHECK_INTERVAL 修改此值


## Compatible Hardware

 - 在 NodeMCU 设备中测试过，其他类型的设备暂无测试，有问题请提 issues

The library cannot currently be used with hardware based on the ENC28J60 chip –
such as the Nanode or the Nuelectronics Ethernet Shield. For those, there is an
[alternative library](https://github.com/njh/NanodeMQTT) available.

## License

This code is released under the MIT License.
