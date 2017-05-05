#!C:/Perl/bin/perl -w

# Application   : 44_503_MkeTag_100_1000.pl
# Client        : Internal
# Copyright (c) : IdentLogic Systems Private Limited
# Author        : Surendra Kadam
# Creation Date : 9 March 2017
# Description   : Make Tag is the procedure which generate tagged data format using input data
#                  Tagged data format is a method of formatting the inpu data when using
#                  ssan3_get_keys ,ssan3_get_ranges  and ssan3_match function calls.
#
#                 In Tagged Format, the offfset and lengths of the data fields being passed do
#                  not need to be specified. Instead, a notation of labels and delimiters is
#                  used to break up the fields.
#                 By default the delimiter is an asterisk but it can be user defined.
# WARNINGS      : There are special character which cannot separate fields. To avoid this problem
#                  i added backslash prefix to field separator or optional enclosing character
#                  within a procedure.
#
#                 PIPE
#                 ANCHOR
#                 BACKSLASH
#                 DOLLAR
#                 OPENSQUAREBRACKET
#                 QUESTION
#                 PLUS
#                 STAR
#                 STOP
#                 OPENBRACKET
#                 CLOSEBRACKET
#
#                 Here document does not supported while generating transource file
#                 it throws an error 'cant find String terminator'
#                 therefore I kept $v_print_file_encl_represntations inside START AND END KEEP

# HARD CODINGS  : Column 12   : Id
#                 Column 11   : Person_Name
#                 Column 10   : Address_Part1
#                 Column  9 ,
#                         8 ,
#                     and 7   : Address_Part2
#                 Column  5   : Postal_Area
#                 Column  4   : Telephone_Number
#                 Column  3   : Telephone_Number
#                 Column  2   : Telephone_Number
#                 Column  1
#                    and  0   : Attribute1

# Limitations   :
# Dependencies  :
# Modifications
# Date       Change Req# Author       Description
# ---------- ----------- ------------ -------------------------------

use strict ;
use warnings ;

use Getopt::Simple ;                                            # Get run parameter
use POSIX qw{strftime} ;                                        # For current date and time

use constant K_PACKAGE        => 44 ;                           # Package number
use constant K_PROCEDURE      => 503 ;                          # Procedure number
use constant K_PROCEDURE_NAME => "44_503_MkeTag_100_1000.pl" ;  # Procedure name
use constant K_PROCEDURE_DS   => "Make Tag" ;                   # Procedure description

use constant K_TAB_SPR  => "\t" ;                               # Tab separator
use constant K_NULL_SPR => "\0" ;                               # null terminator
use constant K_ZERO     => 0 ;                                  # Zero
use constant K_EMPTY    => "" ;                                 # Empty
use constant K_NEW_LN   => "\n" ;                               # New line
use constant K_SPACE    => " " ;                                # Space

# Run variables
my $pp_set_data        = K_EMPTY ;                              # Set data no customer
my $pp_run_data        = K_EMPTY ;                              # Run parameter
my $pp_in_file         = K_EMPTY ;                              # Input file
my $pp_fld_spr         = K_EMPTY ;                              # Field separator character
my $pp_enc_chr         = K_EMPTY ;                              # Optional field encloser character
my $pp_del_chr         = K_EMPTY ;                              # Optional delimiter character
my $pp_f_dt_1          = K_EMPTY ;                              # Flag if data starting from the the first record itself, or, first record has column headers (default)
my $pp_f_ct_c_dp       = K_EMPTY ;                              # Flag if display control character equivalences and stop
my $pp_multiplier      = K_EMPTY ;                              # Multiplier value parameter
my $pp_verbose         = K_EMPTY ;                              # Verbose flag parameter
my $pp_out_file_dir    = K_EMPTY ;                              # Output file directory
my $pp_org_file_dir    = K_EMPTY ;                              # Original data output file directory
my $pp_log_file_dir    = K_EMPTY ;                              # Log file directory
my $pp_bad_file_dir    = K_EMPTY ;                              # Not ok file directory

my $v_del_chr = K_EMPTY ;                                       # Store delimiter character to this variable
my $v_chk_del = K_EMPTY ;                                       # Check delimiter character is special character

my $v_log_file = K_EMPTY ;                                      # Stores log file name
my $v_bad_file = K_EMPTY ;                                      # Bad file name

my $v_out_file_dir    = K_EMPTY ;                               # Output file directory
my $v_org_file_dir    = K_EMPTY ;                               # Original data output file directory
my $v_log_file_dir    = K_EMPTY ;                               # Log file directory
my $v_bad_file_dir    = K_EMPTY ;                               # Not ok file directory

# Run statistics
my $v_rec_knt        = K_ZERO ;                                 # Records read count
my $v_err_no_id_knt  = K_ZERO ;                                 # Count of records without Id
my $v_err_knt        = K_ZERO ;                                 # Count of error records
my $v_err_ky_fld_knt = K_ZERO ;                                 # Count of records without any one or more key field of Person_Name , Organization_Name or Address_Part1
my $v_err_del_knt    = K_ZERO ;                                 # Record already contain delimiter which you are using now
my $v_err_sz_lmt_knt = K_ZERO ;                                 # Size upto limit count

# Work variable
my $v_rec = K_EMPTY ;                                           # Stores record read

# Work array
my @a_fld = () ;                                                # Contains individual fields of current record

# Time variables
my $v_now_ss = K_EMPTY ;                                        # Time elements - second
my $v_now_mi = K_EMPTY ;                                        # Minute
my $v_now_hh = K_EMPTY ;                                        # Hour - 24 hour format
my $v_now_dd = K_EMPTY ;                                        # Day
my $v_now_mm = K_EMPTY ;                                        # Month
my $v_now_yy = K_EMPTY ;                                        # Year

my $t_start_time = time ;                                       # Starting time
my $t_end_time   = K_ZERO ;                                     # End time

# Display control character and meta characters equivalences for field separator or
# optional enclosing character and stop

### START KEEP
my $v_print_file_encl_represntations = <<'FILE_ENCL';

