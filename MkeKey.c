#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ssan3cl.h"
#include <getopt.h>
#include <unistd.h>
#include <time.h>

#define rangeof(arr)  (sizeof(arr)/sizeof((arr)[0]))

  // Global variables
  long  rc;
  long  sockh = -1;
  long  session_id = -1;
  FILE  *input_file_name;                                    // Input file name
  FILE  *output_file_name;                                   // Output file name
  FILE  *log_file_name;                                      // Log file name
  char  population ;                                          // Country name
  char  *input_file_directory ;                                // Input file directory
  char  *output_file_directory ;                               // Output file directory
  char  *log_file_direcory ;                                   // Log file directory
  char  verbose_flag ;                                        // Verbose flag
  
  int data_set_no ;                                         // Data set number
  int run_no ;                                              // Run number
  int cur_rec_len ;                                         // Current record length
  int rec_number = 0 ;                                        //record counter
  
  char current_rec [ BUFSIZ ] ;                                 // Current record of a file
  char *arg_population;                                            // Population name
  
  
  char *pn;
char *on;
char *adp1;
char *M;  

  
  

static long
test_ssa_open (
  long    sockh,
  long    *session_id,
  char    *sysName,
  char    *population,
  char    *controls)
{
  long    rc;
  char    rsp_code[SSA_SI_RSP_SZ];
  char    ssa_msg[SSA_SI_SSA_MSG_SZ];

  printf ("------ ssan3_open ------\n");  //-- log ...
  printf ("Session Id       : %ld\n", *session_id);
  printf ("System           : %s\n", sysName);
  printf ("Population       : %s\n", population);
  printf ("Controls         : %s\n", controls); // -- n/a

  rc = ssan3_open (sockh,
    session_id,
    sysName,
    population,
    controls,
    rsp_code,
    SSA_SI_RSP_SZ,
    ssa_msg,
    SSA_SI_SSA_MSG_SZ);
  if (rc < 0) {
    printf ("rc               : %ld\n", rc);
    rc = 1;
    goto ret;
  }
  if (rsp_code[0] != '0' && *session_id == -1) {
    printf ("rsp_code         : %s\n", rsp_code);
    printf ("ssa_msg          : %s\n", ssa_msg);
    rc = 1;
    goto ret;
  }
  printf ("\n");
  printf ("Session Id       : %ld\n", *session_id);
  printf ("rsp_code         : %s\n", rsp_code);
  printf ("ssa_msg          : %s\n", ssa_msg);  //-- ???
  rc = 0;
  goto ret;

ret:
  return (rc);
}

static long
test_ssa_get_keys (
  long    sockh,
  long    *session_id,
  char    *sysName,
  char    *population,
  char    *controls,
  char    *record,
  long    recordLength,
  char    *recordEncType)
{
  int   i;
  long    rc;
  char    *keys_array[SSA_SI_MAX_KEYS];
  char    keys_data[SSA_SI_MAX_KEYS*SSA_SI_KEY_SZ];
  char    rsp_code[SSA_SI_RSP_SZ];
  char    ssa_msg[SSA_SI_SSA_MSG_SZ];
  long    num;
  char    *p;

  for (i = 0; i < (int)rangeof (keys_array); ++i)
    keys_array[i] = keys_data + i * SSA_SI_KEY_SZ;

  printf ("------ ssan3_get_keys_encoded ------\n");  //-- suppress ... ???
  printf ("Session Id       : %ld\n", *session_id);
  printf ("System           : %s\n", sysName);
  printf ("Population       : %s\n", population);
  printf ("Controls         : %s\n", controls);
  printf ("Key field data   : %s\n", record);
        printf ("Key field size   : %ld\n", recordLength);
        printf ("Key field encoding type : %s\n", recordEncType);

  num = 0;
  rc = ssan3_get_keys_encoded (sockh,
    session_id,
    sysName,
    population,
    controls,
    rsp_code,
    SSA_SI_RSP_SZ,
    ssa_msg,
    SSA_SI_SSA_MSG_SZ,
    record, recordLength, recordEncType,
    &num,
    keys_array,
    SSA_SI_KEY_SZ);
  if (rc < 0) {
    printf ("rc               : %ld\n", rc);
    rc = 1;
    goto ret;
  }
  if (rsp_code[0] != '0') {
    printf ("rsp_code         : %s\n", rsp_code);
    printf ("ssa_msg          : %s\n", ssa_msg);
    rc = -1;
    goto ret;
  }
  printf ("\n");
  printf ("Session Id       : %ld\n", *session_id);
  printf ("rsp_code         : %s\n", rsp_code);
  printf ("ssa_msg          : %s\n", ssa_msg);
  printf ("Count            : %ld\n", num);
  printf ("------ keys ------\n");
  ++rec_number;
  fprintf(output_file_name,"\n%d :",rec_number);
  for (i = 0; i < num; ++i) {
    p = keys_array[i];
    fprintf ( output_file_name,"'%.*s'", SSA_SI_KEY_SZ, p);
    printf ("%3d '%.*s'\n", i+1, SSA_SI_KEY_SZ, p);  //-- OUTPUT
  }
  printf ("\n");
  rc = 0;
  goto ret;

ret:
  return (rc);
}

