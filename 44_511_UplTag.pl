#!C:/Perl/bin/perl.EXE -w

use strict ;
use warnings ;

use IO::File ;                                                  # IO::File - supply object methods for filehandles
use Getopt::Simple ;                                            # Get run parameter module
use POSIX qw{strftime} ;                                        # For current date and time
use DBI ;                                                       # Access database

# Application   : ISPL
# Client        :
# Copyright (c) : 2017 IdentLogic Systems Private Limited
# Author        : Surendra kadam
# Creation Date : 21 April 2017
# Description   :
# WARNINGS      :
# HARD CODINGS  :
# Limitations   :
# Dependencies  :
# Modifications
# Date       Change Req# Author       Description
# ---------- ----------- ------------ -------------------------------

use constant K_PACKAGE_NO         => 44 ;                       # Package number
use constant K_PROCEDURE_NO       => 511 ;                      # Procedure number
use constant K_PROCEDURE_NM_U     => "44_511_UplTag.pl" ;       # Procedure name
use constant K_PROCEDURE_NM_SYS_U => "Upload squashed tags" ;   # Procedure description

use constant K_EMPTY   => "" ;                                  # Empty field
use constant K_NEW_LN  => "\n" ;                                # New Line Character
use constant K_Y       => "Y" ;                                 # Y for Yes
use constant K_n       => "n" ;                                 # n for no
use constant K_SPACE   => " " ;                                 # Space
use constant K_ONE     => 1 ;                                   # One
use constant K_ZERO    => 0 ;                                   # Zero
use constant K_SNG_APO => "'" ;                                 # Single apostrophe

my $p_set_data      = K_EMPTY ;                                 # Data set                       - d
my $p_run_no        = K_EMPTY ;                                 # Run number                     - r
my $p_tag_file_dir  = K_EMPTY ;                                 # Input file directory           - a
my $p_in_user       = K_EMPTY ;                                 # Database user                  - u
my $p_in_dbpw       = K_EMPTY ;                                 # Database password              - g
my $p_in_dbname     = K_EMPTY ;                                 # Database (Sid) name            - x
my $p_in_dbtype     = K_EMPTY ;                                 # Database connection string     - c
my $p_in_dbport     = K_EMPTY ;                                 # Database port                  - p
my $p_in_dbserver   = K_EMPTY ;                                 # Database Server (Host) name    - t
my $p_log_file_dir  = K_EMPTY ;                                 # Log file directory             - l
my $p_work_file_dir = K_EMPTY ;                                 # work file directory            - w
my $p_f_verbose     = K_EMPTY ;                                 # Flag - Verbose - print details - v

my $v_tag_file = K_EMPTY ;                                      # Stores tag file name
my $v_log_file = K_EMPTY ;                                      # Stores log file name

my $v_log_file_dir      = K_EMPTY ;                             # Log file directory
my $v_tag_file_dir      = K_EMPTY ;                             # Tag file directory
my $v_work_file_dir     = K_EMPTY ;                             # Temporary file working directory
my $v_work_file_dir_cmd = K_EMPTY ;                             # Temporary file working directory for cmd
my $v_sql_ldr_log_file  = K_EMPTY ;                             # Control file to run sql loader

my $v_out_rec_knt          = K_ZERO ;                           # Count of output record
my $v_sec_to_exe_upl_tag   = K_EMPTY ;                          # Seconds to execute upload tags
my $v_upl_tag_end_ts       = K_EMPTY ;                          # Upload tags End timestamp
my $v_sql_ldr_sg           = K_EMPTY ;                          # Sql loader string for control file
my $v_f_error              = K_n ;                              # Flag - Error
my $v_logical_rec_read_knt = K_ZERO ;                           # Logical record count
my $v_suc_loaded_knt       = K_ZERO ;                           # Successfully loaded count
my $dbh                    = K_EMPTY ;                          # Database handle
my $v_start_ts             = K_EMPTY ;                          # Varible to store timestamp
my $v_job_sn               = K_EMPTY ;                          # Varible to store job sequence number
my $v_now_ts               = K_EMPTY ;                          # Varible to store time
my $v_tp                   = &sGetTcpIp ;                       # Get TCP/IP of machine

my $FILELOG = IO::File -> new ;                                 # Declaration of filehandle of log file
my $FILECTL = IO::File -> new ;                                 # Declaration of filehandle of control file for sql loader

my $v_now_ss = K_EMPTY ;                                        # Time elements - second
my $v_now_mi = K_EMPTY ;                                        # Minute
my $v_now_hh = K_EMPTY ;                                        # Hour - 24 hour format
my $v_now_dd = K_EMPTY ;                                        # Day
my $v_now_mm = K_EMPTY ;                                        # Month
my $v_now_yy = K_EMPTY ;                                        # Year

# START ##############################################################

