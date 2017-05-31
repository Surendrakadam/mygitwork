#!C:/Perl/bin/perl.EXE -w

use strict ;
use warnings ;

use IO::File ;                                                  # IO::File - supply object methods for filehandles
use Getopt::Simple ;                                            # Get run parameter module
use POSIX qw{strftime} ;                                        # For current date and time
use DBI ;                                                       # Access database

# Application   : ISPL
# Client        : Internal
# Copyright (c) : 2017 IdentLogic Systems Private Limited
# Author        : Surendra Kadam
# Creation Date : 21 April 2017
# Description   : Upload Search id , File id , Purpose no , Conservative Match
#                  Decision ,Conservative Match Score , Typical Match Decision ,
#                  Typical Match Score , Loose Match Decision and Loose Match
#                  score which are sort by search id using 44_516_SortMtc.pl
#                  procedure.
#                 There two types of database use in this procedure such as SQLite
#                  or Oracle.
#                 SQLite : Create control file with extension of .sql for SQLite
#                           to upload .mst file into T_DAT_MTC table
#                 Oracle : Create control file with extension of .ctl for SQl
#                           Loader to upload .mst file into T_DAT_MTC table
#
# WARNINGS      :
# HARD CODINGS  :
# Limitations   :
# Dependencies  :
# Modifications
# Date       Change Req# Author       Description
# ---------- ----------- ------------ -------------------------------

use constant K_PACKAGE_NO         => 44 ;                       # Package number
use constant K_PROCEDURE_NO       => 517 ;                      # Procedure number
use constant K_PROCEDURE_NM_U     => "44_517_UplMtc.pl" ;       # Procedure name
use constant K_PROCEDURE_NM_SYS_U => "Upload sort match" ;      # Procedure description

use constant K_EMPTY   => "" ;                                  # Empty field
use constant K_NEW_LN  => "\n" ;                                # New Line Character
use constant K_Y       => "Y" ;                                 # Y for Yes
use constant K_n       => "n" ;                                 # n for no
use constant K_SPACE   => " " ;                                 # Space
use constant K_ONE     => 1 ;                                   # One
use constant K_ZERO    => 0 ;                                   # Zero
use constant K_SNG_APO => "'" ;                                 # Single apostrophe

# Run parameters
my $p_set_data         = K_EMPTY ;                              # Data set                       - d
my $p_run_no           = K_EMPTY ;                              # Run number                     - r
my $p_prps_no          = K_EMPTY ;                              # Purpose number                 - s
my $p_sort_in_file_dir = K_EMPTY ;                              # Sort input file directory      - a
my $p_in_user          = K_EMPTY ;                              # Database user                  - u
my $p_in_dbpw          = K_EMPTY ;                              # Database password              - g
my $p_in_dbdir         = K_EMPTY ;                              # Database file location         - y              - g
my $p_in_dbname        = K_EMPTY ;                              # Database (Sid) name            - x
my $p_in_dbtype        = K_EMPTY ;                              # Database connection string     - c
my $p_in_dbport        = K_EMPTY ;                              # Database port                  - p
my $p_in_dbserver      = K_EMPTY ;                              # Database Server (Host) name    - t
my $p_log_file_dir     = K_EMPTY ;                              # Log file directory             - l
my $p_work_file_dir    = K_EMPTY ;                              # work file directory            - w
my $p_f_verbose        = K_EMPTY ;                              # Flag - Verbose - print details - v

my $v_sort_in_file      = K_EMPTY ;                             # Stores sort input file name
my $v_log_file          = K_EMPTY ;                             # Stores log file name

my $v_log_file_dir      = K_EMPTY ;                             # Log file directory
my $v_sort_in_file_dir  = K_EMPTY ;                             # Sort input file directory
my $v_work_file_dir     = K_EMPTY ;                             # Temporary file working directory
my $v_in_dbdir          = K_EMPTY ;                             # Database file directory
my $v_work_file_dir_cmd = K_EMPTY ;                             # Temporary file working directory for cmd
my $v_sql_ldr_log_file  = K_EMPTY ;                             # Log file of sql loader
my $v_sqlite_log_file   = K_EMPTY ;                             # Log file of sqlite db

my $v_out_rec_knt             = K_ZERO ;                        # Count of output record
my $v_sec_to_exe_upl_sort_mtc = K_EMPTY ;                       # Seconds to execute upload sort match
my $v_upl_sort_mtc_end_ts     = K_EMPTY ;                       # Upload Sort match end timestamp
my $v_sql_ldr_sg              = K_EMPTY ;                       # Sql loader string for control file
my $v_sqlite_cmd              = K_EMPTY ;                       # SQLite control file commands
my $v_f_error                 = K_n ;                           # Flag - Error
my $v_logical_rec_read_knt    = K_ZERO ;                        # Logical record count
my $v_suc_loaded_knt          = K_ZERO ;                        # Successfully loaded count
my $dbh                       = K_EMPTY ;                       # Database handle
my $v_start_ts                = K_EMPTY ;                       # Varible to store timestamp
my $v_job_sn                  = K_EMPTY ;                       # Varible to store job sequence number
my $v_now_ts                  = K_EMPTY ;                       # Varible to store time
my $v_tp                      = &sGetTcpIp ;                    # Get TCP/IP of machine
my $v_sqlite_log_file_sz      = 0 ;                             # Size of the SQLite log file
my $v_operating_system        = $^O;                            # Operating system which you are using

my $v_in_rec_knt              = 0 ;                             # Count of records in input file
my $v_op_rec_knt              = 0 ;                             # Count of upload records in table

my $FILELOG = IO::File -> new ;                                 # Declaration of filehandle of log file
my $FILECTL = IO::File -> new ;                                 # Declaration of filehandle of control file for sql loader
my $FILESQL = IO::File -> new ;                                 # Declaration of filehandle of SQL file for SQLite db

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

if ( $p_in_dbtype eq "SQLite" ) {                               # SQLite is a default database
   &sUplSortMtcSQLite ;                                         # Upload Sort match data using SQLite
}
elsif ( $p_in_dbtype eq "Oracle" ) {                            # Oracle database
   &sInit ;                                                     # Inserts begin details in table dedupe . T_XST_XBG
   &sUplSortMtcOracle ;                                         # Upload Sort match data using oracle
   &sWindUp ;                                                   # Insert procedure end record and update begin record
}
else {
  die #   
    $0: , K_SPACE   #
    , __PACKAGE__ , K_SPACE , __FILE__ , K_SPACE            #
    , __LINE__ , K_SPACE   # 
    , "JOB ABANDONDED - Database type not SQLite nor "
    , "Oracle but >$p_in_dbtype<" , K_NEW_LN ;
}
      