static long
test_ssa_close (
  long    sockh,
  long    *session_id,
  char    *sysName,
  char    *population,
  char    *controls)
{
  long    rc;
  char    rsp_code[SSA_SI_RSP_SZ];
  char    ssa_msg[SSA_SI_SSA_MSG_SZ];

  printf ("------ ssan3_close ------\n");
  printf ("Session Id       : %ld\n", *session_id);
  printf ("System           : %s\n", sysName);
  printf ("Population       : %s\n", population);
  printf ("Controls         : %s\n", controls);

  rc = ssan3_close (sockh,
    session_id,
    sysName,
    population,
    controls,
    rsp_code,
    SSA_SI_RSP_SZ,
    ssa_msg,
    SSA_SI_SSA_MSG_SZ);
  if (rc < 0) {
    printf ("rc               : %ld\n", rc);
    rc = 1;
    goto ret;
  }
  if (rsp_code[0] != '0') {
    printf ("rsp_code         : %s\n", rsp_code);
    printf ("ssa_msg          : %s\n", ssa_msg);
    rc = 1;
    goto ret;
  }
  printf ("\n");
  printf ("Session Id       : %ld\n", *session_id);
  printf ("rsp_code         : %s\n", rsp_code);
  printf ("ssa_msg          : %s\n", ssa_msg);
  rc = 0;
  goto ret;

ret:
  return (rc);
}

static void doExit ( char *func) {
  printf ("Error occurred in '%s'\n", func);
  exit (EXIT_FAILURE);
}

static void MkeKey_open ( ) {
  // Establish a session.
  rc = test_ssa_open (
    sockh,
    &session_id,
    "default",
    arg_population,
    "");
  if (0 != rc)
    doExit ("test_ssa_open");
}

static void MkeKey_getKey ( ) {
    rc = test_ssa_get_keys (
    sockh,
    &session_id ,
    "default" ,
    arg_population ,
    "FIELD=Person_Name KEY_LEVEL=Extended", //"FIELD=Person_Name KEY_LEVEL=Extended"
    current_rec,
    cur_rec_len,
    "TEXT");

  if (0 != rc)
    doExit ("test_ssa_get_keys");
}

static void MkeKey_close ( ) {
  rc = test_ssa_close (
    sockh,
    &session_id,
    "default",
    arg_population,
    "");

  if (0 != rc)
    doExit ("test_ssa_close");
}

void print_usage() {
    printf("MkeKey -p india -i Tagged_Data.txt -o output.txt -l log.txt");
}

int main (int argc, char *argv[])
{
  int   option = 0;
  //long    rc;
  //long    sockh = -1;
  //long    session_id = -1;

  
  char *arg_infname;                                               // Input file name
  char *arg_outfname;                                              // Output File name
  char *arg_logfname;                                              // Info File name 
  
  
 // Clock t ;                                          // Start time
 // t = clock();                                                      // Start time
 // double time_taken;

  while ((option = getopt(argc, argv,"p:i:o:l:")) != -1) {
        switch (option) {
             case 'p' :
                        arg_population = optarg ;
                        break;
             case 'i' :
                        arg_infname    = optarg ;
                        break;
             case 'o' :
                        arg_outfname   = optarg ;
                        break;
             case 'l' :
                        arg_logfname   = optarg ;
                        break;
             default:
                        print_usage();
                        exit(EXIT_FAILURE);
        }
    }

  input_file_name   = fopen ( arg_infname , "r" ) ;
  output_file_name  = fopen ( arg_outfname , "w" ) ;         // Open file and write
  log_file_name     = fopen ( arg_logfname , "w" );

  if ( ! input_file_name ) {
    printf ( "Could not open file %s for input.\n" , input_file_name ) ;      // Error message while opening file
    exit(1) ;
  }

  if ( ! output_file_name ) {
    printf ( "Could not open file %s for output\n" ,output_file_name ) ;      // Error message while opening file
    exit(1) ;
  }

  if ( ! log_file_name ) {
    printf ( "Could not open file %s for error\n" ,log_file_name  ) ;    // Error message while opening file
    exit(1) ;
  }


  MkeKey_open() ;                                                // Open MkeKey
 

  // Call ssan3_get_keys
  

  // Read a input file line by line
  while( fgets (  current_rec , sizeof (current_rec), input_file_name  ) ) {
      cur_rec_len=strlen( current_rec ) ;
      if ( cur_rec_len > 0 && current_rec[cur_rec_len-1] == '\n' ) {
        current_rec[--cur_rec_len] = '\0';
      }
      
      
      if ( strstr ( current_rec , "Person_Name" ) != NULL ) {
        MkeKey_getKey() ;
      }
      
      if ( strstr ( current_rec , "Organization_Name" ) != NULL ) {
        MkeKey_getKey() ;
      }
      if ( strstr ( current_rec, "Address_Part1" ) != NULL ) {
        MkeKey_getKey() ;
      }
      
  }


  MkeKey_close() ;

/* Close the previously established session.
*/
  

  //t = clock() - t;                                              // End time
  //time_taken = ((double)t)/CLOCKS_PER_SEC;                      // In seconds
  //printf("\nTook %f seconds to execute \n", time_taken);        // Print time

  return (0);
}