(
   $v_now_ss ,                                                  # Seconds
   $v_now_mi ,                                                  # Minutes
   $v_now_hh ,                                                  # Hours
   $v_now_dd ,                                                  # Date
   $v_now_mm ,                                                  # Month
   $v_now_yy                                                    # Year
) = ( localtime )[ 0 , 1 , 2 , 3 , 4 , 5 ] ;                    # Get time elements

$v_now_yy = $v_now_yy + 1900 ;                                  # Adjust year
$v_now_mm = $v_now_mm + 1 ;                                     # Adjust month

my $v_start_timestamp =                                         # Start timestamp
  $v_now_yy . '-' . ( $v_now_mm < 10 ? '0' . $v_now_mm : $v_now_mm ) . '-' .    # Add prefix 0 to single digit month
  ( $v_now_dd < 10 ? '0' . $v_now_dd : $v_now_dd ) . ' ' .      # Add prefix 0 to single digit date
  ( $v_now_hh < 10 ? '0' . $v_now_hh : $v_now_hh ) . ':' .      # Add prefix 0 to single digit hour
  ( $v_now_mi < 10 ? '0' . $v_now_mi : $v_now_mi ) . ':' .      # Add prefix 0 to single digit minute
  ( $v_now_ss < 10 ? '0' . $v_now_ss : $v_now_ss )              # Add prefix 0 to single digit second
  ;                                                             #

my $v_start_time = time () ;                                    # Start time

&sGetParameters ;                                               # Get run parameters
&sInit       () ;                                               # Inserts begin details in table atul . T_XST_XBG
&sUplTagFile () ;                                               # Upload tagged data
&sWriteLog   () ;                                               # Write log
&sWindUp     () ;                                               # Insert procedure end record and update begin record
exit ( 1 ) ;

######################################################################
# End of Main script                                                 #
######################################################################

