# Microsoft Developer Studio Project File - Name="libntp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libntp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libntp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libntp.mak" CFG="libntp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libntp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libntp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ntp/dev/ports/winnt/libntp", AWBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libntp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "." /I "..\include" /I "..\..\..\include" /I "$(OPENSSL)\inc32" /I "..\..\..\libopts" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "__STDC__" /D "SYS_WINNT" /D "HAVE_CONFIG_H" /D _WIN32_WINNT=0x400 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libntp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libntp___Win32_Debug"
# PROP BASE Intermediate_Dir "libntp___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "." /I "..\include" /I "..\..\..\include" /I "$(OPENSSL)\inc32" /I "..\..\..\libopts" /D "_DEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "__STDC__" /D "SYS_WINNT" /D "HAVE_CONFIG_H" /D _WIN32_WINNT=0x400 /FR /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libntp - Win32 Release"
# Name "libntp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\libntp\a_md5encrypt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\adjtime.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libisc\assertions.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\atoint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\atolfp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\atouint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\authkeys.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\authreadkeys.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\authusekey.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\buftvtots.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\caljulian.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\caltontp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\calyearstart.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\clocktime.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\clocktypes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\decodenetnum.c
# End Source File
# Begin Source File

SOURCE=.\dnslookup.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\dofptoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\dolfptoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\emalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libisc\error.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\findconfig.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\fptoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\fptoms.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\getopt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\hextoint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\hextolfp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\humandate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libisc\inet_aton.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libisc\inet_ntop.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libisc\inet_pton.c
# End Source File
# Begin Source File

SOURCE=..\libisc\interfaceiter.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\inttoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\iosignal.c
# End Source File
# Begin Source File

SOURCE=..\libisc\isc_strerror.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libisc\lib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\lib_strbuf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libopts\libopts.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\machines.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\md5c.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libisc\mem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\memmove.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\mfptoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\mfptoms.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\mktime.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\modetoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libisc\msgcat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\mstolfp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\msutotsf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\msyslog.c
# End Source File
# Begin Source File

SOURCE=..\libisc\net.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libisc\netaddr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\netof.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libisc\netscope.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\ntp_random.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\ntp_rfc2553.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\numtoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\numtohost.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\octtoint.c
# End Source File
# Begin Source File

SOURCE=..\libisc\once.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\prettydate.c
# End Source File
# Begin Source File

SOURCE=.\randfile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\recvbuff.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\refnumtoa.c
# End Source File
# Begin Source File

SOURCE=.\SetSystemTime.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libisc\sockaddr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\socktoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\socktohost.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\statestr.c
# End Source File
# Begin Source File

SOURCE=.\syslog.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\syssignal.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\systime.c
# End Source File
# Begin Source File

SOURCE=.\transmitbuff.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\tsftomsu.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\tstotv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\tvtoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\tvtots.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\uglydate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\uinttoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\utvtoa.c
# End Source File
# Begin Source File

SOURCE=..\libisc\win32os.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\ymd2yd.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\binio.h
# End Source File
# Begin Source File

SOURCE=..\include\clockstuff.h
# End Source File
# Begin Source File

SOURCE=..\include\config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\global.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ieee754io.h
# End Source File
# Begin Source File

SOURCE=..\include\netinet\in.h
# End Source File
# Begin Source File

SOURCE=..\include\arpa\inet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\isc\interfaceiter.h
# End Source File
# Begin Source File

SOURCE=..\include\sys\ioctl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\iosignal.h
# End Source File
# Begin Source File

SOURCE=..\include\isc\ipv6.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\l_stdlib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\lib_strbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\md5.h
# End Source File
# Begin Source File

SOURCE=..\include\isc\net.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\isc\netaddr.h
# End Source File
# Begin Source File

SOURCE=..\include\netdb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\isc\netscope.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_calendar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_control.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_fp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_if.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_io.h
# End Source File
# Begin Source File

SOURCE=..\include\ntp_iocompletionport.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_machine.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_malloc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_proto.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_random.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_refclock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_rfc2553.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_select.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_stdlib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_string.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_syslog.h
# End Source File
# Begin Source File

SOURCE=..\include\ntp_timer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_unixtime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntpd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\recvbuff.h
# End Source File
# Begin Source File

SOURCE=..\include\sys\signal.h
# End Source File
# Begin Source File

SOURCE=..\include\sys\socket.h
# End Source File
# Begin Source File

SOURCE=..\include\syslog.h
# End Source File
# Begin Source File

SOURCE=..\include\termios.h
# End Source File
# Begin Source File

SOURCE=..\include\sys\time.h
# End Source File
# Begin Source File

SOURCE=..\include\transmitbuff.h
# End Source File
# Begin Source File

SOURCE=..\include\win32_io.h
# End Source File
# Begin Source File

SOURCE=..\include\isc\win32os.h
# End Source File
# End Group
# Begin Group "Generated Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\messages.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\messages.mc

!IF  "$(CFG)" == "libntp - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Message Compiler
InputDir=.
InputPath=.\messages.mc
InputName=messages

BuildCmds= \
	mc -h $(InputDir) -r $(InputDir) -e h $(InputPath)

"$(InputDir)\$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\msg00001.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "libntp - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Message Compiler
InputDir=.
InputPath=.\messages.mc
InputName=messages

BuildCmds= \
	mc -h $(InputDir) -r $(InputDir) -e h $(InputPath)

"$(InputDir)\$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\msg00001.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
