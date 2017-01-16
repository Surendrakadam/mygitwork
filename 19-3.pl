#!C:/Perl/bin/perl -w

use strict ;
use warnings ;
my @refa = () ;
my @arr =
(
[ 3, 4, 2, 4, 2, 3 ] ,
[ 5, 3, 2, 4, 5 ,4 ] ,
[ 7, 6, 3, 2, 8, 3 ] ,
[ 3, 4, 7, 8, 3, 4 ]
);
print "-----ARRAY-----\n" ;
foreach my $key ( @arr ) {
  print "@{$key} \n" ;
}
print "\n" ;

print "----Output------\n" ;
&rect( \@arr ,0, 0, 3, 3 ) ;
sub  rect {
	my $refa = shift ;
  my ( $r, $c, $width, $height ) = @_ ;
  
  for ( my $i=$r; $i < $width ; $i ++ ) { 
    for (my $j=$c ; $j < $height ; $j ++ ) {
      print "$$refa[$i][$j] " ;
    }
    print "\n" ;
  }
}