DECIMAL CHARACTER

   0    NUL                   Null
   1    SOH                   Start of heading
   2    STX                   Start of text
   3    ETX                   End of text
   4    EOT                   End of transmission
   5    ENQ                   Enquiry
   6    ACK                   Aknowledge
   7    BEL                   Bell
   7    \a                    Bell
   8    BS                    Backspace
   8    \b                    Backspace
   9    TAB                   Tab
   9    \t                    Tab
  10    LF                    Line feed
  10    \n                    Line feed
  11    VT                    Vertical tab
  11    \v                    Vertical tab
  12    FF                    Form feed
  12    \f                    Form feed
  13    CR                    Carriage return
  13    \r                    Carriage return
  14    SO                    Shift out
  15    SI                    Shift in
  16    DLE                   Data link escape
  17    DC1                   Device control 1
  18    DC2                   Device control 2
  19    DC3                   Device control 3
  20    DC4                   Device control 4
  21    NAK                   Negative aknowledge
  22    SYN                   Synchronous idle
  23    ETB                   End of trans. block
  24    CAN                   Cancel
  25    em                    End of medium
  26    sub                   Substitude
  27    ESC                   Escape
  27    \e                    Escape
  28    FS                    File separator
  29    GS                    Group separator
  30    RS                    Record separator
  31    US                    Unit separator
  32    SPACE                 Space
  33    EXCLAMATION           !
  34    DOUBLEQUOTE           \"
  35    HASH                  Hash
  36    DOLLAR                $
  37    PERCENT               %
  38    AND                   &
  39    SINGLEQUOTE           \'
  40    OPENBRACKET           (
  41    CLOSEBRACKET          )
  42    STAR                  *
  43    PLUS                  +
  44    COMMA                 ,
  45    MINUS                 -
  46    STOP                  .
  47    FORWARDSLASH          /
  58    COLON                 :
  59    SEMICOLON             ;
  60    LESSTHAN              <
  61    EQUAL                 =
  62    GREATERTHAN           >
  63    QUESTION              ?
  64    AT                    @
  91    OPENSQUAREBRACKET     [
  92    BACKSLASH             \
  93    CLOSESQUAREBRACKET    ]
  94    ANCHOR                ^
  95    UNDERSCORE            _
  96    BACKTICK              `
 123    OPENCURLYBRAKCKET     {
 124    PIPE                  |
 124    "\|"                  PIPE
 125    CLOSECURLYBRACKET     }
 126    TILDE                 ~
 127    DEL
FILE_ENCL
### END KEEP
# End of $v_print_file_encl_represntations

# Hash containing the decimal ASCII value of the description of meta and control characters < 128

my %h_dec_ctrl_chr = (

   'NUL'                => 0 ,                                  # Null
   'SOH'                => 1 ,                                  # Start of heading
   'STX'                => 2 ,                                  # Start of text
   'ETX'                => 3 ,                                  # End of text
   'EOT'                => 4 ,                                  # End of transmission
   'ENQ'                => 5 ,                                  # Enquiry
   'ACK'                => 6 ,                                  # Aknowledge
   'BEL'                => 7 ,                                  # Bell
   '\a'                 => 7 ,                                  # Bell
   'BS'                 => 8 ,                                  # Backspace
   '\b'                 => 8 ,                                  # Backspace
   'TAB'                => 9 ,                                  # Tab
   '\t'                 => 9 ,                                  # Tab
   'LF'                 => 10 ,                                 # Line feed
   '\n'                 => 10 ,                                 # Line feed
   'VT'                 => 11 ,                                 # Vertical tab
   '\v'                 => 11 ,                                 # Vertical tab
   'FF'                 => 12 ,                                 # Form feed
   '\f'                 => 12 ,                                 # Form feed
   'CR'                 => 13 ,                                 # Carriage return
   '\r'                 => 13 ,                                 # Carriage return
   'SO'                 => 14 ,                                 # Shift out
   'SI'                 => 15 ,                                 # Shift in
   'DLE'                => 16 ,                                 # Data link escape
   'DC1'                => 17 ,                                 # Device control 1
   'DC2'                => 18 ,                                 # Device control 2
   'DC3'                => 19 ,                                 # Device control 3
   'DC4'                => 20 ,                                 # Device control 4
   'NAK'                => 21 ,                                 # Negative aknowledge
   'SYN'                => 22 ,                                 # Synchronous idle
   'ETB'                => 23 ,                                 # End of trans. block
   'CAN'                => 24 ,                                 # Cancel
   'em'                 => 25 ,                                 # End of medium
   'sub'                => 26 ,                                 # Substitude
   'ESC'                => 27 ,                                 # Escape
   '\e'                 => 27 ,                                 # Escape
   'FS'                 => 28 ,                                 # File separator
   'GS'                 => 29 ,                                 # Group separator
   'RS'                 => 30 ,                                 # Record separator
   'US'                 => 31 ,                                 # Unit separator
   'SPACE'              => 32 ,                                 # Space
   'EXCLAMATION'        => 33 ,                                 # !
   'DOUBLEQUOTE'        => 34 ,                                 # Double quote
   'HASH'               => 35 ,                                 # #
   'DOLLAR'             => 36 ,                                 # $
   'PERCENT'            => 37 ,                                 # %
   'AND'                => 38 ,                                 # &
   'SINGLEQUOTE'        => 39 ,                                 # Single quote
   'OPENBRACKET'        => 40 ,                                 # (
   'CLOSEBRACKET'       => 41 ,                                 # )
   'STAR'               => 42 ,                                 # *
   'PLUS'               => 43 ,                                 # +
   'COMMA'              => 44 ,                                 # ,
   'MINUS'              => 45 ,                                 # -
   'STOP'               => 46 ,                                 # .
   'FORWARDSLASH'       => 47 ,                                 # /
   'COLON'              => 58 ,                                 # :
   'SEMICOLON'          => 59 ,                                 # ;
   'LESSTHAN'           => 60 ,                                 # <
   'EQUAL'              => 61 ,                                 # =
   'GREATERTHAN'        => 62 ,                                 # >
   'QUESTION'           => 63 ,                                 # ?
   'AT'                 => 64 ,                                 # @
   'OPENSQUAREBRACKET'  => 91 ,                                 # [
   'BACKSLASH'          => 92 ,                                 # \
   'CLOSESQUAREBRACKET' => 93 ,                                 # ]
   'ANCHOR'             => 94 ,                                 # ^
   'UNDERSCORE'         => 95 ,                                 # _
   'BACKTICK'           => 96 ,                                 # `
   'OPENCURLYBRAKCKET'  => 123 ,                                # {
   'PIPE'               => 124 ,                                # |
   '\|'                 => 124 ,                                # | Pipe
   'CLOSECURLYBRACKET'  => 125 ,                                # }
   'TILDE'              => 126 ,                                # ~
   'DEL'                => 127                                  #

) ;

&sDateTime ;                                                    # Timestamp subroutine
&sGetParameter () ;                                             # Subroutine to get parameters

my $v_tag_file = $pp_set_data . $pp_run_data . '.tgt' ;         # Tag data file

my $v_org_dt_file = $pp_set_data . $pp_run_data . '.odx' ;      # Original data file

$v_log_file =                                                   # Log file
  $pp_set_data . $pp_run_data . '_'                             #
  . 'MkeTag' . '_'                                              #
  . $v_now_yy . '-'                                             #
  . $v_now_mm . '-'                                             #
  . $v_now_dd . '-'                                             #
  . $v_now_hh . '-'                                             #
  . $v_now_mi . '-'                                             #
  . $v_now_ss . '.log' ;                                        #

$v_bad_file =                                                   # Bad file
  $pp_set_data . $pp_run_data . '_'                             #
  . 'MkeTag' . '_'                                              #
  . 'BadFile' . '_'                                             #
  . $v_now_yy . '-'                                             #
  . $v_now_mm . '-'                                             #
  . $v_now_dd . '-'                                             #
  . $v_now_hh . '-'                                             #
  . $v_now_mi . '-'                                             #
  . $v_now_ss . '.NOK' ;                                        #

# Open and read input file
open ( my $IN_FILE1 , '<' , $pp_in_file ) or                    #
  die "Could not open input file $pp_in_file - $!\n" ;

# Open and write tag data output file
open ( my $OUTFILE , '>' , $v_out_file_dir . $v_tag_file ) or   #
  die "Could not open output file $v_tag_file  - $!\n" ;

# Open and write original data output file
open ( my $OUTORGFILE , '>' , $v_org_file_dir . $v_org_dt_file ) or    #
  die "Could not open original data output file $v_org_dt_file - $!\n" ;

# Open and write log data
open ( my $LOGFILE , '>' , $v_log_file_dir . $v_log_file ) or   #
  die "Can not open log file $v_log_file - $!\n" ;

# Open and write errors
open ( my $NOTOKFILE , '>' , $v_bad_file_dir . $v_bad_file ) or    #
  die "Can not open errors file $v_bad_file - $!\n" ;

# LOG #################################################################

my $v_st_ts = &sDateTime ;                                      # Start date and time

my $v_log =                                                     #
  "------ MAKE TAG EXECUTION START DATE AND TIME ------"        #
  . K_NEW_LN                                                    #
  . $v_st_ts                                                    #
  . K_NEW_LN . K_NEW_LN                                         #
  . "------ Run Parameters ------"                              #
  . K_NEW_LN                                                    #
  . "Data set number               :" . K_SPACE . $pp_set_data . K_NEW_LN    #
  . "Run time number               :" . K_SPACE . $pp_run_data . K_NEW_LN    #
  . "Delimiter                     :"                           #
  . K_SPACE
  . (
   $pp_del_chr eq K_EMPTY
   ? "Missing- Default:*"
   : ( $pp_del_chr =~ /[[:cntrl:]]/ ? "Ordinal (" . ord ( $pp_del_chr ) . ")" : $pp_del_chr )
  )
  . K_NEW_LN                                                    #
  . "Input file path               :" . K_SPACE . $pp_in_file . K_NEW_LN     #
  . (
   $pp_out_file_dir eq K_EMPTY
   ? K_EMPTY
   : "Output file directory         :" . K_SPACE . $pp_out_file_dir . K_NEW_LN
  )                                                             #
  . (
   $pp_org_file_dir eq K_EMPTY
   ? K_EMPTY
   : "Original data file directory  :" . K_SPACE . $pp_org_file_dir . K_NEW_LN
  )                                                             #
  . (
   $pp_log_file_dir eq K_EMPTY
   ? K_EMPTY
   : "Log file directory            :" . K_SPACE . $pp_log_file_dir . K_NEW_LN
  )                                                             #
  . (
   $pp_bad_file_dir eq K_EMPTY
   ? K_EMPTY
   : "NOT OK(Errors) file directory :" . K_SPACE . $pp_bad_file_dir . K_NEW_LN
  )                                                             #
  . "Field separator               :"
  . K_SPACE
  . ( $pp_fld_spr =~ /[[:cntrl:]]/ ? "Ordinal (" . ord ( $pp_fld_spr ) . ")" : $pp_fld_spr )
  . K_NEW_LN
  . (
   $pp_enc_chr eq K_EMPTY
   ? ""
   : "Optional encloser character   :" . K_SPACE                #
     . (
      $pp_enc_chr =~ /[[:cntrl:]]/ ?                            #
        "Ordinal (" . ord ( $pp_enc_chr ) . ")"                 #
        . K_NEW_LN
      : $pp_enc_chr . K_NEW_LN
     )                                                          #
  ) . "Flag -Data from 1st record    :"                         #
  . K_SPACE
  . ( $pp_f_dt_1 eq 1 ? "Yes" . K_NEW_LN : "no" . K_NEW_LN )
  . (
   $pp_verbose eq 1
   ? (
      $pp_multiplier eq K_EMPTY
      ? "Multiplier                    : Missing- Default:" . $pp_multiplier . K_NEW_LN
      : "Multiplier                    : " . $pp_multiplier . K_NEW_LN
     )
   : K_EMPTY
  )
  . "Verbose flag                  : "
  . ( $pp_verbose eq 1 ? "Yes" . K_NEW_LN : "no" . K_NEW_LN ) ;

