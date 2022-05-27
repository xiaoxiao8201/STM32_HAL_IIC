#include "mpu6050.h"

//iic读一个寄存器内的值
uint8_t C_MPU_read_byte(iic_class *pin, uint8_t reg){
  uint8_t res;
  C_iic_start(pin);
  C_iic_write_byte(pin,(MPU_ADDR<<1)|0);
  C_iic_wait_ack(pin,1);
  C_iic_write_byte(pin,reg);
  C_iic_wait_ack(pin,1);
  C_iic_start(pin);
  C_iic_write_byte(pin,(MPU_ADDR<<1)|1);
  C_iic_wait_ack(pin,1);
  res = C_iic_read_byte(pin,0);
  C_iic_stop(pin);
  return res;
}
//iic读一个寄存器内的值
uint8_t C_MPU_write_byte(iic_class *pin,uint8_t reg, uint8_t data)
{
  C_iic_start(pin);
  C_iic_write_byte(pin,(MPU_ADDR<<1)|0);
  if(C_iic_wait_ack(pin,1)){
    C_iic_stop(pin);
    return 1;
  }
  C_iic_write_byte(pin,reg);
  C_iic_wait_ack(pin,1);
  C_iic_write_byte(pin,data);
  if(C_iic_wait_ack(pin,1)){
    C_iic_stop(pin);
    return 1;
  }
  C_iic_stop(pin);
  return 0;
}

//iic读连续寄存器内的值
uint8_t C_MPU_read_n_byte(iic_class *pin,uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buffer)
{
  
  C_iic_start(pin);
  C_iic_write_byte(pin,(addr<<1)|0);
  if(C_iic_wait_ack(pin,1))
  {
    C_iic_stop(pin);
    return 1;
  }
  C_iic_write_byte(pin,reg);
  C_iic_wait_ack(pin,1);
  C_iic_start(pin);
  C_iic_write_byte(pin,(addr<<1)|1);
  C_iic_wait_ack(pin,1);
  while(len){
    if(len==1){
      *buffer=C_iic_read_byte(pin,0);
    }else{
      *buffer=C_iic_read_byte(pin,1);
    }
    len--;
    buffer++;
  }
  C_iic_stop(pin);
  return 0;
}

//iic写连续寄存器内的值
uint8_t C_MPU_write_n_byte(iic_class *pin,uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buffer)
{
  
  C_iic_start(pin);
  C_iic_write_byte(pin,(addr<<1)|0);
  if(C_iic_wait_ack(pin,1))
  {
    C_iic_stop(pin);
    return 1;
  }
  C_iic_write_byte(pin,reg);
  C_iic_wait_ack(pin,1);
  for(uint8_t i = 0;i<len;i++){
    C_iic_write_byte(pin,buffer[i]);
    if(C_iic_wait_ack(pin,1)){
      C_iic_stop(pin);
      return 1;
    }
  }
  C_iic_stop(pin);
  return 0;
}

//获得原始加速度值
uint8_t C_MPU_get_accelerometer(iic_class *pin,short *ax, short *ay, short *az)
{
  uint8_t buf[8],res;
  res = C_MPU_read_n_byte(pin,MPU_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);
  if(!res)
  {
    *ax = ((uint16_t)buf[0]<<8)|buf[1];
    *ay = ((uint16_t)buf[2]<<8)|buf[3];
    *az = ((uint16_t)buf[4]<<8)|buf[5];
  }
  return res;
}
//获取陀螺仪的值
uint8_t C_MPU_get_gyroscope(iic_class *pin,short *gx, short *gy, short *gz)
{
  uint8_t buf[8],res;
  res = C_MPU_read_n_byte(pin,MPU_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
  if(!res)
  {
    *gx = ((uint16_t)buf[0]<<8)|buf[1];
    *gy = ((uint16_t)buf[2]<<8)|buf[3];
    *gz = ((uint16_t)buf[4]<<8)|buf[5];
  }
  return res;
}

//获取温度（此时返回的温度被*100）
short C_MPU_get_temperature(iic_class *pin)
{
    uint8_t buf[2]; 
    short raw;
    float temp;
    C_MPU_read_n_byte(pin,MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((uint16_t)buf[0]<<8)|buf[1];  
    temp=36.53+((double)raw)/340;  
    return temp*100;;
}
//设置mpu6050数字低通滤波器
uint8_t C_MPU_set_lpf(iic_class *pin,uint16_t lpf)
{
	uint8_t data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return C_MPU_write_byte(pin,MPU_CFG_REG,data);
}

//设置采样率
//rate:4~1000(Hz)
uint8_t C_MPU_set_rate(iic_class *pin,uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=C_MPU_write_byte(pin,MPU_SAMPLE_RATE_REG,data);
 	return C_MPU_set_lpf(pin,rate/2);
}
//设置陀螺仪满量程范围
//fsr:0,+-250dps;1,+-500dps;2,+-1000dps;3,+-2000dps
uint8_t C_MPU_set_gyro_fsr(iic_class *pin,uint8_t fsr)
{
	return C_MPU_write_byte(pin,MPU_GYRO_CFG_REG,fsr<<3);
}
//设置加速度范围
//fsr:0,+-2g;1,+-4g;2,+-8g;3,+-16g
uint8_t C_MPU_set_accel_fsr(iic_class *pin,uint8_t fsr)
{
	return C_MPU_write_byte(pin,MPU_ACCEL_CFG_REG,fsr<<3);
}
uint8_t C_MPU_Init(iic_class *pin)
{
  uint8_t res; 
  C_MPU_write_byte(pin,MPU_PWR_MGMT1_REG,0X80);
  HAL_Delay(100);
  C_MPU_write_byte(pin,MPU_PWR_MGMT1_REG,0X00);
  C_MPU_set_gyro_fsr(pin,3);					
  C_MPU_set_accel_fsr(pin,0);					
  C_MPU_set_rate(pin,50);						
  C_MPU_write_byte(pin,MPU_INT_EN_REG,0X00);
  C_MPU_write_byte(pin,MPU_USER_CTRL_REG,0X00);
  C_MPU_write_byte(pin,MPU_FIFO_EN_REG,0X00);
  C_MPU_write_byte(pin,MPU_INTBP_CFG_REG,0X80);
  res=C_MPU_read_byte(pin,MPU_DEVICE_ID_REG); 
  if(res==MPU_ADDR)
  {
    C_MPU_write_byte(pin,MPU_PWR_MGMT1_REG,0X01);
    C_MPU_write_byte(pin,MPU_PWR_MGMT2_REG,0X00);
    C_MPU_set_rate(pin,50);
  }else return 1;
  return 0;
}