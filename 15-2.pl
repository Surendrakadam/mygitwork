#!C:/Perl/bin/perl -w

use strict ;
use warnings ;

my ($file1 ,$file2) = @ARGV ;
my $extension = '' ;

if ( $file1 =~ /\.(\w+)$/ ) {
   $extension = $1 ;
   if ( $file2 !~ $extension ) {
     die "Extensions are different in both files" ;
   }
}
open ( FILE1 , $file1 ) or die "Can not open file1: $!\n" ;
open ( FILE2 , $file2 ) or die "Can not open file2: $!\n" ;
open ( MERGE , ">" ,"Merge.$extension" ) or die "Can not open Merged file: $!\n" ;   # > operator Write a file

my $line1 = <FILE1> ;
my $line2 = <FILE2> ;

while( defined ( $line1 ) || defined ( $line2 )) {
  if( defined ( $line1 )) {
    print MERGE $line1 ;
    $line1 = <FILE1> ;
  }
  if( defined ( $line2 )) {
    print MERGE $line2 ;
    $line2 = <FILE2> ;
  }
}
##### Execute : 15-2.pl odd.txt even.txt