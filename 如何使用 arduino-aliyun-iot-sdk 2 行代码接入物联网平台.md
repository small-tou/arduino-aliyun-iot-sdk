> 文中提到的 AliyunIoTSDK 这个 arduino 库，可以在 arduino 库商店里搜索到（搜索 AliyunIoTSDK），但是版本可能不一定是最新的，也可以手动把 github 上的项目 clone 下来，放到 arduino 的 library 库下，保证功能是最新的。

> github 地址：[https://github.com/xinyu198736/arduino-aliyun-iot-sdk](https://github.com/xinyu198736/arduino-aliyun-iot-sdk)

> 有问题，欢迎提 issues 或 PR 共建，我对 c++ 不是特别熟悉，写出来的代码可能不一定是最合理的。

## 阿里云物联网平台简介

阿里云物联网平台提供接入物联网场景的一整套基础设施，强大的在线配置能力，基本可以实现无代码开发接入各类应用，实现大规模的物联网应用场景。

核心能力模块包括：

- 产品&设备创建和管理
- 服务可视化开发
- 控制界面可视化开发
- 数据分析和监控运维等辅助设施

基于以上功能模块，可以快速在阿里云定义设备模型和事件等信息，然后快速创建和接入设备，再利用可视化的开发平台，可以零代码实现非常强大的功能。

不过，今天我们要介绍一个比较特殊的接入场景，使用嵌入式系统快速接入阿里云物联网平台，这里以 esp8266 为例。

> esp8266 是一款集成网络模块的嵌入式芯片，自带 32 位处理器，各种外设接口，wifi 模块等，可以使用 arduino 平台进行编程，兼容大量 arduino 库。


本文就是以 arduino 编程方式，使用的 esp8266 开发板为 NodeMCU 1.0 ，淘宝成本仅为 8~10 元。

<a name="c931653c"></a>
## 场景

本次我们想利用 ESP8266 芯片实现一个简单的可远程控制的空调控制器，具备以下功能：

- 批量部署，可通过云平台集中管理和远程控制空调开关、温度、模式等
- 芯片可感知环境温度、亮度，并上报物联网平台
- 芯片可远程触发事件，上报物联网平台
- 成本尽量低，不超过 30 元

首先，我们进行硬件选型，经过功能评估，选用以下硬件：

- nodemcu 核心板（基于 esp8266） 10.89元/个
- DS18B20模块 温度传感 3.18元/个
- GY-30  光照传感 4.38元/个
- 红外发射板 3.5元/个
- 杜邦线母对母 1.45元/串
- 电源插头  1.4元/个
- 数据线 1.8元/个
- 按钮开关 0.75元/个

<a name="dfe89ca1"></a>
## 开始接入

具体的连线，我们这里不展开，我们主要探讨如何使用 arduino 快速接入物联网平台。

第一步，在物联网创建产品，建立物模型，并创建设备，获取设备配置信息。

![image.png](https://blog.souche.com/content/images/2019/09/1568620395927-b7090d59-5958-4050-a0e7-7205faafb455.png)
创建产品，并定义物模型

![image.png](https://blog.souche.com/content/images/2019/09/1568620350930-270bbfa3-22b8-4711-b6b8-ff6f38a8d49a.png)
创建设备，与产品绑定，获取设备鉴权信息<br />
<br />第二步，用 arduino 连接 nodemcu 开发板，开始硬件端的开发。<br />
![IMG_2546.JPG](https://blog.souche.com/content/images/2019/09/1568642438839-a595c156-3afb-4ef8-8213-d67433be474a.jpeg)
具体接线可以自己研究，这里只是个展示，哈哈<br />

<a name="qMAHi"></a>
## 硬件端开发
这里不讨论如何使用 arduino 进行 esp8266 芯片开发，直接进入代码环节，相关基础知识可以百度一下。

首先，亮相一个我自己封装的 aliyun iot 上层 SDK（ [arduino-aliyun-iot-sdk](https://github.com/xinyu198736/arduino-aliyun-iot-sdk) ），底层连接基于 PubSubClient 库，并且对模型操作做了一些上层封装，可以方便的发送数据和订阅远程指令。
```cpp
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

核心代码只有三句：

```cpp
// 初始化 iot，需传入 wifi 的 client，和设备产品信息
AliyunIoTSDK::begin(espClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

// 绑定一个设备属性回调，当远程修改此属性，会触发 powerCallback
// PowerSwitch 是在设备产品中定义的物联网模型的 id
AliyunIoTSDK::bindData("PowerSwitch", powerCallback);

// 发送一个数据到云平台，LightLuminance 是在设备产品中定义的物联网模型的 id
AliyunIoTSDK::send("LightLuminance", 100);
```

注意这里的数据绑定和数据发送，是非常有用和方便的，不需要自己去写做匹配的代码，也不需要自己组装数据，并且会自动合并数据，节约上报的频率。

将设备配置信息配置进代码，烧录到板子，运行即可，是不是炒鸡方便呢。

注意事项：

- 阿里云对客户端的链接参数有一些限制，需要修改  PubSubClient 的连接参数，否则无法使用
  - PubSubClient 中的 MQTT_MAX_PACKET_SIZE 修改为 1024
  - PubSubClient 中的 MQTT_KEEPALIVE 修改为 60
- 默认 5000ms 检测一次连接状态，可以通过 AliyunIoTSDK.cpp 中的 CHECK_INTERVAL 修改此值
- 掉线后会一直尝试重新连接，可能会触发阿里云的一些限流规则（已经做了规避），并且会导致挤掉其他同设备 ID 的设备
<a name="ZzbqJ"></a>
### 资源
目前 AliyunIoTSDK 这个 arduino 库，可以在 arduino 库商店里搜索到（搜索 AliyunIoTSDK），但是版本可能不一定是最新的，可以手动把 github 上的项目 clone 下来，放到 arduino 的 library 库下，保证功能是最新的。

github 地址：[https://github.com/xinyu198736/arduino-aliyun-iot-sdk](https://github.com/xinyu198736/arduino-aliyun-iot-sdk)

有问题，欢迎提 issues 或 PR 共建，我对 c++ 不是特别熟悉，写出来的代码可能不一定是最合理的。

<a name="gycpL"></a>
## 可视化开发界面和服务
完成了设备的连接之后，已经迈出了第一步，如果你想要自己开发一个界面，可视化的控制空调，或者想利用一些远程设备的数据触发一些自定义的规则和逻辑（例如如果某个数据超标，报警到钉钉群等），用阿里云的物联网平台的话，这些都不需要自己去开发，直接使用物联网平台的“开发服务”拖拖拽拽即可完成。

<a name="7z0mV"></a>
### 界面开发
界面开发的核心是 拖动布局、数据源绑定、交互绑定、自定义变量等。
<br />
![image.png](https://blog.souche.com/content/images/2019/09/1568630094545-d440e173-23f0-4d63-81e7-00d6b6ce8811.png)
布局界面，右侧可以设置各种控件属性
<br />
![image.png](https://blog.souche.com/content/images/2019/09/1568630244900-18ee8b99-1e80-409a-8482-981fd596d095.png)
数据源绑定，可以方便的和产品模型数据绑定
<br />
![image.png](https://blog.souche.com/content/images/2019/09/1568630294665-d5cf7a85-ad06-491c-a7bf-65f6439450f8.png)
交互绑定，可以与服务进行绑定，触发某个服务，并传入设备属性<br />
<br />
![image.png](https://blog.souche.com/content/images/2019/09/1568642695402-cc25fedc-8c3f-4e32-90a9-89c0eec2f712.png)
控件可以给自定义变量赋值，其他组件可以与变量绑定

使用上述的功能，加上各种设备绑定功能，基本可以完成一个很复杂的界面的开发，但是如果你需要通过交互触发远程指令，或者监听远程的数据做一些判断逻辑，光有界面是不够的。

<a name="D26H8"></a>
### 服务开发
上面提到的逻辑开发，就需要使用“服务开发”了，服务开发的触发点主要是两类：设备触发、界面交互触发。

![image.png](https://blog.souche.com/content/images/2019/09/1568643025480-f10c038d-8a9b-44a1-b270-51625b56a137.png)
设备触发的服务，中间的路径选择是用来做条件判断分流的，最右侧是触发通知逻辑节点<br />
<br />
![image.png](https://blog.souche.com/content/images/2019/09/1568643229960-62754bc2-4ddf-4724-ba99-ed81459f9bc2.png)
一个温度控制服务，通过界面控件触发，最终生成设备控制逻辑<br />

<a name="UnwgK"></a>
## 结语
本文不是 arduino 教学，也不是 esp8266 教学，更不是硬件教学，所以没有展开细节，需要读者具备一定的基础，请各位谅解，如果对 arduino 开发感兴趣，可以加我微信交流（微信号：mier963，注明 arduino）。

最后展示一下成果：<br />
![IMG_7254.JPG](https://blog.souche.com/content/images/2019/09/1568643588452-48f528a4-29e2-41f9-99bb-0a004600bd8a.jpeg)
一个成品，包含四个传感器模块，包括一个硬件按钮<br />
<br />
![image.png](https://blog.souche.com/content/images/2019/09/1568643652167-2f7d14b9-77d4-4a36-8172-ad7e9b9bc2ae.png)
空调远程控制界面<br />
<br />另外，还可以给这个控制器加入一些很有意思的功能，例如定点关闭所有设备，根据环境亮度开关空调等。

