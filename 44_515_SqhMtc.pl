#!C:/Perl/bin/perl.EXE -w

use strict ;
use warnings ;

use IO::File ;
use Getopt::Simple ;
use POSIX qw{strftime} ;

# Application   : ISPL
# Client        :
# Copyright (c) : 2017 IdentLogic Systems Private Limited
# Author        : Surendra kadam
# Creation Date : 7 May 2017
# Description   : Generates squashed match from sssrrrr_{purpose_no}.ost
# WARNINGS      :
# HARD CODINGS  :
# Limitations   :
# Dependencies  :
# Modifications
# Date       Change Req# Author       Description
# ---------- ----------- ------------ -------------------------------

use constant K_PACKAGE_NO         => 44 ;                       # Package
use constant K_PROCEDURE_NO       => 515 ;                      # Procedure
use constant K_PROCEDURE_NM_U     => "44_515_SqhMtc.pl" ;       # Procedure name
use constant K_PROCEDURE_NM_SYS_U => "Squash Match" ;           # Procedure description

use constant K_EMPTY  => "" ;                                   # Empty field
use constant K_NEW_LN => "\n" ;                                 # New Line Character
use constant K_Y      => "Y" ;                                  # Y for Yes
use constant K_n      => "n" ;                                  # n for no
use constant K_SPACE  => " " ;                                  # Space
use constant K_ONE    => 1 ;                                    # One
use constant K_ZERO   => 0 ;                                    # Zero
use constant K_TAB    => "\t" ;                                 # TAB

# Run parameters
my $p_data_set        = K_EMPTY ;                               # Data set                       - d
my $p_run_no          = K_EMPTY ;                               # Run number                     - r
my $p_prps_no         = K_EMPTY ;                               # Purpose number                 - p
my $p_input_file_dir  = K_EMPTY ;                               # Input file path                - i
my $p_output_file_dir = K_EMPTY ;                               # Output file directory          - o
my $p_log_file_dir    = K_EMPTY ;                               # Log file directory             - l
my $p_disp_mult       = K_EMPTY ;                               # Display progress multiple      - m
my $p_f_verbose       = K_EMPTY ;                               # Flag - Verbose - print details - v

# File directories
my $v_input_file_dir  = K_EMPTY ;                               # Input file directory
my $v_output_file_dir = K_EMPTY ;                               # Output file directory
my $v_log_file_dir    = K_EMPTY ;                               # Log file directory

# File names
my $v_in_file  = K_EMPTY ;                                      # Input file name
my $v_out_file = K_EMPTY ;                                      # Output file name
my $v_log_file = K_EMPTY ;                                      # Log file name

my $v_rec        = K_EMPTY ;                                    # Current record
my $v_in_rec_cnt = K_ZERO ;                                     # Count of records of input file
my @a_flds       = () ;                                         # Array fields

my $v_last_src_id  = K_EMPTY ;                                  # Last search id
my $v_last_fle_id  = K_EMPTY ;                                  # Last file id
my $v_last_prps_no = K_EMPTY ;                                  # Last purpose number

my @a_rec = () ;                                                # Push records into array of records with same id

# Date and time variables
my $v_now_ss = K_EMPTY ;                                        # Time elements - second ...
my $v_now_mi = K_EMPTY ;                                        # ... minute ...
my $v_now_hh = K_EMPTY ;                                        # ... hour - 24 hour format ...
my $v_now_dd = K_EMPTY ;                                        # ... day ...
my $v_now_mm = K_EMPTY ;                                        # ... month ...
my $v_now_yy = K_EMPTY ;                                        # ... year

# Run summary variables
my $v_emp_rec_knt     = K_ZERO ;                                # Empty records count
my $v_emp_knt         = K_ZERO ;                                # Total empty fields count
my $v_src_id_emp_knt  = K_ZERO ;                                # Search id empty count
my $v_fle_id_emp_knt  = K_ZERO ;                                # File id empty count
my $v_dec_emp_knt     = K_ZERO ;                                # Decision field empty count
my $v_score_emp_knt   = K_ZERO ;                                # Score field empty count
my $v_mtc_lvl_emp_knt = K_ZERO ;                                # Match level field empty count
my $v_wrg_flds_knt    = K_ZERO ;                                # Wrong fields count
my $v_wrg_mtc_lvl_knt = K_ZERO ;                                # Wrong match level count
my $v_wrg_dec_knt     = K_ZERO ;                                # Wrong decision field count
my $v_sqh_rec_knt     = K_ZERO ;                                # Squash record count
my $v_not_sqh_rec_knt = K_ZERO ;                                # Improper squash count
my $v_run_sum         = K_EMPTY ;                               # Run summary variable

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

