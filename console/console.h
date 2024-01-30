#pragma once
#include "mbed.h"
#include "soft_touch.h"

namespace soft_touch
{
 
    class Console 
    {
    public:
        static Console &
        instance(void)
        {
            static Console m_instance;
            return m_instance;
        }
       

        ~Console(void) {}

        Console(const Console&) = delete;
        void operator=(const Console&) = delete;

        void 
        Init(void)
        {            

            IoInit();
            IoSendLine("JD's Soft Touch running on bare-metal Mbed OS\n");
            IoSendString(prompt);

            mReceivedSoFar = 0u;

            for (int i = 0u; i < MAX_COMMAND_LENGTH; i++)
            {
                mReceiveBuffer[i] = nullchar;
            }
        }

        void 
        Process(void)
        {       
            const sCommandTable_T* commandTable;
            int32_t cmdEndLine{0}; 
            int32_t found{0};
            uint32_t cmdIndex{0};
            eCommandResult_T result;

            if (IO_SUCCESS == IoReceive() || mReceiveBufferNeedsChecking)
            {

                mReceiveBufferNeedsChecking = false;
                cmdEndLine = CommandEndLine(mReceiveBuffer, mReceivedSoFar);
                if (cmdEndLine >= 0)
                {
                    IoSendLine(NULL);
                    // printf("[ %s ]\n", mReceiveBuffer);
                    // we have complete string, find command
                    commandTable = CommandsGetTable();
                    cmdIndex = 0; 
                    found = NOT_FOUND;
                    while ( ( NULL != commandTable[cmdIndex].name ) && ( NOT_FOUND == found ) )
                    {
                        if ( CommandMatch(commandTable[cmdIndex].name, mReceiveBuffer) )
                        {
                            result = commandTable[cmdIndex].execute(mReceiveBuffer);
                            if ( COMMAND_SUCCESS != result )
                            {
                                IoSendString("Error: ");
                                IoSendLine(mReceiveBuffer);
                                IoSendString("Help: ");
                                IoSendLine(commandTable[cmdIndex].help);
                            }
                            found = cmdIndex;
                        }
                        else 
                        {
                            cmdIndex++;
                        }
                    }
                    if ((cmdEndLine != 0) && (NOT_FOUND == found))
                    {
                        if (mReceivedSoFar > 2)
                        {
                            IoSendLine("Command not found.");
                        }
                    }
                    // reset the buffer by moving over any leftovers and nulling the rest
                    // clear up to and including the found endline character
                    mReceivedSoFar = ResetBuffer(mReceiveBuffer, mReceivedSoFar, cmdEndLine + 1);
                    mReceiveBufferNeedsChecking = mReceivedSoFar > 0 ? true : false;
                    IoSendString(prompt);
                }
            }
        }

    private:
        static const char * prompt;
        static const char * lf;
        static const char * version;
        static const char param_sep;
        static const char nullchar;
        static const char cr_char;
        static const char nl_char;
        static constexpr int8_t NOT_FOUND{-1};
        static constexpr uint8_t int8_max_str_length{5};
        static constexpr uint8_t int32_max_str_length{16};
        static constexpr uint16_t MAX_LENGTH{256};
        static constexpr uint8_t MAX_COMMAND_LENGTH{10};
        static constexpr uint8_t MAX_HELPTEXT_LENGTH{64};

        BufferedSerial dev_uart;

        char mReceiveBuffer[MAX_LENGTH];

        uint16_t mReceivedSoFar{0};

        bool mReceiveBufferNeedsChecking{false};


        enum eCommandResult_T
        {
            COMMAND_SUCCESS = 0u,
            COMMAND_PARAMETER_ERROR = 0x10u,
            COMMAND_PARAMETER_END = 0x11u,
            COMMAND_ERROR = 0xFFu
        };

        typedef eCommandResult_T
        (*Command_T)(const char buffer[]);

        typedef struct sCommandStruct 
        {
            const char* name;
            Command_T execute;
            char help[MAX_HELPTEXT_LENGTH];
        } sCommandTable_T;

        static const sCommandTable_T mCommandTable[]; 
        
