#ifndef C_IIC_H
#define C_IIC_H
//9:35
#define SDA_IN 1
#define SDA_OUT 0
#define IICDELAY 15
#define IICWAIT 250
typedef enum
{
  IIC_OK,      
  IIC_ERROR
} IIC_State;
//重构            2022.5.8

typedef struct
{
  //SCL
  GPIO_TypeDef *SCL;
  uint16_t scl_pin;
  //SDA
  GPIO_TypeDef *SDA; 
  uint16_t sda_pin;
}iic_class;

void C_scl_h(iic_class *pin);
//拉低SCL;
void C_scl_l(iic_class *pin);
//拉高SDA;
void C_sda_h(iic_class *pin);
//拉低SDA;
void C_sda_l(iic_class *pin);
//等待应答位
IIC_State C_iic_wait_ack(iic_class *pin, uint8_t flag);
IIC_State C_iic_write_byte(iic_class *pin,uint8_t data);
void C_iic_delay(uint16_t t);
void C_iic_start(iic_class *pin);
void C_iic_stop(iic_class *pin);
void C_change_sda(iic_class *pin, uint8_t flag);
void C_iic_send_ack(iic_class *pin);
void C_iic_send_nack(iic_class *pin);
uint8_t C_iic_read_byte(iic_class *pin, uint8_t ack);
#endif