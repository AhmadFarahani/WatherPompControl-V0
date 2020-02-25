/*******************************************************
This program was created by the
CodeWizardAVR V3.12 Advanced
Automatic Program Generator
© Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project :
Version :
Date    : 02/17/2020
Author  :
Company :
Comments:


Chip type               : ATmega32
Program type            : Application
AVR Core Clock frequency: 12.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/

#include <mega32.h>
#include <i2c.h>
#include <ds1307.h>
#include <alcd.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>

#define error   8
#define chanel  3 // number chanel
#define up      14
#define down    13
#define ok      12
#define cancel  11
#define blk     PORTB.2
#define pomp    PORTD.2 // wather pomp
#define R1      PORTD.3 // usage motor
#define R2      PORTD.4 // usage motor
#define R3      PORTD.5 // usage motor
#define R4      PORTD.6 // usage motor
#define read    15
#define readPINA 0b00001111
#define chrDel5 30      // value change delay 5 sec
#define valDel  10     // value delay ms
//**************************
char hour,week,day,mon,year,lcd_buff[32],chr_in,chr_rtn,month,minute,second,chr_out_en;
char out_num,out_state;
bit out_en;
char out_fn_menu; // (LSB)5 bit out number , 1 bit en/ds , 2 bit state type (MSB)
//char days[3][6]={"SUN","MON","TUE","WED","THU","FRI","SAT"};
//*****************************
//      functions
void fn_mnl(char chr_mnl);
char fn_del5();
void fn_time();
void fn_date();
void fn_displaytime (char h,char m,char s);
void fn_displaydate (char y,char m,char d,char w);
void fn_menu(char slt);
void fn_settime(char slt);//function settings time
void fn_setdate(char slt);//function settings date
void fn_daily();//  function daily
void fn_weekly();//  function weekly
void fn_monthly();//  function monthly
char prog_state(char prog_state,char prog_en,char prog_num);
//*****************************
//      main
void main(void)
{

DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (1<<DDB2) | (0<<DDB1) | (0<<DDB0);
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);
DDRC=(1<<DDC7) | (1<<DDC6) | (1<<DDC5) | (1<<DDC4) | (1<<DDC3) | (1<<DDC2) | (0<<DDC1) | (0<<DDC0);
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);
DDRD=(1<<DDD7) | (1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3) | (1<<DDD2) | (0<<DDD1) | (0<<DDD0);
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Bit-Banged I2C Bus initialization
// I2C Port: PORTC
// I2C SDA bit: 1
// I2C SCL bit: 0
// Bit Rate: 100 kHz
// Note: I2C settings are specified in the
// Project|Configure|C Compiler|Libraries|I2C menu.
i2c_init();

// DS1307 Real Time Clock initialization
// Square wave output on pin SQW/OUT: Off
// SQW/OUT pin state: 0
rtc_init(0,0,0);

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTB Bit 1
// RD - PORTB Bit 0
// EN - PORTB Bit 7
// D4 - PORTB Bit 6
// D5 - PORTB Bit 5
// D6 - PORTB Bit 4
// D7 - PORTB Bit 3
// Characters/line: 16
lcd_init(16);
lcd_puts("In The Name Of Allah");
delay_ms(500);
lcd_clear();
while (1)
      {
    chr_in = PINA & readPINA;
    fn_time();
    fn_date();
    delay_ms(50);
    // settings time
    if(chr_in==up)
        {
            blk=1;
            delay_ms(100);
            if((chr_rtn=fn_del5())==1)fn_settime(0);
            delay_ms(1000);
        }
    // settings date
    if(chr_in==down)
        {
            blk=1;
            delay_ms(100);
            if((chr_rtn=fn_del5())==1)fn_setdate(0);
            delay_ms(1000);
        }
    // settings time pomp
    if(chr_in==ok){blk=1;delay_ms(100);if((chr_rtn=fn_del5())==1)fn_menu(1);}
    if(chr_in == 10 | chr_in == 9 | chr_in == 8 | chr_in == 7)fn_mnl(chr_in);
    blk=0;
    }
}


//****  function menu

void fn_menu(char slt)
{
    out_fn_menu=255;
    out_num=out_state=1;
loop:
    while((PINA & readPINA)!=read)
    rtc_get_time(&hour,&minute,&second);
    rtc_get_date(&week,&day,&mon,&year);
    lcd_clear();
    //  select number output
    while(1)
    {
        chr_in = PINA & readPINA;
        if(chr_in==up){delay_ms(200);out_num++;if(out_num==(chanel+1))out_num=1;}
        if(chr_in==down){delay_ms(200);out_num--;if(out_num==0)out_num=chanel;}
        if(chr_in==ok)break;
        if(chr_in==cancel)goto out;
        lcd_gotoxy(0,0);
        sprintf(lcd_buff,"out num = %u",out_num);
        lcd_puts(lcd_buff);
    }
    while((PINA & readPINA)!=read)
    delay_ms(500);
    //  select enable and disable
    while(1)
    {
        chr_in = PINA & readPINA;
        sprintf(lcd_buff," Disable ");
        if(chr_in==up){delay_ms(200);out_en=!out_en;}
        if(chr_in==down){delay_ms(200);out_en=!out_en;}
        if(chr_in==ok){delay_ms(200);chr_out_en=(00000001&&out_en)<<5;break;}
        if(chr_in==cancel)goto out;
        if(out_en==1)sprintf(lcd_buff," Enable  ");
        lcd_gotoxy(0,1);
        lcd_puts(lcd_buff);
    }

    while((PINA & readPINA)!=read)
    delay_ms(500);
    if(out_en==0)goto out2;
    //  select daily or weekly and monthly
    while(1)
    {
        chr_in = PINA & readPINA;
        sprintf(lcd_buff," None   ");
        if(chr_in==up){delay_ms(200);out_state++;if(out_state>3)out_state=1;}
        if(chr_in==down){delay_ms(200);out_state--;if(out_state==0)out_state=3;}
        if(chr_in==ok) goto out2; //break;
        if(chr_in==cancel)goto out;
        if(out_state==1)sprintf(lcd_buff," Daily  ");
        if(out_state==2)sprintf(lcd_buff," Weekly  ");
        if(out_state==3)sprintf(lcd_buff," Monthly ");
        lcd_gotoxy(0,1);
        lcd_puts(lcd_buff);
    }
//  save and exit after 5 sec delay

out:
    lcd_clear();
    lcd_puts(" Cancel Shod ");
    delay_ms(500);
    lcd_clear();
    goto exit;
out2:
     out_fn_menu = (out_num | chr_out_en)| out_state;//|| ;//(00011111 & out_num)|0b11100000; //>1: 00000001 > 11100001>11000001>
     lcd_clear();
     lcd_gotoxy(0,0);lcd_puts("out_fn_menu");lcd_gotoxy(0,1);
     sprintf(lcd_buff,"%u",out_fn_menu);
     lcd_puts(lcd_buff);
     delay_ms(100);
goto loop;
exit:
}

//*****************************     time & date
//  function settings time , slt = 0 set time to ds1307 , slt = 1 set time to memory

void fn_settime(char slt)
{
char i =0;
while((PINA & readPINA)!=read)
rtc_get_time(&hour,&minute,&second);
loop:
lcd_clear();
lcd_gotoxy(0,0);
lcd_puts("Set Time :");
    while(1)
    {
    chr_in = PINA & readPINA;
    switch(i)
          {
          case 0:
          if(up==chr_in){delay_ms(100);hour++;if(hour>23&hour<25)hour=0;}
          if(down==chr_in){delay_ms(100);hour--;if(hour>25)hour=23;}
          break;
          case 3:
          if(up==chr_in){delay_ms(100);minute++;if(minute>59&minute<61)minute=0;}
          if(down==chr_in){delay_ms(100);minute--;if(minute>61)minute=59;}
          break;
          case 6:
          if(up==chr_in){delay_ms(100);second++;if(second>59&hour<60)second=0;}
          if(down==chr_in){delay_ms(100);second--;if(second>61)second=59;}
          break;
          }
    if(ok==chr_in)
        {
        delay_ms(300);
        i=i+3;if(i>7)i=0;
            switch(slt)
            {
                case 0:
                if((chr_rtn=fn_del5())==1)
                {
                    rtc_set_time(hour,minute,second);
                    lcd_clear();
                    lcd_puts("* Anjam Shod *");
                    delay_ms(150);
                    goto exit;
                }
                else goto loop;
                break;
                case 1:
                break;
            }
        }
    if(cancel==chr_in){delay_ms(100);lcd_clear();lcd_puts("* Cancel Shod *");delay_ms(150);goto exit;}
    lcd_gotoxy(0,1);
    fn_displaytime(hour,minute,second);
    delay_ms(50);
    lcd_gotoxy(i,1);
    lcd_puts("__");
    delay_ms(50);
    }
exit:
lcd_clear();
}

//  function settings date , slt = 0 set date to ds1307 , slt = 1 set date to memory

void fn_setdate(char slt)
{
char i =0;
while((PINA & readPINA)!=read)
rtc_get_date(&week,&day,&mon,&year);
loop:
lcd_clear();
lcd_gotoxy(0,0);
lcd_puts("Set Date :");
    while(1)
    {
    chr_in = PINA & readPINA;
    switch(i)
        {
          case 0:
          if(up==chr_in){delay_ms(100);year++;if(year<10)year=50;}
          if(down==chr_in){delay_ms(100);year--;if(year>50)year=10;}
          break;
          case 3:
          if(up==chr_in)
            {
                delay_ms(100);mon++;if(mon>12 & mon<14)mon=1;
                if(mon==4 | mon==6 | mon==9 | mon==11){if(day==31)day=30;}
                if(mon==1 | mon==3 | mon==5 | mon==7 | mon==8 | mon==10 | mon==12){if(day==32)day=1;}
                if(mon==2){if(day==29)day=1;}
            }
                if(down==chr_in)
                {
                    delay_ms(100);mon--;if(mon>14)mon=12;
                    if(mon==4 | mon==6 | mon==9 | mon==11){if(day==31)day=30;}
                    if(mon==1 | mon==3 | mon==5 | mon==7 | mon==8 | mon==10 | mon==12){if(day==32)day=1;}
                    if(mon==2){if(day==29)day=1;}
                }
          break;
          case 6:
          if(up==chr_in)
          {
            delay_ms(100);
            day++;
            if(mon==4 | mon==6 | mon==9 | mon==11){if(day==31)day=1;}
            if(mon==1 | mon==3 | mon==5 | mon==7 | mon==8 | mon==10 | mon==12){if(day==32)day=1;}
            if(mon==2){if(day==29)day=1;}
          }
          if(down==chr_in)
          {
          delay_ms(100);
            day--;
            if(mon==4 | mon==6 | mon==9 | mon==11){if(day==0)day=30;}
            if(mon==1 | mon==3 | mon==5 | mon==7 | mon==8 | mon==10 | mon==12){if(day==0)day=31;}
            if(mon==2){if(day==0)day=28;}
          }
          break;
          case 9:
          if(up==chr_in)
          {
            delay_ms(100);
            week++;
            if(week>7)week=1;
          }
          if(down==chr_in)
          {
            delay_ms(100);
            week--;
            if(week==0)week=7;
          }
          break;
        }
    if(ok==chr_in)
        {
        delay_ms(300);
        i=i+3;if(i>9)i=0;
        switch(slt)
            {
                case 0:
                if((chr_rtn=fn_del5())==1)
                {
                    rtc_set_date(week,day,mon,year);
                    lcd_clear();
                    lcd_puts("* Anjam Shod *");
                    delay_ms(150);
                    goto exit;
                }
                else goto loop;
                break;
                case 1:
                break;
            }
        }
    if(cancel==chr_in){delay_ms(100);lcd_clear();lcd_puts("* Cancel Shod *");delay_ms(150);goto exit;}
    lcd_gotoxy(0,1);
    fn_displaydate(year,mon,day,week);
    delay_ms(50);
    lcd_gotoxy(i,1);lcd_puts("__");
    if(i==9){lcd_gotoxy(9,1);lcd_puts("    ___"); }
    delay_ms(50);
    }

exit:
lcd_clear();
}

//******    Time
void fn_time()
{
        lcd_gotoxy(0,0);
        lcd_puts("T :");
        lcd_gotoxy(3,0);
        rtc_get_time(&hour,&minute,&second);
        if(hour>60 | minute>60 | second>60)rtc_set_time(12,1,1);
        fn_displaytime(hour,minute,second);
}
//******    Date
void fn_date()
{
lcd_gotoxy(0,1);
lcd_puts("D :");
lcd_gotoxy(3,1);
rtc_get_date(&week,&day,&mon,&year);
fn_displaydate(year,mon,day,week);
}
//*******    function display
void fn_displaytime (char h,char m,char s)
    {
        sprintf(lcd_buff,"%u:%u:%u",h,m,s);
        if (h < 10) sprintf(lcd_buff,"0%u:%u:%u",h,m,s);
        if (m < 10) sprintf(lcd_buff,"%u:0%u:%u",h,m,s);
        if (s < 10) sprintf(lcd_buff,"%u:%u:0%u",h,m,s);
        if (h < 10 && m < 10) sprintf(lcd_buff,"0%u:0%u:%u",h,m,s);
        if (h < 10 && s < 10) sprintf(lcd_buff,"0%u:%u:0%u",h,m,s);
        if (m < 10 && s <10) sprintf(lcd_buff,"%u:0%u:0%u",h,m,s);
        if (h < 10 && m <10 && s <10) sprintf(lcd_buff,"0%u:0%u:0%u",h,m,s);
        lcd_puts(lcd_buff);
    }

void fn_displaydate (char y,char m,char d,char w)
    {
    char weeks[];
    sprintf(lcd_buff,"%u/%u/%u",y,m,d);
    if(m<10) sprintf(lcd_buff,"%u/0%u/%u",y,m,d);
    if(d<10) sprintf(lcd_buff,"%u/%u/0%u",y,m,d);
    if(m<10 && d<10) sprintf(lcd_buff,"%u/0%u/0%u",y,m,d);
        switch (w)
        {
            case 1:sprintf(weeks,"SUN");
            break;
            case 2:sprintf(weeks,"MON");
            break;
            case 3:sprintf(weeks,"TUE");
            break;
            case 4:sprintf(weeks,"WED");
            break;
            case 5:sprintf(weeks,"THU");
            break;
            case 6:sprintf(weeks,"FRI");
            break;
            case 7:sprintf(weeks,"SAT");
            break;
        };
        lcd_puts(lcd_buff);
        lcd_gotoxy(13,1);
        lcd_puts(weeks);
    }
//*************************
//  function delay 5 sec
char fn_del5()
{
    char i, rtn=0,chr_in;
    lcd_clear();
    for(i=0;i<chrDel5;i++)
    {
        chr_in = PINA & readPINA;
        delay_ms(valDel);
        if(i!=0)
            {
                lcd_gotoxy(0,0);
                lcd_puts("delay 5 sec :");
                lcd_gotoxy(0,1);
                sprintf(lcd_buff,"%u",i);
                lcd_puts(lcd_buff);
            }
        if(chr_in == read) {rtn=0;lcd_clear();goto next;}
        if(i>10) {rtn=1;goto next;}
    }
next:
return rtn;
}
//*************************
//  function manual wather pomp on         ******   ok    *****
void fn_mnl(char chr_mnl)
{   char i;
    lcd_clear();
    lcd_puts("manual pmop on");
    if(chr_mnl==7)R1=1;
    if(chr_mnl==8)R2=1;
    if(chr_mnl==9)R3=1;
    if(chr_mnl==10)R4=1;

//  start delay 10 sec
    for(i=0;i<50;i++)
    {
        if((PINA & readPINA)==read) goto exit;
        delay_ms(20);
    }
//  end delay 10 sec
    lcd_gotoxy(0,1);
    i=chr_mnl-6;
    sprintf(lcd_buff,"num out = %u",i);
    lcd_puts(lcd_buff);
    while((PINA & readPINA)!=15) pomp=1;
exit:
    R1 = R2 = R3 = R4= pomp = 0;
    lcd_clear();
}


//  function daily

void fn_daily(){}

//  function weekly

void fn_weekly(){}

//  function monthly

void fn_monthly(){}