#!C:/Perl/bin/perl -w

# Application   : File_Stat.pl
# Client        : Internal
# Copyright (c) : IdentLogic Systems Private Limited
# Author        : Surendra Kadam
# Creation Date : 10 February 2017
# Description   : File Statistics will give you over all statistic report of a file

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
#
#                 Splits the string EXPR into a list of strings and returns that list. By default,
#                 empty leading fields are preserved and empty trailing ones are deleted. ( If all
#                 fields are empty, they are considered to be trailing.)

# HARD CODINGS  :

# Limitations   :
# Dependencies  :
# Modifications
# Date       Change Req# Author       Description
# ---------- ----------- ------------ -------------------------------

use strict ;
use warnings ;

use Getopt::Simple ;                                            # GetOpt paramters

# Run variables

my $p_fld_spr   = '' ;                                          # Field separator character
my $p_enc_chr   = '' ;                                          # Optional field encloser character
my $p_in_file   = '' ;                                          # Input file
my $p_f_dt_1    = '' ;                                          # Flag if data starting from the the first record itself, or, first record has column headers (default)
my $p_out_file  = '' ;                                          # Output file
my $p_f_ct_c_dp = '' ;                                          # Flag if display control character equivalences and stop
my $p_trc_chr   = '' ;                                          # Tracing character or characters

# Invalid characters count

my $v_rec_with_invalid_char_knt = 0 ;                           # Records with invalid characters

# Work variable

my $v_rec               = '' ;                                  # Stores record read
my $idx                 = '' ;                                  # Index to process each field
my $v_fld_typ           = '' ;                                  # Field type for currect field
my $v_0_1_rec_inval_chr = '' ;                                  # Indicator if record has invalid character
my $v_no_of_flds        = 0 ;                                   # Work variable to store number of fields in a record
my $v_fld_len           = 0 ;                                   # Length of field currenly being examined
my $vft_fld_typ         = '' ;                                  # work field type
my $vft_prv_fld_typ     = '' ;                                  # Previous field type determined
my $v_trc_chr           = '' ;                                  # Tracing character separated by comma

# Work arrays

my @a_fld     = () ;                                            # Contains individual fields of current record
my @a_fld_nm  = () ;                                            # Array field in first record
my @a_trc_chr = () ;                                            # Array contain tracing characters

# Result variables count

my $v_rec_knt         = 0 ;                                     # Records read count
my $v_rec_fld_typ_knt = 0 ;                                     # Records read count while determining field type

# Field wise counts - in hash

my %h_nll_knt            = () ;                                 # Count of null values per field number
my %h_fld_min            = () ;                                 # Minimum of not null field as per field number
my %h_rec_no_fld_min     = () ;                                 # Record no of minimum of not null field as per field number
my %h_fld_max            = () ;                                 # Maximum of not null field as per field number
my %h_rec_no_fld_max     = () ;                                 # Record no of maximum of not null field as per field number
my %h_fld_typ            = () ;                                 # Field type as per field number - in order: Integer , Signed integer , Decimal , Signed decimal , ASCII , Unicode , Other
my %h_fld_inval_chr_knt  = () ;                                 # Count of occurences of invalid character in field as per field number
my %h_cln_nll_knt        = () ;                                 # After cleaning count of null values per field number
my %h_no_of_occ          = () ;                                 # No of Occcurence of fields as per field number
my %h_fld_min_len        = () ;                                 # Minimum length of field as per field number
my %h_rec_no_fld_min_len = () ;                                 # Record no of minimum length of field as per field number
my %h_fld_max_len        = () ;                                 # Maximum length of field as per field number
my %h_rec_no_fld_max_len = () ;                                 # Record no of maxnimum length of field as per field number
my %h_encl_knt           = () ;                                 # Field wise count where optional enclosures used
my %h_bad_encl_knt       = () ;                                 # Field wise count where optional enclosure used only in begining or at end of field
my %h_unesc_encl_knt     = () ;                                 # Unescape enclosing character count as per field number
my %h_trc_chr_knt        = () ;                                 # Tracing character or characters as per field number count
my %h_val_email_chk_knt  = () ;                                 # Valid email count as per field number

# Maximum and minimum field count related variables

my $v_max_no_flds        = 0 ;                                  # Maximum number of fields
my $v_rec_no_max_no_flds = 0 ;                                  # Record no with maximum number of fields
my $v_min_no_flds        = 999_999_999_999 ;                    # Minimum number of fields (NON EMPTY OR NON FULL WHITESPACE RECORDS) - Arbitrarily high value
my $v_rec_no_min_no_flds = 0 ;                                  # Record no with minimum number of fields (NON EMPTY OR NON FULL WHITESPACE RECORDS)

# Maximum and minimum record length variables

my $v_rec_max_len     = 0 ;                                     # Maximum length of a record
my $v_rec_min_len     = 999_999_999_999 ;                       # Minumum length of a record. Set to an arbitrarily high value so that it reduces.
my $v_rec_no_max_len  = 0 ;                                     # Record no of maximum length
my $v_rec_no_min_len  = 0 ;                                     # Record no of minimum length
my $v_rec_empty_knt   = 0 ;                                     # Empty records count variable
my $v_rec_wht_spc_knt = 0 ;                                     # White space records

# Maximum field length related variables

my $v_fld_max_len        = 0 ;                                  # Maximum field length
my $v_rec_no_fld_max_len = 0 ;                                  # Record no with maximum field length

my $v_max_no_of_fld_rec = 0 ;                                   # Maximum no of fields in record
my $vgft_fld            = '' ;                                  # variable get field type

my $o_file_nm_1st = '' ;                                        # Output file name without extension
my $o_file_ext    = '' ;                                        # Output file extension

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

&sGetParameter () ;                                             # Subroutine to get parameters

# Open and read input file for field type determination

open ( my $IN_FILE1 , "<" , $p_in_file ) or                     #
  die "Could not open input file $p_in_file - $!\n" ;

