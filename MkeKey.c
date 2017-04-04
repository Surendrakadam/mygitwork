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

 WARNINGS      : Length of the Input file directory, Output file directory and
                  Log file directory should not exceed 1,000 bytes, with total
                  filepath not exceeding 1011 , 1011 ,1038 resp.  This is due
                  to the length of the Input file name, Output file name and
                  Log file name are 11, 11 and 38 resp.

                  Increase the array size as per your convenience.

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

#define rangeof( arr )  ( sizeof( arr ) / sizeof( ( arr )[ 0 ] ) )              // Size of an array

// Global variables

// dds NAME3 common variables rc, sockh, session_id

long  l_rc         =  0 ;                                       // Indicate success or failure of open / close sesions

long  l_sockh      = -1 ;                                       // Set to -1 as not calling the dds-NAME3 server

// Should be -1 on the ddsn3_open call, if opening  a new session, a valid Session ID or any other call
long  l_session_id = -1 ;

int i_idx          = 0 ;                                        // i_idx for loop variable

// Time variables
int i_YYYY = 0 ;                                                // Year
int i_MM   = 0 ;                                                // Month
int i_DD   = 0 ;                                                // Date
int i_HH24 = 0 ;                                                // Hour-24
int i_MI   = 0 ;                                                // Minute
int i_SS   = 0 ;                                                // Seconds

// Procedure run parameters
int  p_data_set    = 0 ;                                        // Parameter data set number
int  p_run_time    = 0 ;                                        // Parameter run time data
char *p_system_nm  = "" ;                                       // Parameter System name
char *p_population = "" ;                                       // Population name
char *p_infdir     = "" ;                                       // Parameter input file name
char *p_outfdir    = "" ;                                       // Parameter output file directory
char *p_logfdir    = "" ;                                       // Parameter log file directory
int   p_multiplier = 0 ;                                        // Parameter Multiplier

int  i_len_of_dir  = 0 ;                                        // Length of the directory
char c_flg_slash   = 0 ;                                        // check backslash or forward slash

int  i_option = 0 ;                                             // Switch case variable

char str_current_rec [ BUFSIZ ]  = {0} ;                        // Current record of a file
int i_cur_rec_len    = 0 ;                                      // Current record length

// File Open Statuses
FILE  *f_input_fopen_status ;                                   // Input file name
FILE  *f_output_fopen_status ;                                  // Output file name
FILE  *f_log_fopen_status ;                                     // Log file name

char *str_ID        = "" ;                                      // Substring ID from the current record
char *str_ptr_id    = "" ;                                      // Point to the string *ID* in current record till end of the current record

// File Directories
char a_str_input_file[11]  = {0} ;                              // Input file name
char a_str_output_file[11] = {0} ;                              // Output file name
char a_str_log_file[38]    = {0} ;                              // Log file name

char a_str_file_path_input_file[1011]  = {0} ;                  // Input file path - directory with file name
char a_str_file_path_output_file[1011] = {0} ;                  // Output file path - directory with file name
char a_str_file_path_log_file[1038]    = {0} ;                  // Log file path - directory with file name

int i_id_start_pos       = 0 ;                                  // Starting position of *Id*
int i_pos_afr_id         = 0 ;                                  // Position after *ID*
int i_asterisk_start_pos = 0 ;                                  // After *Id* first * position
int i_frt_ast_pos        = 0 ;                                  // First asterisk position after *Id*

clock_t t_start_time  = 0 ;                                     // Clock object
clock_t t_end_time    = 0 ;                                     // End time
double t_time_taken   = 0.0 ;                                   // Time taken
int i_t_time_taken    = 0 ;                                     // Round of float value

int i_record_read       = 0 ;                                   // No of read records counts
int i_error_record_read = 0 ;                                   // No of error records counts
int i_error_record_id   = 0 ;                                   // Missing id error record count
int i_error_record_flds = 0 ;                                   // Error records where Person_Name Organization_Name AddressPart1 fields are missing
int i_pn_records        = 0 ;                                   // No of Person_Name records counts
int i_on_records        = 0 ;                                   // No of Organization records counts
int i_addp1_records     = 0 ;                                   // No of Address Part1 records counts

int i_PM_ky             = 0 ;                                   // Person Name Limited key
int i_PS_ky             = 0 ;                                   // Person Name Standard key
int i_PX_ky             = 0 ;                                   // Person Name Extended key
int i_Person_Name_keys  = 0 ;                                   // All Person_Name keys

int i_OM_ky             = 0 ;                                   // Organization Name Limited key
int i_OS_ky             = 0 ;                                   // Organization Name Standard key
int i_OX_ky             = 0 ;                                   // Organization Name Extended key
int i_Organ_Name_keys   = 0 ;                                   // All Organization_Name keys

int i_AM_ky             = 0 ;                                   // AddressPart1 Limited key
int i_AS_ky             = 0 ;                                   // AddressPart1 Standard key
int i_AX_ky             = 0 ;                                   // AddressPart1 Extended key
int i_Add_Part1_keys    = 0 ;                                   // All Address_Part1 keys

int i_total_kys_written = 0 ;                                   // Total keys written
int i_addition_key      = 0 ;                                   // Addition of all keys
int i_rec_number        = 0 ;                                   // Record counter

char *S_K_forward_slash = "/" ;                                 // Forward Slash
char *S_K_back_slash    = "\\" ;                                // Back Slash

