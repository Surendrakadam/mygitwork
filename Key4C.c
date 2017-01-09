/* Creates key file on Person_Name from input tagged data file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ssan3cl.h"

#define rangeof( arr )( sizeof( arr )/sizeof( ( arr )[0] ) )


  FILE    *outfile ,*errorfile;                                        // Object of output file
  int     rec_number = 0 ;

static long test_ssa_open (
  long    sockh,
  long    *session_id,
  char    *sysName,
  char    *population,
  char    *controls )
{
  long    rc;
  char    rsp_code[ SSA_SI_RSP_SZ ];
  char    ssa_msg[ SSA_SI_SSA_MSG_SZ ];

  printf ( "------ ssan3_open ------\n" );
  printf ( "Session Id       : %ld\n", *session_id );
  printf ( "System           : %s\n", sysName );
  printf ( "Population       : %s\n", population );
  printf ( "Controls         : %s\n", controls );

  rc = ssan3_open ( sockh,
    session_id,
    sysName,
    population,
    controls,
    rsp_code,
    SSA_SI_RSP_SZ,
    ssa_msg,
    SSA_SI_SSA_MSG_SZ );
  if ( rc < 0 ) {
    printf ( "rc               : %ld\n", rc );
    rc = 1;
    goto ret;
  }
  if ( rsp_code[0] != '0' ) {
    printf ( "rsp_code         : %s\n", rsp_code );
    printf ( "ssa_msg          : %s\n", ssa_msg );
    rc = 1;
    goto ret;
  }
  printf ( "\n" );
  printf ( "Session Id       : %ld\n", *session_id );
  printf ( "rsp_code         : %s\n", rsp_code );
  //printf ( "ssa_msg          : %s\n", ssa_msg );
  fputs ( ssa_msg,errorfile );
  rc = 0;
  goto ret;

ret:
  return ( rc );
}

static long test_ssa_get_keys (
  long    sockh,
  long    *session_id,
  char    *sysName,
  char    *population,
  char    *controls,
  char    *record,
  long    recordLength,
  char    *recordEncType )
{
  int     i;
  long    rc;
  char    *keys_array [ SSA_SI_MAX_KEYS ];
  char    keys_data [ SSA_SI_MAX_KEYS*SSA_SI_KEY_SZ ];
  char    rsp_code [ SSA_SI_RSP_SZ ];
  char    ssa_msg [ SSA_SI_SSA_MSG_SZ ];
  long    num;
  char    *p;

  for ( i = 0; i < ( int )rangeof ( keys_array ); ++i )
    keys_array[i] = keys_data + i * SSA_SI_KEY_SZ;

  /* printf ( "------ ssan3_get_keys_encoded ------\n" );
  printf ( "Session Id       : %ld\n", *session_id );
  printf ( "System           : %s\n", sysName );
  printf ( "Population       : %s\n", population );
  printf ( "Controls         : %s\n", controls );
  printf ( "Key field data   : %s\n", record );
  printf ( "Key field size   : %ld\n", recordLength );
  printf ( "Key field encoding type : %s\n", recordEncType );*/

  num = 0;
  rc = ssan3_get_keys_encoded (
                                sockh,
                                session_id,
                                sysName,
                                population,
                                controls,
                                rsp_code,
                                SSA_SI_RSP_SZ,
                                ssa_msg,
                                SSA_SI_SSA_MSG_SZ,
                                record,
                                recordLength,
                                recordEncType,
                                &num,
                                keys_array,
                                SSA_SI_KEY_SZ );

  if ( rc < 0 ) {
    printf ( "rc               : %ld\n", rc );
    rc = 1;
    goto ret;
  }
  if ( rsp_code[0] != '0' ) {
    //printf ( "rsp_code         : %s\n", rsp_code );
    //printf ( "ssa_msg          : %s\n", ssa_msg );
    rc = -1;
    goto ret;
  }

  /*printf ( "\n" );
  printf ( "Session Id       : %ld\n", *session_id );
  printf ( "rsp_code         : %s\n", rsp_code );
  printf ( "ssa_msg          : %s\n", ssa_msg );
  printf ( "Count            : %ld\n", num );
  printf ( "\n------ keys ------\n" );*/

  for ( i = 0; i < num; ++i ) {
    p = keys_array[i];
    ++rec_number;
    fprintf ( outfile,"%d : ",rec_number );                 // Write Record no
    fputs ( strcat( record," : " ) , outfile );             // Write Record
    fputs ( ( i+1, p ) ,outfile  ) ;                        // Write Key on output file
    fputs ( "\n",outfile );
  }
  rc = 0;
  goto ret;

