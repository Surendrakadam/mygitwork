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
use Getopt::Simple ;

# Control variables
my $p_fld_spr  = '' ;                                           # Field separator character
my $p_in_file  = '' ;                                           # Input file
my $p_f_dt_1   = '' ;                                           # Flag if data starting from the the first record itself, or, first record has column headers (default)
my $p_out_file = '' ;                                           # Output file

&sGetParameter () ;                                             # subroutine to get parameters

# Invalid characters
my $v_rec_with_invalid_char_no = 0 ;                            # Records with invalid characters
my $v_fld_with_invalid_char_no = 0 ;                            # Fields with invalid characters

# Work variable
my $v_rec               = '' ;                                  # Stores record read
my $idx                 = '' ;                                  # Index to process each field
my $v_fld_typ           = '' ;                                  # Field type for currect field
my @a_fld               = () ;                                  # Contains individual fields
my $v_1_0_rec_inval_chr = '' ;                                  # Indicator if record has invalid character
my $v_no_of_flds        = 0 ;                                   # Work variable to store number of fields in a record
my $v_fld_len           = 0 ;                                   # Length of field currenly being examined
my $vft_fld_typ         = '' ;                                  # work field type
my $vft_prv_fld_typ     = '' ;                                  # Previous field type determined

# Result variables
my $v_rec_knt         = 0 ;                                     # Records read count
my $v_rec_fld_typ_knt = 0 ;                                     # Records read count while determining field type

my %h_nll_knt            = () ;                                 # Count of null values per field number
my %h_fld_min            = () ;                                 # Minimum of not null field as per field number
my %h_rec_no_fld_min     = () ;                                 # Record no of minimum of not null field
my %h_fld_max            = () ;                                 # Maximum of not null field as per field number
my %h_rec_no_fld_max     = () ;                                 # Record no of maximum of not null field
my %h_fld_typ            = () ;                                 # Field type - in order: Integer , Signed Integer , Decimal , Signed Decimal , ASCII , Unicode , Other
my %h_fld_inval_chr_knt  = () ;                                 # Count of occurences of invalid character in field
my %h_cln_nll_knt        = () ;                                 # After cleaning count of null values per field number
my %h_no_of_occ          = () ;                                 # No of Occcurence of fields
my %h_fld_min_len        = () ;                                 # Minimum length of field
my %h_rec_no_fld_min_len = () ;                                 # Record no of minimum length of field
my %h_fld_max_len        = () ;                                 # Maximum length of field
my %h_rec_no_fld_max_len = () ;                                 # Record no of maxnimum length of field

# Maximum and minimum field count related variables

my $v_max_no_flds        = 0 ;                                  # Maximum number of fields
my $v_rec_no_max_no_flds = 0 ;                                  # Record no with maximum number of fields
my $v_min_no_flds        = 9999999999 ;                         # Minimum number of fields (NON EMPTY OR NON FULL WHITESPACE RECORDS)
my $v_rec_no_min_no_flds = 0 ;                                  # Record no with minimum number of fields (NON EMPTY OR NON FULL WHITESPACE RECORDS)

# Maximum and minimum record length variables

my $v_rec_max_len     = 0 ;                                     # Maximum length of a record
my $v_rec_min_len     = 9999999999 ;                            # Arbitrarily high value
my $v_rec_no_max_len  = 0 ;
my $v_rec_no_min_len  = 0 ;
my $v_rec_empty_knt   = 0 ;                                     # Empty records count variable
my $v_rec_wht_spc_knt = 0 ;                                     # White space records

# Maximum field length related variables

my $v_fld_max_len        = 0 ;                                  # Maximum field length
my $v_rec_no_fld_max_len = 0 ;                                  # Record no with maximum field length

my $v_max_arr_len = 0 ;                                         # Maximum array length
my $vgft_fld      = 0 ;                                         # variable get field type

my @a_fld_nm = () ;                                             # Array field in first record

my $o_file_nm_1st = '' ;                                        # Output file name without extension
my $o_file_ext    = '' ;                                        # Output file extension

# Open and read input file for field type determination

open ( my $IN_FILE1 , "<" , $p_in_file ) or die "Could not open input file $p_in_file - $!\n" ;

