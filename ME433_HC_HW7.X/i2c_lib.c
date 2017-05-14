#include<xc.h>
#include "i2c_lib.h"
#include "lcd_lib.h"
#include<sys/attribs.h>

void I2C_master_setup(void){
  ANSELBbits.ANSB2=0; 
  ANSELBbits.ANSB3=0; 
  I2C2BRG = 53;                                            
  I2C2CONbits.ON = 1;               
  
  setState(0x10,0b10000010); 
  setState(0x11,0b10001000); 
  setState(0x12,0b00000100);
}

void I2C_master_start(void){
    I2C2CONbits.SEN=1;
    while(I2C2CONbits.SEN){
        ;
    }
}

void I2C_master_restart(void){
    I2C2CONbits.RSEN=1;
    while(I2C2CONbits.RSEN){
        ;
    }
}

void I2C_send(unsigned char b){
    I2C2TRN=b;
    while(I2C2STATbits.TRSTAT){;}
    if(I2C2STATbits.ACKSTAT){}
}

unsigned char I2C_receive(void){
    I2C2CONbits.RCEN=1;
    while(!I2C2STATbits.RBF){;}
    return I2C2RCV;
}

void I2C_ack(int a){
    I2C2CONbits.ACKDT=a;
    I2C2CONbits.ACKEN=1;
    while(I2C2CONbits.ACKEN){;}
}

void I2C_master_stop(void){
    I2C2CONbits.PEN=1;
    while(I2C2CONbits.PEN){;}
}

void init(){
    ANSELBbits.ANSB2=0;
    ANSELBbits.ANSB3=0;
    I2C2BRG=233;
    I2C2CONbits.ON=1;
    
    I2C_master_start();
    I2C_send(slave_w);
    I2C_send(0x00);
    I2C_send(0b11111110);
    I2C_master_stop();
}

void setState(char add, char i){
    I2C_master_start();
    I2C_send(slave_w);
    I2C_send(add);
    I2C_send(i);
    I2C_master_stop();
}

char getState(char p){
    char s;
    char r;
    
    I2C_master_start();
    I2C_send(slave_w);
    I2C_send(p);
    I2C_master_restart();
    I2C_send(slave_r);
    s=I2C_receive();
    I2C_ack(1);
    I2C_master_stop();
   // r=(s>>p)&(0b1);
    
    return s;
}

short combine(unsigned char x, unsigned char y){
    short r;
    r=(y<<8)|x;
    return r;
}

void I2C_read_multiple(unsigned char address, unsigned char reg, unsigned char * data, int length){
    unsigned short i;
    
    I2C_master_start(); 
    I2C_send(address); 
    I2C_send(reg); 
    I2C_master_restart(); 
    I2C_send(address+1);
    
    for(i=0;i<length-1;i=i+1){
        data[i]=I2C_receive();
        I2C_ack(0);
    }
    data[length-1]=I2C_receive();
    I2C_ack(1); 
    I2C_master_stop(); 
}