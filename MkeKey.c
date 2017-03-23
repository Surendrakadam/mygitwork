/*
 Application   :
 Client        :
 Copyright (c) : IdentLogic Systems Private Limited
 Author        : Surendra Kadam
 Creation Date : 20 March 2017
 Description   :

 WARNINGS      :

 HARD CODINGS  :

 Limitations   :
 Dependencies  :
 Modifications
 Date       Change Req# Author       Description
 ---------- ----------- ------------ ---------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ssan3cl.h"                                            // Dedupe header
#include <getopt.h>
#include <unistd.h>
#include <time.h>                                               // Timer header

#define rangeof(arr)  (sizeof(arr)/sizeof((arr)[0]))

  // Global variables
  long  rc ;
  long  sockh = -1 ;
  long  session_id = -1 ;

  // Procedure run parameter
  char *p_data_set = "" ;                                       // Parameter data set number
  char *p_run_time = "";                                        // Parameter run time data
  char *p_infdir   = "" ;                                       // Parameter input file name
  char *p_outfdir  = "" ;                                       // Parameter output file directory
  char *p_logfdir  = "" ;                                       // Parameter log file directory

  // File Open Status
  FILE  *input_fopen_status ;                                   // Input file name
  FILE  *output_fopen_status ;                                  // Output file name
  FILE  *log_fopen_status ;                                     // Log file name

  // File names
  char *infname  = "" ;                                         // Input file name
  char *ofname   = "" ;                                         // Output file name
  char *logfname = "" ;                                         // Log file name

  // File Directories
  char input_file_directory[500] ;                              // Input file directory
  char output_file_directory[500];                              // Output file directory
  char log_file_direcory[500];                                  // Log file directory

  int cur_rec_len    = 0 ;                                      // Current record length
  int rec_number     = 0 ;                                      // Record counter
  int i              = 0 ;                                      // s_mkeKey_getKey for loop variable

  char current_rec [ BUFSIZ ] ;                                 // Current record of a file
  char *p_population ;                                          // Population name

  char *forward_slash = "/";                                    // Forward Slash
  char *back_slash    = "\\";                                   // Back Slash
  int  len_of_dir     = 0;                                      // Length of the directory
  char flg_slash ;                                              // check backslash or forward slash

  clock_t t ;                                                   // Clock object
  double time_taken ;                                           // Time taken

  int  option = 0 ;
  char cpy_infname[400] ;                                       // Copy of Input file name
  char cpy_ofname[400 ] ;                                       // Copy of output file name
  char cpy_logfname[400] ;                                      // Copy of log file name

  char *sss_rrrr = "" ;                                         // data set number and run time number
  char sss_rrrr_in_file[100];                                   // Copy of sssrrrr for making input file
  char sss_rrrr_out_file[100] ;                                 // Copy of sssrrrr for making output file
  char sss_rrrr_log_file[100] ;                                 // Copy of sssrrrr for making log file

  char *dir_in_fl_ext  = "" ;                                   // Directory with input file
  char *dir_out_fl_ext = "" ;                                   // Directory with output file
  char *dir_log_fl_ext = "" ;                                   // Directory with log file

  char cpy_dir_in_fl_ext[500] ;                                 // copy of Directory with input file
  char cpy_dir_out_fl_ext[500] ;                                // copy of Directory with output file
  char cpy_dir_log_fl_ext[500] ;                                // copy of Directory with log file

  char *ID        = "" ;                                        // Substring ID from the current record
  char *ptr_id    = "" ;                                        // Point to the string *ID* in current record till end of the current record
  char *search    = "*ID*" ;                                    // Search word *Id* in current record
  char  character = '*' ;                                       // Character * asterisk
  
  int   id_start_pos       = 0 ;                                // Starting position of *Id*
  int   pos_afr_id         = 0 ;                                // Position after *ID*
  int   asterisk_start_pos = 0 ;                                // After *ID* first * position
  int   id_len             = 0 ;                                // Length of the *ID* i.e 4
  int   frt_ast_pos        = 0 ;                                // First asterisk position after *ID*
  
  // Time variables
  int YYYY = 0 ;                                                // Year
  int MM   = 0 ;                                                // Month
  int DD   = 0 ;                                                // Date
  int HH24  = 0 ;                                                // Hour-24  
  int MI   = 0 ;                                                // Minute
  int SS   = 0 ;                                                // Seconds
  char sss_rrrr_timestamp[500] ;                                // log_file name with timestamp

static long s_test_ssa_open (
  long    sockh ,
  long    *session_id ,
  char    *sysName ,
  char    *population ,
  char    *controls
) {

  long    rc ;
  char    rsp_code[SSA_SI_RSP_SZ] ;
  char    ssa_msg[SSA_SI_SSA_MSG_SZ] ;

  fprintf ( log_fopen_status, "------ ssan3_open ------\n") ;
  fprintf ( log_fopen_status, "Session Id       : %ld\n" , *session_id) ;
  fprintf ( log_fopen_status, "System           : %s\n" , sysName) ;
  fprintf ( log_fopen_status, "Population       : %s\n" , population) ;
  fprintf ( log_fopen_status, "Controls         : %s\n" , controls) ;

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
    fprintf ( log_fopen_status, "rc               : %ld\n" , rc) ;
    rc = 1 ;
    goto ret ;
  }

  if (rsp_code[0] != '0' && *session_id == -1) {
    fprintf ( log_fopen_status, "rsp_code         : %s\n" , rsp_code) ;
    fprintf ( log_fopen_status, "ssa_msg          : %s\n" , ssa_msg) ;
    rc = 1 ;
    goto ret ;
  }

  fprintf ( log_fopen_status, "\n") ;
  fprintf ( log_fopen_status, "Session Id       : %ld\n" , *session_id) ;
  fprintf ( log_fopen_status, "rsp_code         : %s\n" , rsp_code) ;
  fprintf ( log_fopen_status, "ssa_msg          : %s\n" , ssa_msg) ;
  rc = 0 ;
  goto ret ;

ret:
  return (rc) ;
}

/**********************************************************************
 End of subroutine s_test_ssa_open                                    *
**********************************************************************/

