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

  // SSA NAME3 comman variables rc, sockh, session_id

  long  l_rc ;                                                   // Indicate success or failure

  // Only use if your application is calling the SSA-NAME3 server, Otherwise it should be set to -1
  long  l_sockh      = -1 ;

  // Should be -1 on the ssan3_open call, if opening  a new session, a valid Session ID or any other call
  long  l_session_id = -1 ;

  // Procedure run parameter
<<<<<<< HEAD
  char *p_data_set = "" ;                                       // Parameter data set number
  char *p_run_time = "";                                        // Parameter run time data
  char *p_infdir   = "" ;                                       // Parameter input file name
  char *p_outfdir  = "" ;                                       // Parameter output file directory
  char *p_logfdir  = "" ;                                       // Parameter log file directory
=======
  char *p_data_set   = "" ;                                     // Parameter data set number
  char *p_run_time   = "" ;                                     // Parameter run time data
  char *p_population = "" ;                                     // Population name
  char *p_infdir     = "" ;                                     // Parameter input file name
  char *p_outfdir    = "" ;                                     // Parameter output file directory
  char *p_logfdir    = "" ;                                     // Parameter log file directory
>>>>>>> local

  // File Open Status
  FILE  *f_input_fopen_status ;                                 // Input file name
  FILE  *f_output_fopen_status ;                                // Output file name
  FILE  *f_log_fopen_status ;                                   // Log file name

  // File names
  char *c_infname  = "" ;                                       // Input file name
  char *c_ofname   = "" ;                                       // Output file name
  char *c_logfname = "" ;                                       // Log file name

  char  c_data_set[500] ;                                       // Copy data set
  char  c_run_time[500] ;                                       // Copy run time
  
  // File Directories
  char a_input_file_directory[500] ;                            // Input file directory
  char a_output_file_directory[500] ;                           // Output file directory
  char a_log_file_direcory[500] ;                               // Log file directory

  int i_cur_rec_len    = 0 ;                                    // Current record length
  int i_idx            = 0 ;                                    // s_mkeKey_getKey for loop variable

  char c_current_rec [ BUFSIZ ] ;                               // Current record of a file

  char *c_forward_slash = "/";                                  // Forward Slash
  char *c_back_slash    = "\\";                                 // Back Slash
  int  i_len_of_dir     = 0;                                    // Length of the directory
  char c_flg_slash ;                                            // check backslash or forward slash

  clock_t t ;                                                   // Clock object
  clock_t end_time ;
  double d_time_taken ;                                         // Time taken

<<<<<<< HEAD
  int  option = 0 ;
  char cpy_infname[400] ;                                       // Copy of Input file name
  char cpy_ofname[400 ] ;                                       // Copy of output file name
  char cpy_logfname[400] ;                                      // Copy of log file name
=======
  int  i_option = 0 ;
  char a_cpy_infname[400] ;                                     // Copy of Input file name
  char a_cpy_ofname[400 ] ;                                     // Copy of output file name
  char a_cpy_logfname[400] ;                                    // Copy of log file name
>>>>>>> local

  char *c_sss_rrrr = "" ;                                       // data set number and run time number
  char a_sss_rrrr_in_file[100] ;                                // Copy of sssrrrr for making input file
  char a_sss_rrrr_out_file[100] ;                               // Copy of sssrrrr for making output file
  char a_sss_rrrr_log_file[100] ;                               // Copy of sssrrrr for making log file

  char *c_dir_in_fl_ext  = "" ;                                 // Directory with input file
  char *c_dir_out_fl_ext = "" ;                                 // Directory with output file
  char *c_dir_log_fl_ext = "" ;                                 // Directory with log file

  char a_cpy_dir_in_fl_ext[500] ;                               // copy of Directory with input file
  char a_cpy_dir_out_fl_ext[500] ;                              // copy of Directory with output file
  char a_cpy_dir_log_fl_ext[500] ;                              // copy of Directory with log file

<<<<<<< HEAD
  char *ID        = "" ;                                        // Substring ID from the current record
=======
  char *c_ID        = "" ;                                      // Substring ID from the current record
<<<<<<< HEAD
>>>>>>> local
  char *ptr_id    = "" ;                                        // Point to the string *ID* in current record till end of the current record
  char *search    = "*ID*" ;                                    // Search word *Id* in current record
  char  character = '*' ;                                       // Character * asterisk
  
<<<<<<< HEAD
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
=======
=======
  char *c_ptr_id    = "" ;                                      // Point to the string *ID* in current record till end of the current record
  char *c_search    = "*Id*" ;                                  // Search word *Id* in current record
  char  c_character = '*' ;                                     // Character * asterisk

>>>>>>> local
  int   i_id_start_pos       = 0 ;                              // Starting position of *Id*
  int   i_pos_afr_id         = 0 ;                              // Position after *ID*
  int   i_asterisk_start_pos = 0 ;                              // After *Id* first * position
  int   i_id_len             = 0 ;                              // Length of the *ID* i.e 4
  int   i_frt_ast_pos        = 0 ;                              // First asterisk position after *Id*

  // Time variables
  int i_YYYY = 0 ;                                              // Year
  int i_MM   = 0 ;                                              // Month
  int i_DD   = 0 ;                                              // Date
  int i_HH24 = 0 ;                                              // Hour-24
  int i_MI   = 0 ;                                              // Minute
  int i_SS   = 0 ;                                              // Seconds
  char a_sss_rrrr_timestamp[500] ;                              // log_file name with timestamp

  int i_record_read       = 0 ;                                 // No of read records counts
  int i_error_record_read = 0 ;                                 // No of error records counts
  int i_pn_records        = 0 ;                                 // No of Person_Name records counts
  int i_on_records        = 0 ;                                 // No of Organization records counts
  int i_addp1_records     = 0 ;                                 // No of Address Part1 records counts

  int i_PM_ky             = 0 ;                                 // Person Name Limited key
  int i_PS_ky             = 0 ;                                 // Person Name Standard key
  int i_PX_ky             = 0 ;                                 // Person Name Extended key

  int i_OM_ky             = 0 ;                                 // Organization Name Limited key
  int i_OS_ky             = 0 ;                                 // Organization Name Standard key
  int i_OX_ky             = 0 ;                                 // Organization Name Extended key

  int i_AM_ky             = 0 ;                                 // AddressPart1 Limited key
  int i_AS_ky             = 0 ;                                 // AddressPart1 Standard key
  int i_AX_ky             = 0 ;                                 // AddressPart1 Extended key
  int i_total_kys_written = 0 ;                                 // Total keys written
  int i_addition_key      = 0 ;                                 // Addition of all keys
  int i_rec_number        = 0 ;                                 // Record counter

