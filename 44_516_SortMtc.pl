#!C:/Perl/bin/perl.EXE -w

use strict ;
use warnings ;

use IO::File ;
use Getopt::Simple ;
use POSIX qw{strftime} ;

# Application   : ISPL
# Client        : Internal
# Copyright (c) : 2017 IdentLogic Systems Private Limited
# Author        : Surendra Kadam
# Creation Date : 8 May 2017
# Description   : Sort Squashed match
# WARNINGS      :
# HARD CODINGS  :
# Limitations   :
# Dependencies  :
# Modifications
# Date       Change Req# Author       Description
# ---------- ----------- ------------ -------------------------------

use constant K_PACKAGE_NO         => 44 ;                       # Package
use constant K_PROCEDURE_NO       => 516 ;                      # Procedure
use constant K_PROCEDURE_NM_U     => "44_516_SortMtc.pl" ;      # Procedure name
use constant K_PROCEDURE_NM_SYS_U => "Sort Squashed match" ;    # Procedure description

use constant K_EMPTY  => "" ;                                   # Empty field
use constant K_NEW_LN => "\n" ;                                 # New Line Character
use constant K_Y      => "Y" ;                                  # Y for Yes
use constant K_n      => "n" ;                                  # n for no
use constant K_SPACE  => " " ;                                  # Space
use constant K_ONE    => 1 ;                                    # One
use constant K_ZERO   => 0 ;                                    # Zero

# Run Parameters
my $p_data_set       = K_EMPTY ;                                # Data set                       - d
my $p_run_no         = K_EMPTY ;                                # Run number                     - r
my $p_prps_no        = K_EMPTY ;                                # Purpose number                 - p
my $p_input_file_dir = K_EMPTY ;                                # input squashed file directory  - f
my $p_sort_file_dir  = K_EMPTY ;                                # Sort file directory            - a
my $p_log_file_dir   = K_EMPTY ;                                # Log file directory             - l
my $p_f_verbose      = K_EMPTY ;                                # Flag - Verbose - print details - v
my $p_f_erase_work   = K_EMPTY ;                                # Erase intermediate work files  - e
my $p_work_file_dir  = K_EMPTY ;                                # work file directory            - w

my $v_input_file = K_EMPTY ;                                    # Input file
my $v_sort_file  = K_EMPTY ;                                    # Stores sort file name
my $v_log_file   = K_EMPTY ;                                    # Stores log file name

my $v_input_file_dir    = K_EMPTY ;                             # Input squashed file directory
my $v_sort_file_dir     = K_EMPTY ;                             # Sort file directory
my $v_log_file_dir      = K_EMPTY ;                             # Log file directory
my $v_work_file_dir     = K_EMPTY ;                             # Temporary file working directory
my $v_work_file_dir_cmd = K_EMPTY ;                             # Temporary file working directory for cmd

my $v_sec_to_exe_sort_match  = K_EMPTY ;                        # Seconds to execute sort matches
my $v_sort_mtc_end_ts        = K_EMPTY ;                        # Sort matches End timestamp
my $v_temp_sort_log_file     = K_EMPTY ;                        # Temporary sort log file
my $v_temp_sort_err_log_file = K_EMPTY ;                        # Temporary sort error log file
my $v_f_error                = K_n ;                            # Flag - Error

my $FILELOG = IO::File -> new ;                                 # Declaration of filehandle of log file

my $v_now_ss = K_EMPTY ;                                        # Time elements - second ...
my $v_now_mi = K_EMPTY ;                                        # ... minute ...
my $v_now_hh = K_EMPTY ;                                        # ... hour - 24 hour format ...
my $v_now_dd = K_EMPTY ;                                        # ... day ...
my $v_now_mm = K_EMPTY ;                                        # ... month ...
my $v_now_yy = K_EMPTY ;                                        # ... year

# START ###############################################################

( $v_now_ss , $v_now_mi , $v_now_hh , $v_now_dd , $v_now_mm , $v_now_yy )    # Get time elements
  = ( localtime )[ 0 , 1 , 2 , 3 , 4 , 5 ] ;

$v_now_yy = $v_now_yy + 1900 ;                                  # Adjust year
$v_now_mm = $v_now_mm + 1 ;                                     # Adjust month

