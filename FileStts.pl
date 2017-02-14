#!C:/Perl/bin/perl -w

# Application   :
# Client        :
# Copyright (c) : IdentLogic Systems Private Limited
# Author        : Surendra Kadam
# Creation Date : 10 February 2017
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

# Control variables
my $v_fld_spr = ";" ;                                            # Field separator character
my $v_in_file = "test.txt" ;                                     # Input file

# Invalid characters
my $v_rec_with_invalid_char_no = 0 ;                             # Records with invalid characters
my $v_fld_with_invalid_char_no = 0 ;                             # Fields with invalid characters

# Work variable
my $v_rec = '' ;                                                 # Stores record read
my $idx       = '' ;                                             # Index to process each field
my $v_fld_typ = '' ;                                             # Field type for currect field
my @a_fld  = () ;                                                # Contains individual fields
my $v_1_0_rec_inval_chr = '' ;                                   # Indicator if record has invalid character
my $v_no_of_flds = 0 ;                                           # Work variable to store number of fields in a record
my $v_fld_len    = 0 ;                                           # Length of field currenly being examined
my $vft_fld_typ     = '' ;                                       # work field type
my $vft_prv_fld_typ = '' ;                                       # Previous field type determined

# Result variables
my $v_rec_knt = 0 ;                                              # Records read count

my %h_nll_knt = () ;                                             # Count of null values per field number
my %h_fld_min = () ;                                             # Minimum of not null field as per field number
my %h_fld_max = () ;                                             # Maximum of not null field as per field number
my %h_fld_typ = () ;                                             # Field type - in order: Integer , Signed Integer , Decimal , Signed Decimal , ASCII , Unicode , Other
my %h_fld_inval_chr_knt = () ;                                   # Count of occurences of invalid character in field
my %h_cln_nll_knt = () ;                                         # After cleaning count of null values per field number
my %h_no_of_occ  = () ;                                          # No of Occcurence of fields

# Maximum and minimum field count related variables

my $v_max_no_flds        = 0 ;                                   # Maximum number of fields
my $v_rec_no_max_no_flds = 0 ;                                   # Record no with maximum number of fields
my $v_min_no_flds        = 9999999999 ;                          # Minimum number of fields (NON EMPTY OR NON FULL WHITESPACE RECORDS)
my $v_rec_no_min_no_flds = 0 ;                                   # Record no with minimum number of fields (NON EMPTY OR NON FULL WHITESPACE RECORDS)

# Maximum and minimum record length variables

my $v_rec_max_len     = 0 ;                                      # Maximum length of a record
my $v_rec_min_len     = 9999999999 ;                             # Arbitrarily high value
my $v_rec_no_max_len  = 0 ;
my $v_rec_no_min_len  = 0 ;
my $v_rec_empty_knt   = 0 ;                                      # Empty records count variable
my $v_rec_wht_spc_knt = 0 ;                                      # White space records

# Maximum field length related variables

my $v_fld_max_len        = 0 ;                                   # Maximum field length
my $v_rec_no_fld_max_len = 0 ;                                   # Record no with maximum field length

# Open and read input file
open ( my $IN_FILE , "<" , $v_in_file ) or die "Could not open file $v_in_file - $!\n" ;

