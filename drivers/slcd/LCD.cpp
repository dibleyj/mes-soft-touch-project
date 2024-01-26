/*
 * LCD.cpp
 *
 *  Created on: 11 Jul 2016
 *      Author: dl
 */

#include "MKL46Z4.h"
#include "LCD.h"
#include "LCDConfig.h"
#include "stdio.h"



void SLCD::init()
{
    SIM->SCGC5 |= SIM_SCGC5_SLCD_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;

    // configure pins for LCD operation
    PORTC->PCR[20] = 0x00000000;     //VLL2
    PORTC->PCR[21] = 0x00000000;     //VLL1
    PORTC->PCR[22] = 0x00000000;     //VCAP2
    PORTC->PCR[23] = 0x00000000;     //VCAP1
    // Enable IRCLK
    MCG->C1  |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK;
    MCG->C2  |= MCG_C2_IRCS_MASK ;  //0 32KHZ internal reference clock; 1= 4MHz irc

    //Check if the Fast reference has its divide by 2 enabled (default):
    if ((MCG->SC & MCG_SC_FCRDIV_MASK) != 1<<1)
      printf("Invalid clock configuration for SLCD\n");
    LCD->GCR = 0x0;
    LCD->AR  = 0x0;

    // LCD configurartion
    LCD->GCR =   ( LCD_GCR_RVEN_MASK*_LCDRVEN
                   | LCD_GCR_RVTRIM(_LCDRVTRIM)         //0-15
                   | LCD_GCR_CPSEL_MASK*_LCDCPSEL
                   | LCD_GCR_LADJ(_LCDLOADADJUST)       //0-3
                   | LCD_GCR_VSUPPLY_MASK*_LCDSUPPLY    //0-1
                   |!LCD_GCR_FDCIEN_MASK
                   | LCD_GCR_ALTDIV(1)         //divide by something
                   |!LCD_GCR_LCDDOZE_MASK
                   |!LCD_GCR_LCDSTP_MASK
                   |!LCD_GCR_LCDEN_MASK                 //WILL BE ENABLE ON SUBSEQUENT STEP
                   | LCD_GCR_SOURCE_MASK*1
                   | LCD_GCR_ALTSOURCE_MASK*0
                   | LCD_GCR_LCLK(0)    //0-7
                   | LCD_GCR_DUTY(_LCDDUTY)   //0-7
                 );
    uint8_t i;
    uint32_t *p_pen;
    uint8_t pen_offset;   // 0 or 1
    uint8_t pen_bit;      // 0 to 31
    LCD->PEN[0] = 0x0;
    LCD->PEN[1] = 0x0;
    LCD->BPEN[0] = 0x0;
    LCD->BPEN[1] = 0x0;
    p_pen = (uint32_t *)&LCD->PEN[0];
    for (i=0; i<_LCDUSEDPINS; i++) {
        pen_offset = WF_ORDERING_TABLE[i]/32;
        pen_bit    = WF_ORDERING_TABLE[i]%32;
        p_pen[pen_offset] |= 1 << pen_bit;
        if (i>= _LCDFRONTPLANES) {  // Pin is a backplane
            p_pen[pen_offset+2] |= 1 << pen_bit;  // Enable  BPEN
            LCD->WF8B[(uint8_t)WF_ORDERING_TABLE[i]] = 1 << (i - _LCDFRONTPLANES);   // fill with 0x01, 0x02, etc
        }
    }
    LCD->GCR |= LCD_GCR_LCDEN_MASK;
}

void SLCD::Write_Msg(uint8_t *Message, uint8_t Size) {
	// limit message to LCD size
	if (Size > _CHARNUM)
		Size = _CHARNUM;

	//write to LCD until no more chars
	do {
		Write_Char(*Message);
		Message++;
	} while (--Size);

}

/*
 Write_Msg.  Display a Message starting at the fisrt character of display
 until _CHARNUM  or End of string.
 */

void SLCD::Write_Msg(const char *Message) {

	uint8_t Size = 0;
	CharPosition = 0;  //Home display
	while (Size < _CHARNUM && *Message) {
		Write_Char(*Message++);
		Size++;
	}

	if (Size < _CHARNUM) {

		while (Size++ < _CHARNUM)
			Write_Char(BLANK_CHARACTER);  // complete data with blanks
	}

}

int SLCD::_putc(int c)
{
    Write_Char(c);
    return 0;
}