my $v_start_timestamp =                                         # Start timestamp - put leading 0 for 2 digit fields when < 10
  $v_now_yy . '-'                                               #
  . ( $v_now_mm < 10 ? '0' . $v_now_mm : $v_now_mm ) . '-'      #
  . ( $v_now_dd < 10 ? '0' . $v_now_dd : $v_now_dd ) . ' '      #
  . ( $v_now_hh < 10 ? '0' . $v_now_hh : $v_now_hh ) . ':'      #
  . ( $v_now_mi < 10 ? '0' . $v_now_mi : $v_now_mi ) . ':'      #
  . ( $v_now_ss < 10 ? '0' . $v_now_ss : $v_now_ss )            #
  ;                                                             #
my $v_start_time = time () ;                                    # Start time

&sGetParameters ;                                               # Get run parameters

&sSortmatchFile () ;

&sWriteLog () ;                                                 # Write log

if (
   $p_f_erase_work eq K_Y and                                   # If flag - erase - requested
   $v_f_error eq K_n
  ) {                                                           # .. no error
   unlink $v_input_file_dir . $v_input_file ;                   #  Unlink input match file
} ## end if ( $p_f_erase_work eq...)

exit 1 ;

#######################################################################
# End of main                                                         #
#######################################################################

sub sGetCurTimestamp {                                          # Get current timestamp

   my $vg_cur_ss = K_EMPTY ;                                    # Time elements - second ...
   my $vg_cur_mi = K_EMPTY ;                                    # ... minute ...
   my $vg_cur_hh = K_EMPTY ;                                    # ... hour - 24 hour format ...
   my $vg_cur_dd = K_EMPTY ;                                    # ... day ...
   my $vg_cur_mm = K_EMPTY ;                                    # ... month ...
   my $vg_cur_yy = K_EMPTY ;                                    # ... year

   ( $vg_cur_ss , $vg_cur_mi , $vg_cur_hh , $vg_cur_dd , $vg_cur_mm , $vg_cur_yy )    # Get time elements
     = ( localtime )[ 0 , 1 , 2 , 3 , 4 , 5 ] ;

   $vg_cur_yy = $vg_cur_yy + 1900 ;                             # Adjust year
   $vg_cur_mm = $vg_cur_mm + 1 ;                                # Adjust month

   my $vg_cur_timestamp =                                       # End timestamp - put leading 0 for 2 digit fields when < 10
     $vg_cur_yy . '-'                                           #
     . ( $vg_cur_mm < 10 ? '0' . $vg_cur_mm : $vg_cur_mm ) . '-'                      #
     . ( $vg_cur_dd < 10 ? '0' . $vg_cur_dd : $vg_cur_dd ) . ' '                      #
     . ( $vg_cur_hh < 10 ? '0' . $vg_cur_hh : $vg_cur_hh ) . ':'                      #
     . ( $vg_cur_mi < 10 ? '0' . $vg_cur_mi : $vg_cur_mi ) . ':'                      #
     . ( $vg_cur_ss < 10 ? '0' . $vg_cur_ss : $vg_cur_ss )      #
     ;                                                          #

   return $vg_cur_timestamp ;

} ## end sub sGetCurTimestamp
#######################################################################
# End of subroutine sGetCurTimestamp                                  #
#######################################################################

