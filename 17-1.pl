#!C:/Perl/bin/perl -w

use strict ;
use warnings ;
use Cwd ;

my $curdir = cwd();                                                      # Current directory
print "Current Directory: $curdir" ;
opendir ( CURRENTDIR , $curdir) or die "Can not open directory:$!";
my @files = readdir ( CURRENTDIR ) ;                                     #read the directory
close (CURRENTDIR) ;

foreach my $file ( sort @files ) {
  if(-d $file) {                                                         # Check file is a directory or not
    print "$file/ \n";
  }
  else{
    print "$file \n";
  }
}