sub sGetCurTimestamp {
   # Get current timestamp

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

   # Initial: Gets run parameters and check input parameter
   # values. Procedure abort with message if any error.

   my ( $vgp_get_options ) = {
      help => {
         type    => '' ,
         env     => '-' ,
         default => '' ,
         verbose => '' ,
         order   => 1 ,
        } ,
      set_of_data => {                                          # Data set number
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Data set number' ,
         order   => 2 ,
        } ,
      runno => {                                                # Run time number
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Run time number' ,
         order   => 3 ,
        } ,
      atagfldir => {
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Input file directory - optional' ,
         order   => 4 ,
        } ,
      logfldir => {
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Log file directory - optional' ,
         order   => 5 ,
        } ,
      workfldir => {
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'work file directory - optional' ,
         order   => 6 ,
        } ,
      user => {
         type    => "=s" ,
         env     => "-" ,
         default => 'dedupe' ,
         verbose => "Database user name" ,
         order   => 7 ,
        } ,
      guptshabd => {
         type    => '=s' ,
         env     => '-' ,
         default => 'dedupe' ,
         verbose => "Database user password" ,
         order   => 8 ,
        } ,
      port => {
         type    => '=s' ,
         env     => '-' ,
         default => '1521' ,
         verbose => "Database port" ,
         order   => 9 ,
        } ,
      tcpip => {
         type    => '=s' ,
         env     => '-' ,
         default => "192.168.1.214" ,
         verbose => "Database host - Server or TCP/IP" ,
         order   => 10 ,
        } ,
      xdbname => {
         type    => '=s' ,
         env     => '-' ,
         default => 'IIBM' ,
         verbose => "Database (SID) name" ,
         order   => 11 ,
        } ,
      cncdb => {
         type    => '=s' ,
         env     => '-' ,
         default => 'Oracle' ,
         verbose => "Database type connection - Oracle, ODBC, etc." ,
         order   => 12 ,
        } ,
      verboseflag => {
         type    => '!' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Print job details - no argument needed - optional' ,
         order   => 13 ,
      } ,
   } ;

   my ( $parameters ) = Getopt::Simple -> new () ;
   if ( ! $parameters -> getOptions ( $vgp_get_options , "Usage: $0 [options]" ) ) {
      exit ( -1 ) ;                                             # Failure
   }

   # Get run time parameters value
   $p_set_data = $$parameters{ 'switch' }{ 'set_of_data' } || K_EMPTY ;    # Dataset                        - d
   $p_run_no   = $$parameters{ 'switch' }{ 'runno' }       || K_EMPTY ;    # Run number                     - r

   $p_tag_file_dir  = $$parameters{ 'switch' }{ 'atagfldir' } || K_EMPTY ; # tag file directory            - a
   $p_log_file_dir  = $$parameters{ 'switch' }{ 'logfldir' }  || K_EMPTY ; # Log file directory             - l
   $p_work_file_dir = $$parameters{ 'switch' }{ 'workfldir' } || K_EMPTY ; # work file directory            - w

   $p_in_user     = $$parameters{ 'switch' }{ 'user' }      || K_EMPTY ;   # Database user                  - u
   $p_in_dbpw     = $$parameters{ 'switch' }{ 'guptshabd' } || K_EMPTY ;   # Database password              - g
   $p_in_dbname   = $$parameters{ 'switch' }{ 'xdbname' }   || K_EMPTY ;   # Database (Sid) name            - x
   $p_in_dbtype   = $$parameters{ 'switch' }{ 'cncdb' }     || K_EMPTY ;   # Database connection string     - c
   $p_in_dbport   = $$parameters{ 'switch' }{ 'port' }      || K_EMPTY ;   # Database port                  - p
   $p_in_dbserver = $$parameters{ 'switch' }{ 'tcpip' }     || K_EMPTY ;   # Database Server (Host) name    - t
   $p_f_verbose = $$parameters{ 'switch' }{ 'verboseflag' } ;   # Flag - Verbose - print details - v

   if   ( $p_f_verbose eq '1' ) { $p_f_verbose = 'Y' ; }        # Convert to ISPL convention
   else                         { $p_f_verbose = 'n' ; }

   if ( $p_f_verbose eq 'Y' ) {                                 # Display run parameters if verbose option
      print "$0: Started at" . K_SPACE . ( localtime ) . K_NEW_LN . K_NEW_LN ,    #
        'Dataset                     :' , $p_set_data ,      K_NEW_LN ,               #
        'Run number                  :' , $p_run_no ,        K_NEW_LN ,               #
        'Tag file directory          :' , $p_tag_file_dir ,  K_NEW_LN ,               #
        'Log file directory          :' , $p_log_file_dir ,  K_NEW_LN ,               #
        'Database user               :' , $p_in_user ,       K_NEW_LN ,               #
        'Database password           :' , $p_in_dbpw ,       K_NEW_LN ,               #
        'Database (Sid) name         :' , $p_in_dbname ,     K_NEW_LN ,               #
        'Databaseconnection string   :' , $p_in_dbtype ,     K_NEW_LN ,               #
        'Database port               :' , $p_in_dbport ,     K_NEW_LN ,               #
        'Database Server (Host) name :' , $p_in_dbserver ,   K_NEW_LN ,               #
        'work file directory         :' , $p_work_file_dir , K_NEW_LN ,               #
        'Flag - Verbose              :' , $p_f_verbose ,     K_NEW_LN , K_NEW_LN ;    #
   } ## end if ( $p_f_verbose eq 'Y')

   # Check All directory path empty or not
   $v_log_file_dir  = ( $p_log_file_dir eq K_EMPTY  ? './' : $p_log_file_dir ) ;
   $v_tag_file_dir  = ( $p_tag_file_dir eq K_EMPTY  ? './' : $p_tag_file_dir ) ;
   $v_work_file_dir = ( $p_work_file_dir eq K_EMPTY ? './' : $p_work_file_dir ) ;

   if ( $p_set_data eq K_EMPTY ) {                              # Abort if data set specified
      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE .          #
        __LINE__ . "JOB ABANDONDED - Missing data set number" . K_NEW_LN ;
   }

   if (
      $p_set_data =~ /\D/ or                                    # Abort if data set number not numeric
      $p_set_data < 100   or                                    # Abort if data set number less than 100
      $p_set_data > 999
     ) {                                                        # Abort if data set number greater than 999
      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . "JOB ABANDONDED - Dataset number should be integer and " . "between 100 and 999" . K_NEW_LN ;
   } ## end if ( $p_set_data =~ /\D/...)

   if ( $p_run_no eq K_EMPTY ) {                                # Abort if Run number not specified
      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . "JOB ABANDONDED - Missing run number" . K_NEW_LN ;
   }

   if (
      $p_run_no =~ /\D/ or                                      # Abort if run number not numeric
      $p_run_no < 1000  or                                      # Abort if run number less than 100
      $p_run_no > 9999
     ) {                                                        # Abort if run number not between 1000 and 9999
      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . "JOB ABANDONDED - Run number should be integer and " . "between 1000 and 9999" . K_NEW_LN ;
   } ## end if ( $p_run_no =~ /\D/...)

   if ( $v_log_file_dir ne './' ) {                             # If log file directory not default (./)
      if (
         substr ( $v_log_file_dir , -1 , 1 ) ne '\\'            # If log file directory not end with back slash (\\)
        ) {
         $v_log_file_dir .= '\\' ;                              # Add back slash at end
      }
   } ## end if ( $v_log_file_dir ne...)

   $v_log_file_dir =~ s/\\/\//g ;                               # Replace back slash to forward slash

   if ( $v_tag_file_dir ne './' ) {                             # If tag file directory not default (./)
      if (
         substr ( $v_tag_file_dir , -1 , 1 ) ne '\\'            # If tag file directory not end with back slash (\\)
        ) {
         $v_tag_file_dir .= '\\' ;                              # Add back slash at end
      }
   } ## end if ( $v_tag_file_dir ne...)

   $v_tag_file_dir =~ s/\\/\//g ;                               # Replace back slash to forward slash

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

   $v_tag_file = $p_set_data . $p_run_no . '.tag' ;             # Tag file - Data set + Run number

   if ( ! -e $v_tag_file ) {
      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE . __LINE__ . K_SPACE .           #
        "JOB ABANDONDED - Input tagged data file does not exist" . ' - ' . $v_tag_file ;
   }

   if (
      $p_in_dbserver eq K_EMPTY or                              # Input - Database host
      $p_in_dbport eq K_EMPTY   or                              # Input - Database port
      $p_in_user eq K_EMPTY     or                              # Input - Database user
      $p_in_dbpw eq K_EMPTY     or                              # Input - Database password
      $p_in_dbname eq K_EMPTY   or                              # Input - Database name
      $p_in_dbtype eq K_EMPTY                                   # Input - Database type
     ) {

      die                                                       #
        __PACKAGE__ . K_SPACE                                   #
        . __FILE__ . K_SPACE                                    #
        . __LINE__ . K_SPACE                                    #
        . "JOB ABANDONDED - Oracle required database Host, Port, Name, Type, User or Password parameters" ;    #

   } ## end if ( $p_in_dbserver eq...)

   #dbi:Oracle:host=192.168.1.214;sid=IIBM;port=1521;
   my $dsn =                                                    # Data source name string
     "dbi:" . $p_in_dbtype . ":host=" . $p_in_dbserver . ";" .  #
     "sid=" . $p_in_dbname . ";" . "port=" . $p_in_dbport . ";" ;

   $dbh =                                                       # Database connection handle
     DBI -> connect (                                           #
      $dsn ,                                                    #
      $p_in_user ,                                              #
      $p_in_dbpw ,                                              #
      { RaiseError => K_ONE , AutoCommit => K_ONE }             #
     ) or                                                       #
     die                                                        #
     __PACKAGE__ . K_SPACE .                                    #
     __FILE__ . K_SPACE .                                       #
     __LINE__ . K_SPACE .                                       #
     "JOB ABANDONDED - Cannot connect to oracle database" ;     #                                             #

} ## end sub sGetParameters
#######################################################################
# End of subroutine sGetParameters                                    #
#######################################################################

