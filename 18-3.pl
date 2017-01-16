#!C:/Perl/bin/perl -w

use strict ;
use warnings ;

my @list = `dir` ;
foreach (@list) {
  if( /^\w/ ) {
    print "substr($_,39)\n";
  }
}