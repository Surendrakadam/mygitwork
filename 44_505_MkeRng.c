/*
 Procedure     : 44_505_MkeRng.c
 Application   : De-Dupe
 Client        : Internal
 Copyright (c) : IdentLogic Systems Private Limited
 Author        : Surendra Kadam
 Creation Date : 3 April 2017
 Description   : Generates ( Search ) key range from Tagged data for search levels
                  Narrow (N) , Typical (Y) , Exhaustive (H) , Extreme (R) for
                  Person_Name (P) , Organization_name (O) , Address_Part1 ,
                  Sex_Code (G) as found in Tagged data

 WARNINGS      : If your data can contain asterisks , make sure that these are
                  either cleaned out prior to calling the dds-name3 functions ,
                  or use a different DELIMITER=setting.

                 Length of the Input file directory, Output file directory and
                  Log file directory should not exceed 1,000 bytes, with total
                  filepath not exceeding 1011 , 1011 ,1038 resp.  This is due
                  to the length of the Input file name, Output file name and
                  Log file name are 11, 11 and 38 resp.

                 Length of the current record , tag id and tag data should not
                  3000 , 1000 , 2000 resp.

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

#define rangeof( arr )  ( sizeof( arr ) / sizeof( ( arr )[ 0 ] ) )              // Size of an array

// Global variables

// dds NAME3 common variables rc, sockh, session_id

long  l_rc         =  0 ;                                       // Indicate success or failure of open / close sesions

long  l_sockh      = -1 ;                                       // Set to -1 as not calling the dds-NAME3 server

// Should be -1 on the ddsn3_open call, if opening  a new session, a valid Session ID or any other call
long  l_session_id = -1 ;

int i_idx          = 0 ;                                        // i_idx for loop variable

char str_current_rec [ 3000 ]  = {0} ;                          // Current record of a file
char str_tag_id[ 1000 ]        = {0} ;                          // Tag id
char str_tag_data[ 2000 ]      = {0} ;                          // Tag data
int  i_tag_data_len            = 0 ;                            // Length of the tag data

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
char *p_population = "" ;                                       // Population name
char *p_encoding   = "" ;                                       // Encoding datatype
int   p_uni_enc    = 0 ;                                        // Parameter Unicode encoding either 4 , 6 or 8
char *p_nm_fmt     = "" ;                                       // Parameter Name format L or R
char *p_delimiter  = "" ;                                       // Parameter Delimiter
char *p_infdir     = "" ;                                       // Parameter input file name
char *p_outfdir    = "" ;                                       // Parameter output file directory
char *p_logfdir    = "" ;                                       // Parameter log file directory
int   p_multiplier = 0 ;                                        // Parameter Multiplier
int   i_multiplier = 0 ;                                        // Multiplier multiplier variable

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

int i_PN_ky_rng = 0 ;                                           // Person_Name Narrow key ranges count
int i_PY_ky_rng = 0 ;                                           // Person_Name Typical key ranges count
int i_PH_ky_rng = 0 ;                                           // Person_Name Exhaustive key ranges count
int i_PR_ky_rng = 0 ;                                           // Person_Name Extreme key ranges count

int i_ON_ky_rng = 0 ;                                           // Organization_Name Narrow key ranges count
int i_OY_ky_rng = 0 ;                                           // Organization_Name Typical key ranges count
int i_OH_ky_rng = 0 ;                                           // Organization_Name Exhaustive key ranges count
int i_OR_ky_rng = 0 ;                                           // Organization_Name Extreme key ranges count

int i_1N_ky_rng = 0 ;                                           // Address_Part1 Narrow key ranges count
int i_1Y_ky_rng = 0 ;                                           // Address_Part1 Typical key ranges count
int i_1H_ky_rng = 0 ;                                           // Address_Part1 Exhaustive key ranges count
int i_1R_ky_rng = 0 ;                                           // Address_Part1 Extreme key ranges count

int i_GN_ky_rng = 0 ;                                           // Sex_Code Narrow key ranges count
int i_GY_ky_rng = 0 ;                                           // Sex_Code Typical key ranges count
int i_GH_ky_rng = 0 ;                                           // Sex_Code Exhaustive key ranges count
int i_GR_ky_rng = 0 ;                                           // Sex_Code Extreme key ranges count

int i_Person_Name_key_ranges  = 0 ;                             // Person_Name Narrow , Typical , Exhaustive ,Extreme key ranges count
int i_Organ_Name_key_ranges   = 0 ;                             // Organization_Name Narrow , Typical , Exhaustive ,Extreme key ranges count
int i_Add_Part1_key_ranges    = 0 ;                             // Address_Part1 Narrow , Typical , Exhaustive ,Extreme key ranges count
int i_sex_code_key_ranges     = 0 ;                             // Sex_Code Narrow , Typical , Exhaustive ,Extreme key ranges count

int i_total_ky_ranges_written = 0 ;                             // Total key ranges written
int i_addition_key      = 0 ;                                   // Addition of all key ranges
int i_rec_number        = 0 ;                                   // Record counter

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

char *str_system_nm_d   = "" ;                                  // System name
char *str_popln_nm_d    = "" ;                                  // Population name
char *str_encoding_d    = "" ;                                  // Encoding datatype

// Control related variables
char a_uni_enc[19]   = {0} ;                                    // Unicode encoding format e.g UNICODE=4
char a_nm_fmt[13]    = {0} ;                                    // Name Format e.g NAMEFORMAT=R
char a_delimeter[100]= {0} ;                                    // Delimiter in Tagged data

char a_ctrl_pn[300]   = {0} ;                                   // Control with Person_Name Narrow search level
char a_ctrl_py[300]   = {0} ;                                   // Control with Person_Name Typical search level
char a_ctrl_ph[300]   = {0} ;                                   // Control with Person_Name Exhaustive search level
char a_ctrl_pr[300]   = {0} ;                                   // Control with Person_Name Extreme search level

char a_ctrl_on[300]   = {0} ;                                   // Control with Organization_Name Narrow search level
char a_ctrl_oy[300]   = {0} ;                                   // Control with Organization_Name Typical search level
char a_ctrl_oh[300]   = {0} ;                                   // Control with Organization_Name Exhaustive search level
char a_ctrl_or[300]   = {0} ;                                   // Control with Organization_Name Extreme search level

char a_ctrl_an[300]   = {0} ;                                   // Control with Address_Part1 Narrow search level
char a_ctrl_ay[300]   = {0} ;                                   // Control with Address_Part1 Typical search level
char a_ctrl_ah[300]   = {0} ;                                   // Control with Address_Part1 Exhaustive search level
char a_ctrl_ar[300]   = {0} ;                                   // Control with Address_Part1 Extreme search level

char a_ctrl_sn[300]   = {0} ;                                   // Control with Sex_Code Narrow search level
char a_ctrl_sy[300]   = {0} ;                                   // Control with Sex_Code Typical search level
char a_ctrl_sh[300]   = {0} ;                                   // Control with Sex_Code Exhaustive search level
char a_ctrl_sr[300]   = {0} ;                                   // Control with Sex_Code Extreme search level

char *S_K_FLD         = "FIELD=" ;                              // Field format
char *S_K_FLD_PN      = "Person_Name" ;                         // Field Person_Name
char *S_K_FLD_ON      = "Organization_Name" ;                   // Field Organization_Name
char *S_K_FLD_ADP1    = "Address_Part1" ;                       // Field Address_Part1
char *S_K_FLD_SEXC    = "Sex_Code" ;                            // Field Sex_Code
char *S_K_SRCLVL_N    = " SEARCH_LEVEL=Narrow" ;                // SEARCH_LEVEL=Narrow
char *S_K_SRCLVL_Y    = " SEARCH_LEVEL=Typical" ;               // SEARCH_LEVEL=Typical
char *S_K_SRCLVL_H    = " SEARCH_LEVEL=Exhaustive" ;            // SEARCH_LEVEL=Exhaustive
char *S_K_SRCLVL_R    = " SEARCH_LEVEL=Extreme" ;               // SEARCH_LEVEL=Extreme
char *S_K_UNI_ENC     = " UNICODE_ENCODING=" ;                  // UNICODE_ENCODING= format
int   i_uni_enc_d     = 0 ;                                     // Unicode encoding data
char *S_K_NM_FMT      = " NAMEFORMAT=";                         // NAMEFORMAT= format
char *str_nm_fmt_d    = "" ;                                    // Name format data
char *S_K_delimeter   = " DELIMITER=" ;                         // DELIMITER= format
char *str_delimeter_d = "" ;                                    // Delimiter data
char a_Id[10]         = {0} ;                                   // Id with <DELIMITER>Id<DELIMITER>

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
    "44_505_MkeRng -d <DATA_SET> -r <RUN> -s <system-name> -p <population> -c <encoding_datatype>"
    " -e <unicode_encoding> -n <name_format> -t <delimiter>"
    " -i <input_file_directory> -o <output_file_directory> -l <log_file_directory> -m <Multiplier> -v(erbose)\n\n"
    "MANDATORY VALUES IN CAPITALS\n\nExample:\n\n"
    "44_505_MkeRng -d 100 -r 1000 -s default -p india -c TEXT -e 4 -n L"
    " -i E:/SurendraK/Work/DeDupeProcs/Input/ -o E:/SurendraK/Work/DeDupeProcs/Output/"
    " -l E:/SurendraK/Work/SSAProcs/Log/ -m 10000 -v\n"
   ) ;

}
/**********************************************************************
 End of subroutine s_print_usage                                      *
**********************************************************************/

