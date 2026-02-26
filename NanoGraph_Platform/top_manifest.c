/* ----------------------------------------------------------------------
 * top_manifest_computer.c
 * -------------------------------------------------------------------- */


#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../nanograph_interpreter.h"
#include "top_manifest.h"

/*------------------------------------------------------------------------------------
    ALL THE NODES OF ALL PROCESSORS, AND THEIR INDEX 
-------------------------------------------------------------------------------------*/
extern p_nanograph_node NanoGraph_null_task;          /*  0 */
extern p_nanograph_node NanoGraph_script;             /*  1  #define NanoGraph_script_index 1 */
//extern p_nanograph_node arm_nanograph_router;          /*  2 */
//extern p_nanograph_node arm_nanograph_modulator;       /*  3 */
//extern p_nanograph_node arm_nanograph_fixedbf;         /*  4 */
//extern p_nanograph_node arm_nanograph_filter2D;        /*  5 */
extern p_nanograph_node arm_nanograph_filter;            /*  6 */
//extern p_nanograph_node arm_nanograph_demodulator;     /*  7 */
                                                   /*  8 */
//extern p_nanograph_node arm_nanograph_amplifier;       /*  9 */
//extern p_nanograph_node sigp_kws;                   /* 10 */
//extern p_nanograph_node sigp_nanograph_compressor;     /* 11 */
//extern p_nanograph_node sigp_nanograph_decompressor;   /* 12 */
extern p_nanograph_node sigp_nanograph_detector;         /* 13 */
//extern p_nanograph_node sigp_nanograph_detector2D;     /* 14 */
//extern p_nanograph_node sigp_nanograph_resampler;      /* 15 */
//extern p_nanograph_node bitbank_JPEGENC;            /* 16 */
//extern p_nanograph_node TjpgDec;                    /* 17 */


const p_nanograph_node node_entry_points[NB_NODE_ENTRY_POINTS] =
{
    /*  0 */ (p_nanograph_node)&NanoGraph_null_task,       /* ID | PROC ARCH    */
    /*  1 */ (p_nanograph_node)&NanoGraph_script,          /*  1 |   0   0      byte-code interpreter, index "NanoGraph_script_INDEX" */
    /*  2 */ 0,                          /*  2 |   0   1      copy input arcs and subchannel and output arcs and subchannels   */
    /*  3 */ 0,                          /*  3 |   0   1      signal generator with modulation */
    /*  4 */ 0,                          /*  4 |   0   1        */
    /*  5 */ 0,                          /*  5 |   0   1      2D processing on the HP processor PROC_ID=2 */
    /*  6 */ (p_nanograph_node)&arm_nanograph_filter,         /*  6 |   0   1      cascade of DF1 filters */
    /*  7 */ 0,                          /*  7 |   0   1      signal demodulator, frequency estimator */
    /*  8 */ 0,                          /*  8 |   0   1        */
    /*  9 */ 0,                          /*  9 |   0   1      amplifier mute and un-mute with ramp and delay control */
    /* 10 */ 0,                          /* 10 |   0   1      YES/NO KWS */
    /* 11 */ 0,                          /* 11 |   0   1      raw data compression with adaptive prediction */
    /* 12 */ 0,                          /* 12 |   0   1      raw data decompression */
    /* 13 */ (p_nanograph_node)&sigp_nanograph_detector,      /* 13 |   0   1      estimates peaks/floor of the mono input and triggers a flag on high SNR */
    /* 14 */ 0,                          /* 14 |   0   1      2D processing on the HP processor PROC_ID=2 */
    /* 15 */ 0,                          /* 15 |   0   1      asynchronous sample-rate converter */
    /* 16 */ 0,                          /* 16 |   0   1      bitbank_JPEGENC */
    /* 17 */ 0,                          /* 17 |   0   1      TjpgDec */
};


/* 
    IO interfaces functions to the possible streams of the platform
*/
extern void motion_in_0   (uint32_t, nanograph_xdmbuffer_t *);  // 3
extern void gpio_out_0    (uint32_t, nanograph_xdmbuffer_t *);  // 7

/*  
    global read-only index to the current graph interpreter instance 
*/
uint8_t platform_io_instance_idx;
nanograph_instance_t* platform_io_callback_parameter;
extern uint64_t all_ptr_instances[NANOGRAPH_NB_INSTANCE];


