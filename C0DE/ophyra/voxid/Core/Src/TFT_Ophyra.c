/****************************************************************************************
 * www.intesc.mx
 * Autor: Fernando Q.
 * Este documento realiza el mapeo de los pines de la pantalla TFT conectada a la tarjeta
 * Ophyra.
 *
 ****************************************************************************************
*/
#include "ST7735_Driver.h"
#include "stm32f4xx_hal.h"
#ifdef __TFT_BMP
#include "fatfs.h"
#include "sdio.h"
#endif
#include <string.h>
#include "TFT_Ophyra.h"
#include "main.h"

//#endif

/***************************************************************************************
 * Funciones de mapeo de los pines de conexion de la pantalla
 ***************************************************************************************/

ST7735_tenErrCode pfSPI_WRITE (U8* pu8Data, U32 u32Size)
{
	HAL_SPI_Transmit(&hspi1,(uint8_t *)pu8Data, u32Size, 0xFFFFFFFF);

	return ST7735_nenSuccess;
}
ST7735_tenErrCode pfTFT_CS_GPIO(U8 u8State)
{

	if (0 == u8State)
	{
		HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
	}
	else if (1 == u8State)
	{
	    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
	}
	return ST7735_nenSuccess;
}

ST7735_tenErrCode pfTFT_RS_GPIO(U8 u8State)
{
	if (0 == u8State)
	{
		HAL_GPIO_WritePin(TFT_RS_GPIO_Port, TFT_RS_Pin, GPIO_PIN_RESET);
	}
	else if (1 == u8State)
	{
	    HAL_GPIO_WritePin(TFT_RS_GPIO_Port, TFT_RS_Pin, GPIO_PIN_SET);
	}
	return ST7735_nenSuccess;
}

ST7735_tenErrCode pfTFT_RESET_GPIO(U8 u8state)
{
	if (0 == u8state)
	{
		HAL_GPIO_WritePin(TFT_Reset_GPIO_Port, TFT_Reset_Pin, GPIO_PIN_RESET);
	}
	else if (1 == u8state)
	{
	    HAL_GPIO_WritePin(TFT_Reset_GPIO_Port, TFT_Reset_Pin, GPIO_PIN_SET);
	}
	return ST7735_nenSuccess;
}
ST7735_tenErrCode pfTFT_BACKLIGHT_GPIO(U8 u8state){
	if( 0==u8state ){
		HAL_GPIO_WritePin(TFT_BckLight_GPIO_Port, TFT_BckLight_Pin, GPIO_PIN_RESET);
	}else if( 1== u8state ){
		HAL_GPIO_WritePin(TFT_BckLight_GPIO_Port, TFT_BckLight_Pin, GPIO_PIN_SET);
	}

	return ST7735_nenSuccess;
}

ST7735_tenErrCode pfDELAY_MS(U32 u32Delay)
{
	HAL_Delay(u32Delay);
	return ST7735_nenSuccess;
}

ST7735_tenErrCode TFT_Init(U8 Orientation){
	stOphyraBSP.DELAY_MS           = pfDELAY_MS;
	stOphyraBSP.SPI_WRITE          = pfSPI_WRITE;
	stOphyraBSP.TFT_CS_GPIO        = pfTFT_CS_GPIO;
	stOphyraBSP.TFT_RESET_GPIO     = pfTFT_RESET_GPIO;
	stOphyraBSP.TFT_RS_GPIO        = pfTFT_RS_GPIO;
	ST7735_enRegisterBSP(&stOphyraBSP);
	ST7735_enInit(Orientation);

	return ST7735_nenSuccess;
}

/***************************************************************************************
 * Funciones que utilizan la librería Driver_ST7735.a
 ***************************************************************************************/
