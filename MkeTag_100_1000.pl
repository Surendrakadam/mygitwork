#!C:/Perl/bin/perl -w

# Application   :
# Client        :
# Copyright (c) : IdentLogic Systems Private Limited
# Author        : Surendra Kadam
# Creation Date : 9 March 2017
# Description   :

# WARNINGS      :

# HARD CODINGS  :

# Limitations   :
# Dependencies  :
# Modifications
# Date       Change Req# Author       Description
# ---------- ----------- ------------ -------------------------------

use strict ;
use warnings ;

use Getopt::Simple ;

# Run variables
my $pp_set_data  = '' ;                                          # Set data no customer
my $pp_run_data  = '' ;                                          # Run parameter
my $pp_in_file   = '' ;                                          # Input file
my $pp_fld_spr   = '' ;                                          # Field separator character
my $pp_enc_chr   = '' ;                                          # Optional field encloser character
my $pp_f_dt_1    = '' ;                                          # Flag if data starting from the the first record itself, or, first record has column headers (default)
my $pp_f_ct_c_dp = '' ;                                          # Flag if display control character equivalences and stop

# Run statistics
my $v_rec_knt       = 0 ;                                       # Records read count
my $v_err_no_id     = 0 ;                                       # Count of records without Id
my $v_err           = 0 ;                                       # Count of error records
my $v_err_no_ky_fld = 0 ;                                       # Count of records without any one or more key field of Person_Name , Organization_Name or Address_Part1

# Work variable
my $v_rec             = '' ;                                    # Stores record read

# Work array
my @a_fld    = () ;                                             # Contains individual fields of current record


my %h_ctrl_chr = (            # Hash containing description of control characters < 128
    0 => 'NUL' ,
    1 => 'SOH' ,  2 => 'STX' ,  3 => 'ETX' ,  4 => 'EOT' ,  5 => 'ENQ' ,
    6 => 'ACK' ,  7 => 'BEL' ,  8 => 'BS' ,   9 => 'TAB' , 10 => 'LF' ,
   11 => 'VT' ,  12 => 'FF' ,  13 => 'CR' ,  14 => 'SO' ,  15 => 'SI' ,
   16 => 'DLE' , 17 => 'DC1' , 18 => 'DC2' , 19 => 'DC3' , 20 => 'DC4' ,
   21 => 'NAK' , 22 => 'SYN' , 23 => 'ETB' , 24 => 'CAN' , 25 => 'EM' ,
   26 => 'SUB' , 27 => 'ESC' , 28 => 'FS' ,  29 => 'GS' ,  30 => 'RS' ,
   31 => 'US' ,
  127 => 'DEL'
) ;

my %h_dec_ctrl_chr = (                                           # Hash containing the decimal ASCII value of the description of control characters < 128
  '\a'  =>   7 ,
  '\b'  =>   8 ,
  '\t'  =>   9 ,
  '\n'  =>  10 ,
  '\v'  =>  11 ,
  '\f'  =>  12 ,
  '\r'  =>  13 ,
  '\e'  =>  27 ,
  'NUL' =>   0 ,
  'SOH' =>   1 , 'STX' =>  2 , 'ETX' =>  3 , 'EOT' =>  4 , 'ENQ' =>  5 ,
  'ACK' =>   6 , 'BEL' =>  7 , 'BS'  =>  8 , 'TAB' =>  9 , 'LF'  => 10 ,
  'VT'  =>  11 , 'FF'  => 12 , 'CR'  => 13 , 'SO'  => 14 , 'SI'  => 15 ,
  'DLE' =>  16 , 'DC1' => 17 , 'DC2' => 18 , 'DC3' => 19 , 'DC4' => 20 ,
  'NAK' =>  21 , 'SYN' => 22 , 'ETB' => 23 , 'CAN' => 24 , 'EM'  => 25 ,
  'SUB' =>  26 , 'ESC' => 27 , 'FS'  => 28 , 'GS'  => 29 , 'RS'  => 30 ,
  'US'  =>  31 ,
  'DEL' => 127
) ;

 # \OOO Octal ASCII (all O from 0 to 7) \xhh Hexadecimal ASCII (all h from 0 to f)

my $v_fld_spr_len = '' ;

