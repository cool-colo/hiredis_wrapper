#pragma once
#include <iostream>
#include <stdarg.h>
#include <string>
static void AC_ERROR(const char* pszFormat, ... )
{

    char Line[2048] = {0};
    va_list args;
    va_start( args, pszFormat );
    vsnprintf( Line, sizeof(Line), pszFormat, args );
    va_end( args );

    std::cout<<std::string(Line)<<std::endl;
}

static void AC_DEBUG(const char* pszFormat, ... )
{

    char Line[2048] = {0};
    va_list args;
    va_start( args, pszFormat );
    vsnprintf( Line, sizeof(Line), pszFormat, args );
    va_end( args );

    std::cout<<std::string(Line)<<std::endl;
}
#define IF_ERROR_RETURN(ret) if (ret < 0){return ret;}
#define RETURN_IF_ERROR(x) do{if((x) != 0) {return -1;}}while(false);
