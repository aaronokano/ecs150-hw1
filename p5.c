#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t cpid;

void print_usage( int code ) {
  printf("Usage: simple_sum <first number> <second number>\n");
  exit( code );
}

int main( int argc, char *argv[]){

  int sum, status;
  if(argc != 3)
    print_usage(1);

  cpid = fork();
  if( cpid < 0 ) {
    printf("Failed to create Child\n");
    exit(1);
  }
  else if( cpid == 0 ) {
    sum = atoi( argv[1] ) + atoi( argv[2] );
    exit(sum);
  }
  else {
    waitpid(cpid, &status, 0);
  }
 // printf("The sum is : %d\n", sum2);
  printf("The sum is : %d\n", WEXITSTATUS(status));
  
  return 0;
}