my $v_files = K_NEW_LN . "------ File Names ------" . K_NEW_LN  #
  . "Input file name   :" . K_SPACE . $pp_in_file . K_NEW_LN    #
  . "Tag file name     :" . K_SPACE . $v_tag_file . K_NEW_LN    #
  . "Original data     :" . K_SPACE . $v_org_dt_file . K_NEW_LN #
  . "Log file name     :" . K_SPACE . $v_log_file . K_NEW_LN    #
  . "NOTOK file(Error) :" . K_SPACE . $v_bad_file . K_NEW_LN ;

print $LOGFILE $v_log ;
print $LOGFILE $v_files ;

# END LOG #############################################################

while ( $v_rec = <$IN_FILE1> ) {                                #

   chomp $v_rec ;                                               # Remove newlline from current record

   $v_rec_knt ++ ;                                              # Record counter

   # Verbose

   if ( $pp_verbose eq 1 ) {                                    # If verbose flag is on
      if ( $v_rec_knt == $pp_multiplier ) {
         $t_end_time = time - $t_start_time ;
         print "Display" . K_SPACE . $pp_multiplier . K_SPACE . "records in" . K_SPACE . $t_end_time . K_SPACE . "seconds to execute" . K_NEW_LN ;
         $pp_multiplier = $pp_multiplier * 2 ;
      }
   } ## end if ( $pp_verbose eq 1 )

   # If Field separator is PIPE added prefix backslash to field separator
   if ( $pp_fld_spr eq '|' ) {
      $pp_fld_spr = ( "\\" ) . $pp_fld_spr ;
   }

   # If Field separator is ANCHOR added prefix backslash to field separator
   if ( $pp_fld_spr eq '^' ) {
      $pp_fld_spr = ( "\\" ) . $pp_fld_spr ;
   }

   # If Field separator is BACKSLASH added prefix backslash to field separator
   if ( $pp_fld_spr eq '\\' ) {
      $pp_fld_spr = ( "\\" ) . $pp_fld_spr ;
   }

   # If Field separator is DOLLAR added prefix backslash to field separator
   if ( $pp_fld_spr eq '$' ) {
      $pp_fld_spr = ( "\\" ) . $pp_fld_spr ;
   }

   # If Field separator is OPENSQUAREBRACKET added prefix backslash to field separator
   if ( $pp_fld_spr eq '[' ) {
      $pp_fld_spr = ( "\\" ) . $pp_fld_spr ;
   }

   # If Field separator is QUESTION added prefix backslash to field separator
   if ( $pp_fld_spr eq '?' ) {
      $pp_fld_spr = ( "\\" ) . $pp_fld_spr ;
   }

   # If Field separator is PLUS added prefix backslash to field separator
   if ( $pp_fld_spr eq '+' ) {
      $pp_fld_spr = ( "\\" ) . $pp_fld_spr ;
   }

   # If Field separator is STAR added prefix backslash to field separator
   if ( $pp_fld_spr eq '*' ) {
      $pp_fld_spr = ( "\\" ) . $pp_fld_spr ;
   }

   # If Field separator is STOP added prefix backslash to field separator
   if ( $pp_fld_spr eq '.' ) {
      $pp_fld_spr = ( "\\" ) . $pp_fld_spr ;
   }

   # If Field separator is OPENBRACKET added prefix backslash to field separator
   if ( $pp_fld_spr eq '(' ) {
      $pp_fld_spr = ( "\\" ) . $pp_fld_spr ;
   }

   # If Field separator is CLOSEBRACKET added prefix backslash to field separator
   if ( $pp_fld_spr eq ')' ) {
      $pp_fld_spr = ( "\\" ) . $pp_fld_spr ;
   }

   #################################################################################################

   # If Optional field encloser character is PIPE added prefix backslash to Optional field encloser character
   if ( $pp_enc_chr eq '|' ) {
      $pp_enc_chr = ( "\\" ) . $pp_enc_chr ;
   }

   # If Optional field encloser character is ANCHOR added prefix backslash to Optional field encloser character
   if ( $pp_enc_chr eq '^' ) {
      $pp_enc_chr = ( "\\" ) . $pp_enc_chr ;
   }

   # If Optional field encloser character is BACKSLASH added prefix backslash to Optional field encloser character
   if ( $pp_enc_chr eq '\\' ) {
      $pp_enc_chr = ( "\\" ) . $pp_enc_chr ;
   }

   # If Optional field encloser character is DOLLAR added prefix backslash to Optional field encloser character
   if ( $pp_enc_chr eq '$' ) {
      $pp_enc_chr = ( "\\" ) . $pp_enc_chr ;
   }

   # If Optional field encloser character is OPENSQUAREBRACKET added prefix backslash to Optional field encloser character
   if ( $pp_enc_chr eq '[' ) {
      $pp_enc_chr = ( "\\" ) . $pp_enc_chr ;
   }

   # If Optional field encloser character is QUESTION added prefix backslash to Optional field encloser character
   if ( $pp_enc_chr eq '?' ) {
      $pp_enc_chr = ( "\\" ) . $pp_enc_chr ;
   }

   # If Optional field encloser character is PLUS added prefix backslash to Optional field encloser character
   if ( $pp_enc_chr eq '+' ) {
      $pp_enc_chr = ( "\\" ) . $pp_enc_chr ;
   }

   # If Optional field encloser character is STAR added prefix backslash to Optional field encloser character
   if ( $pp_enc_chr eq '*' ) {
      $pp_enc_chr = ( "\\" ) . $pp_enc_chr ;
   }

   # If Optional field encloser character is STOP added prefix backslash to Optional field encloser character
   if ( $pp_enc_chr eq '.' ) {
      $pp_enc_chr = ( "\\" ) . $pp_enc_chr ;
   }

   # If Optional field encloser character is OPENBRACKET added prefix backslash to Optional field encloser character
   if ( $pp_enc_chr eq '(' ) {
      $pp_enc_chr = ( "\\" ) . $pp_enc_chr ;
   }

   # If Optional field encloser character is CLOSEBRACKET added prefix backslash to Optional field encloser character
   if ( $pp_enc_chr eq ')' ) {
      $pp_enc_chr = ( "\\" ) . $pp_enc_chr ;
   }

   if ( $v_rec eq '' ) { next ; }                               # Skip empty record

   if ( $v_rec_knt == 1 and $pp_f_dt_1 eq 'n' ) { next ; }      # If first record contain Field label skip first record

   if ( $pp_del_chr eq '' ) {
      $v_del_chr = '*' ;                                        # Default delimiter character is *
   }
   else {
      $v_del_chr = $pp_del_chr ;                                # Optional delimiter single character taken from user
   }

   #################################################################################################

   # If delimiter character is PIPE added prefix backslash to delimiter character
   if ( $v_del_chr eq '|' ) {
      $v_chk_del = ( "\\" ) . $v_del_chr ;
   }

   # If delimiter character is ANCHOR added prefix backslash to delimiter character
   if ( $v_del_chr eq '^' ) {
      $v_chk_del = ( "\\" ) . $v_del_chr ;
   }

   # If delimiter character is BACKSLASH added prefix backslash to delimiter character
   if ( $v_del_chr eq '\\' ) {
      $v_chk_del = ( "\\" ) . $v_del_chr ;
   }

   # If delimiter character is DOLLAR added prefix backslash to delimiter character
   if ( $v_del_chr eq '$' ) {
      $v_chk_del = ( "\\" ) . $v_del_chr ;
   }

   # If delimiter character is OPENSQUAREBRACKET added prefix backslash to delimiter character
   if ( $v_del_chr eq '[' ) {
      $v_chk_del = ( "\\" ) . $v_del_chr ;
   }

   # If delimiter character is QUESTION added prefix backslash to delimiter character
   if ( $v_del_chr eq '?' ) {
      $v_chk_del = ( "\\" ) . $v_del_chr ;
   }

   # If delimiter character is PLUS added prefix backslash to delimiter character
   if ( $v_del_chr eq '+' ) {
      $v_chk_del = ( "\\" ) . $v_del_chr ;
   }

   # If delimiter character is STAR added prefix backslash to delimiter character
   if ( $v_del_chr eq '*' ) {
      $v_chk_del = ( "\\" ) . $v_del_chr ;
   }

   # If delimiter character is STOP added prefix backslash to delimiter character
   if ( $v_del_chr eq '.' ) {
      $v_chk_del = ( "\\" ) . $v_del_chr ;
   }

   # If delimiter character is OPENBRACKET added prefix backslash to delimiter character
   if ( $v_del_chr eq '(' ) {
      $v_chk_del = ( "\\" ) . $v_del_chr ;
   }

   # If delimiter character is CLOSEBRACKET added prefix backslash to delimiter character
   if ( $v_del_chr eq ')' ) {
      $v_chk_del = ( "\\" ) . $v_del_chr ;
   }
   #################################################################################################

   if ( $pp_del_chr eq '' ) {

      if ( $v_rec =~ /\*/ ) {                                   # Skip record if record already contain delimiter
         $v_err_del_knt ++ ;                                    # Record already contain delimiter which you are using now
         $v_err_knt ++ ;                                        # Count of error records

         print $NOTOKFILE "Record no : "
           . $v_rec_knt
           . K_SPACE
           . "Error Message : "
           . "Record already contain delimiter which you are using now !"
           . K_NEW_LN
           . "Record    : "
           . $v_rec
           . K_NEW_LN
           . K_NEW_LN ;

         next ;
      } ## end if ( $v_rec =~ /\*/ )
   } ##
   else {
      if (  $v_del_chr eq '|'
         || $v_del_chr eq '^'
         || $v_del_chr eq '\\'
         || $v_del_chr eq '$'
         || $v_del_chr eq '['
         || $v_del_chr eq '?'
         || $v_del_chr eq '+'
         || $v_del_chr eq '*'
         || $v_del_chr eq '.'
         || $v_del_chr eq '('
         || $v_del_chr eq ')' ) {                               # These are special delimiter characters

         if ( $v_rec =~ /$v_chk_del/ ) {                        # Skip record if record already contain delimiter
            $v_err_del_knt ++ ;                                 # Record already contain delimiter which you are using now
            $v_err_knt ++ ;                                     # Count of error records

            print $NOTOKFILE "Record no : "
              . $v_rec_knt
              . K_SPACE
              . "Error Message : "
              . "Record already contain delimiter which you are using now !"
              . K_NEW_LN
              . "Record    : "
              . $v_rec
              . K_NEW_LN
              . K_NEW_LN ;

            next ;
         } ## end if ( $v_rec =~ /$v_chk_del/)
      } ##
      else {
         if ( $v_rec =~ /$v_del_chr/ ) {                        # Skip record if record already contain delimiter
            $v_err_del_knt ++ ;                                 # Record already contain delimiter which you are using now
            $v_err_knt ++ ;                                     # Count of error records

            print $NOTOKFILE "Record no : "
              . $v_rec_knt
              . K_SPACE
              . "Error Message : "
              . "Record already contain delimiter which you are using now !"
              . K_NEW_LN
              . "Record    : "
              . $v_rec
              . K_NEW_LN
              . K_NEW_LN ;

            next ;
         } ## end if ( $v_rec =~ /$v_del_chr/)
      } ## end else
   } ## end else

   @a_fld = split $pp_fld_spr , $v_rec ;                        # Split record into fields

   for ( my $idx = 0 ; $idx <= $#a_fld ; $idx ++ ) {
      $a_fld[ $idx ] = $a_fld[ $idx ] || '' ;

      # Remove optional enclosure character if present in the begining and end
      if ( $a_fld[ $idx ] =~ /^$pp_enc_chr/ and $a_fld[ $idx ] =~ /$pp_enc_chr$/ ) {
         $a_fld[ $idx ] =~ s/^$pp_enc_chr(.*)$pp_enc_chr$/$1/ ;
      }

      $a_fld[ $idx ] =~ s/[[:cntrl:]]+/ /g ;                    # Replace control characters to space
      $a_fld[ $idx ] =~ s/p{C}/ /g ;                            # Replace unicode non printable character
      $a_fld[ $idx ] =~ s/\s\s+/ /g ;                           # Replace two or more spaces to single space
      $a_fld[ $idx ] =~ s/^\s+// ;                              # Trim leading space
      $a_fld[ $idx ] =~ s/\s+$// ;                              # Trim trailing space
   } ## end for ( my $idx = 0 ; $idx...)

   my $v_addp2 =
       ( $a_fld[ 9 ] eq '' ? '' : $a_fld[ 9 ] . ' ' )
     . ( $a_fld[ 8 ] eq '' ? '' : $a_fld[ 8 ] . ' ' )
     . ( $a_fld[ 7 ] eq '' ? '' : $a_fld[ 7 ] . ' ' )
     . ( $a_fld[ 5 ] eq '' ? '' : $a_fld[ 5 ] ) ;               # Address_Part2 is join of City ,Sub ditrict and District and post code

   $v_addp2 =~ s/^\s+|\s+$// ;                                  # Remove space if one of the field is empty

   if ( ! defined $a_fld[ 12 ] or ( $a_fld[ 12 ] eq K_EMPTY ) ) {    # Skip if no Id
      $v_err_no_id_knt ++ ;                                     # Count of records without Id
      $v_err_knt ++ ;                                           # Count of error records

      print $NOTOKFILE "Record no : " . $v_rec_knt . K_SPACE . "Error Message : " . "Missing id field !" . K_NEW_LN . "Record    : " . $v_rec . K_NEW_LN . K_NEW_LN ;

      next ;                                                    # Skip record
   } ## end if ( ! defined $a_fld[...])

   print $OUTORGFILE $a_fld[ 12 ] . ( $pp_fld_spr eq K_TAB_SPR ? K_NULL_SPR : K_TAB_SPR ) . $v_rec . "\n" ;

   if (
      $a_fld[ 11 ] eq '' and                                    # Person_Name field missing
      $a_fld[ 10 ] eq ''                                        # Address_Part1 field missing
     ) {
      $v_err_ky_fld_knt ++ ;                                    # Count of records without any one or more key field of Person_Name , Organization_Name or Address_Part1
      $v_err_knt ++ ;                                           # Count of error records

      print $NOTOKFILE "Record no : " . $v_rec_knt . K_SPACE . "Error Message : " . "Missing Person_Name and Address Part1 !" . K_NEW_LN . "Record    : " . $v_rec . K_NEW_LN . K_NEW_LN ;

      next ;                                                    # Skip record
   } ## end if

   # Length of the tag id must be less than or equal to 1000 characters
   if ( length $a_fld[ 12 ] > 1000 ) {
      print $NOTOKFILE "Record no : "
        . $v_rec_knt
        . K_SPACE
        . "Error Message : "
        . "Size of id field should not be greater than 1000 characters!"
        . K_NEW_LN
        . "Record    : "
        . $a_fld[ 12 ]
        . K_NEW_LN
        . K_NEW_LN ;

      $v_err_knt ++ ;                                           # Count of error records
      $v_err_sz_lmt_knt ++ ;                                    # Size of upto limit

      next ;                                                    # Skip record
   } ## end if ( length $a_fld[ 12...])

   # Length of the tag data must be less than or equal to 2000 characters
   if (
      length (
         $v_del_chr . 'Id' . $v_del_chr . $a_fld[ 12 ] .        #
           ( $a_fld[ 11 ] eq ''   ? '' : $v_del_chr . 'Person_Name' . $v_del_chr . $a_fld[ 11 ] )
           . ( $a_fld[ 10 ] eq '' ? '' : $v_del_chr . 'Address_Part1' . $v_del_chr . $a_fld[ 10 ] )
           . (
            ( $a_fld[ 9 ] eq '' and $a_fld[ 8 ] eq '' and $a_fld[ 7 ] eq '' )
            ? ''
            : $v_del_chr . 'Address_Part2' . $v_del_chr . $v_addp2
           )
           .                                                    # Address_Part2 is join of City ,Sub ditrict and District and post code
           ( $a_fld[ 5 ] eq ''   ? '' : $v_del_chr . 'Postal_Area' . $v_del_chr . $a_fld[ 5 ] ) .      #
           ( $a_fld[ 4 ] eq ''   ? '' : $v_del_chr . 'Telephone_Number' . $v_del_chr . $a_fld[ 4 ] )
           . ( $a_fld[ 3 ] eq '' ? '' : $v_del_chr . 'Telephone_Number' . $v_del_chr . $a_fld[ 3 ] )
           . ( $a_fld[ 2 ] eq '' ? '' : $v_del_chr . 'Telephone_Number' . $v_del_chr . $a_fld[ 2 ] )
           . ( $a_fld[ 1 ] eq '' ? '' : $v_del_chr . 'Attribute1' . $v_del_chr . $a_fld[ 1 ] )
           . ( $a_fld[ 0 ] eq '' ? '' : $v_del_chr . 'Attribute1' . $v_del_chr . $a_fld[ 0 ] )
           . $v_del_chr
           . $v_del_chr
           . $v_del_chr
      ) > 2000
     ) {

      print $NOTOKFILE "Record no : "
        . $v_rec_knt
        . K_SPACE
        . "Error Message : "
        . "Size of tag data should not be greater than 2000 characters!"
        . K_NEW_LN
        . "Record    : "
        . $v_del_chr . 'Id'
        . $v_del_chr
        . $a_fld[ 12 ]
        .                                                       #
        ( $a_fld[ 11 ] eq ''   ? '' : $v_del_chr . 'Person_Name' . $v_del_chr . $a_fld[ 11 ] )
        . ( $a_fld[ 10 ] eq '' ? '' : $v_del_chr . 'Address_Part1' . $v_del_chr . $a_fld[ 10 ] )
        . (
         ( $a_fld[ 9 ] eq '' and $a_fld[ 8 ] eq '' and $a_fld[ 7 ] eq '' )
         ? ''
         : $v_del_chr . 'Address_Part2' . $v_del_chr . $v_addp2
        )
        .                                                       # Address_Part2 is join of City ,Sub ditrict and District and post code
        ( $a_fld[ 5 ] eq ''   ? '' : $v_del_chr . 'Postal_Area' . $v_del_chr . $a_fld[ 5 ] ) .      #
        ( $a_fld[ 4 ] eq ''   ? '' : $v_del_chr . 'Telephone_Number' . $v_del_chr . $a_fld[ 4 ] )
        . ( $a_fld[ 3 ] eq '' ? '' : $v_del_chr . 'Telephone_Number' . $v_del_chr . $a_fld[ 3 ] )
        . ( $a_fld[ 2 ] eq '' ? '' : $v_del_chr . 'Telephone_Number' . $v_del_chr . $a_fld[ 2 ] )
        . ( $a_fld[ 1 ] eq '' ? '' : $v_del_chr . 'Attribute1' . $v_del_chr . $a_fld[ 1 ] )
        . ( $a_fld[ 0 ] eq '' ? '' : $v_del_chr . 'Attribute1' . $v_del_chr . $a_fld[ 0 ] )
        . $v_del_chr
        . $v_del_chr
        . $v_del_chr
        . K_NEW_LN
        . K_NEW_LN ;

      $v_err_knt ++ ;                                           # Count of error records
      $v_err_sz_lmt_knt ++ ;                                    # Size of upto limit

      next ;                                                    # Skip record
   } ## end if ( length ( $v_del_chr...))

   # Tag file output
   print $OUTFILE                                               #
     $a_fld[ 12 ] . K_TAB_SPR . $v_del_chr . 'Id' . $v_del_chr . $a_fld[ 12 ] .                     #
     ( $a_fld[ 11 ] eq ''   ? '' : $v_del_chr . 'Person_Name' . $v_del_chr . $a_fld[ 11 ] )
     . ( $a_fld[ 10 ] eq '' ? '' : $v_del_chr . 'Address_Part1' . $v_del_chr . $a_fld[ 10 ] )
     . (
      ( $a_fld[ 9 ] eq '' and $a_fld[ 8 ] eq '' and $a_fld[ 7 ] eq '' )
      ? ''
      : $v_del_chr . 'Address_Part2' . $v_del_chr . $v_addp2
     )
     .                                                          # Address_Part2 is join of City ,Sub ditrict and District and post code
     ( $a_fld[ 5 ] eq ''   ? '' : $v_del_chr . 'Postal_Area' . $v_del_chr . $a_fld[ 5 ] ) .         #
     ( $a_fld[ 4 ] eq ''   ? '' : $v_del_chr . 'Telephone_Number' . $v_del_chr . $a_fld[ 4 ] )
     . ( $a_fld[ 3 ] eq '' ? '' : $v_del_chr . 'Telephone_Number' . $v_del_chr . $a_fld[ 3 ] )
     . ( $a_fld[ 2 ] eq '' ? '' : $v_del_chr . 'Telephone_Number' . $v_del_chr . $a_fld[ 2 ] )
     . ( $a_fld[ 1 ] eq '' ? '' : $v_del_chr . 'Attribute1' . $v_del_chr . $a_fld[ 1 ] )
     . ( $a_fld[ 0 ] eq '' ? '' : $v_del_chr . 'Attribute1' . $v_del_chr . $a_fld[ 0 ] )
     . $v_del_chr
     . $v_del_chr
     . $v_del_chr
     . K_NEW_LN ;

} ## end while ( $v_rec = <$IN_FILE1>)
#######################################################################
# End of While                                                        #
#######################################################################

