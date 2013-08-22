/* SWAPDCP -- list and/or swap keys in a KEYBOARD.DCP file
 * (C) 1992 by Ned Konz -- FREELY DISTRIBUTABLE AND USABLE
 * 6/13/92 version
 */

char *logo_strings[] =
{
	"SWAPDCP -- list and/or swap keys in a KEYBOARD.DCP file (6/13/92)",
	"(C) 1992 Ned Konz -- FREELY DISTRIBUTABLE AND USABLE",
	"",
	"MAKE A COPY OF YOUR KEYBOARD.DCP FILE TO RUN THIS PROGRAM ON!!!",
	"THIS WILL EDIT THE GIVEN FILE IN PLACE!!!",
	"",
	"BIX: nkonz, CI$: 76046,223",
	"",
	0
};

char *usage_strings[] =
{
	"Usage: SWAPDCP [-l n] [-s k1 k2 [...]] [-h] filename.DCP",
	"   -l n         sets listing level to n ( 0-3 )",
	"                0: no listing output",
	"                1: basic info, status output (default)",
	"                2: level 1 plus table headers",
	"                3: level 2 plus key entries",
	"   -s k1 k2     swaps keys with scan codes k1 and k2",
	"                (multiple -s arguments are processed in order)",
	"   -h           prints extended help to standard output and quits",
	"                (run as \"SWAPDCP -h | more\" or \"SWAPDCP -h >file\")",
	0
};

char *doc_strings[] =
{
	"For instance, to swap the control and caps-lock keys on your keyboard,",
	"you might use (assuming OS/2 is on your drive C:):",
	"",
	"    COPY C:\\OS2\\KEYBOARD.DCP C:\\OS2\\MYKBD.DCP",
	"    SWAPDCP -s 29 58 C:\\OS2\\MYKBD.DCP",
	"",
	"This swaps the key definitions in-place.",
	"",
	"You could then change the line in your CONFIG.SYS which goes something like:",
	"    DEVINFO=KBD,US,C:\\OS2\\KEYBOARD.DCP",
	"to:",
	"    DEVINFO=KBD,US,C:\\OS2\\MYKBD.DCP",
	"",
	"The next time you start up, your keys will be changed.",
	0
};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "swapdcp.h"

/* Globals */
FILE *infile = NULL;	/* input/output file handle */
char *filename = NULL;	/* input/output filename */

int listing = LIST_BASIC;		/* listing level, 0-LIST_KEYS */
int swapping = 0;		/* number of key pairs to swap */
int swapped_keys[2][MAX_SWAPPED];	/* swapped key pairs */

void swap_keys( void *table, int num )
{
	int i;
	KeyDef *keytable = table;
	for ( i = 0; i < swapping; i++ )
	{
		int k1 = swapped_keys[0][i] - 1;	/* correct for index */
		int k2 = swapped_keys[1][i] - 1;
		if ( keytable[ k1 ].XlateOp & AccentFlags )
		{
			printf( "ERROR: can't swap accented key %d\n", k1 );
			return;
		}
		if ( keytable[ k2 ].XlateOp & AccentFlags )
		{
			printf( "ERROR: can't swap accented key %d\n", k2 );
			return;
		}
		if ( k1 < num && k2 < num )
		{
			KeyDef tmp = keytable[ k1 ];
			keytable[ k1 ] = keytable[ k2 ];
			keytable[ k2 ] = tmp;
		}
	}
}

void print_shift( unsigned sf, char *prefix )
{
	if ( !sf )
		return;
	if ( prefix )
	{
		printf( "%s", prefix );
	}
	if ( sf & SF_RIGHTSHIFT ) printf( "RIGHTSHIFT " );
	if ( sf & SF_LEFTSHIFT ) printf( "LEFTSHIFT " );
	if ( sf & SF_CONTROL ) printf( "CONTROL " );
	if ( sf & SF_ALT ) printf( "ALT " );
	if ( sf & SF_SCROLLLOCK_ON ) printf( "SCROLLLOCK_ON " );
	if ( sf & SF_NUMLOCK_ON ) printf( "NUMLOCK_ON " );
	if ( sf & SF_CAPSLOCK_ON ) printf( "CAPSLOCK_ON " );
	if ( sf & SF_INSERT_ON ) printf( "INSERT_ON " );
	if ( sf & SF_LEFTCONTROL ) printf( "LEFTCONTROL " );
	if ( sf & SF_LEFTALT ) printf( "LEFTALT " );
	if ( sf & SF_RIGHTCONTROL ) printf( "RIGHTCONTROL " );
	if ( sf & SF_RIGHTALT ) printf( "RIGHTALT " );
	if ( sf & SF_SCROLLLOCK ) printf( "SCROLLLOCK " );
	if ( sf & SF_NUMLOCK ) printf( "NUMLOCK " );
	if ( sf & SF_CAPSLOCK ) printf( "CAPSLOCK " );
	if ( sf & SF_SYSREQ ) printf( "SYSREQ " );
}

