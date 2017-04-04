/*
 Procedure     : MkeRng.c
 Application   : De-Dupe
 Client        : Internal
 Copyright (c) : IdentLogic Systems Private Limited
 Author        : Surendra Kadam
 Creation Date : 3 April 2017
 Description   : Generates ( Search ) key range from Tagged data for search levels
                  Narrow (N) , Typical (Y) , Exhaustive (H) , Extreme (R) for
                  Person_Name (P) , Organization_name (O) , Address_Part1 ,
                  Sex_Code (G) as found in Tagged data
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

/* Should be -1 on the ddsn3_open call,if opening  a new session, a valid Session ID or any other call*/
long    l_session_id  = -1 ;
long    l_rc          = 0 ;                                     // Indicate success or failure of open / close sesions
long    l_sockh       = -1 ;                                    // Set to -1 as not calling the dds-name3 server

int  i_option         = 0 ;                                     // Switch case variable
int  i_idx            = 0 ;                                     // i_idx for loop variable

char str_current_rec [ BUFSIZ ]  = {0} ;                        // Current record of a file
int i_cur_rec_len    = 0 ;                                      // Current record length

// Time variables
int i_YYYY = 0 ;                                                // Year
int i_MM   = 0 ;                                                // Month
int i_DD   = 0 ;                                                // Date
int i_HH24 = 0 ;                                                // Hour-24
int i_MI   = 0 ;                                                // Minute
int i_SS   = 0 ;                                                // Seconds

char a_str_MM[2]        = {0} ;                                 // Compute 2 digit Month
char a_str_DD[2]        = {0} ;                                 // Compute 2 digit Date
char a_str_HH24[2]      = {0} ;                                 // Compute 2 digit Hours
char a_str_MI[2]        = {0} ;                                 // Compute 2 digit Minutes
char a_str_SS[2]        = {0} ;                                 // Compute 2 digit Seconds

// Procedure run parameters
int   p_data_set   = 0 ;                                        // Parameter data set number
int   p_run_time   = 0 ;                                        // Parameter run time data
char *p_system_nm  = "" ;                                       // Parameter System name
char *p_population = "" ;                                       // Population name
char *p_infdir     = "" ;                                       // Parameter input file name
char *p_outfdir    = "" ;                                       // Parameter output file directory
char *p_logfdir    = "" ;                                       // Parameter log file directory
int   p_multiplier = 0 ;                                        // Parameter Multiplier
int i_multiplier   = 0 ;                                        // Multiplier multiplier variable

int  i_len_of_dir  = 0 ;                                        // Length of the directory
char c_flg_slash   = 0 ;                                        // check backslash or forward slash
char *S_K_forward_slash = "/" ;                                 // Forward Slash
char *S_K_back_slash    = "\\" ;                                // Back Slash

// File Open Statuses
FILE  *f_input_fopen_status ;                                   // Input file name
FILE  *f_output_fopen_status ;                                  // Output file name
FILE  *f_log_fopen_status ;                                     // Log file name

// File Directories
char a_str_input_file[11]  = {0} ;                              // Input file name
char a_str_output_file[11] = {0} ;                              // Output file name
char a_str_log_file[38]    = {0} ;                              // Log file name

char a_str_file_path_input_file[1011]  = {0} ;                  // Input file path - directory with file name
char a_str_file_path_output_file[1011] = {0} ;                  // Output file path - directory with file name
char a_str_file_path_log_file[1038]    = {0} ;                  // Log file path - directory with file name

// ID related variables
char *str_ID               = "" ;                               // Substring ID from the current record
char *str_ptr_id           = "" ;                               // Point to the string *ID* in current record till end of the current record
int   i_id_start_pos       = 0 ;                                // Starting position of *Id*
int   i_pos_afr_id         = 0 ;                                // Position after *ID*
int   i_asterisk_start_pos = 0 ;                                // After *Id* first * position
int   i_frt_ast_pos        = 0 ;                                // First asterisk position after *Id*

// Clock execution time variables
clock_t t_start_time  = 0 ;                                     // Clock object
clock_t t_end_time    = 0 ;                                     // End time
double t_time_taken   = 0.0 ;                                   // Time taken
int i_t_time_taken    = 0 ;                                     // Round of float value

// Run summary
int i_record_read       = 0 ;                                   // No of read records counts
int i_error_record_read = 0 ;                                   // No of error records counts
int i_error_record_id   = 0 ;                                   // Missing id error record count
int i_error_record_flds = 0 ;                                   // Error records where Person_Name Organization_Name AddressPart1 fields are missing

