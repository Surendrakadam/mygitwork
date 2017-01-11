#!C:/Perl/bin/perl -w

use strict ;
use warnings ;
use Getopt::Std ;
use vars qw( $opt_u $opt_s $opt_r $opt_c ) ;

if( !getopts ( 'usrc' )) {
  die "Usage : 15-4.pl -u suren " ;
}

my $str = $ARGV[0] ;

if( $opt_u ) {
  $str = uc $str ;
}

if( $opt_s ) {
  $str =~ s/[\.,;:?\'\"]//g ;
}

if( $opt_r ) {
  $str = reverse $str  ;
}

if( $opt_c ) {
  $str = length $str ;
}
 
print "$str\n" ;
###### Execute : 15-4.pl -u suren (only once at a time)