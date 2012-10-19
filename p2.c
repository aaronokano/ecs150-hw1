#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void fatal_error( const char *message, int code ) {
  printf("%s\n", message );
  exit( code );
}

void upper( char *mystring ) {
  int i, diff = 'a' - 'A';
  for( i = 0; i < strlen( mystring ); i++ ) {
    if( mystring[i] >= 'a' && mystring[i] <= 'z' )
      mystring[i] -= diff;
  }
}

void compare( char *a, char *b ) {
  int i;
  char *longer = b;
  int short_length = strlen( a );
  int long_length = strlen( b );
  if( strlen( b ) < short_length ) {
    short_length = strlen( b );
    long_length = strlen( a );
    longer = a;
  }
  for( i = 0; i < short_length; i++ ) {
    if( a[i] != b[i] )
      printf("Character %d differs:  String 1: %c, String2 %c", i+1, a[i], b[i]);
  }
  if( short_length != long_length ) {
    printf( "The strings are of different length\n"
            "The longer string is %d characters longer, and ends with %s\n",
            strlen( longer + i ), longer + i );
  }
}

void process_three( int fd1[], int fd2[] ) {
  char buf3[1024];
  close( fd1[1] );
  close( fd2[0] );

  if( read( fd1[0], buf3, 1024 ) == -1 )
    fatal_error( "Failed to read from pipe!", 1 );
  // Done reading from that pipe
  close( fd1[0] );
  upper( buf3 );

  write( STDOUT_FILENO, buf3, strlen( buf3 ) );
  write( STDOUT_FILENO, "\n", 1 );

  write( fd2[1], buf3, strlen( buf3 ) );
  close( fd2[1] );
  printf("Terminating process 3\n");
  exit( 0 );
}

void process_two( int fd1[], int fd2[] ) {
  int i, pfd1[2], pfd2[2];
  char buf2[2048], rev_str[1024], tmp;
  pid_t child2_pid;

  close( fd2[0] );
  close( fd1[1] );
  if( read( fd1[0], buf2, 1024 ) == -1 )
    fatal_error( "Read from pipe failed!", 1 );
  close( fd1[0] );

  // Reverse the string
  for( i = strlen( buf2 ) - 1; i >= 0; i-- ) {
    rev_str[ strlen( buf2 ) - i - 1 ] = buf2[ i ];
  }
  write( STDOUT_FILENO, rev_str, strlen( rev_str ) );
  write( STDOUT_FILENO, "\n", 1 );
  
  if( pipe( pfd1 ) == -1 )
    fatal_error( "There was an error creating the pipe!", 1 );
  if( pipe( pfd2 ) == -1 )
    fatal_error( "There was an error creating the pipe!", 1 );

  if( write( pfd1[1], rev_str, strlen( rev_str ) ) == -1 )
    fatal_error( "Failed to write to pipe!", 1 );
  close( pfd1[1] );

  // Create process three
  child2_pid = fork();
  if( child2_pid == -1 )
    fatal_error( "The process failed to fork!", 2 );
  else if( child2_pid == 0 ) {
    process_three( pfd1, pfd2 );
  }
  else {
    char buf4[1024];

    close( pfd1[0] );
    close( pfd2[1] );

    wait( NULL );

    read( pfd2[0], buf4, 1024 );
    close( pfd2[0] );

    strncat( buf2, buf4, 1024 );
    printf("%s\n", buf2);
    if( write( fd2[1], buf2, strlen( buf2 ) ) == -1 )
      fatal_error("Failed to write to pipe!", 1);
    close( fd2[1] );
    printf( "Terminating process 2\n" );
    exit( 0 );
  }
}

int main( int argc, char *argv[] ) {
  // One pipe for each direction, even though they're technically
  // bidirectional, this is because some things in life suck
  int pfd1[2];
  int pfd2[2];

  pid_t child1_pid;
  size_t size = 1024;
  char *buf = (char*) malloc( size );
  char final_buf[2048];

  printf( "Enter a string: " );
  getline( &buf, &size, stdin );
  char *c = strchr( buf, '\n' );
  if( c != NULL )
    *c = '\0';
  printf( "%s\n", buf );

  // Set up pipes
  if( pipe( pfd1 ) == -1 )
    fatal_error( "There was an error creating the pipe!", 1 );
  if( pipe( pfd2 ) == -1 )
    fatal_error( "There was an error creating the pipe!", 1 );

  // Write to pipe
  if( write( pfd1[1], buf, strlen( buf ) + 1 ) == -1 )
    fatal_error( "Failure to write to pipe!", 1 );

  // We don't need to write to the pipe after this, so close the write end
  close( pfd1[1] );

  // Create process two
  child1_pid = fork();
  if( child1_pid == -1 )
    fatal_error( "The process failed to fork!", 2 );
  else if( child1_pid == 0 ) {
    process_two( pfd1, pfd2 );
  }
  else {
    wait( NULL );
    if( read( pfd2[0], final_buf, 2048 ) == -1 )
      fatal_error("Failed to read from pipe!", 1 );
    compare( buf, final_buf );
    printf("Terminating process 1\n");
    exit( 0 );
  }
  return 0;
}
