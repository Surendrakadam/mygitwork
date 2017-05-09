#!C:/Perl/bin/perl.EXE -w

use strict ;
use warnings ;

use Getopt::Simple ;

# Application   : Dedupe
# Client        : Internal
# Copyright (c) : 2017 IdentLogic Systems Private Limited
# Author        : Surendra Kadam
# Creation Date : 9 May 2017
# Description   : Run all match procedure together or individually which are 44_515_SqhMtc.pl
#                  44_516_SortMtc.pl and 44_517_SqhMtc.s using 44_518_MtcAll.pl procedure.
# WARNINGS      :
# HARD CODINGS  :
# Limitations   :
# Dependencies  :
# Modifications
# Date       Change Req# Author       Description
# ---------- ----------- ------------ -------------------------------

use constant K_PACKAGE_NO         => 44 ;                       # Package
use constant K_PROCEDURE_NO       => 518 ;                      # Procedure
use constant K_PROCEDURE_NM_U     => "44_518_MtcAll.pl" ;       # Procedure name
use constant K_PROCEDURE_NM_SYS_U => "Run all match procedures" ; # Procedure description

use constant K_EMPTY  => "" ;                                   # Empty field
use constant K_NEW_LN => "\n" ;                                 # New Line Character
use constant K_Y      => "Y" ;                                  # Y for Yes
use constant K_n      => "n" ;                                  # n for no
use constant K_SPACE  => " " ;                                  # Space
use constant K_ONE    => 1 ;                                    # One
use constant K_ZERO   => 0 ;                                    # Zero

# Run parameters
my $p_data_set          = K_EMPTY ;                             # Data set                               - d
my $p_run_no            = K_EMPTY ;                             # Run number                             - r
my $p_prps_no           = K_EMPTY ;                             # Purpose number                         - p
my $p_input_file_dir    = K_EMPTY ;                             # Input file path                        - i
my $p_out_file_dir_sqh  = K_EMPTY ;                             # Output file directory of Squash        - a
my $p_out_file_dir_sort = K_EMPTY ;                             # Output file directory of Sort          - b
my $p_out_file_dir_upl  = K_EMPTY ;                             # Output file directory of upload        - c
my $p_log_file_dir_sqh  = K_EMPTY ;                             # Log file directory of Squash           - f
my $p_log_file_dir_sort = K_EMPTY ;                             # Log file directory of Sort             - g
my $p_log_file_dir_upl  = K_EMPTY ;                             # Log file directory of upload           - h
my $p_disp_mult         = K_EMPTY ;                             # Display progress multiple              - m
my $p_f_verbose         = K_EMPTY ;                             # Flag - Verbose - print details         - v
my $p_typ_mtc           = K_EMPTY ;                             # Type of match -Squash , Sort , Upload  - t

# File names
my $v_in_file  = K_EMPTY ;                                      # Input file name
my $v_out_file = K_EMPTY ;                                      # Output file name
my $v_log_file = K_EMPTY ;                                      # Log file name

# Procedures
my $v_sqh_prc  = K_EMPTY ;                                      # Squash Match procedure
my $v_sort_prc = K_EMPTY ;                                      # Sort Match procedure
my $v_upl_prc  = K_EMPTY ;                                      # Upload Match procedure

my $v_input_file_dir    = K_EMPTY ;                             # Input file directory
my $v_out_file_dir_sqh  = K_EMPTY ;                             # Output file directory of squash
my $v_log_file_dir_sqh  = K_EMPTY ;                             # Log file directory of squash

my $v_out_file_dir_sort = K_EMPTY ;                             # Output file directory of sort
my $v_log_file_dir_sort = K_EMPTY ;                             # Log file directory of sort

my $v_out_file_dir_upl  = K_EMPTY ;                             # Output file directory of upload
my $v_log_file_dir_upl  = K_EMPTY ;                             # Log file directory of upload

# File directories
my $v_sqh_inf_dir   = K_EMPTY ;                                 # Store Squash input file directory
my $v_sqh_outf_dir  = K_EMPTY ;                                 # Store Squash output file directory
my $v_sqh_logf_dir  = K_EMPTY ;                                 # Store Squash log file directory

