#!C:/Perl/bin/perl.EXE -w
#Script: ManageActivity.pl

use strict ;
use warnings ;
use utf8 ;
use CGI qw( :standard ) ;
use CGI::Carp qw/fatalsToBrowser warningsToBrowser/ ;

#use lib '.' ;
use Date::Calc qw( :all ) ;                                     # Object Oriented time objects for calculating week number of year
use DBI ;
use URI::Encode ;
use ISPL qw ( :DEFAULT &sSPutFJsRn $i_f_js_rn ) ;
use Digest::SHA qw ( sha1_hex ) ;
use Encode ( 'decode_utf8' , 'encode_utf8' ) ;
use Config::Tiny ;
binmode STDOUT , ':encoding( UTF-8 )' ;

use ILCL (                                                      #
   ':DEFAULT' ,                                                 # DEFAULT
   'sIDelOldExrFls' ,                                           # Delete number of days old files from extraction directory
   'sIBreadcrumb' ,                                             # Create bread crumb
   '&sITx' ,                                                    # Translate string into locale
   'sIValidate' ,                                               # Return session variables if valid session
   'sIPrtHidSessVars' ,                                         # Hidden fields of session variable
   '$dbh' ,                                                     # Database handle
   '$dbh_web' ,                                                 # Web Database handle
   '$i_formatted_timestamp' ,                                   # Formatted timestamp
   '$X_SES_NO' ,                                                # Session Id
   '$X_UR_U' ,                                                  # User
   '$X_TP' ,                                                    # TCP/IP Address of the Computer
   '$X_I_TM' ,                                                  # Inactivity Timeout in seconds
   '$X_SHA_1' ,                                                 # Hash of all fields tab separated
   '$X_CSS_CC' ,                                                # Cascading Style Sheet Choice
   '$X_L_TY_NO' ,                                               # Preferred Communication Locale Type Number
   '$X_RX_P_AJ' ,                                               # Restrict Points Adjustment
   '$X_MX_RC_XTC' ,                                             # Maximum number of records that can be extracted
   '$X_GR_U' ,                                                  # Group
   '$X_OPL_L_TY_NO' ,                                           # Operational Communication Locale Type Number
   '$X_DRN' ,                                                   # Direction
   '$X_SNO_PG' ,                                                # Site Number for Programme
   '$X_MS_SN' ,                                                 # Membership Serial Number
   '$X_CA_ID_U' ,                                               # Client Card Id
   '$X_SNO_PY' ,                                                # Site Number of Programme Tier
   '$i_q' ,                                                     # CGI object
   '$X_XC_TY_NO' ,                                              # Currency Type Number
   '$X_DE_C_U' ,                                                # Decimal character - Which separates integer and decimal part of a number. Usually dot comma or space.
   '$X_DGT_SI_U' ,                                              # Digit group separator character - Which separates groups of digits in the integer part of a number. usually comma dot or space.
   '$X_XC_DE_DGT' ,                                             # Currency Decimal digits - Negative for Main number, e.g. -1 for units 0, -2 for tens 0, etc.
   '$X_NO_1_SI_PS' ,                                            # Number of digits left of decimal point where a break occurs the first time - Zero means no break. E.g. 3 for 1,234
   '$X_NO_NX_SI_PS' ,                                           # Number of digits left of decimal point where a break occurs after the first break - Zero means no break. E.g. 3 for 1,234,567
   '$X_M_NM_U' ,                                                # Member Name
   '$I_PDBSCM' ,                                                # Schema name
   '$I_PDBSCMWEB' ,                                             # Webschema name
   '$I_PFAVICONFL' ,                                            # Favicon file name
   '$I_PDIRBASE' ,                                              # Base directory
   '$I_PDIRICONS' ,                                             # Icon directory
   '$I_PSEARCHLBLCL' ,                                          # Search label color
   '$I_PSEARCHODDCL' ,                                          # Odd background row color ( Search )
   '$I_PSEARCHEVENCL' ,                                         # Background and background even row color ( Search )
   '$I_PERRORCL' ,                                              # Error color
   '$I_PWARNCL' ,                                               # Warning color
   '$I_PTIMEOUTSECS' ,                                          # Timeout in seconds
   '$I_PMSGCL' ,                                                # Message color
   '$I_PDIRCSS' ,                                               # Directory of css - Relative to base
   '$I_PTEXTLENGTH' ,                                           # Maximum text length
   '$I_PTEXTBOXLINES' ,                                         # Maximum lines in a text box
   '$I_PHEADCL' ,                                               # Table header color ( Header )
   '$I_PVIEWEVENCL' ,                                           # View background even row color
   '$I_PVIEWODDCL' ,                                            # View odd row color
   '$I_PMAXSEARCHRECS' ,                                        # Maximum number of records shown on a screen taken as 50 if unknown
   '$I_PVIEWLBLCL'                                              # View label colour
) ;

#my ( $iv_return_ok ) = &sIValidate ( ) ;

$X_CSS_CC  = "Normal" ;
$X_GR_U    = "TEST1" ;
$X_L_TY_NO = 1 ;

my $q = new CGI ;                                               # Create a new Perl CGI object

use constant K_PACKAGE             => 45 ;                      # Package number
use constant K_PROCEDURE           => 501 ;                     # Procedure number
use constant K_PROCEDURE_NAME      => '45_501_MngActSett.pl' ;  # Procedure name
use constant K_PROCEDURE_NAME_FILE => '45_501_MngActSett.pl' ;  # Procedure name
use constant K_DESCRIPTION         => 'Manage action setting' ; # Procedure name in System Locale ( Description / Alternate Name )

use constant K_ILCL           => "ILCL" ;                       # ILCL
use constant K_EMPTY          => '' ;                           # Empty field
use constant K_YES            => 'Y' ;                          # Yes flag value
use constant K_NO             => 'n' ;                          # No flag value
use constant K_HASH           => "\x{23}" ;                     # Hash character value
use constant K_I_ANY          => 'Any' ;                        # Any indicator value
use constant K_I_BOTH         => 'Both' ;                       # Both indicator value
use constant K_I_YES          => 'Yes' ;                        # Yes indicator value
use constant K_I_NO           => 'no' ;                         # No indicator value
use constant K_I_EITHER       => 'either' ;                     # Either indicator value
use constant K_NEWLINE        => "\n" ;                         # New line
use constant K_SPACE          => " " ;                          # Space
use constant K_SNG_APO        => "'" ;                          # Single quote
use constant K_COMMA          => "," ;                          # Comma
use constant K_ZERO           => 0 ;                            # Zero field
use constant K_ONE            => 1 ;                            # One field
use constant K_SEARCH_LIMIT   => 50 ;                           # Maximum number of records returned after search
use constant K_LEN_CHK_MULT   => 2 ;                            # Multiplier of maximum length of screen field for fatal error check
use constant K_PGM_SNO_MAX_LN => 10 ;                           # Maximum length of screen field Program Serial Number
use constant K_PGM_NM_MAX_LN  => 200 ;                          # Maximum length of screen field Program Name
use constant K_CA_ID_MAX_LN   => 40 ;                           # Maximum length of screen field Card Id
use constant K_NAME_MAX_LN    => 200 ;                          # Maximum length of screen field Name
use constant K_MS_SN_MAX_LN   => 15 ;                           # Maximum length of screen field MS_SN Memebr Serial Number
use constant K_PE_SN_MAX_LN   => 15 ;                           # Maximum length of screen field PE_SN Person Serial Number
use constant K_DGN_MAX_LN     => 200 ;                          # Maximum length of screen field DESIGNATION
use constant K_DPT_MAX_LN     => 200 ;                          # Maximum length of screen field DEPARTMENT
use constant K_OGN_MAX_LN     => 200 ;                          # Maximum length of screen field ORGANIZATION
use constant K_PE_EM_MAX_LN   => 1000 ;                         # Maximum length of screen field EMAIL ID
use constant K_PHN_MAX_LN     => 15 ;                           # Maximum length of screen field PHONE
use constant K_PO_CD_MAX_LN   => 40 ;                           # Maximum length of screen field POST CODE
use constant K_S_APO          => "\x{27}" ;                     # Single apostrophe
use constant K_D_APO          => "\x{22}" ;                     # Double variable

use constant K_MAX_YR_LN            => 4 ;                      # Maximum length for year text field
use constant K_MAX_BG_DT_RNG_ERR_LN => 80 ;                     # Maximum length of text field to display error for begin range field
use constant K_MAX_TCP_IP_ERR_LN    => 30 ;                     # Maximum length of text field to display error for tcp name field
use constant K_MAX_BG_ED_DT_RNG_ERR_LN => 120 ;                 # Maximum length of text field to display error for start and end timestamp range field

my @a_L                   = () ;                                # Contains all labels and messages
my @a_srch_execute_values = () ;                                # Array store values to execute sql

my $v_count                = 0 ;                                # Record count
my $v_srch_per_f_sql_where = K_NO ;                             # Flag to see if where clause started

my $v_message = K_EMPTY ;                                       # Message
my $v_f_err          = K_NO ;                                   # Flag - error in search

my $p_ths_frm = K_EMPTY ;

my $v_bg_frm_ts = K_EMPTY ;                                     # Begin timestamp ( FROM )
my $v_bg_to_ts  = K_EMPTY ;                                     # Begin timestamp ( TO )
my $v_bg_ts_fmt = K_EMPTY ;                                     # Timestamp format
my $v_ed_frm_ts = K_EMPTY ;                                     # End timestamp ( FROM )
my $v_ed_to_ts  = K_EMPTY ;                                     # End timestamp ( TO )
my $v_ed_ts_fmt = K_EMPTY ;                                     # Timestamp format

my $vgc_sql = 0 ;
print $q -> header ( -charset => 'utf-8' ) ;

# Timestamp variables

my $v_now_ss = K_EMPTY ;                                        # Time elements - second ...
my $v_now_mi = K_EMPTY ;                                        # ... minute ...
my $v_now_hh = K_EMPTY ;                                        # ... hour - 24 hour format ...
my $v_now_dd = K_EMPTY ;                                        # ... day ...
my $v_now_mm = K_EMPTY ;                                        # ... month ...
my $v_now_yy = K_EMPTY ;                                        # ... year
( $v_now_ss , $v_now_mi , $v_now_hh , $v_now_dd , $v_now_mm , $v_now_yy )    # Get time elements
  = ( localtime )[ 0 , 1 , 2 , 3 , 4 , 5 ] ;

$v_now_yy = $v_now_yy + 1900 ;                                  # Adjust year
$v_now_mm = $v_now_mm + 1 ;                                     # Adjust month

$v_now_mm = $v_now_mm < 10 ? '0' . $v_now_mm : $v_now_mm ;      #
$v_now_dd = $v_now_dd < 10 ? '0' . $v_now_dd : $v_now_dd ;      #
$v_now_hh = $v_now_hh < 10 ? '0' . $v_now_hh : $v_now_hh ;      #
$v_now_mi = $v_now_mi < 10 ? '0' . $v_now_mi : $v_now_mi ;      #
$v_now_ss = $v_now_ss < 10 ? '0' . $v_now_ss : $v_now_ss ;      #

my $v_title_timestamp =                                         # For use in title bar - put leading 0 for 2 digit fields when < 10
  $v_now_yy . '-'                                               #
  . $v_now_mm . '-'                                             #
  . $v_now_dd . ' '                                             #
  . $v_now_hh . ':'                                             #
  . $v_now_mi . ':'                                             #
  . $v_now_ss                                                   #
  ;                                                             #

# Global variable used in this procedure
my $v_srch_bg_dt_err_msg     = K_EMPTY ;                        # Store error message for field start job time
my $v_srch_ed_dt_err_msg     = K_EMPTY ;                        # Store error message for field end job time
my $v_srch_tcp_ip_err_msg    = K_EMPTY ;                        # Store error message for field tcp ip
my $v_srch_job_bg_ed_err_msg = K_EMPTY ;                        # Common err message field for job start and end timestamp fields

my $v_srch_bg_from_yy = K_EMPTY ;                               # Begin appicable timestamp - year ( FROM )
my $v_srch_bg_from_mm = K_EMPTY ;                               # Begin appicable timestamp - month ( FROM )
my $v_srch_bg_from_dd = K_EMPTY ;                               # Begin appicable timestamp - day ( FROM )
my $v_srch_bg_from_hh = K_EMPTY ;                               # Begin appicable timestamp - hour( FROM )
my $v_srch_bg_from_mi = K_EMPTY ;                               # Begin appicable timestamp - minute( FROM )
my $v_srch_bg_from_ss = K_EMPTY ;                               # Begin appicable timestamp - second ( FROM )
my $v_srch_bg_to_yy   = K_EMPTY ;                               # Begin appicable timestamp - year ( TO )
my $v_srch_bg_to_mm   = K_EMPTY ;                               # Begin appicable timestamp - month ( TO )
my $v_srch_bg_to_dd   = K_EMPTY ;                               # Begin appicable timestamp - day ( TO )
my $v_srch_bg_to_hh   = K_EMPTY ;                               # Begin appicable timestamp - hour ( TO )
my $v_srch_bg_to_mi   = K_EMPTY ;                               # Begin appicable timestamp - minute ( TO )
my $v_srch_bg_to_ss   = K_EMPTY ;                               # Begin appicable timestamp - second( TO )

my $v_srch_ed_from_yy = K_EMPTY ;                               # End appicable timestamp - year ( FROM )
my $v_srch_ed_from_mm = K_EMPTY ;                               # End appicable timestamp - month ( FROM )
my $v_srch_ed_from_dd = K_EMPTY ;                               # End appicable timestamp - day ( FROM )
my $v_srch_ed_from_hh = K_EMPTY ;                               # End appicable timestamp - hour ( FROM )
my $v_srch_ed_from_mi = K_EMPTY ;                               # End appicable timestamp - minute ( FROM )
my $v_srch_ed_from_ss = K_EMPTY ;                               # End appicable timestamp - second ( FROM )
my $v_srch_ed_to_yy   = K_EMPTY ;                               # End appicable timestamp - year ( TO )
my $v_srch_ed_to_mm   = K_EMPTY ;                               # End appicable timestamp - month ( TO )
my $v_srch_ed_to_dd   = K_EMPTY ;                               # End appicable timestamp - day ( TO )
my $v_srch_ed_to_hh   = K_EMPTY ;                               # End appicable timestamp - hour ( TO )
my $v_srch_ed_to_mi   = K_EMPTY ;                               # End appicable timestamp - minute ( TO )
my $v_srch_ed_to_ss   = K_EMPTY ;                               # End appicable timestamp - second ( TO )
my $v_srch_f_err      = K_NO ;                                  # Flag - default value n, set to Y if any error in main screen

my $p_h_rtn_mn_url =
  $q -> param ( 'p_h_rtn_mn_url' ) || K_EMPTY ;                 # Hidden variable for return main url                                                     # Get parameter value of retrieve selected members
my $p_h_brd_crmb = $q -> param ( 'p_h_brd_crmb' ) || K_EMPTY ;  # Hidden variable for breadcrumb                                                     # Get parameter value of retrieve selected members

$p_h_brd_crmb = decode_utf8 ( $p_h_brd_crmb ) ;

# PARAMETERS of Search form

my $p_srch_site_nm = $q -> param ( 'p_srch_site_nm' ) || K_EMPTY ;        # Site number

# Begin applicable timestamp parameters
my $p_srch_bg_frm_yy = $q -> param ( 'p_srch_bg_frm_yy' ) || K_EMPTY ;    # Begin Year ( FROM )
my $p_srch_bg_frm_mm = $q -> param ( 'p_srch_bg_frm_mm' ) || K_EMPTY ;    # Begin Month ( FROM )
my $p_srch_bg_frm_dd = $q -> param ( 'p_srch_bg_frm_dd' ) || K_EMPTY ;    # Begin Day ( FROM )
my $p_srch_bg_frm_hh = $q -> param ( 'p_srch_bg_frm_hh' ) || K_EMPTY ;    # Begin Hour ( FROM )
my $p_srch_bg_frm_mi = $q -> param ( 'p_srch_bg_frm_mi' ) || K_EMPTY ;    # Begin Minutes ( FROM )
my $p_srch_bg_frm_ss = $q -> param ( 'p_srch_bg_frm_ss' ) || K_EMPTY ;    # Begin Seconds ( FROM )

my $p_srch_bg_to_yy = $q -> param ( 'p_srch_bg_to_yy' ) || K_EMPTY ;      # Begin Year ( TO )
my $p_srch_bg_to_mm = $q -> param ( 'p_srch_bg_to_mm' ) || K_EMPTY ;      # Begin Month ( TO )
my $p_srch_bg_to_dd = $q -> param ( 'p_srch_bg_to_dd' ) || K_EMPTY ;      # Begin Day ( TO )
my $p_srch_bg_to_hh = $q -> param ( 'p_srch_bg_to_hh' ) || K_EMPTY ;      # Begin Hour ( TO )
my $p_srch_bg_to_mi = $q -> param ( 'p_srch_bg_to_mi' ) || K_EMPTY ;      # Begin Minutes ( TO )
my $p_srch_bg_to_ss = $q -> param ( 'p_srch_bg_to_ss' ) || K_EMPTY ;      # Begin Seconds ( TO )

# End applicable timestamp
my $p_srch_ed_frm_yy = $q -> param ( 'p_srch_ed_frm_yy' ) || K_EMPTY ;    # End Year ( FROM )
my $p_srch_ed_frm_mm = $q -> param ( 'p_srch_ed_frm_mm' ) || K_EMPTY ;    # End Month ( FROM )
my $p_srch_ed_frm_dd = $q -> param ( 'p_srch_ed_frm_dd' ) || K_EMPTY ;    # End Day ( FROM )
my $p_srch_ed_frm_hh = $q -> param ( 'p_srch_ed_frm_hh' ) || K_EMPTY ;    # End Hour ( FROM )
my $p_srch_ed_frm_mi = $q -> param ( 'p_srch_ed_frm_mi' ) || K_EMPTY ;    # End Minutes ( FROM )
my $p_srch_ed_frm_ss = $q -> param ( 'p_srch_ed_frm_ss' ) || K_EMPTY ;    # End Seconds ( FROM )

my $p_srch_ed_to_yy = $q -> param ( 'p_srch_ed_to_yy' ) || K_EMPTY ;      # End Year ( TO )
my $p_srch_ed_to_mm = $q -> param ( 'p_srch_ed_to_mm' ) || K_EMPTY ;      # End Month ( TO )
my $p_srch_ed_to_dd = $q -> param ( 'p_srch_ed_to_dd' ) || K_EMPTY ;      # End Day ( TO )
my $p_srch_ed_to_hh = $q -> param ( 'p_srch_ed_to_hh' ) || K_EMPTY ;      # End Hour ( TO )
my $p_srch_ed_to_mi = $q -> param ( 'p_srch_ed_to_mi' ) || K_EMPTY ;      # End Minutes ( TO )
my $p_srch_ed_to_ss = $q -> param ( 'p_srch_ed_to_ss' ) || K_EMPTY ;      # End Seconds ( TO )

# All Flag variables
my $p_srch_f_ftp     = $q -> param ( 'p_srch_f_ftp' )     || K_EMPTY ;    # Flag FTP
my $p_srch_f_ssl     = $q -> param ( 'p_srch_f_ssl' )     || K_EMPTY ;    # Flag SSL
my $p_srch_f_mtc_exa = $q -> param ( 'p_srch_f_mtc_exa' ) || K_EMPTY ;    # Exact match
my $p_srch_f_mtc_cse = $q -> param ( 'p_srch_f_mtc_cse' ) || K_EMPTY ;    # Match case
my $p_srch_f_mtc_any = $q -> param ( 'p_srch_f_mtc_any' ) || K_EMPTY ;    # Match any criteria

my $p_srch_acn_clr =                                            # Button clear
  $q -> param ( 'p_srch_acn_clr' ) || K_EMPTY ;
my $p_srch_acn_cnt =                                            # Button count
  $q -> param ( 'p_srch_acn_cnt' ) || K_EMPTY ;

# TextArea
my $p_srch_tcpip = $q -> param ( 'p_srch_tcpip' ) || K_EMPTY ;            # TCP/IP

# Radio buttons Active
my $p_srch_i_mtc_act = $q -> param ( 'p_srch_i_mtc_act' ) || K_I_YES ;    # Default Yes

# Hidden parameters use in this script
my $p_srch_h_bg_from_dt = $q -> param ( 'p_srch_h_bg_from_dt' ) || K_EMPTY ;    # Begin time( FROM )
my $p_srch_h_bg_to_dt   = $q -> param ( 'p_srch_h_bg_to_dt' )   || K_EMPTY ;    # Begin time ( TO )
my $p_srch_h_ed_from_dt = $q -> param ( 'p_srch_h_ed_from_dt' ) || K_EMPTY ;    # End time ( FROM )
my $p_srch_h_ed_to_dt   = $q -> param ( 'p_srch_h_ed_to_dt' )   || K_EMPTY ;    # End time ( TO )

my $v_f_ed_emp = K_NO ;                                         # End timestamp empty flag
#   $q -> param ( 'v_f_ed_emp' ) || K_NO ;            # Flag - default n, set to Y if job end time provided

# Create flag variable and assign value no
my $v_f_ftp     = K_NO ;                                        #
my $v_f_ssl     = K_NO ;                                        #
my $v_f_mtc_exa = K_NO ;                                        #
my $v_f_mtc_cse = K_NO ;                                        #
my $v_f_mtc_any = K_NO ;                                        #

if ( $p_srch_f_ftp eq 'on' )     { $v_f_ftp     = K_YES ; }
if ( $p_srch_f_ssl eq 'on' )     { $v_f_ssl     = K_YES ; }
if ( $p_srch_f_mtc_exa eq 'on' ) { $v_f_mtc_exa = K_YES ; }
if ( $p_srch_f_mtc_cse eq 'on' ) { $v_f_mtc_cse = K_YES ; }
if ( $p_srch_f_mtc_any eq 'on' ) { $v_f_mtc_any = K_YES ; }

&sInitLabels ;                                                  # All Labels

# Begin and End Time applicable timestamp array variables

my @as_list_mm = () ;                                           # Array to store months numbers
my @as_list_dd = () ;                                           # Array to store numbers for days
my @as_list_hh = () ;                                           # Array to store numbers for hours
my @as_list_mi = () ;                                           # Array to store numbers for minutes
my @as_list_ss = () ;                                           # Array to store numbers for seconds

# Hash used in program
my %h_mm_lbl = () ;                                             # Label Month
my %h_dd_lbl = () ;                                             # Label Day
my %h_hh_lbl = () ;                                             # Label Hour
my %h_mi_lbl = () ;                                             # Label Minute
my %h_ss_lbl = () ;                                             # Label Second

push ( @as_list_mm , '' ) ;                                     # Array store month list
push ( @as_list_dd , '' ) ;                                     # Array store day list
push ( @as_list_hh , '' ) ;                                     # Array store hour list
push ( @as_list_mi , '' ) ;                                     # Array store minute list
push ( @as_list_ss , '' ) ;                                     # Array store second list

