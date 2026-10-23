/*www.intesc.mx
 * Autor: Fernando Q.
 * Created on: 07/12/2017
 */
#ifndef TFT_OPHYRA_H_
#define TFT_OPHYRA_H_
// DESCOMENTE LA SIGUIENTE LÍNEA SI SE USARÁ LA SD Y EL DESPLIEGUE DE IMAGENES.
// #define __TFT_BMP

 /*
 * Este archivo realiza el mapeo de los pines de la pantalla TFT conectada a la tarjeta Ophyra.
 *
 * ********************* Instrucciones para incluir la libreria en el proyecto ***************************
 * EN EL PROYECTO:
 * 	Incluir en la carpeta "Inc" los archivos:
 * 			TFT_Ophyra.h ; ST7735_Driver.h ; Intesc_Stdr_Types.h
 *
 * 	Incluir en la carpeta "Src" los archivos:
 * 			TFT_Ophyra.c ; libST7735_Driver.a
 *
 * DENTRO DE "LAS PROPIEDADES (Properties)" DEL PROYECTO AGREGAR LO SIGUIENTE:
 *
 * C/C++Build->
 * 		Settings->
 * 			(pestaña)Tool Settings->
 * 								Libraries->
 * 								 	(ventana) Libraries->
 * 											:libST7735_Driver.a
 *  C/C++Build->
 * 		Settings->
 * 			(pestaña)Tool Settings->
 * 								Libraries->
 * 									(ventana) Library search path->
 *  										"${workspace_loc:/${ProjName}/Src}"
 * COPIAR Y PEGAR LAS LÍNEAS QUE A CONTINUACIÓN SE MUESTRAN,
 * DENTRO DE LA FUNCION PRINCIPAL "main.c" DEL PROYECTO.
 *
	#include "Intesc_Stdr_Types.h"
	#include "ST7735_Driver.h"
	#include "TFT_Ophyra.h"
 *
 */

/* Language linkage */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include "Intesc_Stdr_Types.h"
#include "ST7735_Driver.h"
#include "stm32f4xx_hal.h"
#ifdef __TFT_BMP
#include "fatfs.h"
#endif
#include <string.h>
#define TFT_BackLight 		  pfTFT_BACKLIGHT_GPIO

extern SPI_HandleTypeDef hspi1;
ST7735_tenErrCode 	pfSPI_WRITE (U8* pu8Data, U32 u32Size);
ST7735_tenErrCode 	pfTFT_CS_GPIO(U8 u8State);
ST7735_tenErrCode 	pfTFT_RS_GPIO(U8 u8State);
ST7735_tenErrCode 	pfTFT_RESET_GPIO(U8 u8state);
ST7735_tenErrCode 	pfTFT_BACKLIGHT_GPIO(U8 u8state);
ST7735_tenErrCode 	pfDELAY_MS(U32 u32Delay);


/******************************************************************************
* \brief  initialization of TFT screen of Ophyra
* \param[in] Orientation: This is a integer. Set display orientation of screen.
* 											 0 = horizontal orientation.
* 											 1 = vertical orientation.
* \return Error code.
* \note   Is necessary register BSP before initializer this module
********************************************************************************/
ST7735_tenErrCode 	TFT_Init(U8 Orientation);

/******************************************************************************
*\brief:  This function print a string into screen.
*\param[in]:
*\		   XCursor, YCursor: Pointers to indicate position of cursor in the screen.
*		    	  pchString: This is a pointer to contain a string vector.
*                 StrColour: Set colour font; Format: U32 (0x000000)
*               StrBKColour: Set the colour back side of font; Format: U32 (0x000000)
*           		         if parameter is NULL, then back side is transparent.
*\return Error code
******************************************************************************/
ST7735_tenErrCode 	TFT_PrintStr(U8 XCursor, U8 YCursor, CHAR *pchString, U32 StrColour, U32 StrBkColour);

/******************************************************************************
*\brief:  This function print a rect line into screen.
*\param[in] u8X1Pos, u8Y1Pos: Initial pos. coordenates.
*           u8X2Pos, u8Y2Pos: Final pos. coordenates.
*              u32GeomColour: Set the colour of geometry.
*\return Error code
******************************************************************************/
ST7735_tenErrCode TFT_PrintLine(U8 u8X1Pos,U8 u8Y1Pos ,U8 u8X2Pos,U8 u8Y2Pos, U32 u32GeomColour);
#ifdef __TFT_BMP
/******************************************************************************
*\brief:  This function print a image BMP in a screen, reading from Ophyra's SD-Card.
*\param[in]  u8XPos, u8YPos: Initial pos. coordenates.
*              uSD_FileName: Name of image to read.
*\return Error code
******************************************************************************/
ST7735_tenErrCode TFT_PrintImgBMP(U8 u8XCursor, U8 u8YCursor,CHAR*uSD_FileName);

/*
 * Definition of structures objects
 */
extern ST7735_stImgDes pstImgdesc;
#endif
extern ST7735_tstStrDesc stStrDesc;
extern ST7735_tstBSPDriver stOphyraBSP;
extern ST7735_stGeomDesc stGeomDesc;

#ifdef __cplusplus
}
#endif

#endif /* TFT_OPHYRA_H_ */