my $v_sort_inf_dir  = K_EMPTY ;                                 # Store Sort input file directory
my $v_sort_outf_dir = K_EMPTY ;                                 # Store Sort output file directory
my $v_sort_logf_dir = K_EMPTY ;                                 # Store Sort log file directory

my $v_upl_inf_dir   = K_EMPTY ;                                 # Store Upload input file directory
my $v_upl_outf_dir  = K_EMPTY ;                                 # Store Upload output file directory
my $v_upl_logf_dir  = K_EMPTY ;                                 # Store Upload log file directory

sGetParameters() ;                                              # Call subroutine sGetParameters

### START #############################################################

if ( $p_typ_mtc eq K_EMPTY ) {
  $v_sqh_inf_dir   = $v_input_file_dir ;                        # Squash input file directory
  $v_sqh_outf_dir  = $v_out_file_dir_sqh ;                      # Squash output file directory
  $v_sqh_logf_dir  = $v_log_file_dir_sqh ;                      # Squash log file directory

  $v_sort_inf_dir  = $v_sqh_outf_dir ;                          # Sort input file directory
  $v_sort_outf_dir = $v_out_file_dir_sort ;                     # Sort output file directory
  $v_sort_logf_dir = $v_log_file_dir_sort ;                     # Sort log file directory

  $v_upl_inf_dir   = $v_sort_outf_dir ;                         # Upload input file directory
  $v_upl_outf_dir  = $v_out_file_dir_upl ;                      # Upload output file directory
  $v_upl_logf_dir  = $v_log_file_dir_upl ;                      # Upload log file directory
}

if ( $p_typ_mtc eq "SQUASH" or
     $p_typ_mtc eq K_EMPTY ) {

  $v_sqh_inf_dir   = $v_input_file_dir ;                        # Squash input file directory
  
  $v_sqh_prc  = "perl 44_515_SqhMtc.pl" . K_SPACE
              . "-d" . K_SPACE . $p_data_set . K_SPACE
              . "-r" . K_SPACE . $p_run_no . K_SPACE
              . "-p" . K_SPACE . $p_prps_no . K_SPACE
              .( $v_sqh_inf_dir eq K_EMPTY ? K_EMPTY :
                "-i" . K_SPACE . $v_sqh_inf_dir . K_SPACE )
              .( $v_sqh_outf_dir eq K_EMPTY ? K_EMPTY :
                "-o" . K_SPACE . $v_sqh_outf_dir . K_SPACE )
              .( $v_sqh_logf_dir eq K_EMPTY ? K_EMPTY :
                "-l" . K_SPACE . $v_sqh_logf_dir . K_SPACE )
              .( $p_disp_mult eq K_EMPTY ? K_EMPTY :
                "-m" . K_SPACE . $p_disp_mult . K_SPACE )
              .( $p_f_verbose eq K_n ? K_EMPTY :
                "-v" . K_SPACE . $p_f_verbose . K_SPACE ) ;
}

if ( $p_typ_mtc eq "SORT" or
          $p_typ_mtc eq K_EMPTY ) {

  $v_sort_inf_dir = $v_input_file_dir ;                         # Sort input file directory

  $v_sort_prc  = "perl 44_516_SortMtc.pl" . K_SPACE
              . "-d" . K_SPACE . $p_data_set . K_SPACE
              . "-r" . K_SPACE . $p_run_no . K_SPACE
              . "-p" . K_SPACE . $p_prps_no . K_SPACE
              .( $v_sort_inf_dir eq K_EMPTY ? K_EMPTY :
                "-i" . K_SPACE . $v_sort_inf_dir . K_SPACE )
              .( $v_sort_outf_dir eq K_EMPTY ? K_EMPTY :
                "-a" . K_SPACE . $v_sort_outf_dir . K_SPACE )
              .( $v_sort_logf_dir eq K_EMPTY ? K_EMPTY :
                "-l" . K_SPACE . $v_sort_logf_dir . K_SPACE )
              .( $p_f_verbose eq K_n ? K_EMPTY :
                "-v" . K_SPACE . $p_f_verbose . K_SPACE ) ;
}

