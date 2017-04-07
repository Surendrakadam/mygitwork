/*
 Procedure     : GMtc.c
 Application   : De-Dupe
 Client        : Internal
 Copyright (c) : IdentLogic Systems Private Limited
 Author        : Surendra Kadam
 Creation Date : 5 April 2017
 Description   :

 WARNINGS      : Length of the Input file directory , Output file directory and
                  Log file directory should not exceed 1 ,000 bytes , with total
                  filepath not exceeding 1011  , 1011  ,1038 resp.  This is due
                  to the length of the Input file name , Output file name and
                  Log file name are 11 , 11 and 38 resp.

                  Increase the array size as per your convenience.
 HARD CODINGS  :
 Limitations   :
 Dependencies  :
 Modifications
 Date       Change Req# Author       Description
 ---------- ----------- ------------ ---------------------------

*/

#include <stdio.h>                                              // Use some functions like printf(), scanf() etc
#include <stdlib.h>                                             // Memory allocation, process control, conversions and others
#include <string.h>                                             // Contain String related functions
#include "ssan3cl.h"                                            // Dedupe header
#include <getopt.h>                                             // Getoptions header
#include <unistd.h>                                             // Code's entry point have various constant, type and function declarations
#include <time.h>                                               // Timer header

#define rangeof( arr )  ( sizeof( arr )/sizeof( ( arr )[ 0 ] ) )                // Size of an array

long    l_rc         =  0 ;                                     // Indicate success or failure of open / close sesions
long    l_sockh      = -1 ;                                     // Set to -1 as not calling the dds-name3 server
// Should be -1 on the ddsn3_open call, if opening  a new session, a valid Session ID or any other call
long    l_session_id = -1 ;

char str_current_rec [ BUFSIZ ]  = {0} ;                        // Current record of a file
int i_rec_number  = 0 ;                                         // Record counter
int i_cur_rec_len = 0 ;                                         // Current record length
int i_idx         = 0 ;                                         // Main method for loop initial variable

// Time variables
int i_YYYY = 0 ;                                                // Year
int i_MM   = 0 ;                                                // Month
int i_DD   = 0 ;                                                // Date
int i_HH24 = 0 ;                                                // Hour-24
int i_MI   = 0 ;                                                // Minute
int i_SS   = 0 ;                                                // Seconds

// Procedure run parameters
int   p_data_set   = 0 ;                                        // Parameter data set number
int   p_run_time   = 0 ;                                        // Parameter run time data
char *p_system_nm  = "" ;                                       // Parameter System name
char *p_population = "" ;                                       // Parameter Population name
char *p_purpose    = "" ;                                       // Parameter Purpose
int   p_uni_enc    = 0 ;                                        // Parameter Unicode encoding either 4 , 6 or 8
char *p_nm_fmt     = "" ;                                       // Parameter Name format
int   p_acc_lmt    = 0 ;                                        // Parameter Accept Limit
int   p_rej_lmt    = 0 ;                                        // Parameter Reject Limit
char *p_delimeter  = "" ;                                       // Parameter Delimeter
char *p_infdir     = "" ;                                       // Parameter input file name
char *p_outfdir    = "" ;                                       // Parameter output file directory
char *p_logfdir    = "" ;                                       // Parameter log file directory
int   p_multiplier = 0 ;                                        // Parameter Multiplier

int i_multiplier   = 0 ;                                        // Multiplier variable
int  i_len_of_dir  = 0 ;                                        // Length of the directory
char c_flg_slash   = 0 ;                                        // check backslash or forward slash

int  i_option = 0 ;                                             // Switch case variable

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

char a_search_data[5000] = {0} ;                                // SEARCH DATA
char a_file_data[5000] = {0} ;                                  // FILE DATA
int i_fld_spr = 0 ;                                             // Field separator position

int  i_search_data_len = 0 ;                                    // SEARCH DATA length
int  i_file_data_len   = 0 ;                                    // FILE DATA length

