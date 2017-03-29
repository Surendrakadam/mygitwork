/*
 Procedure     : MkeKey.c
 Application   : De-Dupe
 Client        : Internal
 Copyright (c) : IdentLogic Systems Private Limited
 Author        : Surendra Kadam
 Creation Date : 20 March 2017
 Description   : Generates (Search) Keys from Tagged data for Key Levels
                  Limited (M), Standard (S) and Extended (X) for
                  Person_Name (P), Organisation_Name (O) and
                  Address_Part1 (1) as found in the Tagged data.

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
#include <getopt.h>                                             // Getoptions header
#include <unistd.h>                                             // Code's entry point have various constant, type and function declarations
#include <time.h>                                               // Timer header

#define rangeof( arr )  ( sizeof( arr ) / sizeof( ( arr )[ 0 ] ) )

// Global variables

  // dds NAME3 common variables rc, sockh, session_id

  long  l_rc ;                                                  // Indicate success or failure of open / close sesions

  long  l_sockh      = -1 ;                                     // Set to -1 as not calling the dds-NAME3 server

  // Should be -1 on the ddsn3_open call, if opening  a new session, a valid Session ID or any other call
  long  l_session_id = -1 ;

  int i_idx          = 0 ;                                      // i_idx for loop variable

  // Time variables
  int i_YYYY = 0 ;                                              // Year
  int i_MM   = 0 ;                                              // Month
  int i_DD   = 0 ;                                              // Date
  int i_HH24 = 0 ;                                              // Hour-24
  int i_MI   = 0 ;                                              // Minute
  int i_SS   = 0 ;                                              // Seconds

  // Procedure run parameters
  char *p_data_set   = "" ;                                     // Parameter data set number
  char *p_run_time   = "" ;                                     // Parameter run time data
  char *p_population = "" ;                                     // Population name
  char *p_infdir     = "" ;                                     // Parameter input file name
  char *p_outfdir    = "" ;                                     // Parameter output file directory
  char *p_logfdir    = "" ;                                     // Parameter log file directory
  int   p_verbose    = 0 ;                                      // Parameter verbose

  int  i_len_of_dir       = 0 ;                                 // Length of the directory
  char c_flg_slash ;                                            // check backslash or forward slash

  int  i_option = 0 ;                                           // Switch case variable

  char str_current_rec [ BUFSIZ ]  = {0} ;                      // Current record of a file
  int i_cur_rec_len    = 0 ;                                    // Current record length

  // File Open Statuses
  FILE  *f_input_fopen_status ;                                 // Input file name
  FILE  *f_output_fopen_status ;                                // Output file name
  FILE  *f_log_fopen_status ;                                   // Log file name

  char *str_ID        = "" ;                                    // Substring ID from the current record
  char *str_ptr_id    = "" ;                                    // Point to the string *ID* in current record till end of the current record

  // File Directories
  char a_str_input_file [500]  = {0} ;                          // Input file name
  char a_str_output_file [500] = {0} ;                          // Output file name
  char a_str_log_file [500]    = {0} ;                          // Log file name

  char a_str_dirinput_file [500]  = {0} ;                       // Input file directory with file name
  char a_str_diroutput_file [500] = {0} ;                       // Output file directory with file name
  char a_str_dirlog_file [500]    = {0} ;                       // Log file directory with file name

  int i_id_start_pos       = 0 ;                                // Starting position of *Id*
  int i_pos_afr_id         = 0 ;                                // Position after *ID*
  int i_asterisk_start_pos = 0 ;                                // After *Id* first * position
  int i_frt_ast_pos        = 0 ;                                // First asterisk position after *Id*

  clock_t t           = 0 ;                                     // Clock object
  clock_t end_time    = 0 ;                                     // End time
  double d_time_taken = 0.0 ;                                   // Time taken

  int i_record_read       = 0 ;                                 // No of read records counts
  int i_error_record_read = 0 ;                                 // No of error records counts
  int i_error_record_id   = 0 ;                                 // Missing id error record count
  int i_error_record_flds = 0 ;                                 // Error records where Person_Name Organization_Name AddressPart1 fields are missing
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

  char *S_K_forward_slash = "/" ;                               // Forward Slash
  char *S_K_back_slash    = "\\" ;                              // Back Slash

  char a_str_MM[100]        = {0} ;                             // Month
  char a_str_DD[100]        = {0} ;                             // Date
  char a_str_HH24[100]      = {0} ;                             // Hours
  char a_str_MI[100]        = {0} ;                             // Minutes
  char a_str_SS[100]        = {0} ;                             // Seconds

  int i_multiplier          = 0 ;

static void s_date_time ( ) {                                   // Compute current date and time elements YYYY, MM, DD, HH24, MI and SS

  time_t my_time ;                                              // Time to get current time
  struct tm * timeinfo ;                                        // struct tm pointer
  time ( &my_time) ;                                            // call time() to get current date/time
  timeinfo = localtime ( &my_time);                             // Localtime

  i_YYYY = timeinfo->tm_year+1900 ;                             // Year
  i_MM   = timeinfo->tm_mon+1 ;                                 // Month
  i_DD   = timeinfo->tm_mday ;                                  // Date
  i_HH24 = timeinfo->tm_hour ;                                  // Hours
  i_MI   = timeinfo->tm_min ;                                   // Minutes
  i_SS   = timeinfo->tm_sec ;                                   // Seconds

  if ( i_MM < 10 ) {                                            // Ensure month is a 2 digit string
    sprintf( a_str_MM , "0%d" , i_MM ) ;                        // Add prefix 0 if month less than 10
  }
  else {
    sprintf( a_str_MM , "%d" , i_MM ) ;                         // Keep it as it is
  }

  if( i_DD < 10 ) {                                             // Ensure date is a 2 digit string
    sprintf( a_str_DD , "0%d" , i_DD ) ;                        // Add prefix 0 if date less than 10
  }
  else {
    sprintf( a_str_DD , "%d" , i_DD ) ;                         // Keep it as it is
  }

  if( i_HH24 < 10 ) {                                           // Ensure hours is a 2 digit string
    sprintf( a_str_HH24 , "0%d" ,i_HH24 ) ;                     // Add prefix 0 if hours less than 10
  }
  else {
    sprintf( a_str_HH24 , "%d" ,i_HH24 ) ;                      // Keep it as it is
  }

  if( i_MI < 10 ) {                                             // Ensure minutes is a 2 digit string
    sprintf( a_str_MI , "0%d" ,i_MI ) ;                         // Add prefix 0 if minutes less than 10
  }
  else {
    sprintf( a_str_MI , "%d" ,i_MI ) ;                          // Keep it as it is
  }

  if( i_SS < 10 ) {                                             // Ensure seconds is a 2 digit string
    sprintf( a_str_SS , "0%d" ,i_SS ) ;                         // Add prefix 0 if seconds less than 10
  }
  else {
    sprintf( a_str_SS , "%d" ,i_SS ) ;                          // Keep it as it is
  }

}
/**********************************************************************
 End of subroutine s_date_time                                        *
**********************************************************************/

