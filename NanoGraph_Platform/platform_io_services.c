/* ----------------------------------------------------------------------
 * Project:      NanoGraph
 * Title:        platform_computer_io_services.c
 * Description:  abstraction layer to BSP and streams from the application
 *
 * $Date:        15 February 2023
 * $Revision:    V0.0.1
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2010-2023 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
* 
 */

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#define DATA_FROM_FILES 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../nanograph_common.h"
#include "../nanograph_interpreter.h"
#include "../top_manifest_included.h"
#ifdef __cplusplus
 extern "C" {
#endif

/*-----------------------------------------------------------------------*/
extern uint8_t one_file_is_closed;

extern void NanoGraph_io_ack (uint8_t HW_io_idx, void *data, uintptr_t size);

/*
 * NULL TASK
 */
void NanoGraph_null_task (int32_t c, nanograph_handle_t i, void *d, uint32_t *s)  {}



/* Local IO functions */
extern void data_sink      (uint32_t command, nanograph_xdmbuffer_t* data);  
extern void motion_in_0    (uint32_t command, nanograph_xdmbuffer_t* data);  
extern void gpio_out_0     (uint32_t command, nanograph_xdmbuffer_t* data);  

/*
 * NULL TASK
 */
void arm_stream_null_task (int32_t c, nanograph_handle_t i, void *d, uint32_t *s)  {}


/* --------------------------------------------------------------------------------------- 
    FW IO FUNCTIONS

    const p_io_function_ctrl platform_io [LAST_IO_FUNCTION_PLATFORM] =
    {
        (void *)&data_sink,        // 0
        (void *)&data_in_1,        // 1
        (void *)&sensor_0,         // 2
        (void *)&motion_in_0,      // 3
        (void *)&audio_in_0,       // 4
        (void *)&d2_in_0,          // 5
        (void *)&line_out_0,       // 6
        (void *)&gpio_out_0,       // 7
        (void *)&gpio_out_1,       // 8
        (void *)&data_out_0,       // 9
    };
*/



#define size_motion_in_0 32                                                 // IO_AL_idx = 3
// static int16_t buffer_motion_in_0[size_motion_in_0/ sizeof(int16_t)];       // 16 samples

#define size_gpio_out_0 4                                                   // IO_AL_idx = 7
// static uint32_t buffer_gpio_out_0[size_gpio_out_0/ sizeof(int32_t)];        // 1 sample


/* 
    multichannel tests          IO_AL_idx = 0
*/
void data_sink(uint32_t command, nanograph_xdmbuffer_t* data) {   }


/*          IO_AL_idx = 3           */
void motion_in_0 (uint32_t command, nanograph_xdmbuffer_t *data) 
{
    switch (command)
    {
    case NANOGRAPH_RESET:
        //nanograph_format_io_setting = *(uint32_t *)(data->address);
        break;
    case NANOGRAPH_SET_PARAMETER:  /* presets reloaded */
        break;
    case NANOGRAPH_SET_BUFFER:     /* if memory allocation is made in the graph */
        break;
    case NANOGRAPH_RUN:            /* data moves */
    {   nanograph_xdmbuffer_t* pt_pt = (nanograph_xdmbuffer_t*)data;
        if (pt_pt->size >  size_motion_in_0)
        {   pt_pt->size = size_motion_in_0;
        }
        NanoGraph_io_ack(IO_PLATFORM_MOTION_IN_0, (void *)(pt_pt->address), pt_pt->size);
        break;
    }
    case NANOGRAPH_STOP:           /* stop data moves */
        break;
    case NANOGRAPH_READ_PARAMETER: /* setting done ? device is ready ? calibrated ? */
        break;
    default:
        break;
    }
}

/*
*            IO_AL_idx = 7
*/
void gpio_out_0(uint32_t command, nanograph_xdmbuffer_t* data)
{
    nanograph_xdmbuffer_t pt_pt;
    switch (command)
    {
    case NANOGRAPH_RESET:
        //nanograph_format_io_setting = *(uint32_t *)(data->address);
        break;        
    case NANOGRAPH_SET_PARAMETER:  /* presets reloaded */
        break;
    case NANOGRAPH_SET_BUFFER:     /* if memory allocation is made in the graph */
        break;
    case NANOGRAPH_RUN:            /* data moves */
    {   
        pt_pt.address = data->address;
        if (data->size > size_gpio_out_0)
        {   pt_pt.size = size_gpio_out_0;
        }
        NanoGraph_io_ack (IO_PLATFORM_GPIO_OUT_0, (uint8_t *)pt_pt.address, pt_pt.size);
        break;
     }
    case NANOGRAPH_STOP:           /* stop data moves */
        break;
    case NANOGRAPH_READ_PARAMETER: /* setting done ? device is ready ? calibrated ? */
        break;
    default:
        break;
    }
}



/*
 * -----------------------------------------------------------------------
 */
