#include <stdio.h>
#include "Main.h"
#include "I2C_Support.h"
#include <p18f4620.h>
#include "I2C.h"

extern unsigned char second, minute, hour, dow, day, month, year;
extern unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;


void DS1621_Init()
{
    char Device = 0x48;                                         // Device ID
    I2C_Write_Cmd_Write_Data (Device, ACCESS_CFG, CONT_CONV);   
    I2C_Write_Cmd_Only(Device, START_CONV);                    
}

int DS3231_Read_Temp()
{
    char Device = 0x68;                                 // Device ID
    char Address_E = 0x0E;                              
    char Address_11 = 0x11;  
    char Data_Out = I2C_Write_Address_Read_One_Byte(Device, Address_E);
    Data_Out = Data_Out | 0x20;
    I2C_Write_Address_Write_One_Byte(Device,  Address_E, Data_Out);
    int Data_Ret = I2C_Write_Address_Read_One_Byte(Device, Address_11);

    return Data_Ret;
}

int DS1621_Read_Temp()
{
    char Device = 0x48;                                         // Device ID
    char Cmd = READ_TEMP;                                       // As Given
    int Data_Ret;
    I2C_Start();                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                               // Device address
    I2C_Write(Cmd);                                             // Send register address
    I2C_ReStart();                                              // Restart I2C
    I2C_Write((Device << 1) | 1);                               // Initialize data read
    Data_Ret = I2C_Read(NAK);
    I2C_Stop();                                                 // End I2C protocol

    return Data_Ret;
}

void DS3231_Read_Time()
{
    char Device = 0x68;                                         // Device ID
    char Address = 0x00;                                        // Beginning Address 0
    char Data_Ret;    
    I2C_Start();                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                               // Device address
    I2C_Write(Address);                                         // Send register address
    I2C_ReStart();                                              // Restart I2C
    I2C_Write((Device << 1) | 1);                               // Initialize data read
    second = I2C_Read(ACK);                                     // Read seconds
    minute = I2C_Read(ACK);                                     // Read minutes
    hour = I2C_Read(ACK);                                       // Read hours
    dow = I2C_Read(ACK);                                        // Read day of the week
    day = I2C_Read(ACK);                                        // Read day
    month = I2C_Read(ACK);                                      // Read month
    year = I2C_Read(NAK);                                       // Read year
    I2C_Stop();                                                 // End I2C protocol
}

void DS3231_Write_Time()
{
    char Device = 0x68;                                         // Device ID
    char Address = 0x00;                                        // Beginning Address 0
    second = dec_2_bcd(setup_second);                           // 
    minute = dec_2_bcd(setup_minute);                           // 
    hour = dec_2_bcd(setup_hour);                               // 
    dow = 0x01;                                                 // 
    day = dec_2_bcd(setup_day);                                 // 
    month = dec_2_bcd(setup_month);                             // 
    year = dec_2_bcd(setup_year);                               // 
    I2C_Start();                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                               // Device address Write mode
    I2C_Write(Address);                                         // Send register address
    I2C_Write(second);                                          // Write seconds
    I2C_Write(minute);                                          // Write minutes
    I2C_Write(hour);                                            // Write hours
    I2C_Write(dow);                                             // Write DOW
    I2C_Write(day);                                             // Write day
    I2C_Write(month);                                           // Write month
    I2C_Write(year);                                            // Write year
    I2C_Stop();                                                 // End I2C protocol
}