while ( $v_rec = <$IN_FILE1> ) {                                # Get field types of all fields
   $v_rec_fld_typ_knt ++ ;                                      # Increment record read counter
   chomp $v_rec ;
   $v_rec = $v_rec . "\n" ;                                     # Add newline to record of all fields are empty.

   # If Field separator is PIPE added prefix backslash to field separator
   if ( $p_fld_spr eq '|' ) {
      $p_fld_spr = ( "\\" ) . $p_fld_spr ;
   }

   # If Field separator is ANCHOR added prefix backslash to field separator
   if ( $p_fld_spr eq '^' ) {
      $p_fld_spr = ( "\\" ) . $p_fld_spr ;
   }

   # If Field separator is BACKSLASH added prefix backslash to field separator
   if ( $p_fld_spr eq '\\' ) {
      $p_fld_spr = ( "\\" ) . $p_fld_spr ;
   }

   # If Field separator is DOLLAR added prefix backslash to field separator
   if ( $p_fld_spr eq '$' ) {
      $p_fld_spr = ( "\\" ) . $p_fld_spr ;
   }

   # If Field separator is OPENSQUAREBRACKET added prefix backslash to field separator
   if ( $p_fld_spr eq '[' ) {
      $p_fld_spr = ( "\\" ) . $p_fld_spr ;
   }

   # If Field separator is QUESTION added prefix backslash to field separator
   if ( $p_fld_spr eq '?' ) {
      $p_fld_spr = ( "\\" ) . $p_fld_spr ;
   }

   # If Field separator is PLUS added prefix backslash to field separator
   if ( $p_fld_spr eq '+' ) {
      $p_fld_spr = ( "\\" ) . $p_fld_spr ;
   }

   # If Field separator is STAR added prefix backslash to field separator
   if ( $p_fld_spr eq '*' ) {
      $p_fld_spr = ( "\\" ) . $p_fld_spr ;
   }

   # If Field separator is STOP added prefix backslash to field separator
   if ( $p_fld_spr eq '.' ) {
      $p_fld_spr = ( "\\" ) . $p_fld_spr ;
   }

   # If Field separator is OPENBRACKET added prefix backslash to field separator
   if ( $p_fld_spr eq '(' ) {
      $p_fld_spr = ( "\\" ) . $p_fld_spr ;
   }

   # If Field separator is CLOSEBRACKET added prefix backslash to field separator
   if ( $p_fld_spr eq ')' ) {
      $p_fld_spr = ( "\\" ) . $p_fld_spr ;
   }

   #########################################################################################################

   # If Optional field encloser character is PIPE added prefix backslash to Optional field encloser character
   if ( $p_enc_chr eq '|' ) {
      $p_enc_chr = ( "\\" ) . $p_enc_chr ;
   }

   # If Optional field encloser character is ANCHOR added prefix backslash to Optional field encloser character
   if ( $p_enc_chr eq '^' ) {
      $p_enc_chr = ( "\\" ) . $p_enc_chr ;
   }

   # If Optional field encloser character is BACKSLASH added prefix backslash to Optional field encloser character
   if ( $p_enc_chr eq '\\' ) {
      $p_enc_chr = ( "\\" ) . $p_enc_chr ;
   }

   # If Optional field encloser character is DOLLAR added prefix backslash to Optional field encloser character
   if ( $p_enc_chr eq '$' ) {
      $p_enc_chr = ( "\\" ) . $p_enc_chr ;
   }

   # If Optional field encloser character is OPENSQUAREBRACKET added prefix backslash to Optional field encloser character
   if ( $p_enc_chr eq '[' ) {
      $p_enc_chr = ( "\\" ) . $p_enc_chr ;
   }

   # If Optional field encloser character is QUESTION added prefix backslash to Optional field encloser character
   if ( $p_enc_chr eq '?' ) {
      $p_enc_chr = ( "\\" ) . $p_enc_chr ;
   }

   # If Optional field encloser character is PLUS added prefix backslash to Optional field encloser character
   if ( $p_enc_chr eq '+' ) {
      $p_enc_chr = ( "\\" ) . $p_enc_chr ;
   }

   # If Optional field encloser character is STAR added prefix backslash to Optional field encloser character
   if ( $p_enc_chr eq '*' ) {
      $p_enc_chr = ( "\\" ) . $p_enc_chr ;
   }

   # If Optional field encloser character is STOP added prefix backslash to Optional field encloser character
   if ( $p_enc_chr eq '.' ) {
      $p_enc_chr = ( "\\" ) . $p_enc_chr ;
   }

   # If Optional field encloser character is OPENBRACKET added prefix backslash to Optional field encloser character
   if ( $p_enc_chr eq '(' ) {
      $p_enc_chr = ( "\\" ) . $p_enc_chr ;
   }

   # If Optional field encloser character is CLOSEBRACKET added prefix backslash to Optional field encloser character
   if ( $p_enc_chr eq ')' ) {
      $p_enc_chr = ( "\\" ) . $p_enc_chr ;
   }

   @a_fld = split $p_fld_spr , $v_rec ;                         # Split record into fields

   # Maximum no of fields in record
   my $v_arr_len = $#a_fld + 1 ;
   if ( $v_arr_len > $v_max_no_of_fld_rec ) { $v_max_no_of_fld_rec = $v_arr_len ; }

   if (
      $v_rec ne '' and                                          # Get cleaned field names (header) if in first record and not empty
      $v_rec_fld_typ_knt == 1 and                               #
      $p_f_dt_1 eq "n"
     ) {

      for ( my $idx = 0 ; $idx < $v_max_no_of_fld_rec ; $idx ++ ) {
         $a_fld[ $idx ] =~ s/\n// ;                             # Remove new line from first record
         $a_fld_nm[ $idx ] = $a_fld[ $idx ] || "" ;
         $a_fld_nm[ $idx ] =~ s/[[:cntrl:]]+/ /g ;              # Replace control characters by space
         $a_fld_nm[ $idx ] =~ s/\s+/ /g ;                       # Replace multiple white space by space
         $a_fld_nm[ $idx ] =~ s/^\s(.*)\s$/$1/g ;               # Remove leading and trailing blanks
      } ## end for ( my $idx = 0 ; $idx...)
   }                                                            # end if
   else {
      &sGetFieldTypes () ;
   }
}                                                               # end while

close $IN_FILE1 or                                              #
  die                                                           #
  "Could not close input file while determining field type and optional field header $p_in_file - $!\n" ;

# If whole field is empty in assign null to hashes
if ( $v_max_no_of_fld_rec > 0 ) {
   for ( $idx = 0 ; $idx < $v_max_no_of_fld_rec ; $idx ++ ) {
      if ( ! exists ( $h_fld_typ{ $idx } ) ) {
         $h_fld_typ{ $idx }            = '' ;                   # Field type as per field number - in order: Integer , Signed integer , Decimal , Signed decimal , ASCII , Unicode , Other
         $h_nll_knt{ $idx }            = '' ;                   # Count of null values per field number
         $h_fld_min{ $idx }            = '' ;                   # Minimum of not null field as per field number
         $h_rec_no_fld_min{ $idx }     = '' ;                   # Record no of minimum of not null field
         $h_fld_max{ $idx }            = '' ;                   # Maximum of not null field as per field number
         $h_rec_no_fld_max{ $idx }     = '' ;                   # Record no of maximum of not null field
         $h_fld_inval_chr_knt{ $idx }  = '' ;                   # Count of occurences of invalid character in field
         $h_cln_nll_knt{ $idx }        = '' ;                   # After cleaning count of null values per field number
         $h_no_of_occ{ $idx }          = '' ;                   # No of Occcurence of fields
         $h_fld_min_len{ $idx }        = '' ;                   # Minimum length of field
         $h_rec_no_fld_min_len{ $idx } = '' ;                   # Record no of minimum length of field
         $h_fld_max_len{ $idx }        = '' ;                   # Maximum length of field
         $h_rec_no_fld_max_len{ $idx } = '' ;                   # Record no of maxnimum length of field
         $h_encl_knt{ $idx }           = '' ;                   # Field wise count where optional enclosures used
         $h_bad_encl_knt{ $idx }       = '' ;                   # Field wise count where optional enclosure used only in begining or at end of field
         $h_unesc_encl_knt{ $idx }     = '' ;                   # Unescape enclosing character count as per field number
         $h_trc_chr_knt{ $idx }        = '' ;                   # Tracing character or characters as per field number count
         $h_val_email_chk_knt{ $idx }  = '' ;                   # Count of valid email as per field number
      } ## end if ( ! exists ( $h_fld_typ...))
   } ## end for ( $idx = 0 ; $idx <...)
} ## end if ( $v_max_no_of_fld_rec...)

open ( my $IN_FILE , "<" , $p_in_file ) or                      #
  die "Could not open input file $p_in_file the second time - $!\n" ;

