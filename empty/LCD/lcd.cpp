#include <sciopta++>
#include <LPC177x_8x.h>
#include <stdio.h>
#include "../datamanager/datamanager.hpp"
#include "lcd.hpp"
#include "../../protocol/protocol.hpp"
#include "../can/can.hpp"

#define LCD_COMMAND 0
#define LCD_CHAR 1

#define CLR_DISP		0x01
#define DISP			0x08
#define DISP_ON			0x0C
#define DISP_CUR		0x0E
#define DISP_BINK_CUR	        0x0F
#define	DISP_OFF		DISP

#define	CUR_HOME		0x02
#define	CUR_OFF			DISP_ON
#define	CUR_UNDER		DISP_CUR
#define	CUR_BLINK		DISP_BINK_CUR
#define CUR_LEFT		0x10
#define	CUR_RIGHT		0x14

#define	FIRST_LINE		0x80
#define	SECOND_LINE		0xC0

namespace LCD{

    inline void wait(unsigned int d) {
        Ticks::sleep(Milliseconds(d));
}
    inline void waitMicro(unsigned int d){
        for( uint32_t volatile i=0; i<d*1000; ++i ) {
        }
    }

    void E_pulse(void){
            LPC_GPIO1->SET = (1<<E);
            waitMicro(1);
            LPC_GPIO1->CLR = (1<<E);
    }

    void Set_data(uint8_t data){
	if(data & 0x01)
		LPC_GPIO1->SET = (1<<DB4);
	else
		LPC_GPIO1->CLR = (1<<DB4);
	if(data & 0x02)
		LPC_GPIO1->SET = (1<<DB5);
	else
		LPC_GPIO1->CLR = (1<<DB5);
	if(data & 0x04)
		LPC_GPIO1->SET = (1<<DB6);
	else
		LPC_GPIO1->CLR = (1<<DB6);
	if(data & 0x08)
		LPC_GPIO1->SET = (1<<DB7);
	else
		LPC_GPIO1->CLR = (1<<DB7);
    }

    void Send_to_lcd(uint8_t data, uint8_t type){
        waitMicro(10);
        if(type)
            LPC_GPIO1->SET = (1<<RS);
        else
            LPC_GPIO1->CLR = (1<<RS);
        Set_data((data >> 4));
        E_pulse();
        Set_data((data & 0x0F));
        E_pulse(); 
    }

    void Lcd_init(void){

            LPC_GPIO1->DIR = 
                (1  <<  RS) |
                (1  <<  E)  |
                (1  <<  DB4)|
                (1  <<  DB5)|
                (1  <<  DB6)|
                (1  <<  DB7);

            LPC_GPIO1->CLR = (1<<RS);
            LPC_GPIO1->CLR = (1<<E);
            wait(110);
            LPC_GPIO1->SET = (1<<DB4);
            LPC_GPIO1->SET = (1<<DB5);
            E_pulse();
            wait(10);
            LPC_GPIO1->SET = (1<<DB4);
            LPC_GPIO1->SET = (1<<DB5);
            E_pulse();
            wait(10);
            LPC_GPIO1->SET = (1<<DB4);
            LPC_GPIO1->SET = (1<<DB5);
            E_pulse();
            wait(10);
            LPC_GPIO1->CLR = (1<<DB4);
            LPC_GPIO1->SET = (1<<DB5);
            E_pulse();


            Send_to_lcd(DISP_ON,LCD_COMMAND);
            Send_to_lcd(CLR_DISP,LCD_COMMAND);

    }

    void Send_lcd_text(int line, const char* str)
    {
        uint8_t i;
        if (line == 1){
            Send_to_lcd(FIRST_LINE,LCD_COMMAND);
            for(i=0; i < 16; i++)
                {
                   Send_to_lcd(str[i],LCD_CHAR);
                }
        }

        if (line == 2){
            Send_to_lcd(SECOND_LINE,LCD_COMMAND); 
            for(i=0; i < 16; i++)
                {
                    Send_to_lcd(str[i],LCD_CHAR);
                }
        }
    }

    void lcdRGB(uint8_t rgb){
        // Shift the color code to the desired ports
        // Invert the code, so HIGH will actually turn on a color
        uint32_t shiftrgb = ~(rgb << RED);
        LPC_GPIO_TypeDef * const GPIO = LPC_GPIO1;

        GPIO->DIR |= 
            (1 << RED)      |
            (1 << GREEN)    |
            (1 << BLUE)     ;
           
        GPIO->CLR = 
            (1 << RED)      |
            (1 << GREEN)    |
            (1 << BLUE)     ;

        // Sets the ports to display the desired color
        GPIO->SET = shiftrgb;
                   
        }

    void main() {

        Lcd_init();
        lcdRGB(7);
        Send_lcd_text(1,"WAITING...      ");
        Send_lcd_text(2,"<<<<<<<<>>>>>>>>");
        
        
        Message::send(Can::M::Listen(0, 0x20, 0x26, Process::current()), can);

        
        while (true) {
        
            Message * m = Message::receive();
            if (Can::M::Received * r = message_cast<Can::M::Received>(m)) {
                switch (r->can_id & 0xFF) {
                    case Protocol::CAN::Dashboard::LCD_top_left:
                        for (int i = 0; i < 8; i++){  
                            lcd_text[0][i] = r->data[i];
                        }
                        break;
                    case Protocol::CAN::Dashboard::LCD_top_right:
                        for (int i = 0; i < 8; i++){  
                            lcd_text[0][i+8] = r->data[i];
                        }
                            Send_lcd_text(1,lcd_text[0]);
                        break;
                    case Protocol::CAN::Dashboard::LCD_bottom_left:
                        for (int i = 0; i < 8; i++){  
                            lcd_text[1][i] = r->data[i];
                        }
                        break;
                    case Protocol::CAN::Dashboard::LCD_bottom_right:
                        for (int i = 0; i < 8; i++){  
                            lcd_text[1][i+8] = r->data[i];
                        }
                            Send_lcd_text(2,lcd_text[1]);
                        break;
                    case Protocol::CAN::Dashboard::LCD_color:
                        lcdRGB(r->data[0]); 
                        break;
                    case Protocol::CAN::Dashboard::XBee_send:
                        break;
                    case Protocol::CAN::Dashboard::XBee_receive:
                        break;
                }
                delete r;
            } 
           
        }
            
    }
	
}


Process lcd = Process::createStatic("LCD", LCD::main, 1024, 10);