sub sGetParameters {

   # Gets run parameters

   my ( $vgp_get_options ) = {
      help => {
         type    => '' ,
         env     => '-' ,
         default => '' ,
         verbose => '' ,
         order   => 1 ,
        } ,
      dataset => {
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Data set' ,
         order   => 2 ,
        } ,
      runno => {
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Run number' ,
         order   => 3 ,
        } ,
      purposeno => {
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Purpose number' ,
         order   => 4 ,
        } ,
      infldir => {
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Input squashed file directory - optional' ,
         order   => 4 ,
        } ,
      logfldir => {
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Log file directory - optional' ,
         order   => 5 ,
        } ,
      asortfldir => {
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Sort file directory - optional' ,
         order   => 6 ,
        } ,
      workfldir => {
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'work file directory - optional' ,
         order   => 7 ,
        } ,
      verboseflag => {
         type    => '!' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Print job details - no argument needed - optional' ,
         order   => 8 ,
        } ,
      eraseworkflag => {
         type    => '!' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Erase intermediate work files - no argument needed - optional' ,
         order   => 9 ,
      } ,
   } ;

   my ( $vgp_parameters ) = Getopt::Simple -> new () ;
   if ( ! $vgp_parameters -> getOptions ( $vgp_get_options , "Usage: 44_516_SortMtc.pl [options]" ) ) {
      exit ( -1 ) ;                                             # Failure
   }

   # Get run time parameters value
   $p_data_set =                                                # Dataset                             - d
     $$vgp_parameters{ 'switch' }{ 'dataset' } || K_EMPTY ;
   $p_run_no =                                                  # Run number                          - r
     $$vgp_parameters{ 'switch' }{ 'runno' } || K_EMPTY ;       #
   $p_prps_no =                                                 # Purpose number                      - p
     $$vgp_parameters{ 'switch' }{ 'purposeno' } || K_EMPTY ;   #
   $p_input_file_dir =                                          # Input squashed file directory       - i
     $$vgp_parameters{ 'switch' }{ 'infldir' } || K_EMPTY ;
   $p_log_file_dir =                                            # Log file directory                  - l
     $$vgp_parameters{ 'switch' }{ 'logfldir' } || K_EMPTY ;
   $p_sort_file_dir =                                           # Sort file directory                 - a
     $$vgp_parameters{ 'switch' }{ 'asortfldir' } || K_EMPTY ;
   $p_work_file_dir =                                           # work file directory                 - w
     $$vgp_parameters{ 'switch' }{ 'workfldir' } || K_EMPTY ;
   $p_f_verbose =                                               # Flag - Verbose - print details      - v
     $$vgp_parameters{ 'switch' }{ 'verboseflag' } ;
   $p_f_erase_work = $$vgp_parameters{ 'switch' }{ 'eraseworkflag' } ;    # Erase intermediate work files  - e

   if   ( $p_f_verbose eq '1' ) { $p_f_verbose = 'Y' ; }        # Convert to ISPL convention
   else                         { $p_f_verbose = 'n' ; }

   if   ( $p_f_erase_work eq '1' ) { $p_f_erase_work = 'Y' ; }  # Convert to ISPL convention
   else                            { $p_f_erase_work = 'n' ; }

   if ( $p_f_verbose eq 'Y' ) {                                 # Display run parameters if verbose option
      print "$0: Started at" . K_SPACE . ( localtime ) . K_NEW_LN . K_NEW_LN ,    #
        'Dataset                       - >' , $p_data_set ,       '<' , K_NEW_LN ,                                                                              #
        'Run number                    - >' , $p_run_no ,         '<' , K_NEW_LN ,                                                                              #
        'Purpose number                - >' , $p_prps_no ,        '<' , K_NEW_LN ,                                                                              #
        'Input squashed file directory - >' , $p_input_file_dir , '<' , K_NEW_LN ,                                                                              #
        'Log file directory            - >' , $p_log_file_dir ,   '<' , K_NEW_LN ,                                                                              #
        'Sort file directory           - >' , $p_sort_file_dir ,  '<' , K_NEW_LN , 'work file directory           - >' , $p_work_file_dir , '<' , K_NEW_LN ,    #
        'Flag - erase work             - >' , $p_f_erase_work ,   '<' , K_NEW_LN ,                                                                              #
        'Flag - Verbose                - >' , $p_f_verbose , '<' , K_NEW_LN , K_NEW_LN ;
   } ## end if

   $v_input_file_dir = ( $p_input_file_dir eq K_EMPTY ? './' : $p_input_file_dir ) ;
   $v_sort_file_dir  = ( $p_sort_file_dir eq K_EMPTY  ? './' : $p_sort_file_dir ) ;
   $v_log_file_dir   = ( $p_log_file_dir eq K_EMPTY   ? './' : $p_log_file_dir ) ;
   $v_work_file_dir  = ( $p_work_file_dir eq K_EMPTY  ? './' : $p_work_file_dir ) ;

   if ( $p_data_set eq K_EMPTY ) {                              # Abort if data set not specified
      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . ' - ' . "Data set number not specified" . K_NEW_LN ;
   }

   if (  $p_data_set =~ /\D/i
      or $p_data_set < 100
      or $p_data_set > 999 ) {                                  # Abort if Dataset number not between 100 and 999

      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . ' - ' . "Dataset number must be integer and in a range of 100 and 999" . K_NEW_LN ;
   } ## end if ( $p_data_set =~ /\D/i...)

   if ( $p_run_no eq K_EMPTY ) {                                # Abort if Run number not specified

      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . ' - ' . "Run time number not specified" . K_NEW_LN ;
   }

   if (  $p_run_no =~ /\D/i
      or $p_run_no < 1000
      or $p_run_no > 9999 ) {                                   # Abort if Run number not between 1000 and 9999

      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . ' - ' . "Run number must be integer and in a range of 1000 and 9999" . K_NEW_LN ;
   } ## end if ( $p_run_no =~ /\D/i...)

   if ( $p_prps_no eq K_EMPTY ) {
      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . ' - ' . "Purpose number not specified" . K_NEW_LN ;
   }

   if ( $p_prps_no =~ /\D/i ) {
      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . ' - ' . "Purpose number should be integer" . K_NEW_LN ;
   }

   if ( $v_input_file_dir ne './' ) {                           # If input squashed file directory not default (./)
      if (
         substr ( $v_input_file_dir , -1 , 1 ) ne '\\'          # If input squashed file directory not end with back slash (\\)
        ) {

         $v_input_file_dir .= '\\' ;
         ;                                                      # Add back slash at end
      } ## end if ( substr ( $v_input_file_dir...))
   } ## end if ( $v_input_file_dir...)

   $v_input_file_dir =~ s/\\/\//g ;                             # Replace back slash to forward slash

   if ( $v_log_file_dir ne './' ) {                             # If log file directory not default (./)
      if (
         substr ( $v_log_file_dir , -1 , 1 ) ne '\\'            # If log file directory not end with back slash (\\)
        ) {
         $v_log_file_dir .= '\\' ;                              # Add back slash at end
      }
   } ## end if ( $v_log_file_dir ne...)

   $v_log_file_dir =~ s/\\/\//g ;                               # Replace back slash to forward slash

   if ( $v_sort_file_dir ne './' ) {                            # If sort file directory not default (./)
      if (
         substr ( $v_sort_file_dir , -1 , 1 ) ne '\\'           # If sort file directory not end with back slash (\\)
        ) {
         $v_sort_file_dir .= '\\' ;                             # Add back slash at end
      }
   } ## end if ( $v_sort_file_dir ...)

   $v_sort_file_dir =~ s/\\/\//g ;                              # Replace back slash to forward slash

   if ( $v_work_file_dir ne './' ) {                            # If Temporary file working directory not default (./)
      if (
         substr ( $v_work_file_dir , -1 , 1 ) ne '\\'           # If Temporary file working directory not end with back slash (\\)
        ) {
         $v_work_file_dir .= '\\' ;                             # Add back slash at end
      }
   } ## end if ( $v_work_file_dir ...)

   $v_work_file_dir_cmd = $v_work_file_dir ;
   $v_work_file_dir =~ s/\\/\//g ;                              # Replace back slash to forward slash
   $v_work_file_dir_cmd =~ s/\//\\/g ;                          # Replace forward slash to back slash for cmd command

   $v_input_file = $p_data_set . $p_run_no . '_' . $p_prps_no . '.mqt' ;    # Input file - Data set + Run number + Purpose number .mqt

   if ( ! -e $v_input_file_dir . $v_input_file ) {
      die                                                       #
        __PACKAGE__ . K_SPACE                                   #
        . __FILE__ . K_SPACE                                    #
        . __LINE__ . K_SPACE                                    #
        . "Input match file does not exist" . ' - ' . $v_input_file ;       #
   } ## end if ( ! -e $v_input_file)

} ## end sub sGetParameters
#######################################################################
# End of subroutine sGetParameters                                    #
#######################################################################