while ( $v_rec = <$IN_FILE> ) {

   chomp $v_rec ;
   $v_rec_knt ++ ;                                              # Record counter
   if ( $v_rec_knt == 1 and $p_f_dt_1 eq "n" ) { next ; }       # if first record contain Field label skip first record

   # Empty record or record with spaces

   if ( $v_rec eq '' or $v_rec =~ /^\s*$/ ) {
      $v_rec_empty_knt ++ ;                                     # If record is empty record counter incremented by 1
      next ;
   }

   @a_fld = split $p_fld_spr , $v_rec ;                         # Split record into fields

   $v_0_1_rec_inval_chr = 0 ;                                   # Set to zero - Number 0 or 1 if record has invalid character

   # Calculate maximum array length

   my $v_arr_len = $#a_fld + 1 ;
   if ( $v_arr_len > $v_max_no_of_fld_rec ) { $v_max_no_of_fld_rec = $v_arr_len ; }

   # Calculate Maximum and Minimum record length in all the records

   my $len = length ( $v_rec ) ;
   if ( $len > $v_rec_max_len ) {
      $v_rec_max_len    = $len ;                                # Check maximum record length
      $v_rec_no_max_len = $v_rec_knt ;                          # Record no of maximum record length
   }

   if ( $len < $v_rec_min_len ) {
      $v_rec_min_len    = $len ;                                # Check minimum record length
      $v_rec_no_min_len = $v_rec_knt ;                          # Record no of minimum record length
   }

   # Calculate maximum and minimum fields in all the records

   $v_no_of_flds = $#a_fld + 1 ;

   if ( $v_no_of_flds > $v_max_no_flds ) {                      # Check if maximum number of fields found
      $v_max_no_flds        = $v_no_of_flds ;
      $v_rec_no_max_no_flds = $v_rec_knt ;                      # Check record no of maximum number of fields
   }

   if ( $v_no_of_flds > 0 ) {
      if ( $v_no_of_flds < $v_min_no_flds ) {                   # Check if minimum number of fields found
         $v_min_no_flds        = $v_no_of_flds ;
         $v_rec_no_min_no_flds = $v_rec_knt ;                   # Check record no of minimum number of fields
      }
   } ## end if ( $v_no_of_flds > 0)

   for ( $idx = 0 ; $idx < $v_max_no_of_fld_rec ; $idx ++ ) {
      $a_fld[ $idx ] = $a_fld[ $idx ] || '' ;

      if ( $a_fld[ $idx ] eq '' ) {                             # Is field null
         if ( ! exists $h_nll_knt{ $idx } ) { $h_nll_knt{ $idx } = 1 ; }
         else                               { $h_nll_knt{ $idx }++ ; }
      }
      else {
         if ( ! exists $h_nll_knt{ $idx } ) { $h_nll_knt{ $idx } = "" ; }
      }

      if ( $a_fld[ $idx ] =~ /[[:cntrl:]]/ ) {                  # Field has invalid character(s)
         $v_0_1_rec_inval_chr = 1 ;
         if ( ! exists $h_fld_inval_chr_knt{ $idx } ) {
            $h_fld_inval_chr_knt{ $idx } = 1 ;
         }
         else {
            $h_fld_inval_chr_knt{ $idx }++ ;
         }
         $a_fld[ $idx ] =~ s/[[:cntrl:]]//g ;                   # Remove invalid character(s) in field
      } ## end if ( $a_fld[ $idx ] =~...)
      else {
         if ( ! exists $h_fld_inval_chr_knt{ $idx } ) { $h_fld_inval_chr_knt{ $idx } = "" ; }
      }

      if ( $a_fld[ $idx ] eq '' ) {                             # Is field null after cleaning
         if ( ! exists $h_cln_nll_knt{ $idx } ) { $h_cln_nll_knt{ $idx } = 1 ; }
         else                                   { $h_cln_nll_knt{ $idx }++ ; }
         next ;
      }

      # If type is ASCII OR UNICODE
      if (  $h_fld_typ{ $idx } eq "ASCII"
         or $h_fld_typ{ $idx } eq "UNICODE"
         or $h_fld_typ{ $idx } eq "Unknown" ) {

         # Check for minimum value of field

         if ( ! exists $h_fld_min{ $idx } ) {
            $h_fld_min{ $idx }        = $a_fld[ $idx ] ;
            $h_rec_no_fld_min{ $idx } = $v_rec_knt ;            # Record no of minimum value of each field
         }
         else {
            if ( $h_fld_min{ $idx } gt "" ) {
               if ( $a_fld[ $idx ] lt $h_fld_min{ $idx } ) {
                  $h_fld_min{ $idx }        = $a_fld[ $idx ] ;
                  $h_rec_no_fld_min{ $idx } = $v_rec_knt ;      # Record no of minimum value of each field
               }
            } ## end if ( $h_fld_min{ $idx ...})
         } ## end else [ if ( ! exists $h_fld_min...)]

         # Check for maximum value of field

         if ( ! exists $h_fld_max{ $idx } ) {
            $h_fld_max{ $idx }        = $a_fld[ $idx ] ;
            $h_rec_no_fld_max{ $idx } = $v_rec_knt ;            # Record no of maximum value of each field
         }
         else {
            if ( $a_fld[ $idx ] gt $h_fld_max{ $idx } ) {
               $h_fld_max{ $idx }        = $a_fld[ $idx ] ;
               $h_rec_no_fld_max{ $idx } = $v_rec_knt ;         # Record no of maximum value of each field
            }

         } ## end else [ if ( ! exists $h_fld_max...)]

      } ## end if ( $h_fld_typ{ $idx ...})

      # If type is a Integer, Decimal, Signed integer, Signed decimal
      if (  ( $h_fld_typ{ $idx } eq "Integer" )
         or ( $h_fld_typ{ $idx } eq "Signed integer" )
         or ( $h_fld_typ{ $idx } eq "Decimal" )
         or ( $h_fld_typ{ $idx } eq "Signed decimal" ) ) {

         # Check for minimum value of field

         if ( ! exists $h_fld_min{ $idx } ) {
            $h_fld_min{ $idx }        = $a_fld[ $idx ] ;
            $h_rec_no_fld_min{ $idx } = $v_rec_knt ;            # Record no of minimum value of each field
         }
         else {
            if ( $h_fld_min{ $idx } > 0 ) {
               if ( $a_fld[ $idx ] < $h_fld_min{ $idx } ) {
                  $h_fld_min{ $idx }        = $a_fld[ $idx ] ;
                  $h_rec_no_fld_min{ $idx } = $v_rec_knt ;      # Record no of minimum value of each field
               }
            } ## end if ( $h_fld_min{ $idx ...})
         } ## end else [ if ( ! exists $h_fld_min...)]

         # Check for maximum value of field

         if ( ! exists $h_fld_max{ $idx } ) {
            $h_fld_max{ $idx }        = $a_fld[ $idx ] ;
            $h_rec_no_fld_max{ $idx } = $v_rec_knt ;            # Record no of maximum value of each field
         }
         else {
            if ( $a_fld[ $idx ] > $h_fld_max{ $idx } ) {
               $h_fld_max{ $idx }        = $a_fld[ $idx ] ;
               $h_rec_no_fld_max{ $idx } = $v_rec_knt ;         # Record no of maximum value of each field
            }
         } ## end else [ if ( ! exists $h_fld_max...)]
      } ## end if ( ( $h_fld_typ{ $idx...}))

      # Check for minimum length of field

      if ( ! exists $h_fld_min_len{ $idx } ) {
         $h_fld_min_len{ $idx }        = length ( $a_fld[ $idx ] ) ;
         $h_rec_no_fld_min_len{ $idx } = $v_rec_knt ;           # Record count
      }
      else {
         if ( length ( $a_fld[ $idx ] ) < $h_fld_min_len{ $idx } ) {
            $h_fld_min_len{ $idx }        = length ( $a_fld[ $idx ] ) ;
            $h_rec_no_fld_min_len{ $idx } = $v_rec_knt ;        # Record no of each minimum length of field
         }
      } ## end else [ if ( ! exists $h_fld_min_len...)]

      # Check for maximum length of field

      if ( ! exists $h_fld_max_len{ $idx } ) {
         $h_fld_max_len{ $idx }        = length ( $a_fld[ $idx ] ) ;
         $h_rec_no_fld_max_len{ $idx } = $v_rec_knt ;
      }
      else {
         if ( length ( $a_fld[ $idx ] ) > $h_fld_max_len{ $idx } ) {
            $h_fld_max_len{ $idx }        = length ( $a_fld[ $idx ] ) ;
            $h_rec_no_fld_max_len{ $idx } = $v_rec_knt ;        # Record no of each maximum length of field
         }
      } ## end else [ if ( ! exists $h_fld_max_len...)]

      # No of occurence in fields

      if ( ! exists $h_no_of_occ{ $idx } ) {
         $h_no_of_occ{ $idx } = 1 ;
      }
      else {
         $h_no_of_occ{ $idx }++ ;
      }

      # Check if field is enclosed with Optional enclosing character

      if ( $p_enc_chr ne "" ) {
         if ( $a_fld[ $idx ] =~ /^$p_enc_chr.*$p_enc_chr$/ ) {
            if ( ! exists $h_encl_knt{ $idx } ) { $h_encl_knt{ $idx } = 1 ; }
            else                                { $h_encl_knt{ $idx }++ ; }
         }
         else {
            if ( ! exists $h_encl_knt{ $idx } ) { $h_encl_knt{ $idx } = "" ; }
         }
      } ## end if

      # Bad enclosure count

      if ( $p_enc_chr ne "" ) {
         if ( ( $a_fld[ $idx ] =~ /^$p_enc_chr/ ) and ( $a_fld[ $idx ] !~ /$p_enc_chr$/ ) ) {
            $a_fld[ $idx ] = $a_fld[ $idx ] . $p_enc_chr ;
            if ( ! exists $h_bad_encl_knt{ $idx } ) { $h_bad_encl_knt{ $idx } = 1 ; }
            else                                    { $h_bad_encl_knt{ $idx }++ ; }

         } ## end if ( ( $a_fld[ $idx ] ...))
         else {
            if ( ! exists $h_bad_encl_knt{ $idx } ) { $h_bad_encl_knt{ $idx } = "" ; }
         }

         if ( $a_fld[ $idx ] !~ /^$p_enc_chr/ and $a_fld[ $idx ] =~ /$p_enc_chr$/ ) {
            $a_fld[ $idx ] = $p_enc_chr . $a_fld[ $idx ] ;
            if ( ! exists $h_bad_encl_knt{ $idx } ) { $h_bad_encl_knt{ $idx } = 1 ; }
            else                                    { $h_bad_encl_knt{ $idx }++ ; }

         } ## end if ( $a_fld[ $idx ] !~...)
         else {
            if ( ! exists $h_bad_encl_knt{ $idx } ) { $h_bad_encl_knt{ $idx } = "" ; }
         }
      } ## end if

      # Unescape enclosing character count as per field number

      if ( $p_enc_chr ne "" ) {
         $a_fld[ $idx ] =~ s/^$p_enc_chr// ;                    # Remove leading enclosing character
         $a_fld[ $idx ] =~ s/$p_enc_chr$// ;                    # Remove trailing enclosing character
         $a_fld[ $idx ] =~ s/$p_enc_chr$p_enc_chr//g ;          # Remove pair enclosing character

         if ( $a_fld[ $idx ] =~ /$p_enc_chr/ ) {                # Check field contain enclosing character
            if ( ! exists $h_unesc_encl_knt{ $idx } ) { $h_unesc_encl_knt{ $idx } = 1 ; }
            else                                      { $h_unesc_encl_knt{ $idx }++ ; }

         }
         else {
            if ( ! exists $h_unesc_encl_knt{ $idx } ) { $h_unesc_encl_knt{ $idx } = "" ; }
         }
      } ## end if

      # Tracing character or characters count as per field number

      if ( $p_trc_chr ne "" ) {
         if ( $a_fld[ $idx ] =~ /[$p_trc_chr]/ ) {
            if ( ! exists $h_trc_chr_knt{ $idx } ) { $h_trc_chr_knt{ $idx } = 1 ; }
            else                                   { $h_trc_chr_knt{ $idx }++ ; }
         }
         else {
            if ( ! exists $h_trc_chr_knt{ $idx } ) { $h_trc_chr_knt{ $idx } = "" ; }
         }
      } ## end if

      # Count of valid email address

      if ( $a_fld[ $idx ] ne "" ) {                             # Match email format
         if ( $a_fld[ $idx ] =~ /^(([a-zA-Z0-9_\-\.]+)@([a-zA-Z0-9_\-\.]+)\.([a-zA-Z]{1,5}){1,25})+([, ](([a-zA-Z0-9_\-\.]+)@([a-zA-Z0-9_\-\.]+)\.([a-zA-Z]{1,5}){1,25})+)*$/ ) {

            if ( ! exists $h_val_email_chk_knt{ $idx } ) { $h_val_email_chk_knt{ $idx } = 1 ; }
            else                                         { $h_val_email_chk_knt{ $idx }++ ; }
         }
         else {
            if ( ! exists $h_val_email_chk_knt{ $idx } ) { $h_val_email_chk_knt{ $idx } = "" ; }
         }
      } ## end if

   } ## end for ( $idx = 0 ; $idx <...)

   $v_rec_with_invalid_char_knt = $v_rec_with_invalid_char_knt + $v_0_1_rec_inval_chr ;    # Records with invalid characters

} ## end while ( $v_rec = <$IN_FILE>)