open ( my $Out_FILE , ">" , $p_out_file ) or die "Could not open output file $p_out_file - $!\n" ;

while ( $v_rec = <$IN_FILE1> ) {                                # Get field types of all fields
   $v_rec_fld_typ_knt ++ ;                                      # Increment record read counter
   chomp $v_rec ;
   @a_fld = split $p_fld_spr , $v_rec ;                         # Split record into fields

   if ( $v_rec_fld_typ_knt == 1 and $p_f_dt_1 eq "n" ) {
      for ( my $idx = 0 ; $idx < $#a_fld + 1 ; $idx ++ ) {
         $a_fld_nm[ $idx ] = $a_fld[ $idx ] || "" ;
      }
   }
   else {
      &sGetFieldTypes () ;
   }
} ## end while ( $v_rec = <$IN_FILE1>)

if ( $v_max_arr_len > 0 ) {
   for ( $idx = 0 ; $idx < $v_max_arr_len ; $idx ++ ) {
      if ( ! exists ( $h_fld_typ{ $idx } ) ) {
         $h_fld_typ{ $idx }            = "" ;
         $h_nll_knt{ $idx }            = 0 ;                    # Count of null values per field number
         $h_fld_min{ $idx }            = '' ;                   # Minimum of not null field as per field number
         $h_rec_no_fld_min{ $idx }     = 0 ;                    # Record no of minimum of not null field
         $h_fld_max{ $idx }            = '' ;                   # Maximum of not null field as per field number
         $h_rec_no_fld_max{ $idx }     = 0 ;                    # Record no of maximum of not null field
         $h_fld_inval_chr_knt{ $idx }  = 0 ;                    # Count of occurences of invalid character in field
         $h_cln_nll_knt{ $idx }        = 0 ;                    # After cleaning count of null values per field number
         $h_no_of_occ{ $idx }          = 0 ;                    # No of Occcurence of fields
         $h_fld_min_len{ $idx }        = 0 ;                    # Minimum length of field
         $h_rec_no_fld_min_len{ $idx } = 0 ;                    # Record no of minimum length of field
         $h_fld_max_len{ $idx }        = 0 ;                    # Maximum length of field
         $h_rec_no_fld_max_len{ $idx } = 0 ;                    # Record no of maxnimum length of field
      } ## end if ( ! exists ( $h_fld_typ...))
   } ## end for ( $idx = 0 ; $idx <...)
} ## end if ( $v_max_arr_len > ...)

close $IN_FILE1 or die "Could not close input file $p_in_file - $!\n" ;

open ( my $IN_FILE , "<" , $p_in_file ) or die "Could not open input file $p_in_file the second time - $!\n" ;