sub sSortmatchFile {                                            # Sort output squashed match file

   my $vsk_sys_cmd = K_EMPTY ;                                  # Store system command

   $v_sort_file              = $p_data_set . $p_run_no . '_' . $p_prps_no . '.mst' ;                  # Sort file - Data set + Run number + Purpose no .mst
   $v_temp_sort_log_file     = $p_data_set . $p_run_no . '_' . $p_prps_no . "_" . "temp.log" ;        # Sort log file
   $v_temp_sort_err_log_file = $p_data_set . $p_run_no . '_' . $p_prps_no . "_" . "temp_err.log" ;    # Sort error log

   $vsk_sys_cmd =                                               # Sort output matches file with CMsort
     "CMsort /S=1,0 /Q /D /B" . K_SPACE . $v_input_file_dir . $v_input_file . K_SPACE                 #
     . $v_sort_file_dir . $v_sort_file . K_SPACE                #
     . '1>' . K_SPACE . $v_work_file_dir . $v_temp_sort_log_file . K_SPACE                            #
     . '2>' . K_SPACE . $v_work_file_dir . $v_temp_sort_err_log_file ;

   system ( $vsk_sys_cmd ) ;                                    # Run command in system

   $v_sec_to_exe_sort_match = time () - $v_start_time ;         # Time to execute sort match
   $v_sort_mtc_end_ts       = &sGetCurTimestamp ;               # Sort match end timestamp

   if ( $p_f_verbose eq 'Y' ) {
      print "Match sorting ended" . K_SPACE . $v_sort_mtc_end_ts                           #
        . ' - ' . strftime ( "\%H:\%M:\%S" , gmtime ( $v_sec_to_exe_sort_match ) )                    #
        . K_SPACE . "to execute" . K_NEW_LN . K_NEW_LN ;
   }

} ## end sub sSortmatchFile
#######################################################################
# End of subroutine sSortmatchFile                                      #
#######################################################################