my $t_start_time = time ;                                       # Starting time
my $t_end_time   = K_ZERO ;                                     # End time

&sGetParameters ;                                               # Get run parameters

$v_in_file = $p_data_set . $p_run_no . "_" . $p_prps_no . '.ost' ;           # Input file .ost

$v_out_file = $p_data_set . $p_run_no . "_" . $p_prps_no . '.mqt' ;          # Output file .mqt (Matched Squashed Tab)

$v_log_file =                                                   #
  $p_data_set . $p_run_no . '_' . $p_prps_no . '_'              #
  . 'SqhMtc' . '_'                                              #
  . $v_now_yy . "-"                                             #
  . ( $v_now_mm < 10 ? 0 : K_EMPTY )                            #
  . $v_now_mm . "-"                                             #
  . ( $v_now_dd < 10 ? 0 : K_EMPTY )                            #
  . $v_now_dd . "-"                                             #
  . ( $v_now_hh < 10 ? 0 : K_EMPTY )                            #
  . $v_now_hh . "-"                                             #
  . ( $v_now_mi < 10 ? 0 : K_EMPTY )                            #
  . $v_now_mi . "-"                                             #
  . ( $v_now_ss < 10 ? 0 : K_EMPTY )                            #
  . $v_now_ss . '.log' ;                                        #

open ( my $FILEIN , '<' , $v_input_file_dir . $v_in_file )      # Open input file
  or die 'Can not open input file $p_input_file_path' ;

open ( my $FILEOUT , '>' , $v_output_file_dir . $v_out_file ) or             # Create and open output file
  die "Can not open output file $v_out_file - $!" . K_NEW_LN ;

open ( my $FILELOG , '>' , $v_log_file_dir . $v_log_file ) or   # Create and open log file
  die "Can not open log file $v_log_file - $!" . K_NEW_LN ;

if ( ! -e $v_in_file ) {
   die                                                          #
     __PACKAGE__ . K_SPACE                                      #
     . __FILE__ . K_SPACE                                       #
     . __LINE__ . K_SPACE                                       #
     . "Input file path does not exist" . ' - ' . $v_in_file ;  #
} ## end if ( ! -e $v_in_file )

# Log ##################################################################

my $v_st_ts = &sDateTime ;                                      # Start date and time

my $v_log =                                                     #
  "------ SqhMtc EXECUTION START DATE AND TIME ------"          #
  . K_NEW_LN                                                    #
  . K_NEW_LN                                                    #
  . $v_st_ts . K_NEW_LN . K_NEW_LN                              #
                                                                #
  . "------ Run Parameters ------" . K_NEW_LN                   #
  . "Data set number             :" . K_SPACE                   #
  . $p_data_set . K_NEW_LN                                      #
  . "Run time number             :" . K_SPACE                   #
  . $p_run_no . K_NEW_LN                                        #
  . "Purpose number              :" . K_SPACE                   #
  . $p_prps_no . K_NEW_LN                                       #
  . (
   $p_input_file_dir eq K_EMPTY ? "" :                          #
     "Input File Directory        :" . K_SPACE                  #
     . $p_input_file_dir . K_NEW_LN
  )                                                             #
  . (
   $p_output_file_dir eq K_EMPTY ? "" :                         #
     "Output File Directory       :" . K_SPACE                  #
     . $p_output_file_dir . K_NEW_LN
  )                                                             #
  . (
   $p_log_file_dir eq K_EMPTY ? K_EMPTY :                       #
     "Log File Directory          :" . K_SPACE                  #
     . $p_log_file_dir . K_NEW_LN
  )                                                             #
  . (                                                           #
   $p_f_verbose eq K_Y                                          #
   ? (                                                          #
      $p_disp_mult eq K_EMPTY                                   #
      ? "Multiplier                  : Missing- Default:"       #
        . $p_disp_mult . K_NEW_LN                               #
      : "Multiplier                  : " . $p_disp_mult         #
        . K_NEW_LN                                              #
     )                                                          #
   : K_EMPTY                                                    #
  )                                                             #
  . "Verbose flag                : "                            #
  . (
   $p_f_verbose eq K_Y ? "Yes" . K_NEW_LN :                     #
     "no" . K_NEW_LN
  )                                                             #
  . K_NEW_LN                                                    #
  . "------ File Names ------" . K_NEW_LN                       #
  . "Input file name  :" . K_SPACE . $v_in_file . K_NEW_LN      #
  . "Output file name :" . K_SPACE . $v_out_file . K_NEW_LN     #
  . "Log  file name   :" . K_SPACE . $v_log_file . K_NEW_LN     #
  . K_NEW_LN ;