while ( $v_rec = <$IN_FILE> ) {
   chomp $v_rec ;
   $v_rec_knt ++ ;

   if ( $v_rec_knt == 1 and $p_f_dt_1 eq "n" ) { next ; }       # if first record contain Field label skip first record

   # Empty record or record with spaces
   if ( $v_rec eq '' or $v_rec =~ /^\s*$/ ) { $v_rec_empty_knt ++ ; next ; }

   @a_fld = split $p_fld_spr , $v_rec ;                         # Split record into fields

   $v_1_0_rec_inval_chr = 0 ;                                   # Set to zero - Indicator if record has invalid character

   if ( $v_rec =~ /[[:cntrl:]]/ ) { $v_rec_with_invalid_char_no ++ ; }    # Record has invalid character - HANDLE FOR ALL SEPARATOR WHITESPACE CHARACTER

   # Calculate maximum array length
   my $v_arr_len = $#a_fld + 1 ;
   if ( $v_arr_len > $v_max_arr_len ) { $v_max_arr_len = $v_arr_len ; }

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

   if ( $v_no_of_flds > $v_max_no_flds ) {                      # Check if maximum number of fields found
      $v_max_no_flds        = $v_no_of_flds ;
      $v_rec_no_max_no_flds = $v_rec_knt ;
   }

   if ( $v_no_of_flds > 0 ) {
      if ( $v_no_of_flds < $v_min_no_flds ) {                   # Check if minimum number of fields found
         $v_min_no_flds        = $v_no_of_flds ;
         $v_rec_no_min_no_flds = $v_rec_knt ;
      }
   } ## end if ( $v_no_of_flds > 0)

   # Calculate maximum field length by examining each field in the array

   foreach my $v_x ( @a_fld ) {                                 # Check invalid character and length of each field
      if ( $v_x =~ /[[:cntrl:]]/ ) { $v_fld_with_invalid_char_no ++ ; }    # Field has invalid character
      $v_fld_len = length ( $v_x ) ;
      if ( $v_fld_len > $v_fld_max_len ) {
         $v_fld_max_len        = $v_fld_len ;
         $v_rec_no_fld_max_len = $v_rec_knt ;
      }
   } ## end foreach my $v_x ( @a_fld )

   for ( $idx = 0 ; $idx < $v_max_arr_len ; $idx ++ ) {
      $a_fld[ $idx ] = $a_fld[ $idx ] || '' ;

      if ( $a_fld[ $idx ] eq '' ) {                             # Is field null
         if ( ! exists $h_nll_knt{ $idx } ) { $h_nll_knt{ $idx } = 1 ; }
         else                               { $h_nll_knt{ $idx }++ ; }
         next ;
      }
      else {
         if ( ! exists $h_nll_knt{ $idx } ) { $h_nll_knt{ $idx } = "" ; }
      }

      if ( $a_fld[ $idx ] =~ /[[:cntrl:]]/ ) {                  # Field has invalid character(s)
         $v_1_0_rec_inval_chr = 1 ;
         if ( ! exists $h_fld_inval_chr_knt{ $idx } ) {
            $h_fld_inval_chr_knt{ $idx } = 1 ;
         }
         else {
            $h_fld_inval_chr_knt{ $idx }++ ;
         }
         $a_fld[ $idx ] =~ s/[[:cntrl:]]//g ;                   # Remove invalid character(s) in field
      } ## end if ( $a_fld[ $idx ] =~...)
      else {
         $h_fld_inval_chr_knt{ $idx } = "" ;
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

      # If type is a Integer, Decimal, Signed Integer, Signed Decimal
      if (  ( $h_fld_typ{ $idx } eq "Integer" )
         or ( $h_fld_typ{ $idx } eq "Signed Integer" )
         or ( $h_fld_typ{ $idx } eq "Decimal" )
         or ( $h_fld_typ{ $idx } eq "Signed Decimal" ) ) {

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
            $h_fld_max_len{ $idx }        = length ( $a_fld[ $idx ] ) ;    # Record no of each maximum length of field
            $h_rec_no_fld_max_len{ $idx } = $v_rec_knt ;
         }
      } ## end else [ if ( ! exists $h_fld_max_len...)]

      # No of occurence in fields

      if ( ! exists $h_no_of_occ{ $idx } ) {
         $h_no_of_occ{ $idx } = 1 ;
      }
      else {
         $h_no_of_occ{ $idx }++ ;
      }

   } ## end for ( $idx = 0 ; $idx <...)

   $v_rec_with_invalid_char_no = $v_rec_with_invalid_char_no + $v_1_0_rec_inval_chr ;    # Records with invalid characters

} ## end while ( $v_rec = <$IN_FILE>)

for ( my $idx = 0 ; $idx < $v_max_arr_len ; $idx ++ ) {
   if ( ! exists $a_fld_nm[ $idx ] or $a_fld_nm[ $idx ] eq '' ) # Is field null
   {
      $a_fld_nm[ $idx ] = "*** Field " . ( $idx + 1 ) . " ***" ;
   }
} ## end for ( my $idx = 0 ; $idx...)

close $IN_FILE or die "Could not close input file $p_in_file the second time - $!\n" ;

print $Out_FILE                                                 # Print report header first line
  "Serial No\t" .                                               # 1
  "Description\t" .                                             # 2
  "Occurences\t" .                                              # 3
  "Type\t" .                                                    # 4
  "Empty\t" .                                                   #  5
  "Contains unprintable\t" .                                    # 6
  "Minimum not null length first record\t" .                    # 7
  "Minimum not null length\t" .                                 # 8
  "Maximum length first record \t" .                            # 9
  "Maximum length\t" .                                          # 10
  "Minimum not null value first record \t" .                    #11
  "Minimum not null value\t" .                                  #12
  "Maximum not null value first record \t" .                    #13
  "Maximum not null value\t" .                                  #14
  "Minimum Fields\t" .                                          #15
  "Maximum Fields\n" ;                                          #16