if ( $p_typ_mtc eq "UPLOAD" or
          $p_typ_mtc eq K_EMPTY ) {

  $v_upl_inf_dir = $v_input_file_dir ;                          # Upload input file directory

  $v_upl_prc = "perl 44_517_UplMtc.pl" . K_SPACE
              . "-d" . K_SPACE . $p_data_set . K_SPACE
              . "-r" . K_SPACE . $p_run_no . K_SPACE
              . "-p" . K_SPACE . $p_prps_no . K_SPACE
              .( $v_upl_inf_dir eq K_EMPTY ? K_EMPTY :
                "-i" . K_SPACE . $v_upl_inf_dir . K_SPACE )
              .( $v_upl_outf_dir eq K_EMPTY ? K_EMPTY :
                "-a" . K_SPACE . $v_upl_outf_dir . K_SPACE )
              .( $v_upl_logf_dir eq K_EMPTY ? K_EMPTY :
                "-l" . K_SPACE . $v_upl_logf_dir . K_SPACE )
              .( $p_f_verbose eq K_n ? K_EMPTY :
                "-v" . K_SPACE . $p_f_verbose . K_SPACE ) ;
}

system ( $v_sqh_prc ) ;                                         # Run on cmd
system ( $v_sort_prc ) ;                                        # Run on cmd

exit (1) ;                                                      # Exit from procedure

#####################################################################
# End of Main                                                       #
#####################################################################