static long s_test_ssa_get_keys (

  long    sockh ,
  long    *session_id ,
  char    *sysName ,
  char    *population ,
  char    *controls ,
  char    *record ,
  long    recordLength ,
  char    *recordEncType,
  char    *ky_fld_ky_lvl,
  char    *ID
) {
  int     i ;
  long    rc ;
  char    *keys_array[SSA_SI_MAX_KEYS] ;
  char    keys_data[SSA_SI_MAX_KEYS*SSA_SI_KEY_SZ] ;
  char    rsp_code[SSA_SI_RSP_SZ] ;
  char    ssa_msg[SSA_SI_SSA_MSG_SZ] ;
  long    num ;
  char    *p ;

  for (i = 0 ; i < (int)rangeof (keys_array) ; ++i)
    keys_array[i] = keys_data + i * SSA_SI_KEY_SZ ;

  fprintf ( log_fopen_status, "------ ssan3_get_keys_encoded ------\n") ;
  fprintf ( log_fopen_status, "Session Id       : %ld\n" , *session_id) ;
  fprintf ( log_fopen_status, "System           : %s\n" , sysName) ;
  fprintf ( log_fopen_status, "Population       : %s\n" , population) ;
  fprintf ( log_fopen_status, "Controls         : %s\n" , controls) ;
  fprintf ( log_fopen_status, "Key field data   : %s\n" , record) ;
  fprintf ( log_fopen_status, "Key field size   : %ld\n" , recordLength) ;
  fprintf ( log_fopen_status, "Key field encoding type : %s\n" , recordEncType) ;

  num = 0 ;
  rc  =
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
    fprintf ( log_fopen_status, "rc               : %ld\n" , rc) ;
    rc = 1 ;
    goto ret ;
  }
  
  if (rsp_code[0] != '0') {
    fprintf ( log_fopen_status, "rsp_code         : %s\n" , rsp_code) ;
    fprintf ( log_fopen_status, "ssa_msg          : %s\n" , ssa_msg) ;
    rc = -1 ;
    goto ret ;
  }
  
  fprintf ( log_fopen_status, "\n") ;
  fprintf ( log_fopen_status, "Session Id       : %ld\n" , *session_id) ;
  fprintf ( log_fopen_status, "rsp_code         : %s\n" , rsp_code) ;
  fprintf ( log_fopen_status, "ssa_msg          : %s\n" , ssa_msg) ;
  fprintf ( log_fopen_status, "Count            : %ld\n" , num) ;
  //printf ("------ keys ------\n") ;
  ++rec_number ;
  //fprintf(output_fopen_status ,"\n%d :",rec_number) ;
  for (i = 0 ; i < num ; ++i) {
    p = keys_array[i] ;
    fprintf ( output_fopen_status ,"%.*s%s%s\n" , SSA_SI_KEY_SZ , p ,ky_fld_ky_lvl,ID) ;
    //printf ("%3d '%.*s'\n" , i+1 , SSA_SI_KEY_SZ , p) ;  //-- OUTPUT
  }
  //fprintf(output_fopen_status ,"\n") ;
  rc = 0 ;
  goto ret ;

