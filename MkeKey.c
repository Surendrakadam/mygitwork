/* cl MkeKey.c stssan3cl.lib

 Creates Key file sssrrrr.??? from tagged file sssrrrr.tag.

 Format:
  Column  1 to 8 : Key
  Column  9      : Key type - P (Person_name), O (Organisation), 1 (Address_Part1)
  Column 10      : Key level - M (Limited), S (Standard), X (Extended)
  Column 11 onward : Id

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ssan3cl.h"                                            // Dedupe header
#include <getopt.h>
#include <unistd.h>
#include <time.h>

#define rangeof(arr)  (sizeof(arr)/sizeof((arr)[0]))

  // Global variables
  long  rc ;
  long  sockh = -1 ;
  long  session_id = -1 ;
  
  // Procedure parameter
  char *p_data_set ;                                            // Parameter data set number
  char *p_run_time ;                                            // Parameter run time data
  char *p_infdir ;                                              // Parameter input file name
  char *p_outfdir ;                                             // Parameter output file directory
  char *p_logfdir ;                                             // Parameter log file directory

  // File Open Status
  FILE  *input_fopen_status ;                                   // Input file name
  FILE  *output_fopen_status ;                                  // Output file name
  FILE  *log_fopen_status ;                                     // Log file name
  
  // File names
  char *infname;                                                // Input file name
  char *ofname;                                                 // Output file name
  char *logfname;                                               // Log file name
  
  // File Directories
  char input_file_directory[500] ;                              // Input file directory
  char output_file_directory[500];                              // Output file directory
  char log_file_direcory[500];                                  // Log file directory
  
  
  char  verbose_flag ;                                          // Verbose flag
  int cur_rec_len ;                                             // Current record length
  int rec_number = 0 ;                                          // Record counter
  int i ;

  char current_rec [ BUFSIZ ] ;                                 // Current record of a file
  char *p_population ;                                          // Population name

static long test_ssa_open (
  long    sockh ,
  long    *session_id ,
  char    *sysName ,
  char    *population ,
  char    *controls
) {

  long    rc ;
  char    rsp_code[SSA_SI_RSP_SZ] ;
  char    ssa_msg[SSA_SI_SSA_MSG_SZ] ;

  printf ("------ ssan3_open ------\n") ;  //-- log ...
  printf ("Session Id       : %ld\n" , *session_id) ;
  printf ("System           : %s\n" , sysName) ;
  printf ("Population       : %s\n" , population) ;
  printf ("Controls         : %s\n" , controls) ; // -- n/a

  rc = ssan3_open (sockh ,
    session_id ,
    sysName ,
    population ,
    controls ,
    rsp_code ,
    SSA_SI_RSP_SZ ,
    ssa_msg ,
    SSA_SI_SSA_MSG_SZ) ;
  if (rc < 0) {
    printf ("rc               : %ld\n" , rc) ;
    rc = 1 ;
    goto ret ;
  }
  if (rsp_code[0] != '0' && *session_id == -1) {
    printf ("rsp_code         : %s\n" , rsp_code) ;
    printf ("ssa_msg          : %s\n" , ssa_msg) ;
    rc = 1 ;
    goto ret ;
  }
  printf ("\n") ;
  printf ("Session Id       : %ld\n" , *session_id) ;
  printf ("rsp_code         : %s\n" , rsp_code) ;
  printf ("ssa_msg          : %s\n" , ssa_msg) ;  //-- ???
  rc = 0 ;
  goto ret ;

ret:
  return (rc) ;
}

static long test_ssa_get_keys (

  long    sockh ,
  long    *session_id ,
  char    *sysName ,
  char    *population ,
  char    *controls ,
  char    *record ,
  long    recordLength ,
  char    *recordEncType)
{
  int   i ;
  long    rc ;
  char    *keys_array[SSA_SI_MAX_KEYS] ;
  char    keys_data[SSA_SI_MAX_KEYS*SSA_SI_KEY_SZ] ;
  char    rsp_code[SSA_SI_RSP_SZ] ;
  char    ssa_msg[SSA_SI_SSA_MSG_SZ] ;
  long    num ;
  char    *p ;

  for (i = 0 ; i < (int)rangeof (keys_array) ; ++i)
    keys_array[i] = keys_data + i * SSA_SI_KEY_SZ ;

  printf ("------ ssan3_get_keys_encoded ------\n") ;  //-- suppress ... ???
  printf ("Session Id       : %ld\n" , *session_id) ;
  printf ("System           : %s\n" , sysName) ;
  printf ("Population       : %s\n" , population) ;
  printf ("Controls         : %s\n" , controls) ;
  printf ("Key field data   : %s\n" , record) ;
        printf ("Key field size   : %ld\n" , recordLength) ;
        printf ("Key field encoding type : %s\n" , recordEncType) ;

  num = 0 ;
  rc = 
    ssan3_get_keys_encoded (
      sockh ,
      session_id ,
      sysName ,
      population ,
      controls ,
      rsp_code ,
      SSA_SI_RSP_SZ ,
      ssa_msg ,
      SSA_SI_SSA_MSG_SZ ,
      record , 
      recordLength , 
      recordEncType ,
      &num ,
      keys_array ,
      SSA_SI_KEY_SZ
    ) ;
    
  if (rc < 0) {
    printf ("rc               : %ld\n" , rc) ;
    rc = 1 ;
    goto ret ;
  }
  if (rsp_code[0] != '0') {
    printf ("rsp_code         : %s\n" , rsp_code) ;
    printf ("ssa_msg          : %s\n" , ssa_msg) ;
    rc = -1 ;
    goto ret ;
  }
  printf ("\n") ;
  printf ("Session Id       : %ld\n" , *session_id) ;
  printf ("rsp_code         : %s\n" , rsp_code) ;
  printf ("ssa_msg          : %s\n" , ssa_msg) ;
  printf ("Count            : %ld\n" , num) ;
  printf ("------ keys ------\n") ;
  ++rec_number ;
  fprintf(output_fopen_status ,"\n%d :" ,rec_number) ;
  for (i = 0 ; i < num ; ++i) {
    p = keys_array[i] ;
    fprintf ( output_fopen_status ,"'%.*s'" , SSA_SI_KEY_SZ , p) ;
    printf ("%3d '%.*s'\n" , i+1 , SSA_SI_KEY_SZ , p) ;  //-- OUTPUT
  }
  printf ("\n") ;
  rc = 0 ;
  goto ret ;

ret:
  return (rc) ;
}

static long
test_ssa_close (
  long    sockh ,
  long    *session_id ,
  char    *sysName ,
  char    *population ,
  char    *controls)
{
  long    rc ;
  char    rsp_code[SSA_SI_RSP_SZ] ;
  char    ssa_msg[SSA_SI_SSA_MSG_SZ] ;

  printf ("------ ssan3_close ------\n") ;
  printf ("Session Id       : %ld\n" , *session_id) ;
  printf ("System           : %s\n" , sysName) ;
  printf ("Population       : %s\n" , population) ;
  printf ("Controls         : %s\n" , controls) ;

  rc = ssan3_close (sockh ,
    session_id ,
    sysName ,
    population ,
    controls ,
    rsp_code ,
    SSA_SI_RSP_SZ ,
    ssa_msg ,
    SSA_SI_SSA_MSG_SZ) ;
  if (rc < 0) {
    printf ("rc               : %ld\n" , rc) ;
    rc = 1 ;
    goto ret ;
  }
  if (rsp_code[0] != '0') {
    printf ("rsp_code         : %s\n" , rsp_code) ;
    printf ("ssa_msg          : %s\n" , ssa_msg) ;
    rc = 1 ;
    goto ret ;
  }
  printf ("\n") ;
  printf ("Session Id       : %ld\n" , *session_id) ;
  printf ("rsp_code         : %s\n" , rsp_code) ;
  printf ("ssa_msg          : %s\n" , ssa_msg) ;
  rc = 0 ;
  goto ret ;

ret:
  return (rc) ;
}

static void doExit ( char *func) {
  printf ("Error occurred in '%s'\n" , func) ;
  exit (EXIT_FAILURE) ;
}

static void s_mkeKey_open () {

    // Establish a session.
    rc = test_ssa_open (
      sockh ,
      &session_id ,
      "default" ,
      p_population ,
      "") ;
    if (0 != rc)
      doExit ("test_ssa_open") ;

}

static void s_mkeKey_getKey (
  char *Limited ,
  char *Standard ,
  char *Extended ,
  char *ky_fld_ky_lvl1 ,
  char *ky_fld_ky_lvl2 ,
  char *ky_fld_ky_lvl3
) {

  char *key_level [ 3 ] = { Limited , Standard , Extended } ;
  char *ky_fld_ky_lvl [ 3 ] = { ky_fld_ky_lvl1 , ky_fld_ky_lvl2 , ky_fld_ky_lvl3 } ;

  for( i = 0 ; i <= 2 ; i++ ) {
    rc =
      test_ssa_get_keys (
        sockh ,
        &session_id ,
        "default" ,
        p_population ,
        key_level[i] , //"FIELD=Person_Name KEY_LEVEL=Extended"
        current_rec ,
        cur_rec_len ,
        "TEXT"
      ) ;
    fprintf ( output_fopen_status , "%s" , ky_fld_ky_lvl [ i ] ) ;
    if ( 0 != rc )
      doExit ( "test_ssa_get_keys" ) ;
  }

}

static void s_mkeKey_close ( ) {
  rc = test_ssa_close (
    sockh ,
    &session_id ,
    "default" ,
    p_population ,
    "") ;

  if (0 != rc)
    doExit ("test_ssa_close") ;
}

void print_usage() {
    printf("MkeKey -d 101 -r 1001 -p india -o output.txt -l log.txt") ;
}

int main ( int argc , char *argv [ ] ) {

 int   option = 0 ;
 char cpy_infname[400];                                         // Copy of Input file name
 char cpy_ofname[400];                                          // Copy of output file name
 char cpy_logfname[400];                                        // Copy of log file name
 
 char *sss_rrrr ;                                               // data set number and run time number
 char sss_rrrr_in_file[100];                                    // Copy of sssrrrr for making input file
 char sss_rrrr_out_file[100] ;                                  // Copy of sssrrrr for making output file
 char sss_rrrr_log_file[100] ;                                  // Copy of sssrrrr for making log file
 
 char *dir_in_fl_ext;                                           // Directory with input file
 char *dir_out_fl_ext;                                          // Directory with output file
 char *dir_log_fl_ext;                                          // Directory with log file
 
 char cpy_dir_in_fl_ext[500];                                   // copy of Directory with input file                                    
 char cpy_dir_out_fl_ext[500];                                  // copy of Directory with output file  
 char cpy_dir_log_fl_ext[500];                                  // copy of Directory with log file     
 
  while ( ( option = getopt ( argc , argv , "d:r:p:i:o:l:" ) ) != -1 ) {
    switch (option) {
      case 'd' :
        p_data_set   = optarg ;
        break ;
      case 'r' :
        p_run_time   = optarg ;
        break ;
      case 'p' :
        p_population = optarg ;
        break ;
      case 'i' :
        p_infdir     = optarg ;
        break ;
      case 'o' :
        p_outfdir    = optarg ;
        break ;
      case 'l' :
        p_logfdir    = optarg ;
        break ;
      default:
        print_usage ( ) ;
        exit ( EXIT_FAILURE ) ;
    }

  }
  
  strcpy( input_file_directory ,p_infdir );   
  strcpy( output_file_directory ,p_outfdir );                       
  strcpy( log_file_direcory ,p_logfdir );
  
  sss_rrrr = strcat(p_data_set ,p_run_time) ;                   // Concatenate data set number and run time number
  
  strcpy(sss_rrrr_in_file , sss_rrrr) ;                         // Copy value of sss_rrrr to sss_rrrr_in_file
  strcpy(sss_rrrr_out_file , sss_rrrr) ;                        // Copy value of sss_rrrr to sss_rrrr_out_file
  strcpy(sss_rrrr_log_file , sss_rrrr) ;                        // Copy value of sss_rrrr to sss_rrrr_log_file
  
  infname = strcat(sss_rrrr_in_file ,".tag");                   // Input file name with extension
  ofname  = strcat(sss_rrrr_out_file,".oke" ) ;                 // Output file name with extension
  logfname= strcat(sss_rrrr_log_file,".log" );                  // Log file name with extension
  
  strcpy( cpy_infname,infname );                                // Copy of Input file name
  strcpy( cpy_ofname,ofname );                                  // Copy of output file name
  strcpy( cpy_logfname,logfname );                              // Copy of log file name
  
  // Concatenate Directory path with Input file name
  dir_in_fl_ext = strcat( input_file_directory ,cpy_infname );
  strcpy(cpy_dir_in_fl_ext,dir_in_fl_ext);
  
  // Concatenate Directory path with output file name
  dir_out_fl_ext = strcat( output_file_directory, cpy_ofname );
  strcpy(cpy_dir_out_fl_ext,dir_out_fl_ext);
  
  // Concatenate Directory path with log file name
  dir_log_fl_ext = strcat( log_file_direcory, cpy_logfname );
  strcpy(cpy_dir_log_fl_ext,dir_log_fl_ext);
  
  
  input_fopen_status   = fopen ( cpy_dir_in_fl_ext, "r" ) ;     // Open and read input file
  output_fopen_status  = fopen ( cpy_dir_out_fl_ext, "w" ) ;    // Open and write output file
  log_fopen_status     = fopen ( cpy_dir_log_fl_ext , "w" ) ;   // Open and write log file

  if ( ! input_fopen_status ) {
    printf ( "Could not open file %s for input.\n" , input_fopen_status ) ;     // Error message while opening file
    exit(1) ;
  }

  if ( ! output_fopen_status ) {
    printf ( "Could not open file %s for output\n" ,output_fopen_status ) ;     // Error message while opening file
    exit(1) ;
  }

  if ( ! log_fopen_status ) {
    printf ( "Could not open file %s for error\n" ,log_fopen_status  ) ;        // Error message while opening file
    exit(1) ;
  }


  s_mkeKey_open() ;                                             // subroutine to open ssa connection


  // Call ssan3_get_keys


  // Read a input file line by line
  while( fgets (  current_rec , sizeof (current_rec) , input_fopen_status  ) ) {
      cur_rec_len=strlen( current_rec ) ;
      if ( cur_rec_len > 0 && current_rec[cur_rec_len-1] == '\n' ) {
        current_rec[--cur_rec_len] = '\0' ;
      }


      if ( strstr ( current_rec , "Person_Name" ) != NULL ) {
        char *PL = "FIELD=Person_Name KEY_LEVEL=Limited" ;
        char *PS = "FIELD=Person_Name KEY_LEVEL=Standard" ;
        char *PE = "FIELD=Person_Name KEY_LEVEL=Extended" ;
        char *abv_PLim = "PM" ;
        char *abv_PStand = "PS" ;
        char *abv_PExt = "PX" ;

        s_mkeKey_getKey(PL , PS , PE , abv_PLim , abv_PStand , abv_PExt) ;
      }

      if ( strstr ( current_rec , "Organization_Name" ) != NULL ) {
        char *OL = "FIELD=Organization_Name KEY_LEVEL=Limited" ;
        char *OS = "FIELD=Organization_Name KEY_LEVEL=Standard" ;
        char *OE = "FIELD=Organization_Name KEY_LEVEL=Extended" ;
        char *abv_OLim = "OM" ;
        char *abv_OStand = "OS" ;
        char *abv_OExt = "OX" ;
        s_mkeKey_getKey(OL , OS , OE , abv_OLim , abv_OStand , abv_OExt) ;
      }

      if ( strstr ( current_rec , "Address_Part1" ) != NULL ) {
        char *AL = "FIELD=Address_Part1 KEY_LEVEL=Limited" ;
        char *AS = "FIELD=Address_Part1 KEY_LEVEL=Standard" ;
        char *AE = "FIELD=Address_Part1 KEY_LEVEL=Extended" ;
        char *abv_ALim = "1M" ;
        char *abv_AStand = "1S" ;
        char *abv_AExt = "1X" ;
        s_mkeKey_getKey(AL , AS , AE , abv_ALim , abv_AStand , abv_AExt) ;
      }

  }


  s_mkeKey_close() ;                                            // subroutine to close ssa connection

/* Close the previously established session.
*/


  //t = clock() - t ;                                              // End time
  //time_taken = ((double)t)/CLOCKS_PER_SEC ;                      // In seconds
  //printf("\nTook %f seconds to execute \n" , time_taken) ;        // Print time

  return (0) ;
}

/**********************************************************************
 End of scrit MkeKey.c                                                *
**********************************************************************/
