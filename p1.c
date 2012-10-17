#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

typedef enum { false, true } bool;

void print_usage( ) {
  printf("Usage: compare [-s] [-d] [-u] file1 file2\n"
      "  -s   Compare only until end of shorter file\n"
      "  -d   Indicate files are of different length\n"
      "  -u   Ignore case\n");
  exit( 0 );
}

int min( int one, int two ) {
  if( one > two )
    return two;
  else
    return one;
}

void upper( char *first, char *second ) {
  int i, diff = 'a' - 'A';
  for( i = 0; i < 1024; i++ ) {
    if( first[i] >= 'a' && first[i] <= 'z' )
      first[i] -= diff;
    if( second[i] >= 'a' && second[i] <= 'z' )
      second[i] -= diff;
  }
}

void compare( bool s, bool d, bool u, char* file1, char* file2 ) {
  int fd1, fd2;
  char buf1[1024], buf2[1024];
  int acc = 0; // Accumulator. Counts number of bytes
  int pos;

  fd1 = open( file1, O_RDONLY );
  fd2 = open( file2, O_RDONLY );

  int read_ret1 = read( fd1, buf1, 1024 );
  int read_ret2 = read( fd2, buf2, 1024 );

  while( read_ret1 > 0 && read_ret2 > 0 ) {
    // Convert all to uppercase if ignore case is specified
    if( u )
      upper( buf1, buf2 );

    // Actually compare individual characters if the -s option is specified
    if( s ) {
      for( pos = 0; pos < min( read_ret1, read_ret2 ); pos++ ) {
        if( buf1[pos] != buf2[pos] )
          printf("Byte %d    File1:%c	File2:%c\n", acc + pos + 1, buf1[pos], buf2[pos]);
      }
    }
    // Report whether the files are of different length if the -d flag is set
    if( d ) {
      if( read_ret1 != read_ret2 ) {
        printf("The files are of different length\n");
        exit( 0 );
      }
    }
    // We're reading 1024 bytes at a time, so we need to keep track of how many
    // have been read
    acc += min( read_ret1, read_ret2 );

    read_ret1 = read( fd1, buf1, 1024 );
    read_ret2 = read( fd2, buf2, 1024 );
  }
  close( fd1 );
  close( fd2 );
}

int main( int argc, char* argv[] ) {
  int next_option;
  int index = 1; // Stupid optind doesn't increment on its own :(

  // Short options
  const char* const short_options = "sdu\0";
  // Long options, which I'm making up right now

  bool shorter = false, // To indicate terminate at shorter file
       differ  = false, // To indicate files of different length
       upper   = false; // To indicate case insensitive
  do {
    next_option = getopt( argc, argv, short_options );
    switch( next_option ) {
      case 's':
        shorter = true;
        break;
      case 'd':
        differ = true;
        break;
      case 'u':
        upper = true;
        break;
      case '?': // Unrecognized option
        print_usage( );
      case -1:  // Done with options
        break;
      default:  // This should never happen, but just in case
        abort();
    }
    index++;
  } while( next_option != -1 );

  if( index + 1 < argc ) { // If there are not two filenames present
    print_usage();
  }

  compare( shorter, differ, upper, argv[optind], argv[optind + 1] );
  return 0;
}
