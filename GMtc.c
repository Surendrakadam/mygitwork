/*
 Procedure     : GMtc.c
 Application   : De-Dupe
 Client        : Internal
 Copyright (c) : IdentLogic Systems Private Limited
 Author        : Surendra Kadam
 Creation Date : 5 April 2017
 Description   :

 WARNINGS      : If your data can contain asterisks, make sure that these are
                  either cleaned out prior to calling the dds-name3 functions ,
                  or use a different DELIMITER=setting.

                 Length of the Input file directory , Output file directory and
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
char *p_adjweight  = "" ;                                       // Parameter adjust value
int   p_adjwei_val = 0 ;
char *p_delimeter  = "" ;                                       // Parameter Delimiter
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

char a_str_MM[2]   = {0} ;                                      // Compute 2 digit Month
char a_str_DD[2]   = {0} ;                                      // Compute 2 digit Date
char a_str_HH24[2] = {0} ;                                      // Compute 2 digit Hours
char a_str_MI[2]   = {0} ;                                      // Compute 2 digit Minutes
char a_str_SS[2]   = {0} ;                                      // Compute 2 digit Seconds

char a_str_hour[2] = {0} ;                                      // Compute 2 digit Hours to execute records
char a_str_min[2]  = {0} ;                                      // Compute 2 digit Minutes to execute records
char a_str_sec[2]  = {0} ;                                      // Compute 2 digit Seconds to execute records

// Error
int i_no_fld_spr = 0 ;                                          // No field separator found in current record counter

// Controls related variables
char a_uni_enc[19]   = {0} ;                                    // Unicode encoding format e.g UNICODE=4
char a_nm_fmt[13]    = {0} ;                                    // Name Format e.g NAMEFORMAT=R
char a_adj_wei[50]   = {0} ;                                    // ADJWEIGHT=Person_Name+10
char a_delimeter[100]= {0} ;                                    // Delimiter in Tagged data
char a_prps_ty[100]  = {0} ;                                    // Purpose with match level Typical
char a_prps_con[100] = {0} ;                                    // Purpose with match level Conservative
char a_prps_lse[100] = {0} ;                                    // Purpose with match level Loose
char a_Id[10]        = {0} ;

char a_mtc_lvl_typ_acc_rej[100] = {0} ;                         // Match level Typical with accept limit and reject limit
char a_mtc_lvl_con_acc_rej[100] = {0} ;                         // Match level Conservative with accept limit and reject limit
char a_mtc_lvl_lse_acc_rej[100] = {0} ;                         // Match level Loose with accept limit and reject limit

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
char *S_K_delimeter   = " DELIMITER=" ;                         // DELIMITER= format
char *str_delimeter_d = "" ;                                    // Delimiter data
char *S_K_adjwei      = " ADJWEIGHT=" ;                         // ADJWEIGHT= format
char *str_adjwei_d    = "" ;                                    // Adjweight data
int   i_adjwei_val_d  = 0 ;                                     // Adjweight value

int i_Add_prps_err_knt   = 0 ;                                  // Address purpose errors count
int i_Con_prps_err_knt   = 0 ;                                  // Contact purpose errors count
int i_CEn_prps_err_knt   = 0 ;                                  // Corp Entity purpose errors count
int i_Div_prps_err_knt   = 0 ;                                  // Division purpose errors count
int i_Fam_prps_err_knt   = 0 ;                                  // Family purpose errors count
int i_Fld_prps_err_knt   = 0 ;                                  // Fields purpose errors count
int i_Ftr1_prps_err_knt  = 0 ;                                  // Address purpose errors count
int i_Ftr2_prps_err_knt  = 0 ;                                  // Filter1 purpose errors count
int i_Ftr3_prps_err_knt  = 0 ;                                  // Filter1 purpose errors count
int i_Ftr4_prps_err_knt  = 0 ;                                  // Filter1 purpose errors count
int i_Ftr5_prps_err_knt  = 0 ;                                  // Filter1 purpose errors count
int i_Ftr6_prps_err_knt  = 0 ;                                  // Filter1 purpose errors count
int i_Ftr7_prps_err_knt  = 0 ;                                  // Filter1 purpose errors count
int i_Ftr8_prps_err_knt  = 0 ;                                  // Filter1 purpose errors count
int i_Ftr9_prps_err_knt  = 0 ;                                  // Filter1 purpose errors count
int i_Hsho_prps_err_knt  = 0 ;                                  // Filter1 purpose errors count
int i_ind_prps_err_knt   = 0 ;                                  // Filter1 purpose errors count
int i_OrgN_prps_err_knt  = 0 ;                                  // Filter1 purpose errors count
int i_PerN_prps_err_knt  = 0 ;                                  // Filter1 purpose errors count
int i_Res_prps_err_knt   = 0 ;                                  // Filter1 purpose errors count
int i_WCon_prps_err_knt  = 0 ;                                  // Filter1 purpose errors count
int i_WHsho_prps_err_knt = 0 ;                                  // Filter1 purpose errors count

// Empty records
int i_emp_rec_knt        = 0 ;                                  // Empty records count
int i_src_rec_emp_knt    = 0 ;                                  // Count of empty Search data records
int i_fle_rec_emp_knt    = 0 ;                                  // Count of empty File data records

// Error Record : Id
int i_id_err_rec_knt     = 0 ;                                  // Records error that does not contain Id
int i_id_src_rec_err_knt = 0 ;                                  // Count of errors in search record that does not contain Id
int i_id_fle_rec_err_knt = 0 ;                                  // Count of errors in file record that does not contain Id

int i_src_rec_err_knt    = 0 ;                                  // Search records error count
int i_fle_rec_err_knt    = 0 ;                                  // File records error count

// Decision count
int i_dec_A_knt          = 0 ;                                  // Count of decision results are A ( Accept )
int i_dec_R_knt          = 0 ;                                  // Count of decision results are R ( Reject )
int i_dec_U_knt          = 0 ;                                  // Count of decision results are U ( Undecided )

// Score count
int i_scr_pft_mtc_knt = 0 ;                                     // Perfect match
int i_scr_90_99_knt   = 0 ;                                     // Score range 90 to 99
int i_scr_80_89_knt   = 0 ;                                     // Score range 80 to 89
int i_scr_70_79_knt   = 0 ;                                     // Score range 70 to 79
int i_scr_60_69_knt   = 0 ;                                     // Score range 60 to 69
int i_scr_50_59_knt   = 0 ;                                     // Score range 50 to 59
int i_scr_40_49_knt   = 0 ;                                     // Score range 40 to 49
int i_scr_30_39_knt   = 0 ;                                     // Score range 30 to 39
int i_scr_20_29_knt   = 0 ;                                     // Score range 20 to 29
int i_scr_10_19_knt   = 0 ;                                     // Score range 10 to 19
int i_scr_1_9_knt     = 0 ;                                     // Score range 01 to 09
int i_scr_nt_mtc_knt  = 0 ;                                     // Not matching


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
    "-j<reject_limit> -t<delimeter> -i <input_file_directory>"
    " -o <output_file_directory> -l <log_file_directory> -m <Multiplier> -v<verbose>\n\nExample:\n\n"
    "GMtc -d 101 -r 1001 -s default -p india -u Address -e 4 -n L  -a 20 -j -10 "
    "-t @ -i E:/SurendraK/Work/DeDupeProcs/Input/"
    " -o E:/SurendraK/Work/DeDupeProcs/Output/ -l E:/SurendraK/Work/SSAProcs/Log/ -m 10000 -v\n"
   ) ;

}
/**********************************************************************
 End of subroutine s_print_usage                                      *
**********************************************************************/

