/**
 * @file
 *          ST7775_Driver.h
 * @brief
 *          This File contain API for use ST7775 Driver.
 *
 * @par Developer:
 *          - Francisco Roman
 * @par Co-Developer:
 * 			-Fernando Quinones
 * @par Project or Platform:
 *          - OPHYRA
 * @par SW-Component:
 *          - ST7775 Driver
 * @par SW-Package:
 *          - API
 * @note
 *          - This library was written originally for TFT 1.8in 160 * 128
 *
 * @par Description:
 *          This API contain the main functions to use a TFT with the driver
 *          ST7775.
 *
 * @par Copyright Notice:
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
#ifndef ST7735_DRIVER_H_
#define ST7735_DRIVER_H_

/* Language linkage */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/******************************************************************************
 INCLUDES
 *****************************************************************************/
#include "Intesc_Stdr_Types.h"
/******************************************************************************
 CONSTANTS
 *****************************************************************************/
#define ST7735_nX_AXE_ORIGIN  0x00U       /*!< X axe origin */
#define ST7735_nY_AXE_ORIGIN  0x00U       /*!< Y axe origin */
#define ST7735_nTFTWIDTH        160
#define ST7735_nTFTLENGTH       128

#define Transparent			  0x000001
#define ST7735_nWHITE         0xFFFFFFUL  /*!< Definition of color white */
#define ST7735_nBLACK         0x000000UL  /*!< Definition of color black */
#define ST7735_nRED           0x0000FFUL  /*!< Definition of color red */
#define ST7735_nGREEN         0x00FF00UL  /*!< Definition of color green */
#define ST7735_nBLUE          0xFF0000UL  /*!< Definition of color blue */
#define TFT_ClearDisplay	  ST7735_enFillDisplay

/******************************************************************************
 TYPEDEFS
 *****************************************************************************/

typedef enum
{
    ST7735_nenSuccess  = 0x00U,     /**< Action Success        */
    ST7735_nenError,                /**< System error          */
    ST7735_nenInvalidPrmtrs         /**< Invalid Parameters    */

} ST7735_tenErrCode;

typedef struct
{
    U8    u8XCursor;               /**< Coordinate in X axe        */
    U8    u8Ycursor;               /**< Coordinate in Y axe        */
    U32   u32StrColour;            /**< String Color               */
    U32	  u32StrBkColour;          /**< BackGround Color of string */
    CHAR *pchString;               /**< Pointer to string          */
}ST7735_tstStrDesc;

typedef struct
{
	U8	  u8X1Pos;					   /**< Pos. Initial Coordinate in X axe  */
	U8    u8Y1Pos;					   /**< Pos. Initial Coordinate in Y axe  */
	U8	  u8X2Pos;					   /**< Pos. Final Coordinate in X axe    */
	U8    u8Y2Pos;					   /**< Pos. Final Coordinate in Y axe    */
	U32   u32GeomColour;
}ST7735_stGeomDesc;

typedef struct ST7735_stIMGDescriptor
{
  U8 u8Xsize;                     /**< Size in pixels to X axe  */
  U8 u8Ysize;                     /**< Size in pixels to Y axe  */
  U8 u8XCursor;                   /**< Coordinate in X axe      */
  U8 u8Ycursor;                   /**< Coordinate in Y axe      */
  U8* pu8Img;                     /**< Pointer to image data    */
}ST7735_stImgDes;

typedef struct
{
	/**< Pointer to function with the capacity of write by SPI       */
    ST7735_tenErrCode (*SPI_WRITE)          (U8*, U32);
    /**< Pointer to function with the capacity of write a CS GPIO    */
    ST7735_tenErrCode (*TFT_CS_GPIO)        (U8);
    /**< Pointer to function with the capacity of write a RS GPIO    */
    ST7735_tenErrCode (*TFT_RS_GPIO)        (U8);
    /**< Pointer to function with the capacity of write a RESET GPIO */
    ST7735_tenErrCode (*TFT_RESET_GPIO)     (U8);
    /**< Pointer to function with the capacity of create a delay     */
    ST7735_tenErrCode (*DELAY_MS)           (U32);

}ST7735_tstBSPDriver;



/******************************************************************************
 VARIABLES
 *****************************************************************************/

/******************************************************************************
 MACROS
 *****************************************************************************/

/******************************************************************************
 PROTOTYPES
 *****************************************************************************/
/**************************************************************************//**
* \brief ST7735 initialization
* \param[in] Orientation: This is a integer. Set display orientation of screen.
* 											 0 = horizontal orientation.
* 											 1 = vertical orientation.
* \return Error code.
* \note   Is necessary register BSP before initializer this module
******************************************************************************/
ST7735_tenErrCode ST7735_enInit(U8 Orientation);

/**************************************************************************//**
* \brief  ST7735 Reset Chip by Hardware
* \return Error code.
******************************************************************************/
ST7735_tenErrCode ST7735_enReset(void);

/**************************************************************************//**
* \brief  This function subscribe the BSP functionalities necessaries to this
*          module can work correctly.
* \param[in] pstBSPDriver  This is a pointer structure to functions provided
*                          by the user
* \return Error code
******************************************************************************/
ST7735_tenErrCode ST7735_enRegisterBSP(ST7735_tstBSPDriver* pstBSPDriver);

/**************************************************************************//**
* \brief  This function print a image in BGR format of 6 bits by color,
*         you need sent 3 bytes by Pixel, just the six most significant bites
*         will be use by the module to print pixel.
* \param[in] pstImgdesc  This is a pointer structure to contain all information
*                        necessary to print a BMP image.
* \return Error code
******************************************************************************/
ST7735_tenErrCode ST7735_enDrawImg(ST7735_stImgDes* pstImgdesc);

/**************************************************************************//**
* \brief  This function fill the screen with a specific color.
* \param[in] u32Colour  This is a color to fill screen.
* \return Error code
******************************************************************************/
ST7735_tenErrCode ST7735_enFillDisplay(U32 u32Colour);

/**************************************************************************//**
* \brief  This function print a string in screen.
* \param[in] pstStrDesc  This is a pointer structure to contain all information
* 						 necessary to print a string char.
* \return Error code
******************************************************************************/
ST7735_tenErrCode ST7735_enPrintStr(ST7735_tstStrDesc* pstStrDesc);

/**************************************************************************//**
* \brief  This function print a line in screen.
* \param[in] pstGeomDesc  This is a pointer structure to contain all information
* 						 necessary to print a rect line.
* \return Error code
******************************************************************************/
ST7735_tenErrCode ST7735_enPrintLine(ST7735_stGeomDesc* pstGeomDesc);

/* Language linkage */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ST7735_DRIVER_H_ */
/******************************************************************************
 END OF FILE
 *****************************************************************************/
