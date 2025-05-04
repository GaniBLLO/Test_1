# Test_1
Для имитации работы приёма данных "извне" на STM32F407vet6, подключил плату ESP32 по spi. В связке с логическим анализатором.

Исходники:
```
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"


#define GPIO_HANDSHAKE 2
#define GPIO_MOSI 12
#define GPIO_MISO 13
#define GPIO_SCLK 15
#define GPIO_CS 14

#ifdef CONFIG_IDF_TARGET_ESP32
#define RCV_HOST    HSPI_HOST

#else
#define RCV_HOST    SPI2_HOST
#endif

void app_main(void){
    esp_err_t ret;

    //Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg = {
        .mode = 3,
        .spics_io_num = GPIO_CS,
        .queue_size = 3,
        .flags = 0,
    };

    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

    //Initialize SPI slave interface
    ret = spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    assert(ret == ESP_OK);

    WORD_ALIGNED_ATTR uint8_t sendbuf[4] = {0x00, 0x20, 0x55, 0x77};
    WORD_ALIGNED_ATTR uint8_t recvbuf[3] = {0};
    spi_slave_transaction_t t = {
        .length = 4 * 8,
        .tx_buffer = sendbuf,
        .rx_buffer = recvbuf
    };

    while (1) {
        spi_slave_transmit(RCV_HOST, &t, portMAX_DELAY);
        
        printf("Received: 0x%02X 0x%02X 0x%02X\n", recvbuf[0], recvbuf[1], recvbuf[2]);
        memset(recvbuf, 0, sizeof(recvbuf));
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```