$h_mm_lbl{ '' } = $a_L[ 38 ] ;                                  # Hash month label
$h_dd_lbl{ '' } = $a_L[ 39 ] ;                                  # Hash day label
$h_hh_lbl{ '' } = $a_L[ 40 ] ;                                  # Hash hour label
$h_mi_lbl{ '' } = $a_L[ 41 ] ;                                  # Hash minute label
$h_ss_lbl{ '' } = $a_L[ 42 ] ;                                  # Hash second label

@as_list_mm = ( '01' .. '12' ) ;                                # Array to store month numbers
@as_list_dd = ( '01' .. '31' ) ;                                # Array to store numbers for days
@as_list_hh = ( '00' .. '23' ) ;                                # Array to store hours
@as_list_mi = ( '00' .. '59' ) ;                                # Array to store minutes
@as_list_ss = ( '00' .. '59' ) ;                                # Array to store seconds

my @as_list_lbl_mm = ( '1' .. '12' ) ;                          # Array to store month numbers
my @as_list_lbl_dd = ( '1' .. '31' ) ;                          # Array to store numbers for days
my @as_list_lbl_hh = ( '0' .. '23' ) ;                          # Array to store hours
my @as_list_lbl_mi = ( '0' .. '59' ) ;                          # Array to store minutes
my @as_list_lbl_ss = ( '0' .. '59' ) ;                          # Array to store seconds

for ( my $i = 0 ; $i <= $#as_list_mm ; $i ++ ) {
   $h_mm_lbl{ $as_list_mm[ $i ] } = $as_list_lbl_mm[ $i ] ;
}

for ( my $i = 0 ; $i <= $#as_list_dd ; $i ++ ) {
   $h_dd_lbl{ $as_list_dd[ $i ] } = $as_list_lbl_dd[ $i ] ;
}

for ( my $i = 0 ; $i <= $#as_list_hh ; $i ++ ) {
   $h_hh_lbl{ $as_list_hh[ $i ] } = $as_list_lbl_hh[ $i ] ;
}

for ( my $i = 0 ; $i <= $#as_list_mi ; $i ++ ) {
   $h_mi_lbl{ $as_list_mi[ $i ] } = $as_list_lbl_mi[ $i ] ;
}

for ( my $i = 0 ; $i <= $#as_list_ss ; $i ++ ) {
   $h_ss_lbl{ $as_list_ss[ $i ] } = $as_list_lbl_ss[ $i ] ;
}

@as_list_mm = sort keys ( %h_mm_lbl ) ;
@as_list_dd = sort keys ( %h_dd_lbl ) ;
@as_list_hh = sort keys ( %h_hh_lbl ) ;
@as_list_mi = sort keys ( %h_mi_lbl ) ;
@as_list_ss = sort keys ( %h_ss_lbl ) ;

my $v_odd_count = 0 ;

my $v_r_align = 'right' ;                                       # Adjust alignment of screen fields with default right alignment for LTR
my $v_l_align = 'left' ;                                        # Adjust alignment of screen fields with default left alignment for LTR

if ( $X_DRN eq "RTL" ) {                                        #
   $v_r_align = 'left' ;                                        #
   $v_l_align = 'right' ;                                       #
}                                                               #
else {                                                          #
   $v_r_align = 'right' ;                                       #
   $v_l_align = 'left' ;                                        #
}                                                               #

if ( $p_srch_acn_clr ) {                                        # Clear form by backend
   $p_srch_site_nm   = K_EMPTY ;                                     # Site number
   $p_srch_bg_frm_yy = K_EMPTY ;                                     # Begin timestamp Year ( FROM )
   $p_srch_bg_frm_mm = K_EMPTY ;                                     # Begin timestamp Month ( FROM )
   $p_srch_bg_frm_dd = K_EMPTY ;                                     # Begin timestamp Day ( FROM )
   $p_srch_bg_frm_hh = K_EMPTY ;                                     # Begin timestamp Hour ( FROM )
   $p_srch_bg_frm_mi = K_EMPTY ;                                     # Begin timestamp Minutes ( FROM )
   $p_srch_bg_frm_ss = K_EMPTY ;                                     # Begin timestamp Seconds ( FROM )
   $p_srch_bg_to_yy  = K_EMPTY ;                                     # Begin timestamp Year ( TO )
   $p_srch_bg_to_mm  = K_EMPTY ;                                     # Begin timestamp Month ( TO )
   $p_srch_bg_to_dd  = K_EMPTY ;                                     # Begin timestamp Day ( TO )
   $p_srch_bg_to_hh  = K_EMPTY ;                                     # Begin timestamp Hour ( TO )
   $p_srch_bg_to_mi  = K_EMPTY ;                                     # Begin timestamp Minutes ( TO )
   $p_srch_bg_to_ss  = K_EMPTY ;                                     # Begin timestamp Seconds ( TO )
   $p_srch_ed_frm_yy  = K_EMPTY ;                                     # End timestamp Year ( FROM )
   $p_srch_ed_frm_mm  = K_EMPTY ;                                     # End timestamp Month ( FROM )
   $p_srch_ed_frm_dd  = K_EMPTY ;                                     # End timestamp Day ( FROM )
   $p_srch_ed_frm_hh  = K_EMPTY ;                                     # End timestamp Hour ( FROM )
   $p_srch_ed_frm_mi  = K_EMPTY ;                                     # End timestamp Minutes ( FROM )
   $p_srch_ed_frm_ss  = K_EMPTY ;                                     # End timestamp Seconds ( FROM )
   $p_srch_ed_to_yy  = K_EMPTY ;                                     # End timestamp Year ( TO )
   $p_srch_ed_to_mm  = K_EMPTY ;                                     # End timestamp Month ( TO )
   $p_srch_ed_to_dd  = K_EMPTY ;                                     # End timestamp Day ( TO )
   $p_srch_ed_to_hh  = K_EMPTY ;                                     # End timestamp Hour ( TO )
   $p_srch_ed_to_mi  = K_EMPTY ;                                     # End timestamp Minutes ( TO )
   $p_srch_ed_to_ss  = K_EMPTY ;                                     # End timestamp Seconds ( TO )
   $p_srch_f_ftp     = K_EMPTY ;                                     # Flag FTP
   $p_srch_f_ssl     = K_EMPTY ;                                     # Flag SSL
   $p_srch_f_mtc_exa = K_EMPTY ;                                     # Exact match
   $p_srch_f_mtc_cse = K_EMPTY ;                                     # Match case
   $p_srch_f_mtc_any = K_EMPTY ;                                     # Match any criteria
   $p_srch_tcpip     = K_EMPTY ;                                     # TCP/IP
   $p_srch_i_mtc_act = K_I_YES ;                                     # Active Yes , no or Either
}

if ( $p_srch_acn_cnt ) {                                   # Action on count button in Search screen
   &sCheckInput ;
   if ( $v_srch_f_err eq K_YES ) { #$p_ths_frm = "SEARCH_FORM" ; 
     &sPrintForm ;
     }
   else {
     &sGetCount ;
     &sPrintForm ;                                              # Print form
     #$p_ths_frm = "SEARCH_FORM" ;                                # This form
   }
}
else {
     #&sCheckInput ;                                             # Check input
     &sPrintForm ;                                              # Print form
}


##########################################################################
# End of Main                                                            #
##########################################################################

sub sInitLabels {

   # Labels use in procedure

   $a_L[ 0 ]   = &sITx ( "Manage Action Setting" ,                                                         $X_L_TY_NO ) ;
   $a_L[ 1 ]   = &sITx ( "Site number" ,                                                                   $X_L_TY_NO ) ;
   $a_L[ 2 ]   = &sITx ( "Begin applicable timestamp" ,                                                    $X_L_TY_NO ) ;
   $a_L[ 3 ]   = &sITx ( "End applicable timestamp" ,                                                      $X_L_TY_NO ) ;
   $a_L[ 7 ]   = &sITx ( "From" ,                                                                          $X_L_TY_NO ) ;
   $a_L[ 8 ]   = &sITx ( "To" ,                                                                            $X_L_TY_NO ) ;
   $a_L[ 9 ]   = &sITx ( "Yes" ,                                                                           $X_L_TY_NO ) ;
   $a_L[ 10 ]  = &sITx ( "No" ,                                                                            $X_L_TY_NO ) ;
   $a_L[ 11 ]  = &sITx ( "Active" ,                                                                        $X_L_TY_NO ) ;
   $a_L[ 12 ]  = &sITx ( "Inactive" ,                                                                      $X_L_TY_NO ) ;
   $a_L[ 13 ]  = &sITx ( "Exact matching" ,                                                                $X_L_TY_NO ) ;
   $a_L[ 14 ]  = &sITx ( "Case matching" ,                                                                 $X_L_TY_NO ) ;
   $a_L[ 15 ]  = &sITx ( "Match any criteria" ,                                                            $X_L_TY_NO ) ;
   $a_L[ 18 ]  = &sITx ( "Search" ,                                                                        $X_L_TY_NO ) ;
   $a_L[ 19 ]  = &sITx ( "Count" ,                                                                         $X_L_TY_NO ) ;
   $a_L[ 20 ]  = &sITx ( "Clear" ,                                                                         $X_L_TY_NO ) ;
   $a_L[ 21 ]  = &sITx ( "Reset" ,                                                                         $X_L_TY_NO ) ;
   $a_L[ 22 ]  = &sITx ( "Return to menu" ,                                                                $X_L_TY_NO ) ;
   $a_L[ 23 ]  = &sITx ( "Retrieve selected user" ,                                                        $X_L_TY_NO ) ;
   $a_L[ 33 ]  = &sITx ( "From timestamp after current timestamp" ,                                        $X_L_TY_NO ) ;
   $a_L[ 34 ]  = &sITx ( "To timestamp after current timestamp" ,                                          $X_L_TY_NO ) ;
   $a_L[ 36 ]  = &sITx ("Active records status match selection should be active , inactive or either" ,    $X_L_TY_NO ) ;
   $a_L[ 38 ]  = &sITx ( "Choose month" ,                                                                  $X_L_TY_NO ) ;
   $a_L[ 39 ]  = &sITx ( "Choose day" ,                                                                    $X_L_TY_NO ) ;
   $a_L[ 40 ]  = &sITx ( "Choose hour" ,                                                                   $X_L_TY_NO ) ;
   $a_L[ 41 ]  = &sITx ( "Choose minute" ,                                                                 $X_L_TY_NO ) ;
   $a_L[ 42 ]  = &sITx ( "Choose second" ,                                                                 $X_L_TY_NO ) ;
   $a_L[ 43 ]  = &sITx ( "Year" ,                                                                          $X_L_TY_NO ) ;
   $a_L[ 44 ]  = &sITx ( "Month" ,                                                                         $X_L_TY_NO ) ;
   $a_L[ 45 ]  = &sITx ( "Day" ,                                                                           $X_L_TY_NO ) ;
   $a_L[ 46 ]  = &sITx ( "Hour" ,                                                                          $X_L_TY_NO ) ;
   $a_L[ 47 ]  = &sITx ( "Minute" ,                                                                        $X_L_TY_NO ) ;
   $a_L[ 48 ]  = &sITx ( "Second" ,                                                                        $X_L_TY_NO ) ;
   $a_L[ 53 ]  = &sITx ( "Non digits removed from begin applicable timestamp" ,                            $X_L_TY_NO ) ;
   $a_L[ 54 ]  = &sITx ( "Non digits removed from end applicable timestamp" ,                              $X_L_TY_NO ) ;
   $a_L[ 57 ]  = &sITx ( "From year corrected to" ,                                                        $X_L_TY_NO ) ;
   $a_L[ 58 ]  = &sITx ( "To year corrected to" ,                                                          $X_L_TY_NO ) ;
   $a_L[ 59 ]  = &sITx ( "From day changed to" ,                                                           $X_L_TY_NO ) ;
   $a_L[ 60 ]  = &sITx ( "To day changed to" ,                                                             $X_L_TY_NO ) ;
   $a_L[ 61 ]  = &sITx ( "Year needed in from timestamp if month provided" ,                               $X_L_TY_NO ) ;
   $a_L[ 62 ]  = &sITx ( "Year and month needed in from timestamp if day provided" ,                       $X_L_TY_NO ) ;
   $a_L[ 63 ]  = &sITx ( "Year, month and day needed in from timestamp if hour provided" ,                 $X_L_TY_NO ) ;
   $a_L[ 64 ]  = &sITx ( "Year, month, day and hour needed in from timestamp if minute provided" ,         $X_L_TY_NO ) ;
   $a_L[ 65 ]  = &sITx ( "Year, month, day, hour and minute needed in from timestamp if second provided" , $X_L_TY_NO ) ;
   $a_L[ 66 ]  = &sITx ( "Year needed in to timestamp if month provided" ,                                 $X_L_TY_NO ) ;
   $a_L[ 67 ]  = &sITx ( "Year and month needed in to timestamp if day provided" ,                         $X_L_TY_NO ) ;
   $a_L[ 68 ]  = &sITx ( "Year, month and day needed in to timestamp if hour provided" ,                   $X_L_TY_NO ) ;
   $a_L[ 69 ]  = &sITx ( "Year, month, day and hour needed in to timestamp if minute provided" ,           $X_L_TY_NO ) ;
   $a_L[ 70 ]  = &sITx ( "Year, month, day, hour and minute needed in to timestamp if second provided" ,   $X_L_TY_NO ) ;
   $a_L[ 71 ]  = &sITx ( "From timestamp is after to timestamp" ,                                          $X_L_TY_NO ) ;
   $a_L[ 73 ]  = &sITx ( "No records found" ,                                                              $X_L_TY_NO ) ;    #
   $a_L[ 74 ]  = &sITx ( "Number of records found" ,                                                       $X_L_TY_NO ) ;
   $a_L[ 105 ] = &sITx ( "End time from timestamp earlier than start time from timestamp" ,                $X_L_TY_NO ) ;
   $a_L[ 106 ] = &sITx ( "End time to timestamp earlier than start time to timestamp" ,                    $X_L_TY_NO ) ;
   $a_L[ 113 ] = &sITx ( "TCP/IP address cleaned" ,                                                        $X_L_TY_NO ) ;
} ## end sub sInitLabels
##########################################################################
# End of sInitLabels                                                     #
##########################################################################

