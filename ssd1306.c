#include "ssd1306.h"
#include "oledfont.h"
/************************************************************/
//以下是基本写数据函数
/*
写命令/数据流程：  先写地址0x78
                        |
             写地址0x00表示命令，0x40表示数据
                        |
                      写内容
*/
//ssd1306写一个命令
void C_iic_write_command(iic_class *pin, uint8_t command){
  C_iic_start(pin);
  C_iic_write_byte(pin,0x78);
  C_iic_wait_ack(pin,0);
  C_iic_write_byte(pin,0x00);
  C_iic_wait_ack(pin,0);
  C_iic_write_byte(pin,command);
  C_iic_wait_ack(pin,0);
  C_iic_stop(pin);
}
//ssd1306 写数据
void C_iic_write_data(iic_class *pin,uint8_t data){
  C_iic_start(pin);
  C_iic_write_byte(pin,0x78);
  C_iic_wait_ack(pin,0);
  C_iic_write_byte(pin,0x40);
  C_iic_wait_ack(pin,0);
  C_iic_write_byte(pin,data);
  C_iic_wait_ack(pin,0);
  C_iic_stop(pin);
}
//将上述两个函数简化统一，flag是命令/数据标志
void C_OLED_WR_Byte(iic_class *pin, uint8_t data, uint8_t flag){
  if(flag){
    C_iic_write_data(pin, data);
  }else{
    C_iic_write_command(pin, data);
  }
}
/**************************************************/
//完整写一个整屏的8位数字
void C_fill_picture(iic_class *pin,unsigned char fill_Data){
  unsigned char m,n;
  for(m=0;m<8;m++)
  {
    C_OLED_WR_Byte(pin, 0xb0+m, 0);		//page0-page1
    C_OLED_WR_Byte(pin, 0x00, 0);		//low column start address
    C_OLED_WR_Byte(pin, 0x1F, 0);		//high column start address
    for(n=0;n<128;n++)
    {
      C_OLED_WR_Byte(pin, fill_Data, 1);
    }
  }
}

//清除所有显示内容
void C_OLED_Clear(iic_class *pin){  
  uint8_t i,n;		    
  for(i=0;i<8;i++)  
  {  
    C_OLED_WR_Byte (pin,0xb0+i,OLED_CMD);
    C_OLED_WR_Byte (pin,0x00,OLED_CMD);
    C_OLED_WR_Byte (pin,0x10,OLED_CMD);
    for(n=0;n<128;n++)C_OLED_WR_Byte(pin, 0, OLED_DATA); 
  }
}
// 设置将要写的内存位置
void C_OLED_Set_Pos(iic_class *pin,unsigned char x, unsigned char y){ 	
  C_OLED_WR_Byte(pin, 0xb0+y,OLED_CMD);
  C_OLED_WR_Byte(pin, ((x&0xf0)>>4)|0x10,OLED_CMD);
  C_OLED_WR_Byte(pin, (x&0x0f),OLED_CMD); 
} 
//显示一个字符
/*
*x,y：位置坐标
*chr：字符编码
*char_size：字符大小，有6*8和16*8两个大小可以选择
*/
void C_OLED_ShowChar(iic_class *pin, uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size){      	
  unsigned char c=0,i=0;	
  c=chr-' ';			
  if(x>Max_Column-1){x=0;y=y+2;}
  if(Char_Size ==16)
  {
    C_OLED_Set_Pos(pin,x,y);	
    for(i=0;i<8;i++)
    C_OLED_WR_Byte(pin,F8X16[c*16+i],OLED_DATA);
    C_OLED_Set_Pos(pin,x,y+1);
    for(i=0;i<8;i++)
    C_OLED_WR_Byte(pin,F8X16[c*16+i+8],OLED_DATA);
  }
  else if(Char_Size==6){	
    C_OLED_Set_Pos(pin,x,y);
    for(i=0;i<6;i++)
      C_OLED_WR_Byte(pin,F6x8[c*6+i],OLED_DATA);
  }
}
//显示一个数字
/*
*x,y：位置坐标
*chr：字符编码
*/
void C_OLED_ShowNumber(iic_class *pin, uint8_t x,uint8_t y,uint8_t chr){      	
  unsigned char i=0;				
  if(x>Max_Column-1){x=0;y=y+2;}

  C_OLED_Set_Pos(pin,x,y);	
  for(i=0;i<8;i++)
  C_OLED_WR_Byte(pin,my_number[chr*16+i],OLED_DATA);
  C_OLED_Set_Pos(pin,x,y+1);
  for(i=0;i<8;i++)
  C_OLED_WR_Byte(pin,my_number[chr*16+i+8],OLED_DATA);
}

