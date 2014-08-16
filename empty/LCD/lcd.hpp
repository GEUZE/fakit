#include <sciopta++>

#ifndef LCD_HPP__
#define LCD_HPP__
namespace LCD{

    unsigned int const RED      = 17;   // Port numbers on the LPC1788 
    unsigned int const GREEN    = 18;
    unsigned int const BLUE     = 19;
    unsigned int const RS       = 15;
    unsigned int const E        = 16;
    unsigned int const DB4      = 24;
    unsigned int const DB5      = 25;
    unsigned int const DB6      = 26;
    unsigned int const DB7      = 27;
    void E_pulse(void);
    void Set_data(uint8_t);
    void Send_to_lcd(uint8_t,uint8_t);
    void Lcd_init(void);
    void Send_lcd_text(int line, const char* str);
    void lcdRGB(uint8_t rgb);
    inline void wait(unsigned int d);
    char lcd_text[2][16];
} 
#endif