# If header in the first record contain empty or null values then assign field value to them.
for ( my $idx = 0 ; $idx < $v_max_no_of_fld_rec ; $idx ++ ) {
   if ( ! exists $a_fld_nm[ $idx ] or $a_fld_nm[ $idx ] eq '' ) {                          # Is field null
      $a_fld_nm[ $idx ] = "*** Field " . ( $idx + 1 ) . " ***" ;
   }
}

close $IN_FILE or die "Could not close input file $p_in_file the second time - $!\n" ;

# Open and write file

open ( my $OUTFILE , ">" , $p_out_file ) or                     #
  die "Could not open output file $p_out_file - $!\n" ;

print $OUTFILE                                                  # Print report header first line
  "Serial No" . "\t" .                                          # 1 Serial No
  "Description" . "\t" .                                        # 2 Description
  "Occurences" . "\t" .                                         # 3 Occurences
  "Type (after removing unprintables)" . "\t" .                 # 4 Type
  "Empty" . "\t" .                                              # 5 Empty fields
  "Contains unprintable characters count" . "\t" .              # 6 Unprintable characters
  "Minimum not null length first record no." . "\t" .           # 7 Minimum not null length first record no
  "Minimum not null length" . "\t" .                            # 8 Minimum not null length
  "Maximum length first record no" . "\t" .                     # 9 Maximum length first record no
  "Maximum length" . "\t" .                                     # 10 Maximum length
  "Minimum not null value first record no" . "\t" .             # 11 Minimum not null value first record no
  "Minimum not null value" . "\t" .                             # 12 Minimum not null value
  "Maximum value first record no" . "\t" .                      # 13 Maximum value first record no
  "Maximum value" . "\t" .                                      # 14 Maximum value
  ( $p_enc_chr eq "" ? "" : "Count optional enclosing character" . "\t" ) .                # 15 Count optional enclosing character
  ( $p_enc_chr eq "" ? "" : "Unbalanced enclosure count" . "\t" ) .                        # 16 Unbalanced enclosure count
  ( $p_enc_chr eq "" ? "" : "Unescaped enclosure count" . "\t" ) .                         # 17 Unescaped enclosure count
  ( $p_trc_chr eq "" ? "" : "Tracing characters" . "\t" ) .     # 18 Tracing characters
  "Email like field count" . "\t" .                                  # 19 Valid Email count
  "Minimum Fields" . "\t" .                                     # 20 Minimum Fields
  "Maximum Fields" . "\n" ;                                     # 21 Maximum Fields

