#19-1.pl

  # One liner script : perl -e "while(<>){ while(/t/g) {$c++;}}; print $c;" count.txt
  # Output           : 3

#19-2.pl

  # One liner script : perl -e "while(<>){ $sum+=$_;} print "$sum";" odd.txt
  # Output           : 10

#19-3.pl

  #1 One liner script : perl -pi.txt -e "s/   /\t/g" space.txt
  #2 One liner script : perl -pi.txt -e "s/\s{3}/\t/g" space.txt
  # Output           : 

#19-4.pl

  # One liner script : perl -p "s/t/T/q"
  # Bug solved       : perl -p -e "s/t/T/g" space.txt

#19-5.pl
  
  # One liner script : perl -ne "print "line: ",reverse $_;"
  # Bug solved       : perl -ne "print scalar(reverse $_);" odd.txt
  
#----------------------------------------------------------------------#

# NOTE #
# -n command line argument that loops over the input
# but unlike -p doesn not print the lines automatically, so you have to use print explicitly.