        Console(void) : 
            dev_uart(USBTX, USBRX)
        { } 

        enum eIoError
        {
            IO_SUCCESS = 0u,
            IO_ERROR
        };

        eIoError 
        IoInit(void)
        {
            dev_uart.set_baud(115200);
            dev_uart.set_format(8, BufferedSerial::None, 1);
            return IO_SUCCESS;
        }

        eIoError 
        IoReceive()
        {
            if (dev_uart.readable())
            {
                ssize_t rc = dev_uart.read(&(mReceiveBuffer[mReceivedSoFar]), 1);
                if (rc > 0) {
                    // echo
                    dev_uart.write(&(mReceiveBuffer[mReceivedSoFar]), 1);
                    mReceivedSoFar += rc;
                    return IO_SUCCESS;
                }
            }
            return IO_ERROR;
        }
    
        static eIoError 
        IoSendString(const char *buffer)
        {
            printf("%s", buffer);
            return IO_SUCCESS;
        }

        static eIoError 
        IoSendLine(const char *buffer)
        {
            IoSendString(buffer);
            IoSendString(lf);
            return IO_SUCCESS;
        }

        static int32_t 
        CommandEndLine(const char receiveBuffer[], const uint32_t filledLength)
        {
            uint32_t i = 0;
            int32_t result = NOT_FOUND;

            while ( (cr_char != receiveBuffer[i]) && (nl_char != receiveBuffer[i]) &&
                        (i < filledLength))
            {
                i++;
            }

            if (i < filledLength) 
            {
                result = i;
            }

            return result;
        }

        static uint32_t 
        CommandMatch(const char* name, const char *buffer)
        {
            uint32_t i = 0u; 
            uint32_t result = 0u;
            if (buffer[i] == name[i]) 
            {
                result = 1u;
                i++;
            }

            while ( (1u == result) &&
                    (i < MAX_COMMAND_LENGTH) &&
                    (buffer[i] != param_sep) &&
                    (buffer[i] != cr_char) && (buffer[i] != nl_char) &&
                    (buffer[i] != nullchar))
            {
                if (buffer[i] != name[i])
                {
                    result = 0u;
                }
                i++;
            }
            return result;
        }

        static eCommandResult_T 
        ReceiveParamInt8(const char * buffer, const uint8_t parameterNumber, int8_t* parameterInt8)
        {
            uint32_t startIndex = 0;
            uint32_t i;
            eCommandResult_T result;
            char charVal;
            char str[int8_max_str_length];

            result = ParamFindN(buffer, parameterNumber, &startIndex);

            i = 0;
            charVal = buffer[startIndex + i];
            while ( ( nl_char != charVal ) && ( cr_char != charVal )
                    && ( param_sep != charVal )
                && ( i < int8_max_str_length ) )
            {
                str[i] = charVal;					// copy the relevant part
                i++;
                charVal = buffer[startIndex + i];
            }
            if ( i == int8_max_str_length)
            {
                result = COMMAND_PARAMETER_ERROR;
            }
            if ( COMMAND_SUCCESS == result )
            {
                str[i] = nullchar;
                *parameterInt8 = atoi(str);
	        }
            return result;
        }
        
        static eCommandResult_T 
        SendLine(const char * buffer)
        {
            eCommandResult_T result = COMMAND_SUCCESS;
            return result;
        }

        static eCommandResult_T 
        SendParamInt8(int8_t parameterInt8)
        {
            eCommandResult_T result = COMMAND_SUCCESS;
            return result;
        }

        static eCommandResult_T 
        SendParamHexUint8(uint8_t parameterUint8)
        {
            uint32_t i;
            char out[2u + 1u];  // U8 must be less than 4 hex digits: 0xFFFF, end buffer with a NULL
            eCommandResult_T result = COMMAND_SUCCESS;

            i = 0u;
            result = UtilIntToHexChar( (parameterUint8 >> 4u) & 0xF, &(out[i]));
            i++;
            if ( COMMAND_SUCCESS == result )
            {
                result = UtilIntToHexChar( parameterUint8 & 0xF, &(out[i]));
                i++;
            }
            out[i] = nullchar;
            IoSendString(out);
            return result;
        }
        