# Log #################################################################

print $LOGFILE K_NEW_LN . "------Run summary------" . K_NEW_LN ;
if ( $v_rec_knt != 0 ) {
   print                                                        #
     $LOGFILE K_NEW_LN                                          #
     . "No of records in a file "                               #
     . (
      $pp_f_dt_1 eq 'n' ?                                       #
        "with header : "
      :                                                         #
        "without header : "
     )                                                          #
     . &sCommify ( $v_rec_knt ) ;                               #

   print                                                        #
     K_NEW_LN                                                   #
     . "No of records in a file "                               #
     . (
      $pp_f_dt_1 eq 'n' ?                                       #
        "with header : "
      :                                                         #
        "without header : "
     )                                                          #
     . &sCommify ( $v_rec_knt ) ;                               #
} ## end if ( $v_rec_knt != 0 )

if ( $v_err_knt != 0 ) {                                        #
   print                                                        #
     $LOGFILE                                                   #
     K_NEW_LN . "No of error records                : "         #
     . &sCommify ( $v_err_knt ) ;                               #

   print                                                        #
     K_NEW_LN                                                   #
     . "No of error records                : "                  #
     . &sCommify ( $v_err_knt ) ;                               #
}                                                               #

if ( $v_err_no_id_knt != 0 ) {                                  #
   print                                                        #
     $LOGFILE                                                   #
     K_NEW_LN . " - Id field error                  : "         #
     . &sCommify ( $v_err_no_id_knt ) ;                         #

   print                                                        #
     K_NEW_LN . " - Id field error                  : "         #
     . &sCommify ( $v_err_no_id_knt ) ;                         #
} ## end if ( $v_err_no_id_knt ...)