/* ------------------------------------------------
       Memory map of the platform
       see top_manifest_computer.txt
  ------------------------------------------------ */
uint8_t MEXT[SIZE_MBANK_DMEM_EXT];  
uint8_t DTCM[SIZE_MBANK_DTCM];
uint8_t ITCM[SIZE_MBANK_ITCM];
uint8_t BACKUP[SIZE_MBANK_RETENTION];

const uint8_t* long_offset[MAX_PROC_MEMBANK] =
{ &(MEXT[0]), &(DTCM[0]), &(ITCM[0]), &(BACKUP[0]) };


/*---------------------------------------------------------
  IO AFFINITY WITH PROCESSOR  
  ---------------------------------------------------------*/
#define LAST_IO_FUNCTION_PLATFORM (IO_PLATFORM_DATA_OUT_0+1)  /* table of platform_io[io_al_idx] */

const p_io_function_ctrl platform_io[] =
{   0             ,     // 0
    0             ,     // 1
    0             ,     // 2
    motion_in_0   ,     // 3
    0             ,     // 4   
    0             ,     // 5  reserved to an other processor
    0             ,     // 6   
    gpio_out_0    ,     // 7
    0             ,     // 8
    0             ,     // 9   
};          


/*
    Callback for NODE and scripts (SYSCALL) 
        system call (0..7) 
        application callbacks (8..15))
        ARC debug activities (ARC_APP_CALLBACK1)
    Use-case :
        deep-sleep proposal from the scheduler
        event detection trigger, software timers
        metadata sharing from script
        allow execution of nano_graph_interpreter(NANOGRAPH_RESET..) after a graph reload
*/
const p_nanograph_services_t application_callbacks[MAX_NB_APP_CALLBACKS] =
{   (void*)0,
    (void*)0,
    (void*)0,
    (void*)0,
    (void*)0,
    (void*)0,
    (void*)0,
    (void*)0
};

/*==========================================================================================================================*/
/*
*   the graph to be executed
*/
const uint32_t graph_computer[] =
{
    #include "graphs/graph_computer_filter_detector_bin.txt"
};



/*
 *  TIME (see nanograph_time64 definition)
 *
 *  "nanograph_time64" example of implementation using a global variable
 *  FEDCBA987654321 FEDCBA987654321 FEDCBA987654321 FEDCBA9876543210
 *  ____ssssssssssssssssssssssssssssssssqqqqqqqqqqqqqqqqqqqqqqqqqqqq q32.28 [s]  140 Y + Q28 [s]
 *  systick increment for  1ms =  0x00041893 =  1ms x 2^28
 *  systick increment for 10ms =  0x0028F5C2 = 10ms x 2^28
 *
 * Other implementation rely on an HW timer (RP2040)
*/

uint64_t global_nanograph_time64;


/*
    Table of pairs {node + parameter address} for parameter updates from the application
*/
uintptr_t new_node_parameters[(1 + MAX_NB_PENDING_PARAM_UPDATES) * 2] =
{   0, 0,  // [node idx in the graph ; physical address to the parameters in "boot" format]
    0, 0,  // [node idx; parameter address]
    // .. 
    0, 0,  // end of the list 
};



/*==========================================================================================================================*/

uint32_t* get_graph_address(uint32_t graph_idx)
{
    return (uint32_t*)graph_computer;
}

/*==========================================================================================================================*/
/**
  @brief            Shares the pl
  atform-specific data for the initialization of the interpreter instance
  @param[in/out]    none
  @return           int
 */

void platform_init_specific(NanoGraph_init_t* data)
{
#ifdef GRAPH_FROM_PLATFORM
    data->graph = get_graph_address(0);
#endif

    data->long_offset = (uint8_t**)long_offset;           // pointer to "long_offset[MAX_PROC_MEMBANK]"

    data->node_entry_points = (p_nanograph_node)node_entry_points;             // list of nodes
    data->platform_io = (p_io_function_ctrl)platform_io;                     // list of IO functions
    data->new_parameters = (uintptr_t)new_node_parameters;                   // list of pairs [offset; parameter address]

    data->procID = PLATFORM_PROCESSOR;
    data->archID = PLATFORM_ARCHITECTURE;
}


/*==========================================================================================================================*/
