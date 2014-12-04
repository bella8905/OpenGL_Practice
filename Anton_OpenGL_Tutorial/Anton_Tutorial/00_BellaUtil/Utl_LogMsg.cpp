/////////////////////////////////////////////////////////////////
//
//  Practice_11_19_14 - Utl_ConsoleMsg
//
//
//  Copyright (c) 2014 Bella Q
//  
/////////////////////////////////////////////////////////////////

#include "Utl_LogMsg.h"
#include <assert.h>
#include <iostream>
#include <time.h>

CONSOLE_SCREEN_BUFFER_INFO ColorReverter::_csbi;
ColorReverter* ColorReverter::_instance = 0;

ofstream outFile;
const string LOG_FILE = "GL_LOG.txt";


ColorModifier ColorMod_Warning( COLOR_WARNING );
ColorModifier ColorMod_Error( COLOR_ERROR );
ColorModifier ColorMod_Pass( COLOR_PASS );
ColorReverter* ColorMod_Restore = ColorReverter::GetInstance();

ColorReverter* ColorEndl::_rvt = ColorMod_Restore;
ColorCout LogError( ColorMod_Error );
ColorCout LogWarning( ColorMod_Warning );
ColorCout LogPass( ColorMod_Pass );
ColorCout LogMsg;
ColorEndl LogEndl;

static void _initFileOutput();
static string _getCurrentTime();

std::ostream& operator<<( std::ostream& os, const ColorModifier& mod) {
#ifdef _WIN32
    HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute( hstdout, mod.GetColor() );
#endif  // add something later for osx and linux
    return os;
}

std::ostream& operator<<( std::ostream& os, const ColorReverter* rev) {
    if( rev == 0 )  return os;

#ifdef _WIN32
    HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute( hstdout, rev->_csbi.wAttributes );

#endif
    return os;
}




ColorCout& operator<<( ColorCout& t_os, const  ColorEndl& t_colorEndl) {
    if( t_colorEndl._rvt == 0 ) return t_os;
#ifdef _WIN32
    t_os.SetColorWritten( false );
    cout<<t_colorEndl._rvt<<endl; // reset color and flush buffer

    if( outFile.is_open() ) {
        // don't use fflush to flush the buffer, it's very expensive
        // let the destructor do the work. RAII
        outFile<<'\n';
    }

#endif
    return t_os;
}


static void _initFileOutput()
{
    if( !outFile.is_open() )
    {
        outFile.open( LOG_FILE );
        if( !outFile ) {
            cout<<ColorMod_Warning<<"can't open log file. log file not used"<<endl;
            outFile.close();
            outFile.clear();
            return;
        }

        // write time info
        string time = _getCurrentTime();
        outFile<<time<<'\n';
    }
}

ColorCout::ColorCout( const ColorModifier& t_colorMod ) : _colorMod( t_colorMod ), _colorWritten( false )
{
    _initFileOutput();
}

ColorCout::ColorCout() : _colorWritten( false )
{
    _initFileOutput();
}

ColorEndl::ColorEndl() 
{
    _initFileOutput();
}


static string _getCurrentTime()
{
    time_t rawTime = time( 0 );
    tm timeInfo;
    localtime_s( &timeInfo, &rawTime );
    char buffer[80];
    strftime(buffer,80,"%d-%m-%Y %I:%M:%S", &timeInfo);
    return string( buffer );
}