# If file is empty set constant value to zero
if ( $v_rec_no_min_no_flds == 0 and $v_rec_no_min_len == 0 ) {
   $v_rec_no_min_len = '' ;
   $v_min_no_flds    = '' ;
   $v_rec_min_len    = '' ;
}

# If minimum no fields are constant value set to null
if ( $v_rec_no_min_no_flds == 0 ) {
   $v_min_no_flds = '' ;
}

# If record no of maximum length is zero then set null to maximum length and itself is zero
if ( $v_rec_no_max_len == 0 ) {
   $v_rec_no_max_len = '' ;
   $v_rec_max_len    = '' ;
}

# If record no of maximum no of fields are zero then set maximum no of fields to zero
if ( $v_rec_no_max_no_flds == 0 ) {
   $v_max_no_flds = '' ;
}

# If empty record count is zero set to null
if ( $v_rec_empty_knt == 0 ) {
   $v_rec_empty_knt = '' ;
}

# If record with a invalid character count is zero set to null
if ( $v_rec_with_invalid_char_knt == 0 ) {
   $v_rec_with_invalid_char_knt = '' ;
}

if ( $p_trc_chr ne "" ) {                                       # If tracing character(s) are non empty
   @a_trc_chr = split ( "" , $p_trc_chr ) ;                     # Split into single character
   $v_trc_chr = join ( " , " , @a_trc_chr ) ;                   # Join with comma
}

print $OUTFILE                                                  # Write file information
  ( $p_f_dt_1 eq "n" ? "With header" : "Without header" ) . "\t" .    # 1 With header or Without header
  "File/Fields: " . $p_in_file . "\t" .                         # 2 File name
  $v_rec_knt . "\t" .                                           # 3 No of records
  "Field separator: " .                                         # 4 Field separator
  (
   $p_fld_spr =~ /[[:cntrl:]]/                                  # 4 Check if it is control character
   ?                                                            # 4
     (
      exists $h_dec_ctrl_chr{ ord ( $p_fld_spr ) }              # 4 Check if it exists in a hash
      ?                                                         # 4
        $h_dec_ctrl_chr{ ord ( $p_fld_spr ) } . ' '             # 4
      : ''                                                      # 4
     ) .                                                        # 4
     "Ordinal: " . ord ( $p_fld_spr )                           # 4 Ordinal value of field separator
   :                                                            # 4
     $p_fld_spr
  )
  . "\t"
  .                                                             # 4
  $v_rec_empty_knt . "\t" .                                     # 5 Empty records count
  $v_rec_with_invalid_char_knt . "\t" .                         # 6 Invalid records count
  "$v_rec_no_min_len" . "\t" .                                  # 7 Record no of minimum length
  $v_rec_min_len . "\t" .                                       # 8 Minimum record length
  "$v_rec_no_max_len" . "\t" .                                  # 9 Record no of maximum length
  $v_rec_max_len . "\t" .                                       # 10 Maximum record length
  "\t" .                                                        # 11
  "\t" .                                                        # 12
  "\t" .                                                        # 13
  "\t" .                                                        # 14
  (
   $p_enc_chr eq "" ? "" :                                      # 15 Check enclosing parameter is empty or not
     "Enclosing character: " . (                                # 15
      $p_enc_chr =~ /[[:cntrl:]]/                               # 15 Checking if it control character or not
      ?                                                         # 15
        "Ordinal" . ord ( $p_enc_chr )                          # 15 Ordinal value of enclosing character
      :                                                         # 15
        $p_enc_chr                                              # 15
     )
     . "\t"
  )
  .                                                             # 15 Enclosure parameter are empty then hide this column
  ( $p_enc_chr eq "" ? "" : "\t" ) .                            # 16 Enclosure parameter are empty then hide this column
  ( $p_enc_chr eq "" ? "" : "\t" ) .                            # 17 Enclosure parameter are empty then hide this column
  ( $p_trc_chr eq "" ? "" : $v_trc_chr . "\t" ) .               # 18 Display tracing character if parameter is non empty
  "\t" .                                                        # 19
  ( $v_rec_no_min_no_flds == 0 ? "$v_min_no_flds" : "Record no $v_rec_no_min_no_flds: $v_min_no_flds" ) . "\t" .    # 20 Minimum fields with record no
  ( $v_rec_no_max_no_flds == 0 ? "$v_max_no_flds" : "Record no $v_rec_no_max_no_flds: $v_max_no_flds" ) . "\n" ;    # 21 Maximum fields with record no

for ( $idx = 0 ; $idx < $v_max_no_of_fld_rec ; $idx ++ ) {      # Print field wis information

   print                                                        #
     $OUTFILE                                                   # Write information to the output file
     ( $idx + 1 ) . "\t" .                                      # 1 Serial no
     (
      $p_f_dt_1 eq "n"                                          # 2 If parameter fields in first record is off
      ?                                                         # 2 Field names
        $a_fld_nm[ $idx ]
      :                                                         # 2
        "Field " . ( $idx + 1 )                                 # 2 Increment Field number by 1
     ) . "\t" .                                                 # 2
     $h_no_of_occ{ $idx } . "\t" .                              # 3 Occurences
     $h_fld_typ{ $idx } . "\t" .                                # 4 Field Type
     $h_nll_knt{ $idx } . "\t" .                                # 5 Null field counts
     $h_fld_inval_chr_knt{ $idx } . "\t" .                      # 6 Invalid charcter counts
     $h_rec_no_fld_min_len{ $idx } . "\t" .                     # 7 Field wise Minimum length record no
     $h_fld_min_len{ $idx } . "\t" .                            # 8 Field wise Minimum length
     $h_rec_no_fld_max_len{ $idx } . "\t" .                     # 9 Field wise Maximum length record no
     $h_fld_max_len{ $idx } . "\t" .                            # 10 Field wise Maximum length
     $h_rec_no_fld_min{ $idx } . "\t" .                         # 11 Field wise Minimum field value with record no
     $h_fld_min{ $idx } . "\t" .                                # 12 Field wise Minimum field velue
     $h_rec_no_fld_max{ $idx } . "\t" .                         # 13 Field wise Maximum field value with record no
     $h_fld_max{ $idx } . "\t" .                                # 14 Field wise Maximum field value
     ( $p_enc_chr eq "" ? "" : $h_encl_knt{ $idx } . "\t" ) .   # 15 Count of Enclosing character
     ( $p_enc_chr eq "" ? "" : $h_bad_encl_knt{ $idx } . "\t" ) .                                                   # 16 Bad enclosure counts
     ( $p_enc_chr eq "" ? "" : $h_unesc_encl_knt{ $idx } . "\t" ) .                                                 # 17 Unescaped enclosing characters count
     ( $p_trc_chr eq "" ? "" : $h_trc_chr_knt{ $idx } . "\t" ) .                                                    # 18 Tracing characters
     $h_val_email_chk_knt{ $idx } . "\t" .                      # 19 Count of valid email address
     "\t" .                                                     # 20  Minimum fields with record no
     "\n" ;                                                     # 21  Maximum fields with record no

} ## end for ( $idx = 0 ; $idx <...)