sub sWriteLog {                                                 # Write log file

   $v_log_file =                                                #
     $p_data_set . $p_run_no . '_' . $p_prps_no . '_'           #
     . 'SortMtc' . '_'                                          #
     . $v_now_yy . "-"                                          #
     . ( $v_now_mm < 10 ? 0 : K_EMPTY )                         #
     . $v_now_mm . "-"                                          #
     . ( $v_now_dd < 10 ? 0 : K_EMPTY )                         #
     . $v_now_dd . "-"                                          #
     . ( $v_now_hh < 10 ? 0 : K_EMPTY )                         #
     . $v_now_hh . "-"                                          #
     . ( $v_now_mi < 10 ? 0 : K_EMPTY )                         #
     . $v_now_mi . "-"                                          #
     . ( $v_now_ss < 10 ? 0 : K_EMPTY )                         #
     . $v_now_ss . '.log' ;                                     #

   open ( $FILELOG , '>' , $v_log_file_dir . $v_log_file ) or   # Create and open log file
     die "Can not open log file $v_log_file - $!" . K_NEW_LN ;

   my $v_log =                                                  #
     "------ SortMtc EXECUTION START DATE AND TIME ------" . K_NEW_LN                                 #
     . K_NEW_LN                                                 #
     . $v_start_timestamp . K_NEW_LN . K_NEW_LN                 #

     . "------ Run Parameters ------" . K_NEW_LN                #
     . K_NEW_LN                                                 #
     . "Data set number               :" . K_SPACE . $p_data_set . K_NEW_LN                           #
     . "Run time number               :" . K_SPACE . $p_run_no . K_NEW_LN                             #
     . "Purpose number                :" . K_SPACE . $p_prps_no . K_NEW_LN                            #
     . (
      $p_input_file_dir eq K_EMPTY
      ? K_EMPTY
      : "Input squashed file directory :" . K_SPACE . $p_input_file_dir . K_NEW_LN
     )                                                          #
     . (
      $p_log_file_dir eq K_EMPTY
      ? K_EMPTY
      : "Log File Directory            :" . K_SPACE . $p_log_file_dir . K_NEW_LN
     )                                                          #
     . (
      $p_sort_file_dir eq K_EMPTY
      ? K_EMPTY
      : "Sort file directory           :" . K_SPACE . $p_sort_file_dir . K_NEW_LN
     )                                                          #
     . (
      $p_work_file_dir eq K_EMPTY
      ? K_EMPTY
      : "work file directory           :" . K_SPACE . $p_work_file_dir . K_NEW_LN
     )                                                          #
     . "Flag - erase work             :" . K_SPACE . $p_f_erase_work . K_NEW_LN                       #
     . "Flag - verbose - details dsp  :" . K_SPACE . $p_f_verbose . K_NEW_LN                          #
     . K_NEW_LN                                                 #

     . "------ File Names ------" . K_NEW_LN                    #
     . K_NEW_LN                                                 #
     . "Input file name  :" . K_SPACE . $v_input_file . K_NEW_LN                                      #
     . "Sort file name   :" . K_SPACE . $v_sort_file . K_NEW_LN #
     . "Log file name    :" . K_SPACE . $v_log_file . K_NEW_LN ;

   print $FILELOG $v_log ;                                      # Write log file

   print $FILELOG K_NEW_LN . "Match sorting ended" . K_SPACE . $v_sort_mtc_end_ts          #
     . ' - ' . strftime ( "\%H:\%M:\%S" , gmtime ( $v_sec_to_exe_sort_match ) )                       #
     . K_SPACE . "to execute" . K_NEW_LN ;

   close $FILELOG                                               # Close log file
     or die "Can not close log file $v_log_file - $!" . K_NEW_LN ;

   system (                                                     # Write sort log file into main log file
      "Type" . K_SPACE . "\x22" . $v_work_file_dir_cmd . $v_temp_sort_log_file . "\x22"                     #
        . ">>" . K_SPACE . "\x22" . $v_log_file_dir . $v_log_file . "\x22"                                  #
   ) ;

   unlink $v_work_file_dir . $v_temp_sort_log_file ;            # Delete temporary sort log file after write into main log file

   system (                                                     # Write sort error log file into main log file
      "Type" . K_SPACE . "\x22" . $v_work_file_dir_cmd . $v_temp_sort_err_log_file . "\x22"                 #
        . ">>" . "\x22" . $v_log_file_dir . $v_log_file . "\x22"      #
   ) ;

   unlink $v_work_file_dir . $v_temp_sort_err_log_file ;        # Delete temporary sort error log file after write into main log file

} ## end sub sWriteLog
#######################################################################
# End of subroutine sWriteLog                                         #
#######################################################################

