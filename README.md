# DS1307 library for ESP-IDF

DS1307 library for ESP32 in the ESP-IDF. In this library the DS1307 module works at 24 hours format.


## How to use

If you are using PlatformIO like me, you can just clone this project in the **lib** folder. 

```
cd lib/ && git clone https://github.com/AiltonFidelix/DS1307
```

Otherwise, just copy this project and use however you want.

## Example

```
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

// Include the library
#include <ds1307.h>

void app_main()
{
    printf("Starting ds1307!\n");

    // bool true parameter to install the I2C driver
    ds1307Begin(true);

    // Config date and time
    ds1307SetDate(11, 9, 22, 1);
    ds1307SetTime(16, 32, 0);

    while (1)
    {
        printf("Hour: %d\n", ds1307GetHour());
        printf("Minute: %d\n", ds1307GetMinute());
        printf("Second: %d\n", ds1307GetSecond());
        printf("Day: %d\n", ds1307GetDateDay());
        printf("Month: %d\n", ds1307GetMonth());
        printf("Year: %d\n", ds1307GetYear());
        printf("Week day: %d\n", ds1307GetWeekDay());

        char time[10];
        ds1307GetTime(time);
        printf("Time: %s\n", time);

        char date[10];
        ds1307GetDate(date);
        printf("Date: %s\n", date);

        char timestamp[24];
        ds1307GetTimestamp(timestamp);
        printf("Timestamp: %s\n", timestamp);
    
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
```

### Author

Created by Ailton Fidelix

[![Linkedin Badge](https://img.shields.io/badge/-Ailton-blue?style=flat-square&logo=Linkedin&logoColor=white&link=https://www.linkedin.com/in/ailtonfidelix/)](https://www.linkedin.com/in/ailton-fidelix-9603b31b7/) 
