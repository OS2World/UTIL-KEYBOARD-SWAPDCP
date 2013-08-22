/* SWAPDCP header file */

#if defined( __IBMC__ )
	#pragma pack(1)
	typedef unsigned short 	WORD;    /* 16-bit unsigned thing */
	typedef unsigned char  	BYTE;
	typedef unsigned 		BITFIELD;  /* for readability */
	typedef unsigned long  	ULONG;
#elif defined( __ZTC__ )
	#pragma ZTC align 1
	typedef unsigned short 	WORD;    /* 16-bit unsigned thing */
	typedef unsigned char  	BYTE;
	typedef unsigned long	BITFIELD;  /* for readability */
	typedef unsigned long  	ULONG;
#else
#error	"unknown compiler"
#endif

/* code page header */
typedef struct XHeader
{
    WORD    XTableID;       /* code page number */

	/* note: 32-bit wide field */
    struct
	{
        /* which shift key or key combo affects Char3 of each KeyDef */
        BITFIELD    ShiftAlt    :1; /* use shift-alt instead of ctrl-alt */
        BITFIELD    AltGrafL    :1; /* use left alt key as alt-graphics */
        BITFIELD    AltGrafR    :1; /* use right alt key as alt-graphics */
        /* other modifiers */
        BITFIELD    ShiftLock   :1; /* treat caps lock as shift lock */
        BITFIELD    DefaultTable:1; /* default table for the language */
        BITFIELD    ShiftToggle :1; /* TRUE: toggle, FALSE: latch shiftlock */
        BITFIELD    AccentPass  :1; /* TRUE: pass on accent keys and beep,
                                    FALSE: just beep */
        BITFIELD    CapsShift   :1; /* caps-shift uses Char5 */
        BITFIELD    MachDep     :1; /* machine-dependent table */
    } XTableFlags1;

    WORD    KbdType;            /* keyboard type */
    WORD    KbdSubType;         /* keyboard sub-type */
    WORD    XtableLen;          /* length of table */
    WORD    EntryCount;         /* number of KeyDef entries */
    WORD    EntryWidth;         /* width in bytes of KeyDef entries */
    BYTE    Country[2];         /* country ID, i.e. 'US' */
    WORD    TableTypeID;        /* Table type, 0001=OS/2 */
    BYTE    SubCountryID[4];    /* sub-country ID, ASCII, i.e. '153 ' */
    WORD    Reserved[8];
} XHeader;

/* Key definition, one per scan code in table */
typedef struct
{
    WORD    XlateOp;
    #define     KeyType     0x01ff  /* low 9 bits */
    #define     AccentFlags 0xfe00  /* high 7 bits */

    BYTE    Char1;
    BYTE    Char2;
    BYTE    Char3;
    BYTE    Char4;
    BYTE    Char5;
} KeyDef;

typedef struct
{
    WORD    word1;
    BYTE    Country[2];     /* i.e. 'US' */
    BYTE    SubCountryID[4];    /* i.e. '153 ' */
    WORD    word2;
    WORD    XTableID;       /* i.e. 0x1b5 (437) */
    WORD    word3;
    ULONG   HeaderLocation; /* of beginning of table (header) */
} IndexEntry;

#define MAX_SWAPPED 40

#define LIST_BASIC 1
#define LIST_HEADERS 2
#define LIST_KEYS 3

/* shift flags */
#define SF_RIGHTSHIFT               0x0001
#define SF_LEFTSHIFT                0x0002
#define SF_CONTROL                  0x0004
#define SF_ALT                      0x0008
#define SF_SCROLLLOCK_ON            0x0010
#define SF_NUMLOCK_ON               0x0020
#define SF_CAPSLOCK_ON              0x0040
#define SF_INSERT_ON                0x0080
#define SF_LEFTCONTROL              0x0100
#define SF_LEFTALT                  0x0200
#define SF_RIGHTCONTROL             0x0400
#define SF_RIGHTALT                 0x0800
#define SF_SCROLLLOCK               0x1000
#define SF_NUMLOCK                  0x2000
#define SF_CAPSLOCK                 0x4000
#define SF_SYSREQ                   0x8000