sub sInit {

   # Inserts begin details in table dedupe . T_XST_XBG

   # Called by:

   # Get job sequence number dedupe.seq_jb_sq_no
   my $vi_job_no_sql =                                          # Sql of get job sequence number
     "select dedupe.seq_jb_sq_no.nextval from dual" ;

   my $vi_sth = $dbh -> prepare ( $vi_job_no_sql )              #
     or die __PACKAGE__
     . K_SPACE
     . __FILE__
     . K_SPACE
     . __LINE__
     . K_SPACE
     . "Cannot prepare sql to get begin timestamp"
     .                                                          #
     ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - ' ;

   $vi_sth -> execute                                           #
     or die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE . __LINE__ . K_SPACE
     .                                                          #
     "Cannot execute sql to get begin timestamp" .              #
     ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - ' ;

   while ( my @ai_fld_1 = $vi_sth -> fetchrow_array () ) {
      $v_job_sn = $ai_fld_1[ 0 ] || K_EMPTY ;                   # Store Job sequence number
   }

   $v_now_ts =                                                  # Store current time in perl
     $v_now_yy . '-'                                            #
     . ( $v_now_mm < 10 ? 0 : K_EMPTY )                         #
     . $v_now_mm . '-'                                          #
     . ( $v_now_dd < 10 ? 0 : K_EMPTY )                         #
     . $v_now_dd . '-'                                          #
     . ( $v_now_hh < 10 ? 0 : K_EMPTY )                         #
     . $v_now_hh . '-'                                          #
     . ( $v_now_mi < 10 ? 0 : K_EMPTY )                         #
     . $v_now_mi . '-'                                          #
     . ( $v_now_ss < 10 ? 0 : K_EMPTY )                         #
     . $v_now_ss ;

   my $v_start_ts_sql =                                         # Sql of systimestamp
     "select to_timestamp (" . K_SPACE . K_SNG_APO . $v_now_ts . K_SNG_APO . "," . K_SPACE . K_SNG_APO . "YYYY-MM-DD HH24:MI:SS:ff6" . K_SNG_APO .    #
     K_SPACE . ") from dual" ;

   $vi_sth = $dbh -> prepare ( $v_start_ts_sql )                #
     or die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE .        #
     __LINE__ . K_SPACE .                                       #
     "Cannot execute sql to get timestamp" .                    #
     ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - ' ;

   $vi_sth -> execute                                           #
     or die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE .        #
     __LINE__ . K_SPACE .                                       #
     "Cannot execute sql to get timestamp" .                    #
     ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - ' ;

   while ( my @ai_fld_2 = $vi_sth -> fetchrow_array () ) {
      $v_start_ts = $ai_fld_2[ 0 ] || K_EMPTY ;                 # Store systimestamp
   }

   # Insert records into table
   my $vi_sql =                                                 #
     "insert into" . K_SPACE . "dedupe . T_XST_XBG ("            #
     . K_SPACE . "xbg_set_no ,"                                 #
     . K_SPACE . "xbg_run_no ,"                                 #
     . K_SPACE . "xbg_pc_nm_u ,"                                #
     . K_SPACE . "xbg_pc_bg_ts ,"                               #
     . K_SPACE . "xbg_f_pc_ed_nt ,"                             #
     . K_SPACE . "xbg_f_pc_ed_dtc ,"                            #
     . K_SPACE . "xbg_f_abt ,"                                  #
     . K_SPACE . "xbg_jb_sq_no ,"                               #
     . K_SPACE . "xbg_crr_pc ,"                                 #
     . K_SPACE . "xbg_crr_ur_u ,"                               #
     . K_SPACE . "xbg_crr_tp"                                   #
     . ")"                                                      #
     . K_SPACE . "values ("                                     #
     . K_SPACE . $p_set_data . K_SPACE . ","                    #
     . K_SPACE . $p_run_no . K_SPACE . ","                      #
     . K_SPACE . K_SNG_APO . K_PROCEDURE_NM_U . K_SNG_APO . K_SPACE . ","                                                                             # Procedure name
     . K_SPACE . K_SNG_APO . $v_start_ts . K_SNG_APO . K_SPACE . ","                                                                                  # Begin timestamp
     . K_SPACE . K_SNG_APO . K_Y . K_SNG_APO . K_SPACE . ","    # Flag - Procedure end not
     . K_SPACE . K_SNG_APO . K_n . K_SNG_APO . K_SPACE . ","    # Flag - Procedure ended normally
     . K_SPACE . K_SNG_APO . K_Y . K_SNG_APO . K_SPACE . ","    # Flag - Abort
     . K_SPACE . $v_job_sn . K_SPACE . ","                      # Job number
     . K_SPACE . K_SNG_APO . K_PROCEDURE_NM_U . K_SNG_APO . "," # Procedure name
     . K_SPACE . K_SNG_APO . $p_in_user . K_SNG_APO . K_SPACE . ","                                                                                   # Last modified by user
     . K_SPACE . K_SNG_APO . $v_tp . K_SNG_APO . K_SPACE        #
     . ")" ;                                                    #

   $vi_sth = $dbh -> prepare ( $vi_sql )                        #
     or die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE .        #
     __LINE__ . K_SPACE .                                       #
     "Cannot prepare sql to insert begin details" .             #
     ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - ' ;

   $vi_sth -> execute                                           #
     or die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE . __LINE__ . K_SPACE .                                                                         #
     "Cannot execute sql to insert begin details" .             #
     ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - ' ;

   return 1 ;

} ## end sub sInit
#############################################################################
# End of subroutine sInit                                                   #
#############################################################################