print $FILELOG $v_log ;                                         # Write log file

# End Log #############################################################

while ( $v_rec = <$FILEIN> ) {

   chomp $v_rec ;                                               # Remove newline character

   $v_in_rec_cnt ++ ;                                           # Count of records of input file

   if ( $v_rec eq K_EMPTY ) {
      $v_emp_rec_knt ++ ;                                       # Empty records count
      next ;
   }                                                            # Skip record

   # Verbose

   if ( $p_f_verbose eq K_Y ) {                                 # If verbose flag is on
      if ( $v_in_rec_cnt == $p_disp_mult ) {
         $t_end_time = time - $t_start_time ;
         print "Display" . K_SPACE . $p_disp_mult . K_SPACE . "records in" . K_SPACE . $t_end_time . K_SPACE . "seconds to execute" . K_NEW_LN ;
         $p_disp_mult = $p_disp_mult * 2 ;
      }
   } ## end if ( $p_f_verbose eq K_Y)

   @a_flds = split K_TAB , $v_rec ;                             # Split current records into fields

   my $vl_src_id  = K_EMPTY ;                                   # Source id of each record
   my $vl_fle_id  = K_EMPTY ;                                   # File id of each record
   my $vl_dec     = K_EMPTY ;                                   # Decision of each record
   my $vl_score   = K_EMPTY ;                                   # Score of each record
   my $vl_prps_no = K_EMPTY ;                                   # Purpose no of each record
   my $vl_mtc_lvl = K_EMPTY ;                                   # Match level of each record

   for ( my $idx = 0 ; $idx < $#a_flds + 1 ; $idx ++ ) {

      $vl_src_id  = $a_flds[ 0 ] ;                              # Source id of each record
      $vl_fle_id  = $a_flds[ 1 ] ;                              # File id of each record
      $vl_dec     = $a_flds[ 2 ] ;                              # Decision of each record
      $vl_score   = $a_flds[ 3 ] ;                              # Score of each record
      $vl_prps_no = $a_flds[ 4 ] ;                              # Purpose no of each record
      $vl_mtc_lvl = $a_flds[ 5 ] ;                              # Match level of each record

      if ( $vl_src_id ne K_EMPTY and $vl_fle_id ne K_EMPTY ) {
         if ( ( ( $vl_src_id ne $v_last_src_id ) or ( $vl_fle_id ne $v_last_fle_id ) )
            and $v_in_rec_cnt > 1 ) {
            &sCloseLastId ;                                     # Close Last id
         }
      } ## end if ( $vl_src_id ne K_EMPTY...)

      if ( $vl_src_id ne K_EMPTY and $vl_fle_id ne K_EMPTY ) {
         $v_last_src_id = $vl_src_id ;                          #Last search id
         $v_last_fle_id = $vl_fle_id ;
      }
   } ## end for ( my $idx = 0 ; $idx...)

   if ( $vl_src_id eq K_EMPTY ) {
      $v_emp_knt ++ ;                                           # Empty fields counts
      $v_src_id_emp_knt ++ ;                                    # Search id empty counts
   }

   if ( $vl_fle_id eq K_EMPTY ) {
      $v_emp_knt ++ ;                                           # Empty fields counts
      $v_fle_id_emp_knt ++ ;                                    # File id empty counts
   }

   if ( $vl_mtc_lvl eq K_EMPTY ) {
      $v_emp_knt ++ ;                                           # Empty fields counts
      $v_mtc_lvl_emp_knt ++ ;                                   # Match level empty count
   }

   if ( $vl_dec eq K_EMPTY ) {                                  #
      $v_emp_knt ++ ;                                           # Empty fields counts
      $v_dec_emp_knt ++ ;                                       # Empty decision field count
   }                                                            #

   if ( $vl_score eq K_EMPTY ) {                                #
      $v_emp_knt ++ ;                                           # Empty fields counts
      $v_score_emp_knt ++ ;                                     # Empty score field count
   }

   if ( $vl_dec !~ /[U|R|A]/ ) {
      $v_wrg_flds_knt ++ ;                                      # Wrong fields count
      $v_wrg_dec_knt ++ ;                                       # Wrong decision field count
   }

   if ( $vl_mtc_lvl !~ /[T|C|L]/ ) {
      $v_wrg_flds_knt ++ ;                                      # Wrong fields count
      $v_wrg_mtc_lvl_knt ++ ;                                   # Wrong match level field count
   }

   # Push records into array until new id will get
   push @a_rec , $v_rec ;
} ## end while ( $v_rec = <$FILEIN>)
#####################################################################
# End of while                                                      #
#####################################################################

