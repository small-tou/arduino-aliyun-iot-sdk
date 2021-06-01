#include <ESP8266WiFi.h>

static WiFiClient espClient;
#include <ArduinoJson.h>

#include <AliyunIoTSDK.h>
AliyunIoTSDK iot;

#define PRODUCT_KEY "xxxxx"
#define DEVICE_NAME "Device_D"
#define DEVICE_SECRET "xxxxxxxxxxx"
#define REGION_ID "cn-shanghai"

#define WIFI_SSID "xxxxxx"
#define WIFI_PASSWD "xxxxxxxx"


void wifiInit(const char *ssid, const char *passphrase)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, passphrase);
    WiFi.setAutoConnect (true);
    WiFi.setAutoReconnect (true);
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

    wifiInit(WIFI_SSID, WIFI_PASSWD);

    AliyunIoTSDK::begin(espClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

    // 绑定属性回调
    AliyunIoTSDK::bindData("PowerSwitch", powerCallback);

    // 操作用户自定义Topic
    AliyunIoTSDK::subscribeUser("/get", callback);

/*
    下面为操作其他各类topic方法，注意替换自己的参数
    AliyunIoTSDK::subscribe("/sys/${ProductKey}/${deviceName}/thing/service/${tsl.service.identifier}", callback);
    AliyunIoTSDK::publish("/sys/${ProductKey}/${deviceName}/thing/service/${tsl.service.identifier}_reply", callback);
*/
}

unsigned long lastMsMain = 0;
void loop()
{
    AliyunIoTSDK::loop();
    if (millis() - lastMsMain >= 5000)
    {
        lastMsMain = millis();
        // 发送事件到阿里云平台
        AliyunIoTSDK::sendEvent("xxx"); 
        // 发送模型属性到阿里云平台
        AliyunIoTSDK::send("CurrentTemperature", 30);
    }
}

void callback(JsonVariant p)
{
    Serial.println("custom topic callback");
    serializeJsonPretty(p, Serial);
    Serial.println();
}

void powerCallback(JsonVariant p)
{
    int PowerSwitch = p["PowerSwitch"];
    if (PowerSwitch == 1)
    {
        //
    }
    else
    {
        //
    }
}