print $Out_FILE                                                 # Print file information
  "\t" .                                                        #1
  "File\t" .                                                    #2
  ( $p_f_dt_1 eq "n" ? ( $v_rec_knt - $v_rec_empty_knt - 1 ) : ( $v_rec_knt - $v_rec_empty_knt ) ) . "\t" .    # If flag is zero record counter is decremented by 1 else as it is
  "\t" .                                                        #4
  "$v_rec_empty_knt\t" .                                        #5
  "\t" .                                                        #6
  "Record no $v_rec_no_min_len\t" .                             #7
  "$v_rec_min_len\t" .                                          #8
  "Record no $v_rec_no_max_len\t" .                             #9
  "$v_rec_max_len\t" .                                          #10
  "\t" .                                                        #11
  "\t" .                                                        #12
  "\t" .                                                        #13
  "\t" .                                                        #14
  "Record no $v_rec_no_min_no_flds: $v_min_no_flds\t" .         #15
  "Record no $v_rec_no_max_no_flds: $v_max_no_flds\n" ;         #16

for ( $idx = 0 ; $idx < $v_max_arr_len ; $idx ++ ) {            # Print field wis information

   print $Out_FILE $idx + 1 . "\t" .                            #1
     ( $p_f_dt_1 eq "n" ? $a_fld_nm[ $idx ] : "Field " . ( $idx + 1 ) ) . "\t" .                               #2
     "$h_no_of_occ{$idx}\t" .                                   #3
     "$h_fld_typ{$idx}\t" .                                     #4
     "$h_nll_knt{ $idx }\t" .                                   #5
     "$h_fld_inval_chr_knt{ $idx }\t" .                         #6
     "$h_rec_no_fld_min_len{$idx}\t" .                          #7
     "$h_fld_min_len{ $idx }\t" .                               #8
     "$h_rec_no_fld_max_len{$idx}\t" .                          #9
     "$h_fld_max_len{ $idx }\t" .                               #10
     "$h_rec_no_fld_min{ $idx }\t" .                            #11
     "$h_fld_min{ $idx }\t" .                                   #12
     "$h_rec_no_fld_max{ $idx }\t" .                            #13
     "$h_fld_max{ $idx }\t" .                                   #14
     "\t" .                                                     #15
     "\n" ;                                                     #16

} ## end for ( $idx = 0 ; $idx <...)

#######################################################################
# End of Main                                                         #
#######################################################################