sub sPrintForm {

   # Subroutine for print form

   my @apf_site_nm = () ;                                       # Array of site number
   my %hpf_site_nm = () ;                                       # Hash of site number

   my $vpf_sql =
       "select xhd_sno , xhd_sno ||"
     . K_SPACE
     . K_SNG_APO
     . K_SPACE . "-"
     . K_SPACE
     . K_SNG_APO
     . K_SPACE . "||"
     . K_SPACE
     . " nvl ( "
     . K_SPACE . "( "
     . K_SPACE
     . "select"
     . K_SPACE
     . "sld_sg_tsl_u"
     . K_SPACE
     . "from  ilclpweb . T_MSY_SLD"
     . K_SPACE
     . "where sld_sg_u  = spm_nm_u"
     . K_SPACE
     . "and sld_l_ty_no ="
     . K_SPACE
     . $X_L_TY_NO
     . K_SPACE
     . "and sld_f_a ="
     . K_SPACE
     . K_SNG_APO . "Y"
     . K_SNG_APO
     . K_SPACE . " ) ,"
     . K_SPACE
     . "spm_nm_u ) , spm_nm_u"
     . K_SPACE . "from"
     . K_SPACE
     . "ilclp . T_XSM_XHD ,"
     . K_SPACE
     . "ilclp . T_MPR_SPM"
     . K_SPACE
     . "where xhd_gr_u   ="
     . K_SPACE
     . K_SNG_APO . "TEST1"
     . K_SNG_APO
     . K_SPACE
     . "and   xhd_sch    ="
     . K_SPACE
     . K_SNG_APO . "ILCLP"
     . K_SNG_APO
     . K_SPACE
     . "and   xhd_t_v_nm ="
     . K_SPACE
     . K_SNG_APO
     . "T_MPR_SPM"
     . K_SNG_APO
     . K_SPACE
     . "and   xhd_sno    = spm_sno"
     . K_SPACE
     . "and   xhd_f_a    ="
     . K_SPACE
     . K_SNG_APO . "Y"
     . K_SNG_APO
     . K_SPACE
     . "and   spm_f_a    ="
     . K_SPACE
     . K_SNG_APO . "Y"
     . K_SNG_APO
     . K_SPACE
     . "and  ( xhd_f_v   ="
     . K_SPACE
     . K_SNG_APO . "Y"
     . K_SNG_APO
     . K_SPACE
     . "or xhd_f_xtc     ="
     . K_SPACE
     . K_SNG_APO . "Y"
     . K_SNG_APO
     . K_SPACE . " )"
     . "order by xhd_sno" ;

   my $sth1 = $dbh -> prepare ( $vpf_sql ) ;
   $sth1 -> execute ;

   push ( @apf_site_nm , '' ) ;                                 # Push empty field for label Select site number
   $hpf_site_nm{ '' } =                                         # Empty key
     &sITx ( 'Select site number' , $X_L_TY_NO ) ;              #

   while ( my @rows = $sth1 -> fetchrow_array ) {
      push ( @apf_site_nm , $rows[ 0 ] ) ;
      $hpf_site_nm{ $rows[ 0 ] } = $rows[ 1 ] ;
   }

   print $q -> start_html (
      -title =>                                                 #
        K_ILCL                                                  #
        . K_SPACE                                               #
        . K_PACKAGE                                             #
        . ':'                                                   #
        . K_PROCEDURE                                           #
        . K_SPACE                                               #
        . K_PROCEDURE_NAME . K_SPACE . $X_UR_U                  #
        . K_SPACE . $v_title_timestamp ,                        #
      -style   => $X_CSS_CC . '.css' ,                          #
      -bgcolor => $I_PSEARCHEVENCL                              #
   ) ;

   print $q -> body () ;

   print $q -> start_form (                                     #
      -name => 'main_scrn' , -method => 'post'                  #
   ) ;

   # First header ########################################################

   print $q -> start_table ( {                                  #
         -border      => 0 ,                                    #
         -width       => '100%' ,                               #
         -height      => '100%' ,                               #
         -cellpadding => 1 ,                                    #
         -cellspacing => 1                                      #
      }                                                         #
   ) ;

   print $q -> Tr (
      { -width => '100%' , -valign => 'top' } ,
      $q -> td ( {
            -width => '95%' ,                                   #
            -class => 'head' ,                                  #
            -align => 'center' ,                                #
            -style => 'Font-Size: large; Color: black '         #
         } ,
         $a_L[ 0 ]
        ) ,
      $q -> td ( {
            -width => '5%' ,
            -align => 'right' ,                                 #
         } ,
         $q -> button (
            -name  => 'action_help' ,
            -value => 'Help' ,
            -title => 'Help'
         )
      )
   ) ;

   print $q -> end_table ;

   # End First header ########################################################

   print '<br><br>' ;

   &sIBreadcrumb ( $p_h_brd_crmb ) ;                            # Bread crumb

   print '<br><br>' ;

   print                                                        # Hidden return main url
     $q -> hidden (
      -name  => 'p_h_rtn_mn_url' ,
      -value => $p_h_rtn_mn_url
     ) ;

   print                                                        # Hidden breadcrumb
     $q -> hidden (
      -name     => 'p_h_brd_crmb' ,
      -value    => $p_h_brd_crmb ,
      -override => 1
     ) ;

   print $q -> start_table ( {                                                         #
         -border      => 0 ,                                    #
         -width       => '100%' ,                               #
         -height      => '100%' ,                               #
         -cellpadding => 1 ,                                    #
         -cellspacing => 1                                      #
      }                                                         #
   ) ;

    print $q -> Tr ( {
      -width => '100%' ,
      -valign => 'top'
      } ,
      $q -> td ( {
            -width => '95%' ,                                   #
            -class => 'head' ,                                  #
            -align => 'center'
            }
      ) ,
      $q -> td ( {
            -colspan => 2 ,                                     #
            -width   => '5%' ,                                  #
            -align   => 'right'                                 #
            } ,
         $q -> button (
            -name  => 'action_create' ,
            -value => 'Create' ,
            -title => 'Create'
         )
      )
    ) ;

   print $q -> Tr ( {
         -width   => '100%' ,
         -bgcolor => &sSrhColor
      } ,
      $q -> td ( {
            -width => '25%'
         } ,
         $q -> font ( { -color => $I_PSEARCHLBLCL } , $a_L[ 1 ] )
        ) ,
      $q -> td ( {
            -colspan => 2
         } ,
         $q -> popup_menu (
            -name     => 'p_srch_site_nm' ,
            -values   => \@apf_site_nm ,
            -labels   => \%hpf_site_nm ,
            -override => 1 ,
            -default  => $p_srch_site_nm
         )
      )
   ) ;

   # Begin applicable timestamp #######################################

   print $q -> Tr (
      { -bgcolor => &sSrhColor } ,                              #
      $q -> td (
         { -align => $v_l_align } ,                             #
         '<table><tr><td align="left"><font color=\''           #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . K_EMPTY                                    #
           . '</td></tr><tr><td align="left"><font color=\''    #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 2 ]                                  # Value: Begin applicable timestamp
           . '</td></tr><tr><td align="left">'                  #
           . K_EMPTY                                            #
           . '</td></tr></table>'                               #
        ) ,                                                     #
      $q -> td ( {                                              #
           -colspan => 2                                        #
           -align => $v_l_align                                 #
           } ,                                                  #
         '<table><tr><td></td><td align="center">'              #
           . '<font color=\''                                   #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 43 ]                                 #
           . '</font></td><td align="center">'                  #
           . '<font color=\''                                   #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 44 ]                                 #
           . '</font></td>'                                     #
           . '<td align="center">' . '<font color=\''           #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 45 ]                                 #
           . '</font></td>'                                     #
                                                                #
           . '<td align="center">' . '<font color=\''           #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 46 ]                                 #
           . '</font></td>'                                     #
                                                                #
           . '<td align="center">' . '<font color=\''           #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 47 ]                                 #
           . '</font></td>'                                     #
           . '<td align="center">' . '<font color=\''           #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 48 ]                                 #
           . '</font></td>'                                     #
           . '<td></td>'                                        #
           . '</tr>'                                            #
           . '<tr><td>' . '<font color=\''                      #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 7 ]                                  # Value: From
           . '</font>' . ':' . K_SPACE                          #
           . '</td><td>'                                        #
           . $q -> textfield (                                  # Input text-field for job start Year ( FROM )
            -name      => 'p_srch_bg_frm_yy' ,                       #
            -value     => $p_srch_bg_frm_yy ,                        #
            -size      => K_MAX_YR_LN * 1.1 ,                   #
            -maxlength => K_MAX_YR_LN ,                         #
            -override  => 1                                     #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_bg_frm_mm' ,                        #
            -value    => \@as_list_mm ,                         #
            -labels   => \%h_mm_lbl ,                           #
            -default  => $p_srch_bg_frm_mm ,                         #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_bg_frm_dd' ,                        #
            -value    => \@as_list_dd ,                         #
            -labels   => \%h_dd_lbl ,                           #
            -default  => $p_srch_bg_frm_dd ,                         #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_bg_frm_hh' ,                        #
            -value    => \@as_list_hh ,                         #
            -labels   => \%h_hh_lbl ,                           #
            -default  => $p_srch_bg_frm_hh ,                         #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_bg_frm_mi' ,                        #
            -value    => \@as_list_mi ,                         #
            -labels   => \%h_mi_lbl ,                           #
            -default  => $p_srch_bg_frm_mi ,                         #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_bg_frm_ss' ,                        #
            -value    => \@as_list_ss ,                         #
            -labels   => \%h_ss_lbl ,                           #
            -default  => $p_srch_bg_frm_ss ,                         #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . (
            $p_srch_h_bg_from_dt eq K_EMPTY ? K_EMPTY :         #
              (
               '&nbsp;&nbsp;'                                   #
                 . '<font color=\''                             #
                 . $I_PMSGCL                                    #
                 . '\'>'                                        #
                 . $a_L[ 7 ] . ' : '
                 . $p_srch_h_bg_from_dt
                 . '</font>'
              )
           )                                                    #
           . '</td>' . '</tr>'                                  #
           . '<tr><td><font color=\''                           #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 8 ]                                  # Value: To
           . '</font>' . K_SPACE . ':' . K_SPACE                #
           . '</td><td>'                                        #
                                                                #
           . $q -> textfield (                                  # Input text-field for job start Year ( TO )
            -name      => 'p_srch_bg_to_yy' ,                        #
            -value     => $p_srch_bg_to_yy ,                         #
            -size      => K_MAX_YR_LN * 1.1 ,                   #
            -maxlength => K_MAX_YR_LN ,                         #
            -override  => 1                                     #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_bg_to_mm' ,                         #
            -value    => \@as_list_mm ,                         #
            -labels   => \%h_mm_lbl ,                           #
            -default  => $p_srch_bg_to_mm ,                          #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_bg_to_dd' ,                         #
            -value    => \@as_list_dd ,                         #
            -labels   => \%h_dd_lbl ,                           #
            -default  => $p_srch_bg_to_dd ,                          #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_bg_to_hh' ,                         #
            -value    => \@as_list_hh ,                         #
            -labels   => \%h_hh_lbl ,                           #
            -default  => $p_srch_bg_to_hh ,                          #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_bg_to_mi' ,                         #
            -value    => \@as_list_mi ,                         #
            -labels   => \%h_mi_lbl ,                           #
            -default  => $p_srch_bg_to_mi ,                          #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_bg_to_ss' ,                         #
            -value    => \@as_list_ss ,                         #
            -labels   => \%h_ss_lbl ,                           #
            -default  => $p_srch_bg_to_ss ,                          #
            -override => 1                                      #
           ) . '</td><td>'                                      #
           . (
            $p_srch_h_bg_to_dt eq K_EMPTY
            ? K_EMPTY
            : (
               '&nbsp;&nbsp;'                                   #
                 . '<font color=\''                             #
                 . $I_PMSGCL                                    #
                 . '\'>'                                        #
                 . $a_L[ 8 ] . ' : '
                 . $p_srch_h_bg_to_dt
                 . '</font>'
            )
           )                                                    #
           . '</td>' . '</tr>'                                  #
           . '</table>' . '<br>'                                #
           . $q -> textfield (                                  # Error message for job start date
            -name      => 'v_srch_bg_dt_err_msg' ,              #
            -value     => $v_srch_bg_dt_err_msg ,               #
            -size      => K_MAX_BG_DT_RNG_ERR_LN * 1.1 ,
            -maxlength => K_MAX_BG_DT_RNG_ERR_LN ,
            -override  => 1 ,
            -readonly  => 'true' ,
            -style     => 'color :' . K_HASH . $I_PERRORCL      #
              . '; font-style:italic ; background-color :'      #
              . ' transparent ; border: none ;'
           )
        )                                                       #
   ) ;

   # Close Begin applicable timestamp ###################################

   # End applicable timestamp #######################################

   print $q -> Tr (
      { -bgcolor => &sSrhColor } ,                              #
      $q -> td (
         { -align => $v_l_align } ,                             #
         '<table><tr><td align="left"><font color=\''           #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . K_EMPTY                                    #
           . '</td></tr><tr><td align="left"><font color=\''    #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 3 ]                                  # Value: End applicable timestamp
           . '</td></tr><tr><td align="left">'                  #
           . K_EMPTY                                            #
           . '</td></tr></table>'                               #
        ) ,                                                     #
      $q -> td ( {                                              #
            -colspan => 2                                       #
            -align => $v_l_align                                #
          } ,                                                   #
         '<table><tr><td></td><td align="center">'              #
           . '<font color=\''                                   #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 43 ]                                 #
           . '</font></td><td align="center">'                  #
           . '<font color=\''                                   #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 44 ]                                 #
           . '</font></td>'                                     #
           . '<td align="center">' . '<font color=\''           #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 45 ]                                 #
           . '</font></td>'                                     #
                                                                #
           . '<td align="center">' . '<font color=\''           #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 46 ]                                 #
           . '</font></td>'                                     #
                                                                #
           . '<td align="center">' . '<font color=\''           #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 47 ]                                 #
           . '</font></td>'                                     #
           . '<td align="center">' . '<font color=\''           #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 48 ]                                 #
           . '</font></td>'                                     #
           . '<td></td>'                                        #
           . '</tr>'                                            #
           . '<tr><td>' . '<font color=\''                      #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 7 ]                                  # Value: From
           . '</font>' . ':' . K_SPACE                          #
           . '</td><td>'                                        #
           . $q -> textfield (                                  # Input text-field for job start Year ( FROM )
            -name      => 'p_srch_ed_frm_yy' ,                       #
            -value     => $p_srch_ed_frm_yy ,                 #
            -size      => K_MAX_YR_LN * 1.1 ,                   #
            -maxlength => K_MAX_YR_LN ,                         #
            -override  => 1                                     #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_ed_frm_mm' ,                        #
            -value    => \@as_list_mm ,                         #
            -labels   => \%h_mm_lbl ,                           #
            -default  => $p_srch_ed_frm_mm ,                         #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_ed_frm_dd' ,                        #
            -value    => \@as_list_dd ,                         #
            -labels   => \%h_dd_lbl ,                           #
            -default  => $p_srch_ed_frm_dd ,                         #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_ed_frm_hh' ,                        #
            -value    => \@as_list_hh ,                         #
            -labels   => \%h_hh_lbl ,                           #
            -default  => $p_srch_ed_frm_hh ,                         #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_ed_frm_mi' ,                        #
            -value    => \@as_list_mi ,                         #
            -labels   => \%h_mi_lbl ,                           #
            -default  => $p_srch_ed_frm_mi ,                         #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_ed_frm_ss' ,                        #
            -value    => \@as_list_ss ,                         #
            -labels   => \%h_ss_lbl ,                           #
            -default  => $p_srch_ed_frm_ss ,                         #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . (
            $p_srch_h_ed_from_dt eq K_EMPTY ? K_EMPTY :         #
              (
               '&nbsp;&nbsp;'                                   #
                 . '<font color=\''                             #
                 . $I_PMSGCL                                    #
                 . '\'>'                                        #
                 . $a_L[ 7 ] . ' : '
                 . $p_srch_h_ed_from_dt
                 . '</font>'
              )
           )                                                    #
           . '</td>' . '</tr>'                                  #
           . '<tr><td><font color=\''                           #
           . $I_PSEARCHLBLCL                                    #
           . '\'>' . $a_L[ 8 ]                                  # Value: To
           . '</font>' . K_SPACE . ':' . K_SPACE                #
           . '</td><td>'                                        #
                                                                #
           . $q -> textfield (                                  # Input text-field for job start Year ( TO )
            -name      => 'p_srch_ed_to_yy' ,                        #
            -value     => $p_srch_ed_to_yy ,                         #
            -size      => K_MAX_YR_LN * 1.1 ,                   #
            -maxlength => K_MAX_YR_LN ,                         #
            -override  => 1                                     #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_ed_to_mm' ,                         #
            -value    => \@as_list_mm ,                         #
            -labels   => \%h_mm_lbl ,                           #
            -default  => $p_srch_ed_to_mm ,                          #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_ed_to_dd' ,                         #
            -value    => \@as_list_dd ,                         #
            -labels   => \%h_dd_lbl ,                           #
            -default  => $p_srch_ed_to_dd ,                          #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_ed_to_hh' ,                         #
            -value    => \@as_list_hh ,                         #
            -labels   => \%h_hh_lbl ,                           #
            -default  => $p_srch_ed_to_hh ,                          #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_ed_to_mi' ,                         #
            -value    => \@as_list_mi ,                         #
            -labels   => \%h_mi_lbl ,                           #
            -default  => $p_srch_ed_to_mi ,                          #
            -override => 1                                      #
           )                                                    #
           . '</td><td>'                                        #
           . $q -> popup_menu (                                 #
            -name     => 'p_srch_ed_to_ss' ,                         #
            -value    => \@as_list_ss ,                         #
            -labels   => \%h_ss_lbl ,                           #
            -default  => $p_srch_ed_to_ss ,                          #
            -override => 1                                      #
           ) . '</td><td>'                                      #
           . (
            $p_srch_h_ed_to_dt eq K_EMPTY
            ? K_EMPTY
            : (
               '&nbsp;&nbsp;'                                   #
                 . '<font color=\''                             #
                 . $I_PMSGCL                                    #
                 . '\'>'                                        #
                 . $a_L[ 8 ] . ' : '
                 . $p_srch_h_ed_to_dt
                 . '</font>'
            )
           )                                                    #
           . '</td>' . '</tr>'                                  #
           . '</table>' . '<br>'                                #
           . $q -> textfield (                                  # Error message for job start date
            -name      => 'v_srch_ed_dt_err_msg' ,              #
            -value     => $v_srch_ed_dt_err_msg ,               #
            -size      => K_MAX_BG_DT_RNG_ERR_LN * 1.1 ,
            -maxlength => K_MAX_BG_DT_RNG_ERR_LN ,
            -override  => 1 ,
            -readonly  => 'true' ,
            -style     => 'color :' . K_HASH . $I_PERRORCL      #
              . '; font-style:italic ; background-color :'      #
              . ' transparent ; border: none ;'
           )
        )                                                       #
   ) ;

   # Close End applicable timestamp ###################################

   print $q -> Tr ( {
         -width   => '100%' ,
         -bgcolor => &sSrhColor
      } ,
      $q -> td ( {
            -width => '25%'
         } ,
         $q -> font ( { -color => $I_PSEARCHLBLCL } , "Flag - FTP" )
        ) ,
      $q -> td (
         $q -> checkbox (
            -name     => 'p_srch_f_ftp' ,
            -checked  => $p_srch_f_ftp ,
            -override => 1 ,
            -label    => '' ,
            -title =>                                           #
              &sITx (
               "Checked for ftp" ,                              #
               $X_L_TY_NO
              )                                                 #
         )
      )
   ) ;

   print $q -> Tr ( {
         -width   => '100%' ,
         -bgcolor => &sSrhColor
      } ,
      $q -> td ( {
            -width => '25%'
         } ,
         $q -> font ( { -color => $I_PSEARCHLBLCL } , "Flag - SSL" )
        ) ,
      $q -> td ( {
            -colspan => 2
         } ,
         $q -> checkbox (
            -name     => 'p_srch_f_ssl' ,
            -checked  => $p_srch_f_ssl ,
            -override => 1 ,
            -label    => '' ,
            -title =>                                           #
              &sITx (
               "Checked for SSL" ,                              #
               $X_L_TY_NO
              )                                                 #
         )
      )
   ) ;

   print $q -> Tr ( {
         -width   => '100%' ,
         -bgcolor => &sSrhColor
      } ,
      $q -> td ( {
            -width => '25%'
         } ,
         $q -> font ( { -color => $I_PSEARCHLBLCL } , "Source TCP/IP address" )
        ) ,                                                     #
      $q -> td (                                                #
         $q -> textarea (                                       #
            -name      => 'p_srch_tcpip' ,                           #
            -value     => $p_srch_tcpip ,                            #
            -maxlength => 200 ,                                 #
            -size      => $I_PTEXTLENGTH ,                      #
            -rows      => $I_PTEXTBOXLINES ,                    #
            -columns   => $I_PTEXTLENGTH ,                      #
            -override  => 1 ,                                   #
                                                                #-onBlur    => "fn_srch_user_cln( )"                #
           )                                                    #
        ) ,                                                     #
      $q -> td (
         $q -> textfield (                                      # Error field for Resrict to procedure name
            -name      => 'v_srch_tcp_ip_err_msg' ,
            -value     => $v_srch_tcp_ip_err_msg ,
            -override  => 1 ,
            -size      => K_MAX_TCP_IP_ERR_LN * 1.1 ,
            -maxlength => K_MAX_TCP_IP_ERR_LN ,
            -readonly  => 'true' ,
            -style     => 'color :' . K_HASH . $I_PERRORCL      #
              . '; font-style:italic ; background-color :'      #
              . ' transparent ; border: none ;'
         )
      )
   ) ;                                                          #
                                                                #
   print $q -> Tr (                                             #
      { -bgcolor => &sSrhColor } ,                              #
      $q -> td ( $q -> hr ) ,                                   #
      $q -> td ( {
            -colspan => 2
         } ,
         $q -> hr
      ) ,
   ) ;

   print $q -> Tr (
      { -bgcolor => &sSrhColor } ,
      $q -> td (
         $q -> font ( { -color => $I_PSEARCHLBLCL } , $a_L[ 11 ] . '?' )
        ) ,                                                     # Active
      $q -> td (                                                #
         $q -> radio_group (                                    #
            -name     => 'p_srch_i_mtc_act' ,                        #
            -value    => [ 'Yes' , 'no' , 'either' ] ,          #
            -override => 1 ,                                    #
            -default  => $p_srch_i_mtc_act ,                         #
            -title    => 'Yes' . ':' . K_SPACE                  #
              . $a_L[ 9 ] . K_SPACE                             #
              . 'No' . ':' . K_SPACE                            #
              . $a_L[ 10 ] . K_SPACE                            #
              . 'Either' . ':' . K_SPACE                        #
              . &sITx ( 'Active or inactive' , $X_L_TY_NO )     #
           )                                                    #
      )
   ) ;

   print $q -> Tr (
      { -bgcolor => &sSrhColor } ,
      $q -> td (
         $q -> font ( { -color => $I_PSEARCHLBLCL } , $a_L[ 13 ] )
        ) ,                                                     # Exact matching
      $q -> td ( {
            -colspan => 2
         } ,                                                    #
         $q -> checkbox (                                       #
            -name     => 'p_srch_f_mtc_exa' ,                        #
            -checked  => $p_srch_f_mtc_exa ,                         #
            -override => 1 ,                                    #
            -label    => '' ,                                   #
            -title =>                                           #
              &sITx (                                           #
               "Checked for exact matching" ,                   #
               $X_L_TY_NO                                       #
              )                                                 #
         )
      )
   ) ;
   print $q -> Tr (
      { -bgcolor => &sSrhColor } ,
      $q -> td (
         $q -> font ( { -color => $I_PSEARCHLBLCL } , $a_L[ 14 ] )
        ) ,                                                     # Case matching
      $q -> td ( {
            -colspan => 2
         } ,                                                    #
         $q -> checkbox (                                       #
            -name     => 'p_srch_f_mtc_cse' ,                        #
            -checked  => $p_srch_f_mtc_cse ,                         #
            -override => 1 ,                                    #
            -label    => '' ,                                   #
            -title =>                                           #
              &sITx (                                           #
               "Checked for case matching" ,                    #
               $X_L_TY_NO                                       #
              )                                                 #
           )                                                    #
        )                                                       #
   ) ;                                                          #
                                                                #
   print $q -> Tr (                                             #
      { -bgcolor => &sSrhColor } ,                              #
      $q -> td (                                                #
         $q -> font (                                           #
            { -color => $I_PSEARCHLBLCL } ,                     #
            $a_L[ 15 ]                                          #
           )                                                    #
        ) ,                                                     # Match any criteria
      $q -> td ( {
            -colspan => 2
         } ,                                                    #
         $q -> checkbox (                                       #
            -name     => 'p_srch_f_mtc_any' ,                        #
            -checked  => $p_srch_f_mtc_any ,                         #
            -label    => '' ,                                   #
            -override => 1 ,                                    #
            -title =>                                           #
              &sITx (                                           #
               "Check to match any criteria chosen"             #
                 . ", else uncheck to select users matching"    #
                 . K_SPACE . "all criteria specified" ,         #
               $X_L_TY_NO                                       #
              )                                                 #
           )                                                    #
        )                                                       #
   ) ;                                                          #
   
   print $q -> Tr (                                             #
      { -bgcolor => &sSrhColor } ,                      #
      $q -> td (                                                #
         { -width => '25%' , -align => $v_r_align } ,           #
         K_EMPTY
        ) ,                                                     #
      $q -> td (                                                #
         { -align => $v_l_align } ,           #
         $q -> textfield (                                      # Error message for common error fot timestamp fields
            -name      => 'v_srch_job_bg_ed_err_msg' ,          #
            -value     => $v_srch_job_bg_ed_err_msg ,           #
            -size      => K_MAX_BG_ED_DT_RNG_ERR_LN * 1.1 ,     #
            -maxlength => K_MAX_BG_ED_DT_RNG_ERR_LN ,           #
            -override  => 1 ,                                   #
            -readonly  => 'true' ,                              #
            -style     => 'color :' . K_HASH . $I_PERRORCL      #
              . '; font-style:italic ; background-color :'      #
              . ' transparent ; border: none ;'                 #
           )                                                    #
        )                                                       #
   ) ;                                                          #

   print $q -> end_table ;

   # Submit buttons ###################################################

   print ( p ) ;

   print $q -> submit (                                         #
      -name  => 'p_srch_acn_src' ,                              #
      -value => $a_L[ 18 ] ,                                    #
      -title =>                                                 #
        &sITx ( "Click to search records" , $X_L_TY_NO )        #
                                                                #
      #-onClick => 'javascript:return fn_srch_sub_js( )'        #
   ) ;                                                          #

   print ( ' &nbsp;&nbsp;&nbsp;&nbsp; ' ) ;                     #

   print $q -> submit (                                         #
      -name  => 'p_srch_acn_cnt' ,                              #
      -value => $a_L[ 19 ] ,                                    #
      -title =>                                                 #
        &sITx (                                                 #
         "Click to get count of records" ,                      #
         $X_L_TY_NO                                             #
        )                                                       #
                                                                #-onClick => 'javascript:return fn_srch_sub_js( )'         #
   ) ;                                                          #

   print ( ' &nbsp;&nbsp;&nbsp;&nbsp; ' ) ;                     #

   print $q -> submit (                                         #
      -name  => 'p_srch_acn_clr' ,                              #
      -value => $a_L[ 20 ] ,                                    #
                                                                #-onClick => 'javascript:return fn_js_clear( this )' ,      #
      -title =>                                                 #
        &sITx ( "Click to clear form" , $X_L_TY_NO )            #
   ) ;                                                          #

   print ( ' &nbsp;&nbsp;&nbsp;&nbsp; ' ) ;                     #
                                                                #
   print $q -> reset (                                          #
      -name  => 'p_act_reset' ,                                 #
      -value => $a_L[ 21 ]                                      #
   ) ;                                                          #

   print ( ' &nbsp;&nbsp;&nbsp;&nbsp; ' ) ;                     #

   print $q -> submit (                                         #
      -name  => 'action_cancel' ,                               #
      -value => $a_L[ 22 ] ,                                    #
      -title =>                                                 #
        &sITx ( "Click to return to menu" , $X_L_TY_NO )        #
   ) ;

   # Close Submit buttons #############################################

   #print ( p ) ;
   #print $vgc_sql ;

   #print "@a_srch_execute_values" ;

   #print $v_bg_frm_ts ;
   #print ( p ) ;
   #print $v_bg_to_ts ;

   print ( p ) ;

   print $q -> font ( { -color => $I_PSEARCHLBLCL } , $v_message ) ;

   print ( p ) ;

   print $q -> end_html ;
} ## end sub sPrintForm
##########################################################################
# End of subroutine sPrintForm                                           #
##########################################################################