if ( $v_in_rec_cnt > 1 ) {                                      #Close last Id - more than one input record
   &sCloseLastId ;                                              # Call subroutine sCloseLastId
}

# Log #################################################################
$v_run_sum =                                                    #
  "------ Run summary ------" . K_NEW_LN                        #
  . "No of records read   :" . K_SPACE                          #
  . $v_in_rec_cnt . K_NEW_LN                                    #
  . (
   $v_emp_rec_knt != 0 ?                                        #
     "Empty records count  :" . K_SPACE                         #
     . $v_emp_rec_knt . K_NEW_LN
   :                                                            #
     K_EMPTY                                                    #
  )                                                             #
  . (
   $v_emp_knt != 0 ?                                            #
     "Empty counts         :" . K_SPACE                         #
     . $v_emp_knt . K_NEW_LN
   :                                                            #
     K_EMPTY                                                    #
  )                                                             #
  . (
   $v_src_id_emp_knt != 0 ?                                     #
     " - Search id         :" . K_SPACE                         #
     . $v_src_id_emp_knt . K_NEW_LN
   :                                                            #
     K_EMPTY                                                    #
  )                                                             #
  . (
   $v_fle_id_emp_knt != 0 ?                                     #
     " - File id           :" . K_SPACE                         #
     . $v_fle_id_emp_knt . K_NEW_LN
   :                                                            #
     K_EMPTY                                                    #
  )                                                             #
  . (
   $v_dec_emp_knt != 0 ?                                        #
     " - Decision          :" . K_SPACE                         #
     . $v_dec_emp_knt . K_NEW_LN
   :                                                            #
     K_EMPTY                                                    #
  )                                                             #
  . (
   $v_score_emp_knt != 0 ?                                      #
     " - Score             :" . K_SPACE                         #
     . $v_score_emp_knt . K_NEW_LN
   :                                                            #
     K_EMPTY                                                    #
  )                                                             #
  . (
   $v_mtc_lvl_emp_knt != 0 ?                                    #
     " - Match level       :" . K_SPACE                         #
     . $v_mtc_lvl_emp_knt . K_NEW_LN
   :                                                            #
     K_EMPTY                                                    #
  )                                                             #
  . (
   $v_wrg_flds_knt != 0 ?                                       #
     "Wrong fields count   :" . K_SPACE                         #
     . $v_wrg_flds_knt . K_NEW_LN
   :                                                            #
     K_EMPTY                                                    #
  )                                                             #
  . (
   $v_wrg_dec_knt != 0 ?                                        #
     " - Decision          :" . K_SPACE                         #
     . $v_wrg_dec_knt . K_NEW_LN
   :                                                            #
     K_EMPTY                                                    #
  )                                                             #
  . (
   $v_wrg_mtc_lvl_knt != 0 ?                                    #
     " - Match level       :" . K_SPACE                         #
     . $v_wrg_mtc_lvl_knt . K_NEW_LN
   :                                                            #
     K_EMPTY                                                    #
  )                                                             #
  . (
   $v_not_sqh_rec_knt != 0 ?                                    #
     "Improper squash      :" . K_SPACE                         #
     . $v_not_sqh_rec_knt . K_NEW_LN
   :                                                            #
     K_EMPTY                                                    #
  )                                                             #
  . (
   $v_sqh_rec_knt != 0 ?                                        #
     "Total squash records :" . K_SPACE                         #
     . $v_sqh_rec_knt . K_SPACE
   :                                                            #
     K_EMPTY                                                    #
  )
  . (
   ( $v_in_rec_cnt / 3 ) == $v_sqh_rec_knt ?                    #
     "Perfect Squash" . K_NEW_LN
   :                                                            #
     K_EMPTY . K_NEW_LN
  ) ;

