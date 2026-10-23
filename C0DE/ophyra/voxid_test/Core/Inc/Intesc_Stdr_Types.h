/**
 * @file
 *          Intesc_Stdr_Types.h
 * @brief
 *          Data Conditioner System Types.
 *
 * @par Developer:
 *          - INTESC
 * @par Project or Platform:
 *          Intesc Standar Types.
 * @par SW-Component:
 *          HouseKeep
 * @par SW-Package:
 *          Software Types
 *
 * @par Description:
 *       This is an API that contain all types used in DCS module.
 *
 * @note While porting the API please consider the following:
 *       By default it is defined as 32 bit machine configuration
 *       define your data types based on your machine/compiler/
 *       controller configuration.
 *
 * @par Copyright Notice:
 *
 * @verbatim
 * Copyright (C) 2017 INTESC
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * @endverbatim
 **/

/******************************************************************************
 PREVENT REDUNDANT INCLUSION
 *****************************************************************************/
#ifndef INTESC_STDR_TYPES_H_
#define INTESC_STDR_TYPES_H_

/* Language linkage */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/******************************************************************************
 INCLUDES
 *****************************************************************************/

/******************************************************************************
 CONSTANTS
 *****************************************************************************/

/******************************************************************************
 TYPEDEFS
 *****************************************************************************/

/**< used for bolean     */
typedef unsigned char BOOLEAN;

/**< used for characters */
typedef char CHAR;

/**< used for unsigned 8bit */
typedef unsigned char U8;

/**< used for unsigned 16bit */
typedef unsigned short U16;

/**< used for unsigned 32bit */
typedef unsigned long U32;

/**< used for unsigned U64 */
typedef unsigned long long U64;

/**< used for signed 8bit */
typedef signed char S8;

/**< used for signed 16bit */
typedef signed short S16;

/**< used for signed 32bit */
typedef signed long S32;

/**< used for signed 64bit */
typedef signed long long S64;

/**< used for float of 32bit */
typedef float F32;

/**< used for float of 64bit */
typedef double F64;

/******************************************************************************
 MACROS
 *****************************************************************************/
/**< attribute FALSE */
#ifndef FALSE
#define FALSE           (0U)
#endif

/**< attribute TRUE */
#ifndef TRUE
#define TRUE            (1U)
#endif

/**< NULL pointer */
#ifndef NULL
#define NULL    ((void*)(0))
#endif
/******************************************************************************
 PROTOTYPES
 *****************************************************************************/

/* Language linkage */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* INTESC_STDR_TYPES_H_ */
/******************************************************************************
 END OF FILE
 *****************************************************************************/