sub sCheckInput {

   # Check input validation

   # Check indicator for Active  or inactive or either choice
   if (   $p_srch_i_mtc_act ne K_I_YES
      and $p_srch_i_mtc_act ne K_I_NO
      and $p_srch_i_mtc_act ne 'either' ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_message .= $a_L[ 36 ] ;                                # Value: Active records status match selection should be Active, Inactive or Either.
   }

   # STAGE 1 ##########################################################

   # Remove non digits from begin year
   if (
      $p_srch_bg_frm_yy =~ /\D/g or                                  # Match non digits year from job begin time ( FROM )
      $p_srch_bg_to_yy =~ /\D/g                                      # Match non digits from job begin time ( TO )
     ) {
      $p_srch_bg_frm_yy =~ s/\D//g ;                                 # Remove non-digits
      $p_srch_bg_to_yy =~ s/\D//g ;                                  # Remove non-digits
      $v_srch_bg_dt_err_msg = $a_L[ 53 ] ;                      # Value: Non digits removed from begin applicable timestamp
      $v_srch_f_err         = K_YES ;
   } ## end if ( $p_srch_bg_frm_yy =~ /\D/g...)

   # Remove non digits from end year
   if (
      $p_srch_ed_frm_yy =~ /\D/g or                                  # Match non digits from job end time ( FROM )
      $p_srch_ed_to_yy =~ /\D/g                                      # Match non digits from job end time ( TO )
     ) {
      $p_srch_ed_frm_yy =~ s/\D//g ;                                 # Remove non-digits
      $p_srch_ed_to_yy =~ s/\D//g ;                                  # Remove non-digits
      $v_srch_ed_dt_err_msg = $a_L[ 54 ] ;                      # Value: Non digits removed from end applicable timestamp
      $v_srch_f_err         = K_YES ;
   } ## end if ( $p_srch_ed_frm_yy =~ /\D/g...)

   if (
      $p_srch_tcpip =~ /[^[:print:]]/  or                            # Match non printable characters
      $p_srch_tcpip =~ /^\s|\s$|\s{2}/ or                            # Match multiple spaces or leading or trailing space
      $p_srch_tcpip =~ /[\x{22}\%\&\x{27}\*\+\.\/\;\<\>\?\\\`\|]/    # Match special characters
     ) {
      $p_srch_tcpip =~ s/[^[:print:]]/ / ;                           # Replace non printable to space
      $p_srch_tcpip =~ s/\s+/ /g ;                                   # Crush multiple spaces to single space
      $p_srch_tcpip =~ s/^\s// ;                                     # No leading space
      $p_srch_tcpip =~ s/\s$// ;                                     # No trailing space
      $p_srch_tcpip =~ s/[\x{22}\%\&\x{27}\*\+\.\/\;\<\>\?\\\`\|]+/\_/g ;    # Spacial charater replaced by _
      $v_srch_tcp_ip_err_msg = $a_L[ 113 ] ;                    # Value: TCP/IP address cleaned
                                                                #$v_srch_f_err          = K_YES ;
   } ## end if ( $p_srch_tcpip =~ /[^[:print:]]/...)

   if ( $v_srch_f_err eq K_YES ) {                              # STAGE 1 - Get out if error in main screen
      return ;
   }

   # STAGE 2 ##########################################################

   # Begin applicable timestamp FROM year ( NON EMPTY , GREATER THAN ZERO , LESS THAN 1904 )
   if (
      $p_srch_bg_frm_yy ne K_EMPTY and                               # From year not equal to empty
      $p_srch_bg_frm_yy >= K_ZERO  and                               # Greater than equal to zero
      $p_srch_bg_frm_yy < 1904                                       # Less than 1904
     ) {
      $p_srch_bg_frm_yy = &sYearCorrection ( $p_srch_bg_frm_yy ) ;        # Subroutine to correct year
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 57 ] . ' : ' . $p_srch_bg_frm_yy ;    # Value: From year corrected to
      $v_srch_f_err = K_YES ;
   } ## end if ( $p_srch_bg_frm_yy ne K_EMPTY...)

   # Begin applicable timestamp TO year ( NON EMPTY , GREATER THAN ZERO , LESS THAN 1904 )
   if (
      $p_srch_bg_to_yy ne K_EMPTY and                                # To year not equal to empty
      $p_srch_bg_to_yy >= K_ZERO  and                                # Greater than equal to zero
      $p_srch_bg_to_yy < 1904                                        # Less than 1904
     ) {
      $p_srch_bg_to_yy = &sYearCorrection ( $p_srch_bg_to_yy ) ;          # Subroutine to correct year
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 58 ] . ' : ' . $p_srch_bg_to_yy ;     # Value: To year corrected to
      $v_srch_f_err = K_YES ;
   } ## end if ( $p_srch_bg_to_yy ne K_EMPTY...)

   # End applicable timestamp FROM year ( NON EMPTY , GREATER THAN ZERO , LESS THAN 1904 )
   if (
      $p_srch_ed_frm_yy ne K_EMPTY and                               # From year not equal to empty
      $p_srch_ed_frm_yy >= K_ZERO  and                               # Greater than equal to zero
      $p_srch_ed_frm_yy < 1904                                       # Less than 1904
     ) {
      $p_srch_ed_frm_yy = &sYearCorrection ( $p_srch_ed_frm_yy ) ;        # Subroutine to correct year
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 57 ] . ' : ' . $p_srch_ed_frm_yy ;    # Value: From year corrected to
      $v_srch_f_err = K_YES ;
   } ## end if ( $p_srch_ed_frm_yy ne K_EMPTY...)

   # End applicable timestamp TO year ( NON EMPTY , GREATER THAN ZERO , LESS THAN 1904 )
   if (
      $p_srch_ed_to_yy ne K_EMPTY and                                # To year not equal to empty
      $p_srch_ed_to_yy >= K_ZERO  and                                # Greater than equal to zero
      $p_srch_ed_to_yy < 1904                                        # Less than 1904
     ) {
      $p_srch_ed_to_yy = &sYearCorrection ( $p_srch_ed_to_yy ) ;          # Subroutine to correct year
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 58 ] . ' : ' . $p_srch_ed_to_yy ;     # Value: To year corrected to
      $v_srch_f_err = K_YES ;
   } ## end if ( $p_srch_ed_to_yy ne K_EMPTY...)

   $v_srch_bg_dt_err_msg =~ s/^\s-\s//g ;                       # Remove space hypen space
   $v_srch_ed_dt_err_msg =~ s/^\s-\s//g ;                       # Remove space hypen space

   if ( $v_srch_f_err eq K_YES ) {                              # STAGE 2 - Get out if error in main screen
      return ;
   }

   # STAGE 3 ##########################################################

   # If begin from date are greater than Last day of month i.e 31 > 28 or 29 ( FEB )
   if ( (
         $p_srch_bg_frm_dd ne K_EMPTY and                            # Date
         $p_srch_bg_frm_yy ne K_EMPTY and                            # Year
         $p_srch_bg_frm_mm ne K_EMPTY                                # Month

      )
      and                                                       #
      $p_srch_bg_frm_dd > &sLastDayOfMonth ( $p_srch_bg_frm_yy , $p_srch_bg_frm_mm )    # Get last day of selected month for job start time ( FROM )
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $p_srch_bg_frm_dd = &sLastDayOfMonth ( $p_srch_bg_frm_yy , $p_srch_bg_frm_mm ) ;
      $v_srch_bg_dt_err_msg .=                                  #
        $a_L[ 59 ] . ' : ' . $p_srch_bg_frm_dd ;                     # Value: From day changed to
   } ## end if ( ( $p_srch_bg_frm_dd ne...))

   # If begin to date are greater than Last day of month i.e 31 > 28 or 29 ( FEB )
   if ( (
         $p_srch_bg_to_dd ne K_EMPTY and                             # Date
         $p_srch_bg_to_yy ne K_EMPTY and                             # Year
         $p_srch_bg_to_mm ne K_EMPTY                                 # Month
      )
      and                                                       #
      $p_srch_bg_to_dd >                                             # Get last day of selected month for job start time ( TO )
      &sLastDayOfMonth ( $p_srch_bg_to_yy , $p_srch_bg_to_mm )
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $p_srch_bg_to_dd =                                             #
        &sLastDayOfMonth ( $p_srch_bg_to_yy , $p_srch_bg_to_mm ) ;        #
      $v_srch_bg_dt_err_msg .=                                  # Value: To day changed to
        ' - ' . $a_L[ 60 ] . ' : ' . $p_srch_bg_to_dd ;
   } ## end if ( ( $p_srch_bg_to_dd ne ...))

   # If End from date are greater than Last day of month i.e 31 > 28 or 29 ( FEB )
   if ( (
         $p_srch_ed_frm_dd ne K_EMPTY and                            #
         $p_srch_ed_frm_yy ne K_EMPTY and                            #
         $p_srch_ed_frm_mm ne K_EMPTY                                #
      )                                                         #
      and                                                       #
      $p_srch_ed_frm_dd >                                            # Get last day of selected month for job end time ( FROM )
      &sLastDayOfMonth ( $p_srch_ed_frm_yy , $p_srch_ed_frm_mm )          #
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   #
      $p_srch_ed_frm_dd =                                            #
        &sLastDayOfMonth ( $p_srch_ed_frm_yy , $p_srch_ed_frm_mm ) ;      #
      $v_srch_ed_dt_err_msg .=                                  #
        $a_L[ 59 ] . ' : '                                      # Value: Day changed to
        . $p_srch_ed_frm_dd ;
   }

   # If End to date are greater than Last day of month i.e 31 > 28 or 29 ( FEB )
   if ( (
         $p_srch_ed_to_dd ne K_EMPTY and                             #
         $p_srch_ed_to_yy ne K_EMPTY and                             #
         $p_srch_ed_to_mm ne K_EMPTY                                 #
      )                                                         #
      and                                                       #
      $p_srch_ed_to_dd >                                             # Get last day of selected month for job end time ( TO )
      &sLastDayOfMonth ( $p_srch_ed_to_yy , $p_srch_ed_to_mm )            #
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $p_srch_ed_to_dd =                                             #
        &sLastDayOfMonth ( $p_srch_ed_to_yy , $p_srch_ed_to_mm ) ;        #
      $v_srch_ed_dt_err_msg .=                                  #
        ' - ' . $a_L[ 60 ] . ' : '                              # Value: To day changed to
        . $p_srch_ed_to_dd ;
   }

   $v_srch_bg_dt_err_msg =~ s/^\s-\s//g ;                       # Remove space hypen space
   $v_srch_ed_dt_err_msg =~ s/^\s-\s//g ;                       # Remove space hypen space

   if ( $v_srch_f_err eq K_YES ) {                              # STAGE 3 - Get out if error in main screen
      return ;
   }

   # STAGE 4 ##########################################################

   # Get out if date combination incorrect

   # Begin applicable timestamp( FROM ) - year field are empty and month field are not empty
   if (
      $p_srch_bg_frm_yy eq K_EMPTY and                               # Year field are empty
      $p_srch_bg_frm_mm ne K_EMPTY                                   # Month field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= $a_L[ 61 ] ;                     # Value: Year needed in from timestamp if month provided
      return ;
   } ## end if ( $p_srch_bg_frm_yy eq K_EMPTY...)

   # Begin applicable timestamp( FROM ) - year or month fields are empty and day field are not empty
   if ( (
         $p_srch_bg_frm_yy eq K_EMPTY or                             # Year field are empty
         $p_srch_bg_frm_mm eq K_EMPTY                                # Month field are empty
      )
      and                                                       #
      $p_srch_bg_frm_dd ne K_EMPTY                                   # Day field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= $a_L[ 62 ] ;                     # Value: Year and month needed in from timestamp if day provided
      return ;
   } ## end if ( ( $p_srch_bg_frm_yy eq...))

   # Begin applicable timestamp( FROM ) - year , month or day fields are empty and hour field are not empty
   if ( (
         $p_srch_bg_frm_yy eq K_EMPTY or                             # Year field are empty
         $p_srch_bg_frm_mm eq K_EMPTY or                             # Month field are empty
         $p_srch_bg_frm_dd eq K_EMPTY                                # Day field are empty
      )
      and                                                       #
      $p_srch_bg_frm_hh ne K_EMPTY                                   # Hour field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= $a_L[ 63 ] ;                     # Value: Year, month and day needed in from timestamp if hour provided
      return ;
   } ## end if ( ( $p_srch_bg_frm_yy eq...))

   # Begin applicable timestamp( FROM ) - year , month , day , hour fields are empty and minute are not empty
   if ( (
         $p_srch_bg_frm_yy eq K_EMPTY or                             # Year field are empty
         $p_srch_bg_frm_mm eq K_EMPTY or                             # Month field are empty
         $p_srch_bg_frm_dd eq K_EMPTY or                             # Day field are empty
         $p_srch_bg_frm_hh eq K_EMPTY                                # Hour field are empty
      )
      and                                                       #
      $p_srch_bg_frm_mi ne K_EMPTY                                   # Minute field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= $a_L[ 64 ] ;                     # Value: Year, month, day and hour needed in from timestamp if minute provided
      return ;
   } ## end if ( ( $p_srch_bg_frm_yy eq...))

   # Begin applicable timestamp( FROM ) - year , month , day , hour , minute fields are empty and seconds are not empty
   if ( (                                                         #
         $p_srch_bg_frm_yy eq K_EMPTY or                             # Year field are empty
         $p_srch_bg_frm_mm eq K_EMPTY or                             # Month field are empty
         $p_srch_bg_frm_dd eq K_EMPTY or                             # Day field are empty
         $p_srch_bg_frm_hh eq K_EMPTY or                             # Hour field are empty
         $p_srch_bg_frm_mi eq K_EMPTY                                # Minute field are empty
      )
      and                                                       #
      $p_srch_bg_frm_ss ne K_EMPTY                                   # Seconds field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= $a_L[ 65 ] ;                     # Value: Year, month, day, hour and minute needed in from timestamp if second provided
      return ;
   } ## end if ( (  $p_srch_bg_frm_yy eq...))

   # Begin applicable timestamp( TO ) - year field are empty and month field are not empty
   if (
      $p_srch_bg_to_yy eq K_EMPTY and                                # Year field are empty
      $p_srch_bg_to_mm ne K_EMPTY                                    # Month field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= $a_L[ 66 ] ;                     # Value: Year needed in to timestamp if month provided
      return ;
   } ## end if ( $p_srch_bg_to_yy eq K_EMPTY...)

   # Begin applicable timestamp( TO ) - year or month fields are empty and day field are not empty
   if ( (
         $p_srch_bg_to_yy eq K_EMPTY or                              # Year field are empty
         $p_srch_bg_to_mm eq K_EMPTY                                 # Month field are empty
      )
      and                                                       #
      $p_srch_bg_to_dd ne K_EMPTY                                    # Day field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= $a_L[ 67 ] ;                     # Value: Year and month needed in to timestamp if day provided
      return ;
   } ## end if ( ( $p_srch_bg_to_yy eq ...))

   # Begin applicable timestamp( TO ) - year , month or day fields are empty and hour field are not empty
   if ( (
         $p_srch_bg_to_yy eq K_EMPTY or                              # Year field are empty
         $p_srch_bg_to_mm eq K_EMPTY or                              # Month field are empty
         $p_srch_bg_to_dd eq K_EMPTY                                 # Day field are empty
      )
      and                                                       #
      $p_srch_bg_to_hh ne K_EMPTY                                    # Hour field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= $a_L[ 68 ] ;                     # Value: Year, month and day needed in to timestamp if hour provided
      return ;
   } ## end if ( ( $p_srch_bg_to_yy eq ...))

   # Begin applicable timestamp( TO ) - year , month , day , hour fields are empty and minute are not empty
   if ( (
         $p_srch_bg_to_yy eq K_EMPTY or                              # Year field are empty
         $p_srch_bg_to_mm eq K_EMPTY or                              # Month field are empty
         $p_srch_bg_to_dd eq K_EMPTY or                              # Day field are empty
         $p_srch_bg_to_hh eq K_EMPTY                                 # Hour field are empty
      )
      and                                                       #
      $p_srch_bg_to_mi ne K_EMPTY                                    # Minute field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= $a_L[ 69 ] ;                     # Value: Year, month, day and hour needed in to timestamp if minute provided
      return ;
   } ## end if ( ( $p_srch_bg_to_yy eq ...))

   # Begin applicable timestamp( TO ) - year , month , day , hour , minute fields are empty and seconds are not empty
   if ( (
         $p_srch_bg_to_yy eq K_EMPTY or                              # Year field are empty
         $p_srch_bg_to_mm eq K_EMPTY or                              # Month field are empty
         $p_srch_bg_to_dd eq K_EMPTY or                              # Day field are empty
         $p_srch_bg_to_hh eq K_EMPTY or                              # Hour field are empty
         $p_srch_bg_to_mi eq K_EMPTY                                 # Minute field are empty
      )
      and                                                       #
      $p_srch_bg_to_ss ne K_EMPTY                                    # Seconds field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= $a_L[ 70 ] ;                     # Value: Year, month, day, hour and minute needed in to timestamp if second provided
      return ;
   } ## end if ( ( $p_srch_bg_to_yy eq ...))

   # End applicable timestamp( FROM ) - year field are empty and month field are not empty
   if (
        $p_srch_ed_frm_yy eq K_EMPTY and                             # Year field are empty
        $p_srch_ed_frm_mm ne K_EMPTY                                 # Month field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= $a_L[ 61 ] ;                     # Value: Year needed in from timestamp if month provided
      return ;
   } ## end if ( $p_srch_ed_frm_mm ne K_EMPTY )

   # End applicable timestamp( FROM ) - year or month fields are empty and day field are not empty
   if ( (
         $p_srch_ed_frm_yy eq K_EMPTY or                             # Year field are empty
         $p_srch_ed_frm_mm eq K_EMPTY                                # Month field are empty
      )
      and                                                       #
      $p_srch_ed_frm_dd ne K_EMPTY                                   # Day field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= $a_L[ 62 ] ;                     # Value: Year and month needed in from timestamp if day provided
      return ;
   } ## end if ( ( $p_srch_ed_frm_yy eq...))

   # End applicable timestamp( FROM ) - year , month or day fields are empty and hour field are not empty
   if ( (
         $p_srch_ed_frm_yy eq K_EMPTY or                             # Year field are empty
         $p_srch_ed_frm_mm eq K_EMPTY or                             # Month field are empty
         $p_srch_ed_frm_dd eq K_EMPTY                                # Day field are empty
      )
      and                                                       #
      $p_srch_ed_frm_hh ne K_EMPTY                                   # Hour field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= $a_L[ 63 ] ;                     # Value: Year, month and day needed in from timestamp if hour provided
      return ;
   } ## end if ( ( $p_srch_ed_frm_yy eq...))

   # End applicable timestamp( FROM ) - year , month , day , hour fields are empty and minute are not empty
   if ( (
         $p_srch_ed_frm_yy eq K_EMPTY or                             # Year field are empty
         $p_srch_ed_frm_mm eq K_EMPTY or                             # Month field are empty
         $p_srch_ed_frm_dd eq K_EMPTY or                             # Day field are empty
         $p_srch_ed_frm_hh eq K_EMPTY                                # Hour field are empty
      )
      and                                                       #
      $p_srch_ed_frm_mi ne K_EMPTY                                   # Minute field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= $a_L[ 64 ] ;                     # Value: Year, month, day and hour needed in from timestamp if minute provided
      return ;
   } ## end if ( ( $p_srch_ed_frm_yy eq...))

   # End applicable timestamp( FROM ) - year , month , day , hour , minute fields are empty and seconds are not empty
   if ( (
         $p_srch_ed_frm_yy eq K_EMPTY or                             # Year field are empty
         $p_srch_ed_frm_mm eq K_EMPTY or                             # Month field are empty
         $p_srch_ed_frm_dd eq K_EMPTY or                             # Day field are empty
         $p_srch_ed_frm_hh eq K_EMPTY or                             # Hour field are empty
         $p_srch_ed_frm_mi eq K_EMPTY                                # Minute field are empty
      )
      and                                                       #
      $p_srch_ed_frm_ss ne K_EMPTY                                   # Seconds field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= $a_L[ 65 ] ;                     # Value: Year, month, day, hour and minute needed in from timestamp if second provided
      return ;
   } ## end if ( ( $p_srch_ed_frm_yy eq...))

   # End applicable timestamp( TO ) - year field are empty and month field are not empty
   if (
      $p_srch_ed_to_yy eq K_EMPTY and                                # Year field are empty
      $p_srch_ed_to_mm ne K_EMPTY                                    # Month field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= $a_L[ 66 ] ;                     # Value: Year needed in to timestamp if month provided
      return ;
   } ## end if ( $p_srch_ed_to_yy eq K_EMPTY...)

   # End applicable timestamp( TO ) - year or month fields are empty and day field are not empty
   if ( (
         $p_srch_ed_to_yy eq K_EMPTY or                              # Year field are empty
         $p_srch_ed_to_mm eq K_EMPTY                                 # Month field are empty
      )
      and                                                       #
      $p_srch_ed_to_dd ne K_EMPTY                                    # Day field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= $a_L[ 67 ] ;                     # Value: Year and month needed in to timestamp if day provided
      return ;
   } ## end if ( ( $p_srch_ed_to_yy eq ...))

   # End applicable timestamp( TO ) - year , month or day fields are empty and hour field are not empty
   if ( (
         $p_srch_ed_to_yy eq K_EMPTY or                              # Year field are empty
         $p_srch_ed_to_mm eq K_EMPTY or                              # Month field are empty
         $p_srch_ed_to_dd eq K_EMPTY                                 # Day field are empty
      )
      and                                                       #
      $p_srch_ed_to_hh ne K_EMPTY                                    # Hour field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= $a_L[ 68 ] ;                     # Value: Year, month and day needed in to timestamp if hour provided
      return ;
   } ## end if ( ( $p_srch_ed_to_yy eq ...))

   # End applicable timestamp( TO ) - year , month , day , hour fields are empty and minute are not empty
   if ( (
         $p_srch_ed_to_yy eq K_EMPTY or                              # Year field are empty
         $p_srch_ed_to_mm eq K_EMPTY or                              # Month field are empty
         $p_srch_ed_to_dd eq K_EMPTY or                              # Day field are empty
         $p_srch_ed_to_hh eq K_EMPTY                                 # Hour field are empty
      )
      and                                                       #
      $p_srch_ed_to_mi ne K_EMPTY                                    # Minute field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= $a_L[ 69 ] ;                     # Value: Year, month, day and hour needed in to timestamp if minute provided
      return ;
   } ## end if ( ( $p_srch_ed_to_yy eq ...))

   # End applicable timestamp( TO ) - year , month , day , hour , minute fields are empty and seconds are not empty
   if ( (
         $p_srch_ed_to_yy eq K_EMPTY or                              # Year field are empty
         $p_srch_ed_to_mm eq K_EMPTY or                              # Month field are empty
         $p_srch_ed_to_dd eq K_EMPTY or                              # Day field are empty
         $p_srch_ed_to_hh eq K_EMPTY or                              # Hour field are empty
         $p_srch_ed_to_mi eq K_EMPTY                                 # Minute field are empty
      )
      and                                                       #
      $p_srch_ed_to_ss ne K_EMPTY                                    # Seconds field are not empty
     ) {                                                        #
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= $a_L[ 70 ] ;                     # Value: Year, month, day, hour and minute needed in to timestamp if second provided
      return ;
   } ## end if ( ( $p_srch_ed_to_yy eq ...))

   # Begin applicable timestamp - Store job begin time in variables ( FROM )
   $v_srch_bg_from_yy = $p_srch_bg_frm_yy ;
   $v_srch_bg_from_mm = $p_srch_bg_frm_mm ;
   $v_srch_bg_from_dd = $p_srch_bg_frm_dd ;
   $v_srch_bg_from_hh = $p_srch_bg_frm_hh ;
   $v_srch_bg_from_mi = $p_srch_bg_frm_mi ;
   $v_srch_bg_from_ss = $p_srch_bg_frm_ss ;

   # Begin applicable timestamp - Store job begin time in variables ( TO )
   $v_srch_bg_to_yy = $p_srch_bg_to_yy ;
   $v_srch_bg_to_mm = $p_srch_bg_to_mm ;
   $v_srch_bg_to_dd = $p_srch_bg_to_dd ;
   $v_srch_bg_to_hh = $p_srch_bg_to_hh ;
   $v_srch_bg_to_mi = $p_srch_bg_to_mi ;
   $v_srch_bg_to_ss = $p_srch_bg_to_ss ;

   # End applicable timestamp - Store job end time in variables ( FROM )
   $v_srch_ed_from_yy = $p_srch_ed_frm_yy ;
   $v_srch_ed_from_mm = $p_srch_ed_frm_mm ;
   $v_srch_ed_from_dd = $p_srch_ed_frm_dd ;
   $v_srch_ed_from_hh = $p_srch_ed_frm_hh ;
   $v_srch_ed_from_mi = $p_srch_ed_frm_mi ;
   $v_srch_ed_from_ss = $p_srch_ed_frm_ss ;

   # End applicable timestamp - Store job end time in variables ( TO )
   $v_srch_ed_to_yy = $p_srch_ed_to_yy ;
   $v_srch_ed_to_mm = $p_srch_ed_to_mm ;
   $v_srch_ed_to_dd = $p_srch_ed_to_dd ;
   $v_srch_ed_to_hh = $p_srch_ed_to_hh ;
   $v_srch_ed_to_mi = $p_srch_ed_to_mi ;
   $v_srch_ed_to_ss = $p_srch_ed_to_ss ;

   if ( $v_srch_f_err eq K_YES ) {                              # STAGE 4 - Get out if error in main screen
      return ;
   }

   # STAGE 5 - Display error message if date time after current date time

   # Begin appicable timestamp( FROM ) - Year greater than current year
   if (
      $v_srch_bg_from_yy ne K_EMPTY and                         # Check From timestamp after current timestamp if only year provided
      $v_srch_bg_from_mm eq K_EMPTY and                         #
      $v_srch_bg_from_dd eq K_EMPTY and                         #
      $v_srch_bg_from_hh eq K_EMPTY and                         #
      $v_srch_bg_from_mi eq K_EMPTY and                         #
      $v_srch_bg_from_ss eq K_EMPTY
      and ( $v_srch_bg_from_yy > $v_now_yy )                    # Year greater than current year
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 33 ] ;             # Value: From timestamp after current timestamp
   } ## end if ( $v_srch_bg_from_yy...)

   # Begin appicable timestamp( FROM ) - Timestamp ( Year and month ) greater than current timestamp ( year and month )
   if (
      $v_srch_bg_from_yy ne K_EMPTY and                         # Check From timestamp after current timestamp if only year and month provided
      $v_srch_bg_from_mm ne K_EMPTY and                         #
      $v_srch_bg_from_dd eq K_EMPTY and                         #
      $v_srch_bg_from_hh eq K_EMPTY and                         #
      $v_srch_bg_from_mi eq K_EMPTY and                         #
      $v_srch_bg_from_ss eq K_EMPTY
      and (
         ( $v_srch_bg_from_yy * 100 + $v_srch_bg_from_mm )      #
         >                                                      #
         ( $v_now_yy * 100 + $v_now_mm )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 33 ] ;             # Value: From timestamp after current timestamp
   } ## end if ( $v_srch_bg_from_yy...)

   # Begin appicable timestamp( FROM ) - Timestamp ( Year and month and day ) greater than current timestamp ( year and month and day )
   if (
      $v_srch_bg_from_yy ne K_EMPTY and                         # Check From timestamp after current timestamp if only year, month and day provided
      $v_srch_bg_from_mm ne K_EMPTY and                         #
      $v_srch_bg_from_dd ne K_EMPTY and                         #
      $v_srch_bg_from_hh eq K_EMPTY and                         #
      $v_srch_bg_from_mi eq K_EMPTY and                         #
      $v_srch_bg_from_ss eq K_EMPTY
      and ( (
            $v_srch_bg_from_yy * 10000 +                        # Year multiply by 10000
            $v_srch_bg_from_mm * 100 +                          # Month multiply by 100
            $v_srch_bg_from_dd                                  #
         ) >                                                    #
         (
            $v_now_yy * 10000 +                                 # Year multiply by 10000
              $v_now_mm * 100 +                                 # Month multiply by 100
              $v_now_dd                                         #
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 33 ] ;             # Value: From timestamp after current timestamp
   } ## end if ( $v_srch_bg_from_yy...)

   # Begin appicable timestamp( FROM ) - Timestamp ( Year and month and day and hour ) greater than current timestamp ( year and month and day and hour )
   if (
      $v_srch_bg_from_yy ne K_EMPTY and                         # Check From timestamp after current timestamp if only year, month, day and hour provided
      $v_srch_bg_from_mm ne K_EMPTY and                         #
      $v_srch_bg_from_dd ne K_EMPTY and                         #
      $v_srch_bg_from_hh ne K_EMPTY and                         #
      $v_srch_bg_from_mi eq K_EMPTY and                         #
      $v_srch_bg_from_ss eq K_EMPTY
      and ( (
            $v_srch_bg_from_yy * 1000000 +                      # Year multiply by 1000000
            $v_srch_bg_from_mm * 10000 +                        # Month multiply by 10000
            $v_srch_bg_from_dd * 100 +                          # Day multiply by 100
            $v_srch_bg_from_hh                                  #
         ) >                                                    #
         (
            $v_now_yy * 1000000 +                               # Year multiply by 1000000
              $v_now_mm * 10000 +                               # Month multiply by 10000
              $v_now_dd * 100 +                                 # Day multiply by 100
              $v_now_hh                                         #
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 33 ] ;             # Value: From timestamp after current timestamp
   } ## end if ( $v_srch_bg_from_yy...)

   # Begin appicable timestamp( FROM ) - Timestamp ( Year and month and day and hour and minute ) greater than current timestamp ( year and month and day and hour and minute )
   if (
      $v_srch_bg_from_yy ne K_EMPTY and                         # Check From timestamp after current timestamp if only year, month, day, hour and minute provided
      $v_srch_bg_from_mm ne K_EMPTY and                         #
      $v_srch_bg_from_dd ne K_EMPTY and                         #
      $v_srch_bg_from_hh ne K_EMPTY and                         #
      $v_srch_bg_from_mi ne K_EMPTY and                         #
      $v_srch_bg_from_ss eq K_EMPTY
      and ( (
            $v_srch_bg_from_yy * 100000000 +                    # Year multiply by 100000000
            $v_srch_bg_from_mm * 1000000 +                      # Month multiply by 1000000
            $v_srch_bg_from_dd * 10000 +                        # Day multiply by 10000
            $v_srch_bg_from_hh * 100 +                          # Hours multiply by 100
            $v_srch_bg_from_mi                                  #
         ) >                                                    #
         (
            $v_now_yy * 100000000 +                             # Year multiply by 1000000
              $v_now_mm * 1000000 +                             # Month multiply by 10000
              $v_now_dd * 10000 +                               # Day multiply by 100
              $v_now_hh * 100 +                                 # Hours multiply by 100
              $v_now_mi                                         #
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 33 ] ;             # Value: From timestamp after current timestamp
   } ## end if ( $v_srch_bg_from_yy...)

   # Begin appicable timestamp( FROM ) - Timestamp ( Year and month and day and hour and minute and seconds ) greater than current timestamp ( year and month and day and hour and minute and seconds )
   if (
      $v_srch_bg_from_yy ne K_EMPTY and                         # Check From timestamp after current timestamp if only year, month, day, hour, minute and second provided
      $v_srch_bg_from_mm ne K_EMPTY and                         #
      $v_srch_bg_from_dd ne K_EMPTY and                         #
      $v_srch_bg_from_hh ne K_EMPTY and                         #
      $v_srch_bg_from_mi ne K_EMPTY and                         #
      $v_srch_bg_from_ss ne K_EMPTY
      and ( (
            $v_srch_bg_from_yy * 10000000000 +                  # Year multiply by 10000000000
            $v_srch_bg_from_mm * 100000000 +                    # Month multiply by 100000000
            $v_srch_bg_from_dd * 1000000 +                      # Day multiply by 1000000
            $v_srch_bg_from_hh * 10000 +                        # Hours multiply by 1000000
            $v_srch_bg_from_mi * 100 +                          # Minutes multiply by 100
            $v_srch_bg_from_ss
         ) >                                                    #
         (
            $v_now_yy * 10000000000 +                           # Year multiply by 10000000000
              $v_now_mm * 100000000 +                           # Month multiply by 100000000
              $v_now_dd * 1000000 +                             # Day multiply by 1000000
              $v_now_hh * 10000 +                               # Hours multiply by 1000000
              $v_now_mi * 100 +                                 # Minutes multiply by 100
              $v_now_ss
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 33 ] ;             # Value: From timestamp after current timestamp
   } ## end if ( $v_srch_bg_from_yy...)

   # Begin appicable timestamp( TO ) - Year greater than current year
   if (
      $v_srch_bg_to_yy ne K_EMPTY and                           # Check to timestamp after current timestamp if only year provided
      $v_srch_bg_to_mm eq K_EMPTY and                           #
      $v_srch_bg_to_dd eq K_EMPTY and                           #
      $v_srch_bg_to_hh eq K_EMPTY and                           #
      $v_srch_bg_to_mi eq K_EMPTY and                           #
      $v_srch_bg_to_ss eq K_EMPTY
      and ( $v_srch_bg_to_yy > $v_now_yy )                      # Year greater than current year
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 34 ] ;             # Value: To timestamp after current timestamp
   } ## end if ( $v_srch_bg_to_yy ...)

   # Begin appicable timestamp( TO ) - Timestamp ( Year and month ) greater than current timestamp ( year and month )
   if (
      $v_srch_bg_to_yy ne K_EMPTY and                           # Check to timestamp after current timestamp if only year and month provided
      $v_srch_bg_to_mm ne K_EMPTY and                           #
      $v_srch_bg_to_dd eq K_EMPTY and                           #
      $v_srch_bg_to_hh eq K_EMPTY and                           #
      $v_srch_bg_to_mi eq K_EMPTY and                           #
      $v_srch_bg_to_ss eq K_EMPTY
      and (
         ( $v_srch_bg_to_yy * 100 + $v_srch_bg_to_mm )          #
         >                                                      #
         ( $v_now_yy * 100 + $v_now_mm )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 34 ] ;             # Value: To timestamp after current timestamp
   } ## end if ( $v_srch_bg_to_yy ...)

   # Begin appicable timestamp( TO ) - Timestamp ( Year and month and day ) greater than current timestamp ( year and month and day )
   if (
      $v_srch_bg_to_yy ne K_EMPTY and                           # Check to timestamp after current timestamp if only year, month and day provided
      $v_srch_bg_to_mm ne K_EMPTY and                           #
      $v_srch_bg_to_dd ne K_EMPTY and                           #
      $v_srch_bg_to_hh eq K_EMPTY and                           #
      $v_srch_bg_to_mi eq K_EMPTY and                           #
      $v_srch_bg_to_ss eq K_EMPTY
      and ( (
            $v_srch_bg_to_yy * 10000 +                          # Year multiply by 10000
            $v_srch_bg_to_mm * 100 +                            # Month multiply by 100
            $v_srch_bg_to_dd                                    #
         ) >                                                    #
         (
            $v_now_yy * 10000 +                                 # Year multiply by 10000
              $v_now_mm * 100 +                                 # Month multiply by 100
              $v_now_dd                                         #
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 34 ] ;             # Value: To timestamp after current timestamp
   } ## end if ( $v_srch_bg_to_yy ...)

   # Begin appicable timestamp( TO ) - Timestamp ( Year and month and day and hour ) greater than current timestamp ( year and month and day and hour )
   if (
      $v_srch_bg_to_yy ne K_EMPTY and                           # Check to timestamp after current timestamp if only year, month, day and hour provided
      $v_srch_bg_to_mm ne K_EMPTY and                           #
      $v_srch_bg_to_dd ne K_EMPTY and                           #
      $v_srch_bg_to_hh ne K_EMPTY and                           #
      $v_srch_bg_to_mi eq K_EMPTY and                           #
      $v_srch_bg_to_ss eq K_EMPTY
      and ( (
            $v_srch_bg_to_yy * 1000000 +                        # Year multiply by 1000000
            $v_srch_bg_to_mm * 10000 +                          # Month multiply by 10000
            $v_srch_bg_to_dd * 100 +                            # Day multiply by 100
            $v_srch_bg_to_hh                                    #
         ) >                                                    #
         (
            $v_now_yy * 1000000 +                               # Year multiply by 1000000
              $v_now_mm * 10000 +                               # Month multiply by 10000
              $v_now_dd * 100 +                                 # Day multiply by 100
              $v_now_hh                                         #
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 34 ] ;             # Value: To timestamp after current timestamp
   } ## end if ( $v_srch_bg_to_yy ...)

   # Begin appicable timestamp( TO ) - Timestamp ( Year and month and day and hour and minute ) greater than current timestamp ( year and month and day and hour and minute )
   if (
      $v_srch_bg_to_yy ne K_EMPTY and                           # Check to timestamp after current timestamp if only year, month, day, hour and minute provided
      $v_srch_bg_to_mm ne K_EMPTY and                           #
      $v_srch_bg_to_dd ne K_EMPTY and                           #
      $v_srch_bg_to_hh ne K_EMPTY and                           #
      $v_srch_bg_to_mi ne K_EMPTY and                           #
      $v_srch_bg_to_ss eq K_EMPTY
      and ( (
            $v_srch_bg_to_yy * 100000000 +                      # Year multiply by 100000000
            $v_srch_bg_to_mm * 1000000 +                        # Month multiply by 1000000
            $v_srch_bg_to_dd * 10000 +                          # Day multiply by 10000
            $v_srch_bg_to_hh * 100 +                            # Hours multiply by 100
            $v_srch_bg_to_mi                                    #
         ) >                                                    #
         (
            $v_now_yy * 100000000 +                             # Year multiply by 1000000
              $v_now_mm * 1000000 +                             # Month multiply by 10000
              $v_now_dd * 10000 +                               # Day multiply by 100
              $v_now_hh * 100 +                                 # Hours multiply by 100
              $v_now_mi                                         #
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 34 ] ;             # Value: To timestamp after current timestamp
   } ## end if ( $v_srch_bg_to_yy ...)

   # Begin appicable timestamp( TO ) - Timestamp ( Year and month and day and hour and minute and seconds ) greater than current timestamp ( year and month and day and hour and minute and seconds )
   if (
      $v_srch_bg_to_yy ne K_EMPTY and                           # Check to timestamp after current timestamp if only year, month, day, hour, minute and second provided
      $v_srch_bg_to_mm ne K_EMPTY and                           #
      $v_srch_bg_to_dd ne K_EMPTY and                           #
      $v_srch_bg_to_hh ne K_EMPTY and                           #
      $v_srch_bg_to_mi ne K_EMPTY and                           #
      $v_srch_bg_to_ss ne K_EMPTY
      and ( (
            $v_srch_bg_to_yy * 10000000000 +                    # Year multiply by 10000000000
            $v_srch_bg_to_mm * 100000000 +                      # Month multiply by 100000000
            $v_srch_bg_to_dd * 1000000 +                        # Day multiply by 1000000
            $v_srch_bg_to_hh * 10000 +                          # Hours multiply by 1000000
            $v_srch_bg_to_mi * 100 +                            # Minutes multiply by 100
            $v_srch_bg_to_ss
         ) >                                                    #
         (
            $v_now_yy * 10000000000 +                           # Year multiply by 10000000000
              $v_now_mm * 100000000 +                           # Month multiply by 100000000
              $v_now_dd * 1000000 +                             # Day multiply by 1000000
              $v_now_hh * 10000 +                               # Hours multiply by 1000000
              $v_now_mi * 100 +                                 # Minutes multiply by 100
              $v_now_ss
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_bg_dt_err_msg .= ' - ' . $a_L[ 34 ] ;             # Value: To timestamp after current timestamp
   } ## end if ( $v_srch_bg_to_yy ...)

   $v_srch_bg_dt_err_msg =~ s/^\s-\s//g ;                       # Remove space hypen space

   # End appicable timestamp( FROM ) - Year greater than current year
   if (
      $v_srch_ed_from_yy ne K_EMPTY and                         # Check from timestamp after current timestamp if only year provided
      $v_srch_ed_from_mm eq K_EMPTY and                         #
      $v_srch_ed_from_dd eq K_EMPTY and                         #
      $v_srch_ed_from_hh eq K_EMPTY and                         #
      $v_srch_ed_from_mi eq K_EMPTY and                         #
      $v_srch_ed_from_ss eq K_EMPTY
      and ( $v_srch_ed_from_yy > $v_now_yy )                    # Year greater than current year
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 33 ] ;             # Value: From timestamp after current timestamp
   } ## end if ( $v_srch_ed_from_yy...)

   # End appicable timestamp( FROM ) - Timestamp ( Year and month ) greater than current timestamp ( year and month )
   if (
      $v_srch_ed_from_yy ne K_EMPTY and                         # Check from timestamp after current timestamp if only year and month provided
      $v_srch_ed_from_mm ne K_EMPTY and                         #
      $v_srch_ed_from_dd eq K_EMPTY and                         #
      $v_srch_ed_from_hh eq K_EMPTY and                         #
      $v_srch_ed_from_mi eq K_EMPTY and                         #
      $v_srch_ed_from_ss eq K_EMPTY
      and (
         ( $v_srch_ed_from_yy * 100 + $v_srch_ed_from_mm )      #
         >                                                      #
         ( $v_now_yy * 100 + $v_now_mm )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 33 ] ;             # Value: From timestamp after current timestamp
   } ## end if ( $v_srch_ed_from_yy...)

   # End appicable timestamp( FROM ) - Timestamp ( Year and month and day ) greater than current timestamp ( year and month and day )
   if (
      $v_srch_ed_from_yy ne K_EMPTY and                         # Check from timestamp after current timestamp if only year, month and day provided
      $v_srch_ed_from_mm ne K_EMPTY and                         #
      $v_srch_ed_from_dd ne K_EMPTY and                         #
      $v_srch_ed_from_hh eq K_EMPTY and                         #
      $v_srch_ed_from_mi eq K_EMPTY and                         #
      $v_srch_ed_from_ss eq K_EMPTY
      and ( (
            $v_srch_ed_from_yy * 10000 +                        # Year multiply by 10000
            $v_srch_ed_from_mm * 100 +                          # Month multiply by 100
            $v_srch_ed_from_dd                                  #
         ) >                                                    #
         (
            $v_now_yy * 10000 +                                 # Year multiply by 10000
              $v_now_mm * 100 +                                 # Month multiply by 100
              $v_now_dd                                         #
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 33 ] ;             # Value: From timestamp after current timestamp
   } ## end if ( $v_srch_ed_from_yy...)

   # End appicable timestamp( FROM ) - Timestamp ( Year and month and day and hour ) greater than current timestamp ( year and month and day and hour )
   if (
      $v_srch_ed_from_yy ne K_EMPTY and                         # Check from timestamp after current timestamp if only year, month, day and hour provided
      $v_srch_ed_from_mm ne K_EMPTY and                         #
      $v_srch_ed_from_dd ne K_EMPTY and                         #
      $v_srch_ed_from_hh ne K_EMPTY and                         #
      $v_srch_ed_from_mi eq K_EMPTY and                         #
      $v_srch_ed_from_ss eq K_EMPTY
      and ( (
            $v_srch_ed_from_yy * 1000000 +                      # Year multiply by 1000000
            $v_srch_ed_from_mm * 10000 +                        # Month multiply by 10000
            $v_srch_ed_from_dd * 100 +                          # Day multiply by 100
            $v_srch_ed_from_hh                                  #
         ) >                                                    #
         (
            $v_now_yy * 1000000 +                               # Year multiply by 1000000
              $v_now_mm * 10000 +                               # Month multiply by 10000
              $v_now_dd * 100 +                                 # Day multiply by 100
              $v_now_hh                                         #
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 33 ] ;             # Value: From timestamp after current timestamp
   } ## end if ( $v_srch_ed_from_yy...)

   # End appicable timestamp( FROM ) - Timestamp ( Year and month and day and hour and minute ) greater than current timestamp ( year and month and day and hour and minute )
   if (
      $v_srch_ed_from_yy ne K_EMPTY and                         # Check from timestamp after current timestamp if only year, month, day, hour and minute provided
      $v_srch_ed_from_mm ne K_EMPTY and                         #
      $v_srch_ed_from_dd ne K_EMPTY and                         #
      $v_srch_ed_from_hh ne K_EMPTY and                         #
      $v_srch_ed_from_mi ne K_EMPTY and                         #
      $v_srch_ed_from_ss eq K_EMPTY
      and ( (
            $v_srch_ed_from_yy * 100000000 +                    # Year multiply by 100000000
            $v_srch_ed_from_mm * 1000000 +                      # Month multiply by 1000000
            $v_srch_ed_from_dd * 10000 +                        # Day multiply by 10000
            $v_srch_ed_from_hh * 100 +                          # Hours multiply by 100
            $v_srch_ed_from_mi                                  #
         ) >                                                    #
         (
            $v_now_yy * 100000000 +                             # Year multiply by 1000000
              $v_now_mm * 1000000 +                             # Month multiply by 10000
              $v_now_dd * 10000 +                               # Day multiply by 100
              $v_now_hh * 100 +                                 # Hours multiply by 100
              $v_now_mi                                         #
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 33 ] ;             # Value: From timestamp after current timestamp
   } ## end if ( $v_srch_ed_from_yy...)

   # End appicable timestamp( FROM ) - Timestamp ( Year and month and day and hour and minute and seconds ) greater than current timestamp ( year and month and day and hour and minute and seconds )
   if (
      $v_srch_ed_from_yy ne K_EMPTY and                         # Check from timestamp after current timestamp if only year, month, day, hour, minute and second provided
      $v_srch_ed_from_mm ne K_EMPTY and                         #
      $v_srch_ed_from_dd ne K_EMPTY and                         #
      $v_srch_ed_from_hh ne K_EMPTY and                         #
      $v_srch_ed_from_mi ne K_EMPTY and                         #
      $v_srch_ed_from_ss ne K_EMPTY
      and ( (
            $v_srch_ed_from_yy * 10000000000 +                  # Year multiply by 10000000000
            $v_srch_ed_from_mm * 100000000 +                    # Month multiply by 100000000
            $v_srch_ed_from_dd * 1000000 +                      # Day multiply by 1000000
            $v_srch_ed_from_hh * 10000 +                        # Hours multiply by 1000000
            $v_srch_ed_from_mi * 100 +                          # Minutes multiply by 100
            $v_srch_ed_from_ss
         ) >                                                    #
         (
            $v_now_yy * 10000000000 +                           # Year multiply by 10000000000
              $v_now_mm * 100000000 +                           # Month multiply by 100000000
              $v_now_dd * 1000000 +                             # Day multiply by 1000000
              $v_now_hh * 10000 +                               # Hours multiply by 1000000
              $v_now_mi * 100 +                                 # Minutes multiply by 100
              $v_now_ss
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 33 ] ;             # Value: From timestamp after current timestamp
   } ## end if ( $v_srch_ed_from_yy...)

   # End appicable timestamp( TO ) - Year greater than current year
   if (
      $v_srch_ed_to_yy ne K_EMPTY and                           # Check to timestamp after current timestamp if only year provided
      $v_srch_ed_to_mm eq K_EMPTY and                           #
      $v_srch_ed_to_dd eq K_EMPTY and                           #
      $v_srch_ed_to_hh eq K_EMPTY and                           #
      $v_srch_ed_to_mi eq K_EMPTY and                           #
      $v_srch_ed_to_ss eq K_EMPTY
      and ( $v_srch_ed_to_yy > $v_now_yy )                      # Year greater than current year
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 34 ] ;             # Value: To timestamp after current timestamp
   } ## end if ( $v_srch_ed_to_yy ...)

   # End appicable timestamp( TO ) - Timestamp ( Year and month ) greater than current timestamp ( year and month )
   if (
      $v_srch_ed_to_yy ne K_EMPTY and                           # Check to timestamp after current timestamp if only year and month provided
      $v_srch_ed_to_mm ne K_EMPTY and                           #
      $v_srch_ed_to_dd eq K_EMPTY and                           #
      $v_srch_ed_to_hh eq K_EMPTY and                           #
      $v_srch_ed_to_mi eq K_EMPTY and                           #
      $v_srch_ed_to_ss eq K_EMPTY
      and (
         ( $v_srch_ed_to_yy * 100 + $v_srch_ed_to_mm )          #
         >                                                      #
         ( $v_now_yy * 100 + $v_now_mm )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 34 ] ;             # Value: To timestamp after current timestamp
   } ## end if ( $v_srch_ed_to_yy ...)

   # End appicable timestamp( TO ) - Timestamp ( Year and month and day ) greater than current timestamp ( year and month and day )
   if (
      $v_srch_ed_to_yy ne K_EMPTY and                           # Check to timestamp after current timestamp if only year, month and day provided
      $v_srch_ed_to_mm ne K_EMPTY and                           #
      $v_srch_ed_to_dd ne K_EMPTY and                           #
      $v_srch_ed_to_hh eq K_EMPTY and                           #
      $v_srch_ed_to_mi eq K_EMPTY and                           #
      $v_srch_ed_to_ss eq K_EMPTY
      and ( (
            $v_srch_ed_to_yy * 10000 +                          # Year multiply by 10000
            $v_srch_ed_to_mm * 100 +                            # Month multiply by 100
            $v_srch_ed_to_dd                                    #
         ) >                                                    #
         (
            $v_now_yy * 10000 +                                 # Year multiply by 10000
              $v_now_mm * 100 +                                 # Month multiply by 100
              $v_now_dd                                         #
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 34 ] ;             # Value: To timestamp after current timestamp
   } ## end if ( $v_srch_ed_to_yy ...)

   # End appicable timestamp( TO ) - Timestamp ( Year and month and day and hour ) greater than current timestamp ( year and month and day and hour )
   if (
      $v_srch_ed_to_yy ne K_EMPTY and                           # Check to timestamp after current timestamp if only year, month, day and hour provided
      $v_srch_ed_to_mm ne K_EMPTY and                           #
      $v_srch_ed_to_dd ne K_EMPTY and                           #
      $v_srch_ed_to_hh ne K_EMPTY and                           #
      $v_srch_ed_to_mi eq K_EMPTY and                           #
      $v_srch_ed_to_ss eq K_EMPTY
      and ( (
            $v_srch_ed_to_yy * 1000000 +                        # Year multiply by 1000000
            $v_srch_ed_to_mm * 10000 +                          # Month multiply by 10000
            $v_srch_ed_to_dd * 100 +                            # Day multiply by 100
            $v_srch_ed_to_hh                                    #
         ) >                                                    #
         (
            $v_now_yy * 1000000 +                               # Year multiply by 1000000
              $v_now_mm * 10000 +                               # Month multiply by 10000
              $v_now_dd * 100 +                                 # Day multiply by 100
              $v_now_hh                                         #
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 34 ] ;             # Value: To timestamp after current timestamp
   } ## end if ( $v_srch_ed_to_yy ...)

   # End appicable timestamp( TO ) - Timestamp ( Year and month and day and hour and minute ) greater than current timestamp ( year and month and day and hour and minute )
   if (
      $v_srch_ed_to_yy ne K_EMPTY and                           # Check to timestamp after current timestamp if only year, month, day, hour and minute provided
      $v_srch_ed_to_mm ne K_EMPTY and                           #
      $v_srch_ed_to_dd ne K_EMPTY and                           #
      $v_srch_ed_to_hh ne K_EMPTY and                           #
      $v_srch_ed_to_mi ne K_EMPTY and                           #
      $v_srch_ed_to_ss eq K_EMPTY
      and ( (
            $v_srch_ed_to_yy * 100000000 +                      # Year multiply by 100000000
            $v_srch_ed_to_mm * 1000000 +                        # Month multiply by 1000000
            $v_srch_ed_to_dd * 10000 +                          # Day multiply by 10000
            $v_srch_ed_to_hh * 100 +                            # Hours multiply by 100
            $v_srch_ed_to_mi                                    #
         ) >                                                    #
         (
            $v_now_yy * 100000000 +                             # Year multiply by 1000000
              $v_now_mm * 1000000 +                             # Month multiply by 10000
              $v_now_dd * 10000 +                               # Day multiply by 100
              $v_now_hh * 100 +                                 # Hours multiply by 100
              $v_now_mi                                         #
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 34 ] ;             # Value: To timestamp after current timestamp
   } ## end if ( $v_srch_ed_to_yy ...)

   # End appicable timestamp( TO ) - Timestamp ( Year and month and day and hour and minute and seconds ) greater than current timestamp ( year and month and day and hour and minute and seconds )
   if (
      $v_srch_ed_to_yy ne K_EMPTY and                           # Check to timestamp after current timestamp if only year, month, day, hour, minute and second provided
      $v_srch_ed_to_mm ne K_EMPTY and                           #
      $v_srch_ed_to_dd ne K_EMPTY and                           #
      $v_srch_ed_to_hh ne K_EMPTY and                           #
      $v_srch_ed_to_mi ne K_EMPTY and                           #
      $v_srch_ed_to_ss ne K_EMPTY
      and ( (
            $v_srch_ed_to_yy * 10000000000 +                    # Year multiply by 10000000000
            $v_srch_ed_to_mm * 100000000 +                      # Month multiply by 100000000
            $v_srch_ed_to_dd * 1000000 +                        # Day multiply by 1000000
            $v_srch_ed_to_hh * 10000 +                          # Hours multiply by 1000000
            $v_srch_ed_to_mi * 100 +                            # Minutes multiply by 100
            $v_srch_ed_to_ss
         ) >                                                    #
         (
            $v_now_yy * 10000000000 +                           # Year multiply by 10000000000
              $v_now_mm * 100000000 +                           # Month multiply by 100000000
              $v_now_dd * 1000000 +                             # Day multiply by 1000000
              $v_now_hh * 10000 +                               # Hours multiply by 1000000
              $v_now_mi * 100 +                                 # Minutes multiply by 100
              $v_now_ss
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 34 ] ;             # Value: To timestamp after current timestamp
   } ## end if ( $v_srch_ed_to_yy ...)

   $v_srch_ed_dt_err_msg =~ s/^\s-\s//g ;

   if ( $v_srch_f_err eq K_YES ) {                              # STAGE 5 - Get out if error in main screen
      return ;
   }

   # STAGE 6 ##########################################################

   # Begin applicable timestamp - Internally set job start time ( FROM )
   if (
      $v_srch_bg_from_yy eq K_EMPTY and                         # Set job start time one week ago if all From and To fields of start time not choosen
      $v_srch_bg_from_mm eq K_EMPTY and                         #
      $v_srch_bg_from_dd eq K_EMPTY and                         #
      $v_srch_bg_from_hh eq K_EMPTY and                         #
      $v_srch_bg_from_mi eq K_EMPTY and                         #
      $v_srch_bg_from_ss eq K_EMPTY and                         #
      $v_srch_bg_to_yy eq K_EMPTY   and                         #
      $v_srch_bg_to_mm eq K_EMPTY   and                         #
      $v_srch_bg_to_dd eq K_EMPTY   and                         #
      $v_srch_bg_to_hh eq K_EMPTY   and                         #
      $v_srch_bg_to_mi eq K_EMPTY   and                         #
      $v_srch_bg_to_ss eq K_EMPTY                               #
     ) {                                                        #

      $v_srch_bg_to_yy = $v_now_yy ;                            # Set Start time - To fields internally
      $v_srch_bg_to_mm = $v_now_mm ;                            #
      $v_srch_bg_to_dd = $v_now_dd ;                            #
      $v_srch_bg_to_hh = $v_now_hh ;                            #
      $v_srch_bg_to_mi = $v_now_mi ;                            #
      $v_srch_bg_to_ss = $v_now_ss ;                            #

      ( $v_srch_bg_from_yy , $v_srch_bg_from_mm , $v_srch_bg_from_dd ) =    # Set Start time - From fields internally
        Add_Delta_YMD (                                         #
         $v_srch_bg_to_yy , $v_srch_bg_to_mm ,                  #
         $v_srch_bg_to_dd , 0 , 0 , -7                          #
        ) ;                                                     #

      $v_srch_bg_from_mm =                                      #
        (                                                       #
         $v_srch_bg_from_mm < 10 ?                              #
           '0' . $v_srch_bg_from_mm                             #
         : $v_srch_bg_from_mm                                   #
        ) ;                                                     #

      $v_srch_bg_from_dd =                                      #
        (                                                       #
         $v_srch_bg_from_dd < 10 ?                              #
           '0' . $v_srch_bg_from_dd                             #
         : $v_srch_bg_from_dd                                   #
        ) ;                                                     #

      $v_srch_bg_from_hh = '00' ;
      $v_srch_bg_from_mi = '00' ;
      $v_srch_bg_from_ss = '00' ;

   } ## end if ( $v_srch_bg_from_yy...)

   # Months - 01
   if (
      $v_srch_bg_from_yy ne K_EMPTY and                         #
      $v_srch_bg_from_mm eq K_EMPTY
     ) {                                                        #
      $v_srch_bg_from_mm = '01' ;                               #
   }                                                            #

   # Days - 01
   if (
      $v_srch_bg_from_yy ne K_EMPTY and                         #
      $v_srch_bg_from_mm ne K_EMPTY and                         #
      $v_srch_bg_from_dd eq K_EMPTY
     ) {                                                        #
      $v_srch_bg_from_dd = '01' ;                               #
   }                                                            #

   # Hours - 00
   if (
      $v_srch_bg_from_yy ne K_EMPTY and                         #
      $v_srch_bg_from_mm ne K_EMPTY and                         #
      $v_srch_bg_from_dd ne K_EMPTY and                         #
      $v_srch_bg_from_hh eq K_EMPTY
     ) {                                                        #
      $v_srch_bg_from_hh = '00' ;                               #
   }                                                            #

   # Minutes - 00
   if (
      $v_srch_bg_from_yy ne K_EMPTY and                         #
      $v_srch_bg_from_mm ne K_EMPTY and                         #
      $v_srch_bg_from_dd ne K_EMPTY and                         #
      $v_srch_bg_from_hh ne K_EMPTY and                         #
      $v_srch_bg_from_mi eq K_EMPTY
     ) {                                                        #
      $v_srch_bg_from_mi = '00' ;                               #
   }                                                            #

   # Seconds - 00
   if (
      $v_srch_bg_from_yy ne K_EMPTY and                         #
      $v_srch_bg_from_mm ne K_EMPTY and                         #
      $v_srch_bg_from_dd ne K_EMPTY and                         #
      $v_srch_bg_from_hh ne K_EMPTY and                         #
      $v_srch_bg_from_mi ne K_EMPTY and                         #
      $v_srch_bg_from_ss eq K_EMPTY
     ) {                                                        #
      $v_srch_bg_from_ss = '00' ;                               #
   }                                                            #

   ##############################################################

   # Begin applicable timestamp - From ( not empty ) and To ( empty )
   if ( (
         $v_srch_bg_from_yy ne K_EMPTY and                      # If all fields choosen in ( FROM ) begin timestamp
         $v_srch_bg_from_mm ne K_EMPTY and                      #
         $v_srch_bg_from_dd ne K_EMPTY and                      #
         $v_srch_bg_from_hh ne K_EMPTY and                      #
         $v_srch_bg_from_mi ne K_EMPTY and                      #
         $v_srch_bg_from_ss ne K_EMPTY
      )
      and                                                       #
      (
         $v_srch_bg_to_yy eq K_EMPTY and                        # If all fields not choosen ( TO ) begin timestamp
         $v_srch_bg_to_mm eq K_EMPTY and                        #
         $v_srch_bg_to_dd eq K_EMPTY and                        #
         $v_srch_bg_to_hh eq K_EMPTY and                        #
         $v_srch_bg_to_mi eq K_EMPTY and                        #
         $v_srch_bg_to_ss eq K_EMPTY
      )                                                         #
     ) {                                                        #

      # If all fields of TO timestamp empty then set to 7 days
      ( $v_srch_bg_to_yy , $v_srch_bg_to_mm , $v_srch_bg_to_dd ) =    # Set To Start time - From fields internally
        Add_Delta_YMD (                                         #
         $v_srch_bg_from_yy , $v_srch_bg_from_mm ,              #
         $v_srch_bg_from_dd , 0 , 0 , 7                         #
        ) ;

      # Job start time - If From timestamp after current timestamp then
      # adjust to current timestamp internally

      if ( (
            $v_srch_bg_to_yy * 10000 +                          #
            $v_srch_bg_to_mm * 100 +                            #
            $v_srch_bg_to_dd
         ) >                                                    #
         (
            $v_now_yy * 10000 +                                 #
              $v_now_mm * 100 +                                 #
              $v_now_dd
         )
        ) {                                                     # Job start time - If From timestamp after current timestamp then
         $v_srch_bg_to_yy = $v_now_yy ;                         #  adjust to current timestamp internally
         $v_srch_bg_to_mm = $v_now_mm ;                         #
         $v_srch_bg_to_dd = $v_now_dd ;                         #
      }                                                         #
      else {
         $v_srch_bg_to_mm =                                     #
           (                                                    #
            $v_srch_bg_to_mm < 10 ?                             #
              '0' . $v_srch_bg_to_mm                            #
            : $v_srch_bg_to_mm                                  #
           ) ;                                                  #
         $v_srch_bg_to_dd =                                     #
           (                                                    #
            $v_srch_bg_to_dd < 10 ?                             #
              '0' . $v_srch_bg_to_dd                            #
            : $v_srch_bg_to_dd                                  #
           ) ;                                                  #
      } ## end else [ if ( ( $v_srch_bg_to_yy...))]
   } ## end if ( ( $v_srch_bg_from_yy...))

   # Internally set job start time ( TO ) to latest time
   # Begin applicable timestamp( TO ) - Year ( not empty ) month ( empty )
   if (
      $v_srch_bg_to_yy ne K_EMPTY and                           #
      $v_srch_bg_to_mm eq K_EMPTY                               #
     ) {                                                        #
      if ( $v_srch_bg_to_yy == $v_now_yy ) {                    # If selected year equals to current year
         $v_srch_bg_to_mm = $v_now_mm ;                         # Set current month to TO timestamp
      }                                                         #
      else {                                                    #
         $v_srch_bg_to_mm = '12' ;                              # ...else set 12 (December)
      }                                                         #
   }                                                            #

   # Begin applicable timestamp( TO ) - Year and month ( not empty ) , Day ( empty )
   if (
      $v_srch_bg_to_yy ne K_EMPTY and                           #
      $v_srch_bg_to_mm ne K_EMPTY and                           #
      $v_srch_bg_to_dd eq K_EMPTY
     ) {                                                        #
      if (
         $v_srch_bg_to_yy == $v_now_yy and                      # If selected year equals to current year
         $v_srch_bg_to_mm == $v_now_mm                          # If selected month equals to current month
        ) {                                                     #
         $v_srch_bg_to_dd = $v_now_dd ;                         # Set current day to TO timestamp
      }                                                         #
      else {                                                    #
         $v_srch_bg_to_dd =                                     # else Last day of month using subroutine sLastDayOfMonth
           &sLastDayOfMonth (                                   #
            $v_srch_bg_to_yy , $v_srch_bg_to_mm                 #
           ) ;                                                  #
      }                                                         #
   }                                                            #

   # Begin applicable timestamp( TO ) - Year and month day ( no empty ) , hour ( empty )
   if (
      $v_srch_bg_to_yy ne K_EMPTY and                           #
      $v_srch_bg_to_mm ne K_EMPTY and                           #
      $v_srch_bg_to_dd ne K_EMPTY and                           #
      $v_srch_bg_to_hh eq K_EMPTY
     ) {                                                        #
      if (
         $v_srch_bg_to_yy == $v_now_yy and                      # If selected year equals to current year
         $v_srch_bg_to_mm == $v_now_mm and                      # If selected month equals to current month
         $v_srch_bg_to_dd == $v_now_dd                          # If selected day equals to current day
        ) {                                                     #
         $v_srch_bg_to_hh = $v_now_hh ;                         # Set current hour to TO timestamp
      }                                                         #
      else {                                                    #
         $v_srch_bg_to_hh = '23' ;                              # ...else set 23
      }                                                         #
   }                                                            #

   # Begin applicable timestamp( TO ) - Year , month , day , hour ( not empty ) , minute ( empty )
   if (
      $v_srch_bg_to_yy ne K_EMPTY and                           #
      $v_srch_bg_to_mm ne K_EMPTY and                           #
      $v_srch_bg_to_dd ne K_EMPTY and                           #
      $v_srch_bg_to_hh ne K_EMPTY and                           #
      $v_srch_bg_to_mi eq K_EMPTY
     ) {                                                        #
      if (
         $v_srch_bg_to_yy == $v_now_yy and                      # If selected year equals to current year
         $v_srch_bg_to_mm == $v_now_mm and                      # If selected month equals to current month
         $v_srch_bg_to_dd == $v_now_dd and                      # If selected day equals to current day
         $v_srch_bg_to_hh == $v_now_hh                          # If selected hour equals to current hour
        ) {                                                     #
         $v_srch_bg_to_mi = $v_now_mi ;                         # Set current minute to TO timestamp
      }                                                         #
      else {                                                    #
         $v_srch_bg_to_mi = '59' ;                              # ...else set 59
      }                                                         #
   }                                                            #

   # Begin applicable timestamp( TO ) - Year , month , day , hour , minute ( not empty ) , second( empty )
   if (
      $v_srch_bg_to_yy ne K_EMPTY and                           #
      $v_srch_bg_to_mm ne K_EMPTY and                           #
      $v_srch_bg_to_dd ne K_EMPTY and                           #
      $v_srch_bg_to_hh ne K_EMPTY and                           #
      $v_srch_bg_to_mi ne K_EMPTY and                           #
      $v_srch_bg_to_ss eq K_EMPTY
     ) {                                                        #

      if (
         $v_srch_bg_to_yy == $v_now_yy and                      # If selected year equals to current year
         $v_srch_bg_to_mm == $v_now_mm and                      # If selected month equals to current month
         $v_srch_bg_to_dd == $v_now_dd and                      # If selected day equals to current day
         $v_srch_bg_to_hh == $v_now_hh and                      # If selected hour equals to current hour
         $v_srch_bg_to_mi == $v_now_mi
        ) {                                                     #
         $v_srch_bg_to_ss = $v_now_ss ;                         # Set current seconds to TO timestamp
      }                                                         #
      else {                                                    #
         $v_srch_bg_to_ss = '59' ;                              # ...else set 59
      }                                                         #
   }                                                            #

   ##############################################################

   # Begin applicable timestamp( FROM ) - From ( empty ) and To ( not empty )
   if ( (
         $v_srch_bg_from_yy eq K_EMPTY and                      # If all FROM timestamp selected
         $v_srch_bg_from_mm eq K_EMPTY and                      #
         $v_srch_bg_from_dd eq K_EMPTY and                      #
         $v_srch_bg_from_hh eq K_EMPTY and                      #
         $v_srch_bg_from_mi eq K_EMPTY and                      #
         $v_srch_bg_from_ss eq K_EMPTY
      )
      and                                                       #
      (
         $v_srch_bg_to_yy ne K_EMPTY and                        # If all TO timestamp not selected
         $v_srch_bg_to_mm ne K_EMPTY and                        #
         $v_srch_bg_to_dd ne K_EMPTY and                        #
         $v_srch_bg_to_hh ne K_EMPTY and                        #
         $v_srch_bg_to_mi ne K_EMPTY and                        #
         $v_srch_bg_to_ss ne K_EMPTY
      )                                                         #
     ) {                                                        #

      ( $v_srch_bg_from_yy , $v_srch_bg_from_mm , $v_srch_bg_from_dd ) =    # Set Start time - From fields internally
        Add_Delta_YMD (                                         #
         $v_srch_bg_to_yy , $v_srch_bg_to_mm ,                  #
         $v_srch_bg_to_dd , 0 , 0 , -7                          # Set 7 days before
        ) ;                                                     #
                                                                #
      $v_srch_bg_from_mm =                                      #
        (                                                       #
         $v_srch_bg_from_mm < 10 ?                              #
           '0' . $v_srch_bg_from_mm                             #
         : $v_srch_bg_from_mm                                   #
        ) ;                                                     #
      $v_srch_bg_from_dd =                                      #
        (                                                       #
         $v_srch_bg_from_dd < 10 ?                              #
           '0' . $v_srch_bg_from_dd                             #
         : $v_srch_bg_from_dd                                   #
        ) ;                                                     #
      $v_srch_bg_from_hh = '00' ;
      $v_srch_bg_from_mi = '00' ;
      $v_srch_bg_from_ss = '00' ;
   } ## end if ( ( $v_srch_bg_from_yy...))

   ##############################################################

   # End applicable timestamp - TO ( empty ) , FROM ( not empty )
   if ( (
         $v_srch_ed_to_yy eq K_EMPTY and                        #
         $v_srch_ed_to_mm eq K_EMPTY and                        #
         $v_srch_ed_to_dd eq K_EMPTY and                        #
         $v_srch_ed_to_hh eq K_EMPTY and                        #
         $v_srch_ed_to_mi eq K_EMPTY and                        #
         $v_srch_ed_to_ss eq K_EMPTY
      )
      and                                                       #
      (
         $v_srch_ed_from_yy ne K_EMPTY and                      #
         $v_srch_ed_from_mm ne K_EMPTY and                      #
         $v_srch_ed_from_dd ne K_EMPTY and                      #
         $v_srch_ed_from_hh ne K_EMPTY and                      #
         $v_srch_ed_from_mi ne K_EMPTY and                      #
         $v_srch_ed_from_ss ne K_EMPTY
      )                                                         #
     ) {                                                        #

      ( $v_srch_ed_to_yy , $v_srch_ed_to_mm , $v_srch_ed_to_dd ) =    #
        Add_Delta_YMD (                                         #
         $v_srch_ed_from_yy , $v_srch_ed_from_mm ,              #
         $v_srch_ed_from_dd , 0 , 0 , 7                         # Set job end time (To) one week ago
        ) ;                                                     #
                                                                #
      if ( (                                                      #
            $v_srch_ed_to_yy * 10000 +                          #
            $v_srch_ed_to_mm * 100 +                            #
            $v_srch_ed_to_dd                                    #
         ) >                                                    #
         (                                                      #
            $v_now_yy * 10000 +                                 #
              $v_now_mm * 100 +                                 #
              $v_now_dd                                         #
         )                                                      #
        ) {                                                     # If Job end time - From timestamp after current timestamp then
         $v_srch_ed_to_yy = $v_now_yy ;                         # adjust to current timestamp internally
         $v_srch_ed_to_mm = $v_now_mm ;                         #
         $v_srch_ed_to_dd = $v_now_dd ;                         #
      }                                                         #
      else {                                                    #
         $v_srch_ed_to_mm =                                     #
           (                                                    #
            $v_srch_ed_to_mm < 10 ?                             #
              '0' . $v_srch_ed_to_mm                            #
            : $v_srch_ed_to_mm                                  #
           ) ;                                                  #
         $v_srch_ed_to_dd =                                     #
           (                                                    #
            $v_srch_ed_to_dd < 10 ?                             #
              '0' . $v_srch_ed_to_dd                            #
            : $v_srch_ed_to_dd                                  #
           ) ;                                                  #
      }
   } ## end if ( ( $v_srch_ed_to_yy...))

   ##############################################################

   # End applicable timestamp( FROM ) - Internally set job end time (To) to latest time
   # End applicable timestamp( FROM ) - month ( empty )
   if (
      $v_srch_ed_from_yy ne K_EMPTY and                         #
      $v_srch_ed_from_mm eq K_EMPTY                             #
     ) {                                                        #
      $v_srch_ed_from_mm = '01' ;                               # Set to 01
   }                                                            #

   # End applicable timestamp( FROM ) - day ( empty )
   if (
      $v_srch_ed_from_yy ne K_EMPTY and                         #
      $v_srch_ed_from_mm ne K_EMPTY and                         #
      $v_srch_ed_from_dd eq K_EMPTY
     ) {                                                        #
      $v_srch_ed_from_dd = '01' ;                               # Set to 01
   }                                                            #

   # End applicable timestamp( FROM ) - hour ( empty )
   if (
      $v_srch_ed_from_yy ne K_EMPTY and                         #
      $v_srch_ed_from_mm ne K_EMPTY and                         #
      $v_srch_ed_from_dd ne K_EMPTY and                         #
      $v_srch_ed_from_hh eq K_EMPTY
     ) {                                                        #
      $v_srch_ed_from_hh = '00' ;                               # Set to 00
   }                                                            #

   # End applicable timestamp( FROM ) - minute ( empty )
   if (
      $v_srch_ed_from_yy ne K_EMPTY and                         #
      $v_srch_ed_from_mm ne K_EMPTY and                         #
      $v_srch_ed_from_dd ne K_EMPTY and                         #
      $v_srch_ed_from_hh ne K_EMPTY and                         #
      $v_srch_ed_from_mi eq K_EMPTY
     ) {                                                        #
      $v_srch_ed_from_mi = '00' ;                               # Set to 00
   }                                                            #

   # End applicable timestamp( FROM ) - second ( empty )
   if (
      $v_srch_ed_from_yy ne K_EMPTY and                         #
      $v_srch_ed_from_mm ne K_EMPTY and                         #
      $v_srch_ed_from_dd ne K_EMPTY and                         #
      $v_srch_ed_from_hh ne K_EMPTY and                         #
      $v_srch_ed_from_mi ne K_EMPTY and                         #
      $v_srch_ed_from_ss eq K_EMPTY
     ) {                                                        #
      $v_srch_ed_from_ss = '00' ;                               # Set to 00
   }                                                            #

   ##############################################################

   # Internally set job end time (To) to latest time            #

   # End applicable timestamp( TO ) - year ( not empty ) , month ( empty )
   if (
      $v_srch_ed_to_yy ne K_EMPTY and                           #
      $v_srch_ed_to_mm eq K_EMPTY
     ) {                                                        #
      if ( $v_srch_ed_to_yy == $v_now_yy ) {                    # If selected year equals to current year
         $v_srch_ed_to_mm = $v_now_mm ;                         # Then set current month to To timestamp
      }                                                         #
      else {                                                    #
         $v_srch_ed_to_mm = 12 ;                                # ...else set 12
      }                                                         #
   }                                                            #

   # End applicable timestamp( TO ) - year and month ( not empty ) , day ( empty )
   if (
      $v_srch_ed_to_yy ne K_EMPTY and                           #
      $v_srch_ed_to_mm ne K_EMPTY and                           #
      $v_srch_ed_to_dd eq K_EMPTY
     ) {                                                        #
      if (
         $v_srch_ed_to_yy == $v_now_yy and                      # If selected year equals to current year
         $v_srch_ed_to_mm == $v_now_mm                          # If selected month equals to current month
        ) {                                                     #
         $v_srch_ed_to_dd = $v_now_dd ;                         # Then set current day to To timestamp
      }                                                         #
      else {                                                    #
         $v_srch_ed_to_dd =                                     # ... else last day of month using subroutine sLastDayOfMonth
           &sLastDayOfMonth (                                   #
            $v_srch_ed_to_yy , $v_srch_ed_to_mm                 #
           ) ;                                                  #
      }                                                         #
   }                                                            #

   # End applicable timestamp( TO ) - year , month and day ( not empty ) , hour ( empty )
   if (
      $v_srch_ed_to_yy ne K_EMPTY and                           #
      $v_srch_ed_to_mm ne K_EMPTY and                           #
      $v_srch_ed_to_dd ne K_EMPTY and                           #
      $v_srch_ed_to_hh eq K_EMPTY
     ) {                                                        #
      if (
         $v_srch_ed_to_yy == $v_now_yy and                      # If selected year equals to current year
         $v_srch_ed_to_mm == $v_now_mm and                      # If selected month equals to current month
         $v_srch_ed_to_dd == $v_now_dd                          # If selected day equals to current day
        ) {                                                     #
         $v_srch_ed_to_hh = $v_now_hh ;                         # Then set current hour to To timestamp
      }                                                         #
      else {                                                    #
         $v_srch_ed_to_hh = 23 ;                                # ...else set 23
      }                                                         #
   }                                                            #

   # End applicable timestamp( TO ) - year , month , day and hour ( not empty ) , minute ( empty )
   if (
      $v_srch_ed_to_yy ne K_EMPTY and                           #
      $v_srch_ed_to_mm ne K_EMPTY and                           #
      $v_srch_ed_to_dd ne K_EMPTY and                           #
      $v_srch_ed_to_hh ne K_EMPTY and                           #
      $v_srch_ed_to_mi eq K_EMPTY
     ) {                                                        #
      if (
         $v_srch_ed_to_yy == $v_now_yy and                      # If selected year equals to current year
         $v_srch_ed_to_mm == $v_now_mm and                      # If selected month equals to current month
         $v_srch_ed_to_dd == $v_now_dd and                      # If selected day equals to current day
         $v_srch_ed_to_hh == $v_now_hh                          # If selected hour equals to current hour
        ) {                                                     #
         $v_srch_ed_to_mi = $v_now_mi ;                         # Then set current minute to To timestamp
      }                                                         #
      else {                                                    #
         $v_srch_ed_to_mi = 59 ;                                # ...else set 59
      }                                                         #
   }                                                            #

   # End applicable timestamp( TO ) - year , month , day , hour and minute ( not empty ) , second ( empty )
   if (
      $v_srch_ed_to_yy ne K_EMPTY and                           #
      $v_srch_ed_to_mm ne K_EMPTY and                           #
      $v_srch_ed_to_dd ne K_EMPTY and                           #
      $v_srch_ed_to_hh ne K_EMPTY and                           #
      $v_srch_ed_to_mi ne K_EMPTY and                           #
      $v_srch_ed_to_ss eq K_EMPTY
     ) {                                                        #
                                                                #
      if (
         $v_srch_ed_to_yy == $v_now_yy and                      # If selected year equals to current year
         $v_srch_ed_to_mm == $v_now_mm and                      # If selected month equals to current month
         $v_srch_ed_to_dd == $v_now_dd and                      # If selected day equals to current day
         $v_srch_ed_to_hh == $v_now_hh and                      # If selected hour equals to current hour
         $v_srch_ed_to_mi == $v_now_mi                          # If selected minute equals to current minute
        ) {                                                     #
         $v_srch_ed_to_ss = $v_now_ss ;                         # Then set current second to To timestamp
      }                                                         #
      else {                                                    #
         $v_srch_ed_to_ss = 59 ;                                # ...else set 59
      }                                                         #
   }                                                            #

   # End applicable timestamp - FROM ( empty ) , TO ( not empty )
   if ( (
         $v_srch_ed_from_yy eq K_EMPTY and                      # Set job end time (From) one week ago
         $v_srch_ed_from_mm eq K_EMPTY and                      #  from To timestamp if From null and To chosen
         $v_srch_ed_from_dd eq K_EMPTY and                      #
         $v_srch_ed_from_hh eq K_EMPTY and                      #
         $v_srch_ed_from_mi eq K_EMPTY and                      #
         $v_srch_ed_from_ss eq K_EMPTY
      )
      and                                                       #
      (
         $v_srch_ed_to_yy ne K_EMPTY and                        #
         $v_srch_ed_to_mm ne K_EMPTY and                        #
         $v_srch_ed_to_dd ne K_EMPTY and                        #
         $v_srch_ed_to_hh ne K_EMPTY and                        #
         $v_srch_ed_to_mi ne K_EMPTY and                        #
         $v_srch_ed_to_ss ne K_EMPTY
      )                                                         #
     ) {                                                        #

      ( $v_srch_ed_from_yy , $v_srch_ed_from_mm , $v_srch_ed_from_dd ) =    #
        Add_Delta_YMD (                                         #
         $v_srch_ed_to_yy , $v_srch_ed_to_mm ,                  #
         $v_srch_ed_to_dd , 0 , 0 , -7                          # Set 7 days before
        ) ;                                                     #
                                                                #
      $v_srch_ed_from_mm =                                      #
        (                                                       #
         $v_srch_ed_from_mm < 10 ?                              #
           '0' . $v_srch_ed_from_mm                             #
         : $v_srch_ed_from_mm                                   #
        ) ;                                                     #
      $v_srch_ed_from_dd =                                      #
        (                                                       #
         $v_srch_ed_from_dd < 10 ?                              #
           '0' . $v_srch_ed_from_dd                             #
         : $v_srch_ed_from_dd                                   #
        ) ;                                                     #
                                                                #
      $v_srch_ed_from_hh = '00' ;                               #
      $v_srch_ed_from_mi = '00' ;                               #
      $v_srch_ed_from_ss = '00' ;                               #
   } ## end if ( ( $v_srch_ed_from_yy...))

   ##############################################################
   
   if (
      $v_srch_ed_from_yy ne K_EMPTY and                         #
      $v_srch_ed_from_mm ne K_EMPTY and                         #
      $v_srch_ed_from_dd ne K_EMPTY and                         #
      $v_srch_ed_from_hh ne K_EMPTY and                         #
      $v_srch_ed_from_mi ne K_EMPTY and                         #
      $v_srch_ed_from_ss ne K_EMPTY
     ) {                                                        #
      $v_f_ed_emp = K_YES ;                           # Flag - default n, set to Y if job end time provided
   }
    
   if (
      $v_srch_ed_to_yy ne K_EMPTY and                           #
      $v_srch_ed_to_mm ne K_EMPTY and                           #
      $v_srch_ed_to_dd ne K_EMPTY and                           #
      $v_srch_ed_to_hh ne K_EMPTY and                           #
      $v_srch_ed_to_mi ne K_EMPTY and                           #
      $v_srch_ed_to_ss ne K_EMPTY
     ) {                                                        #
      $v_f_ed_emp = K_YES ;                           # Flag - default n, set to Y if job end time provided
   } ## end if ( $v_srch_ed_to_yy ...)

   if (
      $v_srch_bg_from_yy ne K_EMPTY and                         # Store begin time - from timestamp if not empty
      $v_srch_bg_from_mm ne K_EMPTY and                         #
      $v_srch_bg_from_dd ne K_EMPTY and                         #
      $v_srch_bg_from_hh ne K_EMPTY and                         #
      $v_srch_bg_from_mi ne K_EMPTY and                         #
      $v_srch_bg_from_ss ne K_EMPTY
     ) {
      $p_srch_h_bg_from_dt =                                    # Variable to store begin timestamp (From)
        $v_srch_bg_from_yy . '-'                                #
        . $v_srch_bg_from_mm . '-'                              #
        . $v_srch_bg_from_dd . ' '                              #
        . $v_srch_bg_from_hh . ':'                              #
        . $v_srch_bg_from_mi . ':'                              #
        . $v_srch_bg_from_ss ;                                  #
   }
   else {
     $p_srch_h_bg_from_dt = K_EMPTY ;
   }

   if (
      $v_srch_bg_to_yy ne K_EMPTY and                           # Store begin time - from timestamp if not empty
      $v_srch_bg_to_mm ne K_EMPTY and                           #
      $v_srch_bg_to_dd ne K_EMPTY and                           #
      $v_srch_bg_to_hh ne K_EMPTY and                           #
      $v_srch_bg_to_mi ne K_EMPTY and                           #
      $v_srch_bg_to_ss ne K_EMPTY
     ) {

      $p_srch_h_bg_to_dt =                                      # Variable to store begin timestamp (To)
        $v_srch_bg_to_yy . '-'                                  #
        . $v_srch_bg_to_mm . '-'                                #
        . $v_srch_bg_to_dd . ' '                                #
        . $v_srch_bg_to_hh . ':'                                #
        . $v_srch_bg_to_mi . ':'                                #
        . $v_srch_bg_to_ss ;
   }
   else {
     $p_srch_h_bg_to_dt = K_EMPTY ;
   }

   # Display selected FROM and TO end timestamp beside Timestamp fields
   if (
      $v_srch_ed_from_yy ne K_EMPTY and                         # Store End time - from timestamp if not empty
      $v_srch_ed_from_mm ne K_EMPTY and                         #
      $v_srch_ed_from_dd ne K_EMPTY and                         #
      $v_srch_ed_from_hh ne K_EMPTY and                         #
      $v_srch_ed_from_mi ne K_EMPTY and                         #
      $v_srch_ed_from_ss ne K_EMPTY
     ) {                                                        #
      $p_srch_h_ed_from_dt =                                    # Variable to store end timestamp (From)
        $v_srch_ed_from_yy . '-'                                #
        . $v_srch_ed_from_mm . '-'                              #
        . $v_srch_ed_from_dd . ' '                              #
        . $v_srch_ed_from_hh . ':'                              #
        . $v_srch_ed_from_mi . ':'                              #
        . $v_srch_ed_from_ss                                    #
        ;
   } ## end if ( $v_srch_ed_from_yy...)
   else {
      $p_srch_h_ed_from_dt = K_EMPTY ;                          # Variable to store end timestamp (From)
   }

   if (
      $v_srch_ed_to_yy ne K_EMPTY and                           # Store End time - to timestamp if not empty
      $v_srch_ed_to_mm ne K_EMPTY and                           #
      $v_srch_ed_to_dd ne K_EMPTY and                           #
      $v_srch_ed_to_hh ne K_EMPTY and                           #
      $v_srch_ed_to_mi ne K_EMPTY and                           #
      $v_srch_ed_to_ss ne K_EMPTY
     ) {                                                        #
      $p_srch_h_ed_to_dt =                                      # Variable to store end timestamp (To)
        $v_srch_ed_to_yy . '-'                                  #
        . $v_srch_ed_to_mm . '-'                                #
        . $v_srch_ed_to_dd . ' '                                #
        . $v_srch_ed_to_hh . ':'                                #
        . $v_srch_ed_to_mi . ':'                                #
        . $v_srch_ed_to_ss                                      #
        ;
   } ## end if ( $v_srch_ed_to_yy ...)
   else {
      $p_srch_h_ed_to_dt = K_EMPTY ;                            # Variable to store end timestamp (From)
   }

   # Begin applicable timestamp - Display error if To timestamp earlier than From timestamp
   if (                                                         # Display error if to timestamp before from timestamp
      (                                                         #
         $v_srch_bg_from_yy * 10000000000 +                     #
         $v_srch_bg_from_mm * 100000000 +                       #
         $v_srch_bg_from_dd * 1000000 +                         #
         $v_srch_bg_from_hh * 10000 +                           #
         $v_srch_bg_from_mi * 100 +                             #
         $v_srch_bg_from_ss                                     #
      ) >                                                       #
      (                                                         #
         $v_srch_bg_to_yy * 10000000000 +                       #
           $v_srch_bg_to_mm * 100000000 +                       #
           $v_srch_bg_to_dd * 1000000 +                         #
           $v_srch_bg_to_hh * 10000 +                           #
           $v_srch_bg_to_mi * 100 +                             #
           $v_srch_bg_to_ss                                     #
      )                                                         #
     ) {                                                        #
      $v_srch_f_err         = K_YES ;                           # Error flag
      $v_srch_bg_dt_err_msg = $a_L[ 71 ] ;                      # Value: From timestamp is after to timestamp
   } ## end if (  (  $v_srch_bg_from_yy...))

   # End applicable timestamp - Display error if To timestamp earlier than From timestamp
   if (
      $v_f_ed_emp eq K_YES and                        # Display error if to timestamp before from timestamp
      ( (                                                       #
            $v_srch_ed_from_yy * 10000000000 +                  #
            $v_srch_ed_from_mm * 100000000 +                    #
            $v_srch_ed_from_dd * 1000000 +                      #
            $v_srch_ed_from_hh * 10000 +                        #
            $v_srch_ed_from_mi * 100 +                          #
            $v_srch_ed_from_ss                                  #
         ) >                                                    #
         (                                                      #
            $v_srch_ed_to_yy * 10000000000 +                    #
              $v_srch_ed_to_mm * 100000000 +                    #
              $v_srch_ed_to_dd * 1000000 +                      #
              $v_srch_ed_to_hh * 10000 +                        #
              $v_srch_ed_to_mi * 100 +                          #
              $v_srch_ed_to_ss                                  #
         )                                                      #
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;                                   # Error flag
      $v_srch_ed_dt_err_msg .= ' - ' . $a_L[ 71 ] ;             # Value: From timestamp is after to timestamp
   } ## end if ( ( (  $v_srch_ed_from_yy...)))

   $v_srch_bg_dt_err_msg =~ s/^\s-\s//g ;                       # Remove space hypen space
   $v_srch_ed_dt_err_msg =~ s/^\s-\s//g ;                       # Remove space hypen space

   if ( $v_srch_f_err eq K_YES ) {                              # STAGE 6 - Get out if error in main screen
      return ;
   }

   # STAGE 7 ####################################################

   # Display error if From timestamp of end timestamp earlier than From timestamp of start timestamp
   # End appicable timestamp - ( FROM ) earlier than Begin appicable timestamp - ( FROM )
   if (
      $v_f_ed_emp eq K_YES and                                  # Display error if To timestamp before From timestamp
      ( (
            $v_srch_ed_from_yy * 10000000000 +                  #
            $v_srch_ed_from_mm * 100000000 +                    #
            $v_srch_ed_from_dd * 1000000 +                      #
            $v_srch_ed_from_hh * 10000 +                        #
            $v_srch_ed_from_mi * 100 +                          #
            $v_srch_ed_from_ss
         ) <                                                    #
         (
            $v_srch_bg_from_yy * 10000000000 +                  #
              $v_srch_bg_from_mm * 100000000 +                  #
              $v_srch_bg_from_dd * 1000000 +                    #
              $v_srch_bg_from_hh * 10000 +                      #
              $v_srch_bg_from_mi * 100 +                        #
              $v_srch_bg_from_ss
         )
      )                                                         #
     ) {                                                        #
      $v_srch_f_err             = K_YES ;                       # Error flag
      $v_srch_job_bg_ed_err_msg = $a_L[ 105 ] ;                 # Value: End time from timestamp earlier than start time from timestamp
   } ## end if ( ( ( $v_srch_ed_from_yy...)))

   # Display error if To timestamp of end time earlier than To timestamp of start time
   # End appicable timestamp - ( TO ) earlier than Begin appicable timestamp - ( TO )
   if (
      $v_f_ed_emp eq K_YES and                                  # Display error if To timestamp before From timestamp
      ( (
            $v_srch_ed_to_yy * 10000000000 +                    #
            $v_srch_ed_to_mm * 100000000 +                      #
            $v_srch_ed_to_dd * 1000000 +                        #
            $v_srch_ed_to_hh * 10000 +                          #
            $v_srch_ed_to_mi * 100 +                            #
            $v_srch_ed_to_ss
         ) <                                                    #
         (
            $v_srch_bg_to_yy * 10000000000 +                    #
              $v_srch_bg_to_mm * 100000000 +                    #
              $v_srch_bg_to_dd * 1000000 +                      #
              $v_srch_bg_to_hh * 10000 +                        #
              $v_srch_bg_to_mi * 100 +                          #
              $v_srch_bg_to_ss
         )
      )                                                         #
     ) {
      $v_srch_f_err = K_YES ;
      $v_srch_job_bg_ed_err_msg .= ' - ' . $a_L[ 106 ] ;        # Value: End time to timestamp earlier than start time to timestamp
   } ## end if ( ( ( $v_srch_ed_to_yy...)))

   $v_srch_job_bg_ed_err_msg =~ s/^\s-\s//g ;                   # Remove space hypen space

   if ( $v_srch_f_err eq K_YES ) {                              # STAGE 7 - Get out if error in main screen
      return ;
   }

} ## end sub sCheckInput
##########################################################################
# End of subroutine sCheckInput                                          #
##########################################################################

sub sYearCorrection {                                           # Year correction of one digit, two digit

   # Called by: sCheckInput
   # Calls:

   my $vyc_in_yr = shift ;                                      # Take input

   my $vyc_last1_yr = substr ( $v_now_yy , -1 ) ;               # Get last 1 digit from year to adjust current year if one digit
   my $vyc_last2_yr = substr ( $v_now_yy , -2 ) ;               # Get last 2 digit from year to adjust current year if two digit

   if ( length ( $vyc_in_yr ) == 1 and $vyc_in_yr > $vyc_last1_yr ) {
      $vyc_in_yr = ( int ( $v_now_yy / 10 ) * 10 + $vyc_in_yr ) - 10 ;
   }
   elsif ( length ( $vyc_in_yr ) == 1 and $vyc_in_yr <= $vyc_last1_yr ) {
      $vyc_in_yr = int ( $v_now_yy / 10 ) * 10 + $vyc_in_yr ;
   }
   else { $vyc_in_yr = $vyc_in_yr ; }

   if ( length ( $vyc_in_yr ) == 2 and $vyc_in_yr > $vyc_last2_yr ) {
      $vyc_in_yr = ( int ( $v_now_yy / 100 ) * 100 + $vyc_in_yr ) - 100 ;
   }
   elsif ( length ( $vyc_in_yr ) == 2 and $vyc_in_yr <= $vyc_last2_yr ) {
      $vyc_in_yr = int ( $v_now_yy / 100 ) * 100 + $vyc_in_yr ;
   }
   else { $vyc_in_yr = $vyc_in_yr ; }

   if ( $vyc_in_yr < 1904 ) {
      $vyc_in_yr = 1904 ;                                       # Adjust year to 1904
   }

   return $vyc_in_yr ;
} ## end sub sYearCorrection
#######################################################################
# End of sYearCorrection                                              #
#######################################################################

sub sLastDayOfMonth {

   # Computes the maximum day of month, e.g. 31 for January
   # Input year and month

   # Called by: sCheckInput
   # Calls:

   my $vldom_yr = shift ;                                       # Input year
   my $vldom_mh = shift ;                                       # Input month

   if (
      ! ( defined ( $vldom_mh ) )                               #
      or $vldom_mh =~ /\D/                                      #
      or $vldom_mh < 1                                          #
      or $vldom_mh > 12
     ) {                                                        #
      die                                                       #
        "Month not numeric or between 1 and 12 while"           #
        . K_SPACE . "getting last day of month"                 #
        . ' : ' . $vldom_mh                                     #
        ;
   } ## end if ( ! ( defined ( $vldom_mh...)))

   if (
      $vldom_mh == 1 or $vldom_mh == 3 or $vldom_mh == 5  or    # Jan. Mar. May
      $vldom_mh == 7 or $vldom_mh == 8 or $vldom_mh == 10 or    # ... July, Aug. Oct.
      $vldom_mh == 12                                           # ... Dec.
     ) {
      return 31 ;
   } ## end if ( $vldom_mh == 1 or...)

   if (
      $vldom_mh == 4 or $vldom_mh == 6 or                       # Apr. June
      $vldom_mh == 9 or $vldom_mh == 11                         # Sep. Nov.
     ) {
      return 30 ;
   } ## end if ( $vldom_mh == 4 or...)

   if ( $vldom_mh != 2 ) {                                      # Display error if month is not february after eliminating all other valid months
      die                                                       #
        "Month not 2 after eliminating all other valid"         #
        . K_SPACE . "months while getting last day of month"    #
        . ' : ' . $vldom_mh ;
   } ## end if ( $vldom_mh != 2 )

   if ( $vldom_yr % 4 != K_ZERO ) { return 28 ; }               # Non leap year February - After this year divisible by 4

   if ( $vldom_yr % 100 != 0 ) { return 29 ; }                  # Leap year February - non century year

   if ( $vldom_yr % 400 == 0 ) { return 29 ; }                  # Leap year February - century year divisible by 400

   return 28 ;                                                  # Non leap year February - century not divisible by 400

} ## end sub sLastDayOfMonth
#####################################################################
# End of sLastDayOfMonth                                            #
#####################################################################

sub sSrhColor {

   # Returns Y when called the first ,  third ,  ... times ,  else returns n if even

   my $v_color = '' ;
   if ( $v_odd_count % 2 > 0 ) {
      $v_color = $I_PSEARCHEVENCL ;
   }                                                            # Odd line number
   else {
      $v_color = $I_PSEARCHODDCL ;                              # Even number
   }

   $v_odd_count ++ ;                                            # Increase ( line ) counter by 1
   return $v_color ;

} ## end sub sSrhColor
##########################################################################
# End of subroutine sSrhColor                                            #
##########################################################################

sub sLikeEqualTo {

   # If Exact match flag is on return = else like

   if ( $v_f_mtc_exa eq K_NO ) {
      return ( ' like ' ) ;
   }
   else {
      return ( ' = ' ) ;
   }

} ## end sub sLikeEqualTo
##########################################################################
# End of sLikeEqualTo                                                    #
##########################################################################

sub sWhereAndOr {

   # Starts with where clause if not used earlier else returns and / or

   if ( $v_srch_per_f_sql_where eq K_NO ) {
      $v_srch_per_f_sql_where = K_YES ;
      return ( ' where ' ) ;
   }
   else {
      if   ( $v_f_mtc_any eq 'Y' ) { return ( ' or ' ) ; }
      else                         { return ( ' and ' ) ; }
   }

} ## end sub sWhereAndOr
##########################################################################
# End of sWhereAndOr                                                     #
##########################################################################

sub sGetCount {

   #&sCheckInput ;
   # Subroutine to count of records in table as per query

   # Store parameters data into subroutine variable
   my $vgc_site_nm = $p_srch_site_nm ;

   my $vgc_bg_frm_yy = $v_srch_bg_from_yy ;
   my $vgc_bg_frm_mm = $v_srch_bg_from_mm ;
   my $vgc_bg_frm_dd = $v_srch_bg_from_dd ;
   my $vgc_bg_frm_hh = $v_srch_bg_from_hh ;
   my $vgc_bg_frm_mi = $v_srch_bg_from_mi ;
   my $vgc_bg_frm_ss = $v_srch_bg_from_ss ;

   my $vgc_bg_to_yy = $v_srch_bg_to_yy ;
   my $vgc_bg_to_mm = $v_srch_bg_to_mm ;
   my $vgc_bg_to_dd = $v_srch_bg_to_dd ;
   my $vgc_bg_to_hh = $v_srch_bg_to_hh ;
   my $vgc_bg_to_mi = $v_srch_bg_to_mi ;
   my $vgc_bg_to_ss = $v_srch_bg_to_ss ;

   my $vgc_ed_frm_yy = $v_srch_ed_from_yy ;
   my $vgc_ed_frm_mm = $v_srch_ed_from_mm ;
   my $vgc_ed_frm_dd = $v_srch_ed_from_dd ;
   my $vgc_ed_frm_hh = $v_srch_ed_from_hh ;
   my $vgc_ed_frm_mi = $v_srch_ed_from_mi ;
   my $vgc_ed_frm_ss = $v_srch_ed_from_ss ;

   my $vgc_ed_to_yy = $v_srch_ed_to_yy ;
   my $vgc_ed_to_mm = $v_srch_ed_to_mm ;
   my $vgc_ed_to_dd = $v_srch_ed_to_dd ;
   my $vgc_ed_to_hh = $v_srch_ed_to_hh ;
   my $vgc_ed_to_mi = $v_srch_ed_to_mi ;
   my $vgc_ed_to_ss = $v_srch_ed_to_ss ;

   my $vgc_f_ftp = $v_f_ftp ;
   my $vgc_f_ssl = $v_f_ssl ;
   my $vgc_tcpip = $p_srch_tcpip ;

   if ( $vgc_bg_frm_yy ne K_EMPTY and
        $vgc_bg_to_yy ne K_EMPTY ) {
     $v_bg_frm_ts .= $vgc_bg_frm_yy ;
     $v_bg_to_ts .= $vgc_bg_to_yy ;
     $v_bg_ts_fmt .="YYYY" ;
   }

   if ( $vgc_bg_frm_yy ne K_EMPTY and
        $vgc_bg_to_yy  ne K_EMPTY and
        $vgc_bg_frm_mm ne K_EMPTY and
        $vgc_bg_to_mm ne K_EMPTY ) {
     $v_bg_frm_ts .= $vgc_bg_frm_mm ;
     $v_bg_to_ts .= $vgc_bg_to_mm ;
     $v_bg_ts_fmt .="MM" ;
   }

   if ( $vgc_bg_frm_yy ne K_EMPTY and
        $vgc_bg_to_yy  ne K_EMPTY and
        $vgc_bg_frm_mm ne K_EMPTY and
        $vgc_bg_to_mm ne K_EMPTY and
        $vgc_bg_frm_dd ne K_EMPTY and
        $vgc_bg_to_dd ne K_EMPTY ) {
     $v_bg_frm_ts .= $vgc_bg_frm_dd ;
     $v_bg_to_ts .= $vgc_bg_to_dd ;
     $v_bg_ts_fmt .="DD" ;
   }

   if ( $vgc_bg_frm_yy ne K_EMPTY and
        $vgc_bg_to_yy  ne K_EMPTY and
        $vgc_bg_frm_mm ne K_EMPTY and
        $vgc_bg_to_mm ne K_EMPTY and
        $vgc_bg_frm_dd ne K_EMPTY and
        $vgc_bg_to_dd ne K_EMPTY and
        $vgc_bg_frm_hh ne K_EMPTY and
        $vgc_bg_to_hh ne K_EMPTY ) {
     $v_bg_frm_ts .= $vgc_bg_frm_hh ;
     $v_bg_to_ts .= $vgc_bg_to_hh ;
     $v_bg_ts_fmt .="HH24" ;
   }

   if ( $vgc_bg_frm_yy ne K_EMPTY and
        $vgc_bg_to_yy  ne K_EMPTY and
        $vgc_bg_frm_mm ne K_EMPTY and
        $vgc_bg_to_mm ne K_EMPTY and
        $vgc_bg_frm_dd ne K_EMPTY and
        $vgc_bg_to_dd ne K_EMPTY and
        $vgc_bg_frm_hh ne K_EMPTY and
        $vgc_bg_to_hh ne K_EMPTY and
        $vgc_bg_frm_mi ne K_EMPTY and
        $vgc_bg_to_mi ne K_EMPTY ) {
     $v_bg_frm_ts .= $vgc_bg_frm_mi ;
     $v_bg_to_ts .= $vgc_bg_to_mi ;
     $v_bg_ts_fmt .="MI" ;
   }

   if ( $vgc_bg_frm_yy ne K_EMPTY and
        $vgc_bg_to_yy  ne K_EMPTY and
        $vgc_bg_frm_mm ne K_EMPTY and
        $vgc_bg_to_mm ne K_EMPTY and
        $vgc_bg_frm_dd ne K_EMPTY and
        $vgc_bg_to_dd ne K_EMPTY and
        $vgc_bg_frm_hh ne K_EMPTY and
        $vgc_bg_to_hh ne K_EMPTY and
        $vgc_bg_frm_mi ne K_EMPTY and
        $vgc_bg_to_mi ne K_EMPTY and
        $vgc_bg_frm_ss ne K_EMPTY and
        $vgc_bg_to_ss ne K_EMPTY ) {
     $v_bg_frm_ts .= $vgc_bg_frm_ss ;
     $v_bg_to_ts .= $vgc_bg_to_ss ;
     $v_bg_ts_fmt .="SS" ;
   }

   if ( $vgc_ed_frm_yy ne K_EMPTY and
        $vgc_ed_to_yy ne K_EMPTY ) {
     $v_ed_frm_ts .= $vgc_ed_frm_yy ;
     $v_ed_to_ts  .= $vgc_ed_to_yy ;
     $v_ed_ts_fmt .="YYYY" ;
   }

   if ( $vgc_ed_frm_yy ne K_EMPTY and
        $vgc_ed_to_yy  ne K_EMPTY and
        $vgc_ed_frm_mm ne K_EMPTY and
        $vgc_ed_to_mm ne K_EMPTY ) {
     $v_ed_frm_ts .= $vgc_ed_frm_mm ;
     $v_ed_to_ts  .= $vgc_ed_to_mm ;
     $v_ed_ts_fmt .="MM" ;
   }

   if ( $vgc_ed_frm_yy ne K_EMPTY and
        $vgc_ed_to_yy  ne K_EMPTY and
        $vgc_ed_frm_mm ne K_EMPTY and
        $vgc_ed_to_mm ne K_EMPTY and
        $vgc_ed_frm_dd ne K_EMPTY and
        $vgc_ed_to_dd ne K_EMPTY ) {
     $v_ed_frm_ts .= $vgc_ed_frm_dd ;
     $v_ed_to_ts  .= $vgc_ed_to_dd ;
     $v_ed_ts_fmt .="DD" ;
   }

   if ( $vgc_ed_frm_yy ne K_EMPTY and
        $vgc_ed_to_yy  ne K_EMPTY and
        $vgc_ed_frm_mm ne K_EMPTY and
        $vgc_ed_to_mm ne K_EMPTY and
        $vgc_ed_frm_dd ne K_EMPTY and
        $vgc_ed_to_dd ne K_EMPTY and
        $vgc_ed_frm_hh ne K_EMPTY and
        $vgc_ed_to_hh ne K_EMPTY ) {
     $v_ed_frm_ts .= $vgc_ed_frm_hh ;
     $v_ed_to_ts  .= $vgc_ed_to_hh ;
     $v_ed_ts_fmt .="HH24" ;
   }

   if ( $vgc_ed_frm_yy ne K_EMPTY and
        $vgc_ed_to_yy  ne K_EMPTY and
        $vgc_ed_frm_mm ne K_EMPTY and
        $vgc_ed_to_mm ne K_EMPTY and
        $vgc_ed_frm_dd ne K_EMPTY and
        $vgc_ed_to_dd ne K_EMPTY and
        $vgc_ed_frm_hh ne K_EMPTY and
        $vgc_ed_to_hh ne K_EMPTY and
        $vgc_ed_frm_mi ne K_EMPTY and
        $vgc_ed_to_mi ne K_EMPTY ) {
     $v_ed_frm_ts .= $vgc_ed_frm_mi ;
     $v_ed_to_ts  .= $vgc_ed_to_mi ;
     $v_ed_ts_fmt .="MI" ;
   }

   if ( $vgc_ed_frm_yy ne K_EMPTY and
        $vgc_ed_to_yy  ne K_EMPTY and
        $vgc_ed_frm_mm ne K_EMPTY and
        $vgc_ed_to_mm ne K_EMPTY and
        $vgc_ed_frm_dd ne K_EMPTY and
        $vgc_ed_to_dd ne K_EMPTY and
        $vgc_ed_frm_hh ne K_EMPTY and
        $vgc_ed_to_hh ne K_EMPTY and
        $vgc_ed_frm_mi ne K_EMPTY and
        $vgc_ed_to_mi ne K_EMPTY and
        $vgc_ed_frm_ss ne K_EMPTY and
        $vgc_ed_to_ss ne K_EMPTY ) {
     $v_ed_frm_ts .= $vgc_ed_frm_ss ;
     $v_ed_to_ts  .= $vgc_ed_to_ss ;
     $v_ed_ts_fmt .="SS" ;
   }

   my $v_sql_where = K_EMPTY ;

   # Change fields with % at begin and end, if exact match flag is not Yes ( Y )
   if ( $v_f_mtc_exa ne K_YES ) {
      $vgc_site_nm = '%' . $vgc_site_nm . '%' ;

      #$vgc_bg_frm_yy = '%' . $vgc_bg_frm_yy . '%' ;
      #$vgc_bg_frm_mm = '%' . $vgc_bg_frm_mm . '%' ;
      #$vgc_bg_frm_dd = '%' . $vgc_bg_frm_dd . '%' ;
      #$vgc_bg_frm_hh = '%' . $vgc_bg_frm_hh . '%' ;
      #$vgc_bg_frm_mi = '%' . $vgc_bg_frm_mi . '%' ;
      #$vgc_bg_frm_ss = '%' . $vgc_bg_frm_ss . '%' ;
      #
      #$vgc_bg_to_yy = '%' . $vgc_bg_to_yy . '%' ;
      #$vgc_bg_to_mm = '%' . $vgc_bg_to_mm . '%' ;
      #$vgc_bg_to_dd = '%' . $vgc_bg_to_dd . '%' ;
      #$vgc_bg_to_hh = '%' . $vgc_bg_to_hh . '%' ;
      #$vgc_bg_to_mi = '%' . $vgc_bg_to_mi . '%' ;
      #$vgc_bg_to_ss = '%' . $vgc_bg_to_ss . '%' ;
      #
      #$vgc_ed_frm_yy  = '%' . $vgc_ed_frm_yy  . '%' ;
      #$vgc_ed_frm_mm  = '%' . $vgc_ed_frm_mm  . '%' ;
      #$vgc_ed_frm_dd  = '%' . $vgc_ed_frm_dd  . '%' ;
      #$vgc_ed_frm_hh  = '%' . $vgc_ed_frm_hh  . '%' ;
      #$vgc_ed_frm_mi  = '%' . $vgc_ed_frm_mi  . '%' ;
      #$vgc_ed_frm_ss  = '%' . $vgc_ed_frm_ss  . '%' ;
      #
      #$vgc_ed_to_yy  = '%' . $vgc_ed_to_yy  . '%' ;
      #$vgc_ed_to_mm  = '%' . $vgc_ed_to_mm  . '%' ;
      #$vgc_ed_to_dd  = '%' . $vgc_ed_to_dd  . '%' ;
      #$vgc_ed_to_hh  = '%' . $vgc_ed_to_hh  . '%' ;
      #$vgc_ed_to_mi  = '%' . $vgc_ed_to_mi  . '%' ;
      #$vgc_ed_to_ss  = '%' . $vgc_ed_to_ss  . '%' ;

      $vgc_f_ftp = '%' . $vgc_f_ftp . '%' ;
      $vgc_f_ssl = '%' . $vgc_f_ssl . '%' ;
      $vgc_tcpip = '%' . $vgc_tcpip . '%' ;

   } ## end if ( $v_f_mtc_exa ne K_YES)

   $vgc_site_nm =~ s/\%+/\%/g ;
   $vgc_tcpip =~ s/\%+/\%/g ;
   $vgc_site_nm =~ s/\_+/\_/g ;
   $vgc_tcpip =~ s/\_+/\_/g ;

   # Change fields to uppercase, if exact case flag is not Yes ( Y )
   if ( $v_f_mtc_cse ne K_YES ) {
      $vgc_site_nm = uc $vgc_site_nm ;
      #$vgc_f_ftp  = uc $vgc_f_ftp ;
      #$vgc_f_ssl  = uc $vgc_f_ssl ;
      $vgc_tcpip   = uc $vgc_tcpip ;
   } ## end if ( $v_f_mtc_cse ne K_YES)

   # From clause
   my $v_srch_per_sql_from .= "from" . K_SPACE . $I_PDBSCM . " . T_CTL_SAC" . K_SPACE ;

   # If Site number is not empty
   if ( $p_srch_site_nm ne K_EMPTY ) {
      $v_sql_where .= ( &sWhereAndOr ) . "SAC_SNO" . ( &sLikeEqualTo ) . "?" . K_SPACE ;
      push @a_srch_execute_values , $vgc_site_nm ;
   }

   if ( ( $vgc_bg_frm_yy ne K_EMPTY and $vgc_bg_to_yy ne K_EMPTY ) or
        ( $vgc_bg_frm_mm ne K_EMPTY and $vgc_bg_to_mm ne K_EMPTY ) or
        ( $vgc_bg_frm_dd ne K_EMPTY and $vgc_bg_to_dd ne K_EMPTY ) or
        ( $vgc_bg_frm_hh ne K_EMPTY and $vgc_bg_to_hh ne K_EMPTY ) or
        ( $vgc_bg_frm_mi ne K_EMPTY and $vgc_bg_to_mi ne K_EMPTY ) or
        ( $vgc_bg_frm_ss ne K_EMPTY and $vgc_bg_to_ss ne K_EMPTY ) ) {
    $v_sql_where .= ( &sWhereAndOr ) . "to_number" .K_SPACE . "(" . K_SPACE
       . "to_char" . K_SPACE  . "(" . K_SPACE
       . "SAC_BG_TS ," . K_SNG_APO . $v_bg_ts_fmt . K_SNG_APO . K_SPACE . ")" . K_SPACE . ")"
       . "between" . K_SPACE . $v_bg_frm_ts . K_SPACE
       . "and" . K_SPACE . $v_bg_to_ts ;
    push @a_srch_execute_values , $v_bg_ts_fmt ;
    push @a_srch_execute_values , $v_bg_frm_ts ;
    push @a_srch_execute_values , $v_bg_to_ts ;
    
   }

   if ( ( $vgc_ed_frm_yy ne K_EMPTY and $vgc_ed_to_yy ne K_EMPTY )or        
        ( $vgc_ed_frm_mm ne K_EMPTY and $vgc_ed_to_mm ne K_EMPTY )or
        ( $vgc_ed_frm_dd ne K_EMPTY and $vgc_ed_to_dd ne K_EMPTY )or
        ( $vgc_ed_frm_hh ne K_EMPTY and $vgc_ed_to_hh ne K_EMPTY )or
        ( $vgc_ed_frm_mi ne K_EMPTY and $vgc_ed_to_mi ne K_EMPTY )or
        ( $vgc_ed_frm_ss ne K_EMPTY and $vgc_ed_to_ss ne K_EMPTY ) ) {
    $v_sql_where .= ( &sWhereAndOr ) . "to_number" .K_SPACE . "(" . K_SPACE
       . "to_char" . K_SPACE  . "(" . K_SPACE
       . "SAC_ED_TS ," . K_SNG_APO . $v_ed_ts_fmt . K_SNG_APO . K_SPACE . ")" . K_SPACE . ")"
       . "between" . K_SPACE . $v_ed_frm_ts . K_SPACE
       . "and" . K_SPACE . $v_ed_to_ts ;
       
    push @a_srch_execute_values , $v_ed_ts_fmt ;
    push @a_srch_execute_values , $v_ed_frm_ts ;
    push @a_srch_execute_values , $v_ed_to_ts ;
   }

   #   select count(*) from ILCLP.T_CTL_SAC t
   #   where to_number ( to_char ( t.SAC_BG_TS , 'YYYYMM' ) ) between 201705 and 201705

   # If FTP flag is Y
   if ( $v_f_ftp eq K_YES ) {
      $v_sql_where .= ( &sWhereAndOr ) . "SAC_F_FTP" . ( &sLikeEqualTo ) . "?" . K_SPACE ;
      push @a_srch_execute_values , $vgc_f_ftp ;
   }

   # If SSL flag is Y
   if ( $v_f_ssl eq K_YES ) {
      $v_sql_where .= ( &sWhereAndOr ) . "SAC_F_SSL" . ( &sLikeEqualTo ) . "?" . K_SPACE ;
      push @a_srch_execute_values , $vgc_f_ssl ;
   }

   # If TCP/IP field is not empty
   if ( $p_srch_tcpip ne K_EMPTY ) {
      $v_sql_where .= ( &sWhereAndOr ) . "SAC_FTP_FRM_TCP" . ( &sLikeEqualTo ) . "?" . K_SPACE ;
      push @a_srch_execute_values , $vgc_tcpip ;
   }

   # Active - Yes / no
   if (                                                         #
      $p_srch_i_mtc_act eq K_I_YES or                                #
      $p_srch_i_mtc_act eq K_I_NO                                    #
     ) {                                                        #
      $v_sql_where .=                                           #
        ( &sWhereAndOr )                                               #
        . ( K_SPACE . "SAC_F_A" . K_SPACE )                     #
        . ( &sLikeEqualTo ) ;                                   #
      if ( $p_srch_i_mtc_act eq K_I_YES ) {                          #
         $v_sql_where .=                                        #
           "" . K_SPACE . K_SNG_APO . "Y" . K_SNG_APO . K_SPACE ;    #
      }                                                         #
      elsif ( $p_srch_i_mtc_act eq K_I_NO ) {                        #
         $v_sql_where .=                                        #
           "" . K_SPACE . K_SNG_APO . "n" . K_SNG_APO . K_SPACE ;    #
      }                                                         #
   }

   $vgc_sql =                                                   #
     'select count ( * )' . K_SPACE                             #
     . $v_srch_per_sql_from                                     #
     . $v_sql_where ;                                           #
                                                                #
                                                                #
   my $sth = $dbh -> prepare ( $vgc_sql ) ;                     #
   $sth -> execute ( @a_srch_execute_values ) ;                 #
   $v_count = $sth -> fetchrow || K_ZERO ;                      #
                                                                #
   if ( $v_count == 0 ) {
      $v_message = $a_L[ 73 ] ;                                 #
   }
   else {
      $v_message .= $a_L[ 74 ] . K_SPACE . ':' . K_SPACE . $v_count . K_SPACE ;
   }
   
   #return $v_count ;                                            #
} ## end sub sGetCount
##########################################################################
# End of subroutine sGetCount                                            #
##########################################################################