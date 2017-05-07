 /* File:   i2c_lib.h
 * Author: Harish
 *
 * Created on April 17, 2017, 11:04 AM
 */

#ifndef I2C_LIB_H
#define	I2C_LIB_H

#define slave_w 0b01000000
#define slave_r 0b01000001

void init();
void I2C_master_setup(void);
char getState(char p);
void setState(char add, char i);

void I2C_master_start(void);
void I2C_master_stop(void);
void I2C_master_restart(void);

void I2C_send(unsigned char i);
unsigned char I2C_receive(void);
void i2c_ack(int a);

#endif