ST7735_tenErrCode TFT_PrintStr(U8 XCursor, U8 YCursor, CHAR *pchString, U32 StrColour, U32 StrBkColour){
	stStrDesc.u8XCursor = XCursor;
	stStrDesc.u8Ycursor = YCursor;
	stStrDesc.pchString = pchString;
	stStrDesc.u32StrColour = StrColour;
	stStrDesc.u32StrBkColour = StrBkColour;
	ST7735_enPrintStr(&stStrDesc);

	return ST7735_nenSuccess;
}
ST7735_tenErrCode TFT_PrintLine(U8 u8X1Pos,U8 u8Y1Pos ,U8 u8X2Pos,U8 u8Y2Pos, U32 u32GeomColour){
	stGeomDesc.u8X1Pos = u8X1Pos;
	stGeomDesc.u8Y1Pos = u8Y1Pos;
	stGeomDesc.u8X2Pos = u8X2Pos;
	stGeomDesc.u8Y2Pos = u8Y2Pos;
	stGeomDesc.u32GeomColour = u32GeomColour;
	ST7735_enPrintLine(&stGeomDesc);

	return ST7735_nenSuccess;
}
#ifdef __TFT_BMP
ST7735_tenErrCode TFT_PrintImgBMP(U8 u8XCursor, U8 u8YCursor, TCHAR*uSD_FileName){
	FATFS fatfs;			/*Archivo del sistema para el driver (SD)*/
	FIL fil;				/*Estructura del objeto de archivo*/
	FRESULT fresult;		/*Retorno de ejecucion de funciones (Enum definidos) */
	UINT testBytes;			/*Variable entera de 32 bits sin signo*/

	//	uint32_t FileSize; Varible use to get the total size of the file
	U8 BMInfo_Buffer[54];
	U16 PointerImage = 0;
	U16 HeightImg, WideImg;
	//This function registers/unregisters a filesystem object
	fresult = f_mount(&fatfs, "", 0);
	if(fresult == FR_OK){
		//This function opens a file and sets the write/read pointer to zero
		fresult = f_open(&fil, uSD_FileName, FA_READ | FA_WRITE);
		if(fresult == FR_OK){ 			//checking if a file is open correctly
			fresult = f_lseek(&fil, 0); //Pointing to begin of file BMP.
			// Getting all the parameters of the file BMP.
			// Reads a firts 54 bytes of file and saves in BMInfo_Buffer.
			fresult = f_read(&fil, BMInfo_Buffer, sizeof(BMInfo_Buffer), &testBytes);
			//In this conditional it's verified the "BM" nature of the file
			if (BMInfo_Buffer[0] == 'B' && BMInfo_Buffer[1] == 'M'){
				//extracting size of image.
				HeightImg = (BMInfo_Buffer[19] << 8 | BMInfo_Buffer[18]);
				WideImg = (BMInfo_Buffer[23] << 8 | BMInfo_Buffer[22]);
				if(HeightImg<=160 && WideImg<=128){//checking size of image.
					if(BMInfo_Buffer[28]==8){ //checking the format codifying of the pixels
						/*Here two variables are created:
						 *Line_Buffer: store the 160 numbers, those bytes point to ColorMAP_Buffer.
						 *Image_Line_RGB: will store the 480 numbers, those are bytes information RGB to print in screen.*/
						uint8_t Line_Buffer[BMInfo_Buffer[18]];
						uint8_t Image_Line_RGB[BMInfo_Buffer[18]*3];
						U8 ColorMap_Buffer[1024];
						// Get the color map
						fresult = f_lseek(&fil, 54);
						fresult = f_read(&fil, ColorMap_Buffer, sizeof(ColorMap_Buffer), &testBytes);
						// This for cycle will run many times as the height of the image
						for (uint8_t j = 0; j<BMInfo_Buffer[22]; j++)
						{
							//This pointer it's use to get the information inside the Line buffer
							PointerImage = 0;
							//Get the line N
							fresult = f_lseek(&fil, 1078+(BMInfo_Buffer[18]*j));
							fresult = f_read(&fil, Line_Buffer, sizeof(Line_Buffer), &testBytes);
							//In this for cycle it's created the line tha will be printed in Ophyra's TFT---
							for(uint16_t i = 0; i < sizeof(Image_Line_RGB); i=i+3)
							{
								Image_Line_RGB[i+2] = ColorMap_Buffer[(Line_Buffer[PointerImage])*4];

								Image_Line_RGB[i+1] = ColorMap_Buffer[(Line_Buffer[PointerImage]*4)+1];

								Image_Line_RGB[i] = ColorMap_Buffer[(Line_Buffer[PointerImage]*4)+2];

								PointerImage++;
							}
							//-------------------------Prints the line N on Ophyra's TF-------------------
							pstImgdesc.pu8Img = Image_Line_RGB;
							pstImgdesc.u8Xsize = BMInfo_Buffer[18];
							pstImgdesc.u8Ysize = 1;
							pstImgdesc.u8XCursor = u8XCursor;
							pstImgdesc.u8Ycursor = u8YCursor + (BMInfo_Buffer[22] - j - 1);
							ST7735_enDrawImg(&pstImgdesc);
							//----------------------------------------------------------------------------
						}

					}else if(BMInfo_Buffer[28]==24){
						/*Here two variables are created:
						 *Line_Buffer: store the 160 numbers, those bytes point to ColorMAP_Buffer.*/
						uint8_t Image_Line_RGB[BMInfo_Buffer[18]*3];
						// This for cycle will run many times as the height of the image
						for (uint8_t j = 0; j<BMInfo_Buffer[22]; j++)
						{//Get the line N
							fresult = f_lseek(&fil, 54+(3*BMInfo_Buffer[18]*j));
							fresult = f_read(&fil, Image_Line_RGB, sizeof(Image_Line_RGB), &testBytes);
							//-------------------------Prints the line N on Ophyra's TF-------------------
							for(U16 i=0; i < sizeof(Image_Line_RGB); i=i+3)
							{
								U8 tempo = Image_Line_RGB[i];
								Image_Line_RGB[i] = Image_Line_RGB[i+2];
								Image_Line_RGB[i+2] = tempo;
							}
							pstImgdesc.pu8Img = Image_Line_RGB;
							pstImgdesc.u8Xsize = BMInfo_Buffer[18];
							pstImgdesc.u8Ysize = 1;
							pstImgdesc.u8XCursor = u8XCursor;
							pstImgdesc.u8Ycursor = u8YCursor + (BMInfo_Buffer[22] - j - 1);
							ST7735_enDrawImg(&pstImgdesc);
							//----------------------------------------------------------------------------
						}
					}else{
						TFT_PrintStr(0,0,"Error: only 8 or 24 bits is supported ", 0xFFFFFF, 0xFF0000);
					}
				}else{
					TFT_PrintStr(0,0,"Error: Image too large", 0xFFFFFF, 0xFF0000);
				}
			}else{
				TFT_PrintStr(0,0,"Error: Image not is bmp", 0xFFFFFF, 0xFF0000);
			}
		}else{
			TFT_PrintStr(0,0,"Error: Image no found", 0xFFFFFF, 0xFF0000);
		}
	}
	else{
		TFT_PrintStr(0,0,"Error: No SD mount", 0xFFFFFF, 0xFF0000);
	}
	if(fresult == FR_OK){
		fresult = f_close(&fil);//This function close an open file
	}
	return ST7735_nenSuccess;
}
#endif
