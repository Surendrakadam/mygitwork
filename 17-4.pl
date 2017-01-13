#!C:/Perl/bin/perl -w

use strict ;
use warnings ;

print "Please choose one\n" ;
print "1.Create a file\n" ;
print "2.Rename a file\n" ;
print "3.Delete a file\n" ;
print "4.List all files\n" ;
print "5.Quit\n";

while ( ){
  my $input = '' ;
  print "\nEnter your choice :" ;
  chomp ( $input = <STDIN> ) ;
  
  if ( $input =~ /^[12345]$/ )
  {
    if( $input == 1) {
      &CreateFile() ;
    }
    elsif( $input == 2 ) {
      &RenameFile() ;
    }
    elsif( $input == 3 ) {
      &DeleteFile() ;
    }
    elsif( $input == 4 ) {
      &LisTOfFiles() ;
    }
    elsif( $input == 5 ) {
      last ;
    }
  }
  else {
  	print "Plz enter the choice from 1 to 5\n" ;
  }
}

sub CreateFile {
  my $filename = '' ;
  print "\nEnter a file name for creating:" ;
  chomp ( $filename = <STDIN> ) ;
  if ( -e $filename ) {
    print "File $filename is already exists\n" ;
  }
  else {
    open ( FILE ,">", $filename ) or die "can not open file" ;
    close (FILE) ;
    print "File is created : $filename\n" ;
  }
}

sub RenameFile {
  my $filename   = '' ;
  my $filerename = '' ;
  START:
  print "\nEnter a old file name: ";
  chomp ( $filename = <STDIN> ) ;
  if( -e $filename ) {
    print "Enter a new name for rename the file : " ;
    chomp ( $filerename = <STDIN> ) ;
    if( rename ( $filename , $filerename )) {
      print " File $filename renamed to $filerename\n" ;
    }
    else {
      warn "Can not rename $filename file\n" ;
    }
  }
  else {
    print "Oops $filename is not existed\n";
    goto START ;
  }
}

sub DeleteFile {
  my $filename = '' ;
  print "\nEnter the file name for deleting:" ;
  chomp ( $filename = <STDIN> ) ;
  if( -e $filename ) {
    if( unlink $filename) {
      print "File $filename removed.\n" ;
    }
    else {
      warn "Can not delte $filename file\n" ;
    }
  }
  else {
    print "Oops $filename is not existed\n";
  }
}

sub LisTOfFiles {
    opendir ( CURRENTDIR ,'.') or die "Can not open directory:$!" ;
    my @files = readdir ( CURRENTDIR ) ;
    close (CURRENTDIR) ;
    foreach my $file ( sort @files ) {
      if(-f $file) {                                                         # Check file is a directory or not
        print "$file\n" ;
      }
    }
}