while ( $v_rec = <$IN_FILE> ) {
   chomp $v_rec ;
   $v_rec_knt ++ ;
   
   if ( $v_rec eq '' ) { $v_rec_empty_knt ++ ; next ; }          # Empty record
   
   @a_fld = split $v_fld_spr , $v_rec ;                          # Split record into fields

   $v_1_0_rec_inval_chr = 0 ;                                    # Set to zero - Indicator if record has invalid character

   if ( $v_rec =~ /[[:cntrl:]]/ ) { $v_rec_with_invalid_char_no ++ ; }    # Record has invalid character - HANDLE FOR ALL SEPARATOR WHITESPACE CHARACTER

   # Calculate Maximum and Minimum record length in all the records
   my $len = length ( $v_rec ) ;
   if ( $len > $v_rec_max_len ) {
      $v_rec_max_len    = $len ;
      $v_rec_no_max_len = $v_rec_knt ;
   }
   if ( $len < $v_rec_min_len ) {
      $v_rec_min_len    = $len ;
      $v_rec_no_min_len = $v_rec_knt ;
   }

   # Calculate maximum and minimum fields in all the records
   $v_no_of_flds = $#a_fld + 1 ;

   if ( $v_no_of_flds > $v_max_no_flds ) {                       # Check if maximum number of fields found
      $v_max_no_flds        = $v_no_of_flds ;
      $v_rec_no_max_no_flds = $v_rec_knt ;
   }

   if ( $v_no_of_flds < $v_min_no_flds ) {                       # Check if minimum number of fields found
      $v_min_no_flds        = $v_no_of_flds ;
      $v_rec_no_min_no_flds = $v_rec_knt ;
   }

   # Calculate maximum field length by examining each field in the array
   foreach my $v_x ( @a_fld ) {                                  # Check invalid character and length of each field
      if ( $v_x =~ /[[:cntrl:]]/ ) { $v_fld_with_invalid_char_no ++ ; }    # Field has invalid character
      #if ( $a_fld { $v_x } =~ /[[:cntrl:]]/ ) { $v_fld_with_invalid_char_no ++ ; }   # Field has invalid character
      $v_fld_len = length ( $v_x ) ;
      if ( $v_fld_len > $v_fld_max_len ) {
         $v_fld_max_len        = $v_fld_len ;
         $v_rec_no_fld_max_len = $v_rec_knt ;
      }
   } ## end foreach my $v_x ( @a_fld )

   
   for ( $idx = 0 ; $idx <= $#a_fld ; $idx ++ ) {
   	  
   	  
   	
      if ( $a_fld[ $idx ] eq '' ) {                              # Is field null
         if ( ! exists $h_nll_knt{ $idx } ) { $h_nll_knt{ $idx } = 1 ; }
         else                               { $h_nll_knt{ $idx }++ ; }
         next ;
      }
      
      if ( $a_fld[ $idx ] =~ /[[:cntrl:]]/ ) {                   # Field has invalid character(s)
        $v_1_0_rec_inval_chr = 1 ;
        if ( ! exists $h_fld_inval_chr_knt{ $idx } ) {
          $h_fld_inval_chr_knt { $idx } = 1 ;
        }
        else {
          $h_fld_inval_chr_knt { $idx } ++ ;
        }
        $a_fld[ $idx ] =~ s/[[:cntrl:]]//g ;                     # Remove invalid character(s) in field
      }

      if ( $a_fld[ $idx ] eq '' ) {                              # Is field null after cleaning
         if ( ! exists $h_cln_nll_knt{ $idx } ) { $h_cln_nll_knt{ $idx } = 1 ; }
         else                                   { $h_cln_nll_knt{ $idx }++ ; }
         next ;
      }


      &sGetFldTyp ( ) ;

      # Check for minimum value of field
      if ( ! exists $h_fld_min{ $idx } ) { $h_fld_min{ $idx } = $a_fld[ $idx ] ; }
      else {
         if ( $a_fld[ $idx ] lt $h_fld_min{ $idx } ) { $h_fld_min{ $idx } = $a_fld[ $idx ] ; }
      }

      # Check for maximum value of field
      if ( ! exists $h_fld_max{ $idx } ) { $h_fld_max{ $idx } = $a_fld[ $idx ] ; }
      else {
         if ( $a_fld[ $idx ] gt $h_fld_max{ $idx } ) { $h_fld_max{ $idx } = $a_fld[ $idx ] ; }
      }

      if( ! exists $h_no_of_occ {$idx} ) {
      	$h_no_of_occ {$idx} = 1 ;
      }else {
    	$h_no_of_occ {$idx} ++ ;
    	}
      

  }## end for ( $idx = 0 ; $idx <=...)

   $v_rec_with_invalid_char_no = $v_rec_with_invalid_char_no + $v_1_0_rec_inval_chr ;    # Records with invalid characters

} ## end while ( $v_rec = <$IN_FILE>)

#######################################################################
# End of Main                                                         #
#######################################################################

