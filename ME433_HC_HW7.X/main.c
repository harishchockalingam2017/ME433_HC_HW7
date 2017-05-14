#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "i2c_lib.h"
#include "lcd_lib.h"
#include "stdio.h"

// DEVCFG0
#pragma config DEBUG = 0b11 // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV =  DIV_1// divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 2 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

void main() {
    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    SPI1_init();
    LCD_init();
    I2C_master_setup();
    
    // do your TRIS and LAT commands here
    TRISAbits.TRISA4=0;
    TRISBbits.TRISB4=1;
    LATAbits.LATA4=1;
    
    __builtin_enable_interrupts();
    
    LCD_clearScreen(0x00FF);
    char message[100];
    
    char whoami=getState(0x0F);
    sprintf(message,"WHOAMI ");
    drawString(5,2,message);
    sprintf(message,"%d",whoami);
    drawString(5,12,message);
    
    sprintf(message,"X");
    drawString(120,64,message);
    sprintf(message,"Y");
    drawString(64,2,message);
    
    int i;
    char data_ap[14];
    short comb_data[7];
    
    while(1) {
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT()<4800000){;
            while(!PORTBbits.RB4){;}}
        
        I2C_read_multiple(IMU_ADD,0x20,data_ap,14);
        
        for(i=0; i<7; i=i+1){
            comb_data[i]=combine(data_ap[2*i],data_ap[1+2*i]);
        }
        
        xdirect(64,64,0xFFE0,50,4,comb_data[4]);
        ydirect(64,64,0x07E0,50,4,comb_data[5]);
        LATAINV=0b10000;
	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
		  // remember the core timer runs at half the sysclk
    }
}