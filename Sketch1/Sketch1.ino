/*
 Name:		Sketch1.ino
 Created:	2017/10/26 17:05:18
 Author:	wangxu
*/


#include <TimerOne.h>
#include <MsTimer2.h>

//类型定义
#define uchar unsigned char

//过零检测
#define ZERO_DETECT 0// 引脚2
// Motor
#define WASH_MOTOR 36
// KEY
#define KEY1ANDOUT 64
#define KEY2ANDOUT 65
#define KEY3ANDOUT 66
#define KEY4ANDOUT 67
#define KEY5ANDOUT 68
#define KEY6ANDOUT 69

// HC164
#define HC164_DATA 42
#define HC164_CLOCK 40

// 全局变量定义
// ============================
// 4位7段数码管
//   8.8.:8.'8.
//     3
//     __
// 16 |13| 11
//     --
//  5 |__| 15
//     14
//  
// ============================   
//显示数字
uint8_t NumData[10] = {
	0x5F,//0
	0x09,//1
	0x7C,//2
	0x76,//3
	0x63,//4
	0x37,//5
	0x3F,//6
	0x52,//7
	0x7F,//8
	0x77//9	
};
//显示字符
uint8_t SignData[8] = {
	0x7B,//A
	0x1D,//C
	0x3D,//E
	0x39,//F
	0x6B,//H
	0x0D,//L
	0x79,//P
	0x4F,//U
};

volatile uchar second=0, minute=0;
int MinOpenAngle = 2000;
int MaxOpenAngle = 7000;
int DefaultOpenAngle = 3000;
int IncOpenAngle = 20;

int key[7] = { 1 };

uint8_t data[5];

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(HC164_CLOCK, OUTPUT);
	pinMode(HC164_DATA, OUTPUT);
	
	pinMode(KEY1ANDOUT, OUTPUT);
	pinMode(KEY2ANDOUT, OUTPUT);
	pinMode(KEY3ANDOUT, OUTPUT);
	pinMode(KEY4ANDOUT, OUTPUT);
	pinMode(KEY5ANDOUT, OUTPUT);
	pinMode(KEY6ANDOUT, OUTPUT);

	pinMode(WASH_MOTOR, OUTPUT);
	attachInterrupt(ZERO_DETECT, zero_Change, CHANGE);

	//MsTimer2::set(1000, timerIsr); //1s定时中断
	//MsTimer2::start();

	Timer1.initialize(10);//10us
	Timer1.attachInterrupt(timerInt_Handle);

	Serial.begin(115200);
}

void loop()
{
	//delay(100);
	//
	//read_Key(key);

	//if (key[1] == 1)                //最大延时累加，以及封顶
	//{
	//	m = m + 20;
	//	if (m >= mx)
	//	{
	//		m = mx;
	//	}
	//}

	//if (key[2] == 1)
	//{
	//	m = m - 20;                //最小延时累减，以及封底
	//	if (m <= mi)
	//	{
	//		m = mi;
	//	}
	//}

	//data[0] = NumData[0];
	//data[1] = NumData[1];
	//data[2] = SignData[8];
	//data[3] = NumData[2];
	//data[4] = NumData[3];

	//hc164_Show(data);

	////digitalWrite(WASH_MOTOR, 0);//OFF

	//Serial.println(m);
	
	set_Time();

	msg_Show();
}

void zero_Change()
{
	//Timer1.initialize(DefaultOpenAngle);//过零检测延时
	//Timer1.attachInterrupt(Motor_Open);	
}

//          01234
// data[] = 88:88
// Multi segment digital
void msg_Show()
{
	
	shiftOut(HC164_DATA, HC164_CLOCK, MSBFIRST, NumData[second % 10]);
	msg_select(0, LOW);
	delayMicroseconds(1000);	
	msg_select(0, HIGH);

	shiftOut(HC164_DATA, HC164_CLOCK, MSBFIRST, NumData[second / 10]);
	msg_select(1, LOW);
	delayMicroseconds(1000);	
	msg_select(1, HIGH);
	
	shiftOut(HC164_DATA, HC164_CLOCK, MSBFIRST, NumData[minute % 10]);
	msg_select(3, LOW);
	delayMicroseconds(1000);
	msg_select(3, HIGH);
	
	shiftOut(HC164_DATA, HC164_CLOCK, MSBFIRST, NumData[minute / 10]);
	msg_select(4, LOW);
	delayMicroseconds(1000);
	msg_select(4, HIGH);
}

//num =  0 / 1 / 2=: / 3 /4
//mode = HIGH -> open / LOW -> close 
void msg_select(uchar num, uchar mode)
{
	//显示位引脚，从低到高
	uint8_t DisPin[5] = { KEY5ANDOUT ,KEY4ANDOUT ,KEY1ANDOUT ,KEY2ANDOUT ,KEY3ANDOUT };
	
	if (num<5)
	{
		pinMode(DisPin[num], OUTPUT);
		digitalWrite(DisPin[num], mode);
	}
}

double read_Voltage(int pin)
{
	int n = analogRead(pin);
	double vol = n * (5 / 1024.0);
}

void timerInt_Handle()
{
	second++;
	if (second > 59)
	{
		second = 0;
		minute++;
		if (minute > 59)
		{
			minute = 0;
		}
	}

	Serial.print("Time :  ");
	Serial.print(":");
	Serial.print(minute, DEC);
	Serial.print(":");
	Serial.println(second, DEC);
}

void read_Key(int key[])
{
	pinMode(KEY1ANDOUT, INPUT_PULLUP);
	pinMode(KEY2ANDOUT, INPUT_PULLUP);
	pinMode(KEY3ANDOUT, INPUT_PULLUP);
	pinMode(KEY4ANDOUT, INPUT_PULLUP);
	pinMode(KEY5ANDOUT, INPUT_PULLUP);
	pinMode(KEY6ANDOUT, INPUT_PULLUP);

	for (int i = 1; i < 7; i++)
	{		
		key[i] = digitalRead(KEY1ANDOUT);
		delayMicroseconds(100);
	}
}

void set_Time()
{

}

void motor_Open()
{
	digitalWrite(WASH_MOTOR, 1);//OPEN
	delayMicroseconds(10);
	digitalWrite(WASH_MOTOR, 0);//OFF
}