&sGetParameter () ;                                             # Subroutine to get parameters

# Open and read input file
open ( my $IN_FILE1 , '<' , $pp_in_file ) or                    #
  die "Could not open input file $pp_in_file - $!\n" ;

# Open and write file
open ( my $OUTFILE , '>' ,"$pp_set_data$pp_run_data.tag" ) or   #
  die "Could not open output file  - $!\n" ;

while ( $v_rec = <$IN_FILE1> ) {                                #

   chomp $v_rec ;

   $v_rec_knt ++ ;                                              # Record counter

   # Skip empty record
   if ( $v_rec eq '' ) { next ; }

   if ( $v_rec_knt == 1 and $pp_f_dt_1 eq 'n' ) { next ; }      # If first record contain Field label skip first record

   @a_fld = split $pp_fld_spr , $v_rec ;                        # Split record into fields

   for ( my $idx = 0 ; $idx <= $#a_fld ; $idx++ ) {
     $a_fld[$idx] = $a_fld[$idx] || '' ;

     # Remove optional enclosure character if present in the begining and end
     if ( $a_fld[$idx] =~ /^$pp_enc_chr/ and $a_fld[$idx] =~ /$pp_enc_chr$/ ) {
       $a_fld[$idx] =~ s/^$pp_enc_chr(.*)$pp_enc_chr$/$1/ ;
     }

     $a_fld[$idx] =~ s/[[:cntrl:]]+/ /g ;                       # Replace control characters by space
     $a_fld[$idx] =~ s/\s\s+/ /g;                               # Replace two or more spaces to single space
     $a_fld[$idx] =~ s/^\s+// ;                                 # Trim leading space
     $a_fld[$idx] =~ s/\s+$// ;                                 # Trim trailing space
   }

   my $v_addp2 = $a_fld[ 4 ] . ' ' . $a_fld[ 5 ] ;              # Address part 2 combined city and country
   $v_addp2 =~ s/^\s+|\s+$// ;                                  # Remove space if one of the field is empty

   if ( $a_fld[ 0 ] eq '' ) {                                   # Skip if no Id
     $v_err_no_id ++ ;                                          # Count of records without Id
     $v_err ++ ;                                                # Count of error records
     next ;
   }

     if ( $a_fld[ 1 ] eq '' and                                 # Person_Name field missing
          $a_fld[ 2 ] eq '' and                                 # Organization_Name field missing
          $a_fld[ 3 ] eq '' ) {                                 # Address_Part1 field missing
     $v_err_no_ky_fld ++ ;                                      # Count of records without any one or more key field of Person_Name , Organization_Name or Address_Part1
     $v_err ++ ;                                                # Count of error records
     next ;
   }

   print $OUTFILE                                               #
        '*Id*' , $a_fld[ 0 ]  ,
        ( $a_fld[ 1 ] eq '' ? '' : '*Person_Name*' . $a_fld[ 1 ] ) ,  # Only write field if not empty
        ( $a_fld[ 2 ] eq '' ? '' : '*Organization_Name*' . $a_fld[ 2 ] ) ,
        ( $a_fld[ 3 ] eq '' ? '' : '*Address_Part1*' . $a_fld[ 3 ] ) ,
        ( ( $a_fld[ 4 ] eq '' and $a_fld[ 5 ] eq '' ) ? '' : '*Address_Part2*' . $v_addp2 ) , # Address_Part2 is join of City and Country
        ( $a_fld[ 6 ] eq '' ? '' : '*Postal_Area*' . $a_fld[ 6 ] ) , #
         '***' , "\n" ;
}

close $IN_FILE1 or                                              #
  die                                                           #
  "Could not close input file $pp_in_file - $!\n" ;

close $OUTFILE or die "Could not close output Tagged file $pp_set_data$pp_run_data.tag - $!\n" ;

#######################################################################
# End of Main                                                         #
#######################################################################