void pkey( KeyDef *key, char *name )
{
	BYTE *p = &key->Char1;
	printf( "%s (", name);
	while (p <= &key->Char5)
	{
		if ( isprint( *p ) )
			printf("'%c'", (unsigned)*p);
		else
			printf("%#x", (unsigned)*p);

		if ( p == &key->Char5 )
			printf(")");
		else
			printf(",");
		p++;
	}
}

void print_keys( void *table, int num )
{
	KeyDef *key = table;
	int i;

	for ( i=0; i<num; i++ )
	{
		unsigned keytype = ( unsigned )( key[i].XlateOp & KeyType );
		unsigned keyflags = ( unsigned )( key[i].XlateOp & AccentFlags );

		if ( keytype )
		{
			printf( "%3d (%#02x): ", i+1, i+1 );
			switch ( keytype )
			{
				case 0x01:		/* AlphaKey */
					pkey( key+i, "AlphaKey" );
				break;
				case 0x02:		/* SpecKey */
					pkey( key+i, "SpecKey" );
				break;
				case 0x03:		/* SpecKeyC */
					pkey( key+i, "SpecKeyC" );
				break;
				case 0x04:		/* SpecKeyA */
					pkey( key+i, "SpecKeyA" );
				break;
				case 0x05:		/* SpecKeyCA */
					pkey( key+i, "SpecKeyCA" );
				break;
				case 0x06:		/* FuncKey */
					pkey( key+i, "FuncKey" );
				break;
				case 0x07:		/* PadKey */
					pkey( key+i, "PadKey" );
				break;
				case 0x08:		/* SpecCtlKey */
					pkey( key+i, "SpecCtlKey" );
				break;
				case 0x09:		/* PrtSc */
					pkey( key+i, "PrtSc" );
				break;
				case 0x0a:		/* SysReq */
					pkey( key+i, "SysReq" );
				break;
				case 0x0b:		/* AccentKey */
					pkey( key+i, "AccentKey" );
				break;
				case 0x0c:		/* ShiftKey */
					pkey( key+i, "ShiftKey" );
					print_shift( (unsigned)key[i].Char1, " " );
					print_shift( (unsigned)key[i].Char2 << 8, "/ " );
					print_shift( (unsigned)key[i].Char3 << 8,  "/ " );
				break;
				case 0x0d:		/* ToggleKey */
					pkey( key+i, "ToggleKey" );
				break;
				case 0x0e:		/* AltKey */
					pkey( key+i, "AltKey" );
				break;
				case 0x0f:		/* NumLock */
					pkey( key+i, "NumLock" );
				break;
				case 0x10:		/* CapsLock */
					pkey( key+i, "CapsLock" );
				break;
				case 0x11:		/* ScrollLock */
					pkey( key+i, "ScrollLock" );
				break;
				case 0x12:		/* XShiftKey */
					pkey( key+i, "XShiftKey" );
				break;
				case 0x13:		/* XToggleKey */
					pkey( key+i, "XToggleKey" );
				break;
				case 0x14:		/* SpecKeyCS */
					pkey( key+i, "SpecKeyCS" );
				break;
				case 0x15:		/* SpecKeyAS */
					pkey( key+i, "SpecKeyAS" );
				break;
				case 0x1a:		/* ExtExtKey */
					pkey( key+i, "ExtExtKey" );
				break;
				default:		/* reserved */
					printf( "type %#x", keytype );
					pkey( key+i, "" );
				break;
			}	/* switch keytype */

			if ( keyflags )
				printf( " AccFlags:%#x\n", keyflags );
			else
				printf( "\n" );
		}	/* if keytype */
	}	/* for i */
}

#define PFLAG( flag ) printf( " " #flag ":%d", header->XTableFlags1.##flag )

void do_entry( IndexEntry *entry )
{
	long table_base;
	XHeader *header = calloc( sizeof( XHeader ), 1 );
	void *keys = NULL;

	fseek( infile, ( long )entry->HeaderLocation, SEEK_SET );
	if ( fread( header, sizeof( XHeader ), 1, infile ) != 1 ) 
	{
		fprintf( stderr, "ERROR: can't read table header\n" );
		return;
	}
	table_base = ftell( infile );

	if ( listing >= LIST_KEYS  || swapping )
	{
		keys = calloc( header->XtableLen, 1 );
		if ( fread( keys, header->XtableLen, 1, infile ) != 1 )
		{
			fprintf( stderr, "ERROR: can't read key table\n" );
			return;
		}
	}

	if ( listing >= LIST_HEADERS )
	{
		printf( "ID:%d", header->XTableID );
		PFLAG( ShiftAlt );
		PFLAG( AltGrafL );
		PFLAG( AltGrafR );
		PFLAG( ShiftLock );
		PFLAG( DefaultTable );
		printf( "\n" );
		PFLAG( ShiftToggle );
		PFLAG( AccentPass );
		PFLAG( CapsShift );
		PFLAG( MachDep );
		printf( "\nKbdType:%d, KbdSubType:%d, XtableLen:%#x, EntryCount:%d,\n",
			header->KbdType, header->KbdSubType, header->XtableLen,
			header->EntryCount );
		printf( "Country:\"%2.2s %4.4s\", TableTypeID:%d\n", header->Country,
			header->SubCountryID, header->TableTypeID );
		if ( listing >= LIST_KEYS )
			print_keys( keys, header->EntryCount );
	}

	if ( swapping )
	{
		swap_keys( keys, header->EntryCount );
		fseek( infile, table_base, SEEK_SET );
		if ( fwrite( keys, header->XtableLen, 1, infile ) != 1 )
		{
			fprintf( stderr, "ERROR: can't write key table\n" );
		}
	}

	if ( header ) free( header );
	if ( keys )   free( keys );
}