// SEARCH DATA, Id related variables
char *str_ptr_id_search           = "" ;                        // Search *Id* in search data capture string from *Id* to till end of the string
char *str_ID_search               = "" ;                        // Substring Id of search data
int  i_id_start_pos_search        = 0 ;                         // Start position of id in search data
int  i_pos_afr_id_search          = 0 ;                         // Position after *Id* in search data
int  i_asterisk_start_pos_search  = 0 ;                         // After *Id* first * position in search data
int  i_frt_ast_pos_search         = 0 ;                         // First asterisk position after *Id* in search data

// FIEL DATA , Id related variables
char *str_ptr_id_file           = "" ;                          // Search *Id* in file data capture string from *Id* to till end of the string
char *str_ID_file               = "" ;                          // Substring Id of file data
int  i_id_start_pos_file        = 0 ;                           // Start position of id in file data
int  i_pos_afr_id_file          = 0 ;                           // Position after *Id* in file data
int  i_asterisk_start_pos_file  = 0 ;                           // After *Id* first * position in file data
int  i_frt_ast_pos_file         = 0 ;                           // First asterisk position after *Id* in file data

// Clock execution time variables
clock_t t_start_time  = 0 ;                                     // Clock object
clock_t t_end_time    = 0 ;                                     // End time
double t_time_taken   = 0.0 ;                                   // Time taken
int i_t_time_taken    = 0 ;                                     // Round of float value

// Flags 0 or 1
int i_verbose_flg    = 0 ;                                      // Verbose flag
int i_multiplier_flg = 0 ;                                      // Multiplier flag

const float f_add    = 0.5 ;                                    // Add 0.5 to time taken

int t_hour = 0 ;                                                // Hours to execute records
int t_min  = 0 ;                                                // Minutes to execute records
int t_sec  = 0 ;                                                // Seconds to execute records

char a_str_MM[2]        = {0} ;                                 // Compute 2 digit Month
char a_str_DD[2]        = {0} ;                                 // Compute 2 digit Date
char a_str_HH24[2]      = {0} ;                                 // Compute 2 digit Hours
char a_str_MI[2]        = {0} ;                                 // Compute 2 digit Minutes
char a_str_SS[2]        = {0} ;                                 // Compute 2 digit Seconds

char a_str_hour[2] = {0} ;                                      // Compute 2 digit Hours to execute records
char a_str_min[2]  = {0} ;                                      // Compute 2 digit Minutes to execute records
char a_str_sec[2]  = {0} ;                                      // Compute 2 digit Seconds to execute records

// Error
int i_no_fld_spr = 0 ;                                          // No field separator found in current record counter

// Controls related variables
char a_uni_enc[19]   = {0} ;                                    // Unicode encoding format e.g UNICODE=4
char a_nm_fmt[13]    = {0} ;                                    // Name Format e.g NAMEFORMAT=R
char a_delimeter[100]= {0} ;                                    // Delimiter in Tagged data
char a_prps_ty[100]  = {0} ;                                    // Purpose with match level Typical
char a_prps_con[100] = {0} ;                                    // Purpose with match level Conservative
char a_prps_lse[100] = {0} ;                                    // Purpose with match level Loose

char a_mtc_lvl_typ_acc_rej[100] = {0} ;
char a_mtc_lvl_con_acc_rej[100] = {0} ;
char a_mtc_lvl_lse_acc_rej[100] = {0} ;