sub sGetFieldTypes {

   # Calculate maximum array length for reading each record
   my $v_arr_len = $#a_fld + 1 ;

   if ( $v_arr_len > $v_max_arr_len ) { $v_max_arr_len = $v_arr_len ; }

   for ( my $idx = 0 ; $idx < $v_max_arr_len ; $idx ++ ) {
      $vgft_fld = $a_fld[ $idx ] || '' ;                        # Input field whose type to be determined
      my $vgft_prv_fld_typ = "" ;                               # Previous field type determined
      $vgft_fld =~ s/[[:cntrl:]]|[^[:print:]]//g ;              # Clean - remove non printable characters CHECK AM NK
      if ( defined $vgft_fld ) {
         if ( $vgft_fld eq "" ) {                               # Is field null
            if   ( exists $h_cln_nll_knt{ $idx } ) { $h_cln_nll_knt{ $idx }++ ; }
            else                                   { $h_cln_nll_knt{ $idx } = 1 ; }
            next ;
         }
      } ## end if ( defined $vgft_fld)

      if    ( $vgft_fld =~ /^\d+$/ )     { $vft_fld_typ = "Integer" ; }
      elsif ( $vgft_fld =~ /^[+-]\d+$/ ) { $vft_fld_typ = "Signed Integer" ; }
      elsif ( $vgft_fld =~ /^\d*\.\d*$/     and $vgft_fld =~ /\d/ ) { $vft_fld_typ = "Decimal" ; }
      elsif ( $vgft_fld =~ /^[+-]\d*\.\d*$/ and $vgft_fld =~ /\d/ ) { $vft_fld_typ = "Signed Decimal" ; }
      elsif ( $vgft_fld =~ /[ -~]+/ ) { $vft_fld_typ = "ASCII" ; }     # Space to tilde -- ASCII Regex
      elsif ( $vgft_fld =~ /\p{C}/ )  { $vft_fld_typ = "Unicode" ; }
      else                            { $vft_fld_typ = "Unknown" ; }

      if ( ! exists $h_fld_typ{ $idx } ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
         next ;
      }

      $vft_prv_fld_typ = $h_fld_typ{ $idx } ;                   # Previous field type determine
      if ( $vft_fld_typ eq "Unknown" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      }
      elsif ($vft_fld_typ eq "Unicode"
         and $vft_prv_fld_typ ne "Unknown" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      }
      elsif ($vft_fld_typ eq "ASCII"
         and $vft_prv_fld_typ ne "Unknown"
         and $vft_prv_fld_typ ne "Unicode" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      }
      elsif ($vft_fld_typ eq "Signed Decimal"
         and $vft_prv_fld_typ ne "ASCII"
         and $vft_prv_fld_typ ne "Unknown"
         and $vft_prv_fld_typ ne "Unicode" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      } 
      elsif ($vft_fld_typ eq "Decimal"
         and $vft_prv_fld_typ ne "Signed Decimal"
         and $vft_prv_fld_typ ne "ASCII"
         and $vft_prv_fld_typ ne "Unknown"
         and $vft_prv_fld_typ ne "Unicode" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      }
      elsif ($vft_fld_typ eq "Signed Integer"
         and $vft_prv_fld_typ ne "Decimal"
         and $vft_prv_fld_typ ne "Signed Decimal"
         and $vft_prv_fld_typ ne "ASCII"
         and $vft_prv_fld_typ ne "Unknown"
         and $vft_prv_fld_typ ne "Unicode" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      }
      elsif ($vft_fld_typ eq "Integer"
         and $vft_prv_fld_typ ne "Signed Integer"
         and $vft_prv_fld_typ ne "Decimal"
         and $vft_prv_fld_typ ne "Signed Decimal"
         and $vft_prv_fld_typ ne "ASCII"
         and $vft_prv_fld_typ ne "Unknown"
         and $vft_prv_fld_typ ne "Unicode" ) {
         $h_fld_typ{ $idx } = $vft_fld_typ ;
      }
      $vft_fld_typ = $h_fld_typ{ $idx } ;

   } ## end for ( my $idx = 0 ; $idx...)
} ## end sub sGetFieldTypes

#######################################################################
# End of sub sGetFieldTypes                                           #
#######################################################################

