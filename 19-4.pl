#!C:/Perl/bin/perl -w

use strict ;
use warnings ;


my @board =
(
[0,0,0,0,0],
[0,0,0,0,0],
[0,0,0,0,0],
[0,0,0,0,0],
[0,0,0,0,0]
);

my $hit    = 0 ;
my @coords = &inita() ;

while() {
   &print_board() ;
   my @choice = &get_coords() ;
   if( &compare_coords ( @choice, @coords )) {
    print "Congratulation ! you sank the battleship\n" ;
    last ;
  }
  else {
    print "Miss ! Try again.\n" ;
    &mark_board ( @choice ) ;
  }
}


sub inita {
my $num1 = int ( rand 5 ) ;
my $num2 = int ( rand 5 ) ;
return ( $num1 , $num2 ) ;
}

sub print_board {
  my $i = 1 ;
  print "\n   A B C D E\n" ;
  print "--------------\n" ;
  foreach my $key ( @board ) {
    print "$i| @{$key} \n" ;
    $i++;
  }
  print "--------------\n" ;
}

sub get_coords {
   my ( $c1 , $c2 ) ;
   my $coords ;
   while() {
     print "Enter the coordinates of your  choice (eg A4):" ;
     chomp ( $coords = <> );
     ( $c1, $c2 ) = split ( '' ,$coords) ;
     $c1 = uc $c1 ;
     if ( $c1 !~ /[ABCDE]/i ) {
       print "Invalid letter coordinate. A-E ,please.\n" ;
     }
     elsif ( $c2 !~ /[1-5]/ ){
       print "Invalid number coordinate. 1-5 ,please.\n" ;
     }
     else { last ; }
   }
   ( $c1 eq 'A' ) and $c1 = 0 ;
   ( $c1 eq 'B' ) and $c1 = 1 ;
   ( $c1 eq 'C' ) and $c1 = 2 ;
   ( $c1 eq 'D' ) and $c1 = 3 ;
   ( $c1 eq 'E' ) and $c1 = 4 ;
    $c2 -- ;

    return ($c1 ,$c2) ;
}

sub compare_coords {
  my ( $c1, $c2, $d1, $d2 ) = @_ ;
  if( $c1 == $d1 and $c2 == $d2 ) { return 1 ; }
  else { return 0; }
}

sub mark_board {
  my ( $c2, $c1 ) = @_ ;
  $board [ $c1 ][ $c2 ] = 'X' ;
}
