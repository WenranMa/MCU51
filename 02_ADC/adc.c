/*****************************
* Check the input voltage. 
* and show on the led display.
******************************/

#include <reg52.h>
#include <intrins.h>

#define MAIN_Fosc 11059200UL //

typedef unsigned char uchar;
typedef unsigned char INT8U;

typedef unsigned int uint;
typedef unsigned int INT16U;

#define AD_CH0 0x94 // 光敏电阻cmd
#define AD_CH1 0xd4 // 热敏电阻cmd
#define AD_CH2 0xa4 // 电位器cmd
#define AD_CH3 0xe4 // 外部通道

sbit CS = P3^7;
sbit DCLK = P2^1;
sbit DIN = P2^0;
sbit DOUT = P2^5;

sbit DU = P2^6;
sbit WE = P2^7;

uchar code SMGduan[]= {
0x3F, 0x06, 0x5B, 0x4F, 
0x66, 0x6D, 0x7D, 0x07, 
0x7F, 0x6F, 0x77, 0x7C,
0x39, 0x5C, 0x79, 0x71
};
/**
数码管从
      === A ===
     //      //
     F       B
    //      //
    === G === 
   //      // 
   E       C
  //      //
  === D ===  H(dot)

So, the number 0 is 0x3F, which is 0011 1111, Highest bit is H, lowest is A, 
the number 0, only G and H are not lit.
*/

uchar code SMGwei[] = {0xfe, 0xfd, 0xfb, 0xf7};

uint voltage;

void SPI_WRITE(uchar DAT){
    uchar i;
    for(i=0; i<8; i++){
        DCLK = 0;
        if(DAT & 0x80){
            DIN = 1;
        } else {
            DIN = 0;
        }
        DCLK = 1;
        DAT <<= 1;
    }
}

uint SPI_Read(){
    uchar i;
    uint DAT;
    for(i=0; i<12; i++){
        DAT <<= 1;
        DCLK = 1;
        DCLK = 0;
        if(DOUT){
            DAT |= 0x01;
        }
    }
    return DAT;
}

uint ReadAD(uchar cmd){
    uint DAT;
    CS = 0;
    SPI_WRITE(cmd);
    DCLK = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    DAT = SPI_Read();
    CS = 1;
    return DAT;
}

void display(uint i){
    uchar q, b, s, g;
    static uchar wei;
    q = i / 1000;
    b = i % 1000 / 100;
    s = i % 100 / 10;
    g = i % 10;

    P0 = 0xFF;
    WE = 1;
    P0 = SMGwei[wei];
    WE = 0;
    P0 = 0xFF;
    switch(wei){
        case 0: DU = 1; P0 = SMGduan[q]|0x80; DU = 0; break; //0x80 is the dot.
        case 1: DU = 1; P0 = SMGduan[b]; DU = 0; break;
        case 2: DU = 1; P0 = SMGduan[s]; DU = 0; break;
        case 3: DU = 1; P0 = SMGduan[g]; DU = 0; break;
    }
    wei++;
    if(wei == 4){
        wei = 0;
    }
}

void Delay_MS(INT16U ms) {
    INT16U i;
    do{
        i = MAIN_Fosc / 96000;
        while(--i){}; //96个时钟周期
    }while(--ms);
}
// 1ms = 0.001s
// 0.001 / 1 / 11059200 = 11059.2 个时钟周期。
// 11059.2 / 96 = 115.2;

void main(){
    uchar i;
    while(1){
        if(i >= 100){ //about 00ms to sample once，easy to observe the led
            i = 0;
            voltage = ReadAD(AD_CH3); // 12 bits, so the highest voltage is 4096.
            voltage = voltage * 1.2207; // 5000mv / 4096 = 1.2207
        }
        display(voltage);
        Delay_MS(5);
        i++;
    }
}