sub sUplTagFile {

   # Upload tag file data in table- dedupe . T_DAT_RNG

   my $vuk_bad_file =                                           # Bad file name of sql loader
     $p_set_data . $p_run_no . '_' . 'sqlldr' . '.btr' ;

   my $vuk_discard_file =                                       # Discard file
     $p_set_data . $p_run_no . '_' . 'sqlldr' . '.dsd' ;

   my $vuk_sql_ldr_ctl_file =                                   # Control file to run sql loader
     $p_set_data . $p_run_no . '_' . 'sqlldr' . '.ctl' ;

   $v_sql_ldr_log_file =                                        # log file of sql loader
     $p_set_data . $p_run_no . '_' . 'sqlldr' . '.log' ;

   # Script of SQL loader control file  ############################################################

   $v_sql_ldr_sg =                                              #
     'OPTIONS ( ERRORS = 9999999999 )' . K_NEW_LN               #
     . 'LOAD DATA' . K_NEW_LN . 'INFILE \'' . $v_tag_file_dir . $v_tag_file . '\''                                                                    #
     . K_NEW_LN                                                 #
     . 'BADFILE \'' . $v_work_file_dir . $vuk_bad_file . '\''   #
     . K_NEW_LN                                                 #
     . 'DISCARDFILE \'' . $v_work_file_dir . $vuk_discard_file . '\''                                                                                 #
     . K_NEW_LN                                                 #
     . 'APPEND' . K_NEW_LN                                      #
     . 'INTO TABLE dedupe.T_DAT_TAG' . K_NEW_LN                 #
     . 'fields terminated by \'\t\' optionally enclosed by \'"\'' #
     . K_NEW_LN . 'TRAILING NULLCOLS'          #
     . K_NEW_LN . '(' . K_NEW_LN                                #
     . '  TAG_ID char(1000),'                                             #
     . K_NEW_LN                                                 #
     . '  TAG_DATA char(2000),'                                           #
     . K_NEW_LN                                                 #
     . '  TAG_CRR_PC ' . '"\'' . K_PROCEDURE_NM_U . '\'"' . ' ,'         #
     . K_NEW_LN                                                 #
     . '  TAG_SET_NO ' . '"' . $p_set_data . '"' . ' ,'         #
     . K_NEW_LN                                                 #
     . '  TAG_RUN_NO ' . '"' . $p_run_no . '"' . ' ,'           #
     . K_NEW_LN                                                 #                                               #
     . '  TAG_JB_SQ_NO ' . '"' . $v_job_sn . '"'                #
     . K_NEW_LN . ')' ;

   open ( $FILECTL , '>' , $v_work_file_dir . $vuk_sql_ldr_ctl_file ) or                                                                              # Create and open sql loader control file
     die "Can not open sql loader control file $vuk_sql_ldr_ctl_file - $!" . K_NEW_LN ;

   print $FILECTL $v_sql_ldr_sg ;                               # Write sql loader control file

   close $FILECTL ;                                             # Close control file

   # END SCRIPT OF SQL LOADER CONTROL FILE  ########################################################

   # CALL SQL LOADER ###############################################################################
   # dedupe/dedupe@192.168.1.214:1521/IIBM control=./1001000.ctl log=./1001000.log silent=feedback,header

   system (                                                     # Call sql loader
      "exit | sqlldr "                                          #
        . $p_in_user . "/" . $p_in_dbpw                         #
        . '@' . $p_in_dbserver . ':' . $p_in_dbport . '/' . $p_in_dbname     #
        . K_SPACE . 'control=' . $v_work_file_dir . $vuk_sql_ldr_ctl_file    #
        . K_SPACE . 'log=' . $v_work_file_dir . $v_sql_ldr_log_file          #
        . K_SPACE                                               #
        . ( $p_f_verbose ne K_Y ? 'silent=feedback,header' : K_EMPTY )
   ) ;

   # END CALL SQL LOADER ###########################################################################

   $v_sec_to_exe_upl_tag = time () - $v_start_time ;            # Time to execute uplaod tag
   $v_upl_tag_end_ts     = &sGetCurTimestamp ;                  # Tag upload ended

   if ( $p_f_verbose eq 'Y' ) {
      print K_NEW_LN . "Make tag uploading and script ended" . K_SPACE . $v_upl_tag_end_ts    #
        . ' - '                                                 #
        . strftime ( "\%H:\%M:\%S" , gmtime ( $v_sec_to_exe_upl_tag ) )                       #
        . K_SPACE . "to execute" . K_NEW_LN ;
   } ## end if ( $p_f_verbose eq 'Y')

   # Bad file of sql loader size
   my $vuk_bad_file_size = -s $v_work_file_dir . $vuk_bad_file || K_ZERO ;

   # Discard file of sql loader size
   my $vuk_discard_file_size = -s $v_work_file_dir . $vuk_discard_file || K_ZERO ;

   # Unlink file if size is less than 1 means delete
   if ( $vuk_bad_file_size < 1 )     { unlink $v_work_file_dir . $vuk_bad_file ; }
   if ( $vuk_discard_file_size < 1 ) { unlink $v_work_file_dir . $vuk_discard_file ; }

} ## end sub sUplTagFile
#######################################################################
# End of subroutine sUplTagFile                                     #
#######################################################################