if ( $v_err_ky_fld_knt != 0 ) {                                 #
   print                                                        #
     $LOGFILE K_NEW_LN                                          #
     . " - Key fields error                : "                  #
     . &sCommify ( $v_err_ky_fld_knt ) ;                        #

   print                                                        #
     K_NEW_LN . " - Key fields error                : "         #
     . &sCommify ( $v_err_ky_fld_knt ) ;                        #
}                                                               #

if ( $v_err_del_knt != 0 ) {                                    #
   print                                                        #
     $LOGFILE K_NEW_LN . " - Records found delimiter error   : "    #
     . &sCommify ( $v_err_del_knt ) ;                           #

   print                                                        #
     K_NEW_LN . " - Records found delimiter error   : "         #
     . &sCommify ( $v_err_del_knt ) ;                           #
}                                                               #

if ( $v_err_sz_lmt_knt != 0 ) {                                 #
   print                                                        #
     $LOGFILE K_NEW_LN . " - Records that exceeds size limit : "    #
     . &sCommify ( $v_err_sz_lmt_knt ) ;                        #

   print                                                        #
     K_NEW_LN . " - Records that exceeds size limit : "         #
     . &sCommify ( $v_err_sz_lmt_knt ) ;                        #
} ## end if ( $v_err_sz_lmt_knt...)

$t_end_time = time - $t_start_time ;                            # Total time taken

my $v_end_ts = &sDateTime ;                                     # End timestamp