int i_pn_records        = 0 ;                                   // No of Person_Name records counts
int i_on_records        = 0 ;                                   // No of Organization records counts
int i_addp1_records     = 0 ;                                   // No of Address Part1 records counts
int i_sex_code_records  = 0 ;                                   // No of Sex_Code records counts

int i_PN_ky = 0 ;                                               // Person_Name Narrow keys count
int i_PY_ky = 0 ;                                               // Person_Name Typical keys count
int i_PH_ky = 0 ;                                               // Person_Name Exhaustive keys count
int i_PR_ky = 0 ;                                               // Person_Name Extreme keys count

int i_ON_ky = 0 ;                                               // Organization_Name Narrow keys count
int i_OY_ky = 0 ;                                               // Organization_Name Typical keys count
int i_OH_ky = 0 ;                                               // Organization_Name Exhaustive keys count
int i_OR_ky = 0 ;                                               // Organization_Name Extreme keys count

int i_1N_ky = 0 ;                                               // Address_Part1 Narrow keys count
int i_1Y_ky = 0 ;                                               // Address_Part1 Typical keys count
int i_1H_ky = 0 ;                                               // Address_Part1 Exhaustive keys count
int i_1R_ky = 0 ;                                               // Address_Part1 Extreme keys count

int i_GN_ky = 0 ;                                               // Sex_Code Narrow keys count
int i_GY_ky = 0 ;                                               // Sex_Code Typical keys count
int i_GH_ky = 0 ;                                               // Sex_Code Exhaustive keys count
int i_GR_ky = 0 ;                                               // Sex_Code Extreme keys count

int i_Person_Name_keys  = 0 ;                                   // Person_Name Narrow , Typical , Exhaustive ,Extreme keys count
int i_Organ_Name_keys   = 0 ;                                   // Organization_Name Narrow , Typical , Exhaustive ,Extreme keys count
int i_Add_Part1_keys    = 0 ;                                   // Address_Part1 Narrow , Typical , Exhaustive ,Extreme keys count
int i_sex_code_keys     = 0 ;                                   // Sex_Code Narrow , Typical , Exhaustive ,Extreme keys count

int i_total_kys_written = 0 ;                                   // Total keys written
int i_addition_key      = 0 ;                                   // Addition of all keys
int i_rec_number        = 0 ;                                   // Record counter

// Flags 0 or 1
int i_verbose_flg    = 0 ;                                      // Verbose flag
int i_multiplier_flg = 0 ;                                      // Multiplier flag

const float f_add  = 0.5 ;                                      // Add 0.5 to time taken