$t_end_time = time - $t_start_time ;                            # Total time taken

my $v_end_ts = &sDateTime ;                                     # End timestamp

print $FILELOG $v_run_sum ;                                     # Write run summary in log file

print $FILELOG                                                  #
  K_NEW_LN . "Ended "                                           #
  . K_SPACE . $v_end_ts . '-' . K_SPACE                         #
  . strftime ( "\%H:\%M:\%S" , gmtime ( $t_end_time ) )         #
  . K_SPACE . "to execute" . K_NEW_LN ;                         #

# End Log #############################################################

close $FILEIN
  or die "Can not close input file $v_in_file - $!" . K_NEW_LN ;    #Close input file

close $FILEOUT
  or die "Can not close output file $v_out_file - $!" . K_NEW_LN ;    #Close input file

close $FILELOG                                                  # Close log file
  or die "Can not close log file $v_log_file - $!" . K_NEW_LN ;

#####################################################################
# End of Main                                                       #
#####################################################################

sub sDateTime {

   # sDateTime subroutine to get current date and time

   (
      $v_now_ss ,                                               # Seconds
      $v_now_mi ,                                               # Minutes
      $v_now_hh ,                                               # Hours
      $v_now_dd ,                                               # Date
      $v_now_mm ,                                               # Month
      $v_now_yy                                                 # Year
   ) = ( localtime )[ 0 , 1 , 2 , 3 , 4 , 5 ] ;                 # Get time elements

   $v_now_yy = $v_now_yy + 1900 ;                               # Adjust year
   $v_now_mm = $v_now_mm + 1 ;                                  # Adjust month

   if ( $v_now_mm < 10 ) {
      $v_now_mm = K_ZERO . $v_now_mm ;
   }

   if ( $v_now_dd < 10 ) {
      $v_now_dd = K_ZERO . $v_now_dd ;
   }

   if ( $v_now_hh < 10 ) {
      $v_now_hh = K_ZERO . $v_now_hh ;
   }

   if ( $v_now_mi < 10 ) {
      $v_now_mi = K_ZERO . $v_now_mi ;
   }

   if ( $v_now_ss < 10 ) {
      $v_now_ss = K_ZERO . $v_now_ss ;
   }

   my $v_curr_ts = $v_now_yy . "-" . $v_now_mm . "-" . $v_now_dd . " " . $v_now_hh . ":" . $v_now_mi . ":" . $v_now_ss ;

   return $v_curr_ts ;
} ## end sub sDateTime
#######################################################################
# End of sub sDateTime                                                #
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
      dataset => {                                              # Data set number
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Data set' ,
         order   => 2 ,
        } ,
      runno => {                                                # Run time number
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Run number' ,
         order   => 3 ,
        } ,
      purposeno => {                                            # Purpose number
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Purpose number' ,
         order   => 4 ,
        } ,
      infldir => {                                              # Input file directory
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Input file directory - optional' ,
         order   => 5 ,
        } ,
      outfldir => {                                             # Output file directory
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Output file directory - optional' ,
         order   => 6 ,
        } ,
      logfldir => {                                             # Log file directory
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Log file directory - optional' ,
         order   => 7 ,
        } ,
      multdspl => {                                             # Multiplier
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Display progress multiple - optional' ,
         order   => 8 ,
        } ,
      verboseflag => {                                          # Verbose flag
         type    => '!' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Print job details - no argument needed - optional' ,
         order   => 9 ,
      } ,
   } ;

   my ( $vgp_parameters ) = Getopt::Simple -> new () ;
   if ( ! $vgp_parameters -> getOptions ( $vgp_get_options , "Usage: $0 [options]" ) ) {
      exit ( -1 ) ;                                             # Failure
   }

   # Get run time parameters value
   $p_data_set =                                                # Dataset                             - d
     $$vgp_parameters{ 'switch' }{ 'dataset' } || K_EMPTY ;

   $p_run_no =                                                  # Run number                          - r
     $$vgp_parameters{ 'switch' }{ 'runno' } || K_EMPTY ;       #

   $p_prps_no =                                                 # Purpose number                      - p
     $$vgp_parameters{ 'switch' }{ 'purposeno' } || K_EMPTY ;   #

   $p_input_file_dir =                                          # Input file directory                - i
     $$vgp_parameters{ 'switch' }{ 'infldir' } || K_EMPTY ;

   $p_output_file_dir =                                         # Output file directory               - o
     $$vgp_parameters{ 'switch' }{ 'outfldir' } || K_EMPTY ;

   $p_log_file_dir =                                            # Log file directory                  - l
     $$vgp_parameters{ 'switch' }{ 'logfldir' } || K_EMPTY ;

   $p_disp_mult =                                               # Display progress multiple           - m
     $$vgp_parameters{ 'switch' }{ 'multdspl' } || K_EMPTY ;

   $p_f_verbose =                                               # Flag - Verbose - print details      - v
     $$vgp_parameters{ 'switch' }{ 'verboseflag' } ;

   if   ( $p_f_verbose eq '1' ) { $p_f_verbose = 'Y' ; }        # Convert to ISPL convention
   else                         { $p_f_verbose = 'n' ; }

   if ( $p_f_verbose eq 'Y' ) {                                 # Display run parameters if verbose option
      print "$0: Started at" . K_SPACE . ( localtime ) . K_NEW_LN . K_NEW_LN ,    #
        'Dataset                     - >' , $p_data_set ,        '<' , K_NEW_LN , #
        'Run number                  - >' , $p_run_no ,          '<' , K_NEW_LN , #
        'Input file directory        - >' , $p_input_file_dir ,  '<' , K_NEW_LN , #
        'Output file directory       - >' , $p_output_file_dir , '<' , K_NEW_LN , #
        'Log file directory          - >' , $p_log_file_dir ,    '<' , K_NEW_LN , #
        'Display progress multiple   - >' , ( $p_disp_mult eq K_EMPTY ? "Default- 100000" : $p_disp_mult ) , '<' , K_NEW_LN ,    #
        'Flag - Verbose              - >' , $p_f_verbose , '<' , K_NEW_LN , K_NEW_LN ;
   } ## end if ( $p_f_verbose eq 'Y')

   $v_input_file_dir  = ( $p_input_file_dir eq K_EMPTY  ? './' : $p_input_file_dir ) ;
   $v_output_file_dir = ( $p_output_file_dir eq K_EMPTY ? './' : $p_output_file_dir ) ;
   $v_log_file_dir    = ( $p_log_file_dir eq K_EMPTY    ? './' : $p_log_file_dir ) ;

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

   if ( $v_input_file_dir ne './' ) {                           # If input file directory not default (./)
      if (
         substr ( $v_input_file_dir , -1 , 1 ) ne '\\'          # If input file directory not end with back slash (\\)
        ) {
         $v_input_file_dir .= '\\' ;                            # Add back slash at end
      }
   } ## end if ( $v_input_file_dir...)

   $v_input_file_dir =~ s/\\/\//g ;                             # Replace back slash to forward slash

   if ( $v_output_file_dir ne './' ) {                          # If output file directory not default (./)
      if (
         substr ( $v_output_file_dir , -1 , 1 ) ne '\\'         # If output file directory not end with back slash (\\)
        ) {
         $v_output_file_dir .= '\\' ;                           # Add back slash at end
      }
   } ## end if ( $v_output_file_dir...)

   $v_output_file_dir =~ s/\\/\//g ;                            # Replace back slash to forward slash

   if ( $v_log_file_dir ne './' ) {                             # If log file directory not default (./)
      if (
         substr ( $v_log_file_dir , -1 , 1 ) ne '\\'            # If log file directory not end with back slash (\\)
        ) {
         $v_log_file_dir .= '\\' ;                              # Add back slash at end
      }
   } ## end if ( $v_log_file_dir ne...)

   $v_log_file_dir =~ s/\\/\//g ;                               # Replace back slash to forward slash

   if ( $p_disp_mult eq K_EMPTY ) {                             # Default value of multiplier
      $p_disp_mult = 100000 ;
   }

} ## end sub sGetParameters
#######################################################################
# End of subroutine sGetParameters                                    #
#######################################################################

