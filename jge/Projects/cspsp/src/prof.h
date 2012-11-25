#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define	GMON_PROF_ON	0
#define	GMON_PROF_BUSY	1
#define	GMON_PROF_ERROR	2
#define	GMON_PROF_OFF	3

#define GMONVERSION	0x00051879

#include <pspthreadman.h>

/** gmon.out file header */
struct gmonhdr
{
        char cookie[4];
        char version[4];
        char spare[3*4];
};

struct gmonhisthdr
{
        int lpc;        /* lowest pc address */
        int hpc;        /* highest pc address */
        int ncnt;       /* size of samples + size of header */
        int profrate;   /* profiling clock rate */
        char dimension[15];
        char abbrev;
};

/** frompc -> selfpc graph */
struct rawarc
{
        unsigned int frompc;
        unsigned int selfpc;
        unsigned int count;
};

/** context */
struct gmonparam
{
        int state;
        unsigned int lowpc;
        unsigned int highpc;
        unsigned int textsize;
        unsigned int hashfraction;

        int narcs;
        struct rawarc *arcs;

        int nsamples;
        unsigned short int *samples;

        int timer;

        unsigned int pc;
};

/// holds context statistics
static struct gmonparam gp;

/// one histogram per four bytes of text space
#define	HISTFRACTION	4

/// define sample frequency - 1000 hz = 1ms
#define SAMPLE_FREQ     1000

/// have we allocated memory and registered already
static int initialized = 0;

/// defined by linker
extern int _ftext;
extern int _etext;

/* forward declarations */
void gprof_cleanup();
static SceUInt timer_handler(SceUID uid, SceKernelSysClock *c1, SceKernelSysClock *c2, void *common);