char a_str_MM[2]        = {0} ;                                 // Compute 2 digit Month
char a_str_DD[2]        = {0} ;                                 // Compute 2 digit Date
char a_str_HH24[2]      = {0} ;                                 // Compute 2 digit Hours
char a_str_MI[2]        = {0} ;                                 // Compute 2 digit Minutes
char a_str_SS[2]        = {0} ;                                 // Compute 2 digit Seconds

int i_multiplier          = 0 ;                                 // Multiplier multiplier variable

int t_hour = 0 ;                                                // Hours to execute records
int t_min  = 0 ;                                                // Minutes to execute records
int t_sec  = 0 ;                                                // Seconds to execute records

char a_str_hour[2] = {0} ;                                      // Compute 2 digit Hours to execute records
char a_str_min[2]  = {0} ;                                      // Compute 2 digit Minutes to execute records
char a_str_sec[2]  = {0} ;                                      // Compute 2 digit Seconds to execute records

const float f_add  = 0.5 ;                                      // Add 0.5 to time taken

// Flags 0 or 1
int i_verbose_flg    = 0 ;                                      // Verbose flag
int i_multiplier_flg = 0 ;                                      // Multiplier flag

/**********************************************************************
 Start of subroutines                                                 *
**********************************************************************/

static void s_date_time ( ) {
// Compute current date and time elements YYYY, MM, DD, HH24, MI and SS

  time_t t_current_time ;                                       // Time to get current time
  struct tm * t_timeinfo ;                                      // struct tm pointer
  time ( &t_current_time) ;                                     // call time() to get current date/time
  t_timeinfo = localtime ( &t_current_time);                    // Localtime

  i_YYYY = t_timeinfo->tm_year + 1900 ;                         // Year
  i_MM   = t_timeinfo->tm_mon + 1 ;                             // Month
  i_DD   = t_timeinfo->tm_mday ;                                // Date
  i_HH24 = t_timeinfo->tm_hour ;                                // Hours
  i_MI   = t_timeinfo->tm_min ;                                 // Minutes
  i_SS   = t_timeinfo->tm_sec ;                                 // Seconds

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

void s_print_usage( ) {
// Print help if user enters wrong run parameter
// It displayed with example

  printf
   (
    "MkeKey -d <data_set> -r <run> -s<system-name> -p <population> -i <input_file_directory>"
    " -o <output_file_directory> -l <log_file_directory> -m <Multiplier> -v<verbose>\n\nExample:\n\n"
    "MkeKey -d 101 -r 1001 -s default -p india -i E:/SurendraK/Work/DeDupeProcs/Input/"
    " -o E:/SurendraK/Work/DeDupeProcs/Output/ -l E:/SurendraK/Work/SSAProcs/Log/ -m 10000 -v\n"
   ) ;

}
/**********************************************************************
 End of subroutine s_print_usage                                      *
**********************************************************************/


static void s_getParameter( int argc , char *argv[] ) {
// This subroutine is default parameter of getopt in s_getParameter

  s_date_time();                                                // Call subroutine s_date_time
  while ( ( i_option = getopt ( argc , argv , "d:r:s:p:i:o:l:m:v::" ) ) != -1 ) {
    switch (i_option) {
      case 'd' :                                                // Data set parameter
        p_data_set   = atoi( optarg ) ;
        break ;
      case 'r' :                                                // Run time parameter
        p_run_time   = atoi( optarg ) ;
        break ;
      case 's' :                                                // System name parameter
        p_system_nm  = optarg ;
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
      case 'm' :                                                // Multiplier
        p_multiplier     = atoi( optarg ) ;
        i_multiplier_flg = 1 ;                                  // Multiplier flag on
        break ;
      case 'v' :                                                // Verbose
        i_verbose_flg = 1 ;                                     // Verbose flag on
        break ;
      default:
        s_print_usage ( ) ;                                     // Default parameter
        exit ( 1 ) ;
    }
  }

  // Data set number should be in a range of 100 to 999
  if ( p_data_set > 999 || p_data_set < 100 ) {
    printf ("%s","JOB ABANDONDED - Data set number should be integer and in a range of 100 to 999" ) ;
    exit(1) ;
  }

  // Run time number should be in a range of 1000 to 9999
  if ( p_run_time > 9999 || p_run_time < 1000 ) {
    printf ("%s","JOB ABANDONDED - Run time number should be integer and in a range of 1000 to 9999" ) ;
    exit(1) ;
  }

  if ( !*p_system_nm ) {                                        // Abort if System name is empty
    printf ("%s","JOB ABANDONDED - Missing system name\n" ) ;
  }

  if ( !*p_population ) {                                       // Abort if Population is mepty
    printf ("%s","JOB ABANDONDED - Missing population\n" ) ;
  }

  i_multiplier = p_multiplier ;                                 // Multiplier value

  // Check Input file directory ends with backslash or forward slash , if not add it
  if(strchr(p_infdir,'/')) {
    i_len_of_dir = strlen(p_infdir) ;                           // Length of input file directory
    c_flg_slash = p_infdir[i_len_of_dir-1] ;                    // Last character of a String is / forward slash
    if(c_flg_slash != S_K_forward_slash[0]) {
      p_infdir = strcat(p_infdir,S_K_forward_slash) ;           // Concatenate forward slash to the input file directory
    }
  }

  if(strchr(p_infdir,'\\')) {
    i_len_of_dir = strlen(p_infdir) ;                           // Length of output file directory
    c_flg_slash  = p_infdir[i_len_of_dir-1] ;                   // Last character of a String is / backslash
    if(c_flg_slash != S_K_back_slash[0]) {
      p_infdir = strcat(p_infdir,S_K_back_slash);               // Concatenate back slash to the input file directory
    }
  }

  // Check Output file directory ends with backslash or forward slash , if not add it
  if(strchr(p_outfdir,'/')) {
    i_len_of_dir = strlen(p_outfdir) ;                          // Length of log file directory
    c_flg_slash = p_outfdir[i_len_of_dir-1] ;                   // Last character of a String is / forward slash
    if( c_flg_slash != S_K_forward_slash[0]) {
      p_outfdir = strcat(p_outfdir,S_K_forward_slash) ;         // Concatenate forward slash to the output file directory
    }
  }

  if(strchr(p_outfdir,'\\')) {
    i_len_of_dir = strlen(p_outfdir) ;                          // Length of input file directory
    c_flg_slash = p_outfdir[i_len_of_dir-1] ;                   // Last character of a String is / back slash
    if(c_flg_slash != S_K_back_slash[0]) {
      p_outfdir = strcat(p_outfdir,S_K_back_slash);             // Concatenate back slash to the output file directory
    }
  }

  // Check log file directory ends with backslash or forward slash , if not add it
  if(strchr(p_logfdir,'/')) {
    i_len_of_dir = strlen(p_logfdir) ;                          // Length of input file directory
    c_flg_slash = p_logfdir[i_len_of_dir-1] ;                   // Last character of a String is / forward slash
    if(c_flg_slash != S_K_forward_slash[0]) {
      p_logfdir = strcat(p_logfdir,S_K_forward_slash) ;         // Concatenate forward slash to the log file directory
    }
  }

  if(strchr(p_logfdir,'\\')) {
    i_len_of_dir = strlen(p_logfdir) ;                          // Length of input file directory
    c_flg_slash = p_logfdir[i_len_of_dir-1] ;                   // Last character of a String is / back slash
    if(c_flg_slash != S_K_back_slash[0]) {
      p_logfdir = strcat(p_logfdir,S_K_back_slash) ;            // Concatenate back slash to the log file directory
    }
  }

  // Files Names
  sprintf( a_str_input_file , "%d%d.tag" , p_data_set , p_run_time );
  sprintf( a_str_output_file , "%d%d.oke" , p_data_set , p_run_time );
  sprintf( a_str_log_file , "%d%d_MkeKey_%d-%s-%s-%s-%s-%s.log", p_data_set, p_run_time, i_YYYY, a_str_MM, a_str_DD, a_str_HH24, a_str_MI, a_str_SS );

  // Directories
  sprintf( a_str_file_path_input_file , "%s%s" , p_infdir , a_str_input_file );
  sprintf( a_str_file_path_output_file , "%s%s" , p_outfdir , a_str_output_file );
  sprintf( a_str_file_path_log_file , "%s%s" , p_logfdir , a_str_log_file );

  f_input_fopen_status   = fopen ( a_str_file_path_input_file , "r" ) ;         // Open and read input file

  // If Input file contains error output and log file will not create.
  if ( ! f_input_fopen_status ) {
    printf ( "Could not open file %s for input.\n" , f_input_fopen_status ) ;   // Error message while opening file
    exit(1) ;
  }
  else {
    f_output_fopen_status  = fopen ( a_str_file_path_output_file , "w" ) ;      // Open and write output file
    f_log_fopen_status     = fopen ( a_str_file_path_log_file , "w" ) ;         // Open and write log file
  }

  if ( ! f_output_fopen_status ) {
    printf ( "Could not open file %s for output\n" ,f_output_fopen_status ) ;   // Error message while opening file
    exit(1) ;
  }

  if ( ! f_log_fopen_status ) {
    printf ( "Could not open file %s for error\n" ,f_log_fopen_status  ) ;      // Error message while opening file
    exit(1) ;
  }

  fprintf ( f_log_fopen_status, "------ MkeKey EXECUTION START DATE AND TIME ------\n" ) ;
  fprintf ( f_log_fopen_status, "%d-%s-%s %s:%s:%s \n\n", i_YYYY, a_str_MM, a_str_DD, a_str_HH24, a_str_MI, a_str_SS ) ;

}
/**********************************************************************
 End of subroutine s_getParameter                                     *
**********************************************************************/

static long s_test_dds_open (
// To open a session to the dds-name3 service

  long    l_sockh ,                                             // Set to -1
  long    *l_session_id ,                                       // Should be -1 on the ddsn3 open call ,or opening a new session
  char    *str_sysName ,                                        // Defines location of the population rules
  char    *str_population ,                                     // Country name
  char    *str_controls                                         // Controls

 ) {

  long    l_rc ;                                                // Indicate success or failure of open / close sesions
  char    a_str_rsp_code[SSA_SI_RSP_SZ] ;                       // Indicates the success or failure of a call to dds-name3
  char    a_str_dds_msg[SSA_SI_SSA_MSG_SZ] ;                    // Error Message

  fprintf ( f_log_fopen_status, "Session Id       : %ld\n" , *l_session_id ) ;

  /* ssan3_open
     This function open and initiates an dds-name3 session in preparation for using further API functions.
     It can also be used to set or override the SSAPR and TIMEOUT environment variables
  */
  l_rc =
    ssan3_open (
      l_sockh ,                                                 // Set to -1 as not calling the dds-NAME3 server
      l_session_id ,                                            // Should be -1 on the ssan3 open call ,or opening a new session
      str_sysName ,                                             // Defines location of the population rules
      str_population ,                                          // Country name
      str_controls ,                                            // Controls
      a_str_rsp_code ,                                          // Indicates the success or failure of a call to dds-name3
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
    fprintf ( f_log_fopen_status, "Error-Message    : %s\n" , a_str_dds_msg ) ;
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

static long s_test_dds_get_keys (
// To build keys on names or addresses

  long    l_sockh ,                                             // Set to -1 as not calling the dds-NAME3 server
  long    *l_session_id ,                                       // Should be -1 on the ssan3 open call ,or opening a new session
  char    *str_sysName ,                                        // Defines location of the population rules
  char    *str_population ,                                     // Country name
  char    *str_controls ,                                       // Controls
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
    ssan3_get_keys_encoded
    (
      l_sockh ,                                                 // Set to -1 as not calling the dds-name3 server
      l_session_id ,                                            // Should be -1 on the ssan3 open call ,or opening a new session
      str_sysName ,                                             // Defines location of the population rules
      str_population ,                                          // Country name
      str_controls ,                                            // Controls
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

  for ( i = 0 ; i < l_num ; ++i ) {

    if ( strcmp ( str_ky_fld_ky_lvl , "PM") == 0 ) {            // Compare key field and key level with PM
      i_PM_ky++ ;                                               // Person_Name Limited keys count
      i_Person_Name_keys ++ ;                                   // Person_Name keys count
      i_total_kys_written++ ;                                   // Total keys written
    }
    else if ( strcmp ( str_ky_fld_ky_lvl , "PS") == 0 ) {       // Compare key field and key level with PS
      i_PS_ky++ ;                                               // Person_Name Standard keys count
      i_Person_Name_keys ++ ;                                   // Person_Name keys count
      i_total_kys_written++ ;                                   // Total keys written
    }
    else if ( strcmp ( str_ky_fld_ky_lvl , "PX") == 0 ) {       // Compare key field and key level with PX
      i_PX_ky++ ;                                               // Person_Name Extended keys count
      i_Person_Name_keys ++ ;                                   // Person_Name keys count
      i_total_kys_written++ ;                                   // Total keys written
    }
    else if ( strcmp ( str_ky_fld_ky_lvl , "OM") == 0 ) {       // Compare key field and key level with OM
      i_OM_ky++ ;                                               // Organization_Name Limited keys count
      i_Organ_Name_keys ++ ;                                    // Organzation keys count
      i_total_kys_written++ ;                                   // Total keys written
    }
    else if ( strcmp ( str_ky_fld_ky_lvl , "OS") == 0 ) {       // Compare key field and key level with OS
      i_OS_ky++ ;                                               // Organization_Name Standard keys count
      i_Organ_Name_keys ++ ;                                    // Organzation keys count
      i_total_kys_written++ ;                                   // Total keys written
    }
    else if ( strcmp ( str_ky_fld_ky_lvl , "OX") == 0 ) {       // Compare key field and key level with OX
      i_OX_ky++ ;                                               // Organization_Name Extended keys count
      i_Organ_Name_keys ++ ;                                    // Organzation keys count
      i_total_kys_written++ ;                                   // Total keys written
    }
    else if ( strcmp ( str_ky_fld_ky_lvl , "1M") == 0 ) {       // Compare key field and key level with 1M
      i_AM_ky++ ;                                               // Address_Part1 Limited keys count
      i_Add_Part1_keys ++ ;                                     // Address Part1 keys count
      i_total_kys_written++ ;                                   // Total keys written
    }
    else if ( strcmp ( str_ky_fld_ky_lvl , "1S") == 0 ) {       // Compare key field and key level with 1S
      i_AS_ky++ ;                                               // Address_Part1 Standard keys count
      i_Add_Part1_keys ++ ;                                     // Address Part1 keys count
      i_total_kys_written++ ;                                   // Total keys written
    }
    else if ( strcmp ( str_ky_fld_ky_lvl , "1X") == 0 ) {       // Compare key field and key level with 1X
      i_AX_ky++ ;                                               // Address_Part1 Extended keys count
      i_Add_Part1_keys ++ ;                                     // Address Part1 keys count
      i_total_kys_written++ ;                                   // Total keys written
    }

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

static long s_test_dds_close (
// To close an open session to dds-name3

  long    l_sockh ,                                             // Set to -1 as not calling the dds-name3 server
  long    *l_session_id ,                                       // Should be -1 on the ssan3 open call ,or opening a new session
  char    *str_sysName ,                                        // Defines location of the population rules
  char    *str_population ,                                     // Country name
  char    *str_controls                                         // Controls
 ) {

  long    l_rc ;                                                // Indicate success or failure of open / close sesions
  char    a_str_rsp_code[SSA_SI_RSP_SZ] ;                       // Indicates the success or failure of a call to ssa-name3
  char    a_str_dds_msg[SSA_SI_SSA_MSG_SZ] ;                    // Error Message

  s_date_time();                                                // Call date and time subroutine

  /* ssan3_close
     Close the dds-name3 session and releases memory.
     This session is then available for reuse.
  */
  l_rc = ssan3_close (
    l_sockh ,                                                   // Set to -1 as not calling the dds-name3 server
    l_session_id ,                                              // Should be -1 on the ssan3 open call ,or opening a new session
    str_sysName ,                                               // Defines location of the population rules
    str_population ,                                            // Country name
    str_controls ,                                              // Controls
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

  fprintf ( f_log_fopen_status, "\nEnded %d-%s-%s %s:%s:%s ", i_YYYY, a_str_MM, a_str_DD, a_str_HH24, a_str_MI, a_str_SS ) ;
  l_rc = 0 ;
  goto SUB_RETURN ;

SUB_RETURN:
  return ( l_rc ) ;

}
/**********************************************************************
 End of subroutine s_test_dds_close                                   *
**********************************************************************/

static void s_doExit ( char *func ) {
/* Error occurred in subroutines s_test_dds_open, s_test_dds_get_keys or
  s_test_dds_close
*/

  fprintf ( f_log_fopen_status , "Error occurred in '%s'\n" , func ) ;
  printf ( "Error occurred in '%s'\n" , func ) ;
  exit ( 1 ) ;

}
/**********************************************************************
 End of subroutine s_doExit                                           *
**********************************************************************/

static void s_mkeKey_open ( ) {
/* s_test_dds_open subroutine called in s_mkeKey_open subroutine and
   assign subroutine parameters
*/

  l_rc =
  s_test_dds_open (
    l_sockh ,                                                 // Set to -1 as not calling the dds-name3 server
    &l_session_id ,                                           // Should be -1 on the ddsn3 open call ,or opening a new session
    p_system_nm ,                                             // Defines location of the population rules
    p_population ,                                            // Population name of country
    ""                                                        // Controls
  ) ;

  if ( 0 != l_rc )
    s_doExit ( "s_test_dds_open" ) ;

}
/**********************************************************************
 End of subroutine s_mkeKey_open                                      *
**********************************************************************/

static void s_mkeKey_getKey (
/* s_test_dds_get_keys subroutine called in s_mkeKey_getKey subroutine
   This subroutine taken three key levels in an array
   and format of key field and key level in an array
*/

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
        l_sockh ,                                               // Set to -1 as not calling the dds-NAME3 server
        &l_session_id ,                                         // Should be -1 on the ddsn3 open call ,or opening a new session
        p_system_nm ,                                           // System name
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

static void s_mkeKey_close ( ) {
/* s_test_dds_close subroutine called in s_mkeKey_close subroutine
   and assign subroutine parameter
*/

  l_rc =
    s_test_dds_close (
      l_sockh ,                                                 // Set to -1 as not calling the dds-name3 server
      &l_session_id ,                                           // Should be -1 on the ddsn3 open call ,or opening a new session
      p_system_nm ,                                             // System name
      p_population ,                                            // Country name
      ""                                                        // Controls
  ) ;

  if ( 0 != l_rc )
    s_doExit ( "s_test_dds_close" ) ;

}
/**********************************************************************
 End of subroutine s_mkeKey_close                                     *
**********************************************************************/

int main ( int argc , char *argv[] ) {
// Main function

  t_start_time = clock( ) ;                                     // Start time
  s_getParameter ( argc , argv ) ;                              // Subroutine to get parameter

  fprintf ( f_log_fopen_status, "------ Run Parameters ------" ) ;
  if ( p_data_set != 0 ) {                                      // If data set number is not empty
    fprintf ( f_log_fopen_status, "\nData set number       : %d", p_data_set ) ;
  }

  if ( p_run_time != 0 ) {                                      // If run time number is not empty
    fprintf ( f_log_fopen_status, "\nRun time number       : %d", p_run_time ) ;
  }

  if ( *p_population ) {                                        // If Population is non empty
    fprintf ( f_log_fopen_status, "\nPopulation            : %s", p_population ) ;
  }

  if ( *p_system_nm ) {                                        // If System name is non empty
    fprintf ( f_log_fopen_status, "\nSystem name           : %s", p_system_nm ) ;
  }

  if ( *p_infdir ) {                                            // If Input file directory is non empty
    fprintf ( f_log_fopen_status, "\nInput File Directory  : %s", p_infdir ) ;
  }

  if ( *p_outfdir ) {                                           // If Output file directory is non empty
    fprintf ( f_log_fopen_status, "\nOutput File Directory : %s", p_outfdir ) ;
  }

  if ( *p_logfdir ) {                                           // If Log file directory is non empty
    fprintf ( f_log_fopen_status, "\nLog File Directory    : %s", p_logfdir ) ;
  }

  fprintf ( f_log_fopen_status, "\nVerbose               : %s\n", ( i_verbose_flg == 1 ? "Yes" : "No" ) ) ;

  fprintf ( f_log_fopen_status, "\n------ File Names ------" ) ;
  fprintf ( f_log_fopen_status, "\nInput file name       : %s", a_str_input_file ) ;
  fprintf ( f_log_fopen_status, "\nOutput file name      : %s", a_str_output_file ) ;
  fprintf ( f_log_fopen_status, "\nLog file name         : %s\n\n", a_str_log_file ) ;

  s_mkeKey_open( ) ;                                            // subroutine to open dds connection

  // Read a input file line by line
  while( fgets ( str_current_rec , sizeof ( str_current_rec ) , f_input_fopen_status ) ) {
    ++i_rec_number ;

    // Calculate the length of the current record
    i_cur_rec_len = strlen( str_current_rec ) ;                 // Length of the current record
    if ( i_cur_rec_len > 0 && str_current_rec[i_cur_rec_len-1] == '\n' ) {
      str_current_rec[--i_cur_rec_len] = '\0' ;
    }

    i_record_read++ ;                                           // No of records read

    if ( strstr ( str_current_rec ,"*Id*" ) != NULL ) {         // Check current record contain *Id*

      if ( i_verbose_flg == 1 ) {                               // If Verbose flag is On

        // Display so many records in so many seconds to execute
        if ( i_multiplier_flg == 0 ) {
           i_multiplier = 100000 ;                              // Default multiplier number
        }
        if ( i_rec_number == i_multiplier ) {                   // If Records number equals Multiplier number
          t_end_time = clock( ) - t_start_time ;                // End time
          t_time_taken = ( ( double )t_end_time )/CLOCKS_PER_SEC ;              // In seconds
          printf( "\nDisplay %d records in %.f seconds to execute \n", i_multiplier , t_time_taken ) ;      // Print time

          i_multiplier = i_multiplier * 2 ;                     // Multiplier value multiply by 2
        }
      }

      // To find Id
      str_ptr_id     = strstr ( str_current_rec , "*Id*" ) ;    // Search *Id* in current record capture string from *Id* to till end of the string
      i_id_start_pos = ( strlen(str_ptr_id) - i_cur_rec_len ) ; // Starting position of *Id*
      i_pos_afr_id   = ( i_id_start_pos + 4 ) ;                 // Position after *Id*

      for( i_idx = i_pos_afr_id ; i_idx < i_cur_rec_len; i_idx++ ) {
        if( str_current_rec[i_idx] == '*' ) {
          i_asterisk_start_pos = i_idx ;                        // After *Id* first * position
          break ;
        }
      }

      i_frt_ast_pos = ( i_asterisk_start_pos - i_pos_afr_id ) ; // First asterisk position after *Id*

      strncpy( str_ID ,str_current_rec + i_pos_afr_id, i_frt_ast_pos ) ;        // Substring Id
      str_ID[i_frt_ast_pos] = '\0';                             // Add \0 to ID end position

      // IF Person Name , Organization Name and Address_Part1 are empty throw an error message
      if ( strstr ( str_current_rec , "Person_Name" ) == NULL &&
           strstr ( str_current_rec , "Organization_Name" ) == NULL &&
           strstr ( str_current_rec , "Address_Part1" ) == NULL ) {
        i_error_record_read ++ ;                                // Error record count
        i_error_record_flds ++ ;                                // Missing Person Name, Organization name, Address Part 1 fields records count
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

        // Call s_mkeKey_getKey
        s_mkeKey_getKey( AL , AS , AE , abv_ALim , abv_AStand , abv_AExt ,str_ID ) ;
      }
    }
    else {
      // If Id field is missing display error message
      i_error_record_read ++ ;                                  // Error record count
      i_error_record_id ++ ;                                    // Missing id

      fprintf ( f_log_fopen_status, "\nRecord no : %d Error Message : %s", i_rec_number ,"Missing Id field" ) ;
      fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_current_rec ) ;
    }

  }
/**********************************************************************
 End of While loop                                                    *
**********************************************************************/

  fprintf ( f_log_fopen_status, "\n------Run summary------\n" ) ;
  fprintf ( f_log_fopen_status, "Tagged Records read            : %d", i_record_read ) ;

  if ( i_error_record_read != 0 ) {                             // If error records count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nError records                  : %d", i_error_record_read ) ;
    printf ("Error records : %d", i_error_record_read ) ;
  }

  if ( i_error_record_id != 0 ) {                               // If id error records count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n - Missing Id                  : %d", i_error_record_id ) ;
  }

  if ( i_error_record_flds != 0 ) {                             // If fields error records count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n - Missing all 3 key fields    : %d\n", i_error_record_flds ) ;
  }

  if ( i_pn_records != 0 ) {                                    // If Records with Person_Name count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nRecords with Person_Name       : %d", i_pn_records ) ;
  }

  if ( i_on_records != 0 ) {                                    // If Records with Organization_Name count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nRecords with Organization_Name : %d", i_on_records ) ;
  }

  if ( i_addp1_records != 0 ) {                                 // If Records with Address_Part1 count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nRecords with Address_Part1     : %d\n", i_addp1_records ) ;
  }

  if ( i_Person_Name_keys != 0 ) {                              // If Person_Name keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nPerson_Name keys       : %d", i_Person_Name_keys ) ;
  }

  if ( i_PM_ky != 0 ) {                                         // If Person_Name Limited keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Limited  : %d", i_PM_ky ) ;
  }

  if ( i_PS_ky != 0 ) {                                         // If Person_Name Standard keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Standard : %d",  i_PS_ky ) ;
  }

  if ( i_PX_ky != 0 ) {                                         // If Person_Name Extended keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Extended : %d\n",  i_PX_ky ) ;
  }

  if ( i_Organ_Name_keys != 0 ) {                               // If Organization_Name keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nOrganization_Name keys : %d", i_Organ_Name_keys ) ;
  }

  if ( i_OM_ky != 0 ) {                                         // If Organization_Name Limited keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Limited  : %d", i_OM_ky ) ;
  }

  if ( i_OS_ky != 0 ) {                                         // If Organization_Name Standard keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Standard : %d", i_OS_ky ) ;
  }

  if ( i_OX_ky != 0 ) {                                         // If Organization_Name Extended keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Extended : %d\n", i_OX_ky ) ;
  }

  if ( i_Add_Part1_keys != 0 ) {                               // If Address_Part1 keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nAddress_Part1 keys     : %d", i_Add_Part1_keys ) ;
  }

  if ( i_AM_ky != 0 ) {                                        // If Address_Part1 Limited keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Limited  : %d", i_AM_ky ) ;
  }

  if ( i_AS_ky != 0 ) {                                        // If Address_Part1 Standard keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Standard : %d", i_AS_ky ) ;
  }

  if ( i_AX_ky != 0 ) {                                        // If Address_Part1 Extended keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Extended : %d\n", i_AX_ky ) ;
  }

  if ( i_total_kys_written != 0 ) {                            // If Total keys written count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nTotal keys written     : %d\n", i_total_kys_written ) ;
  }

  // Addition of all the keys
  i_addition_key = i_PM_ky + i_PS_ky + i_PX_ky + i_OM_ky + i_OS_ky + i_OX_ky + i_AM_ky + i_AS_ky + i_AX_ky ;

  if ( i_addition_key != i_total_kys_written ) {                // Addition of all the keys and check it is match with total keys
    fprintf ( f_log_fopen_status, "\nMissmatch in counts\n") ;
  }

  s_mkeKey_close( ) ;                                           // subroutine to close dds connection

  t_end_time = clock( ) - t_start_time ;                        // End time
  t_time_taken = ( ( double )t_end_time )/CLOCKS_PER_SEC ;      // In seconds

  t_time_taken = t_time_taken + f_add ;
  i_t_time_taken = (int)t_time_taken ;

  t_hour = i_t_time_taken / 3600 ;                              // Convert seconds to hours
  t_min  = ( i_t_time_taken % 3600 ) / 60 ;                     // Convert seconds to minutes
  t_sec  = (i_t_time_taken % 60 ) % 60 ;                        // Seconds

  if( t_hour < 10 ) {                                           // Ensure hours is a 2 digit string
    sprintf( a_str_hour , "0%d" ,t_hour ) ;                     // Add prefix 0 if hours less than 10
  }
  else {
    sprintf( a_str_hour , "%d" ,t_hour ) ;                      // Keep it as it is
  }

  if( t_min < 10 ) {                                            // Ensure minutes is a 2 digit string
    sprintf( a_str_min , "0%d" ,t_min ) ;                       // Add prefix 0 if minutes less than 10
  }
  else {
    sprintf( a_str_min , "%d" ,t_min ) ;                        // Keep it as it is
  }

  if( t_sec < 10 ) {                                            // Ensure seconds is a 2 digit string
    sprintf( a_str_sec , "0%d" ,t_sec ) ;                       // Add prefix 0 if seconds less than 10
  }
  else {
    sprintf( a_str_sec , "%d" ,t_sec ) ;                        // Keep it as it is
  }

  printf( "\nProcessed %d tagged data records in %s:%s:%s to execute \n" , i_rec_number , a_str_hour , a_str_min , a_str_sec ) ;    // Print time

  fprintf( f_log_fopen_status , "- %s:%s:%s to execute \n", a_str_hour , a_str_min , a_str_sec ) ;  // Print time
  fprintf( f_log_fopen_status , "\n===============================================\n") ;

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

  KEY_LEVEL                     KEY_FIELD
  ---------                     ---------
  M|Limited                     P|Person_Name
  S|Standard                    O|Organization_Name
  X|Extended                    1|Address_Part1

Procedure Name : MkeKey.c

  Creates Key file sssrrrr.oke from tagged file sssrrrr.tag.

Copyright

  Copyright ( c ) 2017 IdentLogic Systems Private Limited

Warnings

  Length of the Input file directory, Output file directory and
  Log file directory should not exceed 1,000 bytes, with total
  filepath not exceeding 1011 , 1011 ,1038 resp.  This is due
  to the length of the Input file name, Output file name and
  Log file name are 11, 11 and 38 resp.

  Increase the array size as per your convenience.

  Data set number , Run time number ,System name and population parameters
  are mandatory.
  Data set number should be in a range of 100 to 999
  Run time number should be in a range of 1000 to 9999
  Means Input file name should be integer and in range.
  System name should not be empty
  Population name should not be empty
  If data set number and run number are empty it will throw an error
  If data set number and run number are missing or not found it will not create output
  or log file.
  System name is the location( default folder) where india.ysp file is located.
  Population means country name

Format of Output file

  Column  1 to 8   : Key
  Column  9        : KEY_FIELDS  - P ( Person_name ), O ( Organisation ),
                      1 ( Address_Part1 )
  Column 10        : KEY_LEVEL   - M ( Limited ), S ( Standard ), X ( Extended )
  Column 11 onward : Id

Format of log file

  Log file will be created with data set number, run number ,procedure name
   and date time
  for eg. sssrrrr_MkeRng_YYYY_MM_DD_HH24_MI_SS.log

  Log file display only not null or non empty information.
  Those fields values are empty or not null it will not display in log file

  Log file name contain below information.

  ------ MkeRng EXECUTION START DATE AND TIME ------
  YYYY-MM-DD HH24:MI:SS

  ------ Run Parameters ------
  Data set no           : data set number starting from 100 to 999
  Run time number       : Run time number starting from 1000 to 9999
  Population            : india
  Input File Directory  : Input File path
  Output File Directory : Output File path
  Log File Directory    : Log File path
  Verbose               : Yes/No

  ------ File Names ------
  Input file name       : <Input file name>
  Output file name      : <Output file name>
  Log file name         : <Log file name>

  Error message:Missing Person Name, Organization name, Address Part 1 fields

  If Person Name, Organization name and Address Part 1 fields is
  missing in the record then error will be display with
  record no with error message and record.

  Error message: Missing Id field

  If Id field is missing in the record then error will be display with
  record no with error message and record.

  ------Run summary------
  Tagged Records read            : <Count>
  Error records                  : <Count>
   - Missing Id                  : <Count>
   - Missing all 3 key fields    : <Count>
  Records with Person_Name       : <Count>
  Records with Organization_Name : <Count>
  Records with Address_Part1     : <Count>

  Person_Name keys       : <Count>
   -Limited  : <Count>
   -Standard : <Count>
   -Extended : <Count>

  Organization_Name keys : <Count>
   -Limited  : <Count>
   -Standard : <Count>
   -Extended : <Count>

  Address_Part1 keys     : <Count>
   -Limited  : <Count>
   -Standard : <Count>
   -Extended : <Count>

  Total keys written     : <Count>

  Ended YYYY-MM-DD HH24:MI:SS - HH:MM:SS to execute

  =======================================================================

  Note: 
   Addition of all the keys and check it is match with total keys
   If Hours , Minutes and seconds are less than 10 prefix 0 will be added.

  Terminal output:
  
  No of error record will e display if it is not zero.
  
  Verbose :

    Assume that there are millions of records in a file.
    Multiplier parameter show you so many records taken to execute
    so many seconds after a particular Multiplier number.
   
   Multiplier:
     If Multiplier parameter is null it will take default multiplier value
     i.e 1 lakh
     Set multiplier value with Multiplier parameter and to display use
     verbose parameter
     for eg : ... -v -m 100000
              ... -m 100000 -v

     On terminal( command window ) output : 
     Display so many records in  so many seconds
     Display so many records in  so many seconds
     .
     .
     .
     Display so many records in  so many seconds
     Display so many records in  so many seconds
     Display so many records in  so many seconds
     
     multiply by 2 to muliplier number every time until the end of the file.

     Processed so many tagged data records in YYYY:MM:SS to execute.

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
 t_                          Time related variables
 _flg                        Flag 0 or 1

 Run Parameters

 PARAMETER                DESCRIPTION                               ABV   VARIABLE
 ---------                -----------                               ---  ---------
 Set data number          Set data number - 100 to 999               d   p_data_set
 Run number               Run number - 1000 to 9999                  r   p_run_time
 System name              Defines location of the population rules   s   p_system_nm
 Population               Country Name: india                        p   p_population
 Input File Directory     Input File Directory                       i   p_infdir
 Output File Directory    Output File Directory                      o   p_outfdir
 Log File Directory       Log File Directory                         l   p_logfdir
 Multiplier               Multiplier                                 m   p_multiplier
 Verbose                  Display indformation about multiplier      v   i_verbose_flg

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

   -d -r -s -p paramter are mandatory

   1. MkeKey -d 101 -r 1001 -s default -p india

   2. MkeKey -d 101 -r 1001 -s default -p india -i E:/ABC/EFG/HIJ/Input/
     -o E:/ABC/EFG/HIJ/Output/ -l E:/ABC/EFG/HIJ/Log/

   3. MkeKey -d 101 -r 1001 -s default -p india -i E:\ABC\EFG\HIJ\Input\
     -o E:\ABC\EFG\HIJ\Output\ -l E:\ABC\EFG\HIJ\Log\

   4. MkeKey -d 101 -r 1001 -s default -p india -i E:/ABC/EFG/HIJ/Input
     -o E:/ABC/EFG/HIJ/Output -l E:/ABC/EFG/HIJ/Log

   5. MkeKey -d 101 -r 1001 -s default -p india -i E:\ABC\EFG\HIJ\Input
     -o E:\ABC\EFG\HIJ\Output -l E:\ABC\EFG\HIJ\Log -m 100000 -v

   6. MkeKey -d 101 -r 1001 -s default -p india -i E:/ABC/EFG/HIJ/Input/

   7. MkeKey -d 101 -r 1001 -s default -p india -m 100000 -v

 Note :

   If you forget to give backslash(\) or forward slash(/) at the end of the
   Input, Output, and Log file directory path . The procedure will be append
   backslash or forward slash at the end of path as per your directory path.
   If you do not give Input File Directory then procedure will read
   the file from the current directory.
   If you do not give Output File Directory and Log File Directory
   in the run parameter then output file and log file will be created
   in the current directory.
   Output and Log file created in the directory which is given in the
   command prompt.

 Execution Start and End date and time

   In log file, also contain starting and ending execution time.
   Hours, Minutes and Seconds will be display with 2 digits.

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