// To open a session to the SSA-NAME3 services
>>>>>>> local

static long s_test_ssa_open (

  long    l_sockh ,
  long    *l_session_id ,
  char    *c_sysName ,
  char    *c_population ,
  char    *c_controls

 ) {

  long    l_rc ;
  char    c_rsp_code[SSA_SI_RSP_SZ] ;
  char    c_ssa_msg[SSA_SI_SSA_MSG_SZ] ;

  time_t my_time ;                                              // Time to get current time
  struct tm * timeinfo ;                                        // struct tm pointer
  time ( &my_time ) ;                                           // call time() to get current date/time
  timeinfo = localtime ( &my_time ) ;                           // Localtime

  i_YYYY = timeinfo->tm_year+1900 ;                             // Year
  i_MM   = timeinfo->tm_mon+1 ;                                 // Month
  i_DD   = timeinfo->tm_mday ;                                  // Date
  i_HH24 = timeinfo->tm_hour ;                                  // Hours
  i_MI   = timeinfo->tm_min ;                                   // Minutes
  i_SS   = timeinfo->tm_sec ;                                   // Seconds

  fprintf ( f_log_fopen_status, "\n------ EXECUTION START DATE AND TIME ------\n" ) ;
  fprintf ( f_log_fopen_status, "%d/%d/%d %d-%d-%d \n\n", i_YYYY, i_MM, i_DD, i_HH24, i_MI, i_SS ) ;
  fprintf ( f_log_fopen_status, "------ ssan3_open ------\n" ) ;
  fprintf ( f_log_fopen_status, "Session Id       : %ld\n" , *l_session_id ) ;
  fprintf ( f_log_fopen_status, "System           : %s\n" , c_sysName ) ;
  fprintf ( f_log_fopen_status, "Population       : %s\n" , c_population ) ;
  fprintf ( f_log_fopen_status, "Controls         : %s\n" , c_controls ) ;

  l_rc =
    ssan3_open (
      l_sockh ,
      l_session_id ,
      c_sysName ,
      c_population ,
      c_controls ,
      c_rsp_code ,
      SSA_SI_RSP_SZ ,
      c_ssa_msg ,
      SSA_SI_SSA_MSG_SZ
    ) ;

  if ( l_rc < 0 ) {
    fprintf ( f_log_fopen_status, "rc               : %ld\n" , l_rc ) ;
    l_rc = 1 ;
    goto ret ;
  }

  if ( c_rsp_code[0] != '0' && *l_session_id == -1 ) {
    fprintf ( f_log_fopen_status, "rsp_code         : %s\n" , c_rsp_code ) ;
    fprintf ( f_log_fopen_status, "ssa_msg          : %s\n" , c_ssa_msg ) ;
    l_rc = 1 ;
    goto ret ;
  }

<<<<<<< HEAD
  fprintf ( log_fopen_status, "\n") ;
  fprintf ( log_fopen_status, "Session Id       : %ld\n" , *session_id) ;
  fprintf ( log_fopen_status, "rsp_code         : %s\n" , rsp_code) ;
  fprintf ( log_fopen_status, "ssa_msg          : %s\n" , ssa_msg) ;
=======
  fprintf ( f_log_fopen_status, "\n" ) ;
<<<<<<< HEAD
  fprintf ( f_log_fopen_status, "Session Id       : %ld\n" , *session_id ) ;
  fprintf ( f_log_fopen_status, "rsp_code         : %s\n" , rsp_code ) ;
  fprintf ( f_log_fopen_status, "ssa_msg          : %s\n" , ssa_msg ) ;
>>>>>>> local
  rc = 0 ;
=======
  fprintf ( f_log_fopen_status, "Session Id       : %ld\n" , *l_session_id ) ;
  fprintf ( f_log_fopen_status, "rsp_code         : %s\n" , c_rsp_code ) ;
  fprintf ( f_log_fopen_status, "ssa_msg          : %s\n" , c_ssa_msg ) ;
  l_rc = 0 ;
>>>>>>> local
  goto ret ;

ret:
  return ( l_rc ) ;
}

/**********************************************************************
 End of subroutine s_test_ssa_open                                    *
**********************************************************************/

// To build keys on names or addresses