close $OUTFILE or die "Could not close output file $p_out_file - $!\n" ;

exit 1 ;

#######################################################################
# End of Main                                                         #
#######################################################################

sub sGetParameter {

   # Subroutine to get parameter and handle field separator and
   # optional enclosing character

   my ( $p_GetOptions ) = {
      help => {                                                 # Help
         type    => '' ,
         env     => '-' ,
         default => '' ,
         verbose => "File statistics" ,
         order   => 1 ,
        } ,
      input_file => {                                           # Input file
         type    => "=s" ,
         env     => "-" ,
         default => '' ,
         verbose => "Input file path name" ,
         order   => 2 ,
        } ,
      field_separator => {                                      # Pass field separator
         type    => "=s" ,
         env     => "-" ,
         default => '' ,
         verbose => "Field separator - Control and meta character aliases can be used" ,
         order   => 3 ,
        } ,
      encloser_character => {                                   # Pass field separator
         type    => "=s" ,
         env     => "-" ,
         default => '' ,
         verbose => "Optional field enclosing character - Control and meta character aliases can be used" ,
         order   => 4 ,
        } ,
      tracing_character => {                                    # Tracing characters
         type    => "=s" ,
         env     => "-" ,
         default => '' ,
         verbose => "Tracing characters" ,
         order   => 5 ,
        } ,
      data_from_1_record_flag => {                              # Data starting from the first record instead of field labels in the first record
         type    => "!" ,
         env     => "-" ,
         default => "0" ,                                       # Default (parameter not mentioned) means field names in first record
         verbose => "Data starting from the first record - default header record present if not mentioned" ,
         order   => 6 ,
        } ,
      output_file => {                                          # Input file
         type    => "=s" ,
         env     => "-" ,
         default => '' ,
         verbose => "Output file path name" ,
         order   => 7 ,
        } ,
      control_character_display => {                            # Display control character equivalences for field separator or optional enclosing character and stop
         type    => '!' ,
         env     => '-' ,
         default => '0' ,                                       # Default (parameter not mentioned) means script nuns normally
         verbose => 'Display control and meta character aliases for field separator or enclosing character and stop' ,
         order   => 8 ,
        } ,

   } ;

   my ( $parameters ) = Getopt::Simple -> new () ;              # Variable for runtime parameters

   if ( ! $parameters -> getOptions ( $p_GetOptions , 'Usage: File_Stat.pl [options]' ) ) {
      exit ( -1 ) ;                                             # Failure.
   }

   $p_in_file   = $$parameters{ 'switch' }{ 'input_file' } ;
   $p_fld_spr   = $$parameters{ 'switch' }{ 'field_separator' } ;
   $p_enc_chr   = $$parameters{ 'switch' }{ 'encloser_character' } ;
   $p_trc_chr   = $$parameters{ 'switch' }{ 'tracing_character' } ;
   $p_f_dt_1    = $$parameters{ 'switch' }{ 'data_from_1_record_flag' } ;
   $p_out_file  = $$parameters{ 'switch' }{ 'output_file' } ;
   $p_f_ct_c_dp = $$parameters{ 'switch' }{ 'control_character_display' } ;                                              # Flag if display control character equivalences and stop

   # Display control character equivalences and stop option chosen
   if ( $p_f_ct_c_dp eq 1 ) {
      &sDpCtCStp ;                                              # Display Control characters
      exit 1 ;
   }

   if   ( $p_f_dt_1 eq 1 ) { $p_f_dt_1 = "Y" ; }                #
   else                    { $p_f_dt_1 = "n" ; }

   if ( $p_in_file eq "" ) { die "JOB ABANDONDED - No input file\n" ; }
   if ( $p_fld_spr eq "" ) { die "JOB ABANDONDED - No field separator character\n" ; }

   $p_fld_spr = &sHdlFdSiOptEncl ( $p_fld_spr , 'Field Separator' ) ;                                                    # Handle field separator aliases

   if ( $p_enc_chr ne "" ) {
      $p_enc_chr = &sHdlFdSiOptEncl ( $p_enc_chr , 'Optional Field Enclosure' ) ;                                        # Handle optional field enclosure aliases
   }

   # If Output file is null set default output file name as input file name with . as _ and _statistics.tsv at end
   if ( $p_out_file eq "" ) {
      $p_out_file = $p_in_file ;
      $p_out_file =~ s/^.*[\\|\/]//g ;                          # Remove path
      $p_out_file =~ s/\./\_/g ;                                # Replace dot to underscore
      $p_out_file =~ s/\__/\_/g ;                               # Replace double underscore to single underscore
      $p_out_file = $p_out_file . "_statistics.tsv" ;           # Concatinate _statistics.tsv to output file
   }                                                            # end if

   # Field separator to contain only one character
   if ( length $p_fld_spr > 1 ) {
      die "JOB ABANDONDED - Field separator >$p_fld_spr< must contain only one character" ;
   }

   # Optional enclosing separator contain only one character
   if ( length $p_enc_chr > 1 ) {
      die "JOB ABANDONDED - Optional enclosing character >$p_enc_chr< may contain only one character" ;
   }

   return 1 ;

} ## end sub sGetParameter

