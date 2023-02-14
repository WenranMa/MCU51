#include <reg52.h> // 51 related registers
#include <intrins.h> // left and right shift function

#define uint unsigned int
#define uchar unsigned char

// 流水灯
uchar ledBits;// led bits definition

// 中断 + 数码管
uchar icnt0; // t0 interrupt count 
uchar icnt1; 

sbit dula = P2^6; // led location
sbit wela = P2^7; // led display number

uchar num, b, s, g;	// for led display, highest 3 bits;

uchar code table[]= {
0x3F, 0x06, 0x5B, 0x4F, 
0x66, 0x6D, 0x7D, 0x07, 
0x7F, 0x6F, 0x77, 0x7C,
0x39, 0x5C, 0x79, 0x71
};
// code means use rom but not ram.

// 蜂鸣器
sbit beep = P2^3;

// 独立按键
uchar numKey; // for test key;

sbit key1 = P3^0;
sbit key2 = P3^1;
sbit key3 = P3^2;
sbit key4 = P3^3;

void delay(uint z) { // delay z ms, not exact
	uint x,y;
	for(x = z; x > 0; x--)
		for(y = 114; y > 0 ; y--); 		
} 

// 中断+数码管
void displayLed(uchar b, uchar s, uchar g) {
	dula = 1; //display number on
	P0 = table[b];
	dula = 0;
	P0 = 0xff; // clean for blicking
	wela = 1;  //location on 
	P0 = 0Xfe; //1111 1110，lowest bit, p0.0, but highest location on board.
	wela = 0;
	delay(5);

	dula = 1;
	P0 = table[s];
	dula = 0;
	P0 = 0xff;
	wela = 1;
	P0 = 0xfd; // 1111 1101, p0.1
	wela = 0;
	delay(5);

	dula = 1;
	P0 = table[g];
	dula = 0;
	P0 = 0xff;
	wela = 1;
	P0 = 0xfb; // 1111 1011, p0.2
	wela = 0;
	delay(5);
}

// 独立按键
void keyScan() {
  	if (key1 == 0) {
		delay(10);
		if (key1 == 0) {
			numKey+=1;
			while(!key1){
			}
		} 
	}
	if (key2 == 0) {
		delay(10);
		if (key2 == 0) {
			numKey-=1;
			while(!key2){
			}
		} 
	}
	if (key3 == 0) {
		delay(10);
		if (key3 == 0) {
			numKey = 0;
			while(!key3){
			}
		} 
	}
}

void displayLed2(uchar n) {
	uchar s,g;
	s = n / 10;
	g = n % 10;

	dula = 1;
	P0 = table[s];
	dula = 0;
	P0 = 0xff;
	wela = 1;
	P0 = 0xbf; // 1011 1111, p0.6
	wela = 0;
	delay(5);

	dula = 1;
	P0 = table[g];
	dula = 0;
	P0 = 0xff;
	wela = 1;
	P0 = 0x7f; // 0111 1111, p0.7
	wela = 0;
	delay(5);
}

// 蜂鸣器
void buzz() {
	beep = 0;
//	delay(10);
}

// 矩阵键盘
void displayLed3(uchar i) {
	dula = 1;
	P0 = table[i];
	dula = 0;
	P0 = 0xff;
	wela = 1;
	P0 = 0xef; // 1110 1111
	wela = 0;
}