&sWriteLog ;                                                    # Write log

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
      datasetno => {                                            # Data set number
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Data set number' ,
         order   => 2 ,
        } ,
      runno => {                                                # Run number
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Run number' ,
         order   => 3 ,
        } ,
      kpurposeno => {                                           # Purpose number
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Purpose number' ,
         order   => 4 ,
        } ,
      asortfldir => {                                           # Sort input file directory
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Input file directory - optional' ,
         order   => 5 ,
        } ,
      logfldir => {                                             # Log file directory
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Log file directory - optional' ,
         order   => 6 ,
        } ,
      workfldir => {                                            # Work file directory
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Work file directory - optional' ,
         order   => 7 ,
        } ,
      user => {                                                 # Database user name
         type    => "=s" ,
         env     => "-" ,
         default => 'dedupe' ,
         verbose => "Database user name - Needed only for Oracle" ,
         order   => 8 ,
        } ,
      guptshabd => {                                            # Database user password
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => "Database user password - Needed only for Oracle" ,
         order   => 9 ,
        } ,
      port => {                                                 # Database port
         type    => '=s' ,
         env     => '-' ,
         default => '1521' ,
         verbose => "Database port - Needed only for Oracle" ,
         order   => 10 ,
        } ,
      tcpip => {                                                # Database host - Server or TCP/IP
         type    => '=s' ,
         env     => '-' ,
         default => "192.168.1.214" ,
         verbose => "Database host - Server or TCP/IP - Needed only for Oracle" ,
         order   => 11 ,
        } ,
      ydbdir => {                                               # Database file location directory path
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => "Database file location directory path - Needed only for SQLite" ,
         order   => 12 ,
        } ,
      xdbname => {                                              # Database (SID) name
         type    => '=s' ,
         env     => '-' ,
         default => 'IIBM' ,
         verbose => "Database (SID) name - Needed only for Oracle" , # For SQLite file DeDupe{datasetno} used
         order   => 13 ,
        } ,
      cncdb => {                                                # Database type connection - SQLite or Oracle
         type    => '=s' ,
         env     => '-' ,
         default => 'SQLite' ,
         verbose => "Database type connection - SQLite or Oracle" ,
         order   => 14 ,
        } ,
      verboseflag => {                                          # Print job details - no argument needed - optional
         type    => '!' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Print job details - no argument needed - optional' ,
         order   => 15 ,
      } ,
   } ;

   my ( $parameters ) = Getopt::Simple -> new () ;
   if ( ! $parameters -> getOptions ( $vgp_get_options , "Usage: 44_517_UplMtc.pl [options]" ) ) {
      exit ( -1 ) ;                                             # Failure
   }

   # Get run parameters value
   $p_set_data         = $$parameters{ 'switch' }{ 'datasetno' }  || K_EMPTY ;    # Dataset number                          - d
   $p_run_no           = $$parameters{ 'switch' }{ 'runno' }      || K_EMPTY ;    # Run number                              - r
   $p_prps_no          = $$parameters{ 'switch' }{ 'kpurposeno' } || K_EMPTY ;    # Purpose number                          - k
   $p_sort_in_file_dir = $$parameters{ 'switch' }{ 'asortfldir' } || K_EMPTY ;    # Sort input file directory               - a
   $p_log_file_dir     = $$parameters{ 'switch' }{ 'logfldir' }   || K_EMPTY ;    # Log file directory                      - l
   $p_work_file_dir    = $$parameters{ 'switch' }{ 'workfldir' }  || K_EMPTY ;    # work file directory                     - w

   $p_in_user          = $$parameters{ 'switch' }{ 'user' } ;                     # Database user                           - u
   $p_in_dbpw          = $$parameters{ 'switch' }{ 'guptshabd' } ;                # Database password                       - g
   $p_in_dbdir         = $$parameters{ 'switch' }{ 'ydbdir' } ;                   # Database file location directory path   - y
   $p_in_dbname        = $$parameters{ 'switch' }{ 'xdbname' } ;                  # Database (Sid) name                     - x
   $p_in_dbtype        = $$parameters{ 'switch' }{ 'cncdb' } ;                    # Database type                           - c
   $p_in_dbport        = $$parameters{ 'switch' }{ 'port' } ;                     # Database port                           - p
   $p_in_dbserver      = $$parameters{ 'switch' }{ 'tcpip' } ;                    # Database Server (Host) name             - t
   $p_f_verbose        = $$parameters{ 'switch' }{ 'verboseflag' } ;              # Flag - Verbose - print details          - v

   if ( $p_f_verbose eq '1' ) {                                 # Display run parameters if verbose option
      print "$0: Started at" . K_SPACE . ( localtime ) . K_NEW_LN . K_NEW_LN ,                   #
        'Dataset                     : >' , $p_set_data ,         '<' , K_NEW_LN ,               #
        'Run number                  : >' , $p_run_no ,           '<' , K_NEW_LN ,               #
        'Purpose number              : >' , $p_prps_no ,          '<' , K_NEW_LN ,               #
        'Sort input file directory   : >' , $p_sort_in_file_dir , '<' , K_NEW_LN ,               #
        'Log file directory          : >' , $p_log_file_dir ,     '<' , K_NEW_LN ,               #
        'Work file directory         : >' , $p_work_file_dir ,    '<' , K_NEW_LN ,               #
        'Database user               : >' , $p_in_user ,          '<' , K_NEW_LN ,               #
        'Database password           : >' , $p_in_dbpw ,          '<' , K_NEW_LN ,               #
        'Database file location      : >' , $p_in_dbdir ,         '<' , K_NEW_LN ,               #
        'Database (Sid) name         : >' , $p_in_dbname ,        '<' , K_NEW_LN ,               #
        'Database type               : >' , $p_in_dbtype ,        '<' , K_NEW_LN ,               #
        'Database port               : >' , $p_in_dbport ,        '<' , K_NEW_LN ,               #
        'Database Server (Host) name : >' , $p_in_dbserver ,      '<' , K_NEW_LN ,               #
        'Flag - Verbose              : >' , $p_f_verbose ,        '<' , K_NEW_LN , K_NEW_LN ;    #
   } ## end if ( $p_f_verbose eq 'Y')

   if   ( $p_f_verbose eq '1' ) { $p_f_verbose = 'Y' ; }        # Convert to ISPL convention
   else                         { $p_f_verbose = 'n' ; }

   # Process all work directory paths if empty or not
   $v_sort_in_file_dir = ( $p_sort_in_file_dir eq K_EMPTY ? './' : $p_sort_in_file_dir ) ;
   $v_log_file_dir     = ( $p_log_file_dir     eq K_EMPTY ? './' : $p_log_file_dir ) ;
   $v_work_file_dir    = ( $p_work_file_dir    eq K_EMPTY ? './' : $p_work_file_dir ) ;
   
   # Process directory paths not ending in forward or backward slash
   if ( $v_sort_in_file_dir ne './' ) {
     if ( $v_sort_in_file_dir  !~ /(\/|\\)$/ ) { $v_sort_in_file_dir .= '/' ; }
   }
   
   if ( $v_log_file_dir ne './' ) {
     if ( $v_log_file_dir      !~ /(\/|\\)$/ ) { $v_log_file_dir     .= '/' ; }
   }
   
   if ( $v_work_file_dir ne './' ) {
     if ( $v_work_file_dir     !~ /(\/|\\)$/ ) { $v_work_file_dir    .= '/' ; }
   }  
    
   if ( $p_set_data eq K_EMPTY ) {                              # Abort if no data set specified
      die                                                       #
          __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . " JOB ABANDONDED - Missing data set"       #
        . " number >$p_set_data<"                               #
        . K_NEW_LN ;                                            #
   }

   if (
      $p_set_data =~ /\D/ or                                    # Abort if data set number not numeric
      $p_set_data < 100   or                                    # Abort if data set number less than 100
      $p_set_data > 999                                         # Abort if data set number greater than 999
     ) {
      die 
          __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . " JOB ABANDONDED - >$p_set_data< Dataset"  #
        . " number should "                                     #
        . "be integer and between 100 and 999" . K_NEW_LN ;     #
   }

   if ( $p_run_no eq K_EMPTY ) {                                # Abort if Run number not specified
      die                                                       #
          __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . " JOB ABANDONDED - Missing run "           #
        ."number >$p_run_no<"                                   #
        . K_NEW_LN ;                                            #
   }                                                            #

   if (
      $p_run_no =~ /\D/ or                                      # Abort if run number not numeric
      $p_run_no < 1000  or                                      # Abort if run number less than 100
      $p_run_no > 9999                                          # Abort if run number not between 1000 and 9999
     ) {                                                        # 
      die 
          __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . " JOB ABANDONDED - >$p_run_no< Run number" #
        . " should be integer and between 1000 and 9999"        #
        . K_NEW_LN ;                                            #
   } ## end if ( $p_run_no =~ /\D/...)

   if ( $p_prps_no eq K_EMPTY ) {                               # Abort if purpose numbe not specified
      die                                                       #
          __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . ' - ' . "Purpose number not specified"     #
        . " >$p_prps_no<" . K_NEW_LN ;                          #
   }

   if ( $p_prps_no =~ /\D/ or                                   # Abort if Purpose number not numeric
        $p_prps_no < 100   or                                   # Abort if Purpose number less than 100
        $p_prps_no > 999                                        # Abort if Purpose number greater than 999
     ) {
      die                                                       #
          __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__                                              #
        . " $0: JOB ABANDONDED - >$p_prps_no< Purpose number"   #
        . " should be integer" . K_NEW_LN ;                       #
   }                                                            #

   if ( $p_in_dbtype ne "SQLite" and
        $p_in_dbtype ne "Oracle" ) {
      die #
          __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__
        . " $0: JOB ABANDONDED - Database type not SQLite nor "
        . "Oracle but >$p_in_dbtype<" . K_NEW_LN ;
   }

   if ( $p_in_dbtype eq "SQLite" ) {                            # All checking related to SQLite ...
     $v_in_dbdir = ( $p_in_dbdir eq K_EMPTY ? './' : $p_in_dbdir ) ;
     
     if ( $v_in_dbdir ne './') {
       if ( $v_in_dbdir !~ /(\/|\\)$/ ) { $v_in_dbdir .= '/' ; }  # Make SQLite database directory ending in slash or backslash
     }

     $p_in_dbname = "DeDupe" . $p_set_data;                     # Default Database name is DeDupe{data_set_number}
   }
   
   $v_sort_in_file_dir =~ s/\\/\//g ;                           # Replace back slash to forward slash
   $v_log_file_dir     =~ s/\\/\//g ;                           # Replace back slash to forward slash
      
   $v_work_file_dir_cmd = $v_work_file_dir ;                    # Store work file directory in other variable
   $v_work_file_dir     =~ s/\\/\//g ;                          # Replace back slash to forward slash
   $v_work_file_dir_cmd =~ s/\//\\/g ;                          # Replace forward slash to back slash for cmd command
   
   if ( $v_operating_system eq "MSWin32" ) {         
     $v_work_file_dir_cmd =~ s/\//\\/g ;                       # Replace forward slash to back slash for cmd command
   }
   elsif ( $v_operating_system eq "linux" ) {
     $v_work_file_dir_cmd =~ s/\\/\//g ;                       # Replace back slash to forward slash for cmd command
   }
   
   $v_in_dbdir          =~ s/\\/\//g ;                          # Replace back slash to forward slash
   
   $v_sort_in_file = 
       $p_set_data . $p_run_no . '_' . $p_prps_no . '.mst' ;    # Sort match file - Data set + Run number + purposeno

   if ( ! -e $v_sort_in_file_dir . $v_sort_in_file ) {          #
      die                                                       #
          __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . K_SPACE                                    #
        . "JOB ABANDONDED - Input sort match file does not exist" #
        . ' - ' . $v_sort_in_file_dir . $v_sort_in_file ;       #
   }                                                            #

   if ( $p_in_dbtype eq "Oracle" ) {
    
    if ( $p_in_dbname   eq K_EMPTY or                         # Database name
         $p_in_user     eq K_EMPTY or                         # Database user
         $p_in_dbpw     eq K_EMPTY or                         # Database password
         $p_in_dbport   eq K_EMPTY or                         # Database port
         $p_in_dbserver eq K_EMPTY ) {                        # Database Server (Host) name
       die
         $0
         . __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
         . __LINE__   #
         . " JOB ABANDONDED - Parameters needed for Oracle have NOT been specified" . K_NEW_LN 
         . "Database name   >$p_in_dbname<"   . K_NEW_LN                        # Database name
         . "User            >$p_in_user<"     . K_NEW_LN                        # Database user
         . "Password        >NOT SHOWN<"      . K_NEW_LN                        # Database password
         . "Database port   >$p_in_dbport<"   . K_NEW_LN                        # Database port
         . "Database server >$p_in_dbserver<" . K_NEW_LN                        # Database Server (Host) name
         . K_NEW_LN ;
     }

      my $dsn =                                                 # Data source name string
        "dbi:" . $p_in_dbtype . ":host=" . $p_in_dbserver . ";"    #
      . "sid=" . $p_in_dbname . ";" . "port=" . $p_in_dbport . ";" ;

      $dbh =                                                    # Database connection handle
        DBI -> connect (                                        #
         $dsn ,                                                 #
         $p_in_user ,                                           #
         $p_in_dbpw ,                                           #
         { RaiseError => K_ONE , AutoCommit => K_ONE }          #
        ) or                                                    #
        die                                                     #
          $0                                                    #
          . __PACKAGE__ . K_SPACE                               #
          . __FILE__ . K_SPACE                                  #
          . __LINE__ . K_SPACE                                  #
          . "JOB ABANDONDED - Cannot connect to Oracle database - $!" ;  #
   } ## end if ( $p_in_dbtype eq "Oracle")

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
     or die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE . __LINE__ . K_SPACE . "Cannot prepare sql to get begin timestamp" .    #
     ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - ' ;

   $vi_sth -> execute                                           #
     or die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE . __LINE__ . K_SPACE .                                                  #
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
     "select to_timestamp (" 
     . K_SPACE . K_SNG_APO . $v_now_ts . K_SNG_APO . "," 
     . K_SPACE . K_SNG_APO . "YYYY-MM-DD HH24:MI:SS:ff6" . K_SNG_APO     #
     . K_SPACE . ") from dual" ;

   $vi_sth = $dbh -> prepare ( $v_start_ts_sql ) or               #
     die
       $0
       .  __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE        #
       . __LINE__ . K_SPACE                                        #
       . "Cannot execute sql to get timestamp"                     #
       . ' - ' . $DBI::errstr . ' - ' . $DBI::errstr . ' - ' ;

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
     "insert into" . K_SPACE . "dedupe . T_XST_XBG ("           #
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