        static eCommandResult_T 
        SendString(const char * buffer)
        {
            eCommandResult_T result = COMMAND_SUCCESS;
            return result;
        }

        static eCommandResult_T CommandComment(const char buffer[])
        {
            IGNORE_UNUSED_VARIABLE(buffer)
            return COMMAND_SUCCESS;
        }

        static eCommandResult_T CommandHelp(const char buffer[])
        {
            uint32_t i;
            uint32_t tableLength{5};
            eCommandResult_T result = COMMAND_SUCCESS;

            IGNORE_UNUSED_VARIABLE(buffer);

            
            for ( i = 0u ; i < tableLength - 1u ; i++ )
            {
                IoSendString(mCommandTable[i].name);
                IoSendString(" : ");
                IoSendLine(mCommandTable[i].help);
            }
            return result;
        }

        static eCommandResult_T CommandVer(const char buffer[])
        {
            eCommandResult_T result = COMMAND_SUCCESS;

            IGNORE_UNUSED_VARIABLE(buffer);
            IoSendLine(ST_VER_INFO_STRING);
            return result;
        }

        static eCommandResult_T CommandParamExampleInt8(const char buffer[])
        {
            eCommandResult_T result;
            int8_t parameterInt;
            result = ReceiveParamInt8(buffer, 1, &parameterInt);
            if ( COMMAND_SUCCESS == result )
            {
                IoSendString("Parameter is ");
                SendParamInt8(parameterInt);
                IoSendString(" (0x");
                SendParamHexUint8((uint8_t)parameterInt);
                IoSendString(")");
                IoSendString(lf);
            }
            return result;
        }


        const sCommandTable_T *
        CommandsGetTable(void)
        {
            return (mCommandTable);
        }

        static eCommandResult_T 
        ParamFindN(const char *buffer, const uint8_t paramNumber, uint32_t *startLoc)
        {
            uint32_t buffer_index{0}; 
            uint32_t parameter_index{0}; 
            eCommandResult_T result = COMMAND_SUCCESS;
            while ( (paramNumber != parameter_index) && (buffer_index < MAX_COMMAND_LENGTH) )
            {
                if ( param_sep == buffer[buffer_index])
                {
                    parameter_index++;
                }
                buffer_index++;
            }
            if ( MAX_COMMAND_LENGTH == buffer_index )
            {
                result = COMMAND_PARAMETER_ERROR;
            }
            else
            {
                *startLoc = buffer_index;
            }
            return result;
        }

        static uint32_t 
        ResetBuffer(char receiveBuffer[], const uint32_t filledLength, uint32_t usedSoFar)
        {
            uint32_t remaining = (filledLength - usedSoFar);
            uint32_t i = 0;

            while (usedSoFar < filledLength) 
            {
                receiveBuffer[i] = receiveBuffer[usedSoFar];
                i++;
                usedSoFar++;
            }
            for (; i < MAX_COMMAND_LENGTH; i++) 
            {
                receiveBuffer[i] = nullchar;
            }
            return remaining;
        }

        static eCommandResult_T 
        UtilHexCharToInt(char charVal, uint8_t *pInt)
        {
            eCommandResult_T result = COMMAND_SUCCESS;
            if ( ('0' <= charVal ) && ( charVal <= '9' ) )
            {
                *pInt = charVal - '0';
            }
            else if ( ('A' <= charVal ) && ( charVal <= 'F' ) )
            {
                *pInt = 10u + charVal - 'A';
            }
            else if ( ('a' <= charVal ) && ( charVal <= 'f' ) )
            {
                *pInt = 10u + charVal - 'a';
            }
            else if ( ( nl_char != charVal ) || ( cr_char != charVal ) || ( param_sep != charVal ) )
            {
                result = COMMAND_PARAMETER_END;
            }
            else 
            {
                result = COMMAND_PARAMETER_ERROR;
            }
            return result;
        }

        static eCommandResult_T
        UtilIntToHexChar(uint8_t intVal, char *pChar)
        {
            eCommandResult_T result = COMMAND_SUCCESS;

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
                result = COMMAND_PARAMETER_ERROR;
            }

            return result;
        }



    };
}