//显示一个字符串
/*
*x,y：位置坐标
*str：字符串指针
*/
void C_OLED_ShowString(iic_class *pin, uint8_t x,uint8_t y,uint8_t *str){
  for(uint8_t index = 0; str[index]!='\0';index++){
    uint8_t tempChar = str[index];
    C_OLED_ShowChar(pin,x+6*index,y,tempChar,6);
  
  }
}



void C_display_cccp(iic_class *pin){
  unsigned char m,n;
  for(m=0;m<8;m++)
  {
    C_OLED_WR_Byte(pin,0xb0+m,0);		//page0-page1
    C_OLED_WR_Byte(pin,0x00,0);		//low column start address
    C_OLED_WR_Byte(pin,0x1F,0);		//high column start address
    for(n=0;n<128;n++)
    {
      C_OLED_WR_Byte(pin,cccp_flag[m][n],1);
    }
  }
}
void C_showNumber(iic_class *pin, int number){
  for(uint8_t i = 0; i<10; i++){
    uint8_t temp = number % 10;
    number /= 10;
    C_OLED_ShowNumber(pin,8*(9-i),5,temp);
  }
  //C_OLED_ShowNumber(pin,8*10,5,10);
  //C_OLED_ShowNumber(pin,8*11,5,11);

}

/*
x = 0~127
y = 0~63
此操作会清除掉当前列
*/
void C_plotPoint(iic_class *pin, uint8_t x, uint8_t y){
  y = 63 - y;
  uint8_t page = y/8;
  uint8_t point = y%8;
  point = 1 << point;
  for(uint8_t i = 0;i<8;i++){
    C_OLED_Set_Pos(pin,x,i);
    if(page==i){
      C_OLED_WR_Byte(pin,point,OLED_DATA);
    }else{
      C_OLED_WR_Byte(pin,0x00,OLED_DATA);
    }
    
  }
}

			    
void C_OLED_Init(iic_class *pin){ 		 
  HAL_Delay(500);
  C_OLED_WR_Byte(pin,0xAE,OLED_CMD);//--display off
  C_OLED_WR_Byte(pin,0x00,OLED_CMD);//---set low column address
  C_OLED_WR_Byte(pin,0x10,OLED_CMD);//---set high column address
  C_OLED_WR_Byte(pin,0x40,OLED_CMD);//--set start line address  
  C_OLED_WR_Byte(pin,0xB0,OLED_CMD);//--set page address
  C_OLED_WR_Byte(pin,0x81,OLED_CMD); // contract control
  C_OLED_WR_Byte(pin,0xFF,OLED_CMD);//--128   
  C_OLED_WR_Byte(pin,0xA1,OLED_CMD);//set segment remap 
  C_OLED_WR_Byte(pin,0xA6,OLED_CMD);//--normal / reverse
  C_OLED_WR_Byte(pin,0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
  C_OLED_WR_Byte(pin,0x3F,OLED_CMD);//--1/32 duty
  C_OLED_WR_Byte(pin,0xC8,OLED_CMD);//Com scan direction
  C_OLED_WR_Byte(pin,0xD3,OLED_CMD);//-set display offset
  C_OLED_WR_Byte(pin,0x00,OLED_CMD);//
  C_OLED_WR_Byte(pin,0xD5,OLED_CMD);//set osc division
  C_OLED_WR_Byte(pin,0x80,OLED_CMD);//
  C_OLED_WR_Byte(pin,0xD8,OLED_CMD);//set area color mode off
  C_OLED_WR_Byte(pin,0x05,OLED_CMD);//
  C_OLED_WR_Byte(pin,0xD9,OLED_CMD);//Set Pre-Charge Period
  C_OLED_WR_Byte(pin,0xF1,OLED_CMD);//
  C_OLED_WR_Byte(pin,0xDA,OLED_CMD);//set com pin configuartion
  C_OLED_WR_Byte(pin,0x12,OLED_CMD);//
  C_OLED_WR_Byte(pin,0xDB,OLED_CMD);//set Vcomh
  C_OLED_WR_Byte(pin,0x30,OLED_CMD);//
  C_OLED_WR_Byte(pin,0x8D,OLED_CMD);//set charge pump enable
  C_OLED_WR_Byte(pin,0x14,OLED_CMD);//
  C_OLED_WR_Byte(pin,0xAF,OLED_CMD);//--turn on oled panel
}