sub sUplSortMtcSQLite {

   my $vusms_sql_file =                                         # .Control file
     $p_set_data . $p_run_no . '_' . $p_prps_no . '_' . 'SQLite' . '.ctl' ;

   $v_sqlite_log_file =                                        # log file of SQLiteas
     $p_set_data . $p_run_no . '_' . $p_prps_no . '_' . 'SQLite' . '.log' ;

   $v_sqlite_cmd =                                              #
     ".open"                                                    # .open databasename
     . K_SPACE                                                  #
     . $v_in_dbdir                                              #
     . $p_in_dbname                                             #
     . K_NEW_LN                                                 #
     . ".bail on" . K_NEW_LN                                    # .bail on|off Stop after hitting an error.  Default OFF
     . ".log" . K_SPACE                                         # Turn logging on or off.  FILE can be stderr/stdout
     . $v_work_file_dir .$v_sqlite_log_file . K_NEW_LN          #
     . "create table"                                           # create table query
     . K_SPACE                                                  #
     . "T_DAT_MTC"                                              #
     . $p_run_no                                                #
     . K_NEW_LN . "("                                           #
     . K_NEW_LN                                                 #
     . "  mtc_srh_id   NVARCHAR2(1000) not null,"               #
     . K_NEW_LN                                                 #
     . "  mtc_fi_id    NVARCHAR2(1000) not null,"               #
     . K_NEW_LN                                                 #
     . "  mtc_pps_sn   NUMBER(3) not null,"                     #
     . K_NEW_LN                                                 #
     . "  mtc_t_dc     CHAR(1) not null,"                       #
     . K_NEW_LN                                                 #
     . "  mtc_t_no     NUMBER(3) not null,"                     #
     . K_NEW_LN                                                 #
     . "  mtc_c_dc     CHAR(1) not null,"                       #
     . K_NEW_LN                                                 #
     . "  mtc_c_no     NUMBER(3) not null,"                     #
     . K_NEW_LN                                                 #
     . "  mtc_l_dc     CHAR(1) not null,"                       #
     . K_NEW_LN                                                 #
     . "  mtc_l_no     NUMBER(3) not null"                      #
     . K_NEW_LN . ");"                                          #
     . K_NEW_LN                                                 #
     . ".separator"                                             # .separattor "\t"
     . K_SPACE                                                  #
     . "\'\t\'"                                                 #
     . K_NEW_LN                                                 #
     . ".import"                                                # .import filename tablename
     . K_SPACE                                                  #
     . $v_sort_in_file_dir                                      #
     . $v_sort_in_file                                          #
     . K_SPACE                                                  #
     . "T_DAT_MTC"                                              #
     . $p_run_no . K_NEW_LN                                     #
     . ".quit" ;                                                #

   open ( $FILESQL , '>' , $v_work_file_dir . $vusms_sql_file ) or    # Open Sql file
     die "Can not open SQLite sql file $vusms_sql_file - $!" . K_NEW_LN ;

   print $FILESQL $v_sqlite_cmd ;                               # Write Sqlite commands in a file

   close $FILESQL ;                                             # Close sql file

   system (                                                     # sqlite3 dbname < file.sql
      "sqlite3" . K_SPACE                                       #
        . $v_in_dbdir . $p_in_dbname                            #
        . K_SPACE . "<" . K_SPACE                               #
        . $v_work_file_dir . $vusms_sql_file                    #
   ) ;                                                          #
   
   my $vusms_in_db_dir = $v_in_dbdir ;                          # Store input database directory in other variable
   
   if ( $vusms_in_db_dir ne './' ) {
    
     if ( $v_operating_system eq "MSWin32" ) {         
       $vusms_in_db_dir    =~ s/\//\\/g ;                       # Replace forward slash to back slash for cmd command
     }
     elsif ( $v_operating_system eq "linux" ) {
       $vusms_in_db_dir    =~ s/\\/\//g ;                       # Replace back slash to forward slash for cmd command
     }
   }
   
   $v_op_rec_knt = qx( sqlite3 "$vusms_in_db_dir$p_in_dbname" "select count(*) from T_DAT_MTC"$p_run_no" ;" ) ;
 
   $v_sec_to_exe_upl_sort_mtc = time () - $v_start_time ;       # Time to execute upload sort match
   $v_upl_sort_mtc_end_ts     = &sGetCurTimestamp ;             # Sort match upload ended

   print K_NEW_LN . "Sort match data uploading and script ended" . K_SPACE . $v_upl_sort_mtc_end_ts    #
     . ' - '                                                    #
     . strftime ( "\%H:\%M:\%S" , gmtime ( $v_sec_to_exe_upl_sort_mtc ) )                              #
     . K_SPACE . "to execute" . K_NEW_LN ;

   # Size of an file using -s window command
   $v_sqlite_log_file_sz = -s $v_work_file_dir . $v_sqlite_log_file || K_ZERO ;

   # Unlink file if size is less than 1 means delete sqlite log file
   if ( $v_sqlite_log_file_sz < 1 ) { unlink $v_work_file_dir . $v_sqlite_log_file ; }

} ## end sub sUplSortMtcSQLite
#######################################################################
# End of subroutine sUplSortMtcSQLite                                 #
#######################################################################

