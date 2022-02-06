# InfluxGrafana教程

从 ESP32 收集天气数据然后将此数据上传到 InfluxDB 以使用 Grafana 创建仪表板的示例代码。

完整的教程和设置说明可在我的博客上找到 - https://www.the-diy-life.com/grafana-weather-dashboard-using-influxdb-and-an-esp32-in-depth-tutorial/或我的Youtube 频道 - https://youtu.be/7M8MHa6W9w0

在将脚本上传到 ESP32/ESP8266 之前，您需要将自己的 WiFi 设置和 InfluxDB 连接详细信息添加到脚本中。


// ESP32/ESP8266 
// DHT11传感器连接到引脚D4-SIG，用于测量温度和湿度；然后BMP280传感器连接到I2C接口D21-SDA/D22-SCL以测量大气压力。
