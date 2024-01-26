#include "console.h"

namespace soft_touch 
{
    BufferedSerial Console::dev_uart(USBTX, USBRX);
    const char* Console::prompt = "> ";
    const char* Console::lf = "\r\n";
    const char* Console::version = "Soft Touch v0.0.1";
    const char Console::param_sep = ' ';
    const char Console::nullchar = '\0';
    const char Console::cr_char = '\r';
    const char Console::nl_char = '\n';
    const Console::sCommandTable_T Console::mCommandTable[] = 
    {
        {";", Console::CommandComment, "Comment! You do need a space after the semicolon. "},
        {"help", Console::CommandHelp, "Lists the commands available"},
        {"ver", Console::CommandVer, "Get the version string"},
        {"int", Console::CommandParamExampleInt8, "How to get a signed int from params"},
        {NULL, NULL, ""} // must be LAST
    };
}