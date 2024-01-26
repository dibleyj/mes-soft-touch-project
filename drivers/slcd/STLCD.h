/**
 * STLCD.h - Soft Touch LCD wrapper class
 */
#pragma once
#include "soft_touch.h"
#include "LCD.h"

class STLCD : public SLCD 
{
public:
    static STLCD & 
    instance()
    {
        static STLCD m_instance;
        return m_instance;
    }

    

    ~STLCD() {}

    enum Position
    {
        DIGIT_0,
        DIGIT_1,
        DIGIT_2,
        DIGIT_3,
        LH = 0,
        RH = 2
    };

    void 
    ClearDigit(Position pos)
    {
        CharPosition = static_cast<uint8_t>(pos);
        SLCD::Write_Msg(" ");
    }

    void
    ClearDigitPair(Position pos)
    {
        CharPosition = static_cast<uint8_t>(pos);
        Write_Msg("  ");
    }

    void 
    WriteUint8H(Position pos, uint8_t val)
    {
        CharPosition = static_cast<uint8_t>(pos);
        uint8_t field[2]{' ',' '};
        UtilResult res = UtilUint8ToHexDigits(val, field);
        if (res == OK) Write_Msg(field, 2);
    }

    void ProcessQueuedEvents(STEventSource src, STEvent event, uint32_t value)
    {

    }

    private:

    STLCD() : SLCD(), lcd_queue() {}

    EventQueue lcd_queue; 

    enum UtilResult
    {
        OK,
        NOK
    };

    UtilResult UtilIntNibbleToHexChar(uint8_t intVal, uint8_t *pChar)
    {
        if ( intVal <= 9u )
        {
            *pChar = intVal + '0';
        }
        else if ( ( 10u <= intVal ) && ( intVal <= 15u ) )
        {
            *pChar = intVal - 10u + 'A';
        }
        else 
        {
            return NOK;
        }
        return OK;
    }

    UtilResult UtilUint8ToHexDigits(uint8_t intVal, uint8_t *field)
    {
        uint8_t res{0};
        res  = UtilIntNibbleToHexChar((intVal >> 4), &(field[0]));
        res += UtilIntNibbleToHexChar((intVal & 0xF), &(field[1]));
        return static_cast<UtilResult>(res);
    }
};