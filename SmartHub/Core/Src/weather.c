#include "weather.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// static void toLowercase(char* str);

static unsigned short strToNum(const char * str);

my_date_t mdt;

void parse_lvgl_data(my_date_t *date_struct)
{

}


static unsigned short strToNum(const char * str)
{

    unsigned short i = 0;
    unsigned short num = 0;
    while (str[i] != '\0')
    {
        if(str[i] >= '0' && str[i] <= '9')
        {
            num = num * 10 + (str[i] - '0');
        }
        i++;
    }
    return num;
}

// 通用字符串字段解析函数
void parse_field(const char* str, const char* key, char* output, unsigned short max_len) {
    const char* ptr = strstr(str, key);
    if (ptr) {
        ptr += strlen(key);
        const char* end = strchr(ptr, '\"');
        if (end) {
            size_t len = end - ptr;
            if (len < max_len) {
                strncpy(output, ptr, len);
                output[len] = '\0';
            }
        }
    }
}

/**
 * @brief 解析当前的天气
 * 
 * @param json_str 要解析的json格式的字符串
 * @return now_weather_t 
 */
now_weather_t parse_now_weather_data(const char *json_str)
{
    now_weather_t nwt = {
        .city_name = "null",
        .weather_code = "null",
        .date = "null",
        .temp = "null",
        .weather = "null"
    };

    // 一次性找到now对象开始位置
    const char* now_start = strstr(json_str, "\"now\":{");

    //解析城市名
    parse_field(json_str, "\"name\":\"", nwt.city_name, sizeof(nwt.city_name));

    //解析天气
    parse_field(now_start, "\"text\":\"", nwt.weather, sizeof(nwt.weather));

    //解析温度
    parse_field(now_start, "\"temperature\":\"", nwt.temp, sizeof(nwt.temp));

    //解析天气对应的代码
    parse_field(now_start, "\"code\":\"", nwt.weather_code, sizeof(nwt.weather_code));

    //解析日期
    const char *last_update_start = strstr(json_str, "\"last_update\":\"") + strlen("\"last_update\":\"");
    int len = strlen("yyyy-mm-dd");
    strncpy(nwt.date, last_update_start, len);
    nwt.date[len] = '\0';

    return nwt;
}

/**
 * @brief  解析未来3天的天气
 * @param  json_str: 要解析的json格式的字符串
 * @retval 无
 */
future_weather_t parse_future_weather_data(const char *json_str) {
    /* 初始化结构体 */
    future_weather_t wt = {
        .city_name = "null",
        .date = "null",
        .low_temp = "null",
        .high_temp = "null",
        .weather_day = "null",
        .weather_night = "null"
    };

    // 一次性找到daily数组开始位置
    const char* daily_start = strstr(json_str, "\"daily\":[");
    if (!daily_start) return wt;

    //解析城市名
    parse_field(json_str, "\"name\":\"", wt.city_name, sizeof(wt.city_name));
    
    //解析日期
    parse_field(daily_start, "\"date\":\"", wt.date, sizeof(wt.date));

    //解析白天天气
    parse_field(daily_start, "\"text_day\":\"", wt.weather_day, sizeof(wt.weather_day));

    //解析夜晚天气
    parse_field(daily_start, "\"text_night\":\"", wt.weather_night, sizeof(wt.weather_night));

    //解析最高温度
    parse_field(daily_start, "\"high\":\"", wt.high_temp, sizeof(wt.high_temp));

    //解析最低温度
    parse_field(daily_start, "\"low\":\"", wt.low_temp, sizeof(wt.low_temp));

    return wt;
}

/**
 * @brief 解析日期
 * 
 * @param date_str 包含日期的字符串
 * @return unsigned char 1：成功，0：失败
 */
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
        // 4. 提取星期（第0个字段："Wed"）
        strncpy(dt->week, fields[0], 3);
        dt->week[3] = '\0';

        // 5. 提取月份（第1个字段："Oct"）
        strncpy(dt->month, fields[1], 3);
        dt->month[3] = '\0';
        
        //在-o2优化等级下使用atoi函数会出问题，疑似被优化?，重写了一个函数来实现str --> uint8_t
        dt->day = strToNum(fields[2]);
        dt->hour = strToNum(fields[3]);
        dt->minute = strToNum(fields[4]);
        dt->second = strToNum(fields[5]);
        dt->year = strToNum(fields[6]);

        return 1;
    }
    return 0;
    
}

unsigned char intToBCD(int num) {
    if (num < 0 || num > 99) {
        // printf("错误数字必须在0-99范围内\n");
        return 0;
    }
    
    unsigned char tens = num / 10;      // 十位数：2
    unsigned char units = num % 10;     // 个位数：5
    unsigned char bcd = (tens << 4) | units;  // 组合成BCD码
    
    return bcd;
}

// // 将字符串转换为小写
// static void toLowercase(char* str) {
//     for (int i = 0; str[i]; i++) {
//         str[i] = tolower(str[i]);
//     }
// }

/**
 * @brief Get the Month Number
 * 
 * @param monthStr 月份缩写字符串，如"Oct"
 * @return unsigned char 
 */
unsigned char getMonthNumber(const char* monthStr) {
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
    for (int i = 0; i < 12; i++) {
        if (strcmp(monthStr, months[i]) == 0) {
            return intToBCD(i+1);   //返回月份的BCD码，例如返回0x12表示12月份
        }
    }
    return 1;   //匹配失败则默认设置为一月
}

unsigned char getWeekNumber(const char* weekStr) {
    const char* weekdays[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    
    for (int i = 0; i < 7; i++) {
        if (strcmp(weekStr, weekdays[i]) == 0) {
            return i + 1;   //返回星期几对应的值，1：表示星期一，7表示星期天
        }
    }
    return 1;   //匹配失败默认设置为星期一
}