char *S_K_prps        = "PURPOSE=" ;                            // PURPOSE= format
char *str_prps_nm     = "" ;                                    // Purpose name
char *S_K_mtc_lvl     = "MATCH_LEVEL=" ;                        // MATCH_LEVEL= format
char *S_K_mtc_lvl_ty  = "Typical" ;                             // Match level Typical
char *S_K_mtc_lvl_con = "Conservative" ;                        // Match level Conservative
char *S_K_mtc_lvl_lse = "Loose" ;                               // Match level Loose
char *S_K_uni_enc     = " UNICODE_ENCODING=";                   // UNICODE_ENCODING= format
int   i_uni_enc_d     = 0 ;                                     // Unicode encoding data
char *S_K_nm_fmt      = " NAMEFORMAT=";                         // NAMEFORMAT= format
char *str_nm_fmt_d    = "" ;                                    // Name format data
char *S_K_delimeter   = " DELIMETER=" ;                         // DELIMETER= format
char *str_delimeter_d = "" ;                                    // Delimeter data

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
    "GMtc -d <data_set> -r <run> -s<system-name> -p <population> "
    "-u<purpose> -e<unicode_encoding> -n<name_format> -a<accept_limit> "
    "-j<reject_limit> -i <input_file_directory>"
    " -o <output_file_directory> -l <log_file_directory> -m <Multiplier> -v<verbose>\n\nExample:\n\n"
    "GMtc -d 101 -r 1001 -s default -p india -u Address -e 4 -n L  -a 20 -j -10 "
    "-i E:/SurendraK/Work/DeDupeProcs/Input/"
    " -o E:/SurendraK/Work/DeDupeProcs/Output/ -l E:/SurendraK/Work/SSAProcs/Log/ -m 10000 -v\n"
   ) ;

}
/**********************************************************************
 End of subroutine s_print_usage                                      *
**********************************************************************/

