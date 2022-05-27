#include "main.h"
#include "iic.h"
extern TIM_HandleTypeDef htim1;
TIM_HandleTypeDef *htim = &htim1;

void C_scl_h(iic_class *pin){
  HAL_GPIO_WritePin(pin->SCL,pin->scl_pin,1);
}
void C_scl_l(iic_class *pin){
  HAL_GPIO_WritePin(pin->SCL,pin->scl_pin,0);
}
void C_sda_h(iic_class *pin){
  HAL_GPIO_WritePin(pin->SDA,pin->sda_pin,1);
}
void C_sda_l(iic_class *pin){
  HAL_GPIO_WritePin(pin->SDA,pin->sda_pin,0);
}
void C_change_sda(iic_class *pin, uint8_t flag){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(flag==SDA_IN){
    GPIO_InitStruct.Pin = pin->sda_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(pin->SDA, &GPIO_InitStruct);
  }else if(flag==SDA_OUT){
    GPIO_InitStruct.Pin = pin->sda_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(pin->SDA, &GPIO_InitStruct);
  }
}
void C_iic_delay(uint16_t t){
  uint16_t counter = 0;
  __HAL_TIM_SET_AUTORELOAD(htim, t); // 设置定时器自动加载值
  __HAL_TIM_SET_COUNTER(htim, counter); // 设置定时器初始值
  HAL_TIM_Base_Start(htim); // 启动定时器
  while(counter != t) // 直到定时器计数从 0 计数到 t 结束循环,刚好 t us
  {
    counter = __HAL_TIM_GET_COUNTER(htim); // 获取定时器当前计数
  }
  HAL_TIM_Base_Stop(htim); // 停止定时器
}

void C_iic_start(iic_class *pin){
  C_change_sda(pin, SDA_OUT);
  C_sda_h(pin);
  C_scl_h(pin);
  C_iic_delay(IICDELAY);
  C_sda_l(pin);
  C_iic_delay(IICDELAY);
  C_scl_l(pin);
}
void C_iic_stop(iic_class *pin){
  C_change_sda(pin, SDA_OUT);
  C_scl_l(pin);
  C_sda_l(pin);
  C_iic_delay(IICDELAY);
  C_scl_h(pin);
  C_sda_h(pin);
  C_iic_delay(IICDELAY);
}
//iic等待从机应答ack，若flag为0则忽略应答位
IIC_State C_iic_wait_ack(iic_class *pin, uint8_t flag){
  if(flag)
  {
    uint16_t time_out = 0;
    C_change_sda(pin, SDA_IN);
    C_sda_h(pin);
    C_iic_delay(IICDELAY);
    C_scl_h(pin);
    C_iic_delay(IICDELAY);
    while(HAL_GPIO_ReadPin(pin->SDA, pin->sda_pin)){
      if(++time_out>IICWAIT){
        C_iic_stop(pin);
        return IIC_ERROR;
      }
    }
    C_scl_l(pin);
    return IIC_OK;
  }
  else
  {
    C_iic_delay(IICDELAY);
    C_scl_h(pin);
    C_iic_delay(IICDELAY);
    C_scl_l(pin);
    C_iic_delay(IICDELAY);
    return IIC_OK;
  }
}

IIC_State C_iic_write_byte(iic_class *pin,uint8_t data){
  uint8_t d = data;
  uint8_t i = 0;
  C_scl_l(pin);
  C_change_sda(pin, SDA_OUT);
  for(;i<8;++i){
    if(d&0x80){
      C_sda_h(pin);
    }else{
      C_sda_l(pin);
    }
    d = d << 1;
    C_iic_delay(IICDELAY);
    C_scl_h(pin);
    C_iic_delay(IICDELAY);
    C_scl_l(pin);
  }
}

void C_iic_send_ack(iic_class *pin){
  C_scl_l(pin);
  C_change_sda(pin, SDA_OUT);
  C_sda_l(pin);
  C_iic_delay(IICDELAY);
  C_scl_h(pin);
  C_iic_delay(IICDELAY);
  C_scl_l(pin);
}
//发送nack
void C_iic_send_nack(iic_class *pin){
  C_scl_l(pin);
  C_change_sda(pin, SDA_OUT);
  C_sda_h(pin);
  C_iic_delay(IICDELAY);
  C_scl_h(pin);
  C_iic_delay(IICDELAY);
  C_scl_l(pin);
}
uint8_t C_iic_read_byte(iic_class *pin, uint8_t ack){
  uint8_t data=0;
  C_change_sda(pin, SDA_IN);
  for(uint8_t i = 0;i<8;++i){
    C_scl_l(pin);
    C_iic_delay(IICDELAY);
    C_scl_h(pin);
    data = data<<1;
    if(GPIO_PIN_SET==HAL_GPIO_ReadPin(pin->SDA, pin->sda_pin)){
      data++;
    }
    C_iic_delay(IICDELAY);
  }
  if(ack)
  {
    C_iic_send_ack(pin);
  }else{
    C_iic_send_nack(pin);
  }
  return data;
}
