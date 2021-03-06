/*
 Procedure     : 44_506_GMtc.c
 Application   : De-Dupe
 Client        : Internal
 Copyright (c) : 2017 IdentLogic Systems Private Limited
 Author        : Surendra Kadam
 Creation Date : 5 April 2017

 Description   : Used to get a score and match decision for two records, a search
                  record and a file record for match level Typical (Y) ,
                  Conservative (C) , Loose (L) for Address , Contact ,
                  Corp_Entity , Division , Family , Fields , Filter1 ,
                  Filter2 , Filter3 , Filter4 , Filter5 ,
                  Filter6 , Filter7 , Filter8 , Filter9 , Household ,
                  Individual , Person_Name , Organization , Resident ,
                  Wide_Contact , Wide_Household as found in purpose. Any
                  logical combinations of these purposes may be used.

 WARNINGS      : If your data can contain asterisks, make sure that these are
                  either cleaned out prior to calling the dds-name3 functions ,
                  or use a different DELIMITER=setting.

                 Length of the Input file directory , Output file directory and
                  Log file directory should not exceed 1 ,000 bytes , with total
                  filepath not exceeding 1011  , 1015  ,1038 resp.  This is due
                  to the length of the Input file name , Output file name and
                  Log file name are 11 , 15 and 38 resp.

                 Length of the Search data and file data should not exceed 5000
                  and 5000 bytes resp. and the Length of the Controls related
                  variables should not exceed which are specified.

                 Increase the array size as per your convenience.
                 Change the field separator of input file as per your convenience.

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

#define rangeof( arr ) ( sizeof( arr )/sizeof( ( arr )[ 0 ] ) ) // Size of an array

long    l_rc         =  0 ;                                     // Indicate success or failure of open / close sesions
long    l_sockh      = -1 ;                                     // Set to -1 as not calling the dds-name3 server
// Should be -1 on the ddsn3_open call, if opening  a new session, a valid Session ID or any other call
long    l_session_id = -1 ;

// Input file data related variables
char str_current_rec [ 6000 ] = {0} ;                           // Current record of a file
char str_src_tag_dta [ 2000 ] = {0} ;                           // Search tag data
char str_fle_tag_dta [ 2000 ] = {0} ;                           // File tag data
int  i_src_tag_dta_len        = 0 ;                             // Length of search tag data
int  i_fle_tag_dta_len        = 0 ;                             // Length of file tag data
char str_src_id[ 1000 ]       = {0} ;                           // Id of search tag data
char str_fle_id[ 1000 ]       = {0} ;                           // Id of file tag data

int i_rec_number  = 0 ;                                         // Record counter
int i_idx         = 0 ;                                         // Main method for loop initial variable
int i_op_rec_knt  = 0 ;                                         // Output record count

// Time variables of progrm start
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
char *p_encoding   = "" ;                                       // Encoding datatype of search data and file data
char *p_purpose    = "" ;                                       // Parameter purpose
int   p_prpsno     = 0 ;                                        // Parameter purpose no
int   p_uni_enc    = 0 ;                                        // Parameter unicode encoding either 4 , 6 or 8
char *p_nm_fmt     = "" ;                                       // Parameter name format
int   p_acc_lmt    = 0 ;                                        // Parameter accept Limit
int   p_rej_lmt    = 0 ;                                        // Parameter reject Limit
char *p_adjweight  = "" ;                                       // Parameter adjust weight
int   p_adjwei_val = 0 ;                                        // Parameter adjust weight value
char *p_delimiter  = "" ;                                       // Parameter delimiter
char *p_infdir     = "" ;                                       // Parameter input file name
char *p_outfdir    = "" ;                                       // Parameter output file directory
char *p_logfdir    = "" ;                                       // Parameter log file directory
int   p_multiplier = 0 ;                                        // Parameter multiplier

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
char a_str_output_file[15] = {0} ;                              // Output file name
char a_str_log_file[38]    = {0} ;                              // Log file name

char a_str_file_path_input_file[1011]  = {0} ;                  // Input file path - directory with file name
char a_str_file_path_output_file[1011] = {0} ;                  // Output file path - directory with file name
char a_str_file_path_log_file[1038]    = {0} ;                  // Log file path - directory with file name

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

// Controls related variables
char a_uni_enc[19]   = {0} ;                                    // Unicode encoding format e.g UNICODE=4
char a_nm_fmt[13]    = {0} ;                                    // Name Format e.g NAMEFORMAT=R
char a_adj_wei[50]   = {0} ;                                    // ADJWEIGHT=Person_Name+10
char a_delimeter[100]= {0} ;                                    // Delimiter in Tagged data
char a_prps_ty[100]  = {0} ;                                    // Purpose with match level Typical
char a_prps_con[100] = {0} ;                                    // Purpose with match level Conservative
char a_prps_lse[100] = {0} ;                                    // Purpose with match level Loose
char a_Id[10]        = {0} ;                                    // Id with delimiter *Id*
char a_mtc_lvl_typ_acc_rej[100] = {0} ;                         // Match level Typical with accept limit and reject limit
char a_mtc_lvl_con_acc_rej[100] = {0} ;                         // Match level Conservative with accept limit and reject limit
char a_mtc_lvl_lse_acc_rej[100] = {0} ;                         // Match level Loose with accept limit and reject limit

char *str_system_nm_d   = "" ;                                  // System name
char *str_popln_nm_d    = "" ;                                  // Population name
char *str_encoding_d    = "" ;                                  // Encoding datatype

char *S_K_prps        = "PURPOSE=" ;                            // PURPOSE= format
char *str_prps_nm     = "" ;                                    // Purpose name
int  i_prps_no        = 0 ;                                     // Purpose number
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

// Purpose errors counts
int i_Add_prps_err_knt   = 0 ;                                  // Address purpose errors count
int i_Con_prps_err_knt   = 0 ;                                  // Contact purpose errors count
int i_CEn_prps_err_knt   = 0 ;                                  // Corp Entity purpose errors count
int i_Div_prps_err_knt   = 0 ;                                  // Division purpose errors count
int i_Fam_prps_err_knt   = 0 ;                                  // Family purpose errors count
int i_Fld_prps_err_knt   = 0 ;                                  // Fields purpose errors count
int i_Ftr1_prps_err_knt  = 0 ;                                  // Address purpose errors count
int i_Ftr2_prps_err_knt  = 0 ;                                  // Filter1 purpose errors count
int i_Ftr3_prps_err_knt  = 0 ;                                  // Filter2 purpose errors count
int i_Ftr4_prps_err_knt  = 0 ;                                  // Filter3 purpose errors count
int i_Ftr5_prps_err_knt  = 0 ;                                  // Filter4 purpose errors count
int i_Ftr6_prps_err_knt  = 0 ;                                  // Filter5 purpose errors count
int i_Ftr7_prps_err_knt  = 0 ;                                  // Filter6 purpose errors count
int i_Ftr8_prps_err_knt  = 0 ;                                  // Filter7 purpose errors count
int i_Ftr9_prps_err_knt  = 0 ;                                  // Filter8 purpose errors count
int i_Hsho_prps_err_knt  = 0 ;                                  // Filter9 purpose errors count
int i_ind_prps_err_knt   = 0 ;                                  // Individual purpose errors count
int i_OrgN_prps_err_knt  = 0 ;                                  // Organization_Name purpose errors count
int i_PerN_prps_err_knt  = 0 ;                                  // Person_Name purpose errors count
int i_Res_prps_err_knt   = 0 ;                                  // Resident purpose errors count
int i_WCon_prps_err_knt  = 0 ;                                  // WideContact purpose errors count
int i_WHsho_prps_err_knt = 0 ;                                  // Wide_Household purpose errors count

// Error Record : Id
int i_id_err_rec_knt     = 0 ;                                  // Records error that does not contain Id
int i_id_src_rec_err_knt = 0 ;                                  // Count of errors in search record that does not contain Id
int i_id_fle_rec_err_knt = 0 ;                                  // Count of errors in file record that does not contain Id

// Error in Search and file records
int i_src_rec_err_knt    = 0 ;                                  // Search records error count
int i_fle_rec_err_knt    = 0 ;                                  // File records error count

int i_im_rec             = 0 ;                                  // Improper records count

// Total errors count
int i_tot_err_knt        = 0 ;                                  // Total errors count
int i_tot_err_rec_knt    = 0 ;                                  // Total error records count

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

int i_pn_in_src_rec_knt   = 0 ;                                 // Person_Name found in search data count
int i_on_in_src_rec_knt   = 0 ;                                 // Organization_Name found in search data count
int i_adp1_in_src_rec_knt = 0 ;                                 // Address_Part1 found in search data count
int i_adp2_in_src_rec_knt = 0 ;                                 // Address_Part2 found in search data count
int i_pa_in_src_rec_knt   = 0 ;                                 // Postal_Area found in search data count
int i_tel_in_src_rec_knt  = 0 ;                                 // Telephone_Number found in search data count
int i_dte_in_src_rec_knt  = 0 ;                                 // Date found in search data count
int i_id_in_src_rec_knt   = 0 ;                                 // Id found in search data count
int i_att1_in_src_rec_knt = 0 ;                                 // Attribute1 found in search data count
int i_att2_in_src_rec_knt = 0 ;                                 // Attribute2 found in search data count
int i_ft1_in_src_rec_knt  = 0 ;                                 // Filter1 found in search data count
int i_ft2_in_src_rec_knt  = 0 ;                                 // Filter2 found in search data count
int i_ft3_in_src_rec_knt  = 0 ;                                 // Filter3 found in search data count
int i_ft4_in_src_rec_knt  = 0 ;                                 // Filter4 found in search data count
int i_ft5_in_src_rec_knt  = 0 ;                                 // Filter5 found in search data count
int i_ft6_in_src_rec_knt  = 0 ;                                 // Filter6 found in search data count
int i_ft7_in_src_rec_knt  = 0 ;                                 // Filter7 found in search data count
int i_ft8_in_src_rec_knt  = 0 ;                                 // Filter8 found in search data count
int i_ft9_in_src_rec_knt  = 0 ;                                 // Filter9 found in search data count

int i_pn_in_fle_rec_knt   = 0 ;                                 // Person_Name found in file data count
int i_on_in_fle_rec_knt   = 0 ;                                 // Organization_Name found in file data count
int i_adp1_in_fle_rec_knt = 0 ;                                 // Address_Part1 found in file data count
int i_adp2_in_fle_rec_knt = 0 ;                                 // Address_Part2 found in file data count
int i_pa_in_fle_rec_knt   = 0 ;                                 // Postal_Area found in file data count
int i_tel_in_fle_rec_knt  = 0 ;                                 // Telephone_Number found in file data count
int i_dte_in_fle_rec_knt  = 0 ;                                 // Date found in file data count
int i_id_in_fle_rec_knt   = 0 ;                                 // Id found in file data count
int i_att1_in_fle_rec_knt = 0 ;                                 // Attribute1 found in file data count
int i_att2_in_fle_rec_knt = 0 ;                                 // Attribute2 found in file data count
int i_ft1_in_fle_rec_knt  = 0 ;                                 // Filter1 found in file data count
int i_ft2_in_fle_rec_knt  = 0 ;                                 // Filter2 found in file data count
int i_ft3_in_fle_rec_knt  = 0 ;                                 // Filter3 found in file data count
int i_ft4_in_fle_rec_knt  = 0 ;                                 // Filter4 found in file data count
int i_ft5_in_fle_rec_knt  = 0 ;                                 // Filter5 found in file data count
int i_ft6_in_fle_rec_knt  = 0 ;                                 // Filter6 found in file data count
int i_ft7_in_fle_rec_knt  = 0 ;                                 // Filter7 found in file data count
int i_ft8_in_fle_rec_knt  = 0 ;                                 // Filter8 found in file data count
int i_ft9_in_fle_rec_knt  = 0 ;                                 // Filter9 found in file data count

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

  printf (
    "GMtc -d <DATA_SET> -r <RUN> -u <PURPOSE> -b <PURPOSE_NO> -s <system-name> -p <population> "
    "-i <input_file_directory> -o <output_file_directory> -l <log_file_directory> "
    "-t <delimeter> -c <encoding_datatype> -e <unicode_encoding> -n <name_format> "
    "-a <accept_limit> -j <reject_limit> -w <adjweight> -x <adjweight_value> "
    "-m <multiplier> -v(erbose)\n\nMANDATORY VALUES IN CAPITALS\n\nExample:\n\n"
    "GMtc -d 101 -r 1001 -u Resident -b 119 -s default -p india "
    "-i E:/SurendraK/Work/DeDupeProcs/Input/ -o E:/SurendraK/Work/DeDupeProcs/Output/"
    " -l E:/SurendraK/Work/SSAProcs/Log/ -t @ -c TEXT -e 4 -n L -a 20 -j 10"
    " -w Address_Part1 -x 1 -m 10000 -v\n"
   ) ;

}
/**********************************************************************
 End of subroutine s_print_usage                                      *
**********************************************************************/