int t_hour = 0 ;                                                // Hours to execute records
int t_min  = 0 ;                                                // Minutes to execute records
int t_sec  = 0 ;                                                // Seconds to execute records
char a_str_hour[2] = {0} ;                                      // Compute 2 digit Hours to execute records
char a_str_min[2]  = {0} ;                                      // Compute 2 digit Minutes to execute records
char a_str_sec[2]  = {0} ;                                      // Compute 2 digit Seconds to execute records

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
    "MkeRng -d <data_set> -r <run> -s<system-name> -p <population> -i <input_file_directory>"
    " -o <output_file_directory> -l <log_file_directory> -m <Multiplier> -v<verbose>\n\nExample:\n\n"
    "MkeRng -d 101 -r 1001 -s default -p india -i E:/SurendraK/Work/DeDupeProcs/Input/"
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
  if( p_data_set > 999 || p_data_set < 100 ) {
    printf("%s","JOB ABANDONDED - Data set number should be integer and in a range of 100 to 999" ) ;
    exit(1);
  }

  // Run time number should be in a range of 1000 to 9999
  if( p_run_time > 9999 || p_run_time < 1000 ) {
    printf("%s","JOB ABANDONDED - Run time number should be integer and in a range of 1000 to 9999" ) ;
    exit(1);
  }

  if ( !*p_system_nm ) {                                        // Abort if System name is empty
    printf ("%s","JOB ABANDONDED - Missing system name\n" ) ;
  }

  if ( !*p_population ) {                                       // Abort if Population is mepty
    printf ("%s","JOB ABANDONDED - Missing population\n" ) ;
  }

  i_multiplier = p_multiplier ;                                 // Multiplier value

  // Check Input file directory ends with backslash or forward , if not add it
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

  // Check Output file directory ends with backslash or forward , if not add it
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

  // Check log file directory ends with backslash or forward , if not add it
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
  sprintf( a_str_output_file , "%d%d.ore" , p_data_set , p_run_time );
  sprintf( a_str_log_file , "%d%d_MkeRng_%d-%s-%s-%s-%s-%s.log", p_data_set, p_run_time, i_YYYY, a_str_MM, a_str_DD, a_str_HH24, a_str_MI, a_str_SS );

  // File Path
  sprintf( a_str_file_path_input_file , "%s%s" , p_infdir , a_str_input_file );
  sprintf( a_str_file_path_output_file , "%s%s" , p_outfdir , a_str_output_file );
  sprintf( a_str_file_path_log_file , "%s%s" , p_logfdir , a_str_log_file );

  f_input_fopen_status   = fopen ( a_str_file_path_input_file , "r" ) ;         // Open and read input file

  // If Input file contains error output and log file won't create.
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
  char    *str_sysName ,                                        // Default
  char    *str_population ,                                     // Country name
  char    *str_controls                                         // Controls
) {

  char    a_str_rsp_code[ SSA_SI_RSP_SZ ] ;                     // Indicates the success or failure of a call to dds-name3
  char    a_str_dds_msg[ SSA_SI_SSA_MSG_SZ ] ;                  // Error Message

  fprintf ( f_log_fopen_status , "Session Id   : %ld\n" , *l_session_id ) ;

  l_rc = ssan3_open
  (
    l_sockh ,                                                   // Set to -1 as not calling the dds-name3 server
    l_session_id ,                                              // Should be -1 on the ddsn3 open call ,or opening a new session
    str_sysName ,                                               // Defines location of the population rules
    str_population ,                                            // Country name
    str_controls ,                                              // Controls
    a_str_rsp_code ,                                            // Indicates the success or failure of a call to dds-name3
    SSA_SI_RSP_SZ ,                                             // Size of respose code
    a_str_dds_msg ,                                             // Error Message
    SSA_SI_SSA_MSG_SZ                                           // Size of error message
  ) ;

  if ( l_rc < 0 ) {
    fprintf ( f_log_fopen_status , "rc               : %ld\n" , l_rc ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }

  if ( a_str_rsp_code[ 0 ] != '0' && *l_session_id == -1 ) {
    fprintf ( f_log_fopen_status , "rsp_code         : %s\n" , a_str_rsp_code ) ;
    fprintf ( f_log_fopen_status , "Error Message    : %s\n" , a_str_dds_msg ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }
  //printf ( "dds_msg          : %s\n" , a_str_dds_msg ) ;   --Licence Error
  l_rc = 0 ;
  goto SUB_RETURN ;

SUB_RETURN:
  return ( l_rc ) ;

}
/**********************************************************************
 End of subroutine s_test_dds_open                                    *
**********************************************************************/

static long s_test_dds_get_ranges (
/*
Used to get the dds-name3 key rangfes for a name or address which
the application program will use in a select statement to retreive records
from the dds-name3 key table
*/

  long     l_sockh ,                                            // Set to -1 as not calling the dds-name3 server
  long    *l_session_id ,                                       // Should be -1 on the ddsn3 open call ,or opening a new session
  char    *str_sysName ,                                        // Defines location of the population rules
  char    *str_population ,                                     // Country name
  char    *str_controls ,                                       // Format of Field type and Search level
  char    *str_record ,                                         // Current record of a file
  long    l_recordLength ,                                      // Length of the current record in a file
  char    *str_recordEncType ,                                  // Encrypted type by default is Text
  char    *str_ky_fld_search_lvl,                               // abbrevation of a key field and Search leve;
  char    *str_ID                                               // ID
) {

  int     i ;                                                   // i variable
  long    l_rc ;                                                // Indicate success or failure of open / close sesions
  char    *a_str_ranges_array[ SSA_SI_MAX_STAB ] ;              // Ranges array with size
  char    a_ranges_data[ SSA_SI_MAX_STAB*( 2*SSA_SI_KEY_SZ ) ] ;// Ranges field data
  char    a_str_rsp_code[ SSA_SI_RSP_SZ ] ;                     // Indicates the success or failure of a call to dds-name3
  char    a_str_dds_msg[ SSA_SI_SSA_MSG_SZ ] ;                  // Error Message
  long    l_num ;                                               // Keys count
  char    *str_key ;                                            // Generate Keys

  for ( i = 0 ; i < ( int )rangeof ( a_str_ranges_array ) ; ++i )
    a_str_ranges_array[ i ] = a_ranges_data + i * ( 2*SSA_SI_KEY_SZ ) ;

  l_num = 0 ;
  l_rc = ssan3_get_ranges_encoded
  (
    l_sockh ,                                                   // Set to -1 as not calling the dds-name3 server
    l_session_id ,                                              // Should be -1 on the ddsn3 open call ,or opening a new session
    str_sysName ,                                               // Defines location of the population rules
    str_population ,                                            // Country name
    str_controls ,                                              // Format of Field type and Search level
    a_str_rsp_code ,                                            // Indicates the success or failure of a call to dds-name3
    SSA_SI_RSP_SZ ,                                             // Size of respose code
    a_str_dds_msg ,                                             // Error Message
    SSA_SI_SSA_MSG_SZ ,                                         // Size of error message
    str_record ,                                                // Current record of a file
    l_recordLength ,                                            // Length of the current record in a file
    str_recordEncType ,                                         // Encrypted type by default is Text
    &l_num ,                                                    // Keys count
    a_str_ranges_array ,                                        // Ranges Array
    2*SSA_SI_KEY_SZ                                             // Size of the key multiply by 2
  ) ;

  if ( l_rc < 0 ) {
    fprintf ( f_log_fopen_status , "ERROR: Return Code : %ld\n" , l_rc ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }

  if ( a_str_rsp_code[ 0 ] != '0' ) {
    fprintf ( f_log_fopen_status , "rsp_code         : %s\n" , a_str_rsp_code ) ;
    fprintf ( f_log_fopen_status , "dds_msg          : %s\n" , a_str_dds_msg ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }

  for ( i = 0 ; i < l_num ; ++i ) {
    if ( strcmp ( str_ky_fld_search_lvl , "PN") == 0 ) {        // Compare PN with abbreviation of key field and search level
      i_PN_ky ++ ;                                              // Person_Name Narrow keys count
      i_Person_Name_keys ++ ;                                   // Number of Person_Name keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "PY") == 0 ) {   // Compare PY with abbreviation of key field and search level
      i_PY_ky ++ ;                                              // Person_Name Typical keys count
      i_Person_Name_keys ++ ;                                   // Number of Person_Name keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "PH") == 0 ) {   // Compare PH with abbreviation of key field and search level
      i_PH_ky ++ ;                                              // Person_Name Exhaustive keys count
      i_Person_Name_keys ++ ;                                   // Number of Person_Name keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "PR") == 0 ) {   // Compare PR with abbreviation of key field and search level
      i_PR_ky ++ ;                                              // Person_Name Extreme keys count
      i_Person_Name_keys ++ ;                                   // Number of Person_Name keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "ON") == 0 ) {   // Compare ON with abbreviation of key field and search level
      i_ON_ky ++ ;                                              // Organization_Name Narrow keys count
      i_Organ_Name_keys ++ ;                                    // Number of Organization_Name keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "OY") == 0 ) {   // Compare OY with abbreviation of key field and search level
      i_OY_ky ++ ;                                              // Organization_Name Typical keys count
      i_Organ_Name_keys ++ ;                                    // Number of Organization_Name keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "OH") == 0 ) {   // Compare OH with abbreviation of key field and search level
      i_OH_ky ++ ;                                              // Organization_Name Exhaustive keys count
      i_Organ_Name_keys ++ ;                                    // Number of Organization_Name keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "OR") == 0 ) {   // Compare OR with abbreviation of key field and search level
      i_OR_ky ++ ;                                              // Organization_Name Extreme keys count
      i_Organ_Name_keys ++ ;                                    // Number of Organization_Name keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "1N") == 0 ) {   // Compare 1N with abbreviation of key field and search level
      i_1N_ky ++ ;                                              // Address_Part1 Narrow keys count
      i_Add_Part1_keys ++ ;                                     // Number of Address_Part1 keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "1Y") == 0 ) {   // Compare 1Y with abbreviation of key field and search level
      i_1Y_ky ++ ;                                              // Address_Part1 Typical keys count
      i_Add_Part1_keys ++ ;                                     // Number of Address_Part1 keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "1H") == 0 ) {   // Compare 1H with abbreviation of key field and search level
      i_1H_ky ++ ;                                              // Address_Part1 Exhaustive keys count
      i_Add_Part1_keys ++ ;                                     // Number of Address_Part1 keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "1R") == 0 ) {   // Compare 1R with abbreviation of key field and search level
      i_1R_ky ++ ;                                              // Address_Part1 Extreme keys count
      i_Add_Part1_keys ++ ;                                     // Number of Address_Part1 keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "GN") == 0 ) {   // Compare GN with abbreviation of key field and search level
      i_GN_ky ++ ;                                              // Sex_Code Narrow keys count
      i_sex_code_keys ++ ;                                      // Number of Sex_Code keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "GY") == 0 ) {   // Compare GY with abbreviation of key field and search level
      i_GY_ky ++ ;                                              // Sex_Code Typical keys count
      i_sex_code_keys ++ ;                                      // Number of Sex_Code keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "GH") == 0 ) {   // Compare GH with abbreviation of key field and search level
      i_GH_ky ++ ;                                              // Sex_Code Exhaustive keys count
      i_sex_code_keys ++ ;                                      // Number of Sex_Code keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "GR") == 0 ) {   // Compare GR with abbreviation of key field and search level
      i_GR_ky ++ ;                                              // Sex_Code Extreme keys count
      i_sex_code_keys ++ ;                                      // Number of Sex_Code keys count
      i_total_kys_written ++ ;                                  // Total keys written
    }

    str_key = a_str_ranges_array[ i ] ;
    fprintf ( f_output_fopen_status ,"%.*s%.*s%s%s\n",SSA_SI_KEY_SZ , str_key , SSA_SI_KEY_SZ , str_key + SSA_SI_KEY_SZ , str_ky_fld_search_lvl , str_ID ) ;
  }

  l_rc = 0 ;
  goto SUB_RETURN ;

SUB_RETURN:
  return ( l_rc ) ;

}
/**********************************************************************
 End of subroutine s_test_dds_get_ranges                              *
**********************************************************************/

static long s_test_dds_close (
  long    l_sockh ,                                             // Set to -1 as not calling the SSA-name3 server
  long    *l_session_id ,                                       // Should be -1 on the ddsn3 open call ,or opening a new session
  char    *str_sysName ,                                        // Defines location of the population rules
  char    *str_population ,                                     // Country name
  char    *str_controls                                         // Format fo key field and search level
) {

  long    l_rc ;                                                // Indicate success or failure of open / close sesions
  char    a_str_rsp_code[ SSA_SI_RSP_SZ ] ;                     // Indicates the success or failure of a call to dds-name3
  char    a_str_dds_msg[ SSA_SI_SSA_MSG_SZ ] ;                  // Error Message

  s_date_time();                                                // Call date and time subroutine

  /* ddsn3_close
     Close the SSA-name3 session and releases memory.
     This session is then available for reuse.
  */
  l_rc = ssan3_close
  (
    l_sockh ,                                                   // Set to -1 as not calling the dds-name3 server
    l_session_id ,                                              // Should be -1 on the ddsn3 open call ,or opening a new session
    str_sysName ,                                               // Defines location of the population rules
    str_population ,                                            // Country name
    str_controls ,                                              // Default
    a_str_rsp_code ,                                            // Indicates the success or failure of a call to dds-name3
    SSA_SI_RSP_SZ ,                                             // Size of respose code
    a_str_dds_msg ,                                             // Error Message
    SSA_SI_SSA_MSG_SZ                                           // Size of error message
  ) ;

  if ( l_rc < 0 ) {
    fprintf ( f_log_fopen_status , "rc               : %ld\n" , l_rc ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }

  if ( a_str_rsp_code[ 0 ] != '0' ) {
    fprintf ( f_log_fopen_status , "rsp_code         : %s\n" , a_str_rsp_code ) ;
    fprintf ( f_log_fopen_status , "dds_msg          : %s\n" , a_str_dds_msg ) ;
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

  printf ( "Error occurred in '%s'\n" , func ) ;
  exit ( 1 ) ;

}
/**********************************************************************
 End of subroutine s_doExit                                           *
**********************************************************************/

static void s_MkeRng_open ( ) {
/* s_test_dds_open subroutine called in s_mkeKey_open subroutine and
   assign subroutine parameters
*/

  l_rc = s_test_dds_open                                        // Open session
  (
    l_sockh ,                                                   // Set to -1 as not calling the SSA-name3 server
    &l_session_id ,                                             // Should be -1 on the ddsn3 open call ,or opening a new session
    p_system_nm ,                                               // Defines location of the population rules
    p_population ,                                              // Population name of country
    ""                                                          // Controls
  ) ;

  if ( 0 != l_rc )
    s_doExit ( "s_test_dds_open" ) ;

}
/**********************************************************************
 End of subroutine s_MkeRng_open                                      *
**********************************************************************/

static void s_MkeRng_ranges (
/*
s_test_dds_get_ranges subroutine call in s_MkeRng_ranges
s_test_dds_get_ranges subroutine contain ssan3_get_ranges function
Used to get the dds-name3 key rangfes for a name or address which
the application program will use in a select statement to retreive records
from the dds-name3 key table
*/
  char *Narrow ,                                                // Narrow SEARCH LEVEL
  char *Typical ,                                               // Typical SEARCH LEVEL
  char *Exhaustive ,                                            // Exhaustive SEARCH LEVEL
  char *Extreme ,                                               // Extreme SEARCH LEVEL
  char *ky_fld_search_lvl1 ,                                    // Format1 of KEY FIELD AND SEARCH LEVEL
  char *ky_fld_search_lvl2 ,                                    // Format2 of KEY FIELD AND SEARCH LEVEL
  char *ky_fld_search_lvl3 ,                                    // Format3 of KEY FIELD AND SEARCH LEVEL
  char *ky_fld_search_lvl4 ,                                    // Format4 of KEY FIELD AND SEARCH LEVEL
  char *str_ID                                                  // ID
) {

  // Search levels are Narrow , Typical , Exhaustive , Extreme
  char *SEARCH_LEVEL [ 4 ] = { Narrow , Typical , Exhaustive , Extreme } ;

  char *ky_fld_search_lvl [ 4 ] = { ky_fld_search_lvl1 , ky_fld_search_lvl2 , ky_fld_search_lvl3 , ky_fld_search_lvl4 } ;

  for( i_idx = 0 ; i_idx <= 3 ; i_idx++ ) {
    l_rc = s_test_dds_get_ranges
    (
      l_sockh ,                                                 // Set to -1 as not calling the SSA-name3 server
      &l_session_id ,                                           // Should be -1 on the ddsn3 open call ,or opening a new session
      p_system_nm ,                                             // Define the location of population
      p_population ,                                            // Country Name
      SEARCH_LEVEL[i_idx] ,                                     // Search level
      str_current_rec ,                                         // Current record
      i_cur_rec_len ,                                           // Length of a current record
      "TEXT" ,                                                  // Encrypted type by default is Text
      ky_fld_search_lvl[i_idx] ,                                // abbreviation of KEY FIELD and SEARCH LEVEL
      str_ID                                                    // ID
    ) ;

    if ( 0 != l_rc )
      s_doExit ( "s_test_dds_get_ranges" ) ;
  }
}
/**********************************************************************
 End of subroutine s_MkeRng_ranges                                    *
**********************************************************************/

static void s_MkeRng_close ( ) {
// Close the previously established session.
  l_rc = s_test_dds_close
  (
    l_sockh ,                                                   // Set to -1 as not calling the SSA-name3 server
    &l_session_id ,                                             // Set to -1
    p_system_nm ,                                               // System name
    p_population ,                                              // Country name
    ""                                                          // Controls
  ) ;

  if ( 0 != l_rc )
    s_doExit ( "s_test_dds_close" ) ;

}
/**********************************************************************
 End of subroutine s_MkeRng_close                                     *
**********************************************************************/

int main ( int argc , char *argv[] ) {
  // Main function

  t_start_time = clock( ) ;                                     // Start time
  s_getParameter ( argc , argv ) ;                              // Subroutine to get parameter

  fprintf ( f_log_fopen_status, "------ Run Parameters ------" ) ;
  if ( p_data_set != 0 ) {                                      // If data set number is not empty
    fprintf ( f_log_fopen_status, "\nData set number       : %d" , p_data_set ) ;
  }

  if ( p_run_time != 0 ) {                                      // If run time number is not empty
    fprintf ( f_log_fopen_status, "\nRun time number       : %d" , p_run_time ) ;
  }
  
  if ( *p_system_nm ) {                                        // If System name is non empty
    fprintf ( f_log_fopen_status, "\nSystem name           : %s", p_system_nm ) ;
  }

  if ( *p_population ) {                                        // If Population is non empty
    fprintf ( f_log_fopen_status, "\nPopulation            : %s" , p_population ) ;
  }

  if ( *p_infdir ) {                                            // If Input file directory is non empty
    fprintf ( f_log_fopen_status, "\nInput File Directory  : %s" , p_infdir ) ;
  }

  if ( *p_outfdir ) {                                           // If Output file directory is non empty
    fprintf ( f_log_fopen_status, "\nOutput File Directory : %s" , p_outfdir ) ;
  }

  if ( *p_logfdir ) {                                           // If Log file directory is non empty
    fprintf ( f_log_fopen_status, "\nLog File Directory    : %s" , p_logfdir ) ;
  }

  fprintf ( f_log_fopen_status  , "\nVerbose               : %s\n" , ( i_verbose_flg == 1 ? "Yes" : "No" ) ) ;

  fprintf ( f_log_fopen_status, "\n------ File Names ------" ) ;
  fprintf ( f_log_fopen_status, "\nInput file name       : %s", a_str_input_file ) ;
  fprintf ( f_log_fopen_status, "\nOutput file name      : %s", a_str_output_file ) ;
  fprintf ( f_log_fopen_status, "\nLog file name         : %s\n\n", a_str_log_file ) ;

  s_MkeRng_open ( ) ;                                           // Call S_MkeRng_open

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
        i_error_record_flds ++ ;                                // Missing Person Name, Organization name and Address Part 1 fields records count
        fprintf ( f_log_fopen_status, "\nRecord no : %d Error Message : %s", i_rec_number ,"Missing Person Name, Organization name, Address Part 1 fields" ) ;
        fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_current_rec ) ;
      }

      // Check Person_Name is inside the current record
      if ( strstr ( str_current_rec , "Person_Name" ) != NULL ) {
        char *PN = "FIELD=Person_Name SEARCH_LEVEL=Narrow" ;
        char *PT = "FIELD=Person_Name SEARCH_LEVEL=Typical" ;
        char *PH = "FIELD=Person_Name SEARCH_LEVEL=Exhaustive" ;
        char *PR = "FIELD=Person_Name SEARCH_LEVEL=Extreme";

        char *abv_PNarr  = "PN" ;                               // abrevation of Person_Name Narrow
        char *abv_PTyp   = "PY" ;                               // abrevation of Person_Name Typical
        char *abv_PExh   = "PH" ;                               // abrevation of Person_Name Exhaustive
        char *abv_PExtr  = "PR" ;                               // abrevation of Person_Name Extreme

        i_pn_records++ ;                                        // No of records count that contains Person_Name

        s_MkeRng_ranges ( PN , PT ,PH , PR , abv_PNarr , abv_PTyp , abv_PExh , abv_PExtr , str_ID ) ;                                         // Call s_MkeRng_ranges
      }

      // Check Organization_Name is inside the current record
      if ( strstr ( str_current_rec , "Organization_Name" ) != NULL ) {
        char *ON = "FIELD=Organization_Name SEARCH_LEVEL=Narrow" ;
        char *OT = "FIELD=Organization_Name SEARCH_LEVEL=Typical" ;
        char *OH = "FIELD=Organization_Name SEARCH_LEVEL=Exhaustive" ;
        char *OR = "FIELD=Organization_Name SEARCH_LEVEL=Extreme";

        char *abv_ONarr  = "ON" ;                               // abrevation of Organization_Name Narrow
        char *abv_OTyp   = "OY" ;                               // abrevation of Organization_Name Typical
        char *abv_OExh   = "OH" ;                               // abrevation of Organization_Name Exhaustive
        char *abv_OExtr  = "OR" ;                               // abrevation of Organization_Name Extreme

        i_on_records++ ;                                        // No of records count that contains Organization_Name

        // Call s_mkeKey_getKey
        s_MkeRng_ranges ( ON , OT ,OH , OR , abv_ONarr , abv_OTyp , abv_OExh , abv_OExtr , str_ID ) ;                                         // Call s_MkeRng_ranges
      }

      // Check Address_Part1 is inside the current record
      if ( strstr ( str_current_rec , "Address_Part1" ) != NULL ) {
        char *AN = "FIELD=Address_Part1 SEARCH_LEVEL=Narrow" ;
        char *AT = "FIELD=Address_Part1 SEARCH_LEVEL=Typical" ;
        char *AH = "FIELD=Address_Part1 SEARCH_LEVEL=Exhaustive" ;
        char *AR = "FIELD=Address_Part1 SEARCH_LEVEL=Extreme";

        char *abv_ANarr  = "1N" ;                               // abrevation of Address_Part1 Narrow
        char *abv_ATyp   = "1Y" ;                               // abrevation of Address_Part1 Typical
        char *abv_AExh   = "1H" ;                               // abrevation of Address_Part1 Exhaustive
        char *abv_AExtr  = "1R" ;                               // abrevation of Address_Part1 Extreme


        i_addp1_records++ ;                                     // No of records count that contains Address_Part1

        s_MkeRng_ranges ( AN , AT ,AH , AR , abv_ANarr , abv_ATyp , abv_AExh , abv_AExtr , str_ID ) ;                                         // Call s_MkeRng_ranges
      }

      // Check Address_Part1 is inside the current record
      if ( strstr ( str_current_rec , "Sex_Code" ) != NULL ) {
        char *GN = "FIELD=Sex_Code SEARCH_LEVEL=Narrow" ;
        char *GT = "FIELD=Sex_Code SEARCH_LEVEL=Typical" ;
        char *GH = "FIELD=Sex_Code SEARCH_LEVEL=Exhaustive" ;
        char *GR = "FIELD=Sex_Code SEARCH_LEVEL=Extreme";

        char *abv_GNarr  = "GN" ;                               // abrevation of Address_Part1 Narrow
        char *abv_GTyp   = "GY" ;                               // abrevation of Address_Part1 Typical
        char *abv_GExh   = "GH" ;                               // abrevation of Address_Part1 Exhaustive
        char *abv_GExtr  = "GR" ;                               // abrevation of Address_Part1 Extreme


        i_sex_code_records++ ;                                     // No of records count that contains Address_Part1

        s_MkeRng_ranges ( GN , GT ,GH , GR , abv_GNarr , abv_GTyp , abv_GExh , abv_GExtr , str_ID ) ;                                         // Call s_MkeRng_ranges
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
    fprintf ( f_log_fopen_status, "\n - Missing all 4 key fields    : %d\n", i_error_record_flds ) ;
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
  
  if ( i_sex_code_records != 0 ) {                              // If Records with Address_Part1 count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nRecords with Sex_Code          : %d\n", i_sex_code_records ) ;
  }

  if ( i_Person_Name_keys != 0 ) {                              // If Person_Name keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nPerson_Name keys       : %d", i_Person_Name_keys ) ;
  }

  if ( i_PN_ky != 0 ) {                                         // If Person_Name Narrow keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Narrow     : %d", i_PN_ky ) ;
  }

  if ( i_PY_ky != 0 ) {                                         // If Person_Name Typical keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Typical    : %d",  i_PY_ky ) ;
  }

  if ( i_PH_ky != 0 ) {                                         // If Person_Name Exhaustive keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Exhaustive : %d",  i_PH_ky ) ;
  }

  if ( i_PR_ky != 0 ) {                                         // If Person_Name Extreme keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Extreme    : %d\n",  i_PR_ky ) ;
  }

  if ( i_Organ_Name_keys != 0 ) {                               // If Organization_Name keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nOrganization_Name keys : %d", i_Organ_Name_keys ) ;
  }

  if ( i_ON_ky != 0 ) {                                         // If Organization_Name Narrow keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Narrow     : %d", i_ON_ky ) ;
  }

  if ( i_OY_ky != 0 ) {                                         // If Organization_Name Typical keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Typical    : %d", i_OY_ky ) ;
  }

  if ( i_OH_ky != 0 ) {                                         // If Organization_Name Exhaustive keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Exhaustive : %d", i_OH_ky ) ;
  }

  if ( i_OR_ky != 0 ) {                                         // If Organization_Name Extreme keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Extreme    : %d\n", i_OR_ky ) ;
  }

  if ( i_Add_Part1_keys != 0 ) {                               // If Address_Part1 keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nAddress_Part1 keys     : %d", i_Add_Part1_keys ) ;
  }

  if ( i_1N_ky != 0 ) {                                        // If Address_Part1 Narrow keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Narrow     : %d", i_1N_ky ) ;
  }

  if ( i_1Y_ky != 0 ) {                                        // If Address_Part1 Typical keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Typical    : %d", i_1Y_ky ) ;
  }

  if ( i_1H_ky != 0 ) {                                        // If Address_Part1 Exhaustive keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Exhaustive : %d", i_1H_ky ) ;
  }

  if ( i_1R_ky != 0 ) {                                        // If Address_Part1 Extreme keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Extreme    : %d\n", i_1R_ky ) ;
  }

  if ( i_sex_code_keys != 0 ) {                               // If Address_Part1 keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nSex_Code keys          : %d", i_sex_code_keys ) ;
  }

  if ( i_GN_ky != 0 ) {                                        // If Address_Part1 Narrow keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Narrow     : %d", i_GN_ky ) ;
  }

  if ( i_GY_ky != 0 ) {                                        // If Address_Part1 Typical keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Typical    : %d", i_GY_ky ) ;
  }

  if ( i_GH_ky != 0 ) {                                        // If Address_Part1 Exhaustive keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Exhaustive : %d", i_GH_ky ) ;
  }

  if ( i_GR_ky != 0 ) {                                        // If Address_Part1 Extreme keys count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Extreme    : %d\n", i_GR_ky ) ;
  }

  if ( i_total_kys_written != 0 ) {                            // If Total keys written count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nTotal keys written     : %d\n", i_total_kys_written ) ;
  }

  // Addition of all the keys and check it is match with total keys
  i_addition_key =
                   i_PN_ky + i_PY_ky + i_PH_ky + i_PR_ky +
                   i_ON_ky + i_OY_ky + i_OH_ky + i_OR_ky +
                   i_1N_ky + i_1Y_ky + i_1H_ky + i_1R_ky +
                   i_GN_ky + i_GY_ky + i_GH_ky + i_GR_ky ;

  if ( i_addition_key != i_total_kys_written ) {
    fprintf ( f_log_fopen_status, "\nMissmatch in counts\n") ;
  }

  s_MkeRng_close ( ) ;                                          // Call s_MkeRng_close

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