void SLCD::Write_Char (char lbValue)
{
    if (CharPosition >= _CHARNUM)
        CharPosition = 0;

    if (lbValue == '.') {
        // Use built-in dot
        DP(CharPosition-1, true);
        return;
    }

    uint8_t char_val;
    uint8_t temp;
    uint8_t *lbpLCDWF;
    uint8_t lbCounter;
    uint16_t arrayOffset;
    uint8_t position;

    lbpLCDWF = (uint8_t *)&LCD->WF8B[0];
    /* only ascii character if value not writeable write as @ */
    if (lbValue>='a' && lbValue<='z') {
        lbValue -= 32; // UpperCase
    }
    if (lbValue<ASCCI_TABLE_START || lbValue >ASCCI_TABLE_END) {
        lbValue = BLANK_CHARACTER;  // default value as space
    }
    lbValue -=ASCCI_TABLE_START;        // Remove the offset to search in the ascci table
    arrayOffset = (lbValue * _CHAR_SIZE); // Compensate matrix offset
    // ensure bLCD position is in valid limit
    lbCounter = 0;  //number of writings to complete one char
    while (lbCounter<_CHAR_SIZE) {
        position = (CharPosition) *_LCDTYPE + lbCounter;
        temp=0;
        if (lbCounter==1) {
            temp = lbpLCDWF[WF_ORDERING_TABLE[position]] & 0x01;//bit 0 has the special symbol information
        }
        char_val = ASCII_TO_WF_CODIFICATION_TABLE[arrayOffset + lbCounter];
        lbpLCDWF[WF_ORDERING_TABLE[position]] = char_val | temp;
        //  if (char_val==0) lbCounter = _CHAR_SIZE; //end of this character
        lbCounter++;
    }
    CharPosition++;
}

void SLCD::Home()
{
    CharPosition =  0;
}

void SLCD::Contrast (uint8_t lbContrast)
{
    lbContrast &= 0x0F;              //Forced to the only values accepted
    LCD->GCR |= LCD_GCR_RVTRIM(lbContrast);
}

void SLCD::clear()
{
    All_Segments(0);
}

void SLCD::All_Segments (int mode)
{
    uint8_t lbTotalBytes = _CHARNUM * _LCDTYPE;
    uint8_t lbCounter=0;
    uint8_t *lbpLCDWF;

    lbpLCDWF = (uint8_t *)&LCD->WF8B[0];
    while (lbCounter < lbTotalBytes) {
        if (mode==1) {
            lbpLCDWF[(uint8_t)WF_ORDERING_TABLE[lbCounter++]]=_ALLON;
        } else {
            lbpLCDWF[WF_ORDERING_TABLE[lbCounter++]]=0;
        }
    }
}

void SLCD::DP(int pos, bool on)
{
    uint8_t *lbpLCDWF;
    int tableLoc;
    switch (pos) {
        case 0:
            tableLoc = 1;
            break;
        case 1:
            tableLoc = 3;
            break;
        case 2:
            tableLoc = 5;
            break;
        default:
            return; // Bad position
    }
    lbpLCDWF = (uint8_t *)&LCD->WF8B[0];
    if (on) {
        lbpLCDWF[(uint8_t)WF_ORDERING_TABLE[tableLoc]]|=1;
    } else {
        lbpLCDWF[(uint8_t)WF_ORDERING_TABLE[tableLoc]]&=~1;
    }
}

void SLCD::DP1 (int mode)
{
    DP(0, mode==1);
}

void SLCD::DP2 (int mode)
{
    DP(1, mode==1);
}

void SLCD::DP3 (int mode)
{
    DP(2, mode==1);
}

void SLCD::Colon (bool on)
{
    uint8_t *lbpLCDWF;
    lbpLCDWF = (uint8_t *)&LCD->WF8B[0];
    if (on) {
        lbpLCDWF[(uint8_t)WF_ORDERING_TABLE[7]]|=1;
    } else {
        lbpLCDWF[(uint8_t)WF_ORDERING_TABLE[7]]&=~1;
    }
}

void SLCD::blink(int blink)
{
    if (( blink > 7) || (blink < 0))
        LCD->AR &= ~LCD_AR_BLINK_MASK;
    else
        LCD->AR |= LCD_AR_BLINK_MASK | blink;
}

void SLCD::deepsleepEnable(bool enable)
{
    MCG->C1 &= ~MCG_C1_IREFSTEN_MASK;
    MCG->C1 |= enable << MCG_C1_IREFSTEN_SHIFT;
    LCD->GCR &= ~LCD_GCR_LCDSTP_MASK;
    LCD->GCR |= (!enable) << LCD_GCR_LCDSTP_SHIFT;
}


const uint32_t SLCD::MASK_BIT[32] = { 0x00000001, 0x00000002, 0x00000004, 0x00000008,
		0x00000010, 0x00000020, 0x00000040, 0x00000080, 0x00000100, 0x00000200,
		0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
		0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000,
		0x00400000, 0x00800000, 0x01000000, 0x02000000, 0x04000000, 0x08000000,
		0x10000000, 0x20000000, 0x40000000, 0x80000000, };