sub sGetFldTyp {

   if    ( $a_fld[ $idx ] =~ /^\d*$/ )            { $vft_fld_typ = "Integer" ; }
   elsif ( $a_fld[ $idx ] =~ /[+-]\d+$/ )         { $vft_fld_typ = "Signed Integer" ; }
   elsif ( $a_fld[ $idx ] =~ /\d*.\d*/ )          { $vft_fld_typ = "Decimal" ; }
   elsif ( $a_fld[ $idx ] =~ /[+-]\d*\.\d*/ )     { $vft_fld_typ = "Signed Decimal" ; }
   elsif ( $a_fld[ $idx ] =~ /[(\w)(\W)(\s)]+./ ) { $vft_fld_typ = "ASCII" ; }            # PROBLEM ???
   elsif ( $a_fld[ $idx ] =~ /[(\w)(\W)(\s)]+./ ) { $vft_fld_typ = "Unicode" ; }          # PROBLEM ???
   else                                           { $vft_fld_typ = "Other" ; }

    ## end else [ if ( ! exists $h_fld_typ...)]
} ## end sub Field_Type
#######################################################################
# End of sGetFldTyp                                                   #
#######################################################################

  print
  "Serial No \t Description \t Occurences \t Type \t Empty \t Contains unprintable \t Minimum not null length \t Maximum length  \t Maximum field length \t Minimum not null value \t Maximum not null value \t Minimum Fields \t Maximum Fields\n" ;


  print " \t File \t $v_rec_knt \t \t $v_rec_empty_knt \t \tRecord no $v_rec_no_min_len: $v_rec_min_len \t Record no $v_rec_no_max_len: $v_rec_max_len \t Record no $v_rec_no_fld_max_len:$v_fld_max_len \t \t \t Record no $v_rec_no_min_no_flds: $v_min_no_flds \t Record no $v_rec_no_max_no_flds: $v_max_no_flds  \n" ;

  my $v_serial_no = 1;                                             # Internal Serial no
  my $v_field_no = 1;                                              # Internal Field no
for ( $idx = 0 ; $idx <= $#a_fld ; $idx ++ ) {
	
  if ( ! exists $h_nll_knt { $idx } ) { $h_nll_knt{ $idx } = 0 ; }
         else                        { $h_nll_knt{ $idx }= $h_nll_knt{ $idx }; }

  if ( ! exists $h_fld_inval_chr_knt { $idx } ) { $h_fld_inval_chr_knt { $idx } = 0 ; }
         else                                   { $h_fld_inval_chr_knt { $idx } = $h_fld_inval_chr_knt { $idx } ; }
         
  if ( ! exists $h_fld_typ{ $idx } ) { $h_fld_typ{ $idx } = $vft_fld_typ ; }
   else {
      $vft_prv_fld_typ = $h_fld_typ{ $idx } ;
      if    ( $vft_fld_typ eq "Other" )          { $h_fld_typ{ $idx } = "Other" ; }
      elsif ( $vft_fld_typ eq "Unicode" )        { $h_fld_typ{ $idx } = "Unicode" ; }
      elsif ( $vft_fld_typ eq "ASCII" )          { $h_fld_typ{ $idx } = "ASCII" ; }
      elsif ( $vft_fld_typ eq "Signed Decimal" ) { $h_fld_typ{ $idx } = "Signed Decimal" ; }
      elsif ( $vft_fld_typ eq "Decimal" )        { $h_fld_typ{ $idx } = "Decimal" ; }
      elsif ( $vft_fld_typ eq "Signed Integer" ) { $h_fld_typ{ $idx } = "Signed Integer" ; }
      elsif ( $vft_fld_typ eq "Integer" )        { $h_fld_typ{ $idx } = "Integer" ; }
   }
   
  print "$v_serial_no \t Field $v_field_no \t $h_no_of_occ{$idx} \t $h_fld_typ{$idx} \t $h_nll_knt{ $idx } \t $h_fld_inval_chr_knt{ $idx } \t \t \t \t $h_fld_min{ $idx } \t $h_fld_max{ $idx } \t \t \t \n";
  $v_serial_no++ ;
  $v_field_no++ ;

}
#######################################################################
# End of FileStts.pl                                                  #
#######################################################################