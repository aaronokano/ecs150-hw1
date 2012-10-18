#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t cpid;

int main( int argc, char *argv[]) {

  int op, status;

  cpid = fork();
  if( cpid < 0 ) {
    printf("Failed first fork\n");
    exit(1);
  }
  else if (cpid == 0) {
    cpid = fork();
    if( cpid < 0 ) {
      printf("Failed second fork\n");
      exit(1);
    }
    else if ( cpid == 0 ) {
      op = atoi( argv[1] ) +  atoi( argv[2] );
      exit(op);
    }
    else {
      waitpid(cpid, &status, 0);
      //printf("The sum is: %d\n", WEXITSTATUS(status));
      op = (int)WEXITSTATUS(status) * 2;
      exit(op);
    }
  }
  else {
    waitpid(cpid, &status, 0);
    printf("The total is %d\n", WEXITSTATUS(status));
  }
  return 0;
}