ret:
  return (rc) ;
}

/**********************************************************************
 End of subroutine s_test_ssa_get_keys                                *
**********************************************************************/

static long s_test_ssa_close (

  long    sockh ,
  long    *session_id ,
  char    *sysName ,
  char    *population ,
  char    *controls
) {
  long    rc ;
  char    rsp_code[SSA_SI_RSP_SZ] ;
  char    ssa_msg[SSA_SI_SSA_MSG_SZ] ;

  fprintf ( log_fopen_status, "------ ssan3_close ------\n") ;
  fprintf ( log_fopen_status, "Session Id       : %ld\n" , *session_id) ;
  fprintf ( log_fopen_status, "System           : %s\n" , sysName) ;
  fprintf ( log_fopen_status, "Population       : %s\n" , population) ;
  fprintf ( log_fopen_status, "Controls         : %s\n" , controls) ;

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
    fprintf ( log_fopen_status, "rc               : %ld\n" , rc) ;
    rc = 1 ;
    goto ret ;
  }
  
  if (rsp_code[0] != '0') {
    fprintf ( log_fopen_status, "rsp_code         : %s\n" , rsp_code) ;
    fprintf ( log_fopen_status, "ssa_msg          : %s\n" , ssa_msg) ;
    rc = 1 ;
    goto ret ;
  }
  
  fprintf ( log_fopen_status, "\n") ;
  fprintf ( log_fopen_status, "Session Id       : %ld\n" , *session_id) ;
  fprintf ( log_fopen_status, "rsp_code         : %s\n" , rsp_code) ;
  fprintf ( log_fopen_status, "ssa_msg          : %s\n" , ssa_msg) ;
  rc = 0 ;
  goto ret ;

ret:
  return (rc) ;
}

/**********************************************************************
 End of subroutine s_test_ssa_close                                   *
**********************************************************************/

static void s_doExit ( char *func) {
  printf ("Error occurred in '%s'\n" , func) ;
  exit (EXIT_FAILURE) ;
}

/**********************************************************************
 End of subroutine s_doExit                                             *
**********************************************************************/

static void s_mkeKey_open () {

    // Establish a session.
    rc = s_test_ssa_open (
      sockh ,
      &session_id ,
      "default" ,
      p_population ,
      "") ;

    if (0 != rc)
      s_doExit ("s_test_ssa_open") ;

}

/**********************************************************************
 End of subroutine s_mkeKey_open                                      *
**********************************************************************/

static void s_mkeKey_getKey (
  char *Limited ,
  char *Standard ,
  char *Extended ,
  char *ky_fld_ky_lvl1 ,
  char *ky_fld_ky_lvl2 ,
  char *ky_fld_ky_lvl3 ,
  char *ID
) {

  char *key_level [ 3 ]     = { Limited , Standard , Extended } ;
  char *ky_fld_ky_lvl [ 3 ] = { ky_fld_ky_lvl1 , ky_fld_ky_lvl2 , ky_fld_ky_lvl3 } ;

  for( i = 0 ; i <= 2 ; i++ ) {
    rc =
      s_test_ssa_get_keys (
        sockh ,
        &session_id ,
        "default" ,
        p_population ,
        key_level[i] ,
        current_rec ,
        cur_rec_len ,
        "TEXT",
        ky_fld_ky_lvl [ i ] ,
        ID
      ) ;
      
    //fprintf ( output_fopen_status , "%s" , ky_fld_ky_lvl [ i ] ) ;
    //fprintf (output_fopen_status,"%s",ID) ;
    if ( 0 != rc )
      s_doExit ( "s_test_ssa_get_keys" ) ;
  }

}

/**********************************************************************
 End of subroutine s_mkeKey_getKey                                    *
**********************************************************************/