=pod

=head1 44_516_SortMtc.pl - Sort squashed match

 Sort squashed match which are generated from squash match using
  44_515_SqhMtc.pl

=head2 Copyright

 Copyright (c) 2017 IdentLogic Systems Private Limited

=head2 Description

 After squashing matches, we can sort output matches using this procedure and stored
  in .mst file

 Sort a match file using CMSort utility.

=head3 Format of Input squash match file - TAB delimited

 # FIELD
 - -----
 1 Search id       - Search id
 2 File id         - File id
 3 Purpose number  - Purpose number                 - 100 to 999
 4 Decision        - Typical Match Decision         - A / R / U
 5 Score           - Typical Match Score            - 000 to 100
 6 Decision        - Conservative Match Decisions   - A / R / U
 7 Score           - Conservative Match Score       - 000 to 100
 8 Decision        - Loose Match Decision           - A / R / U
 9 Score           - Loose Match Score              - 000 to 100

=head3 Format of Output Sorted match file - TAB delimited

 Sort a match file using CMSort utility.

 Note:

 CMsort /S=1,0 /Q /D /B <Inputfiledirectorypath> <Outputfiledirectorypath>

 /S=F,L = Case sensitive string from position F with length L ascending
 /Q     = Quiet mode (no progress output)
 /D     = Ignore records with duplicate keys (according to given sort keys)
 /B     = Ignore blank or empty records (i.e. empty lines or lines containing
          only blanks).

