#ifndef ALIYUN_IOT_SDK_H
#define ALIYUN_IOT_SDK_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include "Client.h"

typedef void (*poniter_fun)(JsonVariant ele); //定义一个函数指针

typedef struct poniter_desc
{
  char *key;
  poniter_fun fp;
} poniter_desc, *p_poniter_desc;

// 最多绑定20个回调
static poniter_desc poniter_array[20];
static p_poniter_desc p_poniter_array;

class AliyunIoTSDK
{
private:
  // mqtt 链接信息，动态生成的
  static char mqttPwd[256];
  static char clientId[256];
  static char mqttUsername[100];
  static char domain[150];
 
  // 定时检查 mqtt 链接
  static void mqttCheckConnect();

  static void messageBufferCheck();
  static void sendBuffer();
public:
  
  // 标记一些 topic 模板
  static char ALINK_TOPIC_PROP_POST[150];
  static char ALINK_TOPIC_PROP_SET[150];
  static char ALINK_TOPIC_EVENT[150];
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
};
#endif