static void s_mkeKey_close ( ) {
  rc = s_test_ssa_close (
    sockh ,
    &session_id ,
    "default" ,
    p_population ,
    "") ;

  if (0 != rc)
    s_doExit ("s_test_ssa_close") ;
}

/**********************************************************************
 End of subroutine s_mkeKey_close                                     *
**********************************************************************/

void s_print_usage() {
    printf("MkeKey -d 101 -r 1001 -p india -i E:/SurendraK/Work/SSAProcs/Input/ -o E:/SurendraK/Work/SSAProcs/Output/ -l E:/SurendraK/Work/SSAProcs/Log/") ;
}

/**********************************************************************
 End of subroutine s_print_usage                                      *
**********************************************************************/

static void s_getParameter(int argc , char *argv[]) {

  time_t my_time ;                                              // Time to get current time
  struct tm * timeinfo ;
  time (&my_time) ;
  timeinfo = localtime (&my_time);
  
  YYYY = timeinfo->tm_year+1900;
  MM   = timeinfo->tm_mon+1;
  DD   = timeinfo->tm_mday;
  HH24  = timeinfo->tm_hour;
  MI   = timeinfo->tm_min;
  SS   = timeinfo->tm_sec;

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
        s_print_usage ( ) ;
        exit ( EXIT_FAILURE ) ;
    }

  }

// Check Input file directory ends with backslash or forward , if not add it
  if(strchr(p_infdir,'/')) {
      len_of_dir = strlen(p_infdir);                            // Length of input file directory
      flg_slash = p_infdir[len_of_dir-1];                       // Last character of a String is / forward slash

      if(flg_slash != forward_slash[0]) {
        p_infdir = strcat(p_infdir,forward_slash);
      }
  }

  if(strchr(p_infdir,'\\')) {
      len_of_dir = strlen(p_infdir);                            // Length of output file directory
      flg_slash = p_infdir[len_of_dir-1];                       // Last character of a String is / backslash

      if(flg_slash != back_slash[0]) {
        p_infdir = strcat(p_infdir,back_slash);
      }

  }

// Check Output file directory ends with backslash or forward , if not add it
  if(strchr(p_outfdir,'/')) {
      len_of_dir = strlen(p_outfdir);                           // Length of log file directory
      flg_slash = p_outfdir[len_of_dir-1];                      // Last character of a String is / forward slash

      if(flg_slash != forward_slash[0]) {
        p_outfdir = strcat(p_outfdir,forward_slash);
      }
  }

  if(strchr(p_outfdir,'\\')) {
      len_of_dir = strlen(p_outfdir);                           // Length of input file directory
      flg_slash = p_outfdir[len_of_dir-1];                      // Last character of a String is / back slash

      if(flg_slash != back_slash[0]) {
        p_outfdir = strcat(p_outfdir,back_slash);
      }

  }

