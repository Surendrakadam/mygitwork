#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ssan3cl.h"
#include <getopt.h>
#include <unistd.h>
#include <time.h>

#define rangeof(arr)  (sizeof(arr)/sizeof((arr)[0]))


  FILE    *infile, *outfile ,*errorfile , *infofile;                  // Object of output file
  int     rec_number = 0 ;

static long test_ssa_open (
  long    sockh,
  long    *session_id,
  char    *sysName,
  char    *population,
  char    *controls)
{
  long    rc;
  char    rsp_code[SSA_SI_RSP_SZ];
  char    ssa_msg[SSA_SI_SSA_MSG_SZ];

  fprintf ( infofile ,"------ ssan3_open ------\n" );
  fprintf ( infofile ,"Session Id : %ld\n",*session_id);      // Info
  fprintf ( infofile ,"System     : %s\n", sysName );         // Info
  fprintf ( infofile ,"Population : %s\n", population );      // Info
  fprintf ( infofile ,"Controls   : %s\n", controls );        // Info

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
    fprintf (errorfile,"rc               : %ld\n", rc);
    rc = 1;
    goto ret;
  }
  if (rsp_code[0] != '0' && *session_id == -1) {
    fprintf (errorfile,"rsp_code         : %s\n", rsp_code);    // Error
    fprintf (errorfile,"ssa_msg          : %s\n", ssa_msg);
    rc = 1;
    goto ret;
  }
  //printf ( "\n" );
  fprintf ( infofile,"Session Id       : %ld\n", *session_id );  // Info
  fprintf ( errorfile, "rsp_code         : %s\n", rsp_code );
  fprintf ( errorfile, "ssa_msg          : %s\n", ssa_msg );
  rc = 0;
  goto ret;

ret:
  return (rc);
}

static long
test_ssa_get_ranges (
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
  char    *ranges_array[SSA_SI_MAX_STAB];
  char    ranges_data[SSA_SI_MAX_STAB*(2*SSA_SI_KEY_SZ)];
  char    rsp_code[SSA_SI_RSP_SZ];
  char    ssa_msg[SSA_SI_SSA_MSG_SZ];
  long    num;
  char    *p;

  for (i = 0; i < (int)rangeof (ranges_array); ++i)
    ranges_array[i] = ranges_data + i * (2*SSA_SI_KEY_SZ);

  fprintf (infofile,"------ ssan3_get_ranges_encoded ------\n");
  fprintf (infofile,"Session Id       : %ld\n", *session_id);
  fprintf (infofile,"System           : %s\n", sysName);
  fprintf (infofile,"Population       : %s\n", population);
  fprintf (infofile,"Controls         : %s\n", controls);
  fprintf (infofile,"Key field data   : %s\n", record);
  fprintf (infofile,"Key field size   : %ld\n", recordLength);
  fprintf (infofile,"Key field encoding type : %s\n", recordEncType);

  num = 0;
  rc = ssan3_get_ranges_encoded (sockh,
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
    ranges_array,
    2*SSA_SI_KEY_SZ);
  if (rc < 0) {
    fprintf (errorfile,"rc               : %ld\n", rc);
    rc = 1;
    goto ret;
  }
  if (rsp_code[0] != '0') {
    fprintf (errorfile,"rsp_code         : %s\n", rsp_code);
    fprintf (errorfile,"ssa_msg          : %s\n", ssa_msg);
    rc = 1;
    goto ret;
  }
  //fprintf ("\n");
  fprintf (errorfile,"Session Id       : %ld\n", *session_id);
  fprintf (errorfile,"rsp_code         : %s\n", rsp_code);
  fprintf (errorfile,"ssa_msg          : %s\n", ssa_msg);
  fprintf (errorfile,"Count            : %ld\n", num);
  //printf ("------ Start Key ----- End Key ------\n");
  ++rec_number;
  fprintf(outfile,"\n%d : %s",rec_number,record);
  for (i = 0; i < num; ++i) {
    p = ranges_array[i];
    fprintf( outfile ,"  :Range: (%d) '%.*s'   '%.*s'\n",i+1,SSA_SI_KEY_SZ, p, SSA_SI_KEY_SZ, p+SSA_SI_KEY_SZ);
    //printf ("%3d '%.*s'      '%.*s'\n", i+1,SSA_SI_KEY_SZ, p, SSA_SI_KEY_SZ, p+SSA_SI_KEY_SZ);
  }
  //printf ("\n");
  fprintf (outfile,"\n");
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

  fprintf ( infofile ,"------ ssan3_close ------\n" );
  fprintf ( infofile ,"Session Id       : %ld\n", *session_id );
  fprintf ( infofile ,"System           : %s\n", sysName );
  fprintf ( infofile ,"Population       : %s\n", population );
  fprintf ( infofile ,"Controls         : %s\n", controls );

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
    fprintf (errorfile,"rc               : %ld\n", rc);
    rc = 1;
    goto ret;
  }
  if ( rsp_code[0] != '0' ) {
    fprintf ( errorfile,"rsp_code         : %s\n", rsp_code );
    fprintf ( errorfile,"ssa_msg          : %s\n", ssa_msg );
    rc = 1;
    goto ret;
  }
  fprintf ( errorfile,"\n" );
  fprintf ( errorfile,"Session Id       : %ld\n", *session_id );
  fprintf ( errorfile,"rsp_code         : %s\n", rsp_code );
  fprintf ( errorfile,"ssa_msg          : %s\n", ssa_msg );
  rc = 0;
  goto ret;