static void s_getParameter ( int argc , char *argv[] ) {
// This subroutine is default parameter of getopt in s_getParameter

  s_date_time ( ) ;                                             // Call subroutine s_date_time
  while ( ( i_option = getopt ( argc , argv , "d:r:s:p:u:e:n:a:j:t:i:o:l:m:v::" ) ) != -1 ) {
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
      case 'u' :                                                // Purpose parameter
        p_purpose    = optarg ;
        break ;
      case 'e' :                                                // Unicode encoding parameter
        p_uni_enc    = atoi( optarg ) ;
        break ;
      case 'n' :                                                // Name Format parameter
        p_nm_fmt     = optarg ;
        break ;
      case 'a' :                                                // Accept limit parameter
        p_acc_lmt    = atoi( optarg ) ;
        break ;
      case 'j' :                                                // Reject limit parameter
        p_rej_lmt    = atoi( optarg ) ;
        break ;
      case 't' :                                                // Delimeter parameter
        p_delimeter  = optarg ;
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

  str_prps_nm  = p_purpose ;

  i_uni_enc_d  = p_uni_enc ;

  str_nm_fmt_d = p_nm_fmt ;
  // Data set number should be in a range of 100 to 999
  if( p_data_set > 999 || p_data_set < 100 ) {
    printf("%s","JOB ABANDONDED - Data set number should be integer and in a range of 100 to 999" ) ;
    exit(1) ;
  }

  // Run time number should be in a range of 1000 to 9999
  if( p_run_time > 9999 || p_run_time < 1000 ) {
    printf("%s","JOB ABANDONDED - Run time number should be integer and in a range of 1000 to 9999" ) ;
    exit(1) ;
  }

  if ( !*p_system_nm ) {                                        // Abort if System name is empty
    printf ("%s","JOB ABANDONDED - Missing system name\n" ) ;
  }

  if ( !*p_population ) {                                       // Abort if Population is mepty
    printf ("%s","JOB ABANDONDED - Missing population\n" ) ;
  }

  if ( !*str_prps_nm ) {                                        // Abort if Purpose is mepty
    printf ("%s","JOB ABANDONDED - Missing purpose\n" ) ;
  }

  if ( i_uni_enc_d != 0 ) {                                     // If unicode encoding is not zero
    if ( i_uni_enc_d != 4 &&                                    // Unicode Encoding must be 4 , 6 and 8
         i_uni_enc_d != 6 &&
         i_uni_enc_d != 8 ) {
      printf ( "%s" , "Invalid CHARACTER ENCODING value" ) ;
      exit(1);
    }
  }

  if ( *str_nm_fmt_d ) {                                        // If Name Format string is not empty
    if ( ( strcmp ( str_nm_fmt_d , "L" ) != 0 ) &&              // Name Format must be L or R
         ( strcmp ( str_nm_fmt_d , "R" ) != 0 ) ) {
      printf ( "%s" , "Invalid NAMEFORMAT value - must be L or R" ) ;
      exit(1);
    }
  }

  if ( i_uni_enc_d != 0 ) {
    sprintf ( a_uni_enc , "%s%d" , S_K_uni_enc , i_uni_enc_d ) ;
  }

  if ( *str_nm_fmt_d ) {
    sprintf ( a_nm_fmt , "%s%s" , S_K_nm_fmt , str_nm_fmt_d ) ;
  }

  if ( *str_delimeter_d ) {
    sprintf ( a_delimeter , "%s%s" , S_K_delimeter , str_delimeter_d ) ;
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

  // Check Output file directory ends with backslash or forward slash, if not add it
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

  // Check log file directory ends with backslash or forward slash, if not add it
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
  sprintf( a_str_input_file , "%d%d.tag" , p_data_set , p_run_time ) ;
  sprintf( a_str_output_file , "%d%d.umj" , p_data_set , p_run_time ) ;
  sprintf( a_str_log_file , "%d%d_GMtc_%d-%s-%s-%s-%s-%s.log", p_data_set, p_run_time, i_YYYY, a_str_MM, a_str_DD, a_str_HH24, a_str_MI, a_str_SS ) ;

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

  fprintf ( f_log_fopen_status, "------ GMtc EXECUTION START DATE AND TIME ------\n" ) ;
  fprintf ( f_log_fopen_status, "%d-%s-%s %s:%s:%s \n\n", i_YYYY, a_str_MM, a_str_DD, a_str_HH24, a_str_MI, a_str_SS ) ;

}
/**********************************************************************
 End of subroutine s_getParameter                                     *
**********************************************************************/

static long s_test_dds_open (
  long    l_sockh ,
  long    *l_session_id ,
  char    *str_sysName ,
  char    *str_population ,
  char    *str_controls
 ) {

  long    l_rc ;
  char    a_rsp_code[ SSA_SI_RSP_SZ ] ;
  char    a_dds_msg[ SSA_SI_SSA_MSG_SZ ] ;

  l_rc = ssan3_open
  (
    l_sockh ,
    l_session_id ,
    str_sysName ,
    str_population ,
    str_controls ,
    a_rsp_code ,
    SSA_SI_RSP_SZ ,
    a_dds_msg ,
    SSA_SI_SSA_MSG_SZ
  ) ;

  if ( l_rc < 0 ) {
    printf ( "l_rc               : %ld\n" , l_rc ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }

  if ( a_rsp_code[ 0 ] != '0' && *l_session_id == -1 ) {
    printf ( "a_rsp_code         : %s\n" , a_rsp_code ) ;
    printf ( "a_dds_msg          : %s\n" , a_dds_msg ) ;
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

static long s_test_dds_match (
  long    l_sockh ,
  long    *l_session_id ,
  char    *str_sysName ,
  char    *str_population ,
  char    *str_controls ,
  char    *str_search ,
  long    l_searchLength ,
  char    *str_searchEncType ,
  char    *str_file ,
  long    l_fileLength ,
  char    *str_fileEncType ,
  char    *str_ID_search ,
  char    *str_ID_file
 ) {

  long    l_rc ;
  char    a_rsp_code[ SSA_SI_RSP_SZ ] ;
  char    a_dds_msg[ SSA_SI_SSA_MSG_SZ ] ;
  char    a_score[ SSA_SI_SCORE_SZ ] ;
  char    a_decision[ SSA_SI_DECISION_SZ ] ;

  l_rc = ssan3_match_encoded
  (
    l_sockh ,
    l_session_id ,
    str_sysName ,
    str_population ,
    str_controls ,
    a_rsp_code ,
    SSA_SI_RSP_SZ ,
    a_dds_msg ,
    SSA_SI_SSA_MSG_SZ ,
    str_search ,
    l_searchLength ,
    str_searchEncType ,
    str_file ,
    l_fileLength ,
    str_fileEncType ,
    a_score ,
    SSA_SI_SCORE_SZ ,
    a_decision ,
    SSA_SI_DECISION_SZ
  ) ;

  if ( l_rc < 0 ) {
    printf ( "l_rc               : %ld\n" , l_rc ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }

  if ( a_rsp_code[ 0 ] != '0' ) {
    printf ( "a_rsp_code         : %s\n" , a_rsp_code ) ;
    printf ( "a_dds_msg          : %s\n" , a_dds_msg ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }
  fprintf (f_output_fopen_status ,"%s-%s-%s-%s-%s\n" , a_score ,a_decision ,str_controls,str_ID_search ,str_ID_file ) ;

  l_rc = 0 ;
  goto SUB_RETURN ;

SUB_RETURN:
  return ( l_rc ) ;

}
/**********************************************************************
 End of subroutine s_test_dds_match                                   *
**********************************************************************/

static long s_test_dds_close (
  long    l_sockh ,
  long    *l_session_id ,
  char    *str_sysName ,
  char    *str_population ,
  char    *str_controls
 ) {

  long    l_rc ;
  char    a_rsp_code[ SSA_SI_RSP_SZ ] ;
  char    a_dds_msg[ SSA_SI_SSA_MSG_SZ ] ;

  s_date_time ( ) ;                                             // Call date and time subroutine

  l_rc = ssan3_close
  (
    l_sockh ,
    l_session_id ,
    str_sysName ,
    str_population ,
    str_controls ,
    a_rsp_code ,
    SSA_SI_RSP_SZ ,
    a_dds_msg ,
    SSA_SI_SSA_MSG_SZ
  ) ;

  if ( l_rc < 0 ) {
    printf ( "l_rc               : %ld\n" , l_rc ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }

  if ( a_rsp_code[ 0 ] != '0' ) {
    printf ( "a_rsp_code         : %s\n" , a_rsp_code ) ;
    printf ( "a_dds_msg          : %s\n" , a_dds_msg ) ;
    l_rc = 1 ;
    goto SUB_RETURN ;
  }

  l_rc = 0 ;
  goto SUB_RETURN ;

  fprintf ( f_log_fopen_status, "\nEnded %d-%s-%s %s:%s:%s ", i_YYYY, a_str_MM, a_str_DD, a_str_HH24, a_str_MI, a_str_SS ) ;

SUB_RETURN:
  return ( l_rc ) ;

}
/**********************************************************************
 End of subroutine s_test_dds_close                                   *
**********************************************************************/

static void s_doExit ( char *func ) {
/* Error occurred in subroutines s_test_dds_open, s_test_dds_get_ranges or
  s_test_dds_close */

  fprintf ( f_log_fopen_status , "Error occurred in '%s'\n" , func ) ;
  printf ( "Error occurred in '%s'\n" , func ) ;
  exit ( 1 ) ;

}
/**********************************************************************
 End of subroutine s_doExit                                           *
**********************************************************************/

static void s_GMtc_open ( ) {
  // Establish a session.

  l_rc = s_test_dds_open
  (
    l_sockh ,
    &l_session_id ,
    p_system_nm ,
    p_population ,
    ""
  ) ;

  if ( 0 != l_rc )
    s_doExit ( "s_test_dds_open" ) ;

}
/**********************************************************************
 End of subroutine s_GMtc_open                                        *
**********************************************************************/

static void s_GMtc_matches (

char *str_search_data ,
char *str_file_data   ,
int  i_search_data_len ,
int  i_file_data_len ,
char *str_ID_search ,
char *str_ID_file ,
char *str_Typical ,
char *str_Conservative ,
char *str_Loose

) {
  // Call ddsn3_match
  //char *str_prps_ty = "PURPOSE=";
  //char *str_prps_nm = p_purpose ;
  //strcat ( str_prps_ty , str_prps_nm ) ;

  char *str_purpose[3] = { str_Typical , str_Conservative , str_Loose } ;
  for ( i_idx = 0 ; i_idx <= 2 ; i_idx++ ) {
    l_rc = s_test_dds_match
    (
      l_sockh ,
      &l_session_id ,
      p_system_nm ,
      p_population ,
      str_purpose[ i_idx ] ,
      str_search_data ,
      i_search_data_len ,
      "TEXT" ,
      str_file_data ,
      i_file_data_len ,
      "TEXT" ,
      str_ID_search ,
      str_ID_file
    ) ;

    if ( 0 != l_rc )
      s_doExit ( "s_test_dds_match" ) ;
  }

}
/**********************************************************************
 End of subroutine s_GMtc_matches                                     *
**********************************************************************/

static void s_GMtc_close ( ) {
  // Close the previously established session.

  l_rc = s_test_dds_close
  (
    l_sockh ,
    &l_session_id ,
    p_system_nm ,
    p_population ,
    ""
  ) ;

  if ( 0 != l_rc )
    s_doExit ( "s_test_dds_close" ) ;

}
/**********************************************************************
 End of subroutine s_GMtc_close                                       *
**********************************************************************/

int main ( int argc , char *argv[] ) {

t_start_time = clock( ) ;                                       // Start time
s_getParameter ( argc , argv ) ;                                // Subroutine to get parameter

s_GMtc_open ( ) ;

  // Read a input file line by line
while( fgets ( str_current_rec , sizeof ( str_current_rec ) , f_input_fopen_status ) ) {
  i_cur_rec_len = strlen( str_current_rec ) ;                   // Length of the current record
  if ( i_cur_rec_len > 0 && str_current_rec[i_cur_rec_len-1] == '\n' ) {
    str_current_rec[--i_cur_rec_len] = '\0' ;                   // Remove new line character from current record
  }

  for( i_idx = 1 ; i_idx < i_cur_rec_len; i_idx++ ) {
    if( str_current_rec[i_idx] == ';' ) {                       // Search data and file data separator
      i_fld_spr = i_idx ;                                       // After *Id* first * position
      break ;                                                   // Until find separator
    }
    else {
      i_no_fld_spr ++ ;                                         // Current record does not contain field separator
    }
  }

  strncpy                                                       // Copy substring from string
  (
    a_search_data ,                                             // Search data
    str_current_rec ,
    i_fld_spr                                                   // Position of field separator
  ) ;

  strncpy                                                       // Copy substring from string
  (
    a_file_data ,                                               // File data
    str_current_rec + ( i_fld_spr + 1 ) ,
    i_cur_rec_len                                               // Length of current record
  ) ;

  a_search_data[i_fld_spr]   = '\0' ;                           // Add null terminator at the end of search data
  a_file_data[i_cur_rec_len] = '\0' ;                           // Add null terminator at the end of file data

  i_search_data_len = strlen( a_search_data ) ;                 // Length of search data
  i_file_data_len   = strlen( a_file_data ) ;                   // Length of file data

  // Check search data and file data contain *Id* or not
  if ( strstr ( a_search_data ,"*Id*" ) != NULL && strstr ( a_file_data ,"*Id*" ) != NULL ) {

    // Find Id of Search data
    str_ptr_id_search     = strstr ( a_search_data , "*Id*" ) ; // Search *Id* in search data capture string from *Id* to till end of the string

    i_id_start_pos_search =                                     // Starting position of *Id* in search data
    (
      i_search_data_len - strlen( str_ptr_id_search )
    ) ;

    i_pos_afr_id_search   = ( i_id_start_pos_search + 4 ) ;     // Position after *Id* in search data

    for( i_idx = i_pos_afr_id_search ; i_idx < i_search_data_len; i_idx++ ) {
      if( a_search_data[i_idx] == '*' ) {
        i_asterisk_start_pos_search = i_idx ;                   // After *Id* first * position
        break ;                                                 // Break if first * position after *Id* found
      }
    }

    i_frt_ast_pos_search =                                      // First asterisk position after *Id*
    (
      i_asterisk_start_pos_search - i_pos_afr_id_search
    ) ;

    strncpy                                                     // Copy substring from string
    (
      str_ID_search ,                                           // Substring id of search data
      a_search_data + i_pos_afr_id_search ,
      i_frt_ast_pos_search                                      // First * position in search data
    ) ;

    str_ID_search[i_frt_ast_pos_search] = '\0' ;                // Add null terminator at the end of the Id in search data

    // Find Id of File data
    str_ptr_id_file     = strstr ( a_file_data , "*Id*" ) ;     // Search *Id* in current record capture string from *Id* to till end of the string

    i_id_start_pos_file =                                       // Starting position of *Id*
    (
      i_file_data_len - strlen(str_ptr_id_file)
    ) ;

    i_pos_afr_id_file   = ( i_id_start_pos_file + 4 ) ;         // Position after *Id*

    for( i_idx = i_pos_afr_id_file ; i_idx < i_file_data_len; i_idx++ ) {
      if( a_file_data[i_idx] == '*' ) {
        i_asterisk_start_pos_file = i_idx ;                     // After *Id* first * position
        break ;
      }
    }

    i_frt_ast_pos_file =                                        // First asterisk position after *Id*
    (
      i_asterisk_start_pos_file - i_pos_afr_id_file
    ) ;

    strncpy
    (
      str_ID_file ,                                             // Substring Id of file data
      a_file_data + i_pos_afr_id_file ,
      i_frt_ast_pos_file
    ) ;

    str_ID_file[i_frt_ast_pos_file] = '\0' ;                    // Add null terminator at the end of the Id in file data

    /*****************************************************************************/


    if ( p_acc_lmt == 0 && p_rej_lmt == 0 ) {                   // Both accpet limit and reject limit are zero
      sprintf ( a_mtc_lvl_typ_acc_rej ,"%s" , S_K_mtc_lvl_ty ) ;
    }
    else if ( p_acc_lmt != 0 && p_rej_lmt != 0 ) {              // Both accpet limit and reject limit are non zero
      sprintf ( a_mtc_lvl_typ_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_ty , p_acc_lmt , p_rej_lmt ) ;
    }
    else if ( p_acc_lmt == 0 && p_rej_lmt != 0 ) {              // Accept limit are zero and reject limit are not zero
      sprintf ( a_mtc_lvl_typ_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_ty , p_acc_lmt , p_rej_lmt ) ;
    }
    else if ( p_acc_lmt !=0  && p_rej_lmt == 0 ) {              // Accept limit are non zero and reject limit are zero
      sprintf ( a_mtc_lvl_typ_acc_rej ,"%s%+d" , S_K_mtc_lvl_ty , p_acc_lmt ) ;
    }

    if ( p_acc_lmt == 0 && p_rej_lmt == 0 ) {                   // Both accpet limit and reject limit are zero
      sprintf ( a_mtc_lvl_con_acc_rej ,"%s" , S_K_mtc_lvl_con ) ;
    }
    else if ( p_acc_lmt != 0 && p_rej_lmt != 0 ) {              // Both accpet limit and reject limit are non zero
      sprintf ( a_mtc_lvl_con_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_con , p_acc_lmt , p_rej_lmt ) ;
    }
    else if ( p_acc_lmt == 0 && p_rej_lmt != 0 ) {              // Accept limit are zero and reject limit are not zero
      sprintf ( a_mtc_lvl_con_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_con , p_acc_lmt , p_rej_lmt ) ;
    }
    else if ( p_acc_lmt !=0  && p_rej_lmt == 0 ) {              // Accept limit are non zero and reject limit are zero
      sprintf ( a_mtc_lvl_con_acc_rej ,"%s%+d" , S_K_mtc_lvl_con , p_acc_lmt ) ;
    }

    if ( p_acc_lmt == 0 && p_rej_lmt == 0 ) {                   // Both accpet limit and reject limit are zero
      sprintf ( a_mtc_lvl_lse_acc_rej ,"%s" , S_K_mtc_lvl_lse ) ;
    }
    else if ( p_acc_lmt != 0 && p_rej_lmt != 0 ) {              // Both accpet limit and reject limit are non zero
      sprintf ( a_mtc_lvl_lse_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_lse , p_acc_lmt , p_rej_lmt ) ;
    }
    else if ( p_acc_lmt == 0 && p_rej_lmt != 0 ) {              // Accept limit are zero and reject limit are not zero
      sprintf ( a_mtc_lvl_lse_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_lse , p_acc_lmt , p_rej_lmt ) ;
    }
    else if ( p_acc_lmt !=0  && p_rej_lmt == 0 ) {              // Accept limit are non zero and reject limit are zero
      sprintf ( a_mtc_lvl_lse_acc_rej ,"%s%+d" , S_K_mtc_lvl_lse , p_acc_lmt ) ;
    }

    sprintf
    (
       a_prps_ty ,                                              // Purpose with match level Typical
      "%s%s %s%s%s%s%s" ,
       S_K_prps ,                                               // PURPOSE= format
       str_prps_nm ,                                            // Purpose name
       S_K_mtc_lvl ,                                            // MATCH_LEVEL= format
       a_mtc_lvl_typ_acc_rej ,                                  // Match level Typical with accept limit and reject limit
       a_uni_enc ,                                              // Unicode encoding format e.g UNICODE=4
       a_nm_fmt ,                                               // Name Format e.g NAMEFORMAT=R
       a_delimeter                                              // Delimeter
    ) ;

    sprintf
    (
       a_prps_con ,                                             // Purpose with match level Conservative
       "%s%s %s%s%s%s%s" ,
       S_K_prps ,                                               // PURPOSE= format
       str_prps_nm ,                                            // Purpose name
       S_K_mtc_lvl ,                                            // MATCH_LEVEL= format
       a_mtc_lvl_con_acc_rej ,                                  // Match level Conservative with accept limit and reject limit
       a_uni_enc ,                                              // Unicode encoding format e.g UNICODE=4
       a_nm_fmt ,                                               // Name Format e.g NAMEFORMAT=R
       a_delimeter                                              // Delimeter
    ) ;

    sprintf
    (
       a_prps_lse ,                                             // Purpose with match level Loose
      "%s%s %s%s%s%s%s" ,
       S_K_prps ,                                               // PURPOSE= format
       str_prps_nm ,                                            // Purpose name
       S_K_mtc_lvl ,                                            // MATCH_LEVEL= format
       a_mtc_lvl_lse_acc_rej ,                                  // Match level Loose with accept limit and reject limit
       a_uni_enc ,                                              // Unicode encoding format e.g UNICODE=4
       a_nm_fmt ,                                               // Name Format e.g NAMEFORMAT=R
       a_delimeter                                              // Delimeter
    ) ;

    /*****************************************************************************/

    s_GMtc_matches
    (
      a_search_data ,
      a_file_data ,
      i_search_data_len ,
      i_file_data_len ,
      str_ID_search ,
      str_ID_file ,
      a_prps_ty ,
      a_prps_con ,
      a_prps_lse
    ) ;

    printf ( "%s\n" , a_prps_ty ) ;
    printf ( "%s\n" , a_prps_con ) ;
    printf ( "%s\n" , a_prps_lse ) ;

  } // End If

  } // End while loop
  s_GMtc_close ( ) ;



  return ( 0 ) ;


}
/**********************************************************************
 End of script GMtc.c                                             *
**********************************************************************/