// Check log file directory ends with backslash or forward , if not add it
  if(strchr(p_logfdir,'/')) {
      len_of_dir = strlen(p_logfdir);                           // Length of input file directory
      flg_slash = p_logfdir[len_of_dir-1];                      // Last character of a String is / forward slash

      if(flg_slash != forward_slash[0]) {
        p_logfdir = strcat(p_logfdir,forward_slash);
      }
  }

  if(strchr(p_logfdir,'\\')) {
      len_of_dir = strlen(p_logfdir);                           // Length of input file directory
      flg_slash = p_logfdir[len_of_dir-1];                      // Last character of a String is / back slash

      if(flg_slash != back_slash[0]) {
        p_logfdir = strcat(p_logfdir,back_slash);
      }

  }

  // Copy all directory name into different variables
  strcpy( input_file_directory ,p_infdir );
  strcpy( output_file_directory ,p_outfdir );
  strcpy( log_file_direcory ,p_logfdir );

  sss_rrrr = strcat(p_data_set ,p_run_time) ;                   // Concatenate data set number and run time number

  strcpy( sss_rrrr_in_file , sss_rrrr ) ;                       // Copy value of sss_rrrr to sss_rrrr_in_file
  strcpy( sss_rrrr_out_file , sss_rrrr ) ;                      // Copy value of sss_rrrr to sss_rrrr_out_file
  strcpy( sss_rrrr_log_file , sss_rrrr ) ;                      // Copy value of sss_rrrr to sss_rrrr_log_file
  
  // Log_file concatenate with timestamp
  sprintf(sss_rrrr_timestamp,"%s_%d_%d_%d_%d_%d_%d",sss_rrrr_log_file,YYYY,MM,DD,HH24,MI,SS);

  infname  = strcat( sss_rrrr_in_file ,".tag" ) ;               // Input file name with extension
  ofname   = strcat( sss_rrrr_out_file,".oke" ) ;               // Output file name with extension
  logfname = strcat( sss_rrrr_timestamp,".log" ) ;               // Log file name with extension

  strcpy( cpy_infname,infname ) ;                               // Copy of Input file name
  strcpy( cpy_ofname,ofname ) ;                                 // Copy of output file name
  strcpy( cpy_logfname,logfname ) ;                             // Copy of log file name

  // Concatenate Directory path with Input file name
  dir_in_fl_ext = strcat( input_file_directory ,cpy_infname );
  strcpy(cpy_dir_in_fl_ext,dir_in_fl_ext) ;

  // Concatenate Directory path with output file name
  dir_out_fl_ext = strcat( output_file_directory, cpy_ofname );
  strcpy(cpy_dir_out_fl_ext,dir_out_fl_ext) ;

  // Concatenate Directory path with log file name
  dir_log_fl_ext = strcat( log_file_direcory, cpy_logfname );
  strcpy(cpy_dir_log_fl_ext,dir_log_fl_ext) ;

  
  input_fopen_status   = fopen ( cpy_dir_in_fl_ext, "r" ) ;     // Open and read input file
  if ( ! input_fopen_status ) {
    printf ( "Could not open file %s for input.\n" , input_fopen_status ) ;     // Error message while opening file
    exit(1) ;
  }
  else {
  output_fopen_status  = fopen ( cpy_dir_out_fl_ext, "w" ) ;    // Open and write output file
  log_fopen_status     = fopen ( cpy_dir_log_fl_ext , "w" ) ;   // Open and write log file
  }

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
}

/**********************************************************************
 End of subroutine s_getParameter                                     *
**********************************************************************/

int main ( int argc , char *argv[] ) {

  id_len = strlen( search ) ;                                   // Length of search string
  s_getParameter ( argc , argv ) ;                              // Subroutine to get parameter
  t = clock() ;                                                 // Start time

  s_mkeKey_open() ;                                             // subroutine to open ssa connection

  // Call ssan3_get_keys

  // Read a input file line by line
  while( fgets (  current_rec , sizeof (current_rec) , input_fopen_status  ) ) {

      cur_rec_len = strlen( current_rec ) ;
      if ( cur_rec_len > 0 && current_rec[cur_rec_len-1] == '\n' ) {
        current_rec[--cur_rec_len] = '\0' ;
      }

      if ( strstr ( current_rec , search ) != NULL ) {

        ptr_id       = strstr ( current_rec , search ) ;        // Search *ID* in current record capture string from *ID* to till end of the string
        id_start_pos = (ptr_id - current_rec) ;                 // Starting position of *ID*
        pos_afr_id   = ( id_start_pos + id_len ) ;              // Position after *Id*

        for( i = pos_afr_id ; i < cur_rec_len; i++ ) {
          if( current_rec[i] == character ) {
              asterisk_start_pos = i ;                          // After *ID* first * position
              break ;
          }
        }

        frt_ast_pos = ( asterisk_start_pos - pos_afr_id ) ;     // First asterisk position after *Id*
        strncpy( ID,current_rec + pos_afr_id, frt_ast_pos ) ;   // Substring Id

        if ( strstr ( current_rec , "Person_Name" ) != NULL ) {
          char *PL = "FIELD=Person_Name KEY_LEVEL=Limited" ;
          char *PS = "FIELD=Person_Name KEY_LEVEL=Standard" ;
          char *PE = "FIELD=Person_Name KEY_LEVEL=Extended" ;

          char *abv_PLim   = "PM" ;
          char *abv_PStand = "PS" ;
          char *abv_PExt   = "PX" ;

          s_mkeKey_getKey( PL , PS , PE , abv_PLim , abv_PStand , abv_PExt ,ID ) ;
        }

        if ( strstr ( current_rec , "Organization_Name" ) != NULL ) {
          char *OL = "FIELD=Organization_Name KEY_LEVEL=Limited" ;
          char *OS = "FIELD=Organization_Name KEY_LEVEL=Standard" ;
          char *OE = "FIELD=Organization_Name KEY_LEVEL=Extended" ;

          char *abv_OLim   = "OM" ;
          char *abv_OStand = "OS" ;
          char *abv_OExt   = "OX" ;

          s_mkeKey_getKey( OL , OS , OE , abv_OLim , abv_OStand , abv_OExt,ID ) ;
        }

        if ( strstr ( current_rec , "Address_Part1" ) != NULL ) {
          char *AL = "FIELD=Address_Part1 KEY_LEVEL=Limited" ;
          char *AS = "FIELD=Address_Part1 KEY_LEVEL=Standard" ;
          char *AE = "FIELD=Address_Part1 KEY_LEVEL=Extended" ;

          char *abv_ALim   = "1M" ;
          char *abv_AStand = "1S" ;
          char *abv_AExt   = "1X" ;

          s_mkeKey_getKey( AL , AS , AE , abv_ALim , abv_AStand , abv_AExt ,ID ) ;
        }
     }
  }

  s_mkeKey_close() ;                                            // subroutine to close ssa connection

  t = clock() - t;                                        // End time
  time_taken = ((double)t)/CLOCKS_PER_SEC;                // In seconds
  printf("\nTook %f seconds to execute \n", time_taken);  // Print time

  return (0) ;
}

