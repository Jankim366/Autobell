# Autobell
基于野火STM32F103霸道_V1的自动感应门铃

需拔掉B10和URX、B11和UTX之间的跳线帽，它们使B10、B11连接到了板载WIFI模块，会影响同样使用这两个引脚的通信模块正常工作。

连接方式：
VCC连接开发板的5V供电，GND连接开发板的GND。
RXD 连接 PB10
TXD 连接 PB11
