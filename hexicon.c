/*===================================================================
    hexicon - a rudimentary hexadecimal CLI inspection tool.
    / [hexi]decimal [con]sole /

    Author  : White Guy That Don't Smile
    Date    : 2016/05/17, Tuesday, May 17th; 1631 HOURS

    Something I wrote years ago after growing frustrated
    with the fact that opening a hex editor every time, just
    to read hex data, is very inconvenient, especially, when
    the editor takes up more screen space than it should.

    Archiving this due to uncertainty over, whether or not,
    I'll have convenient, local access to this in the future.
===================================================================*/
#include <stdio.h>
#include <stdlib.h>

#ifdef __x86_64__
#define ALIGNMENT "                  "
#define LOWER_BITS 60
#else
#define ALIGNMENT "          "
#define LOWER_BITS 28
#endif

/*=========================================================*/

/*===========================================================
[The C Programming Language - 2nd Edition: Ritchie-Kernighan]
{ pg. 249, Appendix B3 }
Character Class Tests: <ctype.h> (continued)

  int toupper(int c);  "convert c to upper case"
===========================================================*/
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
    this function is written based on interpretation.
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
static int __toupper( int c )
{
  if ( c >= 'a' && c <= 'z' )
    return (int)((c ^ ' ') & 0x7F);
  else
    return c;
}



/*===========================================================
[The C Programming Language - 2nd Edition: Ritchie-Kernighan]
{ pg. 250, Appendix B3 }
String Functions: <string.h> (continued)

  char *strpbrk(cs,ct);

    "return pointer to first occurrence in string cs of any
     character of string ct, or NULL if none are present."
===========================================================*/
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
    this function is written based on interpretation.
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
static char * __strpbrk( const char *cs, const char *ct )
{
  int c, t;

  for ( c = 0; *(cs + c) != '\0'; c++ )
    for ( t = 0; *(ct + t) != '\0'; t++ )
      if ( *(cs + c) == *(ct + t) )
        return (char *)(cs + c);
    ;
  ;

  return (char *)0;
}

/*=========================================================*/


static void regress( FILE *data, long *r );



static void usage( void )
{
  printf( "Usage: hexicon [display] [infile]\n"           );
  printf( "> b  : will display groups of  8.\n"           );
  printf( "> w  : will display groups of 16.\n"           );
  printf( "> d  : will display groups of 32.\n\n"         );
  printf( "> -  : will regress to a displayed section.\n" );
  printf( "> .  : will terminate the program.\n\n"        );
  return;
}



int main( int argc, char *argv[] )
{
  FILE *data = (FILE *)0;
  char *s = (char *)0;
  long  f, /*l,*/ r = 0L;
  int   c, g, n, o = 0;

  if ( argc != 3 )
  {
    usage();
    exit( EXIT_FAILURE );
  }

  if (    (s = argv[1], s[1] || __strpbrk( s, "BWDbwd" ) == (char *)0)
       || (s = argv[2], (data = fopen(argv[2], "rb")) == (FILE *)0) )
  {
    printf( "??? : \"%s\"\n", s );
    usage();
    exit( EXIT_FAILURE );
  }

  g = __toupper( *argv[1] );
  g = ((g == 'B') ? 1 : ((g == 'W') ? 2 : 4));

  do
  {
    if ( o == '-' )
      regress( data, &r );
    else
    {
      /* insert [ALIGNMENT] space characters for padding
         the header displaying [f]iled offsets. */
      /*=============================================
      [alignment padding example (32-bit)]
      >vvvvvvvvvv
      >..........0........4........8........C........
      >00000000:.59617930.00000022.00000014.00000016.
      >00000010:.FFFFFFFE.10014545.45454545.45454545.
      >00000020:.45454545.45454545.45454545.45454545.
      >00000030:.45454545.45454545.
      =============================================*/
      printf( ALIGNMENT );

      /* iterate [f]iles from 0x0 to 0xF while inserting
         divisive space characters every [g]roup breadth. */
      /*=========================================================
      [byte example]
      >           gg gg gg gg gg gg gg gg gg gg gg gg gg gg gg gg
      >..........0..1..2..3..4..5..6..7..8..9..A..B..C..D..E..F..
      >00000000:.59.61.79.30.00.00.00.22.00.00.00.14.00.00.00.16.
      -----------------------------------------------------------
      [word example]
      >           gggg gggg gggg gggg gggg gggg gggg gggg
      >..........0....2....4....6....8....A....C....E....
      >00000000:.5961.7930.0000.0022.0000.0014.0000.0016.
      -----------------------------------------------------------
      [dword example]
      >           gggggggg gggggggg gggggggg gggggggg
      >..........0........4........8........C........
      >00000000:.59617930.00000022.00000014.00000016.
      =========================================================*/
      for ( f = 0; f < 0x10; f++ )
      {
        if ( (f % (long)g) == 0 )
          printf( "%lX", (unsigned long)f );

        if ( (f & (long)(g - 1)) == 0L )
          for ( n = 0; n < g; n++ )
            printf( "  " ); /* .. */
      }

      for ( n = 0; n < 0x100; n++ )
      {
        if ( (c = fgetc(data)) != EOF )
        {
          if ( (n % g) == 0 )
          {
            o = n & 0xFF;
            o &= 0xF;

            if ( !o )
            {
              printf( "\n" ); /* newline: header [f]iles to [r]anks. */

              /* determine number of bits in [r]ank... */
              for ( f = 0L; (unsigned long)(r >> f) != 0UL && r > 0L; f++ );

              /* ...to fill nibbles with [0] characters
                 preceding the current rank value. */
              for ( o = (int)LOWER_BITS; ((long)o >= f) && (o > 0); o -= 4 )
                printf( "0" );

              printf( "%lX: ", (unsigned long)r );
              r += 0x10L;
            }
            else
              printf( " " ); /* next [f]ile group (1/2/4) */
          }

          printf( (c < 0x10) ? "0%1X" : "%2X", (unsigned int)c );
        }
        else
          break;
      }

      printf( "\n" ); /* next [r]ank */
    }
  }
  while ( (o = getchar()) != '.' );

  exit( EXIT_SUCCESS );
}



static void regress( FILE *data, long *r )
{
  long l = ftell( data );

  if ( (l % 0x100L) == 0L )
  {
    if ( l > 0L )
    {
      long t = (l > 0x100L) ? 0x200L : 0x100L;

      fseek( data, (l - t), SEEK_SET );
      *r -= t;
    }
  }
  else
  {
    fseek( data, (l - (l & 0xFFL)), SEEK_SET );
    *r -= (*r % 0x100L);
    l = ftell( data );

    if ( l >= 0x100L )
    {
      fseek( data, ((l - (l % 0x100L)) - 0x100L), SEEK_SET );
      *r -= 0x100L;
    }
  }

  return;
}