void matrixKeyScan() {
	uchar temp, matrixKeyNum;
	
	P3 = 0xfe; // 1111 (column) 1110 (first row is 0) 
	temp = P3;	// read P3,
	temp = temp & 0xf0; // to check highest four bits has any 0
	if (temp != 0xf0) { // higest four bits has 0. means first row has key down.
		delay(10);
		temp = P3;
		temp = temp & 0xf0;	// 去抖动
		if(temp != 0xf0) {
			temp = P3;
			switch(temp){
			case 0xee:	 // 1110 1110
				matrixKeyNum = 0;
				break;
			case 0xde:	 // 1101 1110
				matrixKeyNum = 1;
				break;
			case 0xbe:	 // 1011 1110
				matrixKeyNum = 2;
				break;
			case 0x7e:	 // 0111 1110
				matrixKeyNum = 3;
				break;
			}
			while(temp != 0xf0) { //release key
				temp = P3;
				temp = temp & 0xf0;
			}
			displayLed3(matrixKeyNum);
		}
	}
  
	P3 = 0xfd; // 1111 (column) 1101 (second row is 0) 
	temp = P3;	// read P3,
	temp = temp & 0xf0; // to check highest four bits has any 0
	if (temp != 0xf0) { // higest four bits has 0. means second row has key down.
		delay(10);
		temp = P3;
		temp = temp & 0xf0;
		if(temp != 0xf0) {
			temp = P3;
			switch(temp){
			case 0xed:   // 1110 1101
				matrixKeyNum = 4;
				break;
			case 0xdd:	 // 1101 1101
				matrixKeyNum = 5;
				break;
			case 0xbd:	 // 1011 1101
				matrixKeyNum = 6;
				break;
			case 0x7d:	 // 0111 1101
				matrixKeyNum = 7;
				break;
			}
			while(temp != 0xf0) {
				temp = P3;
				temp = temp & 0xf0;
			}
			displayLed3(matrixKeyNum);
		}
	}

	P3 = 0xfb; // 1111 (column) 1011 (third row is 0) 
	temp = P3;	// read P3,
	temp = temp & 0xf0; // to check highest four bits has any 0
	if (temp != 0xf0) { // higest four bits has 0. means third row has key down.
		delay(10);
		temp = P3;
		temp = temp & 0xf0;
		if(temp != 0xf0) {
			temp = P3;
			switch(temp){
			case 0xeb:	 // 1110 1011
				matrixKeyNum = 8;
				break;
			case 0xdb:	 // 1101 1011
				matrixKeyNum = 9;
				break;
			case 0xbb:	 // 1011 1011
				matrixKeyNum = 10;
				break;
			case 0x7b:	 // 0111 1011
				matrixKeyNum = 11;
				break;
			}
			while(temp != 0xf0) {
				temp = P3;
				temp = temp & 0xf0;
			}
			displayLed3(matrixKeyNum);
		}
	}

	P3 = 0xf7; // 1111 (column) 0111 (third row is 0) 
	temp = P3;	// read P3,
	temp = temp & 0xf0; // to check highest four bits has any 0
	if (temp != 0xf0) { // higest four bits has 0. means third row has key down.
		delay(10);
		temp = P3;
		temp = temp & 0xf0;
		if(temp != 0xf0) {
			temp = P3;
			switch(temp){
			case 0xe7:	// 1110 0111
				matrixKeyNum = 12;
				break;
			case 0xd7:	// 1101 0111
				matrixKeyNum = 13;
				break;
			case 0xb7:	// 1011 0111
				matrixKeyNum = 14;
				break;
			case 0x77:	// 0111 0111
				matrixKeyNum = 15;
				break;
			}
			while(temp != 0xf0) {
				temp = P3;
				temp = temp & 0xf0;
			}
			displayLed3(matrixKeyNum);
		}
	}
}


// 8x8 matrix
sbit dataInput = P3^4;
sbit shiftCLK = P3^5;
sbit lockCLK = P3^6; 

void displayMatrix() {
	uchar i, dat; // 变量定义放在最前面?	
	dat = 0xfe;
	lockCLK = 0;
	for(i=0; i<8; i++) {
		shiftCLK = 0;	
		if (dat & 0x01) {
			dataInput = 1;
		} else {
			dataInput = 0;
		}
		shiftCLK = 1;
		dat >>= 1;
	}

	dat = 0x80;
	for(i = 0; i < 8; i++) {
		shiftCLK = 0;
		if (dat & 0x01) {
			dataInput = 1;
		} else {
			dataInput = 0;
		}
		shiftCLK = 1;
		dat >>= 1;
	}
	lockCLK = 1;
}

void init() {
	TMOD = 0x11; // T0 and T1 work in mode 1. 
	 
	TH0 = (65536 - 45872)/256;
	TL0 = (65536 - 45872)%256;
	TH1 = (65536 - 45872)/256;
	TL1 = (65536 - 45872)%256; // init value for 50ms

	EA = 1; // enble interrupt 
	
	ET0 = 1;
  	ET1 = 1; // enable t0 t1
	
	TR0 = 1;
	TR1 = 1; // start t0 t1
}

void main()//main func can loop
{
	init();

	ledBits = 0xfe;
	P1 = ledBits; //1111 1110
	
	while(1){
		/*
		if(icnt0 == 10) {  // 0.5s
			icnt0 = 0;
		    ledBits = _crol_(ledBits, 1);//shift left by one bit
			P1 = ledBits;
		}
		if(icnt1 == 5) {  // 5 x 50ms = 0.25s
			icnt1 = 0;
			b = num / 100;
			s = num % 100 / 10;
			g = num % 10;
			num++;
			if (num == 200) {
				num = 0;
			}
		}
		*/
		/* put the above code the interrupt function.
			because the keyscan delay will cause the icnt0 or icnt1 pass 10 or 5,
			then they will run to 256 then go back to 0,
			it will take 250 * 50 ms = 12.5s.		  	
		*/
		displayLed(b, s, g);

		keyScan();
		displayLed2(numKey);

	//	buzz();

	//	matrixKeyScan();

		displayMatrix();
	}	
}

void T0_timer() interrupt 1 {
	TH0 = (65536 - 45872)/256;
	TL0 = (65536 - 45872)%256;
	icnt0++;
	if(icnt0 == 10) {  // 0.5s
		icnt0 = 0;
		ledBits = _crol_(ledBits, 1);//shift left by one bit
		P1 = ledBits;
	}
} 

void T1_timer() interrupt 3 {
	TH1 = (65536 - 45872)/256;
	TL1 = (65536 - 45872)%256;
	icnt1++;
	if(icnt1 == 5) {  // 5 x 50ms = 0.25s
		icnt1 = 0;
		b = num / 100;
		s = num % 100 / 10;
		g = num % 10;
		num++;
		if (num == 200) {
			num = 0;
		}
	}	
}