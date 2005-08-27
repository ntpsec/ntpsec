# Microsoft Developer Studio Project File - Name="ntpd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ntpd - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ntpd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ntpd.mak" CFG="ntpd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ntpd - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ntpd - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ntp/dev/ports/winnt/ntpd", JWBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ntpd - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "." /I "..\include" /I "..\..\..\include" /I "\openssl-0.9.7d\inc32" /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /D "_MBCS" /D "__STDC__" /D "SYS_WINNT" /D "HAVE_CONFIG_H" /D _WIN32_WINNT=0x400 /FR /YX"windows.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib \openssl-0.9.7d\out32dll\libeay32.lib /nologo /subsystem:console /machine:I386 /out:"../bin/Release/ntpd.exe"

!ELSEIF  "$(CFG)" == "ntpd - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "..\include" /I "..\..\..\include" /I "\openssl-0.9.7d\inc32" /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /D "_MBCS" /D "__STDC__" /D "SYS_WINNT" /D "HAVE_CONFIG_H" /D _WIN32_WINNT=0x400 /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib \openssl-0.9.7d\out32dll\libeay32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../bin/Debug/ntpd.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ntpd - Win32 Release"
# Name "ntpd - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\ntpd\cmd_args.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\nt_clockstuff.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_config.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_control.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_crypto.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_filegen.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_intres.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_io.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\ntp_iocompletionport.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_loopfilter.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_monitor.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_peer.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_proto.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_refclock.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_request.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_restrict.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_timer.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntp_util.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\ntpd.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\ntservice.c
# End Source File
# Begin Source File

SOURCE=.\win32_io.c
# SUBTRACT CPP /YX
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\ascii.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\binio.h
# End Source File
# Begin Source File

SOURCE=..\include\config.h
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

SOURCE=..\..\..\ntpd\jupiter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\l_stdlib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libntp\log.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\mbg_gps166.h
# End Source File
# Begin Source File

SOURCE=..\include\netdb.h
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

SOURCE=..\..\..\include\ntp_filegen.h
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

SOURCE=..\..\..\include\ntp_request.h
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

SOURCE=..\..\..\include\ntp_types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntp_unixtime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ntpd.h
# End Source File
# Begin Source File

SOURCE=..\include\sys\param.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\recvbuff.h
# End Source File
# Begin Source File

SOURCE=..\include\sys\socket.h
# End Source File
# Begin Source File

SOURCE=..\include\syslog.h
# End Source File
# Begin Source File

SOURCE=..\include\transmitbuff.h
# End Source File
# Begin Source File

SOURCE=..\include\sys\wait.h
# End Source File
# Begin Source File

SOURCE=..\include\win32_io.h
# End Source File
# End Group
# Begin Group "Generated Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\libntp\messages.rc
# End Source File
# Begin Source File

SOURCE=..\libntp\MSG00001.bin
# End Source File
# Begin Source File

SOURCE=.\version.c
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\libntp\messages.mc
# End Source File
# End Group
# Begin Group "Reference Clock"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hopf_PCI_io.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_acts.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_arbiter.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_arc.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_as2201.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_atom.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_bancomm.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_chu.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_conf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_datum.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_dumbclock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_gpsvme.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_heath.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_hopfpci.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_hopfser.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_hpgps.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_irig.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_jupiter.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_leitch.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_local.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_msfees.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_mx4200.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_nmea.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_oncore.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_palisade.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_palisade.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_parse.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_pst.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_ptbacts.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_shm.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_tpro.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_trak.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_true.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_usno.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\ntpd\refclock_wwvb.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Parse Lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\libparse\clk_computime.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libparse\clk_dcf7000.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libparse\clk_hopf6021.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libparse\clk_meinberg.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libparse\clk_rawdcf.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libparse\clk_rcc8000.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libparse\clk_schmid.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libparse\clk_trimtaip.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libparse\clk_trimtsip.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libparse\data_mbg.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libparse\parse.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libparse\parse_conf.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\configure

!IF  "$(CFG)" == "ntpd - Win32 Release"

USERDEP__CONFI="..\scripts\mkver.bat"	
# Begin Custom Build
ProjDir=.
InputPath=..\..\..\configure

"$(ProjDir)\version.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo Using NT Shell Script to generate version.c 
	..\scripts\mkver.bat -P ntpd 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ntpd - Win32 Debug"

USERDEP__CONFI="..\scripts\mkver.bat"	
# Begin Custom Build
ProjDir=.
InputPath=..\..\..\configure

"$(ProjDir)\version.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo Using NT Shell Script to generate version.c 
	..\scripts\mkver.bat -P ntpd 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\scripts\mkver.bat
# PROP Exclude_From_Build 1
# End Source File
# End Target
# End Project