static void s_getParameter ( int argc , char *argv[] ) {
// This subroutine is default parameter of getopt in s_getParameter

  s_date_time ( ) ;                                             // Call subroutine s_date_time
  while ( ( i_option = getopt ( argc , argv , "d:r:s:p:e:n:t:i:o:l:m:v::" ) ) != -1 ) {
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
      case 'c' :                                                // Encoding datatype parameter
        p_encoding   = optarg ;
        break ;
      case 'e' :                                                // Unicode encoding parameter
        p_uni_enc    = atoi( optarg ) ;
        break ;
      case 'n' :                                                // Name Format parameter
        p_nm_fmt     = optarg ;
        break ;
      case 't' :                                                // Delimiter parameter
        p_delimiter  = optarg ;
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

  str_system_nm_d   = p_system_nm ;                             // Initialize system name in another variable
  str_popln_nm_d    = p_population ;                            // Initialize population name in another variable
  str_encoding_d    = p_encoding ;                              // Initialize encoding datatype in another variable
  i_uni_enc_d       = p_uni_enc ;                               // Initialize unicode_Encoding data in another variable
  str_nm_fmt_d      = p_nm_fmt ;                                // Initialize name format data in another variable
  str_delimeter_d   = p_delimiter ;                             // Initialize delimiter data in another variable
  i_multiplier      = p_multiplier ;                            // Multiplier value

  if ( !*p_system_nm ) {                                        // Abort if System name is empty
    str_system_nm_d = "default" ;                               // Default value is default
  }

  if ( !*p_population ) {                                       // Abort if Population is empty
    str_popln_nm_d = "india" ;                                  // Default value is india
  }

  if ( !*p_encoding ) {                                         // Abort if Encoding is empty
    str_encoding_d = "TEXT" ;                                   // Default value is TEXT
  }

  if ( p_multiplier == 0 ) {
    i_multiplier = 100000 ;                                     // Default multiplier number
  }

  // Abort if unknown encoding datatype entered
  if ( *p_encoding ) {
    if ( strcmp ( p_encoding , "TEXT" ) != 0 &&
         strcmp ( p_encoding , "UTF-8" ) != 0 &&
         strcmp ( p_encoding , "UTF-16" ) != 0 &&
         strcmp ( p_encoding , "UTF-16LE" ) != 0 &&
         strcmp ( p_encoding , "UTF-16BE" ) != 0 &&
         strcmp ( p_encoding , "UTF-32" ) != 0 &&
         strcmp ( p_encoding , "UCS-2" ) != 0 &&
         strcmp ( p_encoding , "UCS-4" ) != 0 ) {
      printf ("%s","JOB ABANDONDED - UNKNOWN ENCODING DATATYPE\n" ) ;
      exit(1) ;
    }
  }

  if ( i_uni_enc_d != 0 ) {                                     // If unicode encoding is not zero
    if ( i_uni_enc_d != 4 &&                                    // Unicode Encoding must be 4 , 6 and 8
         i_uni_enc_d != 6 &&
         i_uni_enc_d != 8 ) {
      printf ( "%s" , "JOB ABANDONDED - INVALID CHARACTER ENCODING VALUE\n" ) ;
      exit(1);
    }
  }

  if ( *str_nm_fmt_d ) {                                        // If Name Format string is not empty
    if ( ( strcmp ( str_nm_fmt_d , "L" ) != 0 ) &&              // Name Format must be L or R
         ( strcmp ( str_nm_fmt_d , "R" ) != 0 ) ) {
      printf ( "%s" , "JOB ABANDONDED - INVALID NAMEFORMAT VALUE - MUST BE L OR R\n" ) ;
      exit(1);
    }
  }

  if ( i_uni_enc_d != 0 ) {                                     // If Unicode Encoding value is not zero
    sprintf ( a_uni_enc , "%s%d" , S_K_UNI_ENC , i_uni_enc_d ) ;
  }

  if ( *str_nm_fmt_d ) {                                        // If NAMEFORMAT value is not empty
    sprintf ( a_nm_fmt , "%s%s" , S_K_NM_FMT , str_nm_fmt_d ) ;
  }

  if ( !*p_delimiter ) {                                        // If delimeter parameter is empty
    str_delimeter_d = "*" ;
    sprintf ( a_delimeter , "%s%s" , S_K_delimeter , str_delimeter_d ) ;
  }
  else {
    sprintf ( a_delimeter , "%s%s" , S_K_delimeter , p_delimiter ) ;
  }

  // Data set number should be in a range of 100 to 999
  if( p_data_set > 999 || p_data_set < 100 ) {
    printf("%s","JOB ABANDONDED - DATA SET NUMBER SHOULD BE INTEGER AND IN A RANGE OF 100 TO 999" ) ;
    exit(1) ;
  }

  // Run time number should be in a range of 1000 to 9999
  if( p_run_time > 9999 || p_run_time < 1000 ) {
    printf("%s","JOB ABANDONDED - RUN TIME NUMBER SHOULD BE INTEGER AND IN A RANGE OF 1000 TO 9999" ) ;
    exit(1) ;
  }

  if ( strlen(p_delimiter) > 1 ) {
    printf ("%s","JOB ABANDONDED - DELIMITER MUST BE ONE CHARACTER" ) ;
    exit(1) ;
  }

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
  sprintf( a_str_output_file , "%d%d.rnf" , p_data_set , p_run_time ) ;
  sprintf( a_str_log_file , "%d%d_MkeRng_%d-%s-%s-%s-%s-%s.log", p_data_set, p_run_time, i_YYYY, a_str_MM, a_str_DD, a_str_HH24, a_str_MI, a_str_SS ) ;

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

  fprintf ( f_log_fopen_status, "------ MkeRng EXECUTION START DATE AND TIME ------\n" ) ;
  fprintf ( f_log_fopen_status, "%d-%s-%s %s:%s:%s \n\n", i_YYYY, a_str_MM, a_str_DD, a_str_HH24, a_str_MI, a_str_SS ) ;

}
/**********************************************************************
 End of subroutine s_getParameter                                     *
**********************************************************************/

static long s_test_dds_open (
// To open a session to the dds-name3 service

  long    l_sockh ,                                             // Set to -1 as not calling the dds-name3 server
  long    *l_session_id ,                                       // Should be -1 on the ddsn3 open call ,or opening a new session
  char    *str_sysName ,                                        // Default
  char    *str_population ,                                     // Country name
  char    *str_controls                                         // Controls
) {

  char    a_str_rsp_code[ SSA_SI_RSP_SZ ] ;                     // Indicates the success or failure of a call to dds-name3
  char    a_str_dds_msg[ SSA_SI_SSA_MSG_SZ ] ;                  // Error Message

  //ssan3_open: This function opens and initiates an dds-name3 session in preparation for using further API function
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

  l_num = 0 ;                                                   // No of key ranges count

  /*ssan3_get_ranges_encoded: Used to ge dds-name3 key ranges for name or address which the application
     program will use in a Selct statement to retrieve records from the dds-name3 key table.*/
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
    &l_num ,                                                    // Key ranges count
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
      i_PN_ky_rng ++ ;                                          // Person_Name Narrow key ranges count
      i_Person_Name_key_ranges ++ ;                             // Number of Person_Name key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "PY") == 0 ) {   // Compare PY with abbreviation of key field and search level
      i_PY_ky_rng ++ ;                                          // Person_Name Typical key ranges count
      i_Person_Name_key_ranges ++ ;                             // Number of Person_Name key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "PH") == 0 ) {   // Compare PH with abbreviation of key field and search level
      i_PH_ky_rng ++ ;                                          // Person_Name Exhaustive key ranges count
      i_Person_Name_key_ranges ++ ;                             // Number of Person_Name key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "PR") == 0 ) {   // Compare PR with abbreviation of key field and search level
      i_PR_ky_rng ++ ;                                          // Person_Name Extreme key ranges count
      i_Person_Name_key_ranges ++ ;                             // Number of Person_Name key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "ON") == 0 ) {   // Compare ON with abbreviation of key field and search level
      i_ON_ky_rng ++ ;                                          // Organization_Name Narrow key ranges count
      i_Organ_Name_key_ranges ++ ;                              // Number of Organization_Name key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "OY") == 0 ) {   // Compare OY with abbreviation of key field and search level
      i_OY_ky_rng ++ ;                                          // Organization_Name Typical key ranges count
      i_Organ_Name_key_ranges ++ ;                              // Number of Organization_Name key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "OH") == 0 ) {   // Compare OH with abbreviation of key field and search level
      i_OH_ky_rng ++ ;                                          // Organization_Name Exhaustive key ranges count
      i_Organ_Name_key_ranges ++ ;                              // Number of Organization_Name key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "OR") == 0 ) {   // Compare OR with abbreviation of key field and search level
      i_OR_ky_rng ++ ;                                          // Organization_Name Extreme key ranges count
      i_Organ_Name_key_ranges ++ ;                              // Number of Organization_Name key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "1N") == 0 ) {   // Compare 1N with abbreviation of key field and search level
      i_1N_ky_rng ++ ;                                          // Address_Part1 Narrow key ranges count
      i_Add_Part1_key_ranges ++ ;                               // Number of Address_Part1 key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "1Y") == 0 ) {   // Compare 1Y with abbreviation of key field and search level
      i_1Y_ky_rng ++ ;                                          // Address_Part1 Typical key ranges count
      i_Add_Part1_key_ranges ++ ;                               // Number of Address_Part1 key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "1H") == 0 ) {   // Compare 1H with abbreviation of key field and search level
      i_1H_ky_rng ++ ;                                          // Address_Part1 Exhaustive key ranges count
      i_Add_Part1_key_ranges ++ ;                               // Number of Address_Part1 key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "1R") == 0 ) {   // Compare 1R with abbreviation of key field and search level
      i_1R_ky_rng ++ ;                                          // Address_Part1 Extreme key ranges count
      i_Add_Part1_key_ranges ++ ;                               // Number of Address_Part1 key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "GN") == 0 ) {   // Compare GN with abbreviation of key field and search level
      i_GN_ky_rng ++ ;                                          // Sex_Code Narrow key ranges count
      i_sex_code_key_ranges ++ ;                                // Number of Sex_Code key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "GY") == 0 ) {   // Compare GY with abbreviation of key field and search level
      i_GY_ky_rng ++ ;                                          // Sex_Code Typical key ranges count
      i_sex_code_key_ranges ++ ;                                // Number of Sex_Code key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "GH") == 0 ) {   // Compare GH with abbreviation of key field and search level
      i_GH_ky_rng ++ ;                                          // Sex_Code Exhaustive key ranges count
      i_sex_code_key_ranges ++ ;                                // Number of Sex_Code key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
    }
    else if ( strcmp ( str_ky_fld_search_lvl , "GR") == 0 ) {   // Compare GR with abbreviation of key field and search level
      i_GR_ky_rng ++ ;                                          // Sex_Code Extreme key ranges count
      i_sex_code_key_ranges ++ ;                                // Number of Sex_Code key ranges count
      i_total_ky_ranges_written ++ ;                            // Total key ranges written
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
// Close the previously open session

  long    l_sockh ,                                             // Set to -1 as not calling the SSA-name3 server
  long    *l_session_id ,                                       // Should be -1 on the ddsn3 open call ,or opening a new session
  char    *str_sysName ,                                        // Defines location of the population rules
  char    *str_population ,                                     // Country name
  char    *str_controls                                         // Format fo key field and search level
) {

  long    l_rc ;                                                // Indicate success or failure of open / close sesions
  char    a_str_rsp_code[ SSA_SI_RSP_SZ ] ;                     // Indicates the success or failure of a call to dds-name3
  char    a_str_dds_msg[ SSA_SI_SSA_MSG_SZ ] ;                  // Error Message

  s_date_time ( ) ;                                             // Call date and time subroutine

  /* ddsn3_close
     Close the dds-name3 session and releases memory.
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
/* Error occurred in subroutines s_test_dds_open, s_test_dds_get_ranges or
  s_test_dds_close */

  fprintf ( f_log_fopen_status , "Error occurred in '%s'\n" , func ) ;
  printf ( "Error occurred in '%s'\n" , func ) ;
  exit ( 1 ) ;

}
/**********************************************************************
 End of subroutine s_doExit                                           *
**********************************************************************/

static void s_MkeRng_open ( ) {
/* s_test_dds_open subroutine called in s_mkeKey_open subroutine and
   assign subroutine parameters */

  l_rc = s_test_dds_open                                        // Open session
  (
    l_sockh ,                                                   // Set to -1 as not calling the SSA-name3 server
    &l_session_id ,                                             // Should be -1 on the ddsn3 open call ,or opening a new session
    ( !*p_system_nm ? str_system_nm_d : p_system_nm ) ,         // System name parameter is empty used default value
    ( !*p_population ? str_popln_nm_d : p_population ) ,        // Population parameter is empty used default value
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
from the dds-name3 key table*/

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

  // Array contain abbrevation of key field and search level
  char *ky_fld_search_lvl [ 4 ] = { ky_fld_search_lvl1 , ky_fld_search_lvl2 , ky_fld_search_lvl3 , ky_fld_search_lvl4 } ;

  for( i_idx = 0 ; i_idx <= 3 ; i_idx++ ) {
    l_rc = s_test_dds_get_ranges                                // Call subroutine s_test_dds_get_ranges
    (
      l_sockh ,                                                 // Set to -1 as not calling the SSA-name3 server
      &l_session_id ,                                           // Should be -1 on the ddsn3 open call ,or opening a new session
      ( !*p_system_nm ? str_system_nm_d : p_system_nm ) ,       // System name parameter is empty used default value
      ( !*p_population ? str_popln_nm_d : p_population ) ,      // Population parameter is empty used default value
      SEARCH_LEVEL[i_idx] ,                                     // Search level
      str_tag_data ,                                            // Current tag data
      i_tag_data_len ,                                          // Length of a current tag data
      ( !*p_encoding ? str_encoding_d : p_encoding ) ,          // Encoding data type of tag data
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
    &l_session_id ,                                             // Should be -1 on the ddsn3 open call ,or opening a new session
    ( !*p_system_nm ? str_system_nm_d : p_system_nm ) ,         // System name parameter is empty used default value
    ( !*p_population ? str_popln_nm_d : p_population ) ,        // Population parameter is empty used default value
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

t_start_time = clock( ) ;                                       // Start time
s_getParameter ( argc , argv ) ;                                // Subroutine to get parameter

if ( *str_delimeter_d ) {                                       // If delimeter parameter is not empty
  sprintf ( a_Id ,"%sId%s", str_delimeter_d , str_delimeter_d ) ;
}

fprintf ( f_log_fopen_status, "------ Run Parameters ------\n" ) ;
if ( p_data_set != 0 ) {                                        // If data set number is not empty
  fprintf ( f_log_fopen_status , "Data set number       : %d\n" , p_data_set ) ;
}

if ( p_run_time != 0 ) {                                        // If run time number is not empty
  fprintf ( f_log_fopen_status , "Run time number       : %d\n" , p_run_time ) ;
}

if ( !*p_system_nm ) {                                          // If System name is non empty
  fprintf ( f_log_fopen_status , "System name           : Missing- Default:%s\n", str_system_nm_d ) ;
}
else {
  fprintf ( f_log_fopen_status , "System name           : %s\n" , p_system_nm ) ;
}

if ( !*p_population ) {                                         // If Population is non empty
  fprintf ( f_log_fopen_status , "Population            : Missing- Default:%s\n", str_popln_nm_d ) ;
}
else {
  fprintf ( f_log_fopen_status , "Population            : %s\n" , p_population ) ;
}

if ( !*p_encoding ) {                                           // If Encoding datatype is non empty
  fprintf ( f_log_fopen_status , "Encoding datatype     : Missing- Default:%s\n", str_encoding_d ) ;
}
else {
  fprintf ( f_log_fopen_status , "Encoding datatype     : %s\n" , p_encoding ) ;
}

if ( *p_infdir ) {                                              // If Input file directory is non empty
  fprintf ( f_log_fopen_status , "Input File Directory  : %s\n" , p_infdir ) ;
}

if ( *p_outfdir ) {                                             // If Output file directory is non empty
  fprintf ( f_log_fopen_status , "Output File Directory : %s\n" , p_outfdir ) ;
}

if ( *p_logfdir ) {                                             // If Log file directory is non empty
  fprintf ( f_log_fopen_status , "Log File Directory    : %s\n" , p_logfdir ) ;
}

if ( !*p_delimiter ) {                                          // If delimiter parameter is empty
  fprintf ( f_log_fopen_status , "Delimiter             : Missing- Default:%s\n" , str_delimeter_d ) ;
}
else {
  fprintf ( f_log_fopen_status , "Delimiter             : %s\n" , p_delimiter ) ;
}

if ( i_uni_enc_d != 0 ) {                                       // If unicode encoding parameter is not empty
  fprintf ( f_log_fopen_status , "Unicode encoding      : %d\n" , i_uni_enc_d ) ;
}

if ( *str_nm_fmt_d ) {                                          // If name format parameter is not empty
  fprintf ( f_log_fopen_status , "Name format           : %s\n" , str_nm_fmt_d ) ;
}

// If verbose flag is on then only multiplier value will be display in log file
if ( i_verbose_flg == 1 ) {
  if ( p_multiplier == 0 ) {
    fprintf ( f_log_fopen_status , "Multiplier            : Missing- Default:%d\n" , i_multiplier ) ;
  }
  else {
    fprintf ( f_log_fopen_status , "Multiplier            : %d\n" , p_multiplier ) ;
  }
}

fprintf ( f_log_fopen_status ,   "Verbose               : %s\n" , ( i_verbose_flg == 1 ? "Yes" : "No" ) ) ;

fprintf ( f_log_fopen_status , "\n------ File Names ------" ) ;
fprintf ( f_log_fopen_status , "\nInput file name       : %s", a_str_input_file ) ;
fprintf ( f_log_fopen_status , "\nOutput file name      : %s", a_str_output_file ) ;
fprintf ( f_log_fopen_status , "\nLog file name         : %s\n", a_str_log_file ) ;

fprintf ( f_log_fopen_status , "\n------ Environment variable ------" ) ;
fprintf ( f_log_fopen_status , "\nSSATOP : %s" , getenv("SSATOP") ) ;
fprintf ( f_log_fopen_status , "\nSSAPR  : %s\n" , getenv("SSAPR") ) ;

s_MkeRng_open ( ) ;                                             // Call S_MkeRng_open

// Read a input file line by line
while( fgets ( str_current_rec , sizeof ( str_current_rec ) , f_input_fopen_status ) ) {

  ++i_rec_number ;                                              // Record count

  // Tab delimited split
  sscanf( str_current_rec , "%s\t%[^\n]", str_tag_id , str_tag_data );

  // Calculate the length of the current tag data
  i_tag_data_len = strlen( str_tag_data ) ;                     // Length of the current tag data
  if ( i_tag_data_len > 0 && str_tag_data[i_tag_data_len-1] == '\n' ) {
    str_tag_data[--i_tag_data_len] = '\0' ;
  }

  i_record_read++ ;                                             // No of records read

  if ( strstr ( str_current_rec , a_Id ) != NULL ) {            // Check current tag data contain *Id*

    if ( i_verbose_flg == 1 ) {                                 // If Verbose flag is On

      if ( i_rec_number == i_multiplier ) {                     // If Records number equals Multiplier number
        t_end_time = clock( ) - t_start_time ;                  // End time
        t_time_taken = ( ( double )t_end_time )/CLOCKS_PER_SEC ;              // In seconds
        printf( "\nDisplay %d records in %.f seconds to execute \n", i_multiplier , t_time_taken ) ;      // Print time

        i_multiplier = i_multiplier * 2 ;                       // Multiplier value multiply by 2
      }
    }

    // IF Person Name , Organization Name and Address_Part1 are empty throw an error message
    if ( strstr ( str_tag_data , "Person_Name" ) == NULL &&
         strstr ( str_tag_data , "Organization_Name" ) == NULL &&
         strstr ( str_tag_data , "Address_Part1" ) == NULL ) {
      i_error_record_read ++ ;                                  // Error record count
      i_error_record_flds ++ ;                                  // Missing Person Name, Organization name and Address Part 1 fields records count
      fprintf ( f_log_fopen_status, "\nRecord no : %d Error Message : %s", i_rec_number ,"Missing Person Name, Organization name, Address Part 1 fields" ) ;
      fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_tag_data ) ;
    }

    // Check Person_Name is inside the current record
    if ( strstr ( str_tag_data , "Person_Name" ) != NULL ) {

      char *abv_PNarr  = "PN" ;                                 // abrevation of Person_Name Narrow
      char *abv_PTyp   = "PY" ;                                 // abrevation of Person_Name Typical
      char *abv_PExh   = "PH" ;                                 // abrevation of Person_Name Exhaustive
      char *abv_PExtr  = "PR" ;                                 // abrevation of Person_Name Extreme

      sprintf
      (
        a_ctrl_pn ,                                             // Control with Person_Name Narrow search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_PN ,                                            // Field Person_Name
        S_K_SRCLVL_N ,                                          // SEARCH_LEVEL=Narrow
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      sprintf
      (
        a_ctrl_py ,                                             // Control with Person_Name Typical search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_PN ,                                            // Field Person_Name
        S_K_SRCLVL_Y ,                                          // SEARCH_LEVEL=Typical
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      sprintf
      (
        a_ctrl_ph ,                                             // Control with Person_Name Exhaustive search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_PN ,                                            // Field Person_Name
        S_K_SRCLVL_H ,                                          // SEARCH_LEVEL=Exhaustive
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      sprintf
      (
        a_ctrl_pr ,                                             // Control with Person_Name Extreme search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_PN ,                                            // Field Person_Name
        S_K_SRCLVL_R ,                                          // SEARCH_LEVEL=Extreme
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      i_pn_records++ ;                                          // No of records count that contains Person_Name

      s_MkeRng_ranges                                           // Call s_MkeRng_ranges
      (
        a_ctrl_pn ,
        a_ctrl_py ,
        a_ctrl_ph ,
        a_ctrl_pr ,
        abv_PNarr ,
        abv_PTyp ,
        abv_PExh ,
        abv_PExtr ,
        str_tag_id
      ) ;
    }

    // Check Organization_Name is inside the current record
    if ( strstr ( str_tag_data , "Organization_Name" ) != NULL ) {

      char *abv_ONarr  = "ON" ;                                 // abrevation of Organization_Name Narrow
      char *abv_OTyp   = "OY" ;                                 // abrevation of Organization_Name Typical
      char *abv_OExh   = "OH" ;                                 // abrevation of Organization_Name Exhaustive
      char *abv_OExtr  = "OR" ;                                 // abrevation of Organization_Name Extreme

      sprintf
      (
        a_ctrl_on ,                                             // Control with Organization_Name Narrow search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_ON ,                                            // Field Organization_Name
        S_K_SRCLVL_N ,                                          // SEARCH_LEVEL=Narrow
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      sprintf
      (
        a_ctrl_oy ,                                             // Control with Organization_Name Typical search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_ON ,                                            // Field Organization_Name
        S_K_SRCLVL_Y ,                                          // SEARCH_LEVEL=Typical
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      sprintf
      (
        a_ctrl_oh ,                                             // Control with Organization_Name Exhaustive search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_ON ,                                            // Field Organization_Name
        S_K_SRCLVL_H ,                                          // SEARCH_LEVEL=Exhaustive
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      sprintf
      (
        a_ctrl_or ,                                             // Control with Organization_Name Extreme search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_ON ,                                            // Field Organization_Name
        S_K_SRCLVL_R ,                                          // SEARCH_LEVEL=Extreme
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      i_on_records++ ;                                          // No of records count that contains Organization_Name

      s_MkeRng_ranges                                           // Call s_MkeRng_ranges
      (
        a_ctrl_on ,
        a_ctrl_oy ,
        a_ctrl_oh ,
        a_ctrl_or ,
        abv_ONarr ,
        abv_OTyp ,
        abv_OExh ,
        abv_OExtr ,
        str_tag_id
      ) ;
    }

    // Check Address_Part1 is inside the current record
    if ( strstr ( str_tag_data , "Address_Part1" ) != NULL ) {

      char *abv_ANarr  = "1N" ;                                 // abrevation of Address_Part1 Narrow
      char *abv_ATyp   = "1Y" ;                                 // abrevation of Address_Part1 Typical
      char *abv_AExh   = "1H" ;                                 // abrevation of Address_Part1 Exhaustive
      char *abv_AExtr  = "1R" ;                                 // abrevation of Address_Part1 Extreme

      sprintf
      (
        a_ctrl_an ,                                             // Control with Address_Part1 Narrow search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_ADP1 ,                                          // Field Address_Part1
        S_K_SRCLVL_N ,                                          // SEARCH_LEVEL=Narrow
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      sprintf
      (
        a_ctrl_ay ,                                             // Control with Address_Part1 Typical search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_ADP1 ,                                          // Field Address_Part1
        S_K_SRCLVL_Y ,                                          // SEARCH_LEVEL=Typical
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      sprintf
      (
        a_ctrl_ah ,                                             // Control with Address_Part1 Exhaustive search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_ADP1 ,                                          // Field Address_Part1
        S_K_SRCLVL_H ,                                          // SEARCH_LEVEL=Exhaustive
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      sprintf
      (
        a_ctrl_ar ,                                             // Control with Address_Part1 Extreme search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_ADP1 ,                                          // Field Address_Part1
        S_K_SRCLVL_R ,                                          // SEARCH_LEVEL=Extreme
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      i_addp1_records++ ;                                       // No of records count that contains Address_Part1

      s_MkeRng_ranges                                           // Call s_MkeRng_ranges
      (
        a_ctrl_an ,
        a_ctrl_ay ,
        a_ctrl_ah ,
        a_ctrl_ar ,
        abv_ANarr ,
        abv_ATyp ,
        abv_AExh ,
        abv_AExtr ,
        str_tag_id
      ) ;
    }

    // Check Address_Part1 is inside the current record
    if ( strstr ( str_tag_data , "Sex_Code" ) != NULL ) {

      char *abv_GNarr  = "GN" ;                                 // abrevation of Address_Part1 Narrow
      char *abv_GTyp   = "GY" ;                                 // abrevation of Address_Part1 Typical
      char *abv_GExh   = "GH" ;                                 // abrevation of Address_Part1 Exhaustive
      char *abv_GExtr  = "GR" ;                                 // abrevation of Address_Part1 Extreme

      sprintf
      (
        a_ctrl_sn ,                                             // Control with Sex_Code Narrow search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_SEXC ,                                          // Field Sex_Code
        S_K_SRCLVL_N ,                                          // SEARCH_LEVEL=Narrow
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      sprintf
      (
        a_ctrl_sy ,                                             // Control with Sex_Code Typical search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_SEXC ,                                          // Field Sex_Code
        S_K_SRCLVL_Y ,                                          // SEARCH_LEVEL=Typical
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      sprintf
      (
        a_ctrl_sh ,                                             // Control with Sex_Code Exhaustive search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_SEXC ,                                          // Field Sex_Code
        S_K_SRCLVL_H ,                                          // SEARCH_LEVEL=Exhaustive
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      sprintf
      (
        a_ctrl_sr ,                                             // Control with Sex_Code Extreme search level
        "%s%s%s%s%s%s" ,
        S_K_FLD ,                                               // Format FIELD=
        S_K_FLD_SEXC ,                                          // Field Sex_Code
        S_K_SRCLVL_R ,                                          // SEARCH_LEVEL=Extreme
        a_uni_enc ,                                             // Unicode encoding format e.g UNICODE=4/6/8
        a_nm_fmt ,                                              // Name Format e.g NAMEFORMAT=L/R
        a_delimeter                                             // Delimiter
      ) ;

      i_sex_code_records++ ;                                    // No of records count that contains Address_Part1

      s_MkeRng_ranges                                           // Call s_MkeRng_ranges
      (
        a_ctrl_sn ,
        a_ctrl_sy ,
        a_ctrl_sh ,
        a_ctrl_sr ,
        abv_GNarr ,
        abv_GTyp ,
        abv_GExh ,
        abv_GExtr ,
        str_tag_id
      ) ;
    }
  }
  else {
    // If Id field is missing display error message
    i_error_record_read ++ ;                                    // Error record count
    i_error_record_id ++ ;                                      // Missing id

    fprintf ( f_log_fopen_status, "\nRecord no : %d Error Message : %s", i_rec_number ,"Missing Id field" ) ;
    fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_tag_data ) ;
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

  if ( i_sex_code_records != 0 ) {                              // If Records with Address_Part1 count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nRecords with Sex_Code          : %d\n", i_sex_code_records ) ;
  }

  if ( i_Person_Name_key_ranges != 0 ) {                        // If Person_Name key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nPerson_Name key ranges       : %d", i_Person_Name_key_ranges ) ;
  }

  if ( i_PN_ky_rng != 0 ) {                                     // If Person_Name Narrow key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Narrow     : %d", i_PN_ky_rng ) ;
  }

  if ( i_PY_ky_rng != 0 ) {                                     // If Person_Name Typical key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Typical    : %d",  i_PY_ky_rng ) ;
  }

  if ( i_PH_ky_rng != 0 ) {                                     // If Person_Name Exhaustive key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Exhaustive : %d",  i_PH_ky_rng ) ;
  }

  if ( i_PR_ky_rng != 0 ) {                                     // If Person_Name Extreme key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Extreme    : %d\n",  i_PR_ky_rng ) ;
  }

  if ( i_Organ_Name_key_ranges != 0 ) {                         // If Organization_Name key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nOrganization_Name key ranges : %d", i_Organ_Name_key_ranges ) ;
  }

  if ( i_ON_ky_rng != 0 ) {                                     // If Organization_Name Narrow key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Narrow     : %d", i_ON_ky_rng ) ;
  }

  if ( i_OY_ky_rng != 0 ) {                                     // If Organization_Name Typical key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Typical    : %d", i_OY_ky_rng ) ;
  }

  if ( i_OH_ky_rng != 0 ) {                                     // If Organization_Name Exhaustive key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Exhaustive : %d", i_OH_ky_rng ) ;
  }

  if ( i_OR_ky_rng != 0 ) {                                     // If Organization_Name Extreme key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Extreme    : %d\n", i_OR_ky_rng ) ;
  }

  if ( i_Add_Part1_key_ranges != 0 ) {                          // If Address_Part1 key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nAddress_Part1 key ranges     : %d", i_Add_Part1_key_ranges ) ;
  }

  if ( i_1N_ky_rng != 0 ) {                                     // If Address_Part1 Narrow key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Narrow     : %d", i_1N_ky_rng ) ;
  }

  if ( i_1Y_ky_rng != 0 ) {                                     // If Address_Part1 Typical key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Typical    : %d", i_1Y_ky_rng ) ;
  }

  if ( i_1H_ky_rng != 0 ) {                                     // If Address_Part1 Exhaustive key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Exhaustive : %d", i_1H_ky_rng ) ;
  }

  if ( i_1R_ky_rng != 0 ) {                                     // If Address_Part1 Extreme key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Extreme    : %d\n", i_1R_ky_rng ) ;
  }

  if ( i_sex_code_key_ranges != 0 ) {                           // If Address_Part1 key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nSex_Code key ranges          : %d", i_sex_code_key_ranges ) ;
  }

  if ( i_GN_ky_rng != 0 ) {                                     // If Address_Part1 Narrow key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Narrow     : %d", i_GN_ky_rng ) ;
  }

  if ( i_GY_ky_rng != 0 ) {                                     // If Address_Part1 Typical key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Typical    : %d", i_GY_ky_rng ) ;
  }

  if ( i_GH_ky_rng != 0 ) {                                     // If Address_Part1 Exhaustive key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Exhaustive : %d", i_GH_ky_rng ) ;
  }

  if ( i_GR_ky_rng != 0 ) {                                     // If Address_Part1 Extreme key ranges count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\n -Extreme    : %d\n", i_GR_ky_rng ) ;
  }

  if ( i_total_ky_ranges_written != 0 ) {                       // If Total key ranges written count non zero then only it will write on a file
    fprintf ( f_log_fopen_status, "\nTotal key ranges written     : %d\n", i_total_ky_ranges_written ) ;
  }

  // Addition of all the key ranges
  i_addition_key =
                   i_PN_ky_rng + i_PY_ky_rng + i_PH_ky_rng + i_PR_ky_rng +
                   i_ON_ky_rng + i_OY_ky_rng + i_OH_ky_rng + i_OR_ky_rng +
                   i_1N_ky_rng + i_1Y_ky_rng + i_1H_ky_rng + i_1R_ky_rng +
                   i_GN_ky_rng + i_GY_ky_rng + i_GH_ky_rng + i_GR_ky_rng ;

  if ( i_addition_key != i_total_ky_ranges_written ) {          // Addition of all the key ranges and check it is match with total key ranges
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
/**********************************************************************
 End of scrit MkeRng.c                                                *
**********************************************************************/

/*
  Make Range

1 Procedure Name

2 Copyright

3 Warnings

4 Error Messages

5 Format of input file

6 Format of output file

7 Format of log file

8 Technical

  8.1 Variables used

  8.2 Run Parameters

  8.3 Compile Procedure

  8.4 Execute procedure in different way

  8.5 Execution Start and End date and time

  8.6 Subroutines

      8.6.1 Called by

      8.6.2 Calling

      8.6.3 Subroutine Structure

9 Include Header

Make Range

  Generates ( Search ) key range from Tagged data for search levels
  Narrow (N) , Typical (Y) , Exhaustive (H) , Extreme (R) for
  Person_Name (P) , Organization_name (O) , Address_Part1 ,
  Sex_Code (G) as found in Tagged data

  SEARCH_LEVEL                 KEY_FIELD
  ---------                    ---------
  N|Narrow                     P|Person_Name
  Y|Typical                    O|Organization_Name
  H|Exhaustive                 1|Address_Part1
  R|Extreme                    G|Sex_Code

Procedure Name : 44_505_MkeRng.c

  Generate .rnf file ( Output file ) using .tag ( input file ) that
   contain tag id and tag data with tab delimited.

Copyright

  Copyright ( c ) 2017 IdentLogic Systems Private Limited

Warnings

  If your data can contain asterisks, make sure that these are either
   cleaned out prior to calling the dds-name3 functions , or use a different
   DELIMITER=setting.

  Length of the Input file directory, Output file directory and
   Log file directory should not exceed 1,000 bytes, with total
   filepath not exceeding 1011 , 1011 ,1038 resp. This is due
   to the length of the Input file name, Output file name and
   Log file name are 11, 11 and 38 resp.

  Length of the current record , tag id and tag data should not
   3000 , 1000 , 2000 resp.

  Increase the array size as per your convenience.

Error Messages

  JOB ABANDONDED - DATA SET NUMBER SHOULD BE INTEGER AND IN A RANGE OF 100 TO 999
  JOB ABANDONDED - RUN TIME NUMBER SHOULD BE INTEGER AND IN A RANGE OF 1000 TO 9999
  JOB ABANDONDED - UNKNOWN ENCODING DATATYPE
  JOB ABANDONDED - INVALID CHARACTER ENCODING VALUE
  JOB ABANDONDED - INVALID NAMEFORMAT VALUE - MUST BE L OR R
  JOB ABANDONDED - DELIMITER MUST BE ONE CHARACTER
  
Format of input file

  # FIELD    - sssrrrr.tag - TAB delimited
  - -----
  1 tag id
  2 tag data

Format of Output file - sssrrrr.rnf ( rnf stands for r(ange) n(ot applicable) f(ixed) )

  Column  1 to 8   : key ( From / Begin )
  Column  9 to 16  : key (  To  / End   )
  Column 17 to 17  : KEY FIELDS   - P ( Person_Name ) , O ( Organization_Name )
                                    1 ( Address_Part1 ) , G ( Sex_Code )
  Column 18 to 18  : SEARCH_LEVEL - N ( Narrow ) , Y ( Typical ) ,
                                    H ( Exhaustive ) , R ( Extreme )

Format of log file - sssrrrr_MkeRng_YYYY_MM_DD_HH24_MI_SS.log

  Log file will be created with data set number, run number ,procedure name
   and date time
  for eg. sssrrrr_MkeRng_YYYY_MM_DD_HH24_MI_SS.log

  Log file display only not null or non empty information.
  Those fields values are empty or not null it will not display in log file

  -d -r run parameters are mandatory

  If System name , Population name and Encoding datatype parameters are empty
  then it will take default value and write it on log file with
  message : Missing- Default:<default_value>

  If verbose flag is on then only multiplier value will be write it on log file
  If verbose flag is on and muliplier parameter is empty then it will take
  default value of multiplier which is 1 lakh.

  Log file name contain below information.

  ------ MkeRng EXECUTION START DATE AND TIME ------
  YYYY-MM-DD HH24:MI:SS

  ------ Run Parameters ------
  Data set no           : data set number starting from 100 to 999 <sss>
  Run time number       : Run time number starting from 1000 to 9999 <rrrr>
  System name           : Missing- Default:default / <SYSTEM_NAME>
  Population            : Missing- Default:india   / <POPULATION_NAME>
  Encoding datatype     : Missing- Default:TEXT    / <ENCODING_DATATYPE>
  Input File Directory  : Input File path
  Output File Directory : Output File path
  Log File Directory    : Log File path
  Delimiter             : Missing- Default:* / <DELIMITER>
  Unicode encoding      : <UNICODE_ENCODING>
  Name format           : <NAME_FORMAT>
  Multiplier            : Missing- Default:100000 / <MULTIPLIER>
  Verbose               : Yes/No

  ------ File Names ------
  Input file name       : <INPUT_FILE_NAME>
  Output file name      : <OUTPUT_FILE_NAME>
  Log file name         : <LOG_FILE_NAME>

  ------ Environment variable ------
  SSATOP : <PATH>
  SSAPR  : <PATH>

  Error message:Missing Person Name, Organization name, Address Part 1 fields

  If Person Name, Organization name and Address Part 1 fields is
  missing in the record then error will be display with
  record no with error message and record.

  Error message: Missing Id field

  If Id field is missing in the record then error will be display with
  record no with error message and record.

  ------Run summary------
  Tagged records read            : <COUNT>
  Error records                  : <COUNT>
   - Missing id                  : <COUNT>
   - Missing all 3 key fields    : <COUNT>

  Records with Person_Name       : <COUNT>
  Records with Organization_Name : <COUNT>
  Records with Address_Part1     : <COUNT>

  Person_Name key ranges       : <COUNT>
   -Narrow     : <COUNT>
   -Typical    : <COUNT>
   -Exhaustive : <COUNT>
   -Extreme    : <COUNT>

  Organization_Name key ranges : <COUNT>
   -Narrow     : <COUNT>
   -Typical    : <COUNT>
   -Exhaustive : <COUNT>
   -Extreme    : <COUNT>

  Address_Part1 key ranges     : <COUNT>
   -Narrow     : <COUNT>
   -Typical    : <COUNT>
   -Exhaustive : <COUNT>
   -Extreme    : <COUNT>
  
  Sex_Code key ranges          : <COUNT>
   -Narrow     : <COUNT>
   -Typical    : <COUNT>
   -Exhaustive : <COUNT>
   -Extreme    : <COUNT>

  Total key ranges written     : <COUNT>

  Ended YYYY-MM-DD HH24:MI:SS - HH:MM:SS to execute

  =================================================

  Note :
   Addition of all the key ranges and check it is match with total key ranges
   If Hours , Minutes and seconds are less than 10 prefix 0 will be added
   within a procedure.

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

 Package Number   - 44
 Procedure Number - 505
 Script name      - 44_505_MkeRng.c

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
 Encoding datatype        Encoding type of search data and file data
                           it must be any one of these TEXT , UTF-8,
                           UTF-16 , UTF-16LE , UTF-16BE , UTF-32 ,
                           UCS-2 , UCS-4                             c   p_encoding
 Delimiter                Default delimiter is *                     t   p_delimiter
 Unicode encoding         Unicode encoding must be either
                           4 , 6 ,or 8                               e   p_uni_enc
 Name format              Name format must be either L or R          n   p_nm_fmt
 Input File Directory     Input File Directory                       i   p_infdir
 Output File Directory    Output File Directory                      o   p_outfdir
 Log File Directory       Log File Directory                         l   p_logfdir
 Multiplier               Multiplier                                 m   p_multiplier
 Verbose                  Display information about multiplier       v   i_verbose_flg

 TimeStamp Variables   Description
 -------------------   -----------
   YYYY                  Year
   MM                    Month
   DD                    Date
   HH24                  Hours
   MI                    Minutes
   SS                    Seconds

 The extension of Input file name is .tag
 The extension of Output file name is .rnf ( rnf stands for r(ange) n(ot applicable) f(ixed) )
 The extension of Log file name is .log

 If Month, Date, Hours, Minutes, Seconds are less than 9 prefix 0 will be added to it.

 Compile procedure

  cl 44_505_MkeRng.c stssan3cl.lib

 Execute procedure

   -d -r parameters are mandatory

   1. 44_505_MkeRng -d 100 -r 1000

   2. 44_505_MkeRng -d 100 -r 1000 -s default -p india -c TEXT -i E:/ABC/EFG/HIJ/Input/
     -o E:/ABC/EFG/HIJ/Output/ -l E:/ABC/EFG/HIJ/Log/

   3. 44_505_MkeRng -d 100 -r 1000 -s default -p india -c TEXT -i E:\ABC\EFG\HIJ\Input\
     -o E:\ABC\EFG\HIJ\Output\ -l E:\ABC\EFG\HIJ\Log\

   4. 44_505_MkeRng -d 100 -r 1000 -s default -p india -c TEXT -i E:/ABC/EFG/HIJ/Input
     -o E:/ABC/EFG/HIJ/Output -l E:/ABC/EFG/HIJ/Log

   5. 44_505_MkeRng -d 100 -r 1000 -s default -p india -c TEXT -i E:\ABC\EFG\HIJ\Input
     -o E:\ABC\EFG\HIJ\Output -l E:\ABC\EFG\HIJ\Log -m 100000 -v

   6. 44_505_MkeRng -d 100 -r 1000 -s default -p india -c TEXT -i E:/ABC/EFG/HIJ/Input/

   7. 44_505_MkeRng -d 100 -r 1000 -s default -p india -c TEXT -m 100000 -v
   
   8. 44_505_MkeRng -d 100 -r 1000 -t x
   
   9. 44_505_MkeRng -d 100 -r 1000 -e 4 -n L -t @
   
  10. 44_505_MkeRng -d 100 -r 1000 -e 8 -n L

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
   
   DEFAULT VALUE OF SYSTEM NAME IS default
   DEFAULT VALUE OF POPULATION  IS india
   DEFAULT VALUE OF ENCODING DATATYPE IS TEXT
   DEFAULT VALUE OF DELIMITER  IS *
   DEFAULT VALUE OF MULTIPLIER  IS 100000

 Execution Start and End date and time

   In log file, also contain starting and ending execution time.
   Hours, Minutes and Seconds will be display with 2 digits.

 Subroutines

 Subroutine            Description
 ----------            -----------
 s_date_time           Compute current date and time elements YYYY, MM, DD, HH24, MI and SS
 s_print_usage         This subroutine is default parameter of getopt in s_getParameter
 s_getParameter        This subroutine takes run parameters and perform operations.
 s_test_dds_open       To open a session to the dds-NAME3 services
 s_test_dds_get_ranges To build key ranges on names or addresses
 s_test_dds_close      To close an open session to dds-NAME3
 s_doExit              Error occurred in subroutines s_test_dds_open, s_test_dds_get_ranges, s_test_dds_close
 s_MkeRng_open         s_test_dds_open subroutine called in s_MkeRng_open subroutine
 s_MkeRng_ranges       s_test_dds_get_ranges subroutine called in s_MkeRng_ranges subroutine
 s_MkeRng_close        s_test_dds_close subroutine called in s_MkeRng_close subroutine

 Called by

 Not indicated if only called by Main.

 Subroutine            Called by
 ----------            ---------
 s_test_dds_open        s_MkeRng_open
 s_test_dds_get_ranges  s_MkeRng_ranges
 s_test_dds_close       s_MkeRng_close
 s_doExit               s_MkeRng_open , s_MkeRng_ranges, s_MkeRng_close
 s_print_usage          s_getParameter
 s_date_time            s_getParameter , s_MkeRng_close

 Calling

 Subroutine           Calling Subroutine
 ----------           ------------------
 s_MkeRng_open        s_test_dds_open , s_doExit
 s_MkeRng_ranges      s_test_dds_get_ranges , s_doExit
 s_MkeRng_close       s_test_dds_close , s_doExit , s_date_time
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
  |----- s_MkeRng_open
  |           |
  |           |----- s_test_dds_open
  |           |
  |           \----- s_doExit
  |
  |----- s_MkeRng_ranges
  |           |
  |           |----- s_test_dds_get_ranges
  |           |
  |           \----- s_doExit
  |
  \----- s_MkeRng_close
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