## end sub sGetParameter
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
      and $vhfsoe_what ne 'Optional Field Enclosure' ) {
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

sub sGetFieldTypes {

   # Subroutine to get field types
   # Highest priority of a data type are Unknown , Unicode , ASCII ,
   # Signed decimal , Decimal , Signed integer , Integer

   # Calculate maximum array length for reading each record
   my $v_arr_len = $#a_fld + 1 ;

   if ( $v_arr_len > $v_max_no_of_fld_rec ) { $v_max_no_of_fld_rec = $v_arr_len ; }

   for ( my $idx = 0 ; $idx < $v_max_no_of_fld_rec ; $idx ++ ) {
      $vgft_fld = $a_fld[ $idx ] || '' ;                        # Input field whose type to be determined
      my $vgft_prv_fld_typ = "" ;                               # Previous field type determined
      $vgft_fld =~ s/[[:cntrl:]]|[^[:print:]]//g ;              # Clean - remove non printable characters
      if ( defined $vgft_fld ) {
         if ( $vgft_fld eq "" ) {                               # Is field null
            if   ( exists $h_cln_nll_knt{ $idx } ) { $h_cln_nll_knt{ $idx }++ ; }
            else                                   { $h_cln_nll_knt{ $idx } = 1 ; }
            next ;
         }
      } ## end if ( defined $vgft_fld)

      if    ( $vgft_fld =~ /^\d+$/ )     { $vft_fld_typ = "Integer" ; }
      elsif ( $vgft_fld =~ /^[+-]\d+$/ ) { $vft_fld_typ = "Signed integer" ; }
      elsif ( $vgft_fld =~ /^\d*\.\d*$/     and $vgft_fld =~ /\d/ ) { $vft_fld_typ = "Decimal" ; }
      elsif ( $vgft_fld =~ /^[+-]\d*\.\d*$/ and $vgft_fld =~ /\d/ ) { $vft_fld_typ = "Signed decimal" ; }
      elsif ( $vgft_fld =~ /[\x20-\x7E]+/ ) { $vft_fld_typ = "ASCII" ; }     # Space to tilde -- ASCII Regex
      elsif ( $vgft_fld =~ /\p{C}/ )        { $vft_fld_typ = "Unicode" ; }
      else                                  { $vft_fld_typ = "Unknown" ; }

      if ( ! exists $h_fld_typ{ $idx } ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
         next ;
      }

      $vft_prv_fld_typ = $h_fld_typ{ $idx } ;                   # Previous field type determine
      if ( $vft_fld_typ eq "Unknown" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      }                                                         # end if
      elsif ($vft_fld_typ eq "Unicode"
         and $vft_prv_fld_typ ne "Unknown" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      }                                                         # end elsif
      elsif ($vft_fld_typ eq "ASCII"
         and $vft_prv_fld_typ ne "Unknown"
         and $vft_prv_fld_typ ne "Unicode" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      }                                                         # end elsif
      elsif ($vft_fld_typ eq "Signed decimal"
         and $vft_prv_fld_typ ne "ASCII"
         and $vft_prv_fld_typ ne "Unknown"
         and $vft_prv_fld_typ ne "Unicode" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      }                                                         # end elsif
      elsif ($vft_fld_typ eq "Decimal"
         and $vft_prv_fld_typ ne "Signed decimal"
         and $vft_prv_fld_typ ne "ASCII"
         and $vft_prv_fld_typ ne "Unknown"
         and $vft_prv_fld_typ ne "Unicode" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      }                                                         # end elsif
      elsif ($vft_fld_typ eq "Signed integer"
         and $vft_prv_fld_typ ne "Decimal"
         and $vft_prv_fld_typ ne "Signed decimal"
         and $vft_prv_fld_typ ne "ASCII"
         and $vft_prv_fld_typ ne "Unknown"
         and $vft_prv_fld_typ ne "Unicode" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      }                                                         # end elsif
      elsif ($vft_fld_typ eq "Integer"
         and $vft_prv_fld_typ ne "Signed integer"
         and $vft_prv_fld_typ ne "Decimal"
         and $vft_prv_fld_typ ne "Signed decimal"
         and $vft_prv_fld_typ ne "ASCII"
         and $vft_prv_fld_typ ne "Unknown"
         and $vft_prv_fld_typ ne "Unicode" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      }                                                         # end elsif
      $vft_fld_typ = $h_fld_typ{ $idx } ;

   } ## end for ( my $idx = 0 ; $idx...)

   return 1 ;

} ## end sub sGetFieldTypes
#######################################################################
# End of sub sGetFieldTypes                                           #
#######################################################################

=pod

=head1 File Statistics

 File Statistics about the file

=head2 Copyright

 Copyright (c) 2017 IdentLogic Systems Private Limited

=head2 Description

 File Statistics will give you over all statistic report of a file

=head3 Records

 Count no of non empty and empty records in the file
 Count no of unprintable characters in the records
 Minimum not null length with  record no
 first Maximum length with record no
 Display optional enclosing character if enclosing character run parameter
  used.
 Minimum no of fields in a records with record no.
 Maximum no of fields in a records with record no.

=head3 Fields

  1. first it will determine the data type of all the fields.
  2. count of null values per field number.
  3. Minimum of not null field as per field number.
  4. Record no of minimum of not null field as per field number.
  5. Maximum of not null field as per field number.
  6. Record no of maximum of not null field as per field number.
  7. Count of occurrences of invalid character in field as per field number.
  8. No of Occurrence of fields as per field number.
  9. Minimum length of field as per field number.
 10. Record no of minimum length of field as per field number.
 11. Maximum length of field as per field number.
 12. Record no of maximum length of field as per field number.
 13. Field wise count where optional enclosures used with proper closed.
 14. Field wise count where optional enclosure used only in beginning or at
     end of field.
 16. Unescaped enclosing characters count a per field number
      For checking Unescaped enclosing character i removed leading and trailing
      characters and removed pair enclosing character. This will not affect to your
      input file. its only for checking unescaped enclosing character.
 16. Count of tracing character or characters as per field number
      Checking these characters are how many times came in fields.
 17. Count of valid email addresses field as per field number
     I defined a regex which can take multiple email ids separated with
      either comma(,) or space( ). Review the below code:

      /^(([a-zA-Z0-9_\-\.]+)@([a-zA-Z0-9_\-\.]+)\.([a-zA-Z]{1,5}){1,25})+([, ](([a-zA-Z0-9_\-\.]+)@([a-zA-Z0-9_\-\.]+)\.([a-zA-Z]{1,5}){1,25})+)*$/

 18. Some fields in the output file are showing empty means this fields are empty
      or null in all records.

 Note: If enclosing character parameter is used in run parameter then only Count of
        optional enclosing character , Unbalanced enclosure count , Unescaped enclosure
        count columns get display else it hide.
       If the first record in the file contain header fields this procedure read
        the file from second record otherwise it will start from the first record.
        and if no header fields in file it will increment the fields using index.
        If the header fields are empty in somewhere it will full fill that empty
        space using *** Field field index ***.

=head3 Header fields in first record

 1. If first is empty it means that all fields in first record are empty then it
     will assign its own value.

 2. If first record is non empty then each fields in header cleaned by using
     some conditions.
    a) Replace control characters by space
    b) Replace multiple white space by space
    c) Remove leading and trailing blanks
 3. Assume that there is only one record in our file and that is a header file

=head3 Minimum and Maximum value as per field type

 It first check the data type of a field base on that it will perform the
  operation.
 If data type is ASCII, Unicode and unknown gt:greater than lt:less than
  operator is used.
 If data type is Integer, Signed integer ,Decimal and Signed decimal ( > , < )
  operator  is used.

 Highest priority of a data type are as below:

  1. Unknown
  2. Unicode
  3. ASCII
  4. Signed decimal
  5. Decimal
  6. Signed integer
  7. Integer

=head2 Output file

 In output file first row show you description of columns
 Second column of second row display With or without header means
  if you used data in a first record parameter then it will check
  records of input file from first 1st record else it start from second
  record. if it start from 1st record means without header else
 Second Column of second row shows input file name which you taken.

 Input file name displayed in output file means which input file you taken as a input.
 Whatever field separator and optional enclosure character you taken that also displayed
  in output file.
 If field separator and optional enclosure are control character then in output displayed
  ordinal value.

 Give the output file extension: .tsv( tab separated field values ) file
 If you do not put output file name with extension it will create Output file by
  default.
 for example ( input_File_extension_statistics.tsv )
 If you give input file with path the output file will be stored in that given path.
  else in same folder .

=head2 TSV format

 Output file extension is .tsv
 Open .tsv file in LibreOffice calc
 when you click on it one dialog box prompt it out.
 Choose Tab separator option (Choose as per your convenience otherwise
 your output file show you wrong format)

 Text delimiter choose as per convenience.

=head2 Warning

 There are special character which cannot separate fields. To avoid this problem
 i added backslash prefix to field separator or optional enclosing character within a procedure.

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

 Splits the string EXPR into a list of strings and returns that list. By default,
 empty leading fields are preserved and empty trailing ones are deleted. ( If all
 fields are empty, they are considered to be trailing.)

