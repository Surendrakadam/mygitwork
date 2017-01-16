#!C:/Perl/bin/perl -w 

use strict ;
use warnings ;

my $listref ;

#my @board =
#(
#[1,2,3,4,5],
#[6,7,8,9,10]
#);

my @board = (12,13,15,[1,2,3],[4,5,6]) ;

&reverseall ( @board ) ;

sub reverseall {
  foreach $listref (@_) {
    if( ref ( $listref ) eq "ARRAY" ) {
      my @templist = reverse @$listref ;
      print "@templist\n" ;
    }
    else {
     print "$listref is not a list\n" ;
    }
  } 
  return @_ ;
}