static long s_test_dds_open (                                   // To open a session to the dds-NAME3 services

  long    l_sockh ,                                             // Set to -1
  long    *l_session_id ,                                       // Set to -1
  char    *str_sysName ,                                        // Default
  char    *str_population ,                                     // Country name
  char    *str_controls                                         // Default

 ) {

  long    l_rc ;                                                // Indicate success or failure of open / close sesions
  char    a_str_rsp_code[SSA_SI_RSP_SZ] ;                       // Indicates the success or failure of a call to ssa-name3
  char    a_str_dds_msg[SSA_SI_SSA_MSG_SZ] ;                    // Error Message

  fprintf ( f_log_fopen_status, "Session Id       : %ld\n" , *l_session_id ) ;
  fprintf ( f_log_fopen_status, "System           : %s\n" , str_sysName ) ;
  fprintf ( f_log_fopen_status, "Population       : %s\n" , str_population ) ;

  l_rc =
    ssan3_open (
      l_sockh ,                                                 // Set to -1 as not calling the dds-NAME3 server
      l_session_id ,                                            // Set to -1
      str_sysName ,                                             // Defines location of the population rules
      str_population ,                                          // Country name
      str_controls ,                                            // Default
      a_str_rsp_code ,                                          // Indicates the success or failure of a call to ssa-name3
      SSA_SI_RSP_SZ ,                                           // Size of respose code
      a_str_dds_msg ,                                           // Error Message
      SSA_SI_SSA_MSG_SZ                                         // Size of error message
    ) ;

  if ( l_rc < 0 ) {
    fprintf ( f_log_fopen_status, "rc               : %ld\n" , l_rc ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }

  if ( a_str_rsp_code[0] != '0' && *l_session_id == -1 ) {
    fprintf ( f_log_fopen_status, "rsp_code         : %s\n" , a_str_rsp_code ) ;
    fprintf ( f_log_fopen_status, "dds_msg          : %s\n" , a_str_dds_msg ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }

  l_rc = 0 ;
  goto SUB_RETURN ;

SUB_RETURN:
  return ( l_rc ) ;

}
/**********************************************************************
 End of subroutine s_test_dds_open                                    *
**********************************************************************/

// To build keys on names or addresses

static long s_test_dds_get_keys (

  long    l_sockh ,                                             // Set to -1 as not calling the dds-NAME3 server
  long    *l_session_id ,                                       // Set to -1
  char    *str_sysName ,                                        // Defines location of the population rules
  char    *str_population ,                                     // Country name
  char    *str_controls ,                                       // Default
  char    *str_record ,                                         // Current record of a file
  long    l_recordLength ,                                      // Length of the current record in a file
  char    *str_recordEncType,                                   // Encrypted type by default is Text
  char    *str_ky_fld_ky_lvl,                                   // Format of a key field and key level
  char    *str_ID                                               // ID
 ) {

  int     i ;                                                   // i variable
  long    l_rc ;                                                // Indicate success or failure of open / close sesions
  char    *a_str_keys_array[SSA_SI_MAX_KEYS] ;                  // Key array with size
  char    a_str_keys_data[SSA_SI_MAX_KEYS*SSA_SI_KEY_SZ] ;      // Key field data
  char    a_str_rsp_code[SSA_SI_RSP_SZ] ;                       // Indicates the success or failure of a call to ssa-name3
  char    a_str_dds_msg[SSA_SI_SSA_MSG_SZ] ;                    // Error Message
  long    l_num ;                                               // Keys count
  char    *str_KEY ;                                            // Generate Keys

  for (i = 0 ; i < (int)rangeof (a_str_keys_array) ; ++i)
    a_str_keys_array[i] = a_str_keys_data + i * SSA_SI_KEY_SZ ;

  l_num = 0 ;
  l_rc  =
    ssan3_get_keys_encoded (
      l_sockh ,                                                 // Set to -1 as not calling the SSA-NAME3 server
      l_session_id ,                                            // Set to -1
      str_sysName ,                                             // Defines location of the population rules
      str_population ,                                          // Country name
      str_controls ,                                            // Default
      a_str_rsp_code ,                                          // Indicates the success or failure of a call to ssa-name3
      SSA_SI_RSP_SZ ,                                           // Size of respose code
      a_str_dds_msg ,                                           // Error Message
      SSA_SI_SSA_MSG_SZ ,                                       // Size of error message
      str_record ,                                              // Current record of a file
      l_recordLength ,                                          // Length of the current record in a file
      str_recordEncType ,                                       // Encrypted type by default is Text
      &l_num ,                                                  // Keys count
      a_str_keys_array ,                                        // Key Array
      SSA_SI_KEY_SZ                                             // Size of the key
    ) ;

  if ( l_rc < 0 ) {
    fprintf ( f_log_fopen_status, "ERROR: Return Code : %ld\n" , l_rc ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }

  if ( a_str_rsp_code[0] != '0' ) {
    fprintf ( f_log_fopen_status, "rsp_code         : %s\n" , a_str_rsp_code ) ;
    fprintf ( f_log_fopen_status, "dds_msg          : %s\n" , a_str_dds_msg ) ;
    l_rc = -1 ;
    goto SUB_RETURN ;
  }

  i_total_kys_written++ ;                                       // Total keys written
  for ( i = 0 ; i < l_num ; ++i ) {
    str_KEY = a_str_keys_array[i] ;
    fprintf ( f_output_fopen_status ,"%.*s%s%s\n" , SSA_SI_KEY_SZ , str_KEY ,str_ky_fld_ky_lvl,str_ID) ;
  }

  l_rc = 0 ;
  goto SUB_RETURN ;

SUB_RETURN:
  return ( l_rc ) ;

}
/**********************************************************************
 End of subroutine s_test_dds_get_keys                                *
**********************************************************************/

// To terminate an open session to dds-NAME3

static long s_test_dds_close (

  long    l_sockh ,                                             // Set to -1 as not calling the SSA-NAME3 server
  long    *l_session_id ,                                       // Set to -1
  char    *str_sysName ,                                        // Defines location of the population rules
  char    *str_population ,                                     // Country name
  char    *str_controls                                         // Default
 ) {

  long    l_rc ;                                                // Indicate success or failure of open / close sesions
  char    a_str_rsp_code[SSA_SI_RSP_SZ] ;                       // Indicates the success or failure of a call to ssa-name3
  char    a_str_dds_msg[SSA_SI_SSA_MSG_SZ] ;                    // Error Message

  s_date_time();

  l_rc = ssan3_close (
    l_sockh ,                                                   // Set to -1 as not calling the SSA-NAME3 server
    l_session_id ,                                              // Set to -1
    str_sysName ,                                               // Defines location of the population rules
    str_population ,                                            // Country name
    str_controls ,                                              // Default
    a_str_rsp_code ,                                            // Indicates the success or failure of a call to ssa-name3
    SSA_SI_RSP_SZ ,                                             // Size of respose code
    a_str_dds_msg ,                                             // Error Message
    SSA_SI_SSA_MSG_SZ                                           // Size of error message
  ) ;

  if (l_rc < 0) {
    fprintf ( f_log_fopen_status, "rc               : %ld\n" , l_rc) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }

  if ( a_str_rsp_code[0] != '0' ) {
    fprintf ( f_log_fopen_status, "rsp_code         : %s\n" , a_str_rsp_code ) ;
    fprintf ( f_log_fopen_status, "dds_msg          : %s\n" , a_str_dds_msg ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }

  fprintf ( f_log_fopen_status, "Ended %d-%s-%s %s:%s:%s ", i_YYYY, a_str_MM, a_str_DD, a_str_HH24, a_str_MI, a_str_SS ) ;
  l_rc = 0 ;
  goto SUB_RETURN ;

SUB_RETURN:
  return ( l_rc ) ;

}
/**********************************************************************
 End of subroutine s_test_dds_close                                   *
**********************************************************************/

// Error occurred in subroutines s_test_dds_open, s_test_dds_get_keys, s_test_dds_close

static void s_doExit ( char *func ) {
  printf ( "Error occurred in '%s'\n" , func ) ;
  exit ( 1 ) ;

}
/**********************************************************************
 End of subroutine s_doExit                                           *
**********************************************************************/

// s_test_dds_open subroutine called in s_mkeKey_open subroutine

static void s_mkeKey_open ( ) {
    // Establish a session.
    l_rc =
    s_test_dds_open (
      l_sockh ,                                                 // Set to -1 as not calling the SSA-NAME3 server
      &l_session_id ,                                           // Set to -1
      "default" ,                                               // Defines location of the population rules
      p_population ,                                            // Country Name
      ""
    ) ;

    if ( 0 != l_rc )
      s_doExit ( "s_test_dds_open" ) ;

}
/**********************************************************************
 End of subroutine s_mkeKey_open                                      *
**********************************************************************/

// s_test_dds_get_keys subroutine called in s_mkeKey_getKey subroutine

static void s_mkeKey_getKey (
  char *Limited ,                                               // Limited KEY LEVEL
  char *Standard ,                                              // Standard KEY LEVEL
  char *Extended ,                                              // Extended KEY LEVEL
  char *ky_fld_ky_lvl1 ,                                        // Format of KEY FIELD AND KEY LEVEL
  char *ky_fld_ky_lvl2 ,                                        // Format of KEY FIELD AND KEY LEVEL
  char *ky_fld_ky_lvl3 ,                                        // Format of KEY FIELD AND KEY LEVEL
  char *str_ID                                                  // ID
 ) {

  // Key levels : Limited , Standard , Extended
  char *key_level [ 3 ]     = { Limited , Standard , Extended } ;

  // Controls
  char *ky_fld_ky_lvl [ 3 ] = { ky_fld_ky_lvl1 , ky_fld_ky_lvl2 , ky_fld_ky_lvl3 } ;

  for( i_idx = 0 ; i_idx <= 2 ; i_idx++ ) {
    l_rc =
      s_test_dds_get_keys (
        l_sockh ,                                               // Set to -1 as not calling the SSA-NAME3 server
        &l_session_id ,                                         // Set to -1
        "default" ,                                             // System name
        p_population ,                                          // Country Name
        key_level[i_idx] ,                                      // Key level
        str_current_rec ,                                       // Current record
        i_cur_rec_len ,                                         // Length of a current record
        "TEXT",                                                 // Encrypted type by default is Text
        ky_fld_ky_lvl [ i_idx ] ,                               // Format of KEY FIELD and KEY LEVEL
        str_ID                                                  // ID
    ) ;

    if ( 0 != l_rc )
      s_doExit ( "s_test_dds_get_keys" ) ;
  }

}
/**********************************************************************
 End of subroutine s_mkeKey_getKey                                    *
**********************************************************************/

// s_test_dds_close subroutine called in s_mkeKey_close subroutine

static void s_mkeKey_close ( ) {
  l_rc =
    s_test_dds_close (
      l_sockh ,                                                 // Set to -1 as not calling the SSA-NAME3 server
      &l_session_id ,                                           // Set to -1
      "default" ,                                               // System name
      p_population ,                                            // Country name
      ""
  ) ;

  if ( 0 != l_rc )
    s_doExit ( "s_test_dds_close" ) ;

}
/**********************************************************************
 End of subroutine s_mkeKey_close                                     *
**********************************************************************/

// This subroutine is default parameter of getopt in s_getParameter

void s_print_usage( ) {
  printf(
    "MkeKey -d <data_set> -r <run> -p <population> -i <input_file_directory> -o <output_file_directory> -l <log_file_directory> -v <Verbose>\n\nExample:\n\n" ) ;
  printf(
    "MkeKey -d 101 -r 1001 -p india -i E:/SurendraK/Work/DeDupeProcs/Input/ -o E:/SurendraK/Work/DeDupeProcs/Output/ -l E:/SurendraK/Work/SSAProcs/Log/ -v 10000\n" ) ;

}
/**********************************************************************
 End of subroutine s_print_usage                                      *
**********************************************************************/

// This subroutine takes run parameters.

static void s_getParameter( int argc , char *argv[] ) {
  s_date_time();
  while ( ( i_option = getopt ( argc , argv , "d:r:p:i:o:l:v:" ) ) != -1 ) {
    switch (i_option) {
      case 'd' :                                                // Data set parameter
        p_data_set   = optarg ;
        break ;
      case 'r' :                                                // Run time parameter
        p_run_time   = optarg ;
        break ;
      case 'p' :                                                // Population parameter
        p_population = optarg ;
        break ;
      case 'i' :                                                // Input file directory parameter
        p_infdir     = optarg ;
        break ;
      case 'o' :                                                // Output file directory parameter
        p_outfdir    = optarg ;
        break ;
      case 'l' :                                                // Log file directory parameter
        p_logfdir    = optarg ;
        break ;
      case 'v' :                                                // Verbose
        p_verbose    = atoi( optarg ) ;
        break ;
      default:
        s_print_usage ( ) ;                                     // Default parameter
        exit ( 1 ) ;
    }
  }

  i_multiplier = p_verbose ;                                    // Default records no
// Check Input file directory ends with backslash or forward , if not add it
  if(strchr(p_infdir,'/')) {
      i_len_of_dir = strlen(p_infdir) ;                         // Length of input file directory
      c_flg_slash = p_infdir[i_len_of_dir-1] ;                  // Last character of a String is / forward slash


      if(c_flg_slash != S_K_forward_slash[0]) {
        p_infdir = strcat(p_infdir,S_K_forward_slash) ;
      }
  }

  if(strchr(p_infdir,'\\')) {
      i_len_of_dir = strlen(p_infdir) ;                         // Length of output file directory
      c_flg_slash  = p_infdir[i_len_of_dir-1] ;                 // Last character of a String is / backslash

      if(c_flg_slash != S_K_back_slash[0]) {
        p_infdir = strcat(p_infdir,S_K_back_slash);
      }
  }

// Check Output file directory ends with backslash or forward , if not add it
  if(strchr(p_outfdir,'/')) {
      i_len_of_dir = strlen(p_outfdir) ;                        // Length of log file directory
      c_flg_slash = p_outfdir[i_len_of_dir-1] ;                 // Last character of a String is / forward slash
      if( c_flg_slash != S_K_forward_slash[0]) {
        p_outfdir = strcat(p_outfdir,S_K_forward_slash) ;
      }
  }

  if(strchr(p_outfdir,'\\')) {
      i_len_of_dir = strlen(p_outfdir) ;                        // Length of input file directory
      c_flg_slash = p_outfdir[i_len_of_dir-1] ;                 // Last character of a String is / back slash

      if(c_flg_slash != S_K_back_slash[0]) {
        p_outfdir = strcat(p_outfdir,S_K_back_slash);
      }
  }

// Check log file directory ends with backslash or forward , if not add it
  if(strchr(p_logfdir,'/')) {
      i_len_of_dir = strlen(p_logfdir) ;                        // Length of input file directory
      c_flg_slash = p_logfdir[i_len_of_dir-1] ;                 // Last character of a String is / forward slash

      if(c_flg_slash != S_K_forward_slash[0]) {
        p_logfdir = strcat(p_logfdir,S_K_forward_slash) ;
      }
  }

  if(strchr(p_logfdir,'\\')) {
      i_len_of_dir = strlen(p_logfdir) ;                        // Length of input file directory
      c_flg_slash = p_logfdir[i_len_of_dir-1] ;                 // Last character of a String is / back slash

      if(c_flg_slash != S_K_back_slash[0]) {
        p_logfdir = strcat(p_logfdir,S_K_back_slash) ;
      }
  }

  sprintf( a_str_input_file , "%s%s.tag" , p_data_set , p_run_time );
  sprintf( a_str_output_file , "%s%s.oke" , p_data_set , p_run_time );
  sprintf( a_str_log_file , "%s%s_MkeTag_%d-%s-%s-%s-%s-%s.log", p_data_set, p_run_time, i_YYYY, a_str_MM, a_str_DD, a_str_HH24, a_str_MI, a_str_SS );

  sprintf( a_str_dirinput_file , "%s%s" , p_infdir , a_str_input_file );
  sprintf( a_str_diroutput_file , "%s%s" , p_outfdir , a_str_output_file );
  sprintf( a_str_dirlog_file , "%s%s" , p_logfdir , a_str_log_file );

  f_input_fopen_status   = fopen ( a_str_dirinput_file , "r" ) ; // Open and read input file

  // If Input file contains error output and log file will not create.
  if ( ! f_input_fopen_status ) {
    printf ( "Could not open file %s for input.\n" , f_input_fopen_status ) ;    // Error message while opening file
    exit(1) ;
  }
  else {
    f_output_fopen_status  = fopen ( a_str_diroutput_file , "w" ) ;              // Open and write output file
    f_log_fopen_status     = fopen ( a_str_dirlog_file , "w" ) ;                 // Open and write log file
  }

  if ( ! f_output_fopen_status ) {
    printf ( "Could not open file %s for output\n" ,f_output_fopen_status ) ;    // Error message while opening file
    exit(1) ;
  }

  if ( ! f_log_fopen_status ) {
    printf ( "Could not open file %s for error\n" ,f_log_fopen_status  ) ;       // Error message while opening file
    exit(1) ;
  }

  fprintf ( f_log_fopen_status, "------ MkeKey EXECUTION START DATE AND TIME ------\n" ) ;
  fprintf ( f_log_fopen_status, "%d-%s-%s %s:%s:%s \n\n", i_YYYY, a_str_MM, a_str_DD, a_str_HH24, a_str_MI, a_str_SS ) ;

}
/**********************************************************************
 End of subroutine s_getParameter                                     *
**********************************************************************/

int main ( int argc , char *argv[] ) {

  t = clock( ) ;                                                // Start time
  s_getParameter ( argc , argv ) ;                              // Subroutine to get parameter

  fprintf ( f_log_fopen_status, "------ Run Parameters ------" ) ;
  fprintf ( f_log_fopen_status, "\nData set no           : %s", p_data_set ) ;
  fprintf ( f_log_fopen_status, "\nRun time number       : %s", p_run_time ) ;
  fprintf ( f_log_fopen_status, "\nPopulation            : %s", p_population) ;
  fprintf ( f_log_fopen_status, "\nInput File Directory  : %s", p_infdir ) ;
  fprintf ( f_log_fopen_status, "\nOutput File Directory : %s", p_outfdir ) ;
  fprintf ( f_log_fopen_status, "\nLog File Directory    : %s", p_logfdir ) ;
  fprintf ( f_log_fopen_status, "\nVerbose               : %s\n",( p_verbose == 0 ? "NO" : "YES" ) ) ;

  fprintf ( f_log_fopen_status, "\n------ File Names ------" ) ;
  fprintf ( f_log_fopen_status, "\nInput file name       : %s", a_str_input_file ) ;
  fprintf ( f_log_fopen_status, "\nOutput file name      : %s", a_str_output_file ) ;
  fprintf ( f_log_fopen_status, "\nLog file name         : %s\n", a_str_log_file ) ;

  s_mkeKey_open( ) ;                                            // subroutine to open ssa connection

  // Call ssan3_get_keys

  // Read a input file line by line
  while( fgets ( str_current_rec , sizeof ( str_current_rec ) , f_input_fopen_status ) ) {
    ++i_rec_number ;

    // Calculate the length of the current record
    i_cur_rec_len = strlen( str_current_rec ) ;
    if ( i_cur_rec_len > 0 && str_current_rec[i_cur_rec_len-1] == '\n' ) {
      str_current_rec[--i_cur_rec_len] = '\0' ;
    }

    if ( strstr ( str_current_rec ,"*Id*" ) != NULL ) {

      i_record_read++ ;                                         // No of records read

      // Display so many records in so many seconds to execute
      if ( i_rec_number == i_multiplier ) {
       end_time = clock( ) - t ;                                // End time
       d_time_taken = ( ( double )end_time )/CLOCKS_PER_SEC ;   // In seconds
       printf( "\nDisplay %d records in %f seconds to execute \n", i_multiplier , d_time_taken ) ;      // Print time

       i_multiplier = i_multiplier * 2 ;                        // 10 lakh records multiply by 2
      }

      // To find Id
      str_ptr_id       = strstr ( str_current_rec , "*Id*" ) ;  // Search *Id* in current record capture string from *Id* to till end of the string
      i_id_start_pos = ( strlen(str_ptr_id) - i_cur_rec_len ) ; // Starting position of *Id*
      i_pos_afr_id   = ( i_id_start_pos + 4 ) ;                 // Position after *Id*

      for( i_idx = i_pos_afr_id ; i_idx < i_cur_rec_len; i_idx++ ) {
        if( str_current_rec[i_idx] == '*' ) {
            i_asterisk_start_pos = i_idx ;                      // After *Id* first * position
            break ;
        }
      }

      i_frt_ast_pos = ( i_asterisk_start_pos - i_pos_afr_id ) ; // First asterisk position after *Id*

      strncpy( str_ID ,str_current_rec + i_pos_afr_id, i_frt_ast_pos ) ;        // Substring Id
      str_ID[i_frt_ast_pos] = '\0';



      // IF Person Name , Organization Name and Address_Part1 are empty throw an error message
      if ( strstr ( str_current_rec , "Person_Name" ) == NULL &&
           strstr ( str_current_rec , "Organization_Name" ) == NULL &&
           strstr ( str_current_rec , "Address_Part1" ) == NULL ) {
         i_error_record_read ++ ;                               // Error record count
         i_error_record_flds ++ ;                               // Missing Person Name, Organization name, Address Part 1 fields records count
         fprintf ( f_log_fopen_status, "\nRecord no : %d Error Message : %s", i_rec_number ,"Missing Person Name, Organization name, Address Part 1 fields" ) ;
         fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_current_rec ) ;
      }

      // Check Person_Name is inside the current record
      if ( strstr ( str_current_rec , "Person_Name" ) != NULL ) {
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

        // Call s_mkeKey_getKey
        s_mkeKey_getKey( PL , PS , PE , abv_PLim , abv_PStand , abv_PExt ,str_ID ) ;
      }

      // Check Organization_Name is inside the current record
      if ( strstr ( str_current_rec , "Organization_Name" ) != NULL ) {
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

        // Call s_mkeKey_getKey
        s_mkeKey_getKey( OL , OS , OE , abv_OLim , abv_OStand , abv_OExt, str_ID ) ;
      }

      // Check Address_Part1 is inside the current record
      if ( strstr ( str_current_rec , "Address_Part1" ) != NULL ) {
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

        // Call s_mkeKey_getKey
        s_mkeKey_getKey( AL , AS , AE , abv_ALim , abv_AStand , abv_AExt ,str_ID ) ;
      }
    }
    else {
        // If Id field is missing display error message
      i_error_record_read ++ ;                                 // Error record count
      i_error_record_id ++ ;                                   // Missing id

      fprintf ( f_log_fopen_status, "\nRecord no : %d Error Message : %s", i_rec_number ,"Missing Id field" ) ;
      fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_current_rec ) ;
    }
  }

/**********************************************************************
 End of While loop                                                    *
**********************************************************************/

  fprintf ( f_log_fopen_status, "\n--------------Run summary--------------\n" ) ;
  printf ( "\nNo of records in a file :%d\n", i_rec_number ) ;
  fprintf ( f_log_fopen_status, "Tagged Records read              : %d", i_record_read ) ;
  fprintf ( f_log_fopen_status, "\nError records                 : %d", i_error_record_read ) ;
  fprintf ( f_log_fopen_status, "\n  - Missing Id                : %d", i_error_record_read ) ;
  fprintf ( f_log_fopen_status, "\n  - Missing all 3 key fields  : %d\n", i_error_record_read ) ;

  fprintf ( f_log_fopen_status, "\nRecords with Person_Name       : %d", i_pn_records ) ;
  fprintf ( f_log_fopen_status, "\nRecords with Organization_Name : %d", i_on_records ) ;
  fprintf ( f_log_fopen_status, "\nRecords with Address_Part1     : %d\n", i_addp1_records ) ;

  fprintf ( f_log_fopen_status, "\nPerson_Name Limited keys  : %d", i_PM_ky ) ;
  fprintf ( f_log_fopen_status, "\nPerson_Name Standard keys : %d",  i_PS_ky ) ;
  fprintf ( f_log_fopen_status, "\nPerson_Name Extended keys : %d\n",  i_PX_ky ) ;

  fprintf ( f_log_fopen_status, "\nOrganization_Name Limited keys  : %d", i_OM_ky ) ;
  fprintf ( f_log_fopen_status, "\nOrganization_Name Standard keys : %d", i_OS_ky ) ;
  fprintf ( f_log_fopen_status, "\nOrganization_Name Extended keys : %d\n", i_OX_ky ) ;

  fprintf ( f_log_fopen_status, "\nAddress_Part1 Limited keys  : %d", i_AM_ky ) ;
  fprintf ( f_log_fopen_status, "\nAddress_Part1 Standard keys : %d", i_AS_ky ) ;
  fprintf ( f_log_fopen_status, "\nAddress_Part1 Extended keys : %d\n", i_AX_ky ) ;

  fprintf ( f_log_fopen_status, "\nTotal keys written          : %d\n", i_total_kys_written ) ;

  // Addition of all the keys and check it is match with total keys
  i_addition_key = i_PM_ky + i_PS_ky + i_PX_ky + i_OM_ky + i_OS_ky + i_OX_ky + i_AM_ky + i_AS_ky + i_AX_ky ;

  if ( i_addition_key != i_total_kys_written ) {
    fprintf ( f_log_fopen_status, "\nMissmatch in counts\n") ;
  }

  s_mkeKey_close( ) ;                                           // subroutine to close ssa connection

  end_time = clock( ) - t ;                                     // End time
  d_time_taken = ( ( double )end_time )/CLOCKS_PER_SEC ;        // In seconds

  printf( "\nProcessed %d tagged data records in %f seconds to execute \n" , i_rec_number , d_time_taken ) ;    // Print time

  fprintf( f_log_fopen_status , "- %f seconds to execute \n", d_time_taken ) ;    // Print time

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

    4 Format of Output file

    5 Format of log files

    6 Technical
      6.1 Variables used

      6.2 Run Parameters

      6.3 Compile Procedure

      6.4 Execute procedure in different way

      6.5 Execution Start and End date and time

      6.6 Subroutines

          6.6.1 Called by

          6.6.2 Calling

          6.6.3 Subroutine Structure

    7 Include Header

  Make Key

    Generates (Search) Keys from Tagged data for Key Levels
    Limited (M), Standard (S) and Extended (X) for
    Person_Name, Organisation_Name and Address_Part1 as
    found in the Tagged data.

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

  Procedure Name : MkeKey.c

    Creates Key file sssrrrr.oke from tagged file sssrrrr.tag.

  Copyright

    Copyright ( c ) 2017 IdentLogic Systems Private Limited

  Warnings
    If set data number and run number are empty it will throw an error
    like could not open file for input.
    If any one of the parameter is missing it will not create output
    or log file.

  Format of Output file
    Column  1 to 8   : Key
    Column  9        : Key type  - P ( Person_name ), O ( Organisation ), 1 ( Address_Part1 )
    Column 10        : Key level - M ( Limited ), S ( Standard ), X ( Extended )
    Column 11 onward : Id

  Format of log file
    Log file will be created with date and time
    for eg. sssrrrr_MkeTag_YYYY_MM_DD_HH24_MI_SS.log

    Log file name contains below information.

    ------ EXECUTION START DATE AND TIME ------
    YYYY-MM-DD HH24:MI:SS

    Displayed all run parameters which are used:
    Data set no           : data set number starting from 100 to 999
    Run time number       : Run time number starting from 1000 to 9999
    Population            : india
    Input File Directory  : Input File Directory
    Output File Directory : Output File Directory
    Log File Directory    : Log File Directory
    Verbose               : YES/NO

    Displayed all file names:
    Input file name
    Output file name
    Log file name

    Error message:Missing Person Name, Organization name, Address Part 1 fields

    If Person Name, Organization name and Address Part 1 fields is
    missing in the record then error will be display with
    record no with error message and record.

    Error message: Missing Id field

    If Id field is missing in the record then error will be display with
    record no with error message and record.

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

    Ended YYYY-MM-DD HH24:MI:SS - so many seconds to execute

    Terminal output:

     Verbose:
       Assume that there are millions of records in a file.
       This procedure will read first 10 lakhs records and display
       on terminal( command window ): 10 lakhs records read in so many
       seconds again it will read 2000000 records and display time. it will
       multiplier by 2 every time

       Displayed no of records in a file
     Displayed processed so many tagged data records in so many seconds.

  Technical

   Script name      - MkeKey.c
   Package Number   -
   Procedure Number -

   Variables used

   prefix of a variables       Meaning
   ---------------------       --------
   a_str                       Array string
   c_                          Character
   d_                          double
   f_                          File
   i_                          Integer
   l_                          long
   p_                          parameter
   str_                        String
   C_K                         character constant
   S_K                         String contant

   Run Parameters

   PARAMETER                DESCRIPTION                    ABV   VARIABLE
   ---------                -----------                    ---  ---------
   Set data number          Set data number - 100 to 999    d   p_data_set
   Run number               Run number - 1000 to 9999       r   p_run_time
   Population               Country Name: india             p   p_population
   Input File Directory     Input File Directory            i   p_infdir
   Output File Directory    Output File Directory           o   p_outfdir
   Log File Directory       Log File Directory              l   p_logfdir
   Verbose                  Verbose                         v   p_verbose

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
   
   If Month, Date, Hours, Minutes, Seconds are less than 9 prefix 0 will be added to it.

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
       -o E:\ABC\EFG\HIJ\Output -l E:\ABC\EFG\HIJ\Log -v 100000

     6. MkeKey -d 101 -r 1001 -p india -i E:/ABC/EFG/HIJ/Input/

     7. MkeKey -d 101 -r 1001 -p india -v 100000

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
   s_test_dds_open       To open a session to the dds-NAME3 services
   s_test_dds_get_keys   To build keys on names or addresses
   s_test_dds_close      To terminate an open session to dds-NAME3
   s_doExit              Error occurred in subroutines s_test_dds_open, s_test_dds_get_keys, s_test_dds_close
   s_mkeKey_open         s_test_dds_open subroutine called in s_mkeKey_open subroutine
   s_mkeKey_getKey       s_test_dds_get_keys subroutine called in s_mkeKey_getKey subroutine
   s_mkeKey_close        s_test_dds_close subroutine called in s_mkeKey_close subroutine
   s_print_usage         This subroutine is default parameter of getopt in s_getParameter
   s_date_time           Compute current date and time elements YYYY, MM, DD, HH24, MI and SS


   Called by

   Not indicated if only called by Main.

   Subroutine           Called by
   ----------           ---------
   s_test_dds_open      s_mkeKey_open
   s_test_dds_get_keys  s_mkeKey_getKey
   s_test_dds_close     s_mkeKey_close
   s_doExit             s_mkeKey_open, s_mkeKey_getKey, s_mkeKey_close
   s_print_usage        s_getParameter
   s_date_time          s_getParameter, s_mkeKey_close

   Calling

   Subroutine           Calling Subroutine
   ----------           ------------------
   s_mkeKey_open        s_test_dds_open, s_doExit
   s_mkeKey_getKey      s_test_dds_get_keys, s_doExit
   s_mkeKey_close       s_test_dds_close , s_doExit , s_date_time
   s_getParameter       s_print_usage , s_date_time


   Subroutine Structure

   Main
    |
    |----- s_getParameter
    |           |
    |           |----- s_date_time
    |           |
    |           \----- s_print_usage
    |
    |----- s_mkeKey_open
    |           |
    |           |----- s_test_dds_open
    |           |
    |           \----- s_doExit
    |
    |----- s_mkeKey_getKey
    |           |
    |           |----- s_test_dds_get_keys
    |           |
    |           \----- s_doExit
    |
    \----- s_mkeKey_close
                |
                |----- s_date_time
                |
                |----- s_test_dds_close
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

*/
