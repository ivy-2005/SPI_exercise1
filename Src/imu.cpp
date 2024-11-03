//
// Created by 包辰宸 on 24-11-2.
//

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include <math.h>
#include "imu.h"
#include "spi.h"
#include "main.h"
#include "stm32f4xx_hal.h"

extern float accel[3];
extern float gyro[3];

extern uint8_t i;
uint8_t rx_data[8];
extern uint8_t tx_data;
extern uint8_t range;

void BMI088_ACCEL_NS_L() {
    HAL_GPIO_WritePin(CS1_Acc_GPIO_Port, CS1_Acc_Pin, GPIO_PIN_RESET);
}
void BMI088_ACCEL_NS_H() {
    HAL_GPIO_WritePin(CS1_Acc_GPIO_Port, CS1_Acc_Pin, GPIO_PIN_SET);
}
void BMI088_GYRO_NS_L() {
    HAL_GPIO_WritePin(CS1_Gyro_GPIO_Port, CS1_Gyro_Pin, GPIO_PIN_RESET);
}
void BMI088_GYRO_NS_H() {
    HAL_GPIO_WritePin(CS1_Gyro_GPIO_Port, CS1_Gyro_Pin, GPIO_PIN_SET);
}

/*BMI088_read_write_byte函数中填入你的代码, 它调用了HAL库的函数来读取/写入数据, 但不包含片选的部分.
定义BMI088_accel_write_single_reg(reg, data)函数, 它应可以: (a)
下拉加速度计片选信号
        (b)将data写入到reg寄存器地址里
                (c)上拉加速度计片选信号

定义BMI088_accel_read_single_reg(reg, data)函数, 它应可以: (a)
下拉加速度计片选信号
        (b)将reg寄存器里的数据写入到data变量里
                (c)上拉加速度计片选信号
*/

void BMI088_ReadReg_ACCEL(uint8_t reg, uint8_t length) {
    BMI088_ACCEL_NS_L();
    tx_data = (reg | 0x80);
    HAL_SPI_Transmit(&hspi1, &tx_data, 1, 1000);

    //while(HAL_SPI_GetState(&hspi1)==HAL_SPI_STATE_BUSY_TX);

    HAL_SPI_Receive(&hspi1, rx_data, length, HAL_MAX_DELAY);
    // HAL_SPI_Receive(&hspi1, rx_data, 1, 1000);
    // while(HAL_SPI_GetState(&hspi1)==HAL_SPI_STATE_BUSY_RX);
    //
    // i = 0;
    // while (i < length) {
    //     HAL_SPI_Receive(&hspi1, return_data, 1, 1000);
    //     while(HAL_SPI_GetState(&hspi1)==HAL_SPI_STATE_BUSY_RX);
    //     rx_data[i] = *return_data;
    //     i++;
    // }
    BMI088_ACCEL_NS_H();
    accel[0] = (int16_t)((rx_data[2] << 8) | rx_data[1]) *1000*pow(2,(range+1))*1.5/32768;
    accel[1] = (int16_t)((rx_data[4] << 8) | rx_data[3]) *1000*pow(2,(range+1))*1.5/32768;
    accel[2] = (int16_t)((rx_data[6] << 8) | rx_data[5]) *1000*pow(2,(range+1))*1.5/32768;
}

void BMI088_ReadReg_GYRO(uint8_t reg, uint8_t length) {

    BMI088_GYRO_NS_L();
    tx_data = (reg | 0x80);

    HAL_SPI_Transmit(&hspi1, &tx_data, 1, 1000);

    // while(HAL_SPI_GetState(&hspi1)==HAL_SPI_STATE_BUSY_TX);

    HAL_SPI_Receive(&hspi1, rx_data, length, 1000);
    // i = 0;
    // while (i < length)
    // {
    //     HAL_SPI_Receive(&hspi1, return_data, 1, 1000);
    //     while(HAL_SPI_GetState(&hspi1)==HAL_SPI_STATE_BUSY_RX);
    //     rx_data[i] = *return_data;
    //     i++;
    // }
    BMI088_GYRO_NS_H();

    gyro[0] = ((int16_t)((rx_data[1]) << 8) | rx_data[0])*2000/32767 ;
    gyro[1] = ((int16_t)((rx_data[3]) << 8) | rx_data[2])*2000/32767 ;
    gyro[2] = ((int16_t)((rx_data[5]) << 8) | rx_data[4])*2000/32767 ;
}

void BMI088_WriteReg(uint8_t reg, uint8_t write_data) {
    BMI088_ACCEL_NS_L();
    tx_data = (reg & 0x7F);
    HAL_SPI_Transmit(&hspi1, &tx_data, 1, 1000);
   // while(HAL_SPI_GetState(&hspi1)==HAL_SPI_STATE_BUSY_TX);

    tx_data = write_data;
    HAL_SPI_Transmit(&hspi1, &tx_data, 1, 1000);
   // while(HAL_SPI_GetState(&hspi1)==HAL_SPI_STATE_BUSY_TX);

    HAL_Delay(1);
    BMI088_ACCEL_NS_H();
}

void BMI088_Init() {
    // // Soft Reset ACCEL
    BMI088_ACCEL_NS_L();
    BMI088_WriteReg(0x7E, 0xB6);
    HAL_Delay(1);
    BMI088_ACCEL_NS_H();
    //
    // // Soft Reset GYRO
    BMI088_GYRO_NS_L();
    BMI088_WriteReg(0x14, 0xB6);
    HAL_Delay(30);
    BMI088_GYRO_NS_H();
    //
    // // Switch ACCEL to Normal Mode
    BMI088_ACCEL_NS_L();
    HAL_Delay(1);
    BMI088_WriteReg(0x7D, 0x04);
    HAL_Delay(1);
    BMI088_ACCEL_NS_H();
}


#ifdef __cplusplus
}
#endif