sub sGetParameters {

   # Gets run parameters

   my ( $vgp_get_options ) = {
      help => {
         type    => '' ,
         env     => '-' ,
         default => '' ,
         verbose => '' ,
         order   => 1 ,
        } ,
      dataset => {                                              # Data set number
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Data set number' ,
         order   => 2 ,
        } ,
      runno => {                                                # Run time number
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Run number' ,
         order   => 3 ,
        } ,
      purposeno => {                                            # Purpose number
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Purpose number' ,
         order   => 4 ,
        } ,
      infldir => {                                              # Input file directory
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Input file directory - optional' ,
         order   => 5 ,
        } ,
      aoutfldirsqh => {                                         # Output file directory of squash
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Output file directory of squash' ,
         order   => 6 ,
        } ,
      boutfldirsort => {                                        # Output file directory of sort
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Output file directory of sort' ,
         order   => 7 ,
        } ,
      coutfldirupl => {                                         # Output file directory of upload
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Output file directory of upload' ,
         order   => 8 ,
        } ,
      xlogfldirsqh => {                                         # Log file directory of squash
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Log file directory - optional' ,
         order   => 9 ,
        } ,
      ylogfldirsort => {                                        # Log file directory of sort
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Log file directory - optional' ,
         order   => 10 ,
        } ,
      zlogfldirupl => {                                         # Log file directory of upload
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Log file directory - optional' ,
         order   => 11 ,
        } ,
      multdspl => {                                             # Multiplier
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Display progress multiple - optional' ,
         order   => 12 ,
        } ,
      verboseflag => {                                          # Verbose flag
         type    => '!' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Print job details - no argument needed - optional' ,
         order   => 13 ,
      } ,
      typemtc=> {                                               # Type of match -Squash , Sort , Upload
         type    => '=s' ,
         env     => '-' ,
         default => '' ,
         verbose => 'Type of match -Squash , Sort , Upload' ,
         order   => 14 ,
        } ,
   } ;

   my ( $vgp_parameters ) = Getopt::Simple -> new () ;
   if ( ! $vgp_parameters -> getOptions ( $vgp_get_options , "Usage: 44_518_MtcAll.pl [options]" ) ) {
      exit ( -1 ) ;                                             # Failure
   }

   # Get run time parameters value
   $p_data_set =                                                # Dataset                               - d
     $$vgp_parameters{ 'switch' }{ 'dataset' } || K_EMPTY ;

   $p_run_no =                                                  # Run number                            - r
     $$vgp_parameters{ 'switch' }{ 'runno' } || K_EMPTY ;       #

   $p_prps_no =                                                 # Purpose number                        - p
     $$vgp_parameters{ 'switch' }{ 'purposeno' } || K_EMPTY ;   #

   $p_input_file_dir =                                          # Input file directory                  - i
     $$vgp_parameters{ 'switch' }{ 'infldir' } || K_EMPTY ;

   $p_out_file_dir_sqh =                                        # Output file directory of squash       - a
     $$vgp_parameters{ 'switch' }{ 'aoutfldirsqh' } || K_EMPTY ;

   $p_out_file_dir_sort =                                       # Output file directory of sort         - b
     $$vgp_parameters{ 'switch' }{ 'boutfldirsort' } || K_EMPTY ;

   $p_out_file_dir_upl =                                        # Output file directory of upload       - c
     $$vgp_parameters{ 'switch' }{ 'coutfldirupl' } || K_EMPTY ;

   $p_log_file_dir_sqh =                                        # Log file directory of squash          - f
     $$vgp_parameters{ 'switch' }{ 'xlogfldirsqh' } || K_EMPTY ;

   $p_log_file_dir_sort =                                       # Log file directory of sort            - g
     $$vgp_parameters{ 'switch' }{ 'ylogfldirsort' } || K_EMPTY ;

   $p_log_file_dir_upl =                                        # Log file directory of upload          - h
     $$vgp_parameters{ 'switch' }{ 'zlogfldirupl' } || K_EMPTY ;

   $p_disp_mult =                                               # Display progress multiple             - m
     $$vgp_parameters{ 'switch' }{ 'multdspl' } || K_EMPTY ;

   $p_f_verbose =                                               # Flag - Verbose - print details        - v
     $$vgp_parameters{ 'switch' }{ 'verboseflag' } ;

   $p_typ_mtc =                                                 # Type of match -Squash , Sort , Upload - t
     $$vgp_parameters{ 'switch' }{ 'typemtc' } ;

   if   ( $p_f_verbose eq '1' ) { $p_f_verbose = 'Y' ; }        # Convert to ISPL convention
   else                         { $p_f_verbose = 'n' ; }

   $v_input_file_dir  = ( $p_input_file_dir eq K_EMPTY  ? './' : $p_input_file_dir ) ;

   $v_out_file_dir_sqh  = ( $p_out_file_dir_sqh eq K_EMPTY ? './' : $p_out_file_dir_sqh ) ;
   $v_out_file_dir_sort = ( $p_out_file_dir_sort eq K_EMPTY ? './' : $p_out_file_dir_sort ) ;
   $v_out_file_dir_upl  = ( $p_out_file_dir_upl eq K_EMPTY ? './' : $p_out_file_dir_upl ) ;

   $v_log_file_dir_sqh  = ( $p_log_file_dir_sqh eq K_EMPTY    ? './' : $p_log_file_dir_sqh ) ;
   $v_log_file_dir_sort = ( $p_log_file_dir_sort eq K_EMPTY    ? './' : $p_log_file_dir_sort ) ;
   $v_log_file_dir_upl  = ( $p_log_file_dir_upl eq K_EMPTY    ? './' : $p_log_file_dir_upl ) ;

   if ( $p_data_set eq K_EMPTY ) {                              # Abort if data set not specified
      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . ' - ' . "Data set number not specified" . K_NEW_LN ;
   }

   if (  $p_data_set =~ /\D/i
      or $p_data_set < 100
      or $p_data_set > 999 ) {                                  # Abort if Dataset number not between 100 and 999

      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . ' - ' . "Dataset number must be integer and in a range of 100 and 999" . K_NEW_LN ;
   } ## end if ( $p_data_set =~ /\D/i...)

   if ( $p_run_no eq K_EMPTY ) {                                # Abort if Run number not specified

      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . ' - ' . "Run time number not specified" . K_NEW_LN ;
   }

   if (  $p_run_no =~ /\D/i
      or $p_run_no < 1000
      or $p_run_no > 9999 ) {                                   # Abort if Run number not between 1000 and 9999

      die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
        . __LINE__ . ' - ' . "Run number must be integer and in a range of 1000 and 9999" . K_NEW_LN ;
   } ## end if ( $p_run_no =~ /\D/i...)

   if ( $p_prps_no eq K_EMPTY ) {
     die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE             #
        . __LINE__ . ' - ' . "Purpose number not specified" . K_NEW_LN ;
   }

   if ( $p_prps_no =~ /\D/i ) {
     die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE             #
        . __LINE__ . ' - ' . "Purpose number should be integer" . K_NEW_LN ;
   }

   if ( $v_input_file_dir ne './' ) {                           # If input file directory not default (./)
      if (
         substr ( $v_input_file_dir , -1 , 1 ) ne '\\'          # If input file directory not end with back slash (\\)
        ) {
         $v_input_file_dir .= '\\' ;                            # Add back slash at end
      }
   }

   $v_input_file_dir =~ s/\\/\//g ;                             # Replace back slash to forward slash

   if ( $v_out_file_dir_sqh ne './' ) {                         # If Squash output file directory not default (./)
      if (
         substr ( $v_out_file_dir_sqh , -1 , 1 ) ne '\\'        # If Squash output file directory not end with back slash (\\)
        ) {
         $v_out_file_dir_sqh .= '\\' ;                          # Add back slash at end
      }
   }

   $v_out_file_dir_sqh =~ s/\\/\//g ;                           # Replace back slash to forward slash

   if ( $v_out_file_dir_sort ne './' ) {                        # If Sort output file directory not default (./)
      if (
         substr ( $v_out_file_dir_sort , -1 , 1 ) ne '\\'       # If Sort output file directory not end with back slash (\\)
        ) {
         $v_out_file_dir_sort .= '\\' ;                         # Add back slash at end
      }
   }

   $v_out_file_dir_sort =~ s/\\/\//g ;                          # Replace back slash to forward slash

   if ( $v_out_file_dir_upl ne './' ) {                         # If Upload output file directory not default (./)
      if (
         substr ( $v_out_file_dir_upl , -1 , 1 ) ne '\\'        # If Upload output file directory not end with back slash (\\)
        ) {
         $v_out_file_dir_upl .= '\\' ;                          # Add back slash at end
      }
   }

   $v_out_file_dir_upl =~ s/\\/\//g ;                           # Replace back slash to forward slash

   if ( $v_log_file_dir_sqh ne './' ) {                         # If Squash log file directory not default (./)
      if (
         substr ( $v_log_file_dir_sqh , -1 , 1 ) ne '\\'        # If Squash log file directory not end with back slash (\\)
        ) {
         $v_log_file_dir_sqh .= '\\' ;                          # Add back slash at end
      }
   }

   $v_log_file_dir_sqh =~ s/\\/\//g ;                           # Replace back slash to forward slash

   if ( $v_log_file_dir_sort ne './' ) {                        # If Sort log file directory not default (./)
      if (
         substr ( $v_log_file_dir_sort , -1 , 1 ) ne '\\'       # If Sort log file directory not end with back slash (\\)
        ) {
         $v_log_file_dir_sort .= '\\' ;                         # Add back slash at end
      }
   }

   $v_log_file_dir_sort =~ s/\\/\//g ;                          # Replace back slash to forward slash

   if ( $v_log_file_dir_upl ne './' ) {                         # If Upload log file directory not default (./)
      if (
         substr ( $v_log_file_dir_upl , -1 , 1 ) ne '\\'        # If Upload log file directory not end with back slash (\\)
        ) {
         $v_log_file_dir_upl .= '\\' ;                          # Add back slash at end
      }
   }

   $v_log_file_dir_upl =~ s/\\/\//g ;                           # Replace back slash to forward slash

   if ( $p_disp_mult eq K_EMPTY ) {                             # Default value of multiplier
      $p_disp_mult = 100000 ;
   }

   if ( $p_disp_mult ne K_EMPTY ) {
     if ( $p_disp_mult =~ /\D/ ) {
       die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE           #
        . __LINE__ . ' - ' . "Multiplier number should be integer" . K_NEW_LN ;
     }
   }
   if ( $p_typ_mtc ne K_EMPTY ) {
     $p_typ_mtc = uc ( $p_typ_mtc ) ;
     if ( $p_typ_mtc ne "SQUASH" and
          $p_typ_mtc ne "SORT" and
          $p_typ_mtc ne "UPLOAD" ) {
       die __PACKAGE__ . K_SPACE . __FILE__ . K_SPACE            #
          . __LINE__ . ' - ' . "Type of match must be either SORT , SQUASH or UPLOAD" . K_NEW_LN ;
     }
   }

} ## end sub sGetParameters
#######################################################################
# End of subroutine sGetParameters                                    #
#######################################################################