sub sGetParameter {                                             # Subroutine to get parameter

   my ( $p_GetOptions ) = {
      help => {                                                 # Help
         type    => '' ,
         env     => '-' ,
         default => '' ,
         verbose => "File Statistics" ,
         order   => 1 ,
        } ,
      input_file => {                                           # Input file
         type    => "=s" ,
         env     => "-" ,
         default => '' ,
         verbose => "Input file name with extension" ,
         order   => 2 ,
        } ,
      field_separator => {                                      # Pass field separator
         type    => "=s" ,
         env     => "-" ,
         default => '' ,
         verbose => "Field Separator" ,
         order   => 3 ,
        } ,
      data_from_1_record_flag => {                              # Data starting from the first record instead of field labels in the first record
         type    => "!" ,
         env     => "-" ,
         default => "0" ,                                       # Default (parameter not mentioned) means field names in first record
         verbose => "Data starting from the first record" ,
         order   => 4 ,
        } ,
      output_file => {                                          # Input file
         type    => "=s" ,
         env     => "-" ,
         default => '' ,
         verbose => "Output file name with extension" ,
         order   => 5 ,
      } ,
   } ;

   my ( $parameters ) = Getopt::Simple -> new () ;              # variable for runtime parameters

   if ( ! $parameters -> getOptions ( $p_GetOptions , 'Usage: FileStts.pl [options]' ) ) {
      exit ( -1 ) ;                                             # Failure.
   }

   $p_in_file  = $$parameters{ 'switch' }{ 'input_file' } ;
   $p_fld_spr  = $$parameters{ 'switch' }{ 'field_separator' } ;
   $p_f_dt_1   = $$parameters{ 'switch' }{ 'data_from_1_record_flag' } ;
   $p_out_file = $$parameters{ 'switch' }{ 'output_file' } ;

   if   ( $p_f_dt_1 eq 1 ) { $p_f_dt_1 = "Y" ; }
   else                    { $p_f_dt_1 = "n" ; }

   if ( $p_in_file eq "" ) { die "Please give input file" ; }
   if ( $p_fld_spr eq "" ) { die "Please give separtor( At least one character )" ; }

   # If file with path name remove path name
   # if( $p_in_file =~ )

   # Name of input file without extension
   my $v_index = index ( $p_in_file , '.' ) ;
   if ( $v_index != -1 ) {
      $o_file_nm_1st = substr ( $p_in_file , 0 , $v_index ) ;
   }

   # Extension  of input file
   if ( $v_index != -1 ) {
      $o_file_ext = substr ( $p_in_file , $v_index + 1 ) ;
   }

   # If Output file is null set default output file
   if ( $p_out_file eq "" ) {
      $p_out_file = $o_file_nm_1st . "_" . $o_file_ext . "_statistics.csv" ;
   }

   #Field separator contain only one character
   if ( length $p_fld_spr > 1 ) { die "Field separator contain only one character" ; }

} ## end sub sGetParameter
#######################################################################
# End of sub sGetParameter                                            #
#######################################################################

#######################################################################
# End of FileStts.pl                                                  #
#######################################################################

=pod

=head1 File Statistics

 File Statistics  about the file

=head2 Copyright

 Copyright (c) 2017 IdentLogic Systems Private Limited

=head2 Description
 File Statistics procedure will give you over all statistic report of a file 
 like first it will determine the datatype of all the fields. Base on that 
 it will show you count of null values per field number, Minimum of not null 
 field as per field number, record no of minimum of not null field as per field 
 number, maximum of not null field as per field number, record no of maximum of 
 not null field as per field number, count of occurences of invalid character in
 field as per field number, after cleaning count of null values per field number,
 no of Occcurence of fields as per field number,minimum length of field as per 
 field number, record no of minimum length of field as per field number,
 maximum length of field as per field number, record no of maxnimum length of 
 field as per field number, field wise count where optional enclosures used
 field wise count where optional enclosure used only in begining or at end of 
 field, field wise count where optional enclosure within string not escaped 
 (doubled) properly
 If the first record in the file contain header fields this procedure read the 
 file from second record otherwise it will start from the first record. and 
 If no header fields in file it will increment the fields using index.
 If the header fields are empty in somewhere it will fullfill that empty space
 using *** Field "field index" ***.

=head2 Output file
 Give the output file extension: .csv( comma separator values ) file
 If you do not put output file name with extension it will create Output file by
 default.
 for example ( inputFile_extension_statistics.csv )


=head2 Warning
 Alway give the input file name otherwise it will give an error.
 Field separtor should not be null
 Field separator contain only one character

=head2 Technical

 Package Number   -
 Procedure Number -
 Procedure Name   - File_Stat.pl

=head3 Run parameters

 PARAMETER                DESCRIPTION                         ABV VARIABLE
 input_file               Input file name with extension      i   $p_in_file
 field_separator          Field Separator                     f   $p_fld_spr
 data_from_1_record_flag  Data starting from the first record d   $p_f_dt_1
 output_file              Output file name with extension     o   p_out_file

 ABV - Abbreviation for calling run parameter

 Field separator is mandatory

=head4 Help and defaults

 For detailed help with defaults run: Perl <program_name> -h.

=head3 Perl modules used

 Getopt::Simple

=head3 Subroutines

 Subroutine      Description
 --------------  -----------------------------
 sGetFieldTypes  Gets field datatype.
 sGetParameter   Gets run parameters.
 
 
=head4 Called by

 Subroutine     Called by
 -------------  ----------
 sGetFieldTypes    Main
 sGetParameter     Main

=cut