static long s_test_ssa_get_keys (

<<<<<<< HEAD
  long    sockh ,
  long    *session_id ,
  char    *sysName ,
  char    *population ,
  char    *controls ,
  char    *record ,
  long    recordLength ,
  char    *recordEncType,
  char    *ky_fld_ky_lvl,
<<<<<<< HEAD
  char    *ID
) {
=======
  char    *c_ID
 ) {
>>>>>>> local
=======
  long    l_sockh ,
  long    *l_session_id ,
  char    *c_sysName ,
  char    *c_population ,
  char    *c_controls ,
  char    *c_record ,
  long    l_recordLength ,
  char    *c_recordEncType,
  char    *c_ky_fld_ky_lvl,
  char    *c_ID
 ) {
  
>>>>>>> local
  int     i ;
  long    l_rc ;
  char    *a_keys_array[SSA_SI_MAX_KEYS] ;
  char    a_keys_data[SSA_SI_MAX_KEYS*SSA_SI_KEY_SZ] ;
  char    c_rsp_code[SSA_SI_RSP_SZ] ;
  char    c_ssa_msg[SSA_SI_SSA_MSG_SZ] ;
  long    l_num ;
  char    *c_p ;

  for (i = 0 ; i < (int)rangeof (a_keys_array) ; ++i)
    a_keys_array[i] = a_keys_data + i * SSA_SI_KEY_SZ ;

  fprintf ( f_log_fopen_status, "------ ssan3_get_keys_encoded ------\n") ;
  fprintf ( f_log_fopen_status, "Session Id       : %ld\n" , *l_session_id) ;
  fprintf ( f_log_fopen_status, "System           : %s\n" , c_sysName) ;
  fprintf ( f_log_fopen_status, "Population       : %s\n" , c_population) ;
  fprintf ( f_log_fopen_status, "Controls         : %s\n" , c_controls) ;
  fprintf ( f_log_fopen_status, "Key field data   : %s\n" , c_record) ;
  fprintf ( f_log_fopen_status, "Key field size   : %ld\n" , l_recordLength) ;
  fprintf ( f_log_fopen_status, "Key field encoding type : %s\n" , c_recordEncType) ;

  l_num = 0 ;
  l_rc  =
    ssan3_get_keys_encoded (
      l_sockh ,
      l_session_id ,
      c_sysName ,
      c_population ,
      c_controls ,
      c_rsp_code ,
      SSA_SI_RSP_SZ ,
      c_ssa_msg ,
      SSA_SI_SSA_MSG_SZ ,
      c_record ,
      l_recordLength ,
      c_recordEncType ,
      &l_num ,
      a_keys_array ,
      SSA_SI_KEY_SZ
    ) ;

  if ( l_rc < 0 ) {
    fprintf ( f_log_fopen_status, "rc               : %ld\n" , l_rc ) ;
    l_rc = 1 ;
    goto ret ;
  }

  if ( c_rsp_code[0] != '0' ) {
    fprintf ( f_log_fopen_status, "rsp_code         : %s\n" , c_rsp_code ) ;
    fprintf ( f_log_fopen_status, "ssa_msg          : %s\n" , c_ssa_msg ) ;
    l_rc = -1 ;
    goto ret ;
  }

  fprintf ( f_log_fopen_status, "\n" ) ;
<<<<<<< HEAD
  fprintf ( f_log_fopen_status, "Session Id       : %ld\n" , *session_id ) ;
  fprintf ( f_log_fopen_status, "rsp_code         : %s\n" , rsp_code ) ;
  fprintf ( f_log_fopen_status, "ssa_msg          : %s\n" , ssa_msg ) ;
  fprintf ( f_log_fopen_status, "Count            : %ld\n" , num ) ;
  

  for ( i = 0 ; i < num ; ++i ) {
    p = keys_array[i] ;
<<<<<<< HEAD
    fprintf ( output_fopen_status ,"%.*s%s%s\n" , SSA_SI_KEY_SZ , p ,ky_fld_ky_lvl,ID) ;
    //printf ("%3d '%.*s'\n" , i+1 , SSA_SI_KEY_SZ , p) ;  //-- OUTPUT
=======
    fprintf ( f_output_fopen_status ,"%.*s%s%s\n" , SSA_SI_KEY_SZ , p ,ky_fld_ky_lvl,c_ID) ;
>>>>>>> local
=======
  fprintf ( f_log_fopen_status, "Session Id       : %ld\n" , *l_session_id ) ;
  fprintf ( f_log_fopen_status, "rsp_code         : %s\n" , c_rsp_code ) ;
  fprintf ( f_log_fopen_status, "ssa_msg          : %s\n" , c_ssa_msg ) ;
  fprintf ( f_log_fopen_status, "Count            : %ld\n" , l_num ) ;

  for ( i = 0 ; i < l_num ; ++i ) {
    c_p = a_keys_array[i] ;
    fprintf ( f_output_fopen_status ,"%.*s%s%s\n" , SSA_SI_KEY_SZ , c_p ,c_ky_fld_ky_lvl,c_ID) ;
>>>>>>> local
  }

  l_rc = 0 ;
  goto ret ;

ret:
  return ( l_rc ) ;
}

/**********************************************************************
 End of subroutine s_test_ssa_get_keys                                *
**********************************************************************/

// To terminate an open session to SSA-NAME3

static long s_test_ssa_close (

  long    l_sockh ,
  long    *l_session_id ,
  char    *c_sysName ,
  char    *c_population ,
  char    *c_controls
 ) {

  long    l_rc ;
  char    c_rsp_code[SSA_SI_RSP_SZ] ;
  char    c_ssa_msg[SSA_SI_SSA_MSG_SZ] ;

  time_t my_time ;                                              // Time to get current time
  struct tm * timeinfo ;                                        // struct tm pointer
  time ( &my_time ) ;                                           // call time() to get current date/time
  timeinfo = localtime ( &my_time ) ;                           // Localtime

  i_YYYY = timeinfo->tm_year+1900 ;                             // Year
  i_MM   = timeinfo->tm_mon+1 ;                                 // Month
  i_DD   = timeinfo->tm_mday ;                                  // Date
  i_HH24 = timeinfo->tm_hour ;                                  // Hours
  i_MI   = timeinfo->tm_min ;                                   // Minutes
  i_SS   = timeinfo->tm_sec ;                                   // Seconds

  fprintf ( f_log_fopen_status, "------ ssan3_close ------\n" ) ;
  fprintf ( f_log_fopen_status, "Session Id       : %ld\n" , *l_session_id ) ;
  fprintf ( f_log_fopen_status, "System           : %s\n" , c_sysName ) ;
  fprintf ( f_log_fopen_status, "Population       : %s\n" , c_population ) ;
  fprintf ( f_log_fopen_status, "Controls         : %s\n" , c_controls ) ;

  l_rc = ssan3_close (
    l_sockh ,
    l_session_id ,
    c_sysName ,
    c_population ,
    c_controls ,
    c_rsp_code ,
    SSA_SI_RSP_SZ ,
    c_ssa_msg ,
    SSA_SI_SSA_MSG_SZ
  ) ;

  if (l_rc < 0) {
    fprintf ( f_log_fopen_status, "rc               : %ld\n" , l_rc) ;
    l_rc = 1 ;
    goto ret ;
  }

  if ( c_rsp_code[0] != '0' ) {
    fprintf ( f_log_fopen_status, "rsp_code         : %s\n" , c_rsp_code ) ;
    fprintf ( f_log_fopen_status, "ssa_msg          : %s\n" , c_ssa_msg ) ;
    l_rc = 1 ;
    goto ret ;
  }

  fprintf ( f_log_fopen_status, "\n" ) ;
  fprintf ( f_log_fopen_status, "Session Id       : %ld\n" , *l_session_id ) ;
  fprintf ( f_log_fopen_status, "rsp_code         : %s\n" , c_rsp_code ) ;
  fprintf ( f_log_fopen_status, "ssa_msg          : %s\n" , c_ssa_msg ) ;
  fprintf ( f_log_fopen_status, "\n------ Execution END DATE AND TIME ------\n" ) ;
  fprintf ( f_log_fopen_status, "%d/%d/%d %d-%d-%d \n", i_YYYY, i_MM, i_DD, i_HH24, i_MI, i_SS ) ;
  l_rc = 0 ;
  goto ret ;

ret:
  return ( l_rc ) ;
}

/**********************************************************************
 End of subroutine s_test_ssa_close                                   *
**********************************************************************/

<<<<<<< HEAD
static void s_doExit ( char *func) {
  printf ("Error occurred in '%s'\n" , func) ;
  exit (EXIT_FAILURE) ;
=======
// Error occurred in subroutines s_test_ssa_open, s_test_ssa_get_keys, s_test_ssa_close

static void s_doExit ( char *func ) {
  printf ( "Error occurred in '%s'\n" , func ) ;
  exit ( EXIT_FAILURE ) ;
>>>>>>> local
}

/**********************************************************************
 End of subroutine s_doExit                                           *
**********************************************************************/
<<<<<<< HEAD

static void s_mkeKey_open () {
=======
>>>>>>> local

// s_test_ssa_open subroutine called in s_mkeKey_open subroutine

static void s_mkeKey_open ( ) {
    // Establish a session.
    l_rc =
    s_test_ssa_open (
      l_sockh ,
      &l_session_id ,
      "default" ,
      p_population ,
<<<<<<< HEAD
<<<<<<< HEAD
      "") ;

    if (0 != rc)
      s_doExit ("s_test_ssa_open") ;
=======
      "" 
=======
      ""
>>>>>>> local
    ) ;

    if ( 0 != l_rc )
      s_doExit ( "s_test_ssa_open" ) ;
>>>>>>> local

}

/**********************************************************************
 End of subroutine s_mkeKey_open                                      *
**********************************************************************/

// s_test_ssa_get_keys subroutine called in s_mkeKey_getKey subroutine

static void s_mkeKey_getKey (
  char *Limited ,
  char *Standard ,
  char *Extended ,
  char *ky_fld_ky_lvl1 ,
  char *ky_fld_ky_lvl2 ,
  char *ky_fld_ky_lvl3 ,
  char *c_ID
 ) {

  // Key levels : Limited , Standard , Extended
  char *key_level [ 3 ]     = { Limited , Standard , Extended } ;

  // c_controls
  char *ky_fld_ky_lvl [ 3 ] = { ky_fld_ky_lvl1 , ky_fld_ky_lvl2 , ky_fld_ky_lvl3 } ;

  for( i_idx = 0 ; i_idx <= 2 ; i_idx++ ) {
    l_rc =
      s_test_ssa_get_keys (
        l_sockh ,
        &l_session_id ,
        "default" ,
        p_population ,
<<<<<<< HEAD
        key_level[i] ,
        current_rec ,
        cur_rec_len ,
        "TEXT",
        ky_fld_ky_lvl [ i ] ,
        ID
      ) ;
      
    //fprintf ( output_fopen_status , "%s" , ky_fld_ky_lvl [ i ] ) ;
    //fprintf (output_fopen_status,"%s",ID) ;
=======
        key_level[i_idx] ,
        c_current_rec ,
        i_cur_rec_len ,
        "TEXT",
        ky_fld_ky_lvl [ i_idx ] ,
        c_ID
    ) ;
<<<<<<< HEAD
    
>>>>>>> local
    if ( 0 != rc )
=======

    if ( 0 != l_rc )
>>>>>>> local
      s_doExit ( "s_test_ssa_get_keys" ) ;
  }

}

/**********************************************************************
 End of subroutine s_mkeKey_getKey                                    *
**********************************************************************/

// s_test_ssa_close subroutine called in s_mkeKey_close subroutine

static void s_mkeKey_close ( ) {
  l_rc =
    s_test_ssa_close (
      l_sockh ,
      &l_session_id ,
      "default" ,
      p_population ,
      ""
  ) ;

<<<<<<< HEAD
<<<<<<< HEAD
  if (0 != rc)
    s_doExit ("s_test_ssa_close") ;
=======
  if ( 0 != rc )
=======
  if ( 0 != l_rc )
>>>>>>> local
    s_doExit ( "s_test_ssa_close" ) ;
>>>>>>> local
}

/**********************************************************************
 End of subroutine s_mkeKey_close                                     *
**********************************************************************/

<<<<<<< HEAD
void s_print_usage() {
    printf("MkeKey -d 101 -r 1001 -p india -i E:/SurendraK/Work/SSAProcs/Input/ -o E:/SurendraK/Work/SSAProcs/Output/ -l E:/SurendraK/Work/SSAProcs/Log/") ;
=======
// This subroutine is default parameter of getopt in s_getParameter

void s_print_usage( ) {
    printf( "MkeKey -d 101 -r 1001 -p india -i E:/SurendraK/Work/SSAProcs/Input/ -o E:/SurendraK/Work/SSAProcs/Output/ -l E:/SurendraK/Work/SSAProcs/Log/" ) ;
>>>>>>> local
}

/**********************************************************************
 End of subroutine s_print_usage                                      *
**********************************************************************/

// This subroutine takes run parameters.

static void s_getParameter( int argc , char *argv[] ) {

  time_t my_time ;                                              // Time to get current time
  struct tm * timeinfo ;                                        // struct tm pointer
  time (&my_time) ;                                             // call time() to get current date/time
<<<<<<< HEAD
  timeinfo = localtime (&my_time);                              // Localtime                           
  
<<<<<<< HEAD
  YYYY = timeinfo->tm_year+1900;
  MM   = timeinfo->tm_mon+1;
  DD   = timeinfo->tm_mday;
  HH24  = timeinfo->tm_hour;
  MI   = timeinfo->tm_min;
  SS   = timeinfo->tm_sec;

  while ( ( option = getopt ( argc , argv , "d:r:p:i:o:l:" ) ) != -1 ) {
    switch (option) {
=======
  i_YYYY = timeinfo->tm_year+1900 ;                             // Year   
  i_MM   = timeinfo->tm_mon+1 ;                                 // Month  
  i_DD   = timeinfo->tm_mday ;                                  // Date   
  i_HH24 = timeinfo->tm_hour ;                                  // Hours  
=======
  timeinfo = localtime (&my_time);                              // Localtime

  i_YYYY = timeinfo->tm_year+1900 ;                             // Year
  i_MM   = timeinfo->tm_mon+1 ;                                 // Month
  i_DD   = timeinfo->tm_mday ;                                  // Date
  i_HH24 = timeinfo->tm_hour ;                                  // Hours
>>>>>>> local
  i_MI   = timeinfo->tm_min ;                                   // Minutes
  i_SS   = timeinfo->tm_sec ;                                   // Seconds

  while ( ( i_option = getopt ( argc , argv , "d:r:p:i:o:l:" ) ) != -1 ) {
    switch (i_option) {
>>>>>>> local
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
      i_len_of_dir = strlen(p_infdir) ;                         // Length of input file directory
      c_flg_slash = p_infdir[i_len_of_dir-1] ;                  // Last character of a String is / forward slash

      if(c_flg_slash != c_forward_slash[0]) {
        p_infdir = strcat(p_infdir,c_forward_slash) ;
      }
  }

  if(strchr(p_infdir,'\\')) {
      i_len_of_dir = strlen(p_infdir) ;                         // Length of output file directory
      c_flg_slash = p_infdir[i_len_of_dir-1] ;                  // Last character of a String is / backslash

      if(c_flg_slash != c_back_slash[0]) {
        p_infdir = strcat(p_infdir,c_back_slash);
      }
  }

// Check Output file directory ends with backslash or forward , if not add it
  if(strchr(p_outfdir,'/')) {
      i_len_of_dir = strlen(p_outfdir) ;                        // Length of log file directory
      c_flg_slash = p_outfdir[i_len_of_dir-1] ;                 // Last character of a String is / forward slash

      if(c_flg_slash != c_forward_slash[0]) {
        p_outfdir = strcat(p_outfdir,c_forward_slash) ;
      }
  }

  if(strchr(p_outfdir,'\\')) {
      i_len_of_dir = strlen(p_outfdir) ;                        // Length of input file directory
      c_flg_slash = p_outfdir[i_len_of_dir-1] ;                 // Last character of a String is / back slash

      if(c_flg_slash != c_back_slash[0]) {
        p_outfdir = strcat(p_outfdir,c_back_slash);
      }
  }

// Check log file directory ends with backslash or forward , if not add it
  if(strchr(p_logfdir,'/')) {
      i_len_of_dir = strlen(p_logfdir) ;                        // Length of input file directory
      c_flg_slash = p_logfdir[i_len_of_dir-1] ;                 // Last character of a String is / forward slash

      if(c_flg_slash != c_forward_slash[0]) {
        p_logfdir = strcat(p_logfdir,c_forward_slash) ;
      }
  }

  if(strchr(p_logfdir,'\\')) {
      i_len_of_dir = strlen(p_logfdir) ;                        // Length of input file directory
      c_flg_slash = p_logfdir[i_len_of_dir-1] ;                 // Last character of a String is / back slash

      if(c_flg_slash != c_back_slash[0]) {
        p_logfdir = strcat(p_logfdir,c_back_slash) ;
      }
  }

  // Copy all directory name into different variables
  strcpy( a_input_file_directory ,p_infdir ) ;
  strcpy( a_output_file_directory ,p_outfdir ) ;
  strcpy( a_log_file_direcory ,p_logfdir ) ;
  strcpy( c_data_set , p_data_set ) ;
  strcpy( c_run_time , p_run_time ) ;
  c_sss_rrrr = strcat(p_data_set ,p_run_time) ;                 // Concatenate data set number and run time number

  strcpy( a_sss_rrrr_in_file , c_sss_rrrr ) ;                   // Copy value of sss_rrrr to sss_rrrr_in_file
  strcpy( a_sss_rrrr_out_file , c_sss_rrrr ) ;                  // Copy value of sss_rrrr to sss_rrrr_out_file
  strcpy( a_sss_rrrr_log_file , c_sss_rrrr ) ;                  // Copy value of sss_rrrr to sss_rrrr_log_file

  // Log_file concatenate with timestamp
<<<<<<< HEAD
  sprintf(sss_rrrr_timestamp,"%s_%d_%d_%d_%d_%d_%d",sss_rrrr_log_file,YYYY,MM,DD,HH24,MI,SS);
=======
  sprintf(a_sss_rrrr_timestamp,"%s_MkeTag_%d_%d_%d_%d_%d_%d",a_sss_rrrr_log_file,i_YYYY,i_MM,i_DD,i_HH24,i_MI,i_SS);
>>>>>>> local

  c_infname  = strcat( a_sss_rrrr_in_file ,".tag" ) ;           // Input file name with extension
  c_ofname   = strcat( a_sss_rrrr_out_file,".oke" ) ;           // Output file name with extension
  c_logfname = strcat( a_sss_rrrr_timestamp,".log" ) ;          // Log file name with extension

  strcpy( a_cpy_infname,c_infname ) ;                           // Copy of Input file name
  strcpy( a_cpy_ofname,c_ofname ) ;                             // Copy of output file name
  strcpy( a_cpy_logfname,c_logfname ) ;                         // Copy of log file name

  // Concatenate Directory path with Input file name
  c_dir_in_fl_ext = strcat( a_input_file_directory , a_cpy_infname );
  strcpy( a_cpy_dir_in_fl_ext , c_dir_in_fl_ext ) ;

  // Concatenate Directory path with output file name
  c_dir_out_fl_ext = strcat( a_output_file_directory , a_cpy_ofname );
  strcpy( a_cpy_dir_out_fl_ext , c_dir_out_fl_ext ) ;

  // Concatenate Directory path with log file name
  c_dir_log_fl_ext = strcat( a_log_file_direcory , a_cpy_logfname );
  strcpy( a_cpy_dir_log_fl_ext , c_dir_log_fl_ext ) ;


  f_input_fopen_status   = fopen ( a_cpy_dir_in_fl_ext , "r" ) ; // Open and read input file

<<<<<<< HEAD
  
<<<<<<< HEAD
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
=======
  f_input_fopen_status   = fopen ( a_cpy_dir_in_fl_ext, "r" ) ; // Open and read input file
  
=======
>>>>>>> local
  // If Input file contains error output and log file will not create.
  if ( ! f_input_fopen_status ) {
    printf ( "Could not open file %s for input.\n" , f_input_fopen_status ) ;   // Error message while opening file
>>>>>>> local
    exit(1) ;
  }
  else {
    f_output_fopen_status  = fopen ( a_cpy_dir_out_fl_ext , "w" ) ;              // Open and write output file
    f_log_fopen_status     = fopen ( a_cpy_dir_log_fl_ext , "w" ) ;             // Open and write log file
  }

  if ( ! f_output_fopen_status ) {
    printf ( "Could not open file %s for output\n" ,f_output_fopen_status ) ;   // Error message while opening file
    exit(1) ;
  }

  if ( ! f_log_fopen_status ) {
    printf ( "Could not open file %s for error\n" ,f_log_fopen_status  ) ;      // Error message while opening file
    exit(1) ;
  }
}

/**********************************************************************
 End of subroutine s_getParameter                                     *
**********************************************************************/

int main ( int argc , char *argv[] ) {
  int i_records = 100000 ;                                      // Default records no
  i_id_len = strlen( c_search ) ;                               // Length of search string
  s_getParameter ( argc , argv ) ;                              // Subroutine to get parameter
  t = clock( ) ;                                                // Start time

  fprintf ( f_log_fopen_status, "------ Run Parameters ------" ) ;
  fprintf ( f_log_fopen_status, "\nData set no           : %s", c_data_set ) ;
  fprintf ( f_log_fopen_status, "\nRun time number       : %s", c_run_time ) ;
  fprintf ( f_log_fopen_status, "\nPopulation            : %s", p_population) ;
  fprintf ( f_log_fopen_status, "\nInput File Directory  : %s", p_infdir ) ;
  fprintf ( f_log_fopen_status, "\nOutput File Directory : %s", p_outfdir ) ;
  fprintf ( f_log_fopen_status, "\nLog File Directory    : %s\n", p_logfdir ) ;

  fprintf ( f_log_fopen_status, "\n------ File Names ------" ) ;
  fprintf ( f_log_fopen_status, "\nInput file name       : %s",a_cpy_infname ) ;
  fprintf ( f_log_fopen_status, "\nOutput file name      : %s",a_cpy_ofname ) ;
  fprintf ( f_log_fopen_status, "\nLog file name         : %s\n",a_cpy_logfname ) ;

  s_mkeKey_open( ) ;                                            // subroutine to open ssa connection

  // Call ssan3_get_keys

  // Read a input file line by line
  while( fgets ( c_current_rec , sizeof ( c_current_rec ) , f_input_fopen_status ) ) {
    ++i_rec_number ;

    // Calculate the length of the current record
    i_cur_rec_len = strlen( c_current_rec ) ;
    if ( i_cur_rec_len > 0 && c_current_rec[i_cur_rec_len-1] == '\n' ) {
      c_current_rec[--i_cur_rec_len] = '\0' ;
    }

    if ( strstr ( c_current_rec , c_search ) != NULL ) {

      i_record_read++ ;                                         // No of records read

      // Display so many records in so many seconds to execute
      if ( i_rec_number == i_records ) {
       end_time = clock( ) - t ;                                // End time
       d_time_taken = ( ( double )end_time )/CLOCKS_PER_SEC ;   // In seconds
       printf( "\nDisplay %d records in %f seconds to execute \n", i_records , d_time_taken ) ;      // Print time

       i_records = i_records * 2 ;                              // 10 lakh records multiply by 2
      }

      // To find Id
      c_ptr_id       = strstr ( c_current_rec , c_search ) ;    // Search *Id* in current record capture string from *Id* to till end of the string
      i_id_start_pos = (c_ptr_id - c_current_rec) ;             // Starting position of *Id*
      i_pos_afr_id   = ( i_id_start_pos + i_id_len ) ;          // Position after *Id*

      for( i_idx = i_pos_afr_id ; i_idx < i_cur_rec_len; i_idx++ ) {
        if( c_current_rec[i_idx] == c_character ) {
            i_asterisk_start_pos = i_idx ;                      // After *Id* first * position
            break ;
        }
      }

      i_frt_ast_pos = ( i_asterisk_start_pos - i_pos_afr_id ) ; // First asterisk position after *Id*

      strncpy( c_ID ,c_current_rec + i_pos_afr_id, i_frt_ast_pos ) ;   // Substring Id

      // IF Person Name , Organization Name and Address_Part1 are empty throw an error message
      if ( strstr ( c_current_rec , "Person_Name" ) == NULL &&
           strstr ( c_current_rec , "Organization_Name" ) == NULL &&
           strstr ( c_current_rec , "Address_Part1" ) == NULL ) {
         i_record_read-- ;                                      // Deduct 1 from record read
         i_error_record_read++ ;                                // Error record count
         fprintf ( f_log_fopen_status, "\n\n------ Error records ------\n") ;
         fprintf ( f_log_fopen_status, "\nRecord no : %d Error Message : %s", i_rec_number ,"Missing Person Name, Organization name, Address Part 1 fields" ) ;
         fprintf ( f_log_fopen_status, "\nRecord    : %s\n", c_current_rec ) ;
         fprintf ( f_log_fopen_status, "---------------------------\n\n") ;

         printf ( "\n\n------ Error records ------\n") ;
         printf ( "\nRecord no :%d Error Message :%s", i_rec_number ,"Missing Person Name, Organization name, Address Part 1 fields" ) ;
         printf ( "\nRecord    :%s\n", c_current_rec ) ;
         printf ( "---------------------------\n\n") ;

      }

      // Check Person_Name is inside the current record
      if ( strstr ( c_current_rec , "Person_Name" ) != NULL ) {
        char *PL = "FIELD=Person_Name KEY_LEVEL=Limited" ;
        char *PS = "FIELD=Person_Name KEY_LEVEL=Standard" ;
        char *PE = "FIELD=Person_Name KEY_LEVEL=Extended" ;

        char *abv_PLim   = "PM" ;                               // abrevation of Person_Name Limited
        char *abv_PStand = "PS" ;                               // abrevation of Person_Name Standard
        char *abv_PExt   = "PX" ;                               // abrevation of Person_Name Extended

        i_pn_records++ ;                                        // No of records count that contains Person_Name
        i_PM_ky++ ;                                             // No of records count that contains Person_Name key Limited
        i_PS_ky++ ;                                             // No of records count that contains Person_Name key Standard
        i_PX_ky++ ;                                             // No of records count that contains Person_Name key Extended
        i_total_kys_written = i_total_kys_written + 3 ;

        // Call s_mkeKey_getKey
        s_mkeKey_getKey( PL , PS , PE , abv_PLim , abv_PStand , abv_PExt ,c_ID ) ;
      }

      // Check Organization_Name is inside the current record
      if ( strstr ( c_current_rec , "Organization_Name" ) != NULL ) {
        char *OL = "FIELD=Organization_Name KEY_LEVEL=Limited" ;
        char *OS = "FIELD=Organization_Name KEY_LEVEL=Standard" ;
        char *OE = "FIELD=Organization_Name KEY_LEVEL=Extended" ;

        char *abv_OLim   = "OM" ;                               // abrevation of Organization_Name Limited
        char *abv_OStand = "OS" ;                               // abrevation of Organization_Name Standard
        char *abv_OExt   = "OX" ;                               // abrevation of Organization_Name Extended

        i_on_records++ ;                                        // No of records count that contains Organization_Name
        i_OM_ky++ ;                                             // No of records count that contains Organization_Name key Limited
        i_OS_ky++ ;                                             // No of records count that contains Organization_Name key Standard
        i_OX_ky++ ;                                             // No of records count that contains Organization_Name key Extended
        i_total_kys_written = i_total_kys_written + 3 ;

        // Call s_mkeKey_getKey
        s_mkeKey_getKey( OL , OS , OE , abv_OLim , abv_OStand , abv_OExt, c_ID ) ;
      }

      // Check Address_Part1 is inside the current record
      if ( strstr ( c_current_rec , "Address_Part1" ) != NULL ) {
        char *AL = "FIELD=Address_Part1 KEY_LEVEL=Limited" ;
        char *AS = "FIELD=Address_Part1 KEY_LEVEL=Standard" ;
        char *AE = "FIELD=Address_Part1 KEY_LEVEL=Extended" ;

        char *abv_ALim   = "1M" ;                               // abrevation of Address_Part1 Limited
        char *abv_AStand = "1S" ;                               // abrevation of Address_Part1 Standard
        char *abv_AExt   = "1X" ;                               // abrevation of Address_Part1 Extended

        i_addp1_records++ ;                                     // No of records count that contains Address_Part1
        i_AM_ky++ ;                                             // No of records count that contains Address_Part1 key Limited
        i_AS_ky++ ;                                             // No of records count that contains Address_Part1 key Standard
        i_AX_ky++ ;                                             // No of records count that contains Address_Part1 key Extended
        i_total_kys_written = i_total_kys_written + 3 ;         // Total keys written

        // Call s_mkeKey_getKey
        s_mkeKey_getKey( AL , AS , AE , abv_ALim , abv_AStand , abv_AExt ,c_ID ) ;
      }
    }
    else {

        // If Id field is missing display error message
        i_error_record_read++ ;                                 // Error record count
        fprintf ( f_log_fopen_status, "\n\n------ Error records ------\n") ;
        fprintf ( f_log_fopen_status, "\nRecord no : %d Error Message : %s", i_rec_number ,"Missing Id field" ) ;
        fprintf ( f_log_fopen_status, "\nRecord    : %s\n", c_current_rec ) ;
        fprintf ( f_log_fopen_status, "---------------------------\n") ;

        printf ( "\n\n------ Error record ------\n") ;
        printf ( "\nRecord no : %d Error Message : %s", i_rec_number ,"Missing Id field" ) ;
        printf ( "\nRecord    : %s\n", c_current_rec ) ;
        printf ( "---------------------------\n") ;
    }
  }

/**********************************************************************
 End of While loop                                                    *
**********************************************************************/

  s_mkeKey_close( ) ;                                           // subroutine to close ssa connection

  fprintf ( f_log_fopen_status, "\n-----------------------------------------\n" ) ;
  printf ( "\nNo of records in a file :%d\n", i_rec_number ) ;
  fprintf ( f_log_fopen_status, "\nRecords read                      : %d", i_record_read ) ;
  fprintf ( f_log_fopen_status, "\nError records                     : %d", i_error_record_read ) ;
  fprintf ( f_log_fopen_status, "\nPerson_Name records               : %d", i_pn_records ) ;
  fprintf ( f_log_fopen_status, "\nOrganization_Name records         : %d", i_on_records ) ;
  fprintf ( f_log_fopen_status, "\nAddress_Part1 records             : %d\n", i_addp1_records ) ;

  fprintf ( f_log_fopen_status, "\nPerson_Name Limited keys          : %d", i_PM_ky ) ;
  fprintf ( f_log_fopen_status, "\nPerson_Name Standard keys         : %d",  i_PS_ky ) ;
  fprintf ( f_log_fopen_status, "\nPerson_Name Extended keys         : %d\n",  i_PX_ky ) ;

  fprintf ( f_log_fopen_status, "\nOrganization_Name Limited keys    : %d", i_OM_ky ) ;
  fprintf ( f_log_fopen_status, "\nOrganization_Name Standard keys   : %d", i_OS_ky ) ;
  fprintf ( f_log_fopen_status, "\nOrganization_Name Extended keys   : %d\n", i_OX_ky ) ;

  fprintf ( f_log_fopen_status, "\nAddress_Part1 Limited keys        : %d", i_AM_ky ) ;
  fprintf ( f_log_fopen_status, "\nAddress_Part1 Standard keys       : %d", i_AS_ky ) ;
  fprintf ( f_log_fopen_status, "\nAddress_Part1 Extended keys       : %d\n", i_AX_ky ) ;

  fprintf ( f_log_fopen_status, "\nTotal keys written                : %d\n", i_total_kys_written ) ;

  // Addition of all the keys and check it is match with total keys
  i_addition_key = i_PM_ky + i_PS_ky + i_PX_ky + i_OM_ky + i_OS_ky + i_OX_ky + i_AM_ky + i_AS_ky + i_AX_ky ;

  if ( i_addition_key != i_total_kys_written ) {
    fprintf ( f_log_fopen_status, "\nMissmatch in counts\n") ;
  }

  end_time = clock( ) - t ;                                     // End time
  d_time_taken = ( ( double )end_time )/CLOCKS_PER_SEC ;        // In seconds
  printf( "\nProcessed %d tagged data records in %f seconds to execute \n" , i_rec_number , d_time_taken ) ;    // Print time
  fprintf( f_log_fopen_status , "\nProcessed %d tagged data records in %f seconds to execute \n", i_rec_number ,d_time_taken ) ;    // Print time

  fclose ( f_input_fopen_status ) ;                             // input_fopen_status
  fclose ( f_output_fopen_status ) ;                            // Close output_fopen_status
  fclose ( f_log_fopen_status ) ;                               // Close log_fopen_status

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

<<<<<<< HEAD
          s_mkeKey_getKey( OL , OS , OE , abv_OLim , abv_OStand , abv_OExt,ID ) ;
        }
=======
      4.1 Run Parameters
>>>>>>> local

      4.2 Compile Procedure

      4.3 Execute procedure in different way

      4.4 Execution Start and End date and time

      4.5 Subroutines

          4.5.1 Called by

          4.5.2 Calling

          4.5.3 Subroutine Structure

    5 Include Header

  Make Key

    It creates Key file sssrrrr.oke from tagged file sssrrrr.tag
    This procedure will create keys from tagged data.
    Tagged data contains Id, Person_Name, Organization_Name,
    Address_Part1, Postal_Area and others.
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

    Format of Output file:
      Column  1 to 8   : Key
      Column  9        : Key type  - P (Person_name), O (Organisation), 1 (Address_Part1)
      Column 10        : Key level - M (Limited), S (Standard), X (Extended)
      Column 11 onward : Id

    Format of log file:
      Log file will be created with date and time
      for eg. sssrrrr_MkeTag_YYYY_MM_DD_HH24_MI_SS.log

      Log file name contains below information.

      ------ EXECUTION START DATE AND TIME ------
      YYYY/MM/DD HH24 MI SS

      Displayed all run parameters which are used:
      Data set no           : data set number starting from 100 to 999
      Run time number       : Run time number starting from 1000 to 9999
      Population            : india
      Input File Directory  : Input File Directory
      Output File Directory : Output File Directory
      Log File Directory    : Log File Directory

      Displayed all file names:
      Input file name
      Output file name
      Log file name

      ------ EXECUTION END DATE AND TIME ------
      YYYY/MM/DD HH24 MI SS

      Display no of records in a file
      Display no of records read
      Display no of error records
      Display no of person_Name records
      Display no of organization_Name records
      Display no of address_Part1 records

      Display Person_Name Limited keys counts
      Display Person_Name Standard keys counts
      Display Person_Name Extended keys counts

      Display Organization_Name Limited keys counts
      Display Organization_Name Standard keys counts
      Display Organization_Name Extended keys counts

      Display Address_Part1 Limited keys counts
      Display Address_Part1 Standard keys counts
      Display Address_Part1 Extended keys counts

      Total keys written

      Addition of all the keys and check it is match with total keys
      Processed so many tagged data records in so many seconds to execute

      Displayed processed so many tagged data records in so many seconds.

     Error message:Missing Person Name, Organization name, Address Part 1 fields
     If Person Name, Organization name and Address Part 1 fields is
     missing in the record then error will be display with
     record no with error message and record.

     Error message: Missing Id field

     If Id field is missing in the record then error will be display with
     record no with error message and record.


     Terminal output:

       Display if there are error records in a file with record no,
       error message and record.

       Verbose:
         Assume that there are millions of records in a file.
         This procedure will read first 10 lakhs records and display
         on terminal( command window ): 10 lakhs records read in so many
         seconds again it will read 2000000 records and display time. it will
         multiplier by 2 every time

         Displayed no of records in a file
       Displayed processed so many tagged data records in so many seconds.

  Procedure Name : MkeKey.c

    Creates Key file sssrrrr.oke from tagged file sssrrrr.tag.

  Copyright

    Copyright (c) 2017 IdentLogic Systems Private Limited

  Warnings
    If set data number and run number are empty it will throw an error
    like could not open file for input.
    If any one of the parameter is missing it will not create output
    or log file.

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

   Note :

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

   Execution Start and End date and time

     In log file, also contain starting and ending execution time.

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

   Header           Inbuild / External
   ----------       -------------------
   <stdio.h>      - Inbuild header file
   <stdlib.h>     - Inbuild header file
   <string.h>     - Inbuild header file
   "ssan3cl.h"    - Dedupe header
   <getopt.h>     - External header file
   <unistd.h>     - External header file
   <time.h>       - Inbuild header file


   ssan3cl.h - Dedupe header
   For this type of header file you need to install name3 server software
   after installation E:\ids\nm3\h will be created.
   ssan3cl.h file is there inside the h folder.

   For running this types of procedure successfully you need to set
   environment variables.

   Follow the steps:

   1. My Computer -->Property --> Advanced --> Environment variable -->
      System variables -->

   2. Variable Name  : PATH
      Variable value : E:\ids\nm3\bin;

      Variable Name  : LOG
      Variable value : E:\ids\nm3\log;

      Variable Name  : LIB
      Variable value : E:\ids\nm3\lib;

      Variable Name  : LIBPATH
      Variable value : E:\ids\nm3\lib;

      Variable Name  : INCLUDE
      Variable value : E:\ids\nm3\h;

      Variable Name  : SSATOP
      Variable value : E:\ids\nm3

      If any above variable is already there in the System variable,
      do not create again, only paste the url.
      Not neccessary the url are same in your pc.


   getopt.h
   --------

   If you dont have getopt.h header file download it from the internet.
   Url :
   https://github.com/skandhurkat/Getopt-for-Visual-Studio/blob/master/getopt.h
   and save it in the file with .h extension in E:\ids\nm3\h folder.

   unistd.h
   --------

   If you dont have unistd.h header file either download it from the internet or
   copy below code.

   #ifndef __STRICT_ANSI__
   # ifndef __MINGW32__
   #  include <io.h>
   #  include <process.h>

   # else
   #  include_next <unistd.h>
   # endif
   #endif

<<<<<<< HEAD

<<<<<<< HEAD
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
=======
=======
>>>>>>> local
*/
>>>>>>> local
