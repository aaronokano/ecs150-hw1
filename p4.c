#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

void print_usage( ) {
  printf("Usage: move_files <files ...> directory\n");
  exit( 0 );
}

int main( int argc, char *argv[])
{
  int retval;
  int i;
  char * s;
  char * cwd;
  cwd = getcwd(0,0);
  char * oldpath = (char*) malloc( strlen( cwd  ) + 256 );
  if( strlen( cwd ) > strlen( argv[ argc - 1 ] ) )
    s = (char*) malloc( strlen( cwd ) + strlen( argv[ argc - 1] ) + 1 );
  else
    s = (char*) malloc( strlen( argv[ argc - 1 ] ) );
  s[0] = '\0';
  char * newpath = (char*) malloc( strlen( s ) + 256 );
  if( argv[ argc - 1][0] != '/' ) 
    strcpy( s, cwd );
  strcat( s, "/" );
  char * tok;
  tok = strtok( argv[ argc - 1 ] ,"/");
  while (tok != NULL)
  {
    strcat( s, tok );
    mkdir( s, 0777 );
    tok = strtok (NULL, "/");
    strcat( s, "/" );
  }
  for( i = 1; i < argc - 1; i++ ) {
    strcpy( oldpath, cwd );
    strcat( oldpath, "/");
    strcat( oldpath, argv[i]);
    strcpy( newpath, s );
    strcat( newpath, argv[i]);
    retval = link( oldpath, newpath);
    if( retval != 0 ) {
       if( errno == 2 )
         printf("No file '%s' found in current directory\n", argv[i] );
       else
         printf("Problem creating new link\n");
      } else {
      retval = unlink( oldpath );
      if( retval != 0 ) {
        printf("Problem deleting old file\n");
      }
    }
  }
  return 0;
}
