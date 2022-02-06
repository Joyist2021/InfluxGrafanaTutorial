//InfluxDB和Grafana
// ESP32/ESP8266 
// DHT11传感器连接到引脚D4-SIG，用于测量温度和湿度；然后BMP280传感器连接到I2C接口D21-SDA/D22-SCL以测量大气压力。
// http://av98.byethost10.com/?thread-1332.htm
// https://github.com/mklements/InfluxGrafanaTutorial
//2022 年 2 月 4 日
//The DIY Life by Michael Klements
//21 January 2022


#include <Wire.h>	//导入所需的库
#include "DHT.h"
#include "Seeed_BMP280.h"

#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>


#define WIFI_SSID "GECOOSAP-CA2F"	//网络名字
#define WIFI_PASSWORD "88888888" 	//网络密码
#define INFLUXDB_URL "https://us-west-2-2.aws.cloud2.influxdata.com"	//InfluxDB v2 服务器 url，例如 https://eu-central-1-1.aws.cloud2.influxdata.com（使用：InfluxDB UI -> 加载数据 -> 客户端库）
#define INFLUXDB_TOKEN "XE_1OUn9PztoP_rp4dYKzWgrFiM7-XI4CKUqZxApvGsiCpwpLBB2aZfMGLyfBg7-9jaCjtSUDbkqVxEYqsTqUw==" 	//InfluxDB v2 服务器或云API token令牌（使用：InfluxDB UI -> 数据 -> API 令牌 -> <select token>）
#define INFLUXDB_ORG "02292e09b9029a72"	//InfluxDB v2 组织 ID（使用：InfluxDB UI -> User -> About -> Common Ids ）
#define INFLUXDB_BUCKET "ee17462eba64b797>" 	//InfluxDB v2 存储桶名称（使用：InfluxDB UI -> 数据 -> 存储桶）

// 根据 https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html 设置时区字符串
// 例子：
//  太平洋时间 Pacific Time: "PST8PDT" 
//  东部 Eastern: "EST5EDT"
//  日本Japanesse: "JST-9"
//  中欧Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "Asia/Shanghai" 	//InfluxDB v2 时区

DHT dht(4,DHT11);  	//DHT 和 BMP 传感器参数
BMP280 bmp280;

int temp = 0; 	//存储传感器读数的变量
int humid = 0;
int pressure = 0;

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert); 	//带有预配置 InfluxCloud 证书的 InfluxDB 客户端实例

Point sensor("weather"); //数据点

void setup() 
{
  Serial.begin(115200);  //启动串行通信
  
  dht.begin();           //连接到 DHT 传感器
  if(!bmp280.init())     //连接压力传感器
    Serial.println("bmp280 init error!");

  WiFi.mode(WIFI_STA);   //设置wifi连接
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi"); 	//连接到 WiFi
  while (wifiMulti.run() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  sensor.addTag("device", DEVICE);   //添加标签 - 根据需要重复
  sensor.addTag("SSID", WIFI_SSID);
  // 证书校验和批量写入需要准确的时间
  // 为了最快的时间同步找到您所在地区的 NTP 服务器：https://www.pool.ntp.org/zone/
  // 同步进度和时间将打印到 Serial。
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov"); //证书验证和批量写入需要准确的时间

  if (client.validateConnection())   //检查服务器连接
  {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } 
  else 
  {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop()	//循环功能
{
  temp = dht.readTemperature(); 	//记录温度
  humid = dht.readHumidity();	//记录温度
  pressure = bmp280.getPressure()/100;	//记录压力

  sensor.clearFields();   //清除字段以重用该点。 标签将保持不变

  sensor.addField("temperature", temp); 	// 将测量值存储到点
  sensor.addField("humidity", humid);	// 将测量值存储到点
  sensor.addField("pressure", pressure);	// 将测量值存储到点

    
  if (wifiMulti.run() != WL_CONNECTED)	//检查 WiFi 连接并在需要时重新连接
    Serial.println("Wifi connection lost");

  if (!client.writePoint(sensor))    //写入数据点
  {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  
  Serial.print("Temp: ");	//在串行监视器上显示读数
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(humid);
  Serial.print("Pressure: ");
  Serial.println(pressure);
  delay(60000);	//等待 60 秒
}
