/**
 * @file ds1307.c
 * @author Ailton Fidelix (ailton1626@gmail.com)
 * @brief DS1307 RTC library for ESP-IDF
 * @version 1.0.0
 * @date 07-09-2022
 * @copyright Copyright (c) 2022
 */

#include "ds1307.h"

// TAG used for DS1307 logi
static const char TAG[] = "ds1307";

esp_err_t ds1307Begin()
{
    ESP_LOGI(TAG, "Beginning connection");

    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = GPIO_NUM_22,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = DS1307_I2C_FREQ_HZ,
    };

    i2c_param_config(I2C_NUM_0, &i2c_config);

    esp_err_t ret = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    if (ret != ESP_OK){
        ESP_LOGE(TAG, "Failed to install I2C driver");
        return ret;
    }
    uint8_t mode = (ds1307ReadByte(DS1307_ADDR_HOURS) & 0x1F) + (DS1307_HOUR_MODE & 0xE0);

    ret = ds1307WriteByte(DS1307_ADDR_HOURS, mode);
    if (ret != ESP_OK){
        ESP_LOGE(TAG, "Failed to set 24 hours mode");
        return ret;
    }
    ret = ds1307WriteByte(DS1307_ADDR_CONTROL, 0x93);
    if (ret != ESP_OK){
        ESP_LOGE(TAG, "Failed to config control");
        return ret;
    }

    return ESP_OK;
}

esp_err_t ds1307SetDate(uint8_t day, uint8_t month, uint8_t year, uint8_t week_day)
{
    ESP_LOGI(TAG, "Setting the new date %d-%d-%d", day, month, year);

    esp_err_t ret;
    ret = ds1307WriteByte(DS1307_ADDR_DATE_DAY, dec2bcd(day));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set the day");
        return ret;
    }
    ret = ds1307WriteByte(DS1307_ADDR_MONTH, dec2bcd(month));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set the month");
        return ret;
    }
    ret = ds1307WriteByte(DS1307_ADDR_YEAR, dec2bcd(year));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set the year");
        return ret;
    }
    ret = ds1307WriteByte(DS1307_ADDR_WEEK_DAY, dec2bcd(week_day));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set the week day");
        return ret;
    }

    ESP_LOGI(TAG, "New date save success, returning ESP_OK");

    return ESP_OK;
}

esp_err_t ds1307SetTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    ESP_LOGI(TAG, "Setting the new time %d:%d:%d", hour, minute, second);

    uint8_t temp;

    esp_err_t ret;
    temp = ds1307ReadByte(DS1307_ADDR_HOURS) | hour;
    ret = ds1307WriteByte(DS1307_ADDR_HOURS, dec2bcd(temp));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set the hour");
        return ret;
    }
    ret = ds1307WriteByte(DS1307_ADDR_MINUTES, dec2bcd(minute));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set the minute");
        return ret;
    }
    temp = ds1307ReadByte(DS1307_ADDR_SECONDS) | second;
    ret = ds1307WriteByte(DS1307_ADDR_SECONDS, dec2bcd(temp));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set the second");
        return ret;
    }

    ESP_LOGI(TAG, "New time save success, returning ESP_OK");

    return ESP_OK;
}

uint8_t ds1307GetSecond()
{
    return bcd2dec(ds1307ReadByte(DS1307_ADDR_SECONDS) & 0x7F);
}

uint8_t ds1307GetMinute()
{
    return bcd2dec(ds1307ReadByte(DS1307_ADDR_MINUTES));
}

uint8_t ds1307GetHour()
{
    return bcd2dec(ds1307ReadByte(DS1307_ADDR_HOURS) & 0x3F);
}

uint8_t ds1307GetWeekDay()
{
    return bcd2dec(ds1307ReadByte(DS1307_ADDR_WEEK_DAY));
}

uint8_t ds1307GetDateDay()
{
    return bcd2dec(ds1307ReadByte(DS1307_ADDR_DATE_DAY));
}

uint8_t ds1307GetMonth()
{
    return bcd2dec(ds1307ReadByte(DS1307_ADDR_MONTH));
}

uint8_t ds1307GetYear()
{
    return bcd2dec(ds1307ReadByte(DS1307_ADDR_YEAR));
}

void ds1307GetTime(char *time)
{
    sprintf(time, "%d:%d:%d", ds1307GetHour(), ds1307GetMinute(), ds1307GetSecond());
}

void ds1307GetDate(char *date)
{
    sprintf(date, "%d-%d-20%d",ds1307GetDateDay(), ds1307GetMonth(), ds1307GetYear());
}

void ds1307GetTimestamp(char *timestamp)
{
    char time[20], date[20];
    ds1307GetDate(time);
    ds1307GetTime(date);
    sprintf(timestamp, "%s %s", time, date);
}

/**
 * @brief Read a single register from DS1307
 * @param reg the register to be read
 * @return uint8_t value that was read
 */
uint8_t ds1307ReadByte(uint8_t reg)
{
    // ESP_LOGI(TAG, "Reading the 0x%x register", reg);

    uint8_t value;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, DS1307_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, DS1307_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &value, ACK_VAL);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    return value;
}

/**
 * @brief Write a single register in the DS1307
 * @param reg the register to be write
 * @param value to be write
 * @return esp_err_t ESP_OK if the write was ok, otherwise ESP_ERR
 */
esp_err_t ds1307WriteByte(uint8_t reg, uint8_t value)
{
    ESP_LOGI(TAG, "Writing in the 0x%x register the value 0x%x", reg, value);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, DS1307_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, value, ACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

uint8_t dec2bcd(uint8_t num)
{
  return ((num/10 * 16) + (num % 10));
}

uint8_t bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 16));
}