=head2 Field separator and optional enclosing character

    i. Handle field separator and optional enclosing character.

   ii. If length of the character is more than one convert into upper case
        and check if its alias control character is present in the hash key.
        If it is not there it will throw an error.

  iii. Display control character equivalences for field separator or
        optional enclosing character and stop.

=head2 Error Message

 Always give the input file name otherwise it will give an error.
 Field separator should not be null
 Field separator contain only one character
 Optional enclosing separator contain only one character
 Escape character start with a backslash.

 Some control characters and metacharacter cannot directly used in command promp to
  avoid this kind of problem use control character parameter .
 It will display control and meta character aliases for field separator or
  enclosing character and stop

=head2 Technical

 Package Number   - 44
 Procedure Number - 507
 Procedure Name   - File_Stat.pl

=head3 Run parameters

 PARAMETER                  DESCRIPTION                         ABV   VARIABLE
 ---------                  -----------                         ---  ---------
 input_file                 Input file name with extension       i   $p_in_file
 field_separator            Field Separator                      f   $p_fld_spr
 encloser_character         Enclosed separator                   e   $p_enc_chr
 tracing_character          Tracing character or characters      t   $p_trc_chr
 data_from_1_record_flag    Data starting from the first record  d   $p_f_dt_1
 output_file                Output file name with extension      o   $p_out_file
 control_character_display  Flag if display control character    c   $p_f_ct_c_dp

 ABV - Abbreviation for calling run parameter

=head3 Different examples of run parameter to run a procedure:

 perl File_Stat.pl -i input.txt -f PIPE -e DOUBLEQUOTE -d  -o input_txt_filestatistics.tsv

 perl File_Stat.pl -i input.txt -f PLUS -e DOUBLEQUOTE -o input_txt_filestatistics.tsv

 perl File_Stat.pl -i input.txt -f CLOSEBRACKET -o input_txt_filestatistics.tsv

 perl File_Stat.pl -i input.txt -f QUESTION -e DOUBLEQUOTE

 perl File_Stat.pl -i input.txt -f STOP

 perl File_Stat.pl -i input.txt -f STOP -t @#z

 perl File_Stat.pl -i input.txt -f STOP -t *

 perl File_Stat.pl -i input.txt -f STOP -c

 perl File_Stat.pl -i E:\folder1\folder2\input.txt -f SEMICOLON
  -e DOUBLEQUOTE -d  -o input_txt_filestatistics.tsv

 Input file is mandatory.
 Field separator is mandatory.
 Encloser character is optional
 Tracing character or characters are optional
 Output file is optional
 Control character is optional to check which are the field separator and
 enclosing character.
 data_from_1_record_flag is flag to determine data starting from the first record
  there are no header file in input file.
 Control character are optional , it is specially for new user

=head4 Control character and meta characters

 Display control character and meta characters equivalences for field separator or
  optional enclosing character and stop

 NUL                => 0 ,                                  # Null
 SOH                => 1 ,                                  # Start of heading
 STX                => 2 ,                                  # Start of text
 ETX                => 3 ,                                  # End of text
 EOT                => 4 ,                                  # End of transmission
 ENQ                => 5 ,                                  # Enquiry
 ACK                => 6 ,                                  # Aknowledge
 BEL                => 7 ,                                  # Bell
 \a                 => 7 ,                                  # Bell
 BS                 => 8 ,                                  # Backspace
 \b                 => 8 ,                                  # Backspace
 TAB                => 9 ,                                  # Tab
 \t                 => 9 ,                                  # Tab
 LF                 => 10 ,                                 # Line feed
 \n                 => 10 ,                                 # Line feed
 VT                 => 11 ,                                 # Vertical tab
 \v                 => 11 ,                                 # Vertical tab
 FF                 => 12 ,                                 # Form feed
 \f                 => 12 ,                                 # Form feed
 CR                 => 13 ,                                 # Carriage return
 \r                 => 13 ,                                 # Carriage return
 SO                 => 14 ,                                 # Shift out
 SI                 => 15 ,                                 # Shift in
 DLE                => 16 ,                                 # Data link escape
 DC1                => 17 ,                                 # Device control 1
 DC2                => 18 ,                                 # Device control 2
 DC3                => 19 ,                                 # Device control 3
 DC4                => 20 ,                                 # Device control 4
 NAK                => 21 ,                                 # Negative aknowledge
 SYN                => 22 ,                                 # Synchronous idle
 ETB                => 23 ,                                 # End of trans. block
 CAN                => 24 ,                                 # Cancel
 em                 => 25 ,                                 # End of medium
 sub                => 26 ,                                 # Substitude
 ESC                => 27 ,                                 # Escape
 \e                 => 27 ,                                 # Escape
 FS                 => 28 ,                                 # File separator
 GS                 => 29 ,                                 # Group separator
 RS                 => 30 ,                                 # Record separator
 US                 => 31 ,                                 # Unit separator
 SPACE              => 32 ,                                 # Space
 EXCLAMATION        => 33 ,                                 # !
 DOUBLEQUOTE        => 34 ,                                 # Double quote
 HASH               => 35 ,                                 # #
 DOLLAR             => 36 ,                                 # $
 PERCENT            => 37 ,                                 # %
 AND                => 38 ,                                 # &
 SINGLEQUOTE        => 39 ,                                 # Single quote
 OPENBRACKET        => 40 ,                                 # (
 CLOSEBRACKET       => 41 ,                                 # )
 STAR               => 42 ,                                 # *
 PLUS               => 43 ,                                 # +
 COMMA              => 44 ,                                 # ,
 MINUS              => 45 ,                                 # -
 STOP               => 46 ,                                 # .
 FORWARDSLASH       => 47 ,                                 # /
 COLON              => 58 ,                                 # :
 SEMICOLON          => 59 ,                                 # ;
 LESSTHAN           => 60 ,                                 # <
 EQUAL              => 61 ,                                 # =
 GREATERTHAN        => 62 ,                                 # >
 QUESTION           => 63 ,                                 # ?
 AT                 => 64 ,                                 # @
 OPENSQUAREBRACKET  => 91 ,                                 # [
 BACKSLASH          => 92 ,                                 # \
 CLOSESQUAREBRACKET => 93 ,                                 # ]
 ANCHOR             => 94 ,                                 # ^
 UNDERSCORE         => 95 ,                                 # _
 BACKTICK           => 96 ,                                 # `
 OPENCURLYBRAKCKET  => 123 ,                                # {
 PIPE               => 124 ,                                # |
 \|                 => 124 ,                                # | Pipe
 CLOSECURLYBRACKET  => 125 ,                                # }
 TILDE              => 126 ,                                # ~
 DEL                => 127                                  #

=head4 Help and defaults

 For detailed help with defaults run: Perl <program_name> -h.

=head3 Perl modules used

 Getopt::Simple

=head3 Subroutines

 Subroutine      Description
 --------------  -----------------------------
 sGetParameter   Gets run parameters.
 sHdlFdSiOptEncl Handle field separator and optional enclosing character
 sDpCtCStp       Display control and meta character equivalences for field separator or
                 optional enclosing character and stop
 sGetFieldTypes  Gets field data type.

=head4 Called by

 Subroutine     Called by
 -------------  ----------
 sGetParameter     Main
 sHdlFdSiOptEncl   sGetParameter
 sDpCtCStp         sGetParameter
 sGetFieldTypes    Main

=head4 Structure

  Main
  |-- sGetParameter
  |   |-- sHdlFdSiOptEncl
  |   \-- sDpCtCStp
  \-- sGetFieldTypes

=cut

#######################################################################
# End of File_Stat.pl                                                 #
#######################################################################
