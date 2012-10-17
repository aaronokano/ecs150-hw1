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
  int i, pipe21_fd[2], pipe22_fd[2];
  char buf2[2048], rev_str[1024], tmp;
  pid_t child2_pid;
  if( read( fd1[0], buf2, 1024 ) == -1 )
    fatal_error( "Read from pipe failed!", 1 );
  close( fd1[0] );
  // Reverse the string
  for( i = strlen( buf2 ) - 1; i >= 0; i-- ) {
    rev_str[ strlen( buf2 ) - i - 1 ] = buf2[ i ];
  }
  write( STDOUT_FILENO, rev_str, strlen( rev_str ) );
  write( STDOUT_FILENO, "\n", 1 );
  
  if( pipe( pipe21_fd ) == -1 )
    fatal_error( "There was an error creating the pipe!", 1 );
  if( pipe( pipe22_fd ) == -1 )
    fatal_error( "There was an error creating the pipe!", 1 );

  if( write( pipe21_fd[1], rev_str, strlen( rev_str ) ) == -1 )
    fatal_error( "Failed to write to pipe!", 1 );
  close( pipe21_fd[1] );

  // Create process three
  child2_pid = fork();
  if( child2_pid == -1 )
    fatal_error( "The process failed to fork!", 2 );
  else if( child2_pid == 0 ) {
    process_three( pipe21_fd, pipe22_fd );
  }
  else {
    close( pipe21_fd[0] );
    close( pipe22_fd[1] );
    char buf4[1024];
    wait( NULL );
    read( pipe22_fd[0], buf4, 1024 );
    close( pipe22_fd[0] );
    strncat( buf2, buf4, 1024 );
    write( STDOUT_FILENO, buf2, strlen( buf2 ) );
    write( STDOUT_FILENO, "\n", 1 );
    printf( "Terminating process 2\n" );
    exit( 0 );
  }
}

int main( int argc, char *argv[] ) {
  // One pipe for each direction, even though they're technically
  // bidirectional, this is because some things in life suck
  int pipe11_fd[2];
  int pipe12_fd[2];

  pid_t child1_pid;
  char buf[1024];

  printf( "Enter a string: " );
  scanf( "%s", buf );
  printf( "%s\n", buf );

  // Set up pipes
  if( pipe( pipe11_fd ) == -1 )
    fatal_error( "There was an error creating the pipe!", 1 );
  if( pipe( pipe12_fd ) == -1 )
    fatal_error( "There was an error creating the pipe!", 1 );

  // Write to pipe
  if( write( pipe11_fd[1], buf, strlen( buf ) ) == -1 )
    fatal_error( "Failure to write to pipe!", 1 );

  // We don't need to write to the pipe after this, so close the write end
  close( pipe11_fd[1] );
  // We also never need to write to this
  close( pipe12_fd[1] );

  // Create process two
  child1_pid = fork();
  if( child1_pid == -1 )
    fatal_error( "The process failed to fork!", 2 );
  else if( child1_pid == 0 ) {
    process_two( pipe11_fd, pipe12_fd );
  }
  else {
    wait( NULL );
    exit( 0 );
  }
  return 0;
}
