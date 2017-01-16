#!C:/Perl/bin/perl -w

use strict ;
use warnings ;

package SimpleClass ;                                            # Class name SimpleClass

sub new {                                                        # new Constructor
  my ( $classname, $a, $b, $c ) = @_ ;                           # Three instance variable $a, $b, $c
  my $self = {} ;
  $self -> { a } = $a ;                                          # Initialize variable a
  $self -> { b } = $b ;                                          # Initialize variable b
  $self -> { c } = $c ;                                          # Initialize variable c
  return bless $self ,$classname ;
}

sub getA {
  my $self = shift ;
  return $self -> { a } ;
}

sub setA {
  my $self =shift ;
  if( @_ ) {
    $self -> { a } =shift ;
  }
  else {
    warn "no arguement\n" ;
    $self -> { a } = undef ;
  }
}

sub getB {
  my $self = shift ;
  return $self -> { b } ;
}

sub setB {
  my $self =shift ;
  if( @_ ) {
    $self -> { b } =shift ;
  }
  else {
    warn "no arguement\n" ;
    $self -> { b } = undef ;
  }
}

sub getC {
  my $self = shift ;
  return $self -> { c } ;
}
sub setC {
  my $self =shift ;
  if( @_ ) {
    $self -> { c } =shift ;
  }
  else {
    warn "no arguement\n" ;
    $self -> { c } = undef ;
  }
}

sub sum {
  my $self = shift ;
  my $sum  = 0 ;
  foreach ( 'a', 'b', 'c' ) {
    if( $self -> {$_} !~ /^\d+/ ) {
      warn "\nvariable '$_' does not contain a number\n" ;
    }
    else {
    $sum += $self -> {$_} ;
    }
  }
  return $sum ;
}

package main ;
my $obj = new SimpleClass( 33, 0, 30 ) ;

print "A: ",$obj -> getA() ,"\n" ;
#$obj -> setA("foo") ;
#print "A: ",$obj -> getA() ,"\n" ;

print "B: ",$obj -> getB() ,"\n" ;
#$obj -> setB("foo") ;
#print "B: ",$obj -> getB() ,"\n" ;

print "C: ",$obj -> getC() ,"\n" ;
#$obj -> setC("foo");
#print "C: ",$obj -> getC() ,"\n" ;


print "\nsum:",$obj -> sum(),"\n" ;

###################################