sub sWriteLog {

   # Write log file

   # Name of the log file
   $v_log_file =                                                #
     $p_set_data . $p_run_no . '_'                              #
     . 'UplTag' . '_'                                           #
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
     "------ UPLOAD TAG EXECUTION START DATE AND TIME ------" . K_NEW_LN    #
     . K_NEW_LN                                                 #
     . $v_start_timestamp . K_NEW_LN . K_NEW_LN                 #

     . "------ Run Parameters ------" . K_NEW_LN                #
     . "Data set number             :" . K_SPACE . $p_set_data . K_NEW_LN          #
     . "Run time number             :" . K_SPACE . $p_run_no . K_NEW_LN            #
     . "Tag file directory          :" . K_SPACE . $p_tag_file_dir . K_NEW_LN      #
     . "Database user               :" . K_SPACE . $p_in_user . K_NEW_LN           #
     . "Database password           :" . K_SPACE . "<_not_printed_>" . K_NEW_LN    #
     . "Database (Sid) name         :" . K_SPACE . $p_in_dbname . K_NEW_LN         #
     . "Database connection string  :" . K_SPACE . $p_in_dbtype . K_NEW_LN         #
     . "Database port               :" . K_SPACE . $p_in_dbport . K_NEW_LN         #
     . "Database Server (Host) name :" . K_SPACE . $p_in_dbserver . K_NEW_LN       #
     . "Log File Directory          :" . K_SPACE . $p_log_file_dir . K_NEW_LN      #
     . "work file directory         :" . K_SPACE . $p_work_file_dir . K_NEW_LN     #
     . "Flag - verbose - details dsp:" . K_SPACE . $p_f_verbose . K_NEW_LN         #
     . K_NEW_LN                                                 #

     . "------ File Names ------" . K_NEW_LN                    #                                                #
     . "Input Tag file name  :" . K_SPACE . $v_tag_file . K_NEW_LN                 #
     . "Log  file name       :" . K_SPACE . $v_log_file . K_NEW_LN                 #
     . K_NEW_LN ;

   print $FILELOG $v_log ;                                      # Write log file

   if ( $p_f_verbose eq K_Y ) {

      print $FILELOG K_NEW_LN . "SQL Loader Control file :" . K_NEW_LN ;
      print $FILELOG $v_sql_ldr_sg . K_NEW_LN . K_NEW_LN ;      # Print SQL Loader control file in log file
   } ## end if ( $p_f_verbose eq K_Y)

   print $FILELOG "Tagged data uploading and script ended" . K_SPACE . $v_upl_tag_end_ts    #
        . ' - '                                                 #
        . strftime ( "\%H:\%M:\%S" , gmtime ( $v_sec_to_exe_upl_tag ) )                        #
        . K_SPACE . "to execute" . K_NEW_LN . K_NEW_LN ;
   
   close $FILELOG                                               # Close log file
     or die "Can not close log file $v_log_file - $!" . K_NEW_LN ;

    # Using type command write sql loader file into main procedure log file
   system (
      "Type" . K_SPACE . '"' . $v_work_file_dir_cmd . $v_sql_ldr_log_file . '"'                #
        . ">>" . K_SPACE . '"' . $v_log_file_dir . $v_log_file . '"'                           #
   ) ;

   unlink $v_work_file_dir . $v_sql_ldr_log_file ;              # Delete temporary tag log file after write into main log file

} ## end sub sWriteLog