static void s_getParameter ( int argc , char *argv[] ) {
// This subroutine is default parameter of getopt in s_getParameter

  s_date_time ( ) ;                                             // Call subroutine s_date_time
  while ( ( i_option = getopt ( argc , argv , "d:r:s:p:c:u:b:e:n:a:j:w:x:t:i:o:l:m:v::" ) ) != -1 ) {
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
      case 'u' :                                                // Purpose parameter
        p_purpose    = optarg ;
        break ;
      case 'b' :                                                // Purpose number
        p_prpsno     = atoi( optarg ) ;
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

  str_prps_nm     = p_purpose ;                                 // Store purpose data in another variable
  i_prps_no       = p_prpsno ;                                  // Store purpose no in another varibale
  i_uni_enc_d     = p_uni_enc ;                                 // Store unicode_Encoding data in another variable
  str_nm_fmt_d    = p_nm_fmt ;                                  // Store name format data in another variable
  str_adjwei_d    = p_adjweight ;                               // Store adjweight data in another variable
  i_adjwei_val_d  = p_adjwei_val ;                              // Store adjweight value data in another variable
  str_delimeter_d = p_delimiter ;                               // Store delimiter data in another variable
  i_multiplier    = p_multiplier ;                              // Multiplier value

  str_system_nm_d   = p_system_nm ;                             // Initialize system name in another variable
  str_popln_nm_d    = p_population ;                            // Initialize population name in another variable
  str_encoding_d    = p_encoding ;                              // Initialize encoding datatype in another variable

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

  if ( !*p_delimiter ) {                                        // If delimeter parameter is empty
    str_delimeter_d = "*" ;
    sprintf ( a_delimeter , "%s%s" , S_K_delimeter , str_delimeter_d ) ;
  }
  else {
    sprintf ( a_delimeter , "%s%s" , S_K_delimeter , p_delimiter ) ;
  }

  if ( !*p_system_nm ) {                                        // System name default is default
    str_system_nm_d = "default" ;
  }

  if ( !*p_population ) {                                       // Population default is india
    str_popln_nm_d = "india" ;
  }

  if ( !*p_encoding ) {                                         // Encoding default is TEXT
    str_encoding_d = "TEXT" ;
  }

  if ( p_multiplier == 0 ) {
    i_multiplier = 100000 ;                                     // Default multiplier number
  }

  if ( !*str_prps_nm ) {                                        // Abort if Purpose is empty
    printf ("%s","JOB ABANDONDED - Missing purpose\n" ) ;
    exit(1) ;
  }

  if ( i_prps_no == 0 ) {                                       // Abort if Purpose no is null
    printf ("%s","JOB ABANDONDED - Missing purpose number\n" ) ;
    exit(1) ;
  }

  if ( *p_encoding ) {
    if ( strcmp ( p_encoding , "TEXT" )     != 0 &&
         strcmp ( p_encoding , "UTF-8" )    != 0 &&
         strcmp ( p_encoding , "UTF-16" )   != 0 &&
         strcmp ( p_encoding , "UTF-16LE" ) != 0 &&
         strcmp ( p_encoding , "UTF-16BE" ) != 0 &&
         strcmp ( p_encoding , "UTF-32" )   != 0 &&
         strcmp ( p_encoding , "UCS-2" )    != 0 &&
         strcmp ( p_encoding , "UCS-4" )    != 0 ) {
      printf ("%s >%s<\n","JOB ABANDONDED - Unknown encoding datatype:" , p_encoding ) ;   // Abort if unknown encoding datatype entered
      exit(1) ;
    }
  }

  if ( i_uni_enc_d != 0 ) {                                     // If unicode encoding is not zero
    if ( i_uni_enc_d != 4 &&                                    // Unicode Encoding must be 4 , 6 and 8
         i_uni_enc_d != 6 &&
         i_uni_enc_d != 8 ) {
      printf ( "%s >%s<\n" , "JOB ABANDONDED - Invalid CHARACTER ENCODING value" , i_uni_enc_d ) ;
      exit(1);
    }
  }

  if ( *str_nm_fmt_d ) {                                        // If Name Format string is not empty
    if ( ( strcmp ( str_nm_fmt_d , "L" ) != 0 ) &&              // Name Format must be L or R
         ( strcmp ( str_nm_fmt_d , "R" ) != 0 ) ) {
      printf ( "%s >%s<\n" , "JOB ABANDONDED - Invalid NAMEFORMAT value - must be L or R" , str_nm_fmt_d ) ;
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
      if ( strcmp ( str_adjwei_d , "Person_Name" )       == 0 ||
           strcmp ( str_adjwei_d , "Organization_Name" ) == 0 ||
           strcmp ( str_adjwei_d , "Address_Part1" )     == 0 ||
           strcmp ( str_adjwei_d , "Address_Part2" )     == 0 ||
           strcmp ( str_adjwei_d , "Postal_Area" )       == 0 ||
           strcmp ( str_adjwei_d , "Telephone_Number" )  == 0 ||
           strcmp ( str_adjwei_d , "Date" )              == 0 ||
           strcmp ( str_adjwei_d , "Id" )                == 0 ||
           strcmp ( str_adjwei_d , "Attribute1" )        == 0 ||
           strcmp ( str_adjwei_d , "Attribute2" )        == 0 ) {

        sprintf ( a_adj_wei , "%s%s%+d" , S_K_adjwei , str_adjwei_d , i_adjwei_val_d ) ;
      }
      else {
        printf ("%s >%s< %s >%d<" , "JOB ABANDONDED - Unknown ADJWEIGHT: Output file created without ADJWEIGHT field" , str_adjwei_d ,"and its value" , i_adjwei_val_d ) ;
        exit(1) ;
      }
  }
  else if ( *str_adjwei_d && i_adjwei_val_d == 0 ){             // If ADJWEIGHT value is empty
    if ( strcmp ( str_adjwei_d , "Person_Name" )       == 0 ||
         strcmp ( str_adjwei_d , "Organization_Name" ) == 0 ||
         strcmp ( str_adjwei_d , "Address_Part1" )     == 0 ||
         strcmp ( str_adjwei_d , "Address_Part2" )     == 0 ||
         strcmp ( str_adjwei_d , "Postal_Area" )       == 0 ||
         strcmp ( str_adjwei_d , "Telephone_Number" )  == 0 ||
         strcmp ( str_adjwei_d , "Date" )              == 0 ||
         strcmp ( str_adjwei_d , "Id" )                == 0 ||
         strcmp ( str_adjwei_d , "Attribute1" )        == 0 ||
         strcmp ( str_adjwei_d , "Attribute2" )        == 0 ) {

      printf ("%s" , "JOB ABANDONDED - Syntax error in weight adjustment: Output file created without ADJWEIGHT field and its value\n" ) ;
    }
    else {
      printf ("%s >%s< %s >%d<" , "JOB ABANDONDED - Unknown ADJWEIGHT: Output file created without ADJWEIGHT field" , str_adjwei_d ,"and its value" , i_adjwei_val_d ) ;
      exit(1) ;
    }
  }

  // Check Input file directory ends with backslash or forward slash , if not add it
  if(strchr(p_infdir,'/')) {
    i_len_of_dir = strlen(p_infdir) ;                           // Length of input file directory
    c_flg_slash  = p_infdir[i_len_of_dir-1] ;                   // Last character of a String is / forward slash
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
    c_flg_slash  = p_outfdir[i_len_of_dir-1] ;                  // Last character of a String is / forward slash
    if( c_flg_slash != S_K_forward_slash[0]) {
      p_outfdir = strcat(p_outfdir,S_K_forward_slash) ;         // Concatenate forward slash to the output file directory
    }
  }

  if(strchr(p_outfdir,'\\')) {
    i_len_of_dir = strlen(p_outfdir) ;                          // Length of input file directory
    c_flg_slash  = p_outfdir[i_len_of_dir-1] ;                  // Last character of a String is / back slash
    if(c_flg_slash != S_K_back_slash[0]) {
      p_outfdir = strcat(p_outfdir,S_K_back_slash);             // Concatenate back slash to the output file directory
    }
  }

  // Check log file directory ends with backslash or forward slash, if not add it
  if(strchr(p_logfdir,'/')) {
    i_len_of_dir = strlen(p_logfdir) ;                          // Length of input file directory
    c_flg_slash  = p_logfdir[i_len_of_dir-1] ;                  // Last character of a String is / forward slash
    if(c_flg_slash != S_K_forward_slash[0]) {
      p_logfdir = strcat(p_logfdir,S_K_forward_slash) ;         // Concatenate forward slash to the log file directory
    }
  }

  if(strchr(p_logfdir,'\\')) {
    i_len_of_dir = strlen(p_logfdir) ;                          // Length of input file directory
    c_flg_slash  = p_logfdir[i_len_of_dir-1] ;                  // Last character of a String is / back slash
    if(c_flg_slash != S_K_back_slash[0]) {
      p_logfdir = strcat(p_logfdir,S_K_back_slash) ;            // Concatenate back slash to the log file directory
    }
  }

  // Files Names
  sprintf( a_str_input_file , "%d%d.mst" , p_data_set , p_run_time ) ;
  sprintf( a_str_output_file , "%d%d_%d.ost" , p_data_set , p_run_time , i_prps_no ) ;
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
// To open a session to the dds-name3 service

  long    l_sockh ,                                             // Set to -1
  long    *l_session_id ,                                       // Should be -1 on the ddsn3 open call ,or opening a new session
  char    *str_sysName ,                                        // Defines location of the population rules
  char    *str_population ,                                     // Country name
  char    *str_controls                                         // Controls
 ) {

  long    l_rc ;                                                // Indicate success or failure of open / close sesions
  char    a_rsp_code[ SSA_SI_RSP_SZ ] ;                         // Indicates the success or failure of a call to dds-name3
  char    a_dds_msg[ SSA_SI_SSA_MSG_SZ ] ;                      // Error Message

  /* ssan3_open
     This function open and initiates an dds-name3 session in preparation for using further API functions.
     It can also be used to set or override the SSAPR and TIMEOUT environment variables */
  l_rc = ssan3_open
  (
    l_sockh ,                                                   // Set to -1 as not calling the dds-NAME3 server
    l_session_id ,                                              // Should be -1 on the ssan3 open call ,or opening a new session
    str_sysName ,                                               // Defines location of the population rules
    str_population ,                                            // Country name
    str_controls ,                                              // Controls
    a_rsp_code ,                                                // Indicates the success or failure of a call to dds-name3
    SSA_SI_RSP_SZ ,                                             // Size of respose code
    a_dds_msg ,                                                 // Error Message
    SSA_SI_SSA_MSG_SZ                                           // Size of error message
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
// To compare and match two records

  long    l_sockh ,                                             // Set to -1 as not calling the dds-NAME3 server
  long    *l_session_id ,                                       // Should be -1 on the ssan3 open call ,or opening a new session
  char    *str_sysName ,                                        // Defines location of the population rules
  char    *str_population ,                                     // Country name
  char    *str_controls ,                                       // Controls
  char    *str_search ,                                         // Search data
  long    l_searchLength ,                                      // Length of search data
  char    *str_searchEncType ,                                  // Encoding datatype of search data
  char    *str_file ,                                           // File data
  long    l_fileLength ,                                        // Length of file data
  char    *str_fileEncType ,                                    // Encoding datatype of file data
  char    *str_ID_search ,                                      // Id of search data
  char    *str_ID_file ,                                        // Id of file data
  int     i_prps_no ,                                           // Purpose no
  char    *str_abv_mtc_lvl                                      // Match levels abbrevation
 ) {

  long    l_rc ;                                                // Indicate success or failure of open / close sesions
  char    a_rsp_code[ SSA_SI_RSP_SZ ] ;                         //
  char    a_dds_msg[ SSA_SI_SSA_MSG_SZ ] ;                      // Error message
  char    a_score[ SSA_SI_SCORE_SZ ] ;                          // Score
  char    a_decision[ SSA_SI_DECISION_SZ ] ;                    // Decision

  /*Used to get a score and match decision for two records, a search
    record and a file record. */
  l_rc = ssan3_match_encoded
  (
    l_sockh ,                                                   // Set to -1 as not calling the dds-NAME3 server
    l_session_id ,                                              // Should be -1 on the ssan3 open call ,or opening a new session
    str_sysName ,                                               // System name
    str_population ,                                            // Population
    str_controls ,                                              // Controls
    a_rsp_code ,                                                // Response code
    SSA_SI_RSP_SZ ,                                             // Length of response code
    a_dds_msg ,                                                 // Error message
    SSA_SI_SSA_MSG_SZ ,                                         // Length of error message
    str_search ,                                                // Search data
    l_searchLength ,                                            // Length of search data
    str_searchEncType ,                                         // Encryption type of search data
    str_file ,                                                  // File data
    l_fileLength ,                                              // Length of file data
    str_fileEncType ,                                           // Encryption type of file data
    a_score ,                                                   // Score
    SSA_SI_SCORE_SZ ,                                           // Size of score
    a_decision ,                                                // Decision
    SSA_SI_DECISION_SZ                                          // Size of Decision
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

  fprintf
  (
    f_output_fopen_status ,
    "%s\t%s\t%s\t%s\t%d\t%s\n" ,
    str_ID_search ,                                             // Search id
    str_ID_file ,                                               // File id
    a_decision ,                                                // Decision
    a_score ,                                                   // Score
    i_prps_no ,                                                 // Purpose number
    str_abv_mtc_lvl                                             // Match level
  ) ;

  l_rc = 0 ;
  goto SUB_RETURN ;

SUB_RETURN:
  return ( l_rc ) ;

}
/**********************************************************************
 End of subroutine s_test_dds_match                                   *
**********************************************************************/

static long s_test_dds_close (
// To close an open session to dds-name3

  long    l_sockh ,                                             // Set to -1 as not calling the dds-name3 server
  long    *l_session_id ,                                       // Should be -1 on the ddsn3 open call ,or opening a new session
  char    *str_sysName ,                                        // Defines location of the population rules
  char    *str_population ,                                     // Country name
  char    *str_controls                                         // Controls
 ) {

  long    l_rc ;                                                // Indicate success or failure of open / close sesions
  char    a_rsp_code[ SSA_SI_RSP_SZ ] ;                         // Indicates the success or failure of a call to dds-name3
  char    a_dds_msg[ SSA_SI_SSA_MSG_SZ ] ;                      // Error Message

  s_date_time( ) ;                                              // Call date and time subroutine

  /* ssan3_close
     Close the dds-name3 session and releases memory.
     This session is then available for reuse. */
  l_rc = ssan3_close
  (
    l_sockh ,                                                   // Set to -1 as not calling the dds-name3 server
    l_session_id ,                                              // Should be -1 on the ssan3 open call ,or opening a new session
    str_sysName ,                                               // Defines location of the population rules
    str_population ,                                            // Country name
    str_controls ,                                              // Controls
    a_rsp_code ,                                                // Indicates the success or failure of a call to ssa-name3
    SSA_SI_RSP_SZ ,                                             // Size of respose code
    a_dds_msg ,                                                 // Error Message
    SSA_SI_SSA_MSG_SZ                                           // Size of error message
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
/* Error occurred in subroutines s_test_dds_open, s_test_dds_match or
  s_test_dds_close */

  fprintf ( f_log_fopen_status , "Error occurred in '%s'\n" , func ) ;
  printf ( "Error occurred in '%s'\n" , func ) ;
  exit ( 1 ) ;

}
/**********************************************************************
 End of subroutine s_doExit                                           *
**********************************************************************/

static void s_GMtc_open ( ) {
/* s_test_dds_open subroutine called in s_GMtc_open subroutine and
   assign subroutine parameters*/

  l_rc = s_test_dds_open
  (
    l_sockh ,                                                   // Set to -1 as not calling the dds-name3 server
    &l_session_id ,                                             // Should be -1 on the ddsn3 open call ,or opening a new session
    ( !*p_system_nm ? str_system_nm_d : p_system_nm ) ,         // System name parameter is empty used default value
    ( !*p_population ? str_popln_nm_d : p_population ) ,        // Population parameter is empty used default value
    ""                                                          // Controls
  ) ;

  if ( 0 != l_rc )
    s_doExit ( "s_test_dds_open" ) ;

}
/**********************************************************************
 End of subroutine s_GMtc_open                                        *
**********************************************************************/

static void s_GMtc_matches (
/* s_test_dds_match subroutine called in s_GMtc_matches subroutine
   This subroutine taken three match levels abbrevation in an array
   and purpose.*/

char *str_search_data ,                                         // Search data
char *str_file_data   ,                                         // file data
int  i_src_tag_dta_len ,                                        // Length of search data
int  i_fle_tag_dta_len ,                                        // Length of file data
char *str_ID_search ,                                           // Id of search data
char *str_ID_file ,                                             // Id of file data
char *str_Typical ,                                             // Match level Typical
char *str_Conservative ,                                        // Match level Conservative
char *str_Loose ,                                               // Match level Loose
int  i_purpose_no ,                                             // Purpose no
char *str_abv_ty ,                                              // Abbrevation of Typical (T)
char *str_abv_con ,                                             // Abbrevation of Typical (C)
char *str_abv_lse                                               // Abbrevation of Typical (L)

) {

  // Purpose
  char *str_purpose[3] = { str_Typical , str_Conservative , str_Loose } ;

  // Match levels abbrevarion T(Typical) , C(Conservative) , L(Loose)
  char *str_abv_mtc_lvl[3] = { str_abv_ty , str_abv_con , str_abv_lse } ;

  for ( i_idx = 0 ; i_idx <= 2 ; i_idx++ ) {
    l_rc = s_test_dds_match
    (
      l_sockh ,                                                 // Set to -1 as not calling the dds-name3 server
      &l_session_id ,                                           // Should be -1 on the ddsn3 open call ,or opening a new session
      ( !*p_system_nm ? str_system_nm_d : p_system_nm ) ,       // System name parameter is empty used default value
      ( !*p_population ? str_popln_nm_d : p_population ) ,      // Population parameter is empty used default value
      str_purpose[ i_idx ] ,                                    // Purpose with three match level
      str_search_data ,                                         // Search data
      i_src_tag_dta_len ,                                       // Length of search data
      ( !*p_encoding ? str_encoding_d : p_encoding ) ,          // Encoding data type of search data
      str_file_data ,                                           // File data
      i_fle_tag_dta_len ,                                       // Length of file data
      ( !*p_encoding ? str_encoding_d : p_encoding ) ,          // Encoding data type of file data
      str_ID_search ,                                           // Id of search data
      str_ID_file ,                                             // Id of file data
      i_purpose_no ,                                            // Purpose no
      str_abv_mtc_lvl[ i_idx ]                                  // Abbrevation of match levels
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
    l_sockh ,                                                   // Set to -1 as not calling the dds-name3 server
    &l_session_id ,                                             // Should be -1 on the ddsn3 open call ,or opening a new session
    ( !*p_system_nm ? str_system_nm_d : p_system_nm ) ,         // System name parameter is empty used default value
    ( !*p_population ? str_popln_nm_d : p_population ) ,        // Population parameter is empty used default value
    ""
  ) ;

  if ( 0 != l_rc )
    s_doExit ( "s_test_dds_close" ) ;

}
/**********************************************************************
 End of subroutine s_GMtc_close                                       *
**********************************************************************/

int main ( int argc , char *argv[] ) {
//Main function

t_start_time = clock( ) ;                                       // Start time
s_getParameter ( argc , argv ) ;                                // Subroutine to get parameter

if ( *str_delimeter_d ) {                                       // If delimeter parameter is not empty
  sprintf ( a_Id ,"%sId%s", str_delimeter_d , str_delimeter_d ) ;
}

fprintf ( f_log_fopen_status, "------ Run Parameters ------\n" ) ;
if ( p_data_set != 0 ) {                                        // If data set number is not empty
  fprintf ( f_log_fopen_status , "Data set number        : %d\n", p_data_set ) ;
}

if ( p_run_time != 0 ) {                                        // If run time number is not empty
  fprintf ( f_log_fopen_status , "Run time number        : %d\n", p_run_time ) ;
}

if ( *p_purpose ) {                                             // If purpose name is not empty
  fprintf ( f_log_fopen_status , "Purpose                : %s\n" , p_purpose ) ;
}

if ( p_prpsno != 0 ) {                                         // If purpose number is not empty
  fprintf ( f_log_fopen_status , "Purpose number             : %d\n" , p_prpsno ) ;
}

if ( !*p_system_nm ) {                                          // If System name is non empty
  fprintf ( f_log_fopen_status , "System name            : Missing- Default:%s\n", str_system_nm_d ) ;
}
else {
  fprintf ( f_log_fopen_status , "System name            : %s\n", p_system_nm ) ;
}

if ( !*p_population ) {                                         // If Population is non empty
  fprintf ( f_log_fopen_status , "Population             : Missing- Default:%s\n", str_popln_nm_d ) ;
}
else {
  fprintf ( f_log_fopen_status , "Population             : %s\n", p_population ) ;
}

if ( *p_infdir ) {                                              // If Input file directory is non empty
  fprintf ( f_log_fopen_status , "Input File Directory   : %s\n", p_infdir ) ;
}

if ( *p_outfdir ) {                                             // If Output file directory is non empty
  fprintf ( f_log_fopen_status , "Output File Directory  : %s\n", p_outfdir ) ;
}

if ( *p_logfdir ) {                                             // If Log file directory is non empty
  fprintf ( f_log_fopen_status , "Log File Directory     : %s\n", p_logfdir ) ;
}

if ( !*p_delimiter ) {
  fprintf ( f_log_fopen_status , "Delimiter              : Missing- Default:%s\n" , str_delimeter_d ) ;
}
else {
  fprintf ( f_log_fopen_status , "Delimiter              : %s\n" , p_delimiter ) ;
}

if ( !*p_encoding ) {                                           // If Encoding datatype is non empty
  fprintf ( f_log_fopen_status , "Encoding datatype      : Missing- Default:%s\n", str_encoding_d ) ;
}
else {
  fprintf ( f_log_fopen_status , "Encoding datatype      : %s\n", p_encoding ) ;
}

if ( i_uni_enc_d != 0 ) {                                       // If Unicode encoding is not zero
  fprintf ( f_log_fopen_status , "Unicode encoding       : %d\n" , i_uni_enc_d ) ;
}

if ( *str_nm_fmt_d ) {                                          // If name format is not empty
  fprintf ( f_log_fopen_status , "Name format            : %s\n" , str_nm_fmt_d ) ;
}

if ( p_acc_lmt != 0 ) {                                         // If accept limit is not zero
  fprintf ( f_log_fopen_status , "Accept Limit (+/-nn)   : %+d\n" , p_acc_lmt ) ;
}

if ( p_rej_lmt != 0 ) {                                         // If reject limit is not zero
  fprintf ( f_log_fopen_status , "Reject Limit (+/-nn)   : %+d\n" , p_rej_lmt ) ;
}

if ( *str_adjwei_d ) {                                          // If adjweight is not empty
  fprintf ( f_log_fopen_status , "Adjweight              : %s\n" , str_adjwei_d ) ;
}

if ( i_adjwei_val_d != 0 ) {                                    // If adjweight value is not zero
  fprintf ( f_log_fopen_status , "Adjweight value (+/-nn): %+d\n" , i_adjwei_val_d ) ;
}

if ( i_verbose_flg == 1 ) {
  if ( p_multiplier == 0 ) {
    fprintf ( f_log_fopen_status , "Multiplier             : Missing- Default :%d\n" , i_multiplier ) ;
  }
  else {
    fprintf ( f_log_fopen_status , "Multiplier             : %d\n" , p_multiplier ) ;
  }
}

fprintf ( f_log_fopen_status ,   "Verbose                : %s\n", ( i_verbose_flg == 1 ? "Yes" : "No" ) ) ;

fprintf ( f_log_fopen_status, "\n------ File Names ------" ) ;
fprintf ( f_log_fopen_status, "\nInput file name       : %s", a_str_input_file ) ;
fprintf ( f_log_fopen_status, "\nOutput file name      : %s", a_str_output_file ) ;
fprintf ( f_log_fopen_status, "\nLog file name         : %s\n", a_str_log_file ) ;

fprintf ( f_log_fopen_status, "\n------ Environment variable ------" ) ;
fprintf ( f_log_fopen_status, "\nSSATOP : %s" , getenv("SSATOP") ) ;
fprintf ( f_log_fopen_status, "\nSSAPR  : %s\n" , getenv("SSAPR") ) ;

s_GMtc_open ( ) ;                                               // Open get match connection

  // Read a input file line by line
while( fgets ( str_current_rec , sizeof ( str_current_rec ) , f_input_fopen_status ) ) {

  int i_fld_rec_knt ;                                           // Number of tab delimited fields count in current record of input file
  ++ i_rec_number ;                                             // Record number

  // Tab delimited split
  i_fld_rec_knt =  sscanf
                  (
                    str_current_rec ,
                    "%[^\t]\t%[^\t]\t%[^\t]\t%[^\n]" ,
                    str_src_tag_dta ,
                    str_fle_tag_dta ,
                    str_src_id ,
                    str_fle_id
                  );

  i_src_tag_dta_len = strlen( str_src_tag_dta ) ;               // Length of search data
  i_fle_tag_dta_len = strlen( str_fle_tag_dta ) ;               // Length of file data

/* SEARCH DATA CONTAIN FOLLOWING FIELD COUNTS *****************************************************/

  if ( strstr ( str_src_tag_dta , "Person_Name" )       != NULL ) { i_pn_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Organization_Name" ) != NULL ) { i_on_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Address_Part1" )     != NULL ) { i_adp1_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Address_Part2" )     != NULL ) { i_adp2_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Postal_Area" )       != NULL ) { i_pa_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Telephone_Number" )  != NULL ) { i_tel_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Date" )              != NULL ) { i_dte_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Id" )                != NULL ) { i_id_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Attribute1" )        != NULL ) { i_att1_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Attribute2" )        != NULL ) { i_att2_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Filter1" )           != NULL ) { i_ft1_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Filter2" )           != NULL ) { i_ft2_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Filter3" )           != NULL ) { i_ft3_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Filter4" )           != NULL ) { i_ft4_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Filter5" )           != NULL ) { i_ft5_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Filter6" )           != NULL ) { i_ft6_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Filter7" )           != NULL ) { i_ft7_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Filter8" )           != NULL ) { i_ft8_in_src_rec_knt ++ ; }
  if ( strstr ( str_src_tag_dta , "Filter9" )           != NULL ) { i_ft9_in_src_rec_knt ++ ; }
/* END SEARCH DATA CONTAIN FOLLOWING FIELD COUNTS *************************************************/

/* FILE DATA CONTAIN FOLLOWING FIELD COUNTS *******************************************************/

  if ( strstr ( str_fle_tag_dta , "Person_Name" )       != NULL ) { i_pn_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Organization_Name" ) != NULL ) { i_on_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Address_Part1" )     != NULL ) { i_adp1_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Address_Part2" )     != NULL ) { i_adp2_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Postal_Area" )       != NULL ) { i_pa_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Telephone_Number" )  != NULL ) { i_tel_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Date" )              != NULL ) { i_dte_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Id" )                != NULL ) { i_id_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Attribute1" )        != NULL ) { i_att1_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Attribute2" )        != NULL ) { i_att2_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Filter1" )           != NULL ) { i_ft1_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Filter2" )           != NULL ) { i_ft2_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Filter3" )           != NULL ) { i_ft3_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Filter4" )           != NULL ) { i_ft4_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Filter5" )           != NULL ) { i_ft5_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Filter6" )           != NULL ) { i_ft6_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Filter7" )           != NULL ) { i_ft7_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Filter8" )           != NULL ) { i_ft8_in_fle_rec_knt ++ ; }
  if ( strstr ( str_fle_tag_dta , "Filter9" )           != NULL ) { i_ft9_in_fle_rec_knt ++ ; }
/* END FILE DATA CONTAIN FOLLOWING FIELD COUNTS ***************************************************/

  if ( i_fld_rec_knt == 4 ) {
    // Check search data and file data contain <delmiter>Id<delimiter> or not
    if ( strstr
         (
           str_src_tag_dta , a_Id                               // If delimeter parameter is non empty find id in search data record
         ) != NULL &&
         strstr
         (
           str_fle_tag_dta , a_Id                               // If delimeter parameter is non empty find id in file data record
         ) != NULL
       ) {

       if ( i_verbose_flg == 1 ) {                              // If Verbose flag is On

          // Display so many records in so many seconds to execute

          if ( i_rec_number == i_multiplier ) {                 // If Records number equals Multiplier number
            t_end_time = clock( ) - t_start_time ;              // End time
            t_time_taken = ( ( double )t_end_time )/CLOCKS_PER_SEC ;              // In seconds
            printf( "\nDisplay %d records in %.f seconds to execute \n", i_multiplier , t_time_taken ) ;      // Print time

            i_multiplier = i_multiplier * 2 ;                   // Multiplier value multiply by 2
          }
       }

      /* MATCH LEVEL WITH ACCEPT LIMIT OR REJECT LIMIT AND CONTROLS WITH THREE MATCH LEVEL***********/

      if ( p_acc_lmt == 0 && p_rej_lmt == 0 ) {                 // Both accept limit and reject limit are zero
        sprintf ( a_mtc_lvl_typ_acc_rej ,"%s" , S_K_mtc_lvl_ty ) ;
      }
      else if ( p_acc_lmt != 0 && p_rej_lmt != 0 ) {            // Both accept limit and reject limit are non zero
        sprintf ( a_mtc_lvl_typ_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_ty , p_acc_lmt , p_rej_lmt ) ;
      }
      else if ( p_acc_lmt == 0 && p_rej_lmt != 0 ) {            // Accept limit are zero and reject limit are not zero
        sprintf ( a_mtc_lvl_typ_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_ty , p_acc_lmt , p_rej_lmt ) ;
      }
      else if ( p_acc_lmt !=0  && p_rej_lmt == 0 ) {            // Accept limit are non zero and reject limit are zero
        sprintf ( a_mtc_lvl_typ_acc_rej ,"%s%+d" , S_K_mtc_lvl_ty , p_acc_lmt ) ;
      }

      if ( p_acc_lmt == 0 && p_rej_lmt == 0 ) {                 // Both accept limit and reject limit are zero
        sprintf ( a_mtc_lvl_con_acc_rej ,"%s" , S_K_mtc_lvl_con ) ;
      }
      else if ( p_acc_lmt != 0 && p_rej_lmt != 0 ) {            // Both accept limit and reject limit are non zero
        sprintf ( a_mtc_lvl_con_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_con , p_acc_lmt , p_rej_lmt ) ;
      }
      else if ( p_acc_lmt == 0 && p_rej_lmt != 0 ) {            // Accept limit are zero and reject limit are not zero
        sprintf ( a_mtc_lvl_con_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_con , p_acc_lmt , p_rej_lmt ) ;
      }
      else if ( p_acc_lmt !=0  && p_rej_lmt == 0 ) {            // Accept limit are non zero and reject limit are zero
        sprintf ( a_mtc_lvl_con_acc_rej ,"%s%+d" , S_K_mtc_lvl_con , p_acc_lmt ) ;
      }

      if ( p_acc_lmt == 0 && p_rej_lmt == 0 ) {                 // Both accept limit and reject limit are zero
        sprintf ( a_mtc_lvl_lse_acc_rej ,"%s" , S_K_mtc_lvl_lse ) ;
      }
      else if ( p_acc_lmt != 0 && p_rej_lmt != 0 ) {            // Both accept limit and reject limit are non zero
        sprintf ( a_mtc_lvl_lse_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_lse , p_acc_lmt , p_rej_lmt ) ;
      }
      else if ( p_acc_lmt == 0 && p_rej_lmt != 0 ) {            // Accept limit are zero and reject limit are not zero
        sprintf ( a_mtc_lvl_lse_acc_rej ,"%s%+d%+d" , S_K_mtc_lvl_lse , p_acc_lmt , p_rej_lmt ) ;
      }
      else if ( p_acc_lmt !=0  && p_rej_lmt == 0 ) {            // Accept limit are non zero and reject limit are zero
        sprintf ( a_mtc_lvl_lse_acc_rej ,"%s%+d" , S_K_mtc_lvl_lse , p_acc_lmt ) ;
      }

      sprintf
      (
         a_prps_ty ,                                            // Purpose with match level Typical
        "%s%s %s%s%s%s%s%s" ,
         S_K_prps ,                                             // PURPOSE= format
         str_prps_nm ,                                          // Purpose name
         S_K_mtc_lvl ,                                          // MATCH_LEVEL= format
         a_mtc_lvl_typ_acc_rej ,                                // Match level Typical with accept limit and reject limit
         a_uni_enc ,                                            // Unicode encoding format e.g UNICODE=4
         a_nm_fmt ,                                             // Name Format e.g NAMEFORMAT=R
         a_adj_wei ,
         a_delimeter                                            // Delimiter
      ) ;

      sprintf
      (
         a_prps_con ,                                           // Purpose with match level Conservative
         "%s%s %s%s%s%s%s%s" ,
         S_K_prps ,                                             // PURPOSE= format
         str_prps_nm ,                                          // Purpose name
         S_K_mtc_lvl ,                                          // MATCH_LEVEL= format
         a_mtc_lvl_con_acc_rej ,                                // Match level Conservative with accept limit and reject limit
         a_uni_enc ,                                            // Unicode encoding format e.g UNICODE=4
         a_nm_fmt ,                                             // Name Format e.g NAMEFORMAT=R
         a_adj_wei ,
         a_delimeter                                            // Delimiter
      ) ;

      sprintf
      (
         a_prps_lse ,                                           // Purpose with match level Loose
        "%s%s %s%s%s%s%s%s" ,
         S_K_prps ,                                             // PURPOSE= format
         str_prps_nm ,                                          // Purpose name
         S_K_mtc_lvl ,                                          // MATCH_LEVEL= format
         a_mtc_lvl_lse_acc_rej ,                                // Match level Loose with accept limit and reject limit
         a_uni_enc ,                                            // Unicode encoding format e.g UNICODE=4
         a_nm_fmt ,                                             // Name Format e.g NAMEFORMAT=R
         a_adj_wei ,
         a_delimeter                                            // Delimiter
      ) ;

      /* END MATCH LEVEL WITH ACCEPT LIMIT OR REJECT LIMIT AND CONTROLS WITH THREE MATCH LEVEL*******/

      if ( strstr ( p_purpose , "Address" ) != NULL ) {
        if ( strstr ( str_src_tag_dta , "Address_Part1" ) != NULL &&
             strstr ( str_fle_tag_dta , "Address_Part1" ) != NULL ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Address_Part1" ) == NULL ) {

            i_Add_prps_err_knt ++ ;                             // Address purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count
            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Address_Part1" ) == NULL ) {

            i_Add_prps_err_knt ++ ;                             // Address purpose error count
            i_tot_err_knt ++ ;                                  // Total errors count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End if Resident
      else if ( strstr ( p_purpose , "Contact" ) != NULL ) {
        if ( ( strstr ( str_src_tag_dta , "Person_Name" )       != NULL &&
               strstr ( str_src_tag_dta , "Organization_Name" ) != NULL &&
               strstr ( str_src_tag_dta , "Address_Part1" )     != NULL ) &&
             ( strstr ( str_fle_tag_dta , "Person_Name" )       != NULL &&
               strstr ( str_fle_tag_dta , "Organization_Name" ) != NULL &&
               strstr ( str_fle_tag_dta , "Address_Part1" )     != NULL )
           ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Person_Name" ) == NULL ||
               strstr ( str_src_tag_dta , "Organization_Name" ) == NULL ||
               strstr ( str_src_tag_dta , "Address_Part1" )     == NULL ) {

            i_Con_prps_err_knt ++ ;                             // Contact purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count
            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Person_Name" ) == NULL ||
               strstr ( str_fle_tag_dta , "Organization_Name" ) == NULL ||
               strstr ( str_fle_tag_dta , "Address_Part1" )     == NULL ) {

            i_Con_prps_err_knt ++ ;                             // Contact purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count
            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Contact
      else if ( strstr ( p_purpose , "Corp_Entity" ) != NULL ) {
        if ( strstr ( str_src_tag_dta , "Organization_Name" ) != NULL &&
             strstr ( str_fle_tag_dta , "Organization_Name" ) != NULL ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Organization_Name" ) == NULL ) {

            i_CEn_prps_err_knt ++ ;                             // Corp_Entity purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Organization_Name" ) == NULL ) {

            i_CEn_prps_err_knt ++ ;                             // Corp_Entity purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Corp_Entity
      else if ( strstr ( p_purpose , "Division" ) != NULL ) {
        if ( ( strstr ( str_src_tag_dta , "Organization_Name" ) != NULL   &&
               strstr ( str_src_tag_dta , "Address_Part1" )     != NULL ) &&
             ( strstr ( str_fle_tag_dta , "Organization_Name" ) != NULL   &&
               strstr ( str_fle_tag_dta , "Address_Part1" )     != NULL )
           ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Organization_Name" ) == NULL ||
               strstr ( str_src_tag_dta , "Address_Part1" )     == NULL ) {

            i_Div_prps_err_knt ++ ;                             // Division purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Organization_Name" ) == NULL ||
               strstr ( str_fle_tag_dta , "Address_Part1" ) == NULL ){

            i_Div_prps_err_knt ++ ;                             // Division purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Division
      else if ( strstr ( p_purpose , "Family" ) != NULL ) {
        if ( ( strstr ( str_src_tag_dta , "Person_Name" )      != NULL &&
               strstr ( str_src_tag_dta , "Address_Part1" )    != NULL &&
               strstr ( str_src_tag_dta , "Telephone_Number" ) != NULL ) &&
             ( strstr ( str_fle_tag_dta , "Person_Name" )      != NULL &&
               strstr ( str_fle_tag_dta , "Address_Part1" )    != NULL &&
               strstr ( str_fle_tag_dta , "Telephone_Number" ) != NULL )
           ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Person_Name" )      == NULL ||
               strstr ( str_src_tag_dta , "Address_Part1" )    == NULL ||
               strstr ( str_src_tag_dta , "Telephone_Number" ) == NULL ) {

            i_Fam_prps_err_knt ++ ;                             // Family purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Person_Name" )      == NULL ||
               strstr ( str_fle_tag_dta , "Address_Part1" )    == NULL ||
               strstr ( str_fle_tag_dta , "Telephone_Number" ) == NULL ) {

            i_Fam_prps_err_knt ++ ;                             // Family purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Family
      else if ( strstr ( p_purpose , "Fields" ) != NULL ) {
        if ( ( strstr ( str_src_tag_dta , "Id" )                != NULL ||
               strstr ( str_src_tag_dta , "Person_Name" )       != NULL ||
               strstr ( str_src_tag_dta , "Organization_Name" ) != NULL ||
               strstr ( str_src_tag_dta , "Address_Part1" )     != NULL ||
               strstr ( str_src_tag_dta , "Address_Part2" )     != NULL ||
               strstr ( str_src_tag_dta , "Postal_Area" )       != NULL ||
               strstr ( str_src_tag_dta , "Telephone_Number" )  != NULL ||
               strstr ( str_src_tag_dta , "Date" )              != NULL ||
               strstr ( str_src_tag_dta , "Attribute1" )        != NULL ||
               strstr ( str_src_tag_dta , "Attribute2" )        != NULL )
               &&
             ( strstr ( str_fle_tag_dta , "Id" )                != NULL ||
              strstr ( str_fle_tag_dta , "Person_Name" )        != NULL ||
              strstr ( str_fle_tag_dta , "Organization_Name" )  != NULL ||
              strstr ( str_fle_tag_dta , "Address_Part1" )      != NULL ||
              strstr ( str_fle_tag_dta , "Address_Part2" )      != NULL ||
              strstr ( str_fle_tag_dta , "Postal_Area" )        != NULL ||
              strstr ( str_fle_tag_dta , "Telephone_Number" )   != NULL ||
              strstr ( str_fle_tag_dta , "Date" )               != NULL ||
              strstr ( str_fle_tag_dta , "Attribute1" )         != NULL ||
              strstr ( str_fle_tag_dta , "Attribute2" )         != NULL )
           ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count
          i_Fld_prps_err_knt ++ ;                               // Fields purpose error count

          fprintf
          (
            f_log_fopen_status ,
            "\nRecord no : %d Error Message : %s %s" ,
            i_rec_number , "Record does not contain all fields required for Purpose" , p_purpose
          ) ;
          fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_current_rec ) ;
        }
      } // End of else if Fields
      else if ( strstr ( p_purpose , "Filter1" )    != NULL ) {
        if ( strstr ( str_src_tag_dta , "Filter1" ) != NULL &&
             strstr ( str_fle_tag_dta , "Filter1" ) != NULL ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Filter1" ) == NULL ) {

            i_Ftr1_prps_err_knt ++ ;                            // Filter1 purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Filter1" ) == NULL ) {

            i_Ftr1_prps_err_knt ++ ;                            // Filter1 purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Filter1
      else if ( strstr ( p_purpose , "Filter2" ) != NULL ) {
        if ( strstr ( str_src_tag_dta , "Filter2" ) != NULL &&
             strstr ( str_fle_tag_dta , "Filter2" ) != NULL ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Filter2" ) == NULL ) {

            i_Ftr2_prps_err_knt ++ ;                            // Filter2 purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Filter2" ) == NULL ) {

            i_Ftr2_prps_err_knt ++ ;                            // Filter2 purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Filter2
      else if ( strstr ( p_purpose , "Filter3" ) != NULL ) {
        if ( strstr ( str_src_tag_dta , "Filter3" ) != NULL &&
             strstr ( str_fle_tag_dta , "Filter3" ) != NULL ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Filter3" ) == NULL ) {

            i_Ftr3_prps_err_knt ++ ;                            // Filter3 purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Filter3" ) == NULL ) {

            i_Ftr3_prps_err_knt ++ ;                            // Filter3 purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Filter3
      else if ( strstr ( p_purpose , "Filter4" ) != NULL ) {
        if ( strstr ( str_src_tag_dta , "Filter4" ) != NULL &&
             strstr ( str_fle_tag_dta , "Filter4" ) != NULL ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Filter4" ) == NULL ) {

            i_Ftr4_prps_err_knt ++ ;                            // Filter4 purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Filter4" ) == NULL ) {

            i_Ftr4_prps_err_knt ++ ;                            // Filter4 purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Filter4
      else if ( strstr ( p_purpose , "Filter5" ) != NULL ) {
        if ( strstr ( str_src_tag_dta , "Filter5" ) != NULL &&
             strstr ( str_fle_tag_dta , "Filter5" ) != NULL ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Filter5" ) == NULL ) {

            i_Ftr5_prps_err_knt ++ ;                            // Filter5 purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Filter5" ) == NULL ) {

            i_Ftr5_prps_err_knt ++ ;                            // Filter5 purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Filter5
      else if ( strstr ( p_purpose , "Filter6" ) != NULL ) {
        if ( strstr ( str_src_tag_dta , "Filter6" ) != NULL &&
             strstr ( str_fle_tag_dta , "Filter6" ) != NULL ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Filter6" ) == NULL ) {

            i_Ftr6_prps_err_knt ++ ;                            // Filter6 purpose error counta
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Filter6" ) == NULL ) {

            i_Ftr6_prps_err_knt ++ ;                            // Filter6 purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Filter6
      else if ( strstr ( p_purpose , "Filter7" ) != NULL ) {
        if ( strstr ( str_src_tag_dta , "Filter7" ) != NULL &&
             strstr ( str_fle_tag_dta , "Filter7" ) != NULL ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Filter7" ) == NULL ) {

            i_Ftr7_prps_err_knt ++ ;                            // Filter7 purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Filter7" ) == NULL ) {

            i_Ftr7_prps_err_knt ++ ;                            // Filter7 purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Filter7
      else if ( strstr ( p_purpose , "Filter8" ) != NULL ) {
        if ( strstr ( str_src_tag_dta , "Filter8" ) != NULL &&
             strstr ( str_fle_tag_dta , "Filter8" ) != NULL ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Filter8" ) == NULL ) {

            i_Ftr8_prps_err_knt ++ ;                            // Filter8 purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Filter8" ) == NULL ) {

            i_Ftr8_prps_err_knt ++ ;                            // Filter8 purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Filter8
      else if ( strstr ( p_purpose , "Filter9" ) != NULL ) {
        if ( strstr ( str_src_tag_dta , "Filter9" ) != NULL &&
             strstr ( str_fle_tag_dta , "Filter9" ) != NULL ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Filter9" ) == NULL ) {

            i_Ftr9_prps_err_knt ++ ;                            // Filter9 purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Filter9" ) == NULL ) {

            i_Ftr9_prps_err_knt ++ ;                            // Filter9 purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Filter9
      else if ( strstr ( p_purpose , "Household" ) != NULL ) {
        if ( ( strstr ( str_src_tag_dta , "Person_Name" )   != NULL &&
               strstr ( str_src_tag_dta , "Address_Part1" ) != NULL ) &&
             ( strstr ( str_fle_tag_dta , "Person_Name" )   != NULL &&
               strstr ( str_fle_tag_dta , "Address_Part1" ) != NULL )
           ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Person_Name" )   == NULL ||
               strstr ( str_src_tag_dta , "Address_Part1" ) == NULL ) {

            i_Hsho_prps_err_knt ++ ;                            // House purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Person_Name" )   == NULL ||
               strstr ( str_fle_tag_dta , "Address_Part1" ) == NULL ) {

            i_Hsho_prps_err_knt ++ ;                            // House purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Household
      else if ( strstr ( p_purpose , "Individual" ) != NULL ) {
        if ( ( strstr ( str_src_tag_dta , "Person_Name" ) != NULL &&
             ( strstr ( str_src_tag_dta , "Date" )        != NULL ||
               strstr ( str_src_tag_dta , "Id" )          != NULL ) ) &&

             ( strstr ( str_fle_tag_dta , "Person_Name" ) != NULL &&
             ( strstr ( str_fle_tag_dta , "Date" ) != NULL ||
               strstr ( str_fle_tag_dta , "Id" )   != NULL ) ) ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Person_Name" ) == NULL ||
             ( strstr ( str_src_tag_dta , "Date" ) == NULL ||
               strstr ( str_src_tag_dta , "Id" )   == NULL ) ) {

            i_ind_prps_err_knt ++ ;                             // Individual purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Person_Name" ) == NULL ||
             ( strstr ( str_fle_tag_dta , "Date" ) == NULL ||
               strstr ( str_fle_tag_dta , "Id" )   == NULL ) ) {

            i_ind_prps_err_knt ++ ;                             // Individual purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Individual
      else if ( strstr ( p_purpose , "Organization" ) != NULL ) {
        if ( strstr ( str_src_tag_dta , "Organization_Name" ) != NULL &&
             strstr ( str_fle_tag_dta , "Organization_Name" ) != NULL ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Organization_Name" ) == NULL ) {

            i_OrgN_prps_err_knt ++ ;                            // Organization name purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Organization_Name" ) == NULL ) {

            i_OrgN_prps_err_knt ++ ;                            // Organization name purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Organization
      else if ( strstr ( p_purpose , "Person_Name" ) != NULL ) {
        if ( strstr ( str_src_tag_dta , "Person_Name" ) != NULL &&
             strstr ( str_fle_tag_dta , "Person_Name" ) != NULL ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Person_Name" ) == NULL ) {

            i_PerN_prps_err_knt ++ ;                            // Person_Name purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Person_Name" ) == NULL ) {

            i_PerN_prps_err_knt ++ ;                            // Person_Name purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Person_Name
      else if ( strstr ( p_purpose , "Resident" )  != NULL ) {
        if ( ( strstr ( str_src_tag_dta , "Person_Name" )   != NULL &&
               strstr ( str_src_tag_dta , "Address_Part1" ) != NULL ) &&
             ( strstr ( str_fle_tag_dta , "Person_Name" )   != NULL &&
               strstr ( str_fle_tag_dta , "Address_Part1" ) != NULL )
           ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          if ( str_src_id[0] != '\0' ) {
          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
       }
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Person_Name" ) == NULL ||
               strstr ( str_src_tag_dta , "Address_Part1" ) == NULL ) {\

            i_Res_prps_err_knt ++ ;                             // Resident purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Person_Name" ) == NULL ||
               strstr ( str_fle_tag_dta , "Address_Part1" ) == NULL ) {

            i_Res_prps_err_knt ++ ;                             // Resident purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Resident
      else if ( strstr ( p_purpose , "Wide_Contact" ) != NULL ) {
        if ( ( strstr ( str_src_tag_dta , "Person_Name" )       != NULL   &&
               strstr ( str_src_tag_dta , "Organization_Name" ) != NULL ) &&
             ( strstr ( str_fle_tag_dta , "Person_Name" )       != NULL   &&
               strstr ( str_fle_tag_dta , "Organization_Name" ) != NULL )
           ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Person_Name" ) == NULL ||
               strstr ( str_src_tag_dta , "Organization_Name" ) == NULL ) {

            i_WCon_prps_err_knt ++ ;                            // Wide_Contact purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Person_Name" ) == NULL ||
               strstr ( str_fle_tag_dta , "Organization_Name" ) == NULL ) {

            i_WCon_prps_err_knt ++ ;                            // Wide_Contact purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Wide_Contact
      else if ( strstr ( p_purpose , "Wide_Household" ) != NULL ) {
        if ( ( strstr ( str_src_tag_dta , "Person_Name" )      != NULL &&
               strstr ( str_src_tag_dta , "Address_Part1" )    != NULL &&
               strstr ( str_src_tag_dta , "Telephone_Number" ) != NULL ) &&

             ( strstr ( str_fle_tag_dta , "Person_Name" )      != NULL &&
               strstr ( str_fle_tag_dta , "Address_Part1" )    != NULL &&
               strstr ( str_fle_tag_dta , "Telephone_Number" ) != NULL )
           ) {

          char *str_ty  = "T" ;                                 // Match level Typical abbrevation
          char *str_con = "C" ;                                 // Match level Conservative abbrevation
          char *str_lse = "L" ;                                 // Match level Loose abbrevation

          i_op_rec_knt ++ ;                                     // Output records count

          s_GMtc_matches                                        // Call s_GMtc_matches subroutine
          (
            str_src_tag_dta ,                                   // Search tag data
            str_fle_tag_dta ,                                   // File tag data
            i_src_tag_dta_len ,                                 // Length of search tag data
            i_fle_tag_dta_len ,                                 // Length of file data
            str_src_id ,                                        // Id of search tag data
            str_fle_id ,                                        // Id of file tag data
            a_prps_ty ,                                         // Controls with Typical match level
            a_prps_con ,                                        // Controls with Conservative match level
            a_prps_lse ,                                        // Controls with Loose match level
            i_prps_no ,                                         // Purpose number
            str_ty ,                                            // Match level Typical abbrevation
            str_con ,                                           // Match level Conservative abbrevation
            str_lse                                             // Match level Loose abbrevation
          ) ;
        }
        else {

          i_tot_err_rec_knt ++ ;                                // Total error records count

          // Check error in search data
          if ( strstr ( str_src_tag_dta , "Person_Name" ) == NULL ||
               strstr ( str_src_tag_dta , "Address_Part1" )    == NULL ||
               strstr ( str_src_tag_dta , "Telephone_Number" ) == NULL ) {

            i_WHsho_prps_err_knt ++ ;                           // Wide_Household purpose error count
            i_src_rec_err_knt ++ ;                              // Search record errors count
            i_tot_err_knt ++ ;                                  // Total errors count

            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "Search record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
          }

          // Check error in file data
          if ( strstr ( str_fle_tag_dta , "Person_Name" ) == NULL ||
               strstr ( str_fle_tag_dta , "Address_Part1" )    == NULL ||
               strstr ( str_fle_tag_dta , "Telephone_Number" ) == NULL ) {

            i_WHsho_prps_err_knt ++ ;                           // Wide_Household purpose error count
            i_fle_rec_err_knt ++ ;                              // File record errors count
            i_tot_err_knt ++ ;                                  // Total errors count
            fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s %s" ,
              i_rec_number , "File record does not contain all fields required for Purpose" , p_purpose
            ) ;
            fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
          }
        }
      } // End of else if Wide_Household
      else {
        printf ( "Validate_purpose '%s' Validate_purpose failed\n" , p_purpose ) ;
        exit(1) ;
      } // End of else

    } // End If
    else {
      i_tot_err_rec_knt ++ ;                                    // Total error record count

      if ( strstr
                (
                  str_src_tag_dta , a_Id
                ) == NULL ) {

        i_id_err_rec_knt ++ ;                                   // Records error that does not contain Id
        i_id_src_rec_err_knt ++ ;                               // Count of errors in search record that does not contain Id
        i_tot_err_knt ++ ;                                      // Total errors count
        fprintf
            (
              f_log_fopen_status ,
              "Record no : %d Error Message : %s" ,
              i_rec_number , "Missing Id in search tag data"
            ) ;
        fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_src_tag_dta ) ;
      }

      if ( strstr
                (
                  str_fle_tag_dta , a_Id
                ) == NULL ) {
        i_id_err_rec_knt ++ ;                                   // Records error that does not contain Id
        i_id_fle_rec_err_knt ++ ;                               // Count of errors in file record that does not contain Id
        i_tot_err_knt ++ ;                                      // Total errors count
        fprintf
            (
              f_log_fopen_status ,
              "\nRecord no : %d Error Message : %s" ,
              i_rec_number , "Missing Id in file tag data"
            ) ;
        fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_fle_tag_dta ) ;
      }
    }
  }
  else {
    // If there are improper record then display error with current record
    i_tot_err_rec_knt ++ ;                                      // Total error record count
    fprintf ( f_log_fopen_status, "\nRecord no : %d Error Message : %s", i_rec_number ,"Improper record" ) ;
    fprintf ( f_log_fopen_status, "\nRecord    : %s\n", str_current_rec ) ;
  }

} // End while loop


/*PURPOSE ERRORS COUNT ****************************************************************************/

fprintf ( f_log_fopen_status , "\n------ Run Summary ------" ) ;

if ( i_rec_number != 0 ) {
  fprintf ( f_log_fopen_status , "\nTotal records read  : %d\n" , i_rec_number ) ;
}

fprintf (f_log_fopen_status , "\n") ;

/* RECORDS WITH FIELD IN SEARCH DATA **************************************************************/

if ( i_pn_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Person_Name in Search data        : %d\n" , i_pn_in_src_rec_knt   ) ;
}

if ( i_on_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Organization_Name in Search data  : %d\n" , i_on_in_src_rec_knt   ) ;
}

if ( i_adp1_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Address_Part1 in Search data      : %d\n" , i_adp1_in_src_rec_knt ) ;
}

if ( i_adp2_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Address_Part2 in Search data      : %d\n" , i_adp2_in_src_rec_knt ) ;
}

if ( i_pa_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Postal_Area in Search data        : %d\n" , i_pa_in_src_rec_knt   ) ;
}

if ( i_tel_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Telephone_Number in Search data   : %d\n" , i_tel_in_src_rec_knt  ) ;
}

if ( i_dte_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Date in Search data               : %d\n" , i_dte_in_src_rec_knt  ) ;
}

if ( i_id_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Id in Search data                 : %d\n" , i_id_in_src_rec_knt   ) ;
}

if ( i_att1_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Attribute1 in Search data         : %d\n" , i_att1_in_src_rec_knt ) ;
}

if ( i_att2_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Attribute2 in Search data         : %d\n" , i_att2_in_src_rec_knt ) ;
}

if ( i_ft1_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter1 in Search data            : %d\n" , i_ft1_in_src_rec_knt  ) ;
}

if ( i_ft2_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter2 in Search data            : %d\n" , i_ft2_in_src_rec_knt  ) ;
}

if ( i_ft3_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter3 in Search data            : %d\n" , i_ft3_in_src_rec_knt  ) ;
}

if ( i_ft4_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter4 in Search data            : %d\n" , i_ft4_in_src_rec_knt  ) ;
}

if ( i_ft5_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter5 in Search data            : %d\n" , i_ft5_in_src_rec_knt  ) ;
}

if ( i_ft6_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter6 in Search data            : %d\n" , i_ft6_in_src_rec_knt  ) ;
}

if ( i_ft7_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter7 in Search data            : %d\n" , i_ft7_in_src_rec_knt  ) ;
}

if ( i_ft9_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter8 in Search data            : %d\n" , i_ft9_in_src_rec_knt  ) ;
}

if ( i_ft9_in_src_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter9 in Search data            : %d\n" , i_ft9_in_src_rec_knt  ) ;
}
/* END RECORDS WITH FIELD IN SEARCH DATA **********************************************************/

if ( i_pn_in_fle_rec_knt   != 0 ||
     i_on_in_fle_rec_knt   != 0 ||
     i_adp1_in_fle_rec_knt != 0 ||
     i_adp2_in_fle_rec_knt != 0 ||
     i_pa_in_fle_rec_knt   != 0 ||
     i_tel_in_fle_rec_knt  != 0 ||
     i_dte_in_fle_rec_knt  != 0 ||
     i_id_in_fle_rec_knt   != 0 ||
     i_att1_in_fle_rec_knt != 0 ||
     i_att2_in_fle_rec_knt != 0 ||
     i_ft1_in_fle_rec_knt  != 0 ||
     i_ft2_in_fle_rec_knt  != 0 ||
     i_ft3_in_fle_rec_knt  != 0 ||
     i_ft4_in_fle_rec_knt  != 0 ||
     i_ft5_in_fle_rec_knt  != 0 ||
     i_ft6_in_fle_rec_knt  != 0 ||
     i_ft7_in_fle_rec_knt  != 0 ||
     i_ft9_in_fle_rec_knt  != 0 ||
     i_ft9_in_fle_rec_knt  != 0 ) {

     fprintf (f_log_fopen_status , "\n") ;                      // New line
}

/* RECORDS WITH FIELD IN FILE DATA ****************************************************************/

if ( i_pn_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Person_Name in File data        : %d\n" , i_pn_in_fle_rec_knt   ) ;
}

if ( i_on_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Organization_Name in File data  : %d\n" , i_on_in_fle_rec_knt   ) ;
}

if ( i_adp1_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Address_Part1 in File data      : %d\n" , i_adp1_in_fle_rec_knt ) ;
}

if ( i_adp2_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Address_Part2 in File data      : %d\n" , i_adp2_in_fle_rec_knt ) ;
}

if ( i_pa_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Postal_Area in File data        : %d\n" , i_pa_in_fle_rec_knt   ) ;
}

if ( i_tel_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Telephone_Number in File data   : %d\n" , i_tel_in_fle_rec_knt  ) ;
}

if ( i_dte_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Date in File data               : %d\n" , i_dte_in_fle_rec_knt  ) ;
}

if ( i_id_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Id in File data                 : %d\n" , i_id_in_fle_rec_knt   ) ;
}

if ( i_att1_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Attribute1 in File data         : %d\n" , i_att1_in_fle_rec_knt ) ;
}

if ( i_att2_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Attribute2 in File data         : %d\n" , i_att2_in_fle_rec_knt ) ;
}

if ( i_ft1_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter1 in File data            : %d\n" , i_ft1_in_fle_rec_knt  ) ;
}

if ( i_ft2_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter2 in File data            : %d\n" , i_ft2_in_fle_rec_knt  ) ;
}

if ( i_ft3_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter3 in File data            : %d\n" , i_ft3_in_fle_rec_knt  ) ;
}

if ( i_ft4_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter4 in File data            : %d\n" , i_ft4_in_fle_rec_knt  ) ;
}

if ( i_ft5_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter5 in File data            : %d\n" , i_ft5_in_fle_rec_knt  ) ;
}

if ( i_ft6_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter6 in File data            : %d\n" , i_ft6_in_fle_rec_knt  ) ;
}

if ( i_ft7_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter7 in File data            : %d\n" , i_ft7_in_fle_rec_knt  ) ;
}

if ( i_ft9_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter8 in File data            : %d\n" , i_ft9_in_fle_rec_knt  ) ;
}

if ( i_ft9_in_fle_rec_knt != 0 ) {
  fprintf (f_log_fopen_status , "Records with Filter9 in File data            : %d\n" , i_ft9_in_fle_rec_knt  ) ;
}
/* END RECORDS WITH FIELD IN FILE DATA ************************************************************/

/* PURPOSE ERRORS COUNT ***************************************************************************/

if ( i_Add_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nAddress purpose errors count        : %d\n" , i_Add_prps_err_knt ) ;
}

if ( i_Con_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nContact purpose errors count        : %d\n" , i_Con_prps_err_knt ) ;
}

if ( i_CEn_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nCorp Entity errors count            : %d\n" , i_CEn_prps_err_knt ) ;
}

if ( i_Div_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nDivision purpose errors count       : %d\n" , i_Div_prps_err_knt ) ;
}

if ( i_Fam_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFamily purpose errors count         : %d\n" , i_Fam_prps_err_knt ) ;
}

if ( i_Fld_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFields purpose errors count         : %d\n" , i_Fld_prps_err_knt ) ;
}

if ( i_Ftr1_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter1 purpose errors count        : %d\n" , i_Ftr1_prps_err_knt ) ;
}

if ( i_Ftr2_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter2 purpose errors count        : %d\n" , i_Ftr2_prps_err_knt ) ;
}

if ( i_Ftr3_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter3 purpose errors count        : %d\n" , i_Ftr3_prps_err_knt ) ;
}

if ( i_Ftr4_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter4 purpose errors count        : %d\n" , i_Ftr4_prps_err_knt ) ;
}

if ( i_Ftr5_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter5 purpose errors count        : %d\n" , i_Ftr5_prps_err_knt ) ;
}

if ( i_Ftr6_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter6 purpose errors count        : %d\n" , i_Ftr6_prps_err_knt ) ;
}

if ( i_Ftr7_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter7 purpose errors count        : %d\n" , i_Ftr7_prps_err_knt ) ;
}

if ( i_Ftr8_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter8 purpose errors count        : %d\n" , i_Ftr8_prps_err_knt ) ;
}

if ( i_Ftr9_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nFilter9 purpose errors count        : %d\n" , i_Ftr9_prps_err_knt ) ;
}

if ( i_Hsho_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nHouseHold purpose errors count      : %d\n" , i_Hsho_prps_err_knt ) ;
}

if ( i_ind_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nIndividual purpose errors count     : %d\n" , i_ind_prps_err_knt ) ;
}

if ( i_OrgN_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nOrganization purpose errors count   : %d\n" , i_OrgN_prps_err_knt ) ;
}

if ( i_PerN_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nPerson_Name purpose errors count    : %d\n" , i_PerN_prps_err_knt ) ;
}

if ( i_Res_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nResident purpose errors count       : %d\n" , i_Res_prps_err_knt ) ;
}

if ( i_WCon_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nWide Contact purpose errors count   : %d\n" , i_WCon_prps_err_knt ) ;
}

if ( i_WHsho_prps_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nWide Household purpose errors count : %d\n" , i_WHsho_prps_err_knt ) ;
}

/* END PURPOSE ERRORS COUNT ***********************************************************************/

/* SEARCH RECORDS AND FILE RECORDS ERROR COUNT ****************************************************/

if ( i_src_rec_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , " - Search records errors count : %d\n" , i_src_rec_err_knt ) ;
}

if ( i_fle_rec_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , " - File records errors count   : %d\n" , i_fle_rec_err_knt ) ;
}
/* END SEARCH RECORDS AND FILE RECORDS ERROR COUNT ************************************************/

/* RECORDS THAT DOES NOT CONTAIN ID ERROR COUNT ***************************************************/

if ( i_id_err_rec_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nMissing Id error records            : %d\n" , i_id_err_rec_knt ) ;
}

if ( i_id_src_rec_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , " - Search records : %d\n" , i_id_src_rec_err_knt ) ;
}

if ( i_id_fle_rec_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , " - File records   : %d\n" , i_id_fle_rec_err_knt ) ;
}
/* END RECORDS THAT DOES NOT CONTAIN ID ERROR COUNT ***********************************************/

if ( i_tot_err_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nTotal errors count                  : %d\n" , i_tot_err_knt ) ;
}

if ( i_tot_err_rec_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nTotal error records count           : %d\n" , i_tot_err_rec_knt ) ;
  printf ("Total error records count           : %d\n" , i_tot_err_rec_knt ) ;
}

if ( i_op_rec_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\nOutput records count : %d\n" , i_op_rec_knt ) ;
}

/* DECISION COUNT *********************************************************************************/

if ( i_dec_A_knt != 0 ||
     i_dec_R_knt != 0 ||
     i_dec_U_knt != 0 ) {
  fprintf ( f_log_fopen_status , "\n------ Decision counts ------\n" ) ;
}
if ( i_dec_A_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Accept      (A) : %d \n" , i_dec_A_knt ) ;
}

if ( i_dec_R_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Reject      (R) : %d\n"  , i_dec_R_knt );
}

if ( i_dec_U_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Undecided   (U) : %d\n" , i_dec_U_knt ) ;
}
/* END DECISION COUNT *****************************************************************************/

/* SCORE RANGE COUNT ******************************************************************************/

if ( i_scr_pft_mtc_knt != 0 ||
     i_scr_90_99_knt   != 0 ||
     i_scr_80_89_knt   != 0 ||
     i_scr_70_79_knt   != 0 ||
     i_scr_60_69_knt   != 0 ||
     i_scr_50_59_knt   != 0 ||
     i_scr_40_49_knt   != 0 ||
     i_scr_30_39_knt   != 0 ||
     i_scr_20_29_knt   != 0 ||
     i_scr_10_19_knt   != 0 ||
     i_scr_1_9_knt     != 0 ||
     i_scr_nt_mtc_knt
   ) {
  fprintf ( f_log_fopen_status , "\n------ Score range counts ------\n" ) ;
}

if ( i_scr_pft_mtc_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Perfect Match : %d\n" , i_scr_pft_mtc_knt ) ;
}

if ( i_scr_90_99_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Score 90 - 99 : %d\n" , i_scr_90_99_knt ) ;
}

if ( i_scr_80_89_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Score 80 - 89 : %d\n" , i_scr_80_89_knt ) ;
}

if ( i_scr_70_79_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Score 70 - 79 : %d\n" , i_scr_70_79_knt ) ;
}

if ( i_scr_60_69_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Score 60 - 69 : %d\n" , i_scr_60_69_knt ) ;
}

if ( i_scr_50_59_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Score 50 - 59 : %d\n" , i_scr_50_59_knt ) ;
}

if ( i_scr_40_49_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Score 40 - 49 : %d\n" , i_scr_40_49_knt ) ;
}

if ( i_scr_30_39_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Score 30 - 39 : %d\n" , i_scr_30_39_knt ) ;
}

if ( i_scr_20_29_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Score 20 - 29 : %d\n" , i_scr_20_29_knt ) ;
}

if ( i_scr_10_19_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Score 10 - 19 : %d\n" , i_scr_10_19_knt ) ;
}

if ( i_scr_1_9_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Score  1 -  9 : %d\n" , i_scr_1_9_knt ) ;
}

if ( i_scr_nt_mtc_knt != 0 ) {
  fprintf ( f_log_fopen_status , "Not Matching  : %d\n" , i_scr_nt_mtc_knt ) ;
}
/* END SCORE RANGE COUNT **************************************************************************/

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
 End of Main                                                          *
**********************************************************************/

/*
  Get Match

1 Procedure Name

2 Copyright

3 Warnings

4 Format of input file

5 Format of output file

6 Format of log file

7 Technical

  7.1 Variables used

  7.2 Run Parameters

  7.3 Compile Procedure

  7.4 Execute procedure in different way

  7.5 Execution Start and End date and time

  7.6 Subroutines

      7.6.1 Called by

      7.6.2 Calling

      7.6.3 Subroutine Structure

8 Include Header

Get Match

  Used to get a score and match decision for two records, a search
  record and a file record for match level Typical (Y) , Conservative (C) ,
  Loose (L) for Address , Contact , Corp_Entity , Division , Family , Fields ,
  Filter1 , Filter2 , Filter3 , Filter4 , Filter5 , Filter6 , Filter7 , Filter8 ,
  Filter9 , Household , Individual , Person_Name , Organization , Resident ,
  Wide_Contact , Wide_Household as found in purpose. Any logical combinations
  of these purposes may be used.

  MATCH_LEVEL
  ---------
  Y|Typical
  C|Conservative
  L|Loose

  PURPOSE               MANDATORY FIELDS
  ----------------      ---------------------------------------
  S|Address             Address_Part1
  N|Contact             Person_Name and Organization_Name and Address_Part1
  E|Corp_Entity         Organization_Name
  D|Division            Organization_Name and Address_Part1
  M|Family              Person_Name and Address_Part1 and Telephone_Number
  L|Fields              No mandatory fields
  1|Filter1             Filter1
  2|Filter2             Filter2
  3|Filter3             Filter3
  4|Filter4             Filter4
  5|Filter5             Filter5
  6|Filter6             Filter6
  7|Filter7             Filter7
  8|Filter8             Filter8
  9|Filter9             Filter9
  H|Household           Person_Name and Address_Part1
  I|Individual          Person_Name and ( Date or Id )
  O|Organization        Organization_Name
  P|Person_Name         Person_Name
  R|Resident            Person_Name and Address_Part1
  W|Wide_Contact        Person_Name and Organization_Name
  Y|Wide_Household      Person_Name and Address_Part1 and Telephone_Number

Procedure Name : 44_506_GMtc.c

  Generate .ost file ( Output file ) using .mst file ( Input file ) that contain
   search data , tab delimiter , file data.
   Note that search data and file data must be tagged data.

Copyright

  Copyright ( c ) 2017 IdentLogic Systems Private Limited

Warnings

  If your data can contain asterisks, make sure that these are
   either cleaned out prior to calling the dds-name3 functions ,
   or use a different DELIMITER=setting.

  Length of the Input file directory , Output file directory and
   Log file directory should not exceed 1 ,000 bytes , with total
   filepath not exceeding 1011  , 1015  ,1038 resp.  This is due
   to the length of the Input file name , Output file name and
   Log file name are 11 , 15 and 38 resp.

  Length of the Search data and file data
   should not exceed 5000 and 5000 bytes resp.
   and the Length of the Controls related variables should not exceed
   which are i specified.

  Increase the array size as per your convenience.

  Data set number , Run time number and Purpose parameters are mandatory.

  Data set number should be integer and in a range of 100 to 999
  Run time number should be integer and in a range of 1000 to 9999
  Means Input file name should be integer and in range.
  System name should not be empty
  Population name should not be empty
  Encoding datatype should not be empty and it must be
   any one of these TEXT , UTF-8 , UTF-16 , UTF-16LE , UTF-16BE , UTF-32 ,
   UCS-2 , UCS-4
  Purpose should not be empty and it must be any one of these
   Address , Contact , Corp_Entity , Division  , Family , Fields ,
   Filter1 , Filter2 , Filter3 , Filter4 , Filter5 , Filter6 , Filter7 , Filter8 ,
   Filter9 , Household , Individual , Person_Name , Organization , Resident , Wide_Contact ,
   Wide_Household

  If data set number and run number are empty it will throw an error
  If data set number and run number are missing or not found it will not create output
  or log file.
  System name is the location( default folder) where india.ysp file is located.
  Population means country name

Format of input file  : sssrrrr.mst ( mst stand for m(atched-to be) s(orted) t(ab) )

  #  FIELD
  -  ------
  1  Search tag data
  2  File tag data
  3  Search id
  4  File id

Format of output file : sssrrrr_nnn.ost ( ost stand for o(utput-of match) s(orted) t(ab) )- TAB delimited

  #  FIELD
  -  ------------
  1 Search data Id  - Up to 1000 Unicode characters

  2 File data Id    - Up to 1000 Unicode characters

  3 Decision        - 1 character      - A(cccept), U(ndecided) or R(eject)

  4 Score           - 1 to 3 digits    - 000 to 100


  3 Purpose number  - 3 digits         - 100 to 999
                         Should be in table dedupe . T_CTL PPS

  4 Match_level     - 1 character      -  C(onservative ) ,  T(ypical )
                                          L(oose )

  SORT: Search data Id , File data Id

  Description :

  Score          : A 3 byte number between 0-100 indicating a confidence level of the match.
                    The score value depends on the data, Match Purpose and Match Level

  Match Decision : A 1-byte character value identifying the judgment on the matched records.
                    The judgment can be either:
                   'A' meaning 'Accept'. The Score is above the Accept Limit for the specified
                    Purpose and Match Level
                   'R' meaning 'Reject'. The Score is below the Reject Limit for the specified
                     Purpose and Match Level
                   'U' meaning 'Undecided'. The Score is in between the Accept Limit and the
                    Reject Limit for the specified Purpose and Match Level.

Format of log file : sssrrrr_GMtc_YYYY_MM_DD_HH24_MI_SS.log

  Log file will be created with data set number, run number ,procedure name
   and date time
  for eg. sssrrrr_GMtc_YYYY_MM_DD_HH24_MI_SS.log

  Log file display only not null or non empty information.
  Those fields values are empty or not null it will not display in log file

  -d -r -u and -b options are mandatory

  If System name , Population name and Encoding datatype and delimiter parameters are empty
  then it will take default value and write it on log file with
  message : Missing- Default:<default_value>

  If verbose flag is on then only multiplier value will be write it on log file
  If verbose flag is on and muliplier parameter is empty then it will take
  default value of multiplier which is 1 lakh.

  Log file name contain below information.

  ------ GMtc EXECUTION START DATE AND TIME ------
  YYYY-MM-DD HH24:MI:SS

  ------ Run Parameters ------
  Data set number        : data set number starting from 100 to 999
  Run time number        : Run time number starting from 1000 to 9999
  Purpose                : Purpose
  Purpose number         : Purpose number
  System name            : default
  Population             : india
  Input File Directory   : Input File path
  Output File Directory  : Output File path
  Log File Directory     : Log File path
  DELIMITER              : single character variable
  Encoding datatype      : Unicode encoding of search data and file data
  Unicode encoding       : <DIGIT> Either 4 , 6 or 8
  Name format            : Either L or R
  Accept Limit (+/-nn)   : <DIGIT>
  Reject Limit (+/-nn)   : <DIGIT>
  Adjweight              : <FIELD-NAME>
  Adjweight value (+/-nn): <DIGIT>
  Multiplier             : <MULTIPLIER_VALUE>
  Verbose                : Yes/No

  ------ File Names ------
  Input file name       : <Input file name>
  Output file name      : <Output file name>
  Log file name         : <Log file name>

  ------ Environment variable ------
  SSATOP : <PATH>
  SSAPR  : <PATH>

  Error message: Missing Id field

  If Id field is missing in the record then error will be display with
  record no with error message and record.

  Error message:Missing Person Name, Organization name, Address Part 1 fields

  If Person Name, Organization name and Address Part 1 fields is
  missing in the record then error will be display with
  record no with error message and record.

  Error message: Improper record
  If current record does not contain Tag id and Tag data with tab delimited.
  and Either tag id or tag data missing then this error will write it on log file

  ------ Run Summary ------
  Total records read  : <Count>

  Records with Person_Name in Search data       : <COUNT>
  Records with Organization_Name in Search data : <COUNT>
  Records with Address_Part1 in Search data     : <COUNT>
  Records with Address_Part2 in Search data     : <COUNT>
  Records with Postal_Area in Search data       : <COUNT>
  Records with Telephone_Number in Search data  : <COUNT>
  Records with Date in Search data              : <COUNT>
  Records with Id in Search data                : <COUNT>
  Records with Attribute1 in Search data        : <COUNT>
  Records with Attribute2 in Search data        : <COUNT>
  Records with Filter1 in Search data           : <COUNT>
  Records with Filter2 in Search data           : <COUNT>
  Records with Filter3 in Search data           : <COUNT>
  Records with Filter4 in Search data           : <COUNT>
  Records with Filter5 in Search data           : <COUNT>
  Records with Filter6 in Search data           : <COUNT>
  Records with Filter7 in Search data           : <COUNT>
  Records with Filter8 in Search data           : <COUNT>
  Records with Filter9 in Search data           : <COUNT>

  Records with Person_Name in File data       : <COUNT>
  Records with Organization_Name in File data : <COUNT>
  Records with Address_Part1 in File data     : <COUNT>
  Records with Address_Part2 in File data     : <COUNT>
  Records with Postal_Area in File data       : <COUNT>
  Records with Telephone_Number in File data  : <COUNT>
  Records with Date in File data              : <COUNT>
  Records with Id in File data                : <COUNT>
  Records with Attribute1 in File data        : <COUNT>
  Records with Attribute2 in File data        : <COUNT>
  Records with Filter1 in File data           : <COUNT>
  Records with Filter2 in File data           : <COUNT>
  Records with Filter3 in File data           : <COUNT>
  Records with Filter4 in File data           : <COUNT>
  Records with Filter5 in File data           : <COUNT>
  Records with Filter6 in File data           : <COUNT>
  Records with Filter7 in File data           : <COUNT>
  Records with Filter8 in File data           : <COUNT>
  Records with Filter9 in File data           : <COUNT>

  Address purpose errors count        : <COUNT>
   - Search records errors count : <COUNT>
   - File records errors count   : <COUNT>

  Missing Id error records            : <COUNT>
   - Search records : <COUNT>
   - File records   : <COUNT>

  Total errors count                  : <COUNT>

  Total error records count           : <COUNT>

  Output records count : <COUNT>

  ------ Decision counts ------
  Accept      (A) : <COUNT>
  Reject      (R) : <COUNT>
  Undecided   (U) : <COUNT>

 ------ Score range counts ------
  Perfect Match : <COUNT>
  Score 90 - 99 : <COUNT>
  Score 80 - 89 : <COUNT>
  Score 70 - 79 : <COUNT>
  Score 60 - 69 : <COUNT>
  Score 50 - 59 : <COUNT>
  Score 40 - 49 : <COUNT>
  Score 30 - 39 : <COUNT>
  Score 20 - 29 : <COUNT>
  Score 10 - 19 : <COUNT>
  Score  1 -  9 : <COUNT>
  Not Matching  : <COUNT>

  Ended YYYY-MM-DD HH24:MI:SS - HH:MM:SS to execute

  =================================================

  Note: Only that data will be display in log file which is not null

  Terminal output:

  Total error records count

  If your input file contain less than 1 lakh records and your doing verbose
  it will not display on command prompt. If you really want to see how how it process
  then you need to change the multiplier default value.

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
  Procedure Number - 506
  Script name      - 44_506_GMtc.c

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

  PARAMETER                DESCRIPTION                                   ABV   VARIABLE
  ---------                -------------------------------------------   ---  ---------
  Set data number          Set data number - 100 to 999                   d   p_data_set

  Run number               Run number - 1000 to 9999                      r   p_run_time

  System name              Defines location of the population rules       s   p_system_nm

  Population               Country Name: india                            p   p_population

  Encoding datatype        Encoding type of search data and file data
                            it must be any one of these TEXT , UTF-8 ,
                            UTF-16 , UTF-16LE , UTF-16BE , UTF-32 ,
                            UCS-2 , UCS-4                                 c   p_encoding

  Purpose                  The PURPOSE Control specifies the name of
                            the Matching Purpose to use in the Match
                            call.                                         u   p_pupose

  Purpose no               Purpose number is the number taken from Table
                            dedupe . T_CTL_PPS                            b   p_prpsno

  Unicode encoding         UNICODE_ENCODING instructs dds-NAME3 to
                            accept Unicode data input, and in what
                            Unicode format the data will be passed.
                           Unicode encoding must be 4 , 6 or 8.           e   p_uni_enc

  Name format              Defines major word in the name or address
                            can be commonly found (left or right end)
                           Name format must be L or R                     n   p_nm_fmt

  Accept limit             The MATCH_LEVEL Control can also be used to
                            adjust the pre-defined Accept Score Limits
                            that affect the Match Decisions.
                           If accept limit are null and reject limit
                            contain value it assign to 0 to accept
                            limit.                                        a   p_acc_lmt

  Reject limit             The MATCH_LEVEL Control can also be used to
                            adjust the pre-defined Accept Score Limits
                            that affect the Match Decisions.              j   p_rej_lmt

  Adjweight                Used to adjust the weight of a single field
                            in a Match Purpose up or down relative to
                            the other fields in the Purpose.
                           Always write Adjweight with its value          w   p_adjweight

  Adjweight value          Increase and Decrease importance of fields     x   p_adjwei_val

  Delimiter                If the Search and File data is being passed
                            using the Tagged Data Format, this control
                            may be used to override the default
                            delimiter � asterisk (*).                     t   p_delimeter

  Input File Directory     Input File Directory                           i   p_infdir

  Output File Directory    Output File Directory                          o   p_outfdir

  Log File Directory       Log File Directory                             l   p_logfdir

  Multiplier               Multiplier                                     m   p_multiplier

  Verbose                  Display indformation about multiplier          v   i_verbose_flg

 TimeStamp Variables   Description
 -------------------   -----------
   YYYY                  Year
   MM                    Month
   DD                    Date
   HH24                  Hours
   MI                    Minutes
   SS                    Seconds

 The extension of Input file name is .mst
 The extension of Output file name is .ost ( o(utput- of match) s(orted) t(tab) )
 The extension of Log file name is .log

 If Month, Date, Hours, Minutes, Seconds are less than 9 prefix 0 will be added to it.

 Compile procedure

  cl 44_506_GMtc.c stssan3cl.lib

 Execute procedure

   -d -r -u paramters are mandatory

   1. 44_506_GMtc -d 100 -r 1000 -u Resident

   2. 44_506_GMtc -d 100 -r 1000 -u Address -i E:/ABC/EFG/HIJ/Input/
     -o E:/ABC/EFG/HIJ/Output/ -l E:/ABC/EFG/HIJ/Log/

   3. 44_506_GMtc -d 100 -r 1000 -s default -p india -c TEXT -u Address -e 4 -n L -a 10 -j 5
      -w Address_Part1 -x 1 -t @

   4. 44_506_GMtc -d 100 -r 1000 -s default -p india -c TEXT -u Address -e 4 -n L -a 10 -j 5
      -w Address_Part1 -x 1 -t @ -i E:/ABC/EFG/HIJ/Input
     -o E:/ABC/EFG/HIJ/Output -l E:/ABC/EFG/HIJ/Log

   5. 44_506_GMtc -d 100 -r 1000 -s default -p india -c TEXT -u Address -e 4 -n L -a 10 -j 5
      -w Address_Part1 -x 1 -i E:\ABC\EFG\HIJ\Input
     -o E:\ABC\EFG\HIJ\Output -l E:\ABC\EFG\HIJ\Log -m 100000 -v

   6. 44_506_GMtc -d 100 -r 1000 -s default -p india -c TEXT -u Address -i E:/ABC/EFG/HIJ/Input/

   7. 44_506_GMtc -d 100 -r 1000 -s default -p india -c TEXT -u Address -m 100000 -v

 Note :

   If you forget to give backslash(\) or forward slash(/) at the end of the
    Input file directory path , Output file directory path , and Log file directory path .
   The procedure will be append backslash or forward slash at the end of path as per your
    directory path.
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
 s_date_time           Compute current date and time elements YYYY, MM, DD, HH24, MI and SS
 s_print_usage         This subroutine is default parameter of getopt in s_getParameter
 s_getParameter        This subroutine takes run parameters and perform operations.
 s_test_dds_open       To open a session to the dds-NAME3 services
 s_test_dds_match      Used to get a score and match decision for two records, a search
                        record and a file record
 s_test_dds_close      To close an open session to dds-NAME3
 s_doExit              Error occurred in subroutines s_test_dds_open, s_test_dds_match, s_test_dds_close
 s_GMtc_open           s_test_dds_open subroutine called in s_GMtc_open subroutine
 s_GMtc_matches        s_test_dds_match subroutine called in s_GMtc_matches subroutine
 s_GMtc_close          s_test_dds_close subroutine called in s_GMtc_close subroutine

 Called by

 Not indicated if only called by Main.

 Subroutine            Called by
 ----------            ---------
 s_test_dds_open        s_GMtc_open
 s_test_dds_match       s_GMtc_matches
 s_test_dds_close       s_GMtc_close
 s_doExit               s_GMtc_open , s_GMtc_matches, s_GMtc_close
 s_print_usage          s_getParameter
 s_date_time            s_getParameter , s_GMtc_close

 Calling

 Subroutine           Calling Subroutine
 ----------           ------------------
 s_GMtc_open         s_test_dds_open , s_doExit
 s_GMtc_matches      s_test_dds_match , s_doExit
 s_GMtc_close        s_test_dds_close , s_doExit , s_date_time
 s_getParameter      s_print_usage , s_date_time

 Subroutine Structure

 Main
  |
  |----- s_getParameter
  |           |
  |           |----- s_date_time
  |           |
  |           \----- s_print_usage
  |
  |----- s_GMtc_open
  |           |
  |           |----- s_test_dds_open
  |           |
  |           \----- s_doExit
  |
  |----- s_GMtc_matches
  |           |
  |           |----- s_test_dds_match
  |           |
  |           \----- s_doExit
  |
  \----- s_GMtc_close
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
    Variable value : E:\ids\nm3;

    Variable Name  : SSAPR
    Variable value : E:\ids\nm3\pr\

    If any above variable is already there in the System variable,
    do not create again, only paste the path.
    Check ids directory where it is located.

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

/**********************************************************************
 End of script 44_506_GMtc.c                                                 *
**********************************************************************/
