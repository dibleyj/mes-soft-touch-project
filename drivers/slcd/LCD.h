#pragma once

  class SLCD {
  public:
	    //! Construct an SLCD object
	    SLCD()
        {
            init();
            CharPosition = 0;
        }

        ~SLCD();

	    //! Move the SLCD cursor to the first character
	    void Home();

	    // Write array of ASCII codes, Size is limited display length
	    void Write_Msg(uint8_t *Message, uint8_t Size);

	    // Write a c-style string
	    void Write_Msg(const char *Message);

	    /**
	    * Set contrast
	    * @param lbContrast 0 - 15, 0 lightest, 15 darkest
	    */
	    void Contrast (uint8_t lbContrast);
	    void All_Segments (int);
	    //! Turn off all segments
	    void clear();
	    /**
	    * Turn a decimal point on or off
	    * @param pos decimal point position: 0-2
	    * @param on True or False
	    */
	    void DP(int pos, bool on);
	    void DP1 (int);
	    void DP2 (int);
	    void DP3 (int);
	    //! Turn the colon symbol on or off
	    void Colon (bool on);
	    //! Current cursor position
	    uint8_t CharPosition;
	    void blink(int blink = 3);
	    void deepsleepEnable(bool enable);

	private:
	    void init();
	    // Write char to next display element
	    void Write_Char(char lbValue);
	    virtual int _putc(int c);
	  static const uint32_t MASK_BIT[32];

  };