sub sUplSortMtcOracle {

   # Upload sort match file data in table- dedupe . T_DAT_MTC

   my $vuk_bad_file =                                           # Bad file name of sql loader
     $p_set_data . $p_run_no . '_' . $p_prps_no . '_' . 'sqlldr' . '.btr' ;

   my $vuk_discard_file =                                       # Discard file
     $p_set_data . $p_run_no . '_' . $p_prps_no . '_' . 'sqlldr' . '.dsd' ;

   my $vuk_sql_ldr_ctl_file =                                   # Control file to run sql loader
     $p_set_data . $p_run_no . '_' . $p_prps_no . '_' . 'sqlldr' . '.ctl' ;

   $v_sql_ldr_log_file =                                        # log file of sql loader
     $p_set_data . $p_run_no . '_' . $p_prps_no . '_' . 'sqlldr' . '.log' ;

   # Script of SQL loader control file  ############################################################

   $v_sql_ldr_sg =                                              #
     'OPTIONS ( ERRORS = 9999999999 )' . K_NEW_LN               #
     . 'LOAD DATA' . K_NEW_LN . 'INFILE \'' . $v_sort_in_file_dir . $v_sort_in_file . '\''             #
     . K_NEW_LN                                                 #
     . 'BADFILE \'' . $v_work_file_dir . $vuk_bad_file . '\''   #
     . K_NEW_LN                                                 #
     . 'DISCARDFILE \'' . $v_work_file_dir . $vuk_discard_file . '\''                                  #
     . K_NEW_LN                                                 #
     . 'APPEND' . K_NEW_LN                                      #
     . 'INTO TABLE dedupe.T_DAT_MTC' . K_NEW_LN                 #
     . 'fields terminated by \'\t\' optionally enclosed by \'"\''                                      #
     . K_NEW_LN . 'TRAILING NULLCOLS'                           #
     . K_NEW_LN . '(' . K_NEW_LN                                #
     . '  MTC_SRH_ID char(1000) ,'                              #
     . K_NEW_LN                                                 #
     . '  MTC_FI_ID char(1000) ,'                               #
     . K_NEW_LN                                                 #
     . '  MTC_PPS_SN ' . '"' . $p_prps_no . '"' . ' ,'          #
     . K_NEW_LN                                                 #
     . '  MTC_T_DC char(1) ,'                                   #
     . K_NEW_LN                                                 #
     . '  MTC_T_NO ,'                                           #
     . K_NEW_LN                                                 #
     . '  MTC_C_DC char(1) ,'                                   #
     . K_NEW_LN                                                 #
     . '  MTC_C_NO ,'                                           #
     . K_NEW_LN                                                 #
     . '  MTC_L_DC char(1) ,'                                   #
     . K_NEW_LN                                                 #
     . '  MTC_L_NO ,'                                           #
     . K_NEW_LN                                                 #                                               #
     . '  MTC_JB_SQ_NO ' . '"' . $v_job_sn . '"' . ' ,'         #
     . K_NEW_LN                                                 #
     . '  MTC_CRR_PC ' . '"\'' . K_PROCEDURE_NM_U . '\'"' . ' ,'                                       #
     . K_NEW_LN                                                 #
     . '  MTC_SET_NO ' . '"' . $p_set_data . '"' . ' ,'         #
     . K_NEW_LN                                                 #
     . '  MTC_RUN_NO ' . '"' . $p_run_no . '"'                  #
     . K_NEW_LN . ')' ;

   open ( $FILECTL , '>' , $v_work_file_dir . $vuk_sql_ldr_ctl_file ) or                               # Create and open sql loader control file
     die "Can not open sql loader control file $vuk_sql_ldr_ctl_file - $!" . K_NEW_LN ;

   print $FILECTL $v_sql_ldr_sg ;                               # Write sql loader control file

   close $FILECTL ;                                             # Close control file

   # END SCRIPT OF SQL LOADER CONTROL FILE  ########################################################

   # CALL SQL LOADER ###############################################################################

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

   $v_sec_to_exe_upl_sort_mtc = time () - $v_start_time ;       # Time to execute upload sort match
   $v_upl_sort_mtc_end_ts     = &sGetCurTimestamp ;             # Sort match upload ended

   print K_NEW_LN . "Sort match data uploading and script ended" . K_SPACE . $v_upl_sort_mtc_end_ts    #
     . ' - '                                                    #
     . strftime ( "\%H:\%M:\%S" , gmtime ( $v_sec_to_exe_upl_sort_mtc ) )                              #
     . K_SPACE . "to execute" . K_NEW_LN ;

   # Bad file of sql loader size
   my $vuk_bad_file_size = -s $v_work_file_dir . $vuk_bad_file || K_ZERO ;

   # Discard file of sql loader size
   my $vuk_discard_file_size = -s $v_work_file_dir . $vuk_discard_file || K_ZERO ;

   # Unlink file if size is less than 1 means delete
   if ( $vuk_bad_file_size < 1 )     { unlink $v_work_file_dir . $vuk_bad_file ; }
   if ( $vuk_discard_file_size < 1 ) { unlink $v_work_file_dir . $vuk_discard_file ; }

} ## end sub sUplSortMtcOracle
#######################################################################
# End of subroutine sUplSortMtcOracle                                 #
#######################################################################