=pod

=head1 44_518_MtcAll.pl - Match All

 Run all match procedure together or individually

=head2 Copyright

 Copyright (c) 2017 IdentLogic Systems Private Limited

=head2 Description

 Run all match procedure together or individually which are 44_515_SqhMtc.pl
  44_516_SortMtc.pl and 44_517_SqhMtc.s using 44_518_MtcAll.pl procedure.
 
 1. I taken only one input file directory from run parameter because 
 2. Input file directory of Sort will be Output file directory of Squash
 3. Input file directory of Upload will be Output file directory of Sort 

=head3 Checks leading to procedure abort

   i. Data set number not specified
  ii. Dataset number must be integer and in a range of 100 and 999
 iii. Run time number not specified
  iv. Run number must be integer and in a range of 1000 and 9999
   v. Purpose number not specified
  vi. Purpose number should be integer
 vii. Multiplier number should be integer
viii. Type of match must be either SORT , SQUASH or UPLOAD
  xi. Input file not exist

=head2 Technical

 Script name      - 44_518_MtcAll.pl
 Package Number   - 44
 Procedure Number - 518

=head3 Run parameters

 PARAMETER      DESCRIPTION                       ABV  VARIABLE
 ---------      --------------------------------- ---  -------------------
 aoutfldirsqh  Output file directory of squash          a   $p_out_file_dir_sqh
 boutfldirsort Output file directory of sort            b   $p_out_file_dir_sort
 coutfldirupl  Output file directory of upload          c   $p_out_file_dir_upl
 dataset       Dataset number                           d   $p_data_set
 xlogfldirsqh  Log file directory of squash             x   $p_log_file_dir_sqh
 ylogfldirsort Log file directory of sort               y   $p_log_file_dir_sort
 zlogfldirupl  Log file directory of upload             z   $p_log_file_dir_upl
 infldir       Input file directory                     i   $p_input_file_dir
 multdspl      Display progress multiple                m   $p_disp_mult
 purposeno     Purpose number                           p   $p_prps_no
 runno         Run number                               r   $p_run_no
 typemtc       Type of match -Squash , Sort , Upload    t   $p_typ_mtc
 verboseflag   Flag - Verbose - print details *         v   $p_f_verbose

 * - No argument needed

 Parameter Dataset number(d) and Run number (r) and Purpose number (p) are mandatory.

 ABV:- Abbreviation for calling run parameter,

 e.g. 44_518_MtcAll.pl -d 100 -r 1000 -p 119<RESIDENT>
 
 Terminal op: 
 
   Squash Match Ended 2017-05-09 14:25:49- Processed 28 records in 00:00:00 to execute

   Match sorting ended 2017-05-09 14:25:50 - 00:00:00 to execute

=head4 Help and defaults

 For detailed help with defaults run: Perl <program_name> -h.

=head3 Subroutines

 Subroutine          Description
 ------------------  -----------------------------------------------------------
 sGetParameters      Initial: Gets run parameters and check input parameter
                      values. Procedure abort with message if any error.
                      
=head4 Called by

 Subroutine          Called by
 ----------------    ------------------------------------
 sGetParameters      Main

=head4 Subroutine structure

 Main
  |-- sGetParameters

=head3 Perl modules used

 Getopt::Simple

=cut

#######################################################################
# End of 44_518_MtcAll.pl                                             #
#######################################################################