print $LOGFILE K_NEW_LN . K_NEW_LN . "Ended " . K_SPACE . $v_end_ts . '-' . K_SPACE . strftime ( "\%H:\%M:\%S" , gmtime ( $t_end_time ) ) . K_SPACE . "to execute" . K_NEW_LN ;

# END LOG #############################################################

close $IN_FILE1 or                                              # Close input file
  die                                                           #
  "Could not close input file $pp_in_file - $!\n" ;

close $OUTFILE or                                               # Close tag data output file
  die "Could not close output Tag file $pp_set_data$pp_run_data.tag - $!\n" ;

close $OUTORGFILE or                                            # Close original data output file
  die                                                           #
  "Could not close original data output file $pp_set_data$pp_run_data.org - $!\n" ;

close $LOGFILE or                                               # Close log file
  die                                                           #
  "Can not close log file $v_log_file - $!\n" ;

close $NOTOKFILE or                                             # Close error file
  die                                                           #
  "Can not close log file $v_bad_file - $!\n" ;

# Delete file if there are no errors in a file
my $v_notok_file_size = -s $v_bad_file_dir . $v_bad_file || K_ZERO ;

if ( $v_notok_file_size < 1 ) {
   unlink $v_bad_file_dir . $v_bad_file ;
}

#######################################################################
# End of Main                                                         #
#######################################################################

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

sub sGetParameter {

   # Subroutine to get parameter and handle field separator and
   # optional enclosing character and delimiter character

   my ( $pp_GetOptions ) = {
      help => {                                                 # Help
         type    => '' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Transform dedupe input data to tagged format for set 100, run 1000, Tab \t- ouput file incurrent directory' ,
         order   => 1 ,
        } ,
      set_of_data => {                                          # Set data
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Set data number - 100 to 999' ,
         order   => 2 ,
        } ,
      run_number => {                                           # run data
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Run number - 1000 to 9999' ,
         order   => 3 ,
        } ,
      input_file_path => {                                      # Input file
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Input filepath name' ,
         order   => 4 ,
        } ,
      tag_file_dir => {                                         # Output file directory
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Output file directory' ,
         order   => 5 ,
        } ,
      org_data_file_dir => {                                    # Orginal data output file directory
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'OrIginal data output file directory' ,
         order   => 6 ,
        } ,
      log_file_dir => {                                         # Log file directory
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Log file directory' ,
         order   => 7 ,
        } ,
      bad_file_dir => {                                      # Not ok file directory
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Not ok file directory' ,
         order   => 8 ,
        } ,
      field_separator => {                                      # Pass field separator
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Field separator - for control characters use escaped value or term, e.g. \t or TAB for tab character' ,
         order   => 9 ,
        } ,
      encloser_character => {                                   # Optional enclosing separator
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Optional enclosing character - for control characters ' .    #
           'use escaped value or term or hexadecimal, ' .       #
           'e.g. \t or TAB or \011 or \x09 for tab character' ,
         order => 10 ,
        } ,
      delimiter_character => {                                  # Optional delimiter
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Optional delimiter character' ,
         order   => 11 ,
        } ,
      data_from_1_record_flag => {                              # Data starting from the first record instead of field labels in the first record
         type    => '!' ,
         env     => '-' ,
         default => '0' ,                                       # Default (parameter not mentioned) means field names in first record
         verbose => 'Data starting from the first record' ,
         order   => 12 ,
        } ,
      control_character_display => {                            # Display control character equivalences for field separator or optional enclosing character and stop
         type    => '!' ,
         env     => '-' ,
         default => '0' ,                                       # Default (parameter not mentioned) means script nuns normally
         verbose => 'Display control character equivalences for field separator or optional enclosing character and stop' ,
         order   => 13 ,
        } ,
      multiplier => {                                           # Multiplier parameter
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Optional multiplier parameter' ,
         order   => 14 ,
        } ,
      verbose => {                                              # Verbose flag
         type    => '!' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Verbose' ,
         order   => 15 ,
      } ,
   } ;

   my ( $parameters ) = Getopt::Simple -> new () ;              # variable for runtime parameters

   if ( ! $parameters -> getOptions ( $pp_GetOptions , 'Usage: File_Stat.pl [options]' ) ) {
      exit ( -1 ) ;                                             # Failure.
   }

   $pp_set_data   = $$parameters{ 'switch' }{ 'set_of_data' } ;
   $pp_run_data   = $$parameters{ 'switch' }{ 'run_number' } ;
   $pp_in_file    = $$parameters{ 'switch' }{ 'input_file_path' } ;
   $pp_fld_spr    = $$parameters{ 'switch' }{ 'field_separator' } ;
   $pp_enc_chr    = $$parameters{ 'switch' }{ 'encloser_character' } ;
   $pp_del_chr    = $$parameters{ 'switch' }{ 'delimiter_character' } ;
   $pp_f_dt_1     = $$parameters{ 'switch' }{ 'data_from_1_record_flag' } ;
   $pp_f_ct_c_dp  = $$parameters{ 'switch' }{ 'control_character_display' } ;                                                 # Flag if display control character equivalences and stop
   $pp_multiplier = $$parameters{ 'switch' }{ 'multiplier' } ;
   $pp_verbose    = $$parameters{ 'switch' }{ 'verbose' } ;

   $pp_out_file_dir    = $$parameters{ 'switch' }{ 'tag_file_dir' } ;
   $pp_org_file_dir    = $$parameters{ 'switch' }{ 'org_data_file_dir' } ;
   $pp_log_file_dir    = $$parameters{ 'switch' }{ 'log_file_dir' } ;
   $pp_bad_file_dir = $$parameters{ 'switch' }{ 'bad_file_dir' } ;

   # Check All directory path empty or not
   $v_out_file_dir    = ( $pp_out_file_dir eq K_EMPTY    ? './' : $pp_out_file_dir ) ;
   $v_org_file_dir    = ( $pp_org_file_dir eq K_EMPTY    ? './' : $pp_org_file_dir ) ;
   $v_log_file_dir    = ( $pp_log_file_dir eq K_EMPTY    ? './' : $pp_log_file_dir ) ;
   $v_bad_file_dir = ( $pp_bad_file_dir eq K_EMPTY ? './' : $pp_bad_file_dir ) ;

   if ( $v_out_file_dir ne './' ) {                             # If Output file directory not default (./)
      if (
         substr ( $v_out_file_dir , -1 , 1 ) ne '\\'            # If Output file directory not end with back slash (\\)
        ) {
         $v_out_file_dir .= '\\' ;                              # Add back slash at end
      }
   } ## end if ( $v_out_file_dir ne...)

   $v_out_file_dir =~ s/\\/\//g ;                               # Replace back slash to forward slash

   if ( $v_org_file_dir ne './' ) {                             # If Original data output file directory not default (./)
      if (
         substr ( $v_org_file_dir , -1 , 1 ) ne '\\'            # If Original data output file directory not end with back slash (\\)
        ) {
         $v_org_file_dir .= '\\' ;                              # Add back slash at end
      }
   } ## end if ( $v_org_file_dir ne...)

   $v_org_file_dir =~ s/\\/\//g ;                               # Replace back slash to forward slash

   if ( $v_log_file_dir ne './' ) {                             # If Log file directory not default (./)
      if (
         substr ( $v_log_file_dir , -1 , 1 ) ne '\\'            # If Log file directory not end with back slash (\\)
        ) {
         $v_log_file_dir .= '\\' ;                              # Add back slash at end
      }
   } ## end if ( $v_log_file_dir ne...)

   $v_log_file_dir =~ s/\\/\//g ;                               # Replace back slash to forward slash

   if ( $v_bad_file_dir ne './' ) {                             # If Not ok file(Errors) directory not default (./)
      if (
         substr ( $v_bad_file_dir , -1 , 1 ) ne '\\'            # If Not ok file(Errors) directory not end with back slash (\\)
        ) {
         $v_bad_file_dir .= '\\' ;                              # Add back slash at end
      }
   } ## end if ( $v_bad_file_dir...)

   $v_bad_file_dir =~ s/\\/\//g ;                               # Replace back slash to forward slash

   # Display control character equivalences and stop option chosen
   if ( $pp_f_ct_c_dp eq 1 ) {
      &sDpCtCStp ;
      exit 1 ;
   }

   if   ( $pp_f_dt_1 eq 1 ) { $pp_f_dt_1 = 'Y' ; }              # If data in first record transform flag to Y
   else                     { $pp_f_dt_1 = 'n' ; }              # If there are field name in first record flag set to n

   if ( $pp_set_data eq '' ) { die "JOB ABANDONDED - No set of data\n" ; }
   if ( $pp_run_data eq '' ) { die "JOB ABANDONDED - No run number\n" ; }
   if ( $pp_in_file eq '' )  { die "JOB ABANDONDED - No input file path\n" ; }
   if ( $pp_fld_spr eq '' )  { die "JOB ABANDONDED - No field separator character\n" ; }

   $pp_fld_spr = &sHdlFdSiOptEncl ( $pp_fld_spr , 'Field Separator' ) ;    # Handle field separator aliases

   if ( $pp_enc_chr ne "" ) {                                   # If enclosure character is non empty
      $pp_enc_chr = &sHdlFdSiOptEncl ( $pp_enc_chr , 'Optional Field Enclosure' ) ;    # Handle optional field enclosure aliases
   }

   if ( $pp_del_chr ne "" ) {                                   # If delimiter character is non empty
      $pp_del_chr = &sHdlFdSiOptEncl ( $pp_del_chr , 'Optional delimiter' ) ;          # Handle delimiter character alises
   }

   if ( $pp_set_data =~ /\D/ or $pp_set_data < 100 or $pp_set_data > 999 ) {
      die "JOB ABANDONDED - Data set number must be a number in the range of 100 to 999 instead of $pp_set_data\n" ;
   }

   if ( $pp_run_data =~ /\D/ or $pp_run_data < 1000 or $pp_run_data > 9999 ) {
      die "JOB ABANDONDED - Run number must be a number in the range of 1000 to 9999 instead of $pp_run_data\n" ;
   }

   # Field separator must contain only one character
   if ( length ( $pp_fld_spr ) > 1 ) {
      die "JOB ABANDONDED - Field separator >$pp_fld_spr< must contain one character\n" ;
   }

   # Optional enclosing separator contain only one character
   if ( length ( $pp_enc_chr ) > 1 ) {
      die "JOB ABANDONDED - Optional enclosing character >$pp_enc_chr< may contain only one character" ;
   }

   if ( $pp_del_chr ne "" ) {                                   # If optional delimiter character parameter is non empty
      if ( length ( $pp_del_chr ) > 1 ) {                       # Only one character
         die "JOB ABANDONDED - Delimiter character >$pp_del_chr< must contain one character\n" ;
      }
   }

   if ( $pp_multiplier eq K_EMPTY ) {
      $pp_multiplier = 100000 ;
   }

} ## end sub sGetParameter
#######################################################################
# End of sub sGetParameter                                            #
#######################################################################

