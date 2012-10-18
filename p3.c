#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

pid_t cpid;

void alarm_handler( int parameter ) {
  kill( cpid, SIGKILL );
  exit( 0 );
}

void print_usage( int code ) {
  printf("Usage: kill_it <program to run> <time to run program>\n");
  exit( code );
}


int main( int argc, char* argv[] ) {
  if( argc != 3 )
    print_usage( 1 );

  signal( SIGALRM, alarm_handler );
  alarm( atoi( argv[2] ) );
  cpid = fork();
  if( cpid == -1 ) {
    printf("Failed to create child\n");
    exit( 1 );
  }
  else if( cpid == 0 ) {
    execl( argv[1], NULL );
  }
  else {
    pause();
  }
  return 0;
}
  