#######################################################################
# End of subroutine sWriteLog                                         #
#######################################################################

sub sWindUp {

   # Inserts procedure end record in dedupe . T_XST_XED and updates start record in
   # dedupe . T_XST_XBG.

   my $v_end_ts = sGetCurTimestamp ;

   my $v_end_ts_sql =                                           # Sql of end  systimestamp
     "select to_timestamp (" . K_SPACE . K_SNG_APO . $v_end_ts  #
     . K_SNG_APO . "," . K_SPACE . K_SNG_APO                    #
     . "YYYY-MM-DD HH24:MI:SS:ff6" . K_SNG_APO                  #
     . K_SPACE . ") from dual"                                  #
     ;                                                          #

   my $vw_sth = $dbh -> prepare ( $v_end_ts_sql )               #
     or die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE          #
     . __LINE__ . K_SPACE                                       #
     . "Cannot execute sql to get timestamp"                    #
     . ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - '      #
     ;

   $vw_sth -> execute                                           #
     or die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE          #
     . __LINE__ . K_SPACE                                       #
     . "Cannot execute sql to get timestamp"                    #
     . ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - '      #
     ;

   while ( my @ai_fld_2 = $vw_sth -> fetchrow_array () ) {
      $v_end_ts = $ai_fld_2[ 0 ] || K_EMPTY ;                   # Store systimestamp
   }

   my $vwu_xed_sql =                                            # Sql for insert download end activity in T_XST_XED
     "insert into dedupe . T_XST_XED ("                          #
     . K_SPACE . "xed_set_no ,"                                 #
     . K_SPACE . "xed_run_no ,"                                 #
     . K_SPACE . "xed_pc_nm_u ,"                                #
     . K_SPACE . "xed_pc_bg_ts ,"                               #
     . K_SPACE . "xed_pc_ed_ts ,"                               #
     . K_SPACE . "xed_no_cse_ins ," . K_SPACE . "xed_jb_sq_no ,"    #
     . K_SPACE . "xed_crr_ts ,"                                 #
     . K_SPACE . "xed_crr_tp ,"                                 #
     . K_SPACE . "xed_crr_pc ,"                                 #
     . K_SPACE . "xed_crr_ur_u"                                 #
     . K_SPACE . ")" . K_SPACE                                  #
     . "values ("                                               #
     . K_SPACE                                                  #
     . $p_set_data                                              #
     . K_SPACE . ","                                            #
     . K_SPACE                                                  #
     . $p_run_no                                                #
     . K_SPACE . ","                                            #
     . K_SNG_APO . K_PROCEDURE_NM_U . K_SNG_APO . K_SPACE . "," #
     . K_SPACE                                                  #
     . K_SNG_APO . $v_start_ts . K_SNG_APO . K_SPACE . ","      #
     . K_SPACE                                                  #
     . K_SNG_APO . $v_end_ts . K_SNG_APO . ","                  #
     . K_SPACE . $v_suc_loaded_knt . K_SPACE . ","              #
     . K_SPACE . $v_job_sn . K_SPACE . ","                      #
     . K_SPACE                                                  #
     . K_SPACE . "systimestamp" . K_SPACE . ","                 #
     . K_SNG_APO . $v_tp . K_SNG_APO . K_SPACE . ","            #
     . K_SPACE . K_SNG_APO . K_PROCEDURE_NM_U                   # Procedure name
     . K_SNG_APO . "," . K_SPACE                                #
     . K_SPACE . K_SNG_APO . $p_in_user . K_SNG_APO . K_SPACE   # Last modified by user
     . ")"                                                      #
     ;                                                          #

   $vw_sth = $dbh -> prepare ( $vwu_xed_sql )                   #
     or die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE          #
     . __LINE__ . K_SPACE                                       #
     . "Cannot prepare sql to insert end details"               #
     . ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - '      #
     ;

   $vw_sth -> execute                                           #
     or die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE          #
     . __LINE__ . K_SPACE                                       #
     . "Cannot execute sql to insert end details"               #
     . ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - '      #
     ;

   my $vwu_xbg_u_sql =                                          # Sql to update download begin activity in T_XST_XBG
     "update dedupe . T_XST_XBG set"                             #
     . K_SPACE                                                  #
     . "xbg_pc_ed_ts =" . K_SPACE . K_SNG_APO . $v_end_ts . K_SNG_APO . ","    #
     . K_SPACE                                                  #
     . "xbg_f_pc_ed_nt ="                                       #
     . K_SPACE                                                  #
     . K_SNG_APO . "n"                                          #
     . K_SNG_APO                                                #
     . K_SPACE . ","                                            #
     . K_SPACE                                                  #
     . K_SPACE                                                  #
     . "xbg_tlm_ts = systimestamp ,"                            #
     . K_SPACE . "xbg_tlm_pc ="                                 #
     . K_SPACE                                                  #
     . K_SNG_APO . K_PROCEDURE_NM_U                             #
     . K_SNG_APO                                                #
     . K_SPACE . ","                                            #
     . K_SPACE . "xbg_tlm_ur_u ="                               #
     . K_SPACE                                                  #
     . K_SNG_APO . $p_in_user                                   #
     . K_SNG_APO                                                #
     . K_SPACE . ","                                            #
     . K_SPACE . "xbg_tlm_tp ="                                 #
     . K_SPACE                                                  #
     . K_SNG_APO . $v_tp                                        #
     . K_SNG_APO                                                #
     . K_SPACE . ","                                            #
     . K_SPACE                                                  #
     . "xbg_f_pc_ed_dtc ="                                      #
     . K_SPACE                                                  #
     . K_SNG_APO . "Y"                                          #
     . K_SNG_APO                                                #
     . K_SPACE . ","                                            #
     . K_SPACE                                                  #
     . "xbg_f_abt ="                                            #
     . K_SPACE                                                  #
     . K_SNG_APO . "n"                                          #
     . K_SNG_APO                                                #
     . K_SPACE                                                  #
     . "where xbg_set_no ="                                     #
     . K_SPACE                                                  #
     . $p_set_data                                              #
     . K_SPACE                                                  #
     . "and xbg_run_no ="                                       #
     . K_SPACE                                                  #
     . $p_run_no                                                #
     . K_SPACE                                                  #
     . "and xbg_crr_ur_u ="                                     #
     . K_SPACE                                                  #
     . K_SNG_APO                                                #
     . $p_in_user                                               #
     . K_SNG_APO                                                #
     . K_SPACE                                                  #
     . "and xbg_pc_nm_u ="                                      #
     . K_SPACE                                                  #
     . K_SNG_APO                                                #
     . K_PROCEDURE_NM_U                                         #
     . K_SNG_APO                                                #
     . K_SPACE                                                  #
     . "and to_char( xbg_pc_bg_ts ,"                            #
     . K_SPACE . K_SNG_APO . "YYYY-MM-DD-HH24-MI-SS"            #
     . K_SNG_APO . K_SPACE . ") =" . K_SPACE . K_SNG_APO        #
     . $v_now_ts                                                #
     . K_SNG_APO                                                #
     ;

   $vw_sth = $dbh -> prepare ( $vwu_xbg_u_sql )                 #
     or die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE          #
     . __LINE__ . K_SPACE                                       #
     . "Cannot prepare sql to update begin details"             #
     . ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - '      #
     ;

   $vw_sth -> execute                                           #
     or die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE          #
     . __LINE__ . K_SPACE                                       #
     . "Cannot execute sql to update begin details"             #
     . ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - '      #
     ;

   return 1 ;

} ## end sub sWindUp
#####################################################################
# End of sub routine sWindUp                                        #
#####################################################################

sub sGetTcpIp {                                                 # Get TCP/IP of machine

   use Sys::Hostname ;
   my $host = hostname ;

   use Socket ;
   my $vgti_ip = inet_ntoa ( scalar gethostbyname ( $host || 'localhost' ) ) ;

   return $vgti_ip ;

} ## end sub sGetTcpIp
#####################################################################
# End of sub routine sGetTcpIp                                      #
#####################################################################