sub sGetParameter {                                             # Subroutine to get parameter

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
      field_separator => {                                      # Pass field separator
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Field separator - for control characters use escaped value or term, e.g. \t or TAB for tab character' ,
         order   => 5 ,
        } ,
      encloser_character => {                                   # Optional enclosing separator
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Optional enclosing character - for control characters use escaped value or term, e.g. \t or TAB for tab character' ,
         order   => 6 ,
        } ,
      data_from_1_record_flag => {                              # Data starting from the first record instead of field labels in the first record
         type    => '!' ,
         env     => '-' ,
         default => '0' ,                                       # Default (parameter not mentioned) means field names in first record
         verbose => 'Data starting from the first record' ,
         order   => 7 ,
        } ,
      control_character_display => {                            # Display control character equivalences for field separator or optional enclosing character and stop
         type    => '!' ,
         env     => '-' ,
         default => '0' ,                                       # Default (parameter not mentioned) means script nuns normally
         verbose => 'Display control character equivalences for field separator or optional enclosing character and stop' ,
         order   => 8 ,
        } ,
   } ;

   my ( $parameters ) = Getopt::Simple -> new () ;              # variable for runtime parameters

   if ( ! $parameters -> getOptions ( $pp_GetOptions , 'Usage: File_Stat.pl [options]' ) ) {
      exit ( -1 ) ;                                             # Failure.
   }

   $pp_set_data  = $$parameters{ 'switch' }{ 'set_of_data' } ;
   $pp_run_data  = $$parameters{ 'switch' }{ 'run_number' } ;
   $pp_in_file   = $$parameters{ 'switch' }{ 'input_file_path' } ;
   $pp_fld_spr   = $$parameters{ 'switch' }{ 'field_separator' } ;
   $pp_enc_chr   = $$parameters{ 'switch' }{ 'encloser_character' } ;
   $pp_f_dt_1    = $$parameters{ 'switch' }{ 'data_from_1_record_flag' } ;
   $pp_f_ct_c_dp = $$parameters{ 'switch' }{ 'control_character_display' } ;   # Flag if display control character equivalences and stop

   if ( $pp_f_ct_c_dp eq 1 ) {        # Display control character equivalences and stop option chosen
      &sDpCtCStp ;
      exit 1 ;
   }



   else                        { $pp_f_ct_c_dp = 'n' ; }

   if   ( $pp_f_dt_1 eq 1 ) { $pp_f_dt_1 = 'Y' ; }              # Transform flag of ???
   else                     { $pp_f_dt_1 = 'n' ; }

   if ( $pp_set_data eq '' ) { die "JOB ABANDONDED - No set of data\n" ; }
   if ( $pp_run_data eq '' ) { die "JOB ABANDONDED - No run number\n" ; }
   if ( $pp_in_file  eq '' ) { die "JOB ABANDONDED - No input file path\n" ; }
   if ( $pp_fld_spr  eq '' ) { die "JOB ABANDONDED - No field separator character\n" ; }

   $pp_fld_spr = eval '"' . $pp_fld_spr . '"' ;
   $v_fld_spr_len = length $pp_fld_spr ;

   # Field separator to contain only one character
   if ( $v_fld_spr_len > 1 ) {
     die "JOB ABANDONDED - Field separator >$pp_fld_spr< must contain one character\n" ;
   }

   # Optional enclosing separator contain only one character
   if ( length $pp_enc_chr > 1 ) {
     die "JOB ABANDONDED - Optional enclosing character >$pp_enc_chr< may contain only one character or be omitted\n" ;
   }

   if ( $pp_set_data =~ /\D/ or $pp_set_data < 100  or $pp_set_data > 999 ) {
     die "JOB ABANDONDED - Data set number must be a number in the range of 100 to 999 instead of $pp_set_data\n" ;
   }

   if ( $pp_run_data =~ /\D/ or $pp_run_data < 1000 or $pp_run_data > 9999 ) {
     die "JOB ABANDONDED - rrrr set data must be a number in the range of 1000 to 9999 instead of $pp_run_data\n" ;
   }

} ## end sub sGetParameter

## end sub sGetParameter
#######################################################################
# End of sub sGetParameter                                            #
#######################################################################


   if ( $pp_f_ct_c_dp eq 1 ) {        # Display control character equivalences and stop option chosen
      &sDpCtCStp ;
      exit 1 ;
   }

#######################################################################
# End of sub sGetParameter                                            #
#######################################################################


#######################################################################
# End of MkeTag_100_999.pl                                            #
#######################################################################

