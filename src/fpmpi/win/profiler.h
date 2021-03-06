#include <stdio.h>
#include <sys/types.h>

/* Global defs & variables */

/* The number of different message sizes to use */
#define NBINS 29

/* Maximum desired output line width */
#define MAX_LINE_WIDTH 120

/* typedefs */

//#ifndef HAVE_INT32_T
typedef int int32_t;  /* @int32@ determined by configure */
//#endif

/* In more recent MPI versions, C sized types (e.g., uint8_t) are available. */
#if SIZEOF_LONG_LONG >= 8 
typedef long long msgsize_t;
#define MSGSIZE_T MPI_LONG_LONG
#else
/* Use doubles for the message size if we don't have an 8-byte long long */
typedef double msgsize_t;
#define MSGSIZE_T MPI_DOUBLE
#endif

/* Prototypes for visible routines */
void fpmpiOutputPartnerTopology( int32_t [], FILE *, int );
void fpmpiOutputDetailedPartners( int32_t [], FILE *, int );
void fpmpiOutputDetailedDataVol( FILE *, int );

void fpmpi_getrange_int_single(int, int *, int *, int *, int *, int *);
void fpmpi_getrange_double_single(double, double *, int *, 
				  double *, int *, double *);
#if SIZEOF_LONG_LONG >= 8
void fpmpi_getrange_msgsize_single(msgsize_t, msgsize_t *, 
     				    int *, msgsize_t *, 
				    int *, msgsize_t *);
#else
#define fpmpi_getrange_msgsize_single fpmpi_getrange_double_single
#endif
void fpmpi_Output_TestWait( FILE *, int, int );
void fpmpi_Output_DupSplit( FILE *, int);

FILE *OutputOpenFile( int );
void OutputProgramDescription( FILE *, int, int );
void OutputResourceUsage( FILE *, int, int  );
void OutputCommunicationData( FILE *, int, int );
void OutputPartners( FILE *, int );
void OutputHeader( FILE *, int );
void OutputTail( FILE *, int );
void fpmpiInitPerformanceCounters( void );
void fpmpiOutputPerformanceCounters( FILE * , int, int );


/*
 * The following structure contains the information that we give
 * in a "general" report about the resource usage by a program.
 */
/* Bits for the fields in ProgramResourceUsage */
#define FPMPI_PRU_USER_TIME 0x1
#define FPMPI_PRU_SYS_TIME  0x2
#define FPMPI_PRU_MINOR_PAGE_FAULTS 0x4
#define FPMPI_PRU_MAJOR_PAGE_FAULTS 0x8
#define FPMPI_PRU_RSS 0x10
typedef struct {
  double   userTime, systemTime;
  unsigned int minorPageFaults,
               majorPageFaults,
               residentSetSize;
  unsigned int validFields;           /* bits are set for each valid field
					 above */
} ProgramResourceUsage;
int fpmpi_GetResourceUsage( ProgramResourceUsage *, int );


/* We also define a structure that may be used for routines that
   do not communicate data (they may include delays, so we include a 
   synctime field) */
typedef struct {
    double time, synctime;
    int    calls;
} NoCommData;

typedef struct {
    const char *name;
    int        hasSyncTime;
    NoCommData *data;
    double      totalTime, totalSync;
    int         totalCalls;
} CallBasicData;
void fpmpi_TestWaitInit( CallBasicData [] );