sub sHdlFdSiOptEncl {

   # Handle field separator and optional enclosing character
   # If length of the character is more than one convert into upper case
   # and check if its alias control character is present in the hash value
   # If it is not there throw an error

   my $vhfsoe_c    = shift || '' ;                              # Capture input string
   my $vhfsoe_what = shift || '' ;                              # Capture input type

   if (   $vhfsoe_what ne 'Field Separator'
      and $vhfsoe_what ne 'Optional Field Enclosure'
      and $vhfsoe_what ne 'Optional delimiter' ) {
      die "$0: JOB ABANDONED: INTERNAL SCRIPT ERROR: " .        #
        "Parameter >$vhfsoe_what< not 'Field Separator' nor " . #
        "'Optional Field Enclosure'\n" ;
   } ## end if

   if (
      $vhfsoe_what eq 'Field Separator' and                     # Field separator must
      length ( $vhfsoe_c ) < 1
     ) {                                                        #
      die "$0: JOB ABANDONED: ERROR: Field separator MUST be specified\n" ;
   } ## end if

   if ( length ( $vhfsoe_c ) == 1 ) { return ( $vhfsoe_c ) ; }  # No need of further processing

   if ( length ( $vhfsoe_c ) > 1 ) {
      if (
         length ( $vhfsoe_c ) == 2 and                          # Convert to lower case if in form \c, where c = 1 character
         substr ( $vhfsoe_c , 0 , 1 ) eq "\\"                   # If field separator or enclosure begin with \
        ) {
         $vhfsoe_c = lc ( $vhfsoe_c ) ;                         # Lowercase string
      } ## end if ( length ( $vhfsoe_c...))
      else {
         $vhfsoe_c = uc ( $vhfsoe_c ) ;                         # Uppercase string
      }                                                         ## ... else make upper case

      if ( exists ( $h_dec_ctrl_chr{ $vhfsoe_c } ) ) {          # Process usual definition or escape of control characters
         return ( chr ( $h_dec_ctrl_chr{ $vhfsoe_c } ) ) ;
      }
   } ## end if ( length ( $vhfsoe_c...))

   # Unknown Field Separator or Optional Field Enclosure
   die "$0: JOB ABANDONED: ERROR: Unknown $vhfsoe_what >$vhfsoe_c<\n" ;

   return 1 ;

} ## end sub sHdlFdSiOptEncl
#######################################################################
# End of sub sHdlFdSiOptEncl                                          #
#######################################################################

sub sDpCtCStp {

   # Display control character and meta characters equivalences for field separator or
   # optional enclosing character and stop
   print $v_print_file_encl_represntations ;
   exit 1 ;

} ## end sub sDpCtCStp
#######################################################################
# End of sub sDpCtCStp                                                #
#######################################################################

sub sCommify {

   # Putting Commas in Numbers
   my $text = reverse $_[ 0 ] ;
   $text =~ s/(\d\d\d)(?=\d)(?!\d*\.)/$1,/g ;
   return scalar reverse $text ;
} ## end sub sCommify
#######################################################################
# End of sub sCommify                                                 #
#######################################################################

=pod

=head1 Make Tag

 Generate .tgt( tag data ) and .odx(original data) file using input file which
  are delimited by field separator.

=head2 Copyright

 Copyright (c) 2017 IdentLogic Systems Private Limited

=head2 Description

 Make Tag is the procedure which generate tagged data format using input data
 Tagged data format is a method of formatting the input data when using
 ssan3_get_keys ,ssan3_get_ranges  and ssan3_match function calls.

 In Tagged Format, the offfset and lengths of the data fields being passed do
  not need to be specified. Instead, a notation of labels and delimiters is
  used to break up the fields.
 By default the delimiter is an asterisk but it can be user defined.

=head2 Warning

 There are special controls and meta character which cannot separate fields and cannot directly
  use for delimiter.
  To avoid this problem i added backslash prefix to field separator or optional enclosing
  character within a procedure.

 PIPE
 ANCHOR
 BACKSLASH
 DOLLAR
 OPENSQUAREBRACKET
 QUESTION
 PLUS
 STAR
 STOP
 OPENBRACKET
 CLOSEBRACKET

 Here document does not supported while generating transource file
 it throws an error 'cant find String terminator'
 therefore I kept $v_print_file_encl_represntations inside START AND END KEEP

=head2 Input file

 Some changes applied to input file which are below:
 1. Replace control characters to space
 2. Replace two or more spaces to single space
 3. Trim leading space
 4. Trim trailing space
 5. Combined to fields City , Sub district and District

 Input file format

 # FIELD
 - -----
 Fields which are separated by field separator character

=head2 Output file format

 1 : sssrrrr.tgt

     Extension of output file is .tgt which are created with data set number and run number

     #   FIELD
     -   -----
     1   Id        - Length should not be greater than 1000 character ( Column 12 ) - TAB delimiter
     2   Tag data  - Length should not be greater than 2000 character

     At this time Tag data format are as follows :

     Column 12   : Id
     Column 11   : Person_Name
     Column 10   : Address_Part1
     Column  9 ,
             8 ,
         and 7   : Address_Part2
     Column  5   : Postal_Area
     Column  4   : Telephone_Number
     Column  3   : Telephone_Number
     Column  2   : Telephone_Number
     Column  1
        and  0   : Attribute1

 2 : sssrrrr.odx

     If field separator of input file are TAB delimiter
     then .odx file generated with \0 delimiter between id and fields

     and

     If field separator of input file are ( Any ) delimiter except tab delimiter
     then .odx file generated with \t delimiter between id and fields

     #   FIELD
     -   -----
     1   Id
     2   fields