=head3 Format of log file

  Log file will be created with data set number , run number , purpose number
   procedure name and date time
  for eg. sssrrrr_purposeno_SortMtc_YYYY-MM-DD-HH24-MI-SS.log

  Log file name contains below information.

  ------ SortMtc EXECUTION START DATE AND TIME ------

  YYYY-MM-DD HH24:MI:SS

  ------ Run Parameters ------

  Displayed all run parameters which are used:
  Data set number             : Data set number starting from 100 to 999
  Run time number             : Run time number starting from 1000 to 9999
  Purpose number              : Purpose number
  Input File Directory        : Input File Directory path
  Log File Directory          : Log File Directory path
  Sort file directory         : Sort file directory path
  Work file directory         : Work file directory path
  Verbose - print details     : Y/n

  ------ File Names ------

  Input file name      : sssrrrr_purposeno.mqt
  Sort file name       : sssrrrr_purposeno.mst
  Log file name        : sssrrrr_purposeno_SortMtc_YYYY_MM_DD_HH_MI_SS.log

  Match sorting ended YYYY-MM-DD HH24:MI:SS - hh:mm:ss to execute

  CMsort version 2.02 - Sort a DOS, WINDOWS, UNIX, MAC, or mixed text file
  (c) 2014 Christian Maas // chmaas@handshake.de // www.chmaas.handshake.de
  Elapsed time: HH:MI:SS

=head3 Terminal

 44_516_SortMtc.pl: Started at DAY MONTH MM HH:MI:SS YYYY

 Dataset                       - >100<
 Run number                    - >1000<
 Purpose number                - >119<
 Input squashed file directory - ><
 Log file directory            - ><
 Sort file directory           - ><
 work file directory           - ><
 Flag - erase work             - >n<
 Flag - Verbose                - >Y<

 Match sorting ended YYYY-MM-DD HH:MI:SS - HH:MI:SS to execute

=head3 Checks leading to procedure abort

   i. Data set number not specified
  ii. Dataset number must be integer and in a range of 100 and 999
 iii. Run time number not specified
  iv. Run number must be integer and in a range of 1000 and 9999
   v. Purpose number not specified
  vi. Purpose number should be integer
 vii. Input file not exist

=head2 Technical

 Script name      - 44_516_SortMtc.pl
 Package Number   - 44
 Procedure Number - 516

=head3 Run parameters

 PARAMETER      DESCRIPTION                       ABV  VARIABLE
 ---------      --------------------------------- ---  -------------------
 asortfldir     Sort file directory                a   $p_sort_file_dir
 dataset        Data set number                    d   $p_data_set
 eraseworkflag  Erase work flag                    e   $p_f_erase_work
 infldir        Input file directory               i   $p_input_file_dir
 logfldir       Log file directory                 l   $p_log_file_dir
 purposeno      Purpose number                     p   $p_prps_no
 runno          Run number                         r   $p_run_no
 verboseflag    Flag - Verbose - print details *   v   $p_f_verbose
 workfldir      work file directory                w   $p_work_file_dir

 * - No argument needed

 Parameter of Dataset number(d) and Run number (r) and Purpose no (p) are mandatory.

 ABV:- Abbreviation for calling run parameter,

 e.g. 44_516_SortMtc.pl -d 100 -r 1000 -p 119 -i d:\test\Temp -a d:\test\Temp
   -l d:\test\Temp\ -v

=head4 Help and defaults

 For detailed help with defaults run: Perl <program_name> -h.

=head3 Subroutines

 Subroutine          Description
 ------------------  -----------------------------------------------------------
 sGetParameters      Initial: Gets run parameters and check input parameter
                      values. Procedure abort with message if any error.

 sWriteLog           Write procedure run summary in log file

 sGetCurTimestamp    Get current formatted timestamp

 sSortmatchFile      Sort matches using system command CMsort if requested.
                      Sort match file generated from output file with file
                      name <input_dateset><input_run_number>_<Purpose_no>.mst

=head4 Called by

 Subroutine          Called by
 ----------------    ------------------------------------

 sGetCurTimestamp    sSortmatchFile
 sGetParameters      Main
 sWriteLog           Main
 sSortmatchFile      Main

=head4 Calling

 Subroutine          Calling Subroutine
 ---------------     ----------------------------------

 sSortmatchFile      sGetCurTimestamp

=head4 Subroutine structure

 Main
  |
  |-- sGetParameters
  |        |-- sGetCurTimestamp
  |
  |-- sWriteLog
  \-- sSortmatchFile

=head3 Perl modules used

 DBI
 Getopt::Simple
 IO::File
 POSIX qw{strftime}

=cut

#######################################################################
# End of 44_516_SortMtc.pl                                            #
#######################################################################