void print_index( ULONG index_location )
{
    WORD number_of_entries = 0;
    WORD i;
    IndexEntry *entry;
	int nr;

    fseek( infile, ( long )index_location, SEEK_SET );
    fread( &number_of_entries, sizeof( number_of_entries ), 1, infile );

	if ( listing >= LIST_BASIC )
	{
		printf( "%d keyboard tables in file\n", number_of_entries );
	}
	entry = calloc( number_of_entries, sizeof( IndexEntry ) );

	if ( ( nr = fread( entry, sizeof(IndexEntry), number_of_entries, infile ) )
			!= number_of_entries )
	{
		fprintf( stderr, "ERROR: only read %d of %d index entries\n",
			nr, number_of_entries );
		return;
	}

    for ( i = 0; i < number_of_entries; i++ )
	{
		if ( entry[i].HeaderLocation )
		{
			if ( listing >= LIST_HEADERS )
			{
				printf( "======== table %d =========\n", i );
				printf( "%2.2s %4.4s ID: %d table loc: %#lx ( %04x %04x %04x )\n",
					entry[i].Country,
					entry[i].SubCountryID,
					( unsigned )entry[i].XTableID,
					( unsigned long )entry[i].HeaderLocation,
					( unsigned )entry[i].word1,
					( unsigned )entry[i].word2,
					( unsigned )entry[i].word3 );
			}
            do_entry( entry+i );
		}
    }
    free( entry );
}

/* prints usage message to given output */
void usage( char **strings, FILE *out )
{
	char **s;
	for ( s=strings; *s; s++ )
	{
		fprintf( out, "%s\n", *s );
	}
}

int main( int argc, char *argv[] )
{
    ULONG index_location = 0UL;

	for ( --argc, ++argv; argc > 0; argc--, argv++ )
	{
		if ( !stricmp( *argv, "-h" ) )
		{
			usage( logo_strings, stdout );
			usage( usage_strings, stdout );
			printf( "\n" );
			usage( doc_strings, stdout );
			return 0;
		}
		else if ( !stricmp( *argv, "-l" ) )
		{
			if ( argc >= 2 )
			{
				listing = atoi( *++argv );
				--argc;
			}
			else
			{
				fprintf( stderr, "ERROR: missing listing level\n" );
				usage( logo_strings, stderr );
				usage( usage_strings, stderr );
				return 1;
			}
		}
		else if ( !stricmp( *argv, "-s" ) )
		{
			if ( argc >= 3 )
			{
				if ( swapping < MAX_SWAPPED )
				{
					int key1 = atoi( *++argv );
					int key2 = atoi( *++argv );
					argc -= 2;
					swapped_keys[0][ swapping ] = key1;
					swapped_keys[1][ swapping ] = key2;
					swapping++;
				}
				else
				{
					fprintf( stderr, "ERROR: too many swap pairs ( maximum %d )\n",
						MAX_SWAPPED );
					usage( logo_strings, stderr );
					usage( usage_strings, stderr );
					return 1;
				}
			}
			else
			{
				fprintf( stderr, "ERROR: need two key codes after -s\n" );
				usage( logo_strings, stderr );
				usage( usage_strings, stderr );
				return 1;
			}
		}
		else if ( **argv == '-' )
		{
			fprintf( stderr, "ERROR: unrecognized option \"%s\"\n", *argv );
			usage( logo_strings, stderr );
			usage( usage_strings, stderr );
			return 1;
		}
		else
		{
			filename = *argv;
		}
	}

    if ( filename == NULL )
	{
		fprintf( stderr, "ERROR: missing filename\n" );
		usage( logo_strings, stderr );
		usage( usage_strings, stderr );
		return 1;
    }

	if ( listing >= LIST_BASIC )
	{
		usage( logo_strings, stdout );
	}

    if ( (infile = fopen( filename, "r+b" ) ) == NULL )
	{
        fprintf( stderr, "ERROR: can't open file \"%s\" for read/write\n",
			filename );
        return 2;
    }

    /* read location of index */
    fread( &index_location, sizeof( index_location ), 1, infile );

	if ( listing >= LIST_HEADERS )
	{
		printf( "table index at %#lx\n", ( long )index_location );
	}
    print_index( index_location );

	fclose( infile );
    return 0;
}