/**********************************************************************
 End of scrit MkeKey.c                                                *
**********************************************************************/

/*
  Make Key
   
    1 Procedure Name
     
    2 Copyright
    
    3 Warnings
     
    4 Technical
      3.1 Run Parameters
      
      3.2 Compile Procedure
      
      3.3 Execute procedure in different way
      
      3.2 Subroutines
      
          3.2.1 Called by
          
          3.2.2 Calling
          
          3.2.3 Subroutine Structure
          
      3.3 Include Header
          
          
     
     
  Make Key
    
    It creates Key file sssrrrr.oke from tagged file sssrrrr.tag
    This procedure will create keys from tagged data.
    Tagged data contains ID, Person_Name, Organization_Name, 
    Address_Part1, Postal_Area.
    Key will generate based on the KEY_LEVEL and KEY_FIELDS.
    
      KEY_LEVEL                     KEY_Field
      ---------                     ---------
    M|Limited                     P|Person_Name
    S|Standard                    O|Organization_Name
    X|Extended                    1|Address_Part1
    
    If Person_Name key field is present in the tagged data record it 
    will generate three keys using KEY_LEVEL Limited, Standard and 
    Extended.
    
    If Organization_Name key field is present in the tagged data 
    record it will generate three keys using KEY_LEVEL Limited, 
    Standard and Extended.
    
    If Address_Part1 key field is present in the tagged data record 
    it will generate three keys using KEY_LEVEL Limited, Standard and Extended.
   
    Format:
    Column  1 to 8 : Key
    Column  9      : Key type - P (Person_name), O (Organisation), 1 (Address_Part1)
    Column 10      : Key level - M (Limited), S (Standard), X (Extended)
    Column 11 onward : Id
    
    
  
  Procedure Name : MkeKey.c
   
    Creates Key file sssrrrr.oke from tagged file sssrrrr.tag.
    
  Copyright
  
    Copyright (c) 2017 IdentLogic Systems Private Limited
    
  Warnings
    If set data number and run number are empty it will throw an error
    like could not open file for input.

  Technical
  
   Script name      - MkeKey.c
   Package Number   - 
   Procedure Number - 
   
   Run Parameters
   
   PARAMETER                DESCRIPTION                    ABV   VARIABLE
   ---------                -----------                    ---  ---------
   Set data number          Set data number - 100 to 999    d   p_data_set
   Run number               Run number - 1000 to 9999       r   p_run_time
   Population               Country Name: india             p   p_population
   Input File Directory     Input File Directory            i   p_infdir
   Output File Directory    Output File Directory           o   p_outfdir
   Log File Directory       Log File Directory              l   p_logfdir

     TimeStamp Variables   Description
     -------------------   -----------
     YYYY                  Year
     MM                    Month
     DD                    Date
     HH24                  Hours
     MI                    Minutes
     SS                    Seconds
   
     The extension of Input file name is .tag
     The extension of Output file name is .oke
     The extension of Log file name is .log
     
   Compile procedure

    cl MkeKey.c stssan3cl.lib
    
   Execute procedure
   
   d -r -p paramter are mandatory
   
   1. MkeKey -d 101 -r 1001 -p india
   
   2. MkeKey -d 101 -r 1001 -p india -i E:/ABC/EFG/HIJ/Input/
     -o E:/ABC/EFG/HIJ/Output/ -l E:/ABC/EFG/HIJ/Log/
   
   3. MkeKey -d 101 -r 1001 -p india -i E:\ABC\EFG\HIJ\Input\
     -o E:\ABC\EFG\HIJ\Output\ -l E:\ABC\EFG\HIJ\Log\
   
   4. MkeKey -d 101 -r 1001 -p india -i E:/ABC/EFG/HIJ/Input
     -o E:/ABC/EFG/HIJ/Output -l E:/ABC/EFG/HIJ/Log
   
   5. MkeKey -d 101 -r 1001 -p india -i E:\ABC\EFG\HIJ\Input
     -o E:\ABC\EFG\HIJ\Output -l E:\ABC\EFG\HIJ\Log
     
   6. MkeKey -d 101 -r 1001 -p india -i E:/ABC/EFG/HIJ/Input/
  
   Notes :
   
   If you forget to give backslash(\)or forward slash(/) at the end of the 
   Input, Output, and Log file directory. The procedure will be append 
   backslash or forward slash at the end of the.
   
   If you do not give Input File Directory then procedure will read 
   the file from the current directory.
   If you do not give Output File Directory and Log File Directory 
   in the run parameter then output file and log file will be created
   in the current directory.
   
   Output and Log file created in the directory which is given in the 
   command prompt.
   
   Log file will be created with date and time
   for eg. sssrrrr_YYYY_MM_DD_HH24_MI_SS.log
   
   
   Subroutines
                                                                 
   Subroutine            Description
   ----------            -----------
   s_getParameter        This subroutine takes run parameters.
   s_test_ssa_open       To open a session to the SSA-NAME3 services
   s_test_ssa_get_keys   To build keys on names or addresses
   s_test_ssa_close      To terminate an open session to SSA-NAME3
   s_doExit              Error occurred in subroutines s_test_ssa_open, s_test_ssa_get_keys, s_test_ssa_close 
   s_mkeKey_open         s_test_ssa_open subroutine called in s_mkeKey_open subroutine
   s_mkeKey_getKey       s_test_ssa_get_keys subroutine called in s_mkeKey_getKey subroutine
   s_mkeKey_close        s_test_ssa_close subroutine called in s_mkeKey_close subroutine
   s_print_usage         This subroutine is default parameter of getopt in s_getParameter
   
   
   Called by
   
   Not indicated if only called by Main.
   
   Subroutine           Called by
   ----------           ---------       
   s_test_ssa_open      s_mkeKey_open
   s_test_ssa_get_keys  s_mkeKey_getKey
   s_test_ssa_close     s_mkeKey_close
   s_doExit             s_mkeKey_open, s_mkeKey_getKey, s_mkeKey_close       
   s_print_usage        s_getParameter
   
   Calling

   Subroutine           Calling Subroutine
   ----------           ------------------
   s_mkeKey_open        s_test_ssa_open, s_doExit
   s_mkeKey_getKey      s_test_ssa_get_keys, s_doExit
   s_mkeKey_close       s_test_ssa_close, s_doExit
   s_getParameter       s_print_usage
   
   Subroutine Structure
   
   Main
    |
    |----- s_getParameter
    |           |
    |           |----- s_print_usage
    |                 
    |
    |
    |----- s_mkeKey_open
    |           |
    |           |----- s_test_ssa_open
    |           |
    |           \----- s_doExit
    |            
    |
    |
    |----- s_mkeKey_getKey
    |           |
    |           |----- s_test_ssa_get_keys
    |           |
    |           \----- s_doExit
    |
    |
    |
    |
    |----- s_mkeKey_close
                |
                |----- s_test_ssa_close
                |
                \----- s_doExit
    
   
   
   Include Header
   --------------
   <stdio.h>      - Inbuild header file   
   <stdlib.h>     - Inbuild header file 
   <string.h>     - Inbuild header file 
   "ssan3cl.h"    - Dedupe header
   <getopt.h>     - External header file
   <unistd.h>     - External header file
   <time.h>       - Inbuild header file 

*/