ret:
  return (rc);
}

static void
doExit (
  char    *func)
{
  printf ("Error occurred in '%s'\n", func);
  exit (EXIT_FAILURE);
}

void print_usage() {
    printf("RangeNextC -i Tagged_Data.txt -o Rangeoutput.txt -l Rangelog.txt -e Rangeerror.txt");
}

int main ( int argc, char *argv[] )
{
  long    rc;
  long    sockh;
  long    session_id;

  char    *population  = "India" ;                           // Population
  char    *field       = "FIELD=" ;
  char    *key         = "Person_Name SEARCH_LEVEL=Typical" ;                     // key
  char    *key_level   = "Standard" ;                        // Key level

  //char    *infname     = "Tagged_Data.txt" ;
  //char    *outfname    = "Tagged_Data_output.txt" ;
  //char    *errorfname  = "Error_File.txt" ;
  //char    *infofname   = "Info_File.txt" ;
  char line_buffer [ BUFSIZ ] ;
  int   option = 0;
  char *infname ;                                           // Input file name
  char *outfname ;                                          // Output File name
  char *logfname ;                                          // Info File name
  char *errorfname ;                                        // Error File name
  
  clock_t t;                                            // Clock object
  double time_taken;
  
  t = clock();                                          // Start time

  while ((option = getopt(argc, argv,"i:o:l:e:")) != -1) {
        switch (option) {
             case 'i' :
                        infname = optarg ;
                        break;
             case 'o' :
                        outfname = optarg ;
                        break;
             case 'l' :
                        logfname = optarg ;
                        break;
             case 'e' :
                        errorfname = optarg ;
                        break;
             default:
                        print_usage();
                        exit(EXIT_FAILURE);
        }
    }

  infile               = fopen ( infname , "r" ) ;
  outfile              = fopen ( outfname , "w" ) ;         // Open file and write
  errorfile            = fopen ( errorfname , "w" );
  infofile             = fopen ( logfname , "w" );

  sockh                = -1 ;
  session_id           = -1 ;

  if ( ! infile ) {
    printf ( "Could not open file %s for input.\n" , infile ) ;      // Error message while opening file
    exit(1) ;
  }

  if ( ! outfile ) {
    printf ( "Could not open file %s for output\n" ,outfile ) ;      // Error message while opening file
    exit(1) ;
  }

  if ( ! errorfile ) {
    printf ( "Could not open file %s for error\n" ,errorfile  ) ;      // Error message while opening file
    exit(1) ;
  }

  if ( ! infofile ) {
    printf ( "Could not open file %s for error\n" ,infofile  ) ;      // Error message while opening file
    exit(1) ;
  }


   strcat( field,key );

  // Establish a session.

  rc = test_ssa_open ( sockh, &session_id, "default", population,"" );
  if ( 0 != rc )
    doExit ( "test_ssa_open" );

/* Call ssan3_get_ranges
*/
while( fgets (  line_buffer , sizeof (line_buffer), infile  ) ){
rc = test_ssa_get_ranges (
    sockh,
    &session_id,
    "default",
    "population",
     field, line_buffer, 23, "TEXT" );
  if (0 != rc)
    doExit ("test_ssa_get_ranges");
}
/* Close the previously established session.
*/
  rc = test_ssa_close (
    sockh,
    &session_id,
    "default",
    population,
    "");
  if (0 != rc)
    doExit ("test_ssa_close");
t = clock() - t;                                        // End time
  time_taken = ((double)t)/CLOCKS_PER_SEC;                // In seconds
  printf("\nTook %f seconds to execute \n", time_taken);  // Print time



  return (0);
}