sub sCloseLastId {

   # Close last id and process records of last same id

   my $v_grp_rec = K_EMPTY ;
   my $v_rec_knt = K_ZERO ;                                     # Read record count

   my @a_recs_grp     = @a_rec ;                                # Sort array for process record
   my @a_flds_grp_rec = () ;                                    # Split group records in array of fields

   my @a_src_id  = () ;                                         # Array of search id
   my @a_fle_id  = () ;                                         # Array of file id
   my @a_dec     = () ;                                         # Array of Decision
   my @a_score   = () ;                                         # Array of score
   my @a_mtc_lvl = () ;                                         # Array of match level

   my $v_search_id_t = K_EMPTY ;                                # Typical search id
   my $v_search_id_c = K_EMPTY ;                                # Conservative search id
   my $v_search_id_l = K_EMPTY ;                                # Loose search id

   my $v_gr_src_id = K_EMPTY ;                                  # Search id of group
   my $v_gr_fle_id = K_EMPTY ;                                  # File id of group

   my $v_fle_id_t = K_EMPTY ;                                   # Typical file id
   my $v_fle_id_c = K_EMPTY ;                                   # Conservative file id
   my $v_fle_id_l = K_EMPTY ;                                   # Loose file id

   my $v_dec   = K_EMPTY ;                                      # Decision
   my $v_score = K_EMPTY ;                                      # Score

   my $v_dec_t = K_EMPTY ;                                      # Typical Match Decision
   my $v_dec_c = K_EMPTY ;                                      # Conservative Match Decision
   my $v_dec_l = K_EMPTY ;                                      # Loose Match Decision

   my $v_score_t = K_EMPTY ;                                    # Typical Match Score
   my $v_score_c = K_EMPTY ;                                    # Conservative Match Score
   my $v_score_l = K_EMPTY ;                                    # Loose Match Score

   my $v_mtc_lvl_t = K_EMPTY ;                                  # Match level Typical
   my $v_mtc_lvl_c = K_EMPTY ;                                  # Match level Conservative
   my $v_mtc_lvl_l = K_EMPTY ;                                  # Match level Loose

   my $v_prv_src_id   = K_EMPTY ;                               # Previous search id
   my $v_prv_fle_id   = K_EMPTY ;                               # Previous file id
   my $v_prv_prps_no  = K_EMPTY ;                               # Previous purpose no
   my $vl_mtc_lvl_grp = K_EMPTY ;                               # Previous match level
   my $v_sqh_rec      = K_EMPTY ;                               # Squash record

   foreach $v_grp_rec ( @a_recs_grp ) {                         # Group of same search id record

      $v_rec_knt ++ ;                                           # Record counter

      @a_flds_grp_rec = split K_TAB , $v_grp_rec ;              # Split ggroup records

      $v_prv_src_id  = $a_flds_grp_rec[ 0 ] ;                   # Previous search id
      $v_prv_fle_id  = $a_flds_grp_rec[ 1 ] ;                   # Previous file id
      $v_prv_prps_no = $a_flds_grp_rec[ 4 ] ;                   # Previous purpose no

      $v_dec          = $a_flds_grp_rec[ 2 ] ;                  # Decision
      $v_score        = $a_flds_grp_rec[ 3 ] ;                  # Score
      $vl_mtc_lvl_grp = $a_flds_grp_rec[ 5 ] ;

      push @a_src_id , $v_prv_src_id ;                          # Push search id into array

      $v_search_id_t = $a_src_id[ 0 ] ;                         # Typical search id
      $v_search_id_c = $a_src_id[ 1 ] ;                         # Conservative search id
      $v_search_id_l = $a_src_id[ 2 ] ;                         # Loose search id

      push @a_fle_id , $v_prv_fle_id ;                          # Push file id into array

      $v_fle_id_t = $a_fle_id[ 0 ] ;                            # Typical file id
      $v_fle_id_c = $a_fle_id[ 1 ] ;                            # Conservative file id
      $v_fle_id_l = $a_fle_id[ 2 ] ;                            # Loose file id

      push @a_dec , $v_dec ;                                    # Push decision of all match level in an array

      $v_dec_t = $a_dec[ 0 ] ;                                  # Typical Match Decision
      $v_dec_c = $a_dec[ 1 ] ;                                  # Conservative Match Decision
      $v_dec_l = $a_dec[ 2 ] ;                                  # Loose Match Decision

      push @a_score , $v_score ;                                # Push score of all match level in an array

      $v_score_t = $a_score[ 0 ] ;                              # Typical Match Score
      $v_score_c = $a_score[ 1 ] ;                              # Conservative Match Score
      $v_score_l = $a_score[ 2 ] ;                              # Loose Match Score

      push @a_mtc_lvl , $vl_mtc_lvl_grp ;                       # Push match level into array

      $v_mtc_lvl_t = $a_mtc_lvl[ 0 ] ;                          # Match level Typical
      $v_mtc_lvl_c = $a_mtc_lvl[ 1 ] ;                          # Match level Conservative
      $v_mtc_lvl_l = $a_mtc_lvl[ 2 ] ;                          # Match level Loose

   } ## end foreach $v_grp_rec ( @a_recs_grp)

   # Check Search id of Typical Conservative and Loose are same or not
   if (   $v_search_id_t eq $v_search_id_c
      and $v_search_id_c eq $v_search_id_l
      and $v_search_id_l eq $v_search_id_t ) {

      $v_gr_src_id = $v_search_id_t ;
   } ## end if ( $v_search_id_t eq...)

   # Check File id of Typical Conservative and Loose are same or not
   if (   $v_fle_id_t eq $v_fle_id_c
      and $v_fle_id_c eq $v_fle_id_l
      and $v_fle_id_l eq $v_fle_id_t ) {

      $v_gr_fle_id = $v_fle_id_t ;
   } ## end if ( $v_fle_id_t eq $v_fle_id_c...)

   if (   $v_gr_src_id ne K_EMPTY
      and $v_gr_fle_id ne K_EMPTY
      and length $v_dec_t == 1
      and length $v_dec_c == 1
      and length $v_dec_l == 1
      and $v_dec_t =~ /[A|R|U]/
      and $v_dec_c =~ /[A|R|U]/
      and $v_dec_l =~ /[A|R|U]/
      and length $v_score_t == 3
      and length $v_score_c == 3
      and length $v_score_l == 3
      and ( $v_score_t =~ /\d/g and ( $v_score_t >= 000 or $v_score_t <= 100 ) )
      and ( $v_score_c =~ /\d/g and ( $v_score_c >= 000 or $v_score_c <= 100 ) )
      and ( $v_score_l =~ /\d/g and ( $v_score_l >= 000 or $v_score_l <= 100 ) )
      and $v_mtc_lvl_t eq "T"
      and $v_mtc_lvl_c eq "C"
      and $v_mtc_lvl_l eq "L" ) {

      $v_sqh_rec_knt ++ ;                                       #Squash records count

      $v_sqh_rec                                                # Squash record
        = $v_prv_src_id
        . K_TAB
        . $v_prv_fle_id
        . K_TAB
        . $v_prv_prps_no
        . K_TAB
        . $v_dec_t
        . K_TAB
        . $v_score_t
        . K_TAB
        . $v_dec_c
        . K_TAB
        . $v_score_c
        . K_TAB
        . $v_dec_l
        . K_TAB
        . $v_score_l
        . K_NEW_LN ;

      print $FILEOUT $v_sqh_rec ;                               # Print Squash record
   } ## end if ( $v_gr_src_id ne K_EMPTY...)
   else {
      $v_not_sqh_rec_knt ++ ;
      $v_sqh_rec                                                # Squash record
        = $v_prv_src_id
        . K_TAB
        . $v_prv_fle_id
        . K_TAB
        . $v_prv_prps_no
        . K_TAB
        . $v_dec_t
        . K_TAB
        . $v_score_t
        . K_TAB
        . $v_dec_c
        . K_TAB
        . $v_score_c
        . K_TAB
        . $v_dec_l
        . K_TAB
        . $v_score_l
        . K_NEW_LN ;

      print $FILELOG "Record no :" . K_SPACE . $v_rec_knt . K_SPACE . "Error Message :" . K_SPACE . "Improper squash record" . K_NEW_LN . "Record :" . K_SPACE . $v_sqh_rec . K_NEW_LN ;
   } ## end else [ if ( $v_gr_src_id ne K_EMPTY...)]

   @a_rec = () ;                                                # Null @_rec

} ## end sub sCloseLastId
#######################################################################
# End of subroutine sCloseLastId                                      #
#######################################################################