ret:
  return ( rc );
}

static long test_ssa_close (
  long    sockh,
  long    *session_id,
  char    *sysName,
  char    *population,
  char    *controls )
{
  long    rc;
  char    rsp_code[SSA_SI_RSP_SZ];
  char    ssa_msg[SSA_SI_SSA_MSG_SZ];

  printf ( "------ ssan3_close ------\n" );
  printf ( "Session Id       : %ld\n", *session_id );
  printf ( "System           : %s\n", sysName );
  printf ( "Population       : %s\n", population );
  printf ( "Controls         : %s\n", controls );

  rc = ssan3_close ( sockh,
    session_id,
    sysName,
    population,
    controls,
    rsp_code,
    SSA_SI_RSP_SZ,
    ssa_msg,
    SSA_SI_SSA_MSG_SZ );
  if ( rc < 0 ) {
    printf ( "rc               : %ld\n", rc );
    rc = 1;
    goto ret;
  }
  if ( rsp_code[0] != '0' ) {
    printf ( "rsp_code         : %s\n", rsp_code );
    printf ( "ssa_msg          : %s\n", ssa_msg );
    rc = 1;
    goto ret;
  }
  printf ( "\n" );
  printf ( "Session Id       : %ld\n", *session_id );
  printf ( "rsp_code         : %s\n", rsp_code );
  printf ( "ssa_msg          : %s\n", ssa_msg );
  rc = 0;
  goto ret;

ret:
  return ( rc );
}

static void doExit ( char *func )
{
  printf ( "Error occurred in '%s'\n", func );
  exit ( EXIT_FAILURE );
}

int main ( )
{
  long    rc;
  long    sockh;
  long    session_id;

  char    *population  = "India" ;                           // Population
  char    *field       = "FIELD=" ;
  char    *key         = "Person_Name" ;                     // key
  char    *key_level   = "Standard" ;                        // Key level
  char    *infname     = "Tagged_Data.txt" ;                 // Input file name
  char    *outfname    = "Tagged_Data_output.txt" ;          // Output File name
  char    *errorfname  = "Error_File.txt" ;                  // Error File name

  FILE    *infile ;
  char line_buffer [ BUFSIZ ] ;

  infile               = fopen ( infname , "r" ) ;
  outfile              = fopen ( outfname , "w" ) ;         // Open file and write
  errorfile            = fopen ( errorfname , "w" );

  sockh                = -1 ;
  session_id           = -1 ;

  if ( ! infile ) {
    printf ( "Could not open file %s for input.\n" , infname  ) ;
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


   strcat( field,key );

  // Establish a session.

  rc = test_ssa_open ( sockh, &session_id, "default", population,"" );
  if ( 0 != rc )
    doExit ( "test_ssa_open" );

  // Call ssan3_get_keys

  while( fgets (  line_buffer , sizeof (  line_buffer  ), infile  ) ){
    int len_line=strlen( line_buffer ) ;
    if ( len_line > 0 && line_buffer[len_line-1] == '\n' ) {
      line_buffer[--len_line] = '\0';
    }

  rc = test_ssa_get_keys ( sockh, &session_id, "default", population, field, line_buffer, len_line, "TEXT" );
  //printf ( "Key level: %s\n",key_level ) ;                    //%s for character
  if ( 0 != rc )
    doExit ( "test_ssa_get_keys" );
  }

  // Close the previously established session.
  rc = test_ssa_close ( sockh,&session_id,"default",population,"" );
  if ( 0 != rc )
    doExit ( "test_ssa_close" );

  exit ( 0 );
}
//End of script Key4C.c
