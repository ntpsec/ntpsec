@rem = '--*-Perl-*--';
@rem = '
@echo off
perl -S %0.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';
######################################################################
#
# Revision: mkver.bat
# Author:   Greg Schueman
# Date:     05/03/1996 
# Purpose:  Provide a perl script for NT to replace the
#           mkversion shell script.
#           
#
#
# Subroutines:
#     print_help
#     
#
#
######################################################################

use English;
use Getopt::Long;

#********************************************************************* 
#  Program Dependency Requirements
#*********************************************************************

#********************************************************************* 
#  Set Environment
#*********************************************************************
$PROGRAM = $0;
$USAGE   = "Usage: ${PROGRAM} [ -P <Program Name> -H ]\n";


#********************************************************************* 
#  Subroutine Print Help 
#*********************************************************************

sub print_help 
{
   print STDERR $USAGE;
   print STDERR " -P --Program Name      Database Name\n";
   print STDERR " -H --Help         Help on options\n";
   print STDERR "\n";
} # print_help end



#********************************************************************* 
#  Main program
#*********************************************************************

#
# Process runtime options
#
$result = GetOptions('help|H', 'prog|P=s'); 

if ($opt_help == 1)
{ 
   print_help(); 
   exit();
};


if (length($opt_prog) > 0)
   { $GENERATED_PROGRAM=$opt_prog; } 
else 
   { die "Program Name parameter required."; }

#
# Program logic
#

$DATE =  localtime;
chomp $DATE;
$RUN = "0"; # Not working yet

open( INPUT, '<..\..\..\configure' );
open( OUTPUT, '>version.c' );

while ($_ = <INPUT> )
{
#   if (/^[0-9a-zA-Z_]*=/) 
   if (/^VERSION=/) 
   {
      $FLAGS = $POSTMATCH;
   }
}
chomp $FLAGS;

print OUTPUT "char * Version = \"$GENERATED_PROGRAM $FLAGS $DATE ($RUN)\" ;";

close( OUTPUT );
close( INPUT );

__END__
:endofperl
