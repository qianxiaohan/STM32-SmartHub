# STM32-SmartHub

## 实现功能

1. 使用ESP8266-01S获取天气，将数据转发给STM32，并通过LCD显示出`最高温度`、`最低温度`、`当前温度`、`天气`
2. ESP8266-01S使用AT指令获取SNTP时间，对本地RTC校准，通过LCD显示`当前小时、分钟`，`日期`
3. 通过LCD触摸板控制LED灯
4. 一些外设模块，例如温湿度模块（待更新）
5. IAP升级（待更新）
6. OTA升级（待更新）
7. 

## 介绍

- 开发环境：VSCode + EIDE插件，STM32CUBEMX生成代码

- 主控芯片：STM32F407VET6
- 外设：
  - ESP8266-01S WIFI模块
  - TFT LCD 2.8寸
  - USB TO TTL（用于调试）
  - 待拓展....
- 操作系统：FreeRTOS
- 图形界面框架：使用Gui Guider设计界面，生成LVGL v8.3代码

连线如下表所示：

| STM32接线 | ESP8266 | USB TO TTL |
| --------- | ------- | ---------- |
| PA9       | RX      |            |
| PA10      | TX      |            |
| PB10      |         | RX         |
| PB11      |         | TX         |

界面设计如下：

**主界面**

![SmartHub-1.png](./img/SmartHub-1.png)

**子界面**

![SmartHub-2.png](./img/SmartHub-2.png)

## 天气解析

考虑到ESP8266返回的JSON数据过大，如果使用串口会丢失数据，因此使用**DMA2来接收ESP8266发来的数据**，并通过**DMA1将解析后的数据发送**至串口助手中查看结果。

通过心知天气API获取到未来天气的JSON数据：

```json
{
  "results": [
    {
      "location": {
        "id": "WM6N2PM3WY2K",
        "name": "Chengdu",
        "country": "CN",
        "path": "Chengdu,Chengdu,Sichuan,China",
        "timezone": "Asia/Shanghai",
        "timezone_offset": "+08:00"
      },
      "daily": [
        {
          "date": "2025-10-21",
          "text_day": "Overcast",
          "code_day": "9",
          "text_night": "Overcast",
          "code_night": "9",
          "high": "17",
          "low": "12",
          "rainfall": "0.00",
          "precip": "0.00",
          "wind_direction": "NE",
          "wind_direction_degree": "45",
          "wind_speed": "8.4",
          "wind_scale": "2",
          "humidity": "88"
        }
      ],
      "last_update": "2025-10-21T08:00:00+08:00"
    }
  ]
}
```

可以cJSON来解析json数据，但是需要修改STM32启动文件的堆栈大小，本例程中使用字符串匹配的方式来进行了简单的解析，分别对应：地区，日期，白天天气，晚上天气，最高温度，最低温度

```shell
Chengdu, 2025-10-21, Overcast, Overcast, 17, 12
```

同样地，获取实况天气的json数据：

```json
{
  "results": [
    {
      "location": {
        "id": "WM6N2PM3WY2K",
        "name": "Chengdu",
        "country": "CN",
        "path": "Chengdu,Chengdu,Sichuan,China",
        "timezone": "Asia/Shanghai",
        "timezone_offset": "+08:00"
      },
      "now": {
        "text": "Light rain",
        "code": "13",
        "temperature": "15"
      },
      "last_update": "2025-11-03T12:47:42+08:00"
    }
  ]
}
```

解析后如下，分别对应：地区，日期，当前温度，天气，天气代码

```shell
Chengdu, 2025-11-03, 15, Light rain, 13
```

## 日期解析

使用AT指令`AT+CIPSNTPTIME?`获取SNTP时间，返回：

```
+CIPSNTPTIME:Tue Oct 19 17:47:56 2021
OK
```

ESP8266通过串口发送给STM32实际上接收到的是, `"\r\n"`是回车+换行符

```
+CIPSNTPTIME:Tue Oct 19 17:47:56 2021\r\n
OK\r\n
```

HAL库生成的Keil工程，使用的是-o2优化等级，经过测试使用`sscanf`、`atoi`库函数，在普通的C程序中可以解析出上面的时间。但是在STM32中使用有些程序不会执行，可能是优化等级的问题？使用以下代码解析时间，strToNum函数是将字符串的数转为无符号数："1234" --> 1234。

```c
unsigned char parse_date(const char *date_str, date_t *dt)
{

    char *pos = strstr(date_str, "TIME:") + strlen("TIME:");
    char *end = strstr(date_str, "OK");

    volatile unsigned short len = end - pos - strlen("\r\n");
    char buffer[64];
    strncpy(buffer, pos, len);
    buffer[len] = '\0';

    char* fields[7];  // 存储6个字段的指针
    unsigned char fieldCount = 0;
    char* token = strtok(buffer, " ");  // 用空格分割

    while (token != NULL && fieldCount < 7) {
        fields[fieldCount++] = token;
        token = strtok(NULL, " :");
    }
    if (fieldCount == 7) 
    {
        // 提取星期（第0个字段："Wed"）
        strncpy(dt->week, fields[0], 3);
        dt->week[3] = '\0';

        // 提取月份（第1个字段："Oct"）
        strncpy(dt->month, fields[1], 3);
        dt->month[3] = '\0';
        
        //在-o2优化等级下使用atoi函数会出问题，疑似被优化?，重写了一个函数来实现str --> uint8_t
        //提取日期和时间
        dt->day = strToNum(fields[2]);
        dt->hour = strToNum(fields[3]);
        dt->minute = strToNum(fields[4]);
        dt->second = strToNum(fields[5]);
        dt->year = strToNum(fields[6]);

        return 1;
    }
    return 0;
}
```

## 实现效果

在串口助手中输出调试内容：

![test.gif](./img/test.gif)

## 参考链接

1. AT指令集：https://docs.espressif.com/projects/esp-at/zh_CN/latest/esp32/AT_Command_Set/index.html