static void s_getParameter ( int argc , char *argv[] ) {
// This subroutine is default parameter of getopt in s_getParameter

  s_date_time ( ) ;                                             // Call subroutine s_date_time
  while ( ( i_option = getopt ( argc , argv , "d:r:s:p:u:e:n:a:j:w:x:t:i:o:l:m:v::" ) ) != -1 ) {
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
      case 'w' :                                                // Adjweight parameter
        p_adjweight  = optarg ;
        break ;
      case 'x' :                                                // Adjweight value parameter
        p_adjwei_val = atoi( optarg ) ;
        break ;
      case 't' :                                                // Delimiter parameter
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

  str_prps_nm     = p_purpose ;

  i_uni_enc_d     = p_uni_enc ;

  str_nm_fmt_d    = p_nm_fmt ;

  str_adjwei_d    = p_adjweight ;

  i_adjwei_val_d  = p_adjwei_val ;

  str_delimeter_d = p_delimeter ;

  // Data set number should be in a range of 100 to 999
  if( p_data_set > 999 || p_data_set < 100 ) {
    printf("%s","JOB ABANDONDED - Data set number should be integer and in a range of 100 to 999\n" ) ;
    exit(1) ;
  }

  // Run time number should be in a range of 1000 to 9999
  if( p_run_time > 9999 || p_run_time < 1000 ) {
    printf("%s","JOB ABANDONDED - Run time number should be integer and in a range of 1000 to 9999\n" ) ;
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
      printf ( "%s" , "JOB ABANDONDED - Invalid CHARACTER ENCODING value\n" ) ;
      exit(1);
    }
  }

  if ( *str_nm_fmt_d ) {                                        // If Name Format string is not empty
    if ( ( strcmp ( str_nm_fmt_d , "L" ) != 0 ) &&              // Name Format must be L or R
         ( strcmp ( str_nm_fmt_d , "R" ) != 0 ) ) {
      printf ( "%s" , "JOB ABANDONDED - Invalid NAMEFORMAT value - must be L or R\n" ) ;
      exit(1);
    }
  }

  if ( i_uni_enc_d != 0 ) {                                     // If Unicode Encoding value is not zero
    sprintf ( a_uni_enc , "%s%d" , S_K_uni_enc , i_uni_enc_d ) ;
  }

  if ( *str_nm_fmt_d ) {                                        // If NAMEFORMAT value is not empty
    sprintf ( a_nm_fmt , "%s%s" , S_K_nm_fmt , str_nm_fmt_d ) ;
  }

  if ( *str_adjwei_d && i_adjwei_val_d != 0 ) {                 // If ADJWEIGHT data and value are non empty
      if ( strcmp ( str_adjwei_d , "Person_Name" ) == 0 ||
           strcmp ( str_adjwei_d , "Organization_Name" ) == 0 ||
           strcmp ( str_adjwei_d , "Address_Part1" ) == 0 ||
           strcmp ( str_adjwei_d , "Address_Part2" ) == 0 ||
           strcmp ( str_adjwei_d , "Postal_Area" ) == 0 ||
           strcmp ( str_adjwei_d , "Telephone_Number" ) == 0 ||
           strcmp ( str_adjwei_d , "Date" ) == 0 ||
           strcmp ( str_adjwei_d , "Id" ) == 0 ||
           strcmp ( str_adjwei_d , "Attribute1" ) == 0 ||
           strcmp ( str_adjwei_d , "Attribute2" ) == 0 ) {

        sprintf ( a_adj_wei , "%s%s%+d" , S_K_adjwei , str_adjwei_d , i_adjwei_val_d ) ;
      }
      else {
        printf ("%s" , "Unknown ADJWEIGHT: Output file created without ADJWEIGHT field and its value\n" ) ;
      }
  }
  else if ( *str_adjwei_d && i_adjwei_val_d == 0 ){             // If ADJWEIGHT value is empty
    if ( strcmp ( str_adjwei_d , "Person_Name" ) == 0 ||
         strcmp ( str_adjwei_d , "Organization_Name" ) == 0 ||
         strcmp ( str_adjwei_d , "Address_Part1" ) == 0 ||
         strcmp ( str_adjwei_d , "Address_Part2" ) == 0 ||
         strcmp ( str_adjwei_d , "Postal_Area" ) == 0 ||
         strcmp ( str_adjwei_d , "Telephone_Number" ) == 0 ||
         strcmp ( str_adjwei_d , "Date" ) == 0 ||
         strcmp ( str_adjwei_d , "Id" ) == 0 ||
         strcmp ( str_adjwei_d , "Attribute1" ) == 0 ||
         strcmp ( str_adjwei_d , "Attribute2" ) == 0 ) {

      printf ("%s" , "Syntax error in weight adjustment: Output file created without ADJWEIGHT field and its value\n" ) ;
    }
    else {
      printf ("%s" , "Unknown ADJWEIGHT: Output file created without ADJWEIGHT field and its value\n" ) ;
    }
  }


  if ( *str_delimeter_d ) {                                     // If delimeter value is not empty
    sprintf ( a_delimeter , "%s%s" , S_K_delimeter , str_delimeter_d ) ;
    //printf ("DELI:%s\n" ,a_delimeter ) ;
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
  sprintf( a_str_input_file , "%d%d.smp" , p_data_set , p_run_time ) ;
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

  if ( atoi( a_score ) == 100 ) {
    i_scr_pft_mtc_knt ++ ;                                      // Perfect match count
  }
  else if ( atoi( a_score ) >= 90 && atoi( a_score ) <= 99 ) {
    i_scr_90_99_knt ++ ;                                        // Score range 90 to 99
  }
  else if ( atoi( a_score ) >= 80 && atoi( a_score ) <= 89 ) {
    i_scr_80_89_knt ++ ;                                        // Score range 80 to 89
  }
  else if ( atoi( a_score ) >= 70 && atoi( a_score ) <= 79 ) {
    i_scr_70_79_knt ++ ;                                        // Score range 70 to 79
  }
  else if ( atoi( a_score ) >= 60 && atoi( a_score ) <= 69 ) {
    i_scr_60_69_knt ++ ;                                        // Score range 60 to 69
  }
  else if ( atoi( a_score ) >= 50 && atoi( a_score ) <= 59 ) {
    i_scr_50_59_knt ++ ;                                        // Score range 50 to 59
  }
  else if ( atoi( a_score ) >= 40 && atoi( a_score ) <= 49 ) {
    i_scr_40_49_knt ++ ;                                        // Score range 40 to 49
  }
  else if ( atoi( a_score ) >= 30 && atoi( a_score ) <= 39 ) {
    i_scr_30_39_knt ++ ;                                        // Score range 30 to 39
  }
  else if ( atoi( a_score ) >= 20 && atoi( a_score ) <= 29 ) {
    i_scr_20_29_knt ++ ;                                        // Score range 20 to 29
  }
  else if ( atoi( a_score ) >= 10 && atoi( a_score ) <= 19 ) {
    i_scr_10_19_knt ++ ;                                        // Score range 10 to 19
  }
  else if ( atoi( a_score ) >= 1  && atoi( a_score ) <= 9 ) {
    i_scr_1_9_knt ++ ;                                          // Score range 01 to 09
  }
  else if ( atoi( a_score ) == 0 ) {
    i_scr_nt_mtc_knt ++ ;                                       // Not matching
  }

  if ( strcmp ( a_decision , "A" ) == 0 ) {
    i_dec_A_knt ++ ;                                            // Count of decision results are A ( Accept )
  }

  if ( strcmp ( a_decision , "R" ) == 0 ) {
    i_dec_R_knt ++ ;                                            // Count of decision results are R ( Reject )
  }

  if ( strcmp ( a_decision , "U" ) == 0 ) {
    i_dec_U_knt ++ ;                                            // Count of decision results are U ( Undecided )
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

  s_date_time( ) ;                                             // Call date and time subroutine

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

if ( *str_delimeter_d ) {                                       // If delimeter parameter is not empty
  sprintf ( a_Id ,"%sId%s", str_delimeter_d , str_delimeter_d ) ;
}

fprintf ( f_log_fopen_status, "------ Run Parameters ------" ) ;
if ( p_data_set != 0 ) {                                        // If data set number is not empty
  fprintf ( f_log_fopen_status, "\nData set number       : %d", p_data_set ) ;
}

if ( p_run_time != 0 ) {                                        // If run time number is not empty
  fprintf ( f_log_fopen_status, "\nRun time number       : %d", p_run_time ) ;
}

if ( *p_population ) {                                          // If Population is non empty
  fprintf ( f_log_fopen_status, "\nPopulation            : %s", p_population ) ;
}

if ( *p_system_nm ) {                                           // If System name is non empty
  fprintf ( f_log_fopen_status, "\nSystem name           : %s", p_system_nm ) ;
}

if ( *p_infdir ) {                                              // If Input file directory is non empty
  fprintf ( f_log_fopen_status, "\nInput File Directory  : %s", p_infdir ) ;
}

if ( *p_outfdir ) {                                             // If Output file directory is non empty
  fprintf ( f_log_fopen_status, "\nOutput File Directory : %s", p_outfdir ) ;
}

if ( *p_logfdir ) {                                             // If Log file directory is non empty
  fprintf ( f_log_fopen_status, "\nLog File Directory    : %s", p_logfdir ) ;
}

fprintf ( f_log_fopen_status, "\nVerbose               : %s\n", ( i_verbose_flg == 1 ? "Yes" : "No" ) ) ;

fprintf ( f_log_fopen_status, "\n------ File Names ------" ) ;
fprintf ( f_log_fopen_status, "\nInput file name       : %s", a_str_input_file ) ;
fprintf ( f_log_fopen_status, "\nOutput file name      : %s", a_str_output_file ) ;
fprintf ( f_log_fopen_status, "\nLog file name         : %s\n", a_str_log_file ) ;

fprintf ( f_log_fopen_status , "\n------ CONTROLS ------ \n") ;

if ( *p_purpose ) {
  fprintf ( f_log_fopen_status , "PURPOSE                : %s\n" , p_purpose ) ;
}

fprintf ( f_log_fopen_status , "MATCH_LEVEL            : ( %s ,%s ,%s )\n" ,
           S_K_mtc_lvl_ty , S_K_mtc_lvl_con ,S_K_mtc_lvl_lse ) ;

if ( i_uni_enc_d != 0 ) {
  fprintf ( f_log_fopen_status , "UNICODE_ENCODING       : %d\n" , i_uni_enc_d ) ;
}

if ( *str_nm_fmt_d ) {
  fprintf ( f_log_fopen_status , "NAMEFORMAT             : %s\n" , str_nm_fmt_d ) ;
}

if ( p_acc_lmt != 0 ) {
  fprintf ( f_log_fopen_status , "Accept Limit (+/-nn)   : %+d\n" , p_acc_lmt ) ;
}

if ( p_rej_lmt != 0 ) {
  fprintf ( f_log_fopen_status , "Reject Limit (+/-nn)   : %+d\n" , p_rej_lmt ) ;
}

if ( *str_adjwei_d ) {
  fprintf ( f_log_fopen_status , "ADJWEIGHT              : %s\n" , str_adjwei_d ) ;
}

if ( i_adjwei_val_d != 0 ) {
  fprintf ( f_log_fopen_status , "Adjweight value (+/-nn): %+d\n" , i_adjwei_val_d ) ;
}

if ( *str_delimeter_d ) {
  fprintf ( f_log_fopen_status , "DELIMITER              : %s\n" , str_delimeter_d ) ;
}

s_GMtc_open ( ) ;

  // Read a input file line by line
while( fgets ( str_current_rec , sizeof ( str_current_rec ) , f_input_fopen_status ) ) {
  ++ i_rec_number ;                                             // Record number
  i_cur_rec_len = strlen( str_current_rec ) ;                   // Length of the current record
  if ( i_cur_rec_len > 0 && str_current_rec[i_cur_rec_len-1] == '\n' ) {
    str_current_rec[--i_cur_rec_len] = '\0' ;                   // Remove new line character from current record
  }

  for( i_idx = 1 ; i_idx < i_cur_rec_len; i_idx++ ) {
    if( str_current_rec[i_idx] == '\t' ) {                      // Search data and file data separator
      i_fld_spr = i_idx ;                                       // After *Id* first * position
      break ;                                                   // Until find separator
    }
  }

  strncpy                                                       // Copy substring from string
  (
    a_search_data ,                                             // Search data record
    str_current_rec ,
    i_fld_spr                                                   // Position of field separator
  ) ;

  strncpy                                                       // Copy substring from string
  (
    a_file_data ,                                               // File data record
    str_current_rec + ( i_fld_spr + 1 ) ,
    i_cur_rec_len                                               // Length of current record
  ) ;

  a_search_data[i_fld_spr]   = '\0' ;                           // Add null terminator at the end of search data
  a_file_data[i_cur_rec_len] = '\0' ;                           // Add null terminator at the end of file data

  if ( !*a_search_data ) {
    i_src_rec_emp_knt ++ ;                                      // Count of empty Search data records
    i_emp_rec_knt ++ ;                                          // Empty records count
  }

  if ( !*a_file_data ) {
    i_fle_rec_emp_knt ++ ;                                      // Count of empty File data records
    i_emp_rec_knt ++ ;                                          // Empty records count
  }

  i_search_data_len = strlen( a_search_data ) ;                 // Length of search data
  i_file_data_len   = strlen( a_file_data ) ;                   // Length of file data

  // Check search data and file data contain *Id* or not
  if ( strstr
       (
         a_search_data , ( *str_delimeter_d ? a_Id : "*Id*" )   // If delimeter parameter is non empty find id in search data record
       ) != NULL &&
       strstr
       (
         a_file_data ,( *str_delimeter_d ? a_Id : "*Id*" )      // If delimeter parameter is non empty find id in file data record
       ) != NULL ) {

    // Find Id of Search data
    str_ptr_id_search     = strstr
                           (
                             a_search_data , ( *str_delimeter_d ? a_Id : "*Id*" )
                           ) ;                                  // Search *Id* in search data capture string from *Id* to till end of the string

    i_id_start_pos_search =                                     // Starting position of *Id* in search data
    (
      i_search_data_len - strlen( str_ptr_id_search )
    ) ;

    i_pos_afr_id_search   =
                            (
                              i_id_start_pos_search + ( *str_delimeter_d ? strlen(a_Id) : 4 )
                            ) ;                                 // Position after *Id* in search data

    for( i_idx = i_pos_afr_id_search ; i_idx < i_search_data_len; i_idx++ ) {
      if( a_search_data[i_idx] == ( *str_delimeter_d ? str_delimeter_d[0] : '*' ) ) {
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
    str_ptr_id_file     = strstr
                         (
                           a_file_data , ( *str_delimeter_d ? a_Id : "*Id*" )
                         ) ;                                    // Search *Id* in current record capture string from *Id* to till end of the string

    i_id_start_pos_file =                                       // Starting position of *Id*
    (
      i_file_data_len - strlen(str_ptr_id_file)
    ) ;

    i_pos_afr_id_file   = ( i_id_start_pos_file + ( *str_delimeter_d ? strlen(a_Id) : 4 )  ) ;         // Position after *Id*

    for( i_idx = i_pos_afr_id_file ; i_idx < i_file_data_len; i_idx++ ) {
      if( a_file_data[i_idx] == ( *str_delimeter_d ? str_delimeter_d[0] : '*' ) ) {
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

    if ( p_acc_lmt == 0 && p_rej_lmt == 0 ) {                   // Both accept limit and reject limit are zero
      sprintf ( a_mtc_lvl_typ_acc_rej ,"%s" , S_K_mtc_lvl_ty ) ;
    }
    else if ( p_acc_lmt != 0 && p_rej_lmt != 0 ) {              // Both accept limit and reject limit are non zero
      sprintf ( a_mtc_lvl_typ_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_ty , p_acc_lmt , p_rej_lmt ) ;
    }
    else if ( p_acc_lmt == 0 && p_rej_lmt != 0 ) {              // Accept limit are zero and reject limit are not zero
      sprintf ( a_mtc_lvl_typ_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_ty , p_acc_lmt , p_rej_lmt ) ;
    }
    else if ( p_acc_lmt !=0  && p_rej_lmt == 0 ) {              // Accept limit are non zero and reject limit are zero
      sprintf ( a_mtc_lvl_typ_acc_rej ,"%s%+d" , S_K_mtc_lvl_ty , p_acc_lmt ) ;
    }

    if ( p_acc_lmt == 0 && p_rej_lmt == 0 ) {                   // Both accept limit and reject limit are zero
      sprintf ( a_mtc_lvl_con_acc_rej ,"%s" , S_K_mtc_lvl_con ) ;
    }
    else if ( p_acc_lmt != 0 && p_rej_lmt != 0 ) {              // Both accept limit and reject limit are non zero
      sprintf ( a_mtc_lvl_con_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_con , p_acc_lmt , p_rej_lmt ) ;
    }
    else if ( p_acc_lmt == 0 && p_rej_lmt != 0 ) {              // Accept limit are zero and reject limit are not zero
      sprintf ( a_mtc_lvl_con_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_con , p_acc_lmt , p_rej_lmt ) ;
    }
    else if ( p_acc_lmt !=0  && p_rej_lmt == 0 ) {              // Accept limit are non zero and reject limit are zero
      sprintf ( a_mtc_lvl_con_acc_rej ,"%s%+d" , S_K_mtc_lvl_con , p_acc_lmt ) ;
    }

    if ( p_acc_lmt == 0 && p_rej_lmt == 0 ) {                   // Both accept limit and reject limit are zero
      sprintf ( a_mtc_lvl_lse_acc_rej ,"%s" , S_K_mtc_lvl_lse ) ;
    }
    else if ( p_acc_lmt != 0 && p_rej_lmt != 0 ) {              // Both accept limit and reject limit are non zero
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
      "%s%s %s%s%s%s%s%s" ,
       S_K_prps ,                                               // PURPOSE= format
       str_prps_nm ,                                            // Purpose name
       S_K_mtc_lvl ,                                            // MATCH_LEVEL= format
       a_mtc_lvl_typ_acc_rej ,                                  // Match level Typical with accept limit and reject limit
       a_uni_enc ,                                              // Unicode encoding format e.g UNICODE=4
       a_nm_fmt ,                                               // Name Format e.g NAMEFORMAT=R
       a_adj_wei ,
       a_delimeter                                              // Delimiter
    ) ;

    sprintf
    (
       a_prps_con ,                                             // Purpose with match level Conservative
       "%s%s %s%s%s%s%s%s" ,
       S_K_prps ,                                               // PURPOSE= format
       str_prps_nm ,                                            // Purpose name
       S_K_mtc_lvl ,                                            // MATCH_LEVEL= format
       a_mtc_lvl_con_acc_rej ,                                  // Match level Conservative with accept limit and reject limit
       a_uni_enc ,                                              // Unicode encoding format e.g UNICODE=4
       a_nm_fmt ,                                               // Name Format e.g NAMEFORMAT=R
       a_adj_wei ,
       a_delimeter                                              // Delimiter
    ) ;

    sprintf
    (
       a_prps_lse ,                                             // Purpose with match level Loose
      "%s%s %s%s%s%s%s%s" ,
       S_K_prps ,                                               // PURPOSE= format
       str_prps_nm ,                                            // Purpose name
       S_K_mtc_lvl ,                                            // MATCH_LEVEL= format
       a_mtc_lvl_lse_acc_rej ,                                  // Match level Loose with accept limit and reject limit
       a_uni_enc ,                                              // Unicode encoding format e.g UNICODE=4
       a_nm_fmt ,                                               // Name Format e.g NAMEFORMAT=R
       a_adj_wei ,
       a_delimeter                                              // Delimiter
    ) ;

    /*****************************************************************************/

    if ( strcmp ( p_purpose , "Address" ) == 0 ) {
      if ( strstr ( a_search_data , "Address_Part1" ) != NULL &&
           strstr ( a_file_data ,   "Address_Part1" ) != NULL ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Add_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Address_Part1" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Address_Part1" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End if Resident
    else if ( strcmp ( p_purpose , "Contact" ) == 0 ) {
      if ( ( strstr ( a_search_data , "Person_Name" ) != NULL &&
             strstr ( a_search_data , "Organization_Name" ) != NULL &&
             strstr ( a_search_data , "Address_Part1" ) != NULL ) &&
           ( strstr ( a_file_data , "Person_Name" ) != NULL &&
             strstr ( a_file_data , "Organization_Name" ) != NULL &&
             strstr ( a_file_data , "Address_Part1" ) != NULL )
         ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Con_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Person_Name" ) == NULL ||
             strstr ( a_search_data , "Organization_Name" ) == NULL ||
             strstr ( a_search_data , "Address_Part1" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Person_Name" ) == NULL ||
             strstr ( a_file_data , "Organization_Name" ) == NULL ||
             strstr ( a_file_data , "Address_Part1" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Contact
    else if ( strcmp ( p_purpose , "Corp_Entity" ) == 0 ) {
      if ( strstr ( a_search_data , "Organization_Name" ) != NULL &&
           strstr ( a_file_data ,   "Organization_Name" ) != NULL ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_CEn_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Organization_Name" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data ,   "Organization_Name" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Corp_Entity
    else if ( strcmp ( p_purpose , "Division" ) == 0 ) {
      if ( ( strstr ( a_search_data , "Organization_Name" ) != NULL &&
             strstr ( a_search_data , "Address_Part1" ) != NULL ) &&
           ( strstr ( a_file_data , "Organization_Name" ) != NULL &&
             strstr ( a_file_data , "Address_Part1" ) != NULL )
         ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Div_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Organization_Name" ) == NULL ||
             strstr ( a_search_data , "Address_Part1" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Organization_Name" ) == NULL ||
             strstr ( a_file_data , "Address_Part1" ) == NULL ){

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Division
    else if ( strcmp ( p_purpose , "Family" ) == 0 ) {
      if ( ( strstr ( a_search_data , "Person_Name" ) != NULL &&
             strstr ( a_search_data , "Address_Part1" ) != NULL &&
             strstr ( a_search_data , "Telephone_Number" ) != NULL ) &&
           ( strstr ( a_file_data , "Person_Name" ) != NULL &&
             strstr ( a_file_data , "Address_Part1" ) != NULL &&
             strstr ( a_file_data , "Telephone_Number" ) != NULL )
         ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Fam_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Person_Name" ) == NULL ||
             strstr ( a_search_data , "Address_Part1" ) == NULL ||
             strstr ( a_search_data , "Telephone_Number" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Person_Name" ) == NULL ||
             strstr ( a_file_data , "Address_Part1" ) == NULL ||
             strstr ( a_file_data , "Telephone_Number" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Family
    else if ( strcmp ( p_purpose , "Fields" ) == 0 ) {
      if ( ( strstr ( a_search_data , "Id" ) != NULL ||
             strstr ( a_search_data , "Person_Name" ) != NULL ||
             strstr ( a_search_data , "Organization_Name" ) != NULL ||
             strstr ( a_search_data , "Address_Part1" ) != NULL ||
             strstr ( a_search_data , "Address_Part2" ) != NULL ||
             strstr ( a_search_data , "Postal_Area" ) != NULL ||
             strstr ( a_search_data , "Telephone_Number" ) != NULL ||
             strstr ( a_search_data , "Date" ) != NULL ||
             strstr ( a_search_data , "Attribute1" ) != NULL ||
             strstr ( a_search_data , "Attribute2" ) != NULL )
             &&
           ( strstr ( a_file_data , "Id" ) != NULL ||
            strstr ( a_file_data , "Person_Name" ) != NULL ||
            strstr ( a_file_data , "Organization_Name" ) != NULL ||
            strstr ( a_file_data , "Address_Part1" ) != NULL ||
            strstr ( a_file_data , "Address_Part2" ) != NULL ||
            strstr ( a_file_data , "Postal_Area" ) != NULL ||
            strstr ( a_file_data , "Telephone_Number" ) != NULL ||
            strstr ( a_file_data , "Date" ) != NULL ||
            strstr ( a_file_data , "Attribute1" ) != NULL ||
            strstr ( a_file_data , "Attribute2" ) != NULL )
         ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Fld_prps_err_knt ++ ;

        fprintf
        (
          f_log_fopen_status ,
          "\nRecord no : %d Error Message : %s %s" ,
          i_rec_number , "Record does not contain all fields required for Purpose" , p_purpose
        ) ;
        fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_current_rec ) ;
      }
    } // End of else if Fields
    else if ( strcmp ( p_purpose , "Filter1" ) == 0 ) {
      if ( strstr ( a_search_data , "Filter1" ) != NULL &&
           strstr ( a_file_data ,   "Filter1" ) != NULL ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Ftr1_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Filter1" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Filter1" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Filter1
    else if ( strcmp ( p_purpose , "Filter2" ) == 0 ) {
      if ( strstr ( a_search_data , "Filter2" ) != NULL &&
           strstr ( a_file_data ,   "Filter2" ) != NULL ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Ftr2_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Filter2" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Filter2" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Filter2
    else if ( strcmp ( p_purpose , "Filter3" ) == 0 ) {
      if ( strstr ( a_search_data , "Filter3" ) != NULL &&
           strstr ( a_file_data ,   "Filter3" ) != NULL ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Ftr3_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Filter3" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Filter3" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Filter3
    else if ( strcmp ( p_purpose , "Filter4" ) == 0 ) {
      if ( strstr ( a_search_data , "Filter4" ) != NULL &&
           strstr ( a_file_data ,   "Filter4" ) != NULL ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Ftr4_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Filter4" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Filter4" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Filter4
    else if ( strcmp ( p_purpose , "Filter5" ) == 0 ) {
      if ( strstr ( a_search_data , "Filter5" ) != NULL &&
           strstr ( a_file_data ,   "Filter5" ) != NULL ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Ftr5_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Filter5" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Filter5" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Filter5
    else if ( strcmp ( p_purpose , "Filter6" ) == 0 ) {
      if ( strstr ( a_search_data , "Filter6" ) != NULL &&
           strstr ( a_file_data ,   "Filter6" ) != NULL ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Ftr6_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Filter6" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Filter6" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Filter6
    else if ( strcmp ( p_purpose , "Filter7" ) == 0 ) {
      if ( strstr ( a_search_data , "Filter7" ) != NULL &&
           strstr ( a_file_data ,   "Filter7" ) != NULL ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Ftr7_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Filter7" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Filter7" ) == NULL ) {
          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Filter7
    else if ( strcmp ( p_purpose , "Filter8" ) == 0 ) {
      if ( strstr ( a_search_data , "Filter8" ) != NULL &&
           strstr ( a_file_data ,   "Filter8" ) != NULL ) {

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
      }
      else {
        i_Ftr8_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Filter8" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Filter8" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Filter8
    else if ( strcmp ( p_purpose , "Filter9" ) == 0 ) {
      if ( strstr ( a_search_data , "Filter9" ) != NULL &&
           strstr ( a_file_data ,   "Filter9" ) != NULL ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Ftr9_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Filter9" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Filter9" ) == NULL ) {
          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Filter9
    else if ( strcmp ( p_purpose , "Household" ) == 0 ) {
      if ( ( strstr ( a_search_data , "Person_Name" ) != NULL &&
             strstr ( a_search_data , "Address_Part1" ) != NULL ) &&
           ( strstr ( a_file_data , "Person_Name" ) != NULL &&
             strstr ( a_file_data , "Address_Part1" ) != NULL )
         ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Hsho_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Person_Name" ) == NULL ||
             strstr ( a_search_data , "Address_Part1" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Person_Name" ) == NULL ||
             strstr ( a_file_data , "Address_Part1" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Household
    else if ( strcmp ( p_purpose , "Individual" ) == 0 ) {
      if ( ( strstr ( a_search_data , "Person_Name" ) != NULL &&
           ( strstr ( a_search_data , "Date" ) != NULL ||
             strstr ( a_search_data , "Id" ) != NULL ) ) &&

           ( strstr ( a_file_data , "Person_Name" ) != NULL &&
           ( strstr ( a_file_data , "Date" ) != NULL ||
             strstr ( a_file_data , "Id" ) != NULL ) ) ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_ind_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Person_Name" ) == NULL ||
           ( strstr ( a_search_data , "Date" ) == NULL ||
             strstr ( a_search_data , "Id" ) == NULL ) ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Person_Name" ) == NULL ||
           ( strstr ( a_file_data , "Date" ) == NULL ||
             strstr ( a_file_data , "Id" ) == NULL ) ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Individual
    else if ( strcmp ( p_purpose , "Organization" ) == 0 ) {
      if ( strstr ( a_search_data , "Organization_Name" ) != NULL &&
           strstr ( a_file_data , "Organization_Name" ) != NULL ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_OrgN_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Organization_Name" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Organization_Name" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Organization
    else if ( strcmp ( p_purpose , "Person_Name" ) == 0 ) {
      if ( strstr ( a_search_data , "Person_Name" ) != NULL &&
           strstr ( a_file_data , "Person_Name" ) != NULL ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_PerN_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Person_Name" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Person_Name" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Person_Name
    else if ( strcmp ( p_purpose , "Resident" ) == 0 ) {
      if ( ( strstr ( a_search_data , "Person_Name" ) != NULL &&
             strstr ( a_search_data , "Address_Part1" ) != NULL ) &&
           ( strstr ( a_file_data , "Person_Name" ) != NULL &&
             strstr ( a_file_data , "Address_Part1" ) != NULL )
         ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_Res_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Person_Name" ) == NULL ||
             strstr ( a_search_data , "Address_Part1" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Person_Name" ) == NULL ||
             strstr ( a_file_data , "Address_Part1" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Resident
    else if ( strcmp ( p_purpose , "Wide_Contact" ) == 0 ) {
      if ( ( strstr ( a_search_data , "Person_Name" ) != NULL &&
             strstr ( a_search_data , "Organization_Name" ) != NULL ) &&
           ( strstr ( a_file_data , "Person_Name" ) != NULL &&
             strstr ( a_file_data , "Organization_Name" ) != NULL )
         ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_WCon_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Person_Name" ) == NULL ||
             strstr ( a_search_data , "Organization_Name" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Person_Name" ) == NULL ||
             strstr ( a_file_data , "Organization_Name" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Wide_Contact
    else if ( strcmp ( p_purpose , "Wide_Household" ) == 0 ) {
      if ( ( strstr ( a_search_data , "Person_Name" ) != NULL &&
             strstr ( a_search_data , "Address_Part1" ) != NULL &&
             strstr ( a_search_data , "Telephone_Number" ) != NULL ) &&

           ( strstr ( a_file_data , "Person_Name" ) != NULL &&
             strstr ( a_file_data , "Address_Part1" ) != NULL &&
             strstr ( a_file_data , "Telephone_Number" ) != NULL )
         ) {

        s_GMtc_matches
        (
          a_search_data ,                                       // Search data record
          a_file_data ,                                         // File data record
          i_search_data_len ,                                   // Search data record length
          i_file_data_len ,                                     // File data record length
          str_ID_search ,                                       // Id from Search data record
          str_ID_file ,                                         // Id from file data record
          a_prps_ty ,                                           // Controls with Typical match level
          a_prps_con ,                                          // Controls with Conservative match level
          a_prps_lse                                            // Controls with Loose match level
        ) ;
      }
      else {
        i_WHsho_prps_err_knt ++ ;

        // Check error in search data
        if ( strstr ( a_search_data , "Person_Name" ) == NULL ||
             strstr ( a_search_data , "Address_Part1" ) == NULL ||
             strstr ( a_search_data , "Telephone_Number" ) == NULL ) {

          i_src_rec_err_knt ++ ;                                // Search record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
        }

        // Check error in file data
        if ( strstr ( a_file_data , "Person_Name" ) == NULL ||
             strstr ( a_file_data , "Address_Part1" ) == NULL ||
             strstr ( a_file_data , "Telephone_Number" ) == NULL ) {

          i_fle_rec_err_knt ++ ;                                // File record errors count
          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
        }
      }
    } // End of else if Wide_Household
    else {
      printf ( "Validate_purpose '%s' Validate_purpose failed\n" , p_purpose ) ;
      exit(1) ;
    } // End of else

  } // End If
  else {

    if ( strstr
              (
                a_search_data , ( *str_delimeter_d ? a_Id : "*Id*" )
              ) == NULL ) {
      i_id_err_rec_knt ++ ;                                     // Records error that does not contain Id
      i_id_src_rec_err_knt ++ ;                                 // Count of errors in search record that does not contain Id
      fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s" ,
            i_rec_number , "Missing Id in search record"
          ) ;
      fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_search_data ) ;
    }

    if ( strstr
              (
                a_file_data , ( *str_delimeter_d ? a_Id : "*Id*" )
              ) == NULL ) {
      i_id_err_rec_knt ++ ;                                     // Records error that does not contain Id
      i_id_fle_rec_err_knt ++ ;                                 // Count of errors in file record that does not contain Id
      fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s" ,
            i_rec_number , "Missing Id in file record"
          ) ;
      fprintf ( f_log_fopen_status, "\nRecord    : %s\n", a_file_data ) ;
    }
  }

} // End while loop


/*PURPOSE ERRORS COUNT ****************************************************************************/
if ( i_Add_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nAddress purpose errors count    : %d\n" , i_Add_prps_err_knt ) ;
}

if ( i_Con_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nContact purpose errors count    :%d\n" , i_Con_prps_err_knt ) ;
}

if ( i_CEn_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nCorp Entity errors count        :%d\n" , i_CEn_prps_err_knt ) ;
}

if ( i_Div_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nDivision purpose errors count   :%d\n" , i_Div_prps_err_knt ) ;
}

if ( i_Fam_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFamily purpose errors count     :%d\n" , i_Fam_prps_err_knt ) ;
}

if ( i_Fld_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFields purpose errors count     :%d\n" , i_Fld_prps_err_knt ) ;
}

if ( i_Ftr1_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter1 purpose errors count    :%d\n" , i_Ftr1_prps_err_knt ) ;
}

if ( i_Ftr2_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter2 purpose errors count    :%d\n" , i_Ftr2_prps_err_knt ) ;
}

if ( i_Ftr3_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter3 purpose errors count    :%d\n" , i_Ftr3_prps_err_knt ) ;
}

if ( i_Ftr4_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter4 purpose errors count    :%d\n" , i_Ftr4_prps_err_knt ) ;
}

if ( i_Ftr5_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter5 purpose errors count    :%d\n" , i_Ftr5_prps_err_knt ) ;
}

if ( i_Ftr6_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter6 purpose errors count    :%d\n" , i_Ftr6_prps_err_knt ) ;
}

if ( i_Ftr7_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter7 purpose errors count    :%d\n" , i_Ftr7_prps_err_knt ) ;
}

if ( i_Ftr8_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter8 purpose errors count    :%d\n" , i_Ftr8_prps_err_knt ) ;
}

if ( i_Ftr9_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter9 purpose errors count    :%d\n" , i_Ftr9_prps_err_knt ) ;
}

if ( i_Hsho_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nHouseHold purpose errors count  :%d\n" , i_Hsho_prps_err_knt ) ;
}

if ( i_ind_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nIndividual purpose errors count :%d\n" , i_ind_prps_err_knt ) ;
}

if ( i_OrgN_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nOrganization purpose errors count :%d\n" , i_OrgN_prps_err_knt ) ;
}

if ( i_PerN_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nPerson_Name purpose errors count :%d\n" , i_PerN_prps_err_knt ) ;
}

if ( i_Res_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nResident purpose errors count :%d\n" , i_Res_prps_err_knt ) ;
}

if ( i_WCon_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nWide Contact purpose errors count :%d\n" , i_WCon_prps_err_knt ) ;
}

if ( i_WHsho_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nWide Household purpose errors count :%d\n" , i_WHsho_prps_err_knt ) ;
}

/* END PURPOSE ERRORS COUNT ***********************************************************************/

/* SEARCH RECORDS AND FILE RECORDS ERROR COUNT ****************************************************/

if ( i_src_rec_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , " - Search records errors count    :%d\n" , i_src_rec_err_knt ) ;
}

if ( i_fle_rec_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , " - File records errors count      :%d\n" , i_fle_rec_err_knt ) ;
}
/* END SEARCH RECORDS AND FILE RECORDS ERROR COUNT ************************************************/

/* RECORDS THAT DOES NOT CONTAIN ID ERROR COUNT ***************************************************/

if ( i_id_err_rec_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nMissing Id error records : %d\n" , i_id_err_rec_knt ) ;
}

if ( i_id_src_rec_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , " - Search records        : %d\n" , i_id_src_rec_err_knt ) ;
}

if ( i_id_fle_rec_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , " - File records          : %d\n" , i_id_fle_rec_err_knt ) ;
}
/* END RECORDS THAT DOES NOT CONTAIN ID ERROR COUNT ***********************************************/

if ( i_emp_rec_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nEmpty records count : %d\n" , i_emp_rec_knt ) ;
}

if ( i_src_rec_emp_knt != 0 ) {
  fprintf ( f_log_fopen_status , " - Search records   : %d\n" , i_src_rec_emp_knt ) ;
}

if ( i_fle_rec_emp_knt != 0 ) {
  fprintf ( f_log_fopen_status , " - File records     : %d\n" , i_fle_rec_emp_knt ) ;
}
/* END EMPTY RECORDS COUNT ************************************************************************/

fprintf ( f_log_fopen_status , "\n------ Decision counts ------\n" ) ;
fprintf ( f_log_fopen_status , "- Accept      (A) : %d \n" , i_dec_A_knt ) ;
fprintf ( f_log_fopen_status , "- Reject      (R) : %d\n"  , i_dec_R_knt );
fprintf ( f_log_fopen_status , "- Undecided   (U) : %d\n" , i_dec_U_knt ) ;

fprintf ( f_log_fopen_status , "\n------ Score range counts ------\n" ) ;
fprintf ( f_log_fopen_status , "Perfect Match : %d\n" , i_scr_pft_mtc_knt ) ;
fprintf ( f_log_fopen_status , "Score 90 - 99 : %d\n" , i_scr_90_99_knt ) ;
fprintf ( f_log_fopen_status , "Score 80 - 89 : %d\n" , i_scr_80_89_knt ) ;
fprintf ( f_log_fopen_status , "Score 70 - 79 : %d\n" , i_scr_70_79_knt ) ;
fprintf ( f_log_fopen_status , "Score 60 - 69 : %d\n" , i_scr_60_69_knt ) ;
fprintf ( f_log_fopen_status , "Score 50 - 59 : %d\n" , i_scr_50_59_knt ) ;
fprintf ( f_log_fopen_status , "Score 40 - 49 : %d\n" , i_scr_40_49_knt ) ;
fprintf ( f_log_fopen_status , "Score 30 - 39 : %d\n" , i_scr_30_39_knt ) ;
fprintf ( f_log_fopen_status , "Score 20 - 29 : %d\n" , i_scr_20_29_knt ) ;
fprintf ( f_log_fopen_status , "Score 10 - 19 : %d\n" , i_scr_10_19_knt ) ;
fprintf ( f_log_fopen_status , "Score  0 -  9 : %d\n" , i_scr_1_9_knt ) ;
fprintf ( f_log_fopen_status , "Not Matching  : %d\n" , i_scr_nt_mtc_knt ) ;

s_GMtc_close( ) ;                                               // Close the previously open connection

t_end_time = clock( ) - t_start_time ;                          // End time
t_time_taken = ( ( double )t_end_time )/CLOCKS_PER_SEC ;        // In seconds

t_time_taken = t_time_taken + f_add ;
i_t_time_taken = (int)t_time_taken ;

t_hour = i_t_time_taken / 3600 ;                                // Convert seconds to hours
t_min  = ( i_t_time_taken % 3600 ) / 60 ;                       // Convert seconds to minutes
t_sec  = (i_t_time_taken % 60 ) % 60 ;                          // Seconds

if( t_hour < 10 ) {                                             // Ensure hours is a 2 digit string
  sprintf( a_str_hour , "0%d" ,t_hour ) ;                       // Add prefix 0 if hours less than 10
}
else {
  sprintf( a_str_hour , "%d" ,t_hour ) ;                        // Keep it as it is
}

if( t_min < 10 ) {                                              // Ensure minutes is a 2 digit string
  sprintf( a_str_min , "0%d" ,t_min ) ;                         // Add prefix 0 if minutes less than 10
}
else {
  sprintf( a_str_min , "%d" ,t_min ) ;                          // Keep it as it is
}

if( t_sec < 10 ) {                                              // Ensure seconds is a 2 digit string
  sprintf( a_str_sec , "0%d" ,t_sec ) ;                         // Add prefix 0 if seconds less than 10
}
else {
  sprintf( a_str_sec , "%d" ,t_sec ) ;                          // Keep it as it is
}

printf( "\nProcessed %d tagged data records in %s:%s:%s to execute \n" , i_rec_number , a_str_hour , a_str_min , a_str_sec ) ;    // Print time

fprintf( f_log_fopen_status , "- %s:%s:%s to execute \n", a_str_hour , a_str_min , a_str_sec ) ;  // Print time
fprintf( f_log_fopen_status , "\n===============================================\n") ;

fclose ( f_input_fopen_status ) ;                               // input_fopen_status
fclose ( f_output_fopen_status ) ;                              // Close output_fopen_status
fclose ( f_log_fopen_status ) ;                                 // Close log_fopen_status

return ( 0 ) ;

}
/**********************************************************************
 End of script GMtc.c                                             *
**********************************************************************/