=head2 Log file format

 ------ MAKE TAG EXECUTION START DATE AND TIME ------
 YYYY-MM-DD HH:MI:SS

  ------ Run Parameters ------
 Data set number               : Data set number
 Run time number               : Run time number
 Delimiter                     : Delimiter
 Input file path               : Input file path
 Output file directory         : Output file directory
 Original data file directory  : Original data output file directory
 Log file directory            : Log file directory
 NOT OK(Errors) file directory : NOT OK ( Errors ) file directory
 Field separator               : Field separator
 Optional encloser character   : Optional enclosing character
 Flag -Data from 1st record    : Yes/no
 Multiplier                    : Multiplier number
 Verbose flag                  : Yes/no

 ------ File Names ------
 Input file name   : <INPUT_FILE_NAME>
 Tag file name     : <OUTPUT_FILE_NAME>
 Original data     : <ORIGINAL_DATA_FILE_NAME>
 Log file name     : <LOG_FILE_NAME>
 NOTOK file(Error) : <NOTOK_FILE_NAME>

 No of records in a file with header / without header  : <COUNT>
 No of error records                : <COUNT>
  - Id field error                  : <COUNT>
  - Key fields error                : <COUNT>
  - Records found delimiter error   : <COUNT>
  - Records that exceeds size limit : <COUNT>

 Ended  YYYY-MM-DD HH:MI:SS- HH:MI:SS to execute


=head2 Terminal output

 No of records in a file with header / without header  : <COUNT>
 No of error records                : <COUNT>
  - Id field error                  : <COUNT>
  - Key fields error                : <COUNT>
  - Records found delimiter error   : <COUNT>
  - Records that exceeds size limit : <COUNT>

=head2 Delimiter

 1. If delimiter character is empty it will take default delimiter which is
     asterisk (*).

 2. You can also used delimiter character which is user defined using delimiter character
     parameter. If delimiter is contol or meta character check Display control character
     paramter using -c command you will get what to use for .

 3. If record in a input file found delimiter that is a error record

 4. Tagged data will not create where record found delimiter

=head2 Error Message

 Error occured if missing data set number , run number , input file , field separator
 Data set number must be integer and in a range of 100 to 999
 Run number must be integer and in a range of 1000 to 9999
 Field separator must contain one character
 Optional enclosing character may contain only one character
 Delimiter character must contain one character
 Unknown field separator error occured if wrong input given

 Handle field separator , optional enclosing character and Delimiter :
 - If length of the character is more than one convert into upper case
    and check if its alias control character is present in the hash value
   If it is not there throw an error

=head2 Technical

 Package Number   - 44
 Procedure Number - 503
 Procedure Name   - 44_503_MkeTag_100_1000.pl

=head3 Run parameters

 PARAMETER                  DESCRIPTION                           ABV    VARIABLE
 ---------                  ----------------------------------   -----   --------
 set_of_data                Set data number - 100 to 999          s      $pp_set_data
 run_number                 Run number - 1000 to 9999             r      $pp_run_data
 input_file_path            Input file path name                  i      $pp_in_file
 field_separator            Field separator - for control         f      $pp_fld_spr
                             characters use escaped value
                             or term, e.g. \t or TAB for
                             tab character
 encloser_character         Optional enclosing character -        e      $pp_enc_chr
                             for control characters and
                             meta characters
 delimiter_character        Optional delimiter character          del    $pp_del_chr
 data_from_1_record_flag    Data starting from the first record   data   $pp_f_dt_1
 control_character_display  Flag if display control character     c      $pp_f_ct_c_dp
 tag_file_dir               Tag file directory                    t      $pp_out_file_dir
 org_data_file_dir          Original data file directory          o      $pp_org_file_dir
 log_file_dir               Log file directory                    l      $pp_log_file_dir
 bad_file_dir               Not Ok file directory                 n      $pp_bad_file_dir
 multiplier                 Multiplier value                      m      $pp_multiplier
 verbose                    Verbose flag                          v      $pp_verbose

 ABV - Abbreviation for calling run parameter

=head3 Different examples of run parameter to run a procedure:

 perl 44_503_MkeTag_100_1000.pl -s 100 -r 1000 -i E:\SurendraK\MkeTag\INPUT\Xtract.txt
  -f TAB -t E:\SurendraK\MkeTag\TAG -o E:\SurendraK\MkeTag\ORG
  -l E:\SurendraK\MkeTag\LOG -b E:\SurendraK\MkeTag\NOTOK -m 1000 -v

 perl 44_503_MkeTag_100_1000.pl -s 100 -r 1000 -i input.txt -f TAB

 perl 44_503_MkeTag_100_1000.pl -s 100 -r 1001 -i input.txt -f PLUS -e DOUBLEQUOTE

 perl 44_503_MkeTag_100_1000.pl -s 100 -r 1002 -i input.txt -f CLOSEBRACKET

 perl 44_503_MkeTag_100_1000.pl -s 100 -r 1003 -i input.txt -f QUESTION -data

 perl 44_503_MkeTag_100_1000.pl -s 100 -r 1004 -i input.txt -f SEMICOLON  -del x

 perl 44_503_MkeTag_100_1000.pl -s 100 -r 1005 -i input.txt -f STOP

 perl 44_503_MkeTag_100_1000.pl -c

 -s , -r , -i and -f parameters are mandatory

=head4 Control character and meta characters

 Display control character and meta characters equivalences for field separator or
  optional enclosing character and stop

 NUL                => 0         Null
 SOH                => 1         Start of heading
 STX                => 2         Start of text
 ETX                => 3         End of text
 EOT                => 4         End of transmission
 ENQ                => 5         Enquiry
 ACK                => 6         Aknowledge
 BEL                => 7         Bell
 \a                 => 7         Bell
 BS                 => 8         Backspace
 \b                 => 8         Backspace
 TAB                => 9         Tab
 \t                 => 9         Tab
 LF                 => 10        Line feed
 \n                 => 10        Line feed
 VT                 => 11        Vertical tab
 \v                 => 11        Vertical tab
 FF                 => 12        Form feed
 \f                 => 12        Form feed
 CR                 => 13        Carriage return
 \r                 => 13        Carriage return
 SO                 => 14        Shift out
 SI                 => 15        Shift in
 DLE                => 16        Data link escape
 DC1                => 17        Device control 1
 DC2                => 18        Device control 2
 DC3                => 19        Device control 3
 DC4                => 20        Device control 4
 NAK                => 21        Negative aknowledge
 SYN                => 22        Synchronous idle
 ETB                => 23        End of trans. block
 CAN                => 24        Cancel
 em                 => 25        End of medium
 sub                => 26        Substitude
 ESC                => 27        Escape
 \e                 => 27        Escape
 FS                 => 28        File separator
 GS                 => 29        Group separator
 RS                 => 30        Record separator
 US                 => 31        Unit separator
 SPACE              => 32        Space
 EXCLAMATION        => 33        !
 DOUBLEQUOTE        => 34        Double quote
 HASH               => 35        #
 DOLLAR             => 36        $
 PERCENT            => 37        %
 AND                => 38        &
 SINGLEQUOTE        => 39        Single quote
 OPENBRACKET        => 40        (
 CLOSEBRACKET       => 41        )
 STAR               => 42        *
 PLUS               => 43        +
 COMMA              => 44        ,
 MINUS              => 45        -
 STOP               => 46        .
 FORWARDSLASH       => 47        /
 COLON              => 58        :
 SEMICOLON          => 59        ;
 LESSTHAN           => 60        <
 EQUAL              => 61        =
 GREATERTHAN        => 62        >
 QUESTION           => 63        ?
 AT                 => 64        @
 OPENSQUAREBRACKET  => 91        [
 BACKSLASH          => 92        \
 CLOSESQUAREBRACKET => 93        ]
 ANCHOR             => 94        ^
 UNDERSCORE         => 95        _
 BACKTICK           => 96        `
 OPENCURLYBRAKCKET  => 123       {
 PIPE               => 124       |
 "\|"               => 124       | Pipe
 CLOSECURLYBRACKET  => 125       }
 TILDE              => 126       ~
 DEL                => 127

=head4 Help and defaults

 For detailed help with defaults run: Perl <program_name> -h.

=head3 Perl modules used

 Getopt::Simple

=head3 Subroutines

 Subroutine      Description
 --------------  -----------------------------
 sDateTime       Get current date and time
 sGetParameter   Gets run parameters.
 sHdlFdSiOptEncl Handle field separator , optional enclosing character and delimiter character
 sDpCtCStp       Display control and meta character equivalences for field separator or
                 optional enclosing character and stop
 sCommify        Putting Commas in Numbers

=head4 Called by

 Subroutine     Called by
 -------------  ----------
 sDateTime         Main
 sGetParameter     Main
 sHdlFdSiOptEncl   sGetParameter
 sDpCtCStp         sGetParameter
 sCommify          Main

=head4 Structure

  Main
  |-- sDateTime
  |-- sGetParameter
  |   |-- sHdlFdSiOptEncl
  |    \-- sDpCtCStp
  \-- sCommify

=cut

#######################################################################
# End of 44_503_MkeTag_100_1000.pl                                    #
#######################################################################