sub sWriteLog {

   # Write log file
   my $vwl_sort_in_file_dir = $v_sort_in_file_dir ; 
   if ( $vwl_sort_in_file_dir ne './' ) { 
     
     if ( $v_operating_system eq "MSWin32" ) {         
       $vwl_sort_in_file_dir    =~ s/\//\\/g ;                  # Replace forward slash to back slash for cmd command
     }
     elsif ( $v_operating_system eq "linux" ) {
       $vwl_sort_in_file_dir    =~ s/\\/\//g ;                  # Replace back slash to forward slash for cmd command
     }
   }
   
   # Count of input records in a file
   $v_in_rec_knt = qx( findstr /R /N "^" "$vwl_sort_in_file_dir$v_sort_in_file" | find /C ":" ) ;
   
   # Name of the log file
   $v_log_file =                                                #
     $p_set_data . $p_run_no . '_' . $p_prps_no . '_'           #
     . 'UplMtc' . '_'                                           #
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
     "------ UPLOAD SORT MATCH EXECUTION START DATE AND TIME ------" . K_NEW_LN    #
     . K_NEW_LN                                                 #
     . $v_start_timestamp . K_NEW_LN . K_NEW_LN                 #

     . "------ Run Parameters ------" . K_NEW_LN                #
     . "Data set number             :" . K_SPACE . $p_set_data . K_NEW_LN          #
     . "Run number                  :" . K_SPACE . $p_run_no . K_NEW_LN            #
     . "Purpose number              :" . K_SPACE . $p_prps_no . K_NEW_LN           #
     . (
      $p_sort_in_file_dir eq K_EMPTY
      ? K_EMPTY
      : "Sort input file directory   :" . K_SPACE . $p_sort_in_file_dir . K_NEW_LN
     )                                                          #
     . (
      $p_log_file_dir eq K_EMPTY
      ? K_EMPTY
      : "Log File Directory          :" . K_SPACE . $p_log_file_dir . K_NEW_LN
     )                                                          #
     . (
      $p_work_file_dir eq K_EMPTY
      ? K_EMPTY
      : "Work file directory         :" . K_SPACE . $p_work_file_dir . K_NEW_LN
     )                                                          #
     . "Database user               :" . K_SPACE . $p_in_user . K_NEW_LN           #
     . "Database password           :" . K_SPACE . "<_not_printed_>" . K_NEW_LN    #
     . "Database file location      :" . K_SPACE . $v_in_dbdir . K_NEW_LN          #
     . "Database (Sid) name         :" . K_SPACE . $p_in_dbname . $p_set_data . K_NEW_LN         #
     . "Database type               :" . K_SPACE . $p_in_dbtype . K_NEW_LN         #
     . "Database port               :" . K_SPACE . $p_in_dbport . K_NEW_LN         #
     . "Database Server (Host) name :" . K_SPACE . $p_in_dbserver . K_NEW_LN       #
     . "Flag - verbose - details dsp:" . K_SPACE . $p_f_verbose . K_NEW_LN         #
     . K_NEW_LN                                                                    #

     . "------ File Names ------" . K_NEW_LN                                       #                                               #
     . "Input sort match file name  :" . K_SPACE . $v_sort_in_file . K_NEW_LN      #
     . "Log file name               :" . K_SPACE . $v_log_file . K_NEW_LN          #
     . K_NEW_LN
     . "------ Run summary------" . K_NEW_LN                                                                  #
     . "Input records count         :" . K_SPACE . $v_in_rec_knt                   #
     . ( $v_sqlite_log_file_sz >=1 ?                                               #
       "Upload records count        : *** No data uploaded in Table***" :          #
       "Upload records count        :" . K_SPACE . $v_op_rec_knt )                 #
     . K_NEW_LN 
     . ( 
         ($v_sqlite_log_file_sz < 1 ) and ( $v_in_rec_knt == $v_op_rec_knt ) ? 
         "****** File uploaded Successfully ******" . K_NEW_LN : K_EMPTY
       );                                                        #
                                                                                   
   print $FILELOG $v_log ;                                       # Write log file   
     
   if ( $p_f_verbose eq K_Y and $p_in_dbtype eq "Oracle" ) {     # If
      print $FILELOG K_NEW_LN . "SQL Loader Control file :" . K_NEW_LN ;
      print $FILELOG $v_sql_ldr_sg . K_NEW_LN . K_NEW_LN ;      # Print SQL Loader control file in log file
   }

   if ( $p_f_verbose eq K_Y and $p_in_dbtype eq "SQLite" ) {
      print $FILELOG K_NEW_LN . "SQLite Control file :" . K_NEW_LN ;
      print $FILELOG $v_sqlite_cmd . K_NEW_LN . K_NEW_LN ;      # Print SQLte control file in log file
   }

   print $FILELOG "Sort match data uploading and script ended" . K_SPACE . $v_upl_sort_mtc_end_ts    #
     . ' - '                                                    #
     . strftime ( "\%H:\%M:\%S" , gmtime ( $v_sec_to_exe_upl_sort_mtc ) )                            #
     . K_SPACE . "to execute" . K_NEW_LN . K_NEW_LN;

   close $FILELOG                                               # Close log file
     or die "Can not close log file $v_log_file - $!" . K_NEW_LN ;

   if ( $p_in_dbtype eq "SQLite" and $v_sqlite_log_file_sz >= 1 ) {
      # Using type command write SQLite log file into main procedure log file
      system (
         "Type" . K_SPACE . '"' . $v_work_file_dir_cmd . $v_sqlite_log_file . '"'                   #
           . ">>" . K_SPACE . '"' . $v_log_file_dir . $v_log_file . '"'                              #
      ) ;

      unlink $v_work_file_dir . $v_sqlite_log_file ;            # Delete SQLite log file after write into main log file
   } ## end if ( $p_in_dbtype eq "SQLite")

   if ( $p_in_dbtype eq "Oracle" and $v_sqlite_log_file_sz >= 1 ) {

      # Using type command write sql loader file into main procedure log file
      system (
         "Type" . K_SPACE . '"' . $v_work_file_dir_cmd . $v_sql_ldr_log_file . '"'                   #
           . ">>" . K_SPACE . '"' . $v_log_file_dir . $v_log_file . '"'                              #
      ) ;

      unlink $v_work_file_dir . $v_sql_ldr_log_file ;           # Delete temporary sort match log file after write into main log file
   } ## end if ( $p_in_dbtype eq "Oracle")

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
     "insert into dedupe . T_XST_XED ("                         #
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
     "update dedupe . T_XST_XBG set"                            #
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
# End of subroutine sWindUp                                        #
#####################################################################

sub sGetTcpIp {                                                 # Get TCP/IP of machine

   use Sys::Hostname ;
   my $host = hostname ;

   use Socket ;
   my $vgti_ip = inet_ntoa ( scalar gethostbyname ( $host || 'localhost' ) ) ;

   return $vgti_ip ;

} ## end sub sGetTcpIp
#####################################################################
# End of subroutine sGetTcpIp                                      #
#####################################################################

=pod

=head1 44_517_UplMtc.pl - Upload sort match data

 Upload Search id , File id , Purpose no , Conservative Match Decision ,
  Conservative Match Score , Typical Match Decision , Typical Match Score ,
  Loose Match Decision and Loose Match Score which are sort by search id
  using 44_516_SortMtc.pl procedure.

 Upload above details in dedupe . T_DAT_MTC table

=head2 Copyright

 Copyright (c) 2017 IdentLogic Systems Private Limited

=head2 Description

 After creating .mst file , they are uploaded into table  -
  dedupe . T_DAT_MTC.
 Upload sort match file using sqlloader utility.
 Create control file with extension of .ctl for SQl Loader to upload .mst file
  into table

 Format of Input sort match file : sssrrrr_purposeno.mst - TAB delimited

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

=head3 Format of log file

  Log file will be created with data set number, run number , purpose number ,
   procedure name and date time
  for eg. sssrrrr_purposeno_UplMtc_YYYY-MM-DD-HH24-MI-SS.log

  Log file name contains below information.

  ------ UPLOAD SORT MATCH EXECUTION START DATE AND TIME ------

  YYYY-MM-DD HH24:MI:SS

  ------ Run Parameters ------
  Displayed all run parameters which are used:
  Data set number             : Data set number starting from 100 to 999
  Run number                  : Run number starting from 1000 to 9999
  Purpose number              : Purpose number starting from 100 to 999
  Sort input file directory   : Sort input file directory
  Log File Directory          : Log File Directory path
  Work file directory         : Work file directory path
  Database user               : Database user name
  Database password           : Database user password
  Database file location      : Location of database file created in SQLite
  Database (Sid) name         : Database (Sid) name
  Database connection string  : Database connection string
  Database port               : Database port
  Database Server (Host) name : Database server
  Verbose - print details     : Y/n

  ------ File Names ------

  Displayed all file names:
  Input sort match file name  : <sssrrrr_purposeno.mst>
  Log file name               : <sssrrrr__purposeno_UplMtc_YYYY-MM-DD-HH24-MI-SS.log>

  Display if verbose flag is on

  EITHER - Oracle
  ---------------

  SQL Loader Control file :
  Print SQL Loader control file

  SQL Loader log also inserted in this log file.

  OR     - SQLite
  ---------------

  SQLite Control file :
  .open Database_directory_path /dedupe{rrrr}
  .log ./sssrrrr_{purposeno}_SQLite.log
  create table T_DAT_MTC{1000}
  (
    mtc_srh_id   NVARCHAR2(1000) not null,
    mtc_fi_id    NVARCHAR2(1000) not null,
    mtc_pps_sn   NUMBER(3) not null,
    mtc_t_dc     CHAR(1) not null,
    mtc_t_no     NUMBER(3) not null,
    mtc_c_dc     CHAR(1) not null,
    mtc_c_no     NUMBER(3) not null,
    mtc_l_dc     CHAR(1) not null,
    mtc_l_no     NUMBER(3) not null
  );
  .separator '\t'
  .import ./sssrrrr_{purposeno}.mst T_DAT_MTC{rrrr}
  .bail on
  .quit

  If error in SQLite log file write in main procedure log file

  Sort match data uploading and script ended YYYY-MM-DD HH24:MI:SS - hh:mm:ss to execute

=head3 Terminal

 When Verbose flag is on

 Started at localtime

 Run parameter which entered by user

 Dataset                     : Data set number starting from 100 to 999
 Run number                  : Run number starting from 1000 to 9999
 Purpose number              : Purpose number starting from 100 to 999
 Sort input file directory   : Sort input file directory
 Log file directory          : Log File Directory path
 Work file directory         : Work file directory path
 Database user               : Database user name
 Database password           : Database user password
 Database file location      : E:\SurendraK\Work\DeDupeProcs\UplMtc\Database
 Database (Sid) name         : Database (Sid) name
 Database connection string  : Database connection string
 Database port               : Database port
 Database Server (Host) name : Database server
 Flag - Verbose              : Y/n

 Sort match data uploading and script ended YYYY-MM-DD HH24:MI:SS - hh:mm:ss to execute

=head3 Checks leading to procedure abort

   i. Data set number not in the range of 100 to 999
  ii. Run number not in the range of 1000 to 9999
 iii. Data set number not specified
  iv. Run number not specified
   v. Input file not exist

=head2 Technical

 Script name      - 44_517_UplMtc.pl
 Package Number   - 44
 Procedure Number - 517

=head3 Run parameters

 PARAMETER      DESCRIPTION                            ABV  VARIABLE
 ---------      ---------------------------------      ---  -------------------
 datasetno      Dataset number                          d   $p_set_data
 runno          Run number                              r   $p_run_no
 purposeno      Purpose number                          k   $p_prps_no
 asortfldir     Input file directory - optional         a   $p_sort_in_file_dir
 logfldir       Log file directory - optional           l   $p_log_file_dir
 workfldir      work file directory - optional          w   $p_work_file_dir
 user           Database user                           u   $p_in_user
 guptshabd      Database password                       g   $p_in_dbpw
 port           Database port                           p   $p_in_dbport
 cncdb          Database Server (Host) name             c   $p_in_dbtype
 ydbdir         Database file location directory path   y   $p_in_dbdir
 xdbname        Database (Sid) name                     x   $p_in_dbname
 tcpip          Database connection string              t   $p_in_dbserver
 verboseflag    Flag - Verbose - print details *        v   $p_f_verbose

 * - No argument needed

 SQLite db:

    Parameter of Dataset number(s) , Run number (r) , Purpose number (k) and
     Database file location (y) are mandatory.
    user , password , dbport , tcpip are not required for SQlite

 Oracle db:

    Parameter of Dataset number(s) , Run number (r) , Purpose number (k) are mandatory.
    Database file location (y) option are not required.
    user , password , port , tcpip , xdbname options are required for Oracle

 ABV:- Abbreviation for calling run parameter,

 SQLite db : e.g  44_517_UplMtc.pl -d 100 -r 1000 -k 119 -y E:\SurendraK\Database -v
 Oracle db : e.g. 44_517_UplMtc.pl -d 100 -r 1000 -k 119 -v

=head4 Help and defaults

 For detailed help with defaults run: Perl <program_name> -h.

=head3 Subroutines

 Subroutine          Description
 ------------------  -----------------------------------------------------------
 sGetCurTimestamp    Get current formatted timestamp

 sGetParameters      Initial: Gets run parameters and check input parameter
                      values. Procedure abort with message if any error.

 sInit               Inserts begin details in table dedupe . T_XST_XBG

 sUplSortMtcSQLite   Upload sort match file data in SQLite database table
                      - dedupe{sss} . T_DAT_MTC{rrrr}
                      New database for every next dataset and New table
                      for every next run no

 sUplSortMtcOracle   Upload sort match file data in Oracle database table
                      - dedupe . T_DAT_MTC

 sWriteLog           Write procedure run summury in log file

 sWindUp             Inserts procedure end record in dedupe . T_XST_XED and
                      updates start record in dedupe . T_XST_XBG.

 sGetTcpIp           Get TCP/IP of run machine

=head4 Called by

 Subroutine          Called by
 ----------------    ------------------------------------
 sGetCurTimestamp    sUplSortMtcSQLite , sUplSortMtcOracle , sWindUp
 sGetParameters      Main
 sInit               Main
 sUplSortMtcSQLite   Main
 sUplSortMtcOracle   Main
 sWriteLog           Main
 sWindUp             Main
 sGetTcpIp           Main

=head4 Calling

 Subroutine          Calling Subroutine
 ---------------     ------------------------------------
 sUplSortMtcOracle   sGetCurTimestamp
 sWindUp             sGetCurTimestamp

=head4 Subroutine structure

 Main
  |--- sGetTcpIp
  |
  |--- sGetParameters
  |
  |--- sUplSortMtcSQLite
  |       |--- sGetCurTimestamp
  |
  |--- sInit
  |
  |--- sUplSortMtcOracle
  |       |--- sGetCurTimestamp
  |
  |--- sWindUp
  |       |--- sGetCurTimestamp
  |
  \--- sWriteLog

=head3 SQLite db Tables and fields referenced

 dedupe{sss} . T_DAT_MTC{rrrr}

 Fields            : DESCRIPTION
 ---------------     ----------------------------------------------------------
 MTC_SRH_ID          Search ID ~ Record of this run
 MTC_FI_ID           File ID ~ Record of this and previous runs
 MTC_PPS_SN          Serial number ~ Of the purpose within the Data Set
 MTC_T_DC            Typical Match Decision
 MTC_T_NO            Typical Match Score
 MTC_C_DC            Conservative Match Decision
 MTC_C_NO            Conservative Match Score
 MTC_L_DC            Loose Match Decision
 MTC_L_NO            Loose Match Score

=head3 Oracle db Tables and fields referenced

 dedupe . T_DAT_MTC  : WRITE MODE : Match

 Fields            : DESCRIPTION
 ---------------     ----------------------------------------------------------
 MTC_SET_NO          Data set number
 MTC_RUN_NO          Run number
 MTC_PPS_SN          Serial number ~ Of the purpose within the Data Set
 MTC_SRH_ID          Search ID ~ Record of this run
 MTC_FI_ID           File ID ~ Record of this and previous runs
 MTC_C_DC            Conservative Match Decision
 MTC_C_NO            Conservative Match Score
 MTC_T_DC            Typical Match Decision
 MTC_T_NO            Typical Match Score
 MTC_L_DC            Loose Match Decision
 MTC_L_NO            Loose Match Score
 MTC_JB_SQ_NO        Unique identification of job sequence no
 MTC_CRR_UR_U        User Creating ~ Login Id
 MTC_CRR_TS          Timestamp of Creation
 MTC_CRR_TP          TCP/IP Address of the Computer Creating this row
 MTC_CRR_PC          Procedure Creating this row

 dedupe . T_XST_XBG  : WRITE MODE : Activity Begin Security Log

 Fields            : DESCRIPTION
 ---------------     ----------------------------------------------------------

 XBG_SET_NO        : Data Set number
 XBG_RUN_NO        : Run number
 XBG_CRR_UR_U      : User Creating ~ Login Id
 XBG_PC_NM_U       : Procedure Name
 XBG_PC_BG_TS      : Timestamp of Procedure Begin
 XBG_F_PC_ED_NT    : Flag - Procedure End not Flagged
 XBG_F_PC_ED_DTC   : Flag - Procedure End detected
 XBG_PC_ED_TS      : Timestamp of Procedure End
 XBG_F_ABT         : Flag - Abort
 XBG_CRR_TS        : Timestamp of Creation
 XBG_CRR_TP        : TCP/IP Address of the Computer Creating this row
 XBG_CRR_PC        : Procedure Creating this row
 XBG_TLM_UR_U      : Last modified by user
 XBG_TLM_TS        : Last modification timestamp
 XBG_TLM_TP        : TCP/IP address of the computer last modifying this record
 XBG_TLM_PC        : Procedure last modifying this record
 XBG_JB_SQ_NO      : Job sequence number
 XBG_F_A           : Flag - Active

 dedupe . T_XST_XED  : WRITE MODE : Activity End Security Log

 Fields             : DESCRIPTION
 ---------------      ----------------------------------------------------------

 XED_SET_NO         : Data Set number
 XED_RUN_NO         : Run number
 XED_CRR_UR_U       : User ~ Login Id
 XED_PC_NM_U        : Procedure Name
 XED_PC_BG_TS       : Timestamp of Procedure Begin
 XED_PC_ED_TS       : Timestamp of Procedure End
 XED_NO_CSE_INS     : Number of cases inserted
 XED_F_PC_BG_NT_DTC : Flag - Procedure Begin Not detected
 XED_F_ABT          : Flag - Abort
 XED_CRR_TS         : Timestamp of Creation
 XED_CRR_TP         : TCP/IP Address of the Computer Creating this row
 XED_CRR_PC         : Procedure Creating this row
 XED_JB_SQ_NO       : Job Sequence number

=head3 Perl modules used

 DBI
 Getopt::Simple
 IO::File
 POSIX qw{strftime}

=cut

#######################################################################
# End of 44_517_UplMtc.pl                                             #
#######################################################################

