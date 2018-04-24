::catch {} ;#\
@ECHO OFF
::catch {} ;#\
set REGPROCESSOR=%1
::catch {} ;#\
set RUNDXE=%2
::catch {} ;#\
set VDSP_VERSION=%3
::catch {} ;#\
tclsh83 %DORUNLOC%%0
::catch {} ;#\
@goto eof
source [ file join [ file dirname $argv0 ] invoke_debugger.tcl ]
#\
:eof
