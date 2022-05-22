/*****************************************************************************
* | File      	:	LCD_Touch.c
* | Author      :   Waveshare team
* | Function    :	LCD Touch Pad Driver and Draw
* | Info        :
*   Image scanning
*      Please use progressive scanning to generate images or fonts
*----------------
* |	This version:   V1.0
* | Date        :   2017-08-16
* | Info        :   Basic version
*
******************************************************************************/
#include <math.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/float.h"

#include "FatFS/sd_driver/spi.h"
//#include "../sdspi_config.h"
#include "Adafruit_SPITFT.h" // for color info
#include "Touch.h"

void spi1_dma_isr();

static spi_t spitouch = {
    .hw_inst = spi1,  // SPI component
    .miso_gpio = 12, // GPIO number (not pin number)
    .mosi_gpio = 11,
    .sck_gpio = 10,
    .baud_rate = 3 * 1000 * 1000,  // The limitation here is SPI slew rate.        

    .dma_isr = spi1_dma_isr
};

void spi1_dma_isr() { spi_irq_handler(&spitouch); }

//extern uint8_t id;
//#define WIDTH 240
//#define HEIGHT 320
uint8_t cTouch::SPI4W_Write_Byte(uint8_t value)
{
    uint8_t rxDat;
    spi_write_read_blocking(spi1, &value, &rxDat, 1);
    return rxDat;
}

uint8_t cTouch::SPI4W_Read_Byte(uint8_t value)
{
    return SPI4W_Write_Byte(value);
}

/*******************************************************************************
function:
		Read the ADC of the channel
parameter:
	Channel_Cmd :	0x90: Read channel Y +, select the ADC resolution is 12 bits, set to differential mode
					0xd0: Read channel x +, select the ADC resolution is 12 bits, set to differential mode
*******************************************************************************/
uint16_t cTouch::Read_ADC(uint8_t CMD)
{
    uint16_t Data = 0;
    uint8_t dat1 = 0;
    uint8_t dat2 = 0;
    uint8_t fillchr = 0;
    //A cycle of at least 400ns.
    gpio_put(LCD_CS_PIN, 1);
    gpio_put(SD_CS_PIN, 1);
    gpio_put(TP_CS_PIN,0);

//    spi_transfer(&spitouch, &CMD, NULL, 1);
    SPI4W_Write_Byte(CMD);
    sleep_us(200);

    //	dont write 0xff, it will block xpt2046  
    //Data = SPI4W_Read_Byte(0Xff);
//    spi_transfer(&spitouch, &fillchr, &dat1, 1);
//    spi_transfer(&spitouch, &fillchr, &dat2, 1);

    Data = SPI4W_Read_Byte(0X00);
    Data = (Data << 8);// | dat2;//7bit
    Data |= SPI4W_Read_Byte(0X00);
    //Data = SPI4W_Read_Byte(0Xff);
    Data >>= 3;//5bit
    gpio_put(TP_CS_PIN,1);
//    printf("Data: %i\r\n", Data);
    return Data;
}

/*******************************************************************************
function:
		Read the 5th channel value and exclude the maximum and minimum returns the average
parameter:
	Channel_Cmd :	0x90 :Read channel Y +
					0xd0 :Read channel x +
*******************************************************************************/
#define READ_TIMES  5	//Number of readings
#define LOST_NUM    1	//Discard value
uint16_t cTouch::Read_ADC_Average(uint8_t Channel_Cmd)
{
    uint8_t i, j;
    uint16_t Read_Buff[READ_TIMES];
    uint16_t Read_Sum = 0, Read_Temp = 0;
    //LCD SPI speed = 3 MHz
    spi_set_baudrate(spi1, SPI_TP_FREQ);
    //Read and save multiple samples
    for(i = 0; i < READ_TIMES; i++){
		Read_Buff[i] = Read_ADC(Channel_Cmd);
		sleep_us(200);
	}
    //LCD SPI speed = 18 MHz
    spi_set_baudrate(spi1,SPI_LCD_FREQ);
    //Sort from small to large
    for (i = 0; i < READ_TIMES  -  1; i ++) {
        for (j = i + 1; j < READ_TIMES; j ++) {
            if (Read_Buff[i] > Read_Buff[j]) {
                Read_Temp = Read_Buff[i];
                Read_Buff[i] = Read_Buff[j];
                Read_Buff[j] = Read_Temp;
            }
        }
    }

    //Exclude the largest and the smallest
    for (i = LOST_NUM; i < READ_TIMES - LOST_NUM; i ++)
        Read_Sum += Read_Buff[i];

    //Averaging
    Read_Temp = Read_Sum / (READ_TIMES - 2 * LOST_NUM);

    return Read_Temp;
}

/*******************************************************************************
function:
		Read X channel and Y channel AD value
parameter:
	Channel_Cmd :	0x90 :Read channel Y +
					0xd0 :Read channel x +
*******************************************************************************/
void cTouch::Read_ADC_XY(uint16_t *pXCh_Adc, uint16_t  *pYCh_Adc )
{
    *pXCh_Adc = Read_ADC_Average(0xD0);
    *pYCh_Adc = Read_ADC_Average(0x90);
}

/*******************************************************************************
function:
		2 times to read the touch screen IC, and the two can not exceed the deviation,
		ERR_RANGE, meet the conditions, then that the correct reading, otherwise the reading error.
parameter:
	Channel_Cmd :	pYCh_Adc = 0x90 :Read channel Y +
					pXCh_Adc = 0xd0 :Read channel x +
*******************************************************************************/
#define ERR_RANGE 50	//tolerance scope
bool cTouch::Read_TwiceADC(uint16_t *pXCh_Adc, uint16_t  *pYCh_Adc )
{
    uint16_t XCh_Adc1, YCh_Adc1, XCh_Adc2, YCh_Adc2;

    //Read the ADC values Read the ADC values twice
    Read_ADC_XY(&XCh_Adc1, &YCh_Adc1);
	sleep_us(10);
    Read_ADC_XY(&XCh_Adc2, &YCh_Adc2);
	sleep_us(10);
	
    //The ADC error used twice is greater than ERR_RANGE to take the average
    if( ((XCh_Adc2 <= XCh_Adc1 && XCh_Adc1 < XCh_Adc2 + ERR_RANGE) ||
         (XCh_Adc1 <= XCh_Adc2 && XCh_Adc2 < XCh_Adc1 + ERR_RANGE))
        && ((YCh_Adc2 <= YCh_Adc1 && YCh_Adc1 < YCh_Adc2 + ERR_RANGE) ||
            (YCh_Adc1 <= YCh_Adc2 && YCh_Adc2 < YCh_Adc1 + ERR_RANGE))) {
        *pXCh_Adc = (XCh_Adc1 + XCh_Adc2) / 2;
        *pYCh_Adc = (YCh_Adc1 + YCh_Adc2) / 2;
        return true;
    }

    //The ADC error used twice is less than ERR_RANGE returns failed
    return false;
}

/*******************************************************************************
function:
		Calculation
parameter:
		chCoordType:
					1 : calibration
					0 : relative position
*******************************************************************************/
uint8_t cTouch::Scan(bool chCoordType)
{
    //In X, Y coordinate measurement, IRQ is disabled and output is low
    if (!gpio_get(TP_IRQ_PIN)) {//Press the button to press
        //Read the physical coordinates
        if (chCoordType) {

                Read_TwiceADC(&sTP_DEV.Xpoint, &sTP_DEV.Ypoint);
        }
        else if (Read_TwiceADC(&sTP_DEV.Xpoint, &sTP_DEV.Ypoint))
        {
            if (tft->getRotation() % 2 == 1)
            {
                uint16_t tmp = sTP_DEV.Xpoint;
                sTP_DEV.Xpoint = sTP_DEV.Ypoint;
                sTP_DEV.Ypoint = tmp;
                sTP_Draw.Xpoint = sTP_DEV.fXfac * sTP_DEV.Xpoint - sTP_DEV.iXoff;
                sTP_Draw.Ypoint = tft->height() - (sTP_DEV.fYfac * sTP_DEV.Ypoint - sTP_DEV.iYoff);
            }
            else {
                sTP_Draw.Xpoint = tft->width() -
                    sTP_DEV.fXfac * sTP_DEV.Xpoint -
                    sTP_DEV.iXoff;
                sTP_Draw.Ypoint = tft->height() -
                    sTP_DEV.fYfac * sTP_DEV.Ypoint -
                    sTP_DEV.iYoff;
            }
        }
    
        if (0 == (sTP_DEV.chStatus & TP_PRESS_DOWN)) {	//Not being pressed
            sTP_DEV.chStatus = TP_PRESS_DOWN | TP_PRESSED;
            sTP_DEV.Xpoint0 = sTP_DEV.Xpoint;
            sTP_DEV.Ypoint0 = sTP_DEV.Ypoint;
        }
    } else {
        if (sTP_DEV.chStatus & TP_PRESS_DOWN) {	//0x80
            sTP_DEV.chStatus &= ~(1 << 7);		//0x00
        } else {
            sTP_DEV.Xpoint0 = 0;
            sTP_DEV.Ypoint0 = 0;
            sTP_DEV.Xpoint = 0xffff;
            sTP_DEV.Ypoint = 0xffff;
        }
    }

    return (sTP_DEV.chStatus & TP_PRESS_DOWN);
}

/*******************************************************************************
function:
		Draw Cross
parameter:
			Xpoint :	The x coordinate of the point
			Ypoint :	The y coordinate of the point
			Color  :	Set color
*******************************************************************************/
void cTouch::DrawCross(POINT Xpoint, POINT Ypoint, COLOR Color)
{	
    tft->drawFastHLine(Xpoint - 12, Ypoint,24, Color);
    tft->drawFastVLine(Xpoint, Ypoint - 12, 24, Color);
    tft->drawCircle(Xpoint, Ypoint, 6, Color);
    //GUI_DrawPoint(Xpoint, Ypoint, Color, DOT_PIXEL_2X2 , DOT_FILL_AROUND);
}


/*******************************************************************************
function:
		Touch screen adjust
*******************************************************************************/
void cTouch::Adjust(void)
{
    uint8_t  cnt = 0;
    uint16_t XYpoint_Arr[4][2];
    uint32_t Dx, Dy;
    uint16_t Sqrt1, Sqrt2;
    float Dsqrt;

    tft->fillRect(0, 0, tft->width(), tft->height(), WHITE);
    tft->setTextSize(1);
    tft->setCursor(0, 60);
    tft->setTextColor(RED);
    tft->print("Please use the stylus to\r\n"\
               "click the cross on the\r\nscreen.\r\n"\
               "The cross will always move\r\n"\
               "until the screen\r\nadjustment is completed."
                     );

    uint8_t Mar_Val = 12;
    DrawCross(Mar_Val, Mar_Val, RED);

    sTP_DEV.chStatus = 0;
    while (1) {
        Scan(1);
        if((sTP_DEV.chStatus & 0xC0) == TP_PRESSED) {
            sTP_DEV.chStatus &= ~(1 << 6);
            XYpoint_Arr[cnt][0] = sTP_DEV.Xpoint;
            XYpoint_Arr[cnt][1] = sTP_DEV.Ypoint;
			printf("X%d,Y%d = %d,%d\r\n",cnt,cnt,XYpoint_Arr[cnt][0],XYpoint_Arr[cnt][1]);
            cnt ++;
            sleep_ms(200);

            switch(cnt) {
            case 1:
                //DEBUG("not touch TP_IRQ 2 = %d\r\n", GET_TP_IRQ);
                DrawCross(Mar_Val, Mar_Val, WHITE);
                DrawCross(tft->width() - Mar_Val, Mar_Val, RED);
                sleep_ms(200);
                break;
            case 2:
                //DEBUG("not touch TP_IRQ 3 = %d\r\n", GET_TP_IRQ);
                DrawCross(tft->width() - Mar_Val, Mar_Val, WHITE);
                DrawCross(Mar_Val, tft->height() - Mar_Val, RED);
                sleep_ms(200);
                break;
            case 3:
                //DEBUG("not touch TP_IRQ 4 = %d\r\n", GET_TP_IRQ);
                DrawCross(Mar_Val, tft->height() - Mar_Val, WHITE);
                DrawCross(tft->width() - Mar_Val, tft->height() - Mar_Val, RED);
                sleep_ms(200);
                break;
            case 4:
                // 1.Compare the X direction
                Dx = abs((int16_t)(XYpoint_Arr[0][0] -
                                  XYpoint_Arr[1][0]));//x1 - x2
                Dy = abs((int16_t)(XYpoint_Arr[0][1] -
                                  XYpoint_Arr[1][1]));//y1 - y2
                Dx *= Dx;
                Dy *= Dy;
                Sqrt1 = sqrt(Dx + Dy);

                Dx = abs((int16_t)(XYpoint_Arr[2][0] -
                                  XYpoint_Arr[3][0]));//x3 - x4
                Dy = abs((int16_t)(XYpoint_Arr[2][1] -
                                  XYpoint_Arr[3][1]));//y3 - y4
                Dx *= Dx;
                Dy *= Dy;
                Sqrt2 = sqrt(Dx + Dy);

                Dsqrt = (float)Sqrt1 / Sqrt2;
                if(Dsqrt < 0.95 || Dsqrt > 1.05 || Sqrt1 == 0 || Sqrt2 == 0) {
                   //DEBUG("Adjust X direction \r\n");
                   cnt = 0;
                   //ShowInfo(XYpoint_Arr[0][0], XYpoint_Arr[0][1],
                   //            XYpoint_Arr[1][0], XYpoint_Arr[1][1],
                   //            XYpoint_Arr[2][0], XYpoint_Arr[2][1],
                   //            XYpoint_Arr[3][0], XYpoint_Arr[3][1],
                   //            Dsqrt * 100);
                   sleep_ms(1000);
                   DrawCross(tft->width() - Mar_Val, tft->height() - Mar_Val, WHITE);
                   DrawCross(Mar_Val, Mar_Val, RED);
                   continue;
                }

                // 2.Compare the Y direction
                Dx = abs((int16_t)(XYpoint_Arr[0][0] -
                                  XYpoint_Arr[2][0]));//x1 - x3
                Dy = abs((int16_t)(XYpoint_Arr[0][1] -
                                  XYpoint_Arr[2][1]));//y1 - y3
                Dx *= Dx;
                Dy *= Dy;
                Sqrt1 = sqrt(Dx + Dy);

                Dx = abs((int16_t)(XYpoint_Arr[1][0] -
                                  XYpoint_Arr[3][0]));//x2 - x4
                Dy = abs((int16_t)(XYpoint_Arr[1][1] -
                                  XYpoint_Arr[3][1]));//y2 - y4
                Dx *= Dx;
                Dy *= Dy;
                Sqrt2 = sqrt(Dx + Dy);//

                Dsqrt = (float)Sqrt1 / Sqrt2;
                if(Dsqrt < 0.95 || Dsqrt > 1.05) {
                   //DEBUG("Adjust Y direction \r\n");
                   cnt = 0;
                   //ShowInfo(XYpoint_Arr[0][0], XYpoint_Arr[0][1],
                   //            XYpoint_Arr[1][0], XYpoint_Arr[1][1],
                   //            XYpoint_Arr[2][0], XYpoint_Arr[2][1],
                   //            XYpoint_Arr[3][0], XYpoint_Arr[3][1],
                   //            Dsqrt * 100);
                   sleep_ms(1000);
                   DrawCross(tft->width() - Mar_Val, tft->height() - Mar_Val, WHITE);
                   DrawCross(Mar_Val, Mar_Val, RED);
                   continue;
                }//

                //3.Compare diagonal
                Dx = abs((int16_t)(XYpoint_Arr[1][0] -
                                  XYpoint_Arr[2][0]));//x1 - x3
                Dy = abs((int16_t)(XYpoint_Arr[1][1] -
                                  XYpoint_Arr[2][1]));//y1 - y3
                Dx *= Dx;
                Dy *= Dy;
                Sqrt1 = sqrt(Dx + Dy);//;

                Dx = abs((int16_t)(XYpoint_Arr[0][0] -
                                  XYpoint_Arr[3][0]));//x2 - x4
                Dy = abs((int16_t)(XYpoint_Arr[0][1] -
                                  XYpoint_Arr[3][1]));//y2 - y4
                Dx *= Dx;
                Dy *= Dy;
                Sqrt2 = sqrt(Dx + Dy);//

                Dsqrt = (float)Sqrt1 / Sqrt2;
                if(Dsqrt < 0.95 || Dsqrt > 1.05) {
                   printf("Adjust diagonal direction\r\n");
                   cnt = 0;
                   //ShowInfo(XYpoint_Arr[0][0], XYpoint_Arr[0][1],
                   //            XYpoint_Arr[1][0], XYpoint_Arr[1][1],
                   //            XYpoint_Arr[2][0], XYpoint_Arr[2][1],
                   //            XYpoint_Arr[3][0], XYpoint_Arr[3][1],
                   //            Dsqrt * 100);
                   sleep_ms(1000);
                   DrawCross(tft->width() - Mar_Val, tft->height() - Mar_Val, WHITE);
                   DrawCross(Mar_Val, Mar_Val, RED);
                   continue;
                }

                //4.Get the scale factor and offset
                //Get the scanning direction of the touch screen
//                sTP_DEV.TP_Scan_Dir = sLCD_DIS.LCD_Scan_Dir;
                sTP_DEV.fXfac = 0;

                //According to the display direction to get
                //the corresponding scale factor and offset
                if(tft->getRotation() == R2L_D2U) {
                    printf("R2L_D2U\r\n");

                   sTP_DEV.fXfac = (float)(tft->width() - 2 * Mar_Val) /
                                    (int16_t)(XYpoint_Arr[1][0] -
                                              XYpoint_Arr[0][0]);
                    sTP_DEV.fYfac = (float)(tft->height() - 2 * Mar_Val) /
                                    (int16_t)(XYpoint_Arr[2][1] -
                                              XYpoint_Arr[0][1]);

                    sTP_DEV.iXoff = (tft->width() -
                                     sTP_DEV.fXfac * (XYpoint_Arr[1][0] +
                                                      XYpoint_Arr[0][0])
                                    ) / 2;
                    sTP_DEV.iYoff = (tft->height() -
                                     sTP_DEV.fYfac * (XYpoint_Arr[2][1] +
                                                      XYpoint_Arr[0][1])
                                    ) / 2;

                } else if(tft->getRotation() == L2R_U2D) {
                    printf("L2R_U2D\r\n");

                    sTP_DEV.fXfac = (float)(tft->width() - 2 * Mar_Val) /
                                    (int16_t)(XYpoint_Arr[0][0] -
                                              XYpoint_Arr[1][0]);
                    sTP_DEV.fYfac = (float)(tft->height() - 2 * Mar_Val) /
                                    (int16_t)(XYpoint_Arr[0][1] -
                                              XYpoint_Arr[2][1]);

                    sTP_DEV.iXoff = (tft->width() -
                                     sTP_DEV.fXfac * (XYpoint_Arr[0][0] +
                                                      XYpoint_Arr[1][0])
                                    ) / 2;
                    sTP_DEV.iYoff = (tft->height() - sTP_DEV.fYfac *
                                     (XYpoint_Arr[0][1] + XYpoint_Arr[2][1])) / 2;
                } else if (tft->getRotation() == U2D_R2L) {
                    printf("U2D_R2L\r\n");

                    sTP_DEV.fXfac = (float)(tft->width() - 2 * Mar_Val) /
                                    (int16_t)(XYpoint_Arr[1][1] - XYpoint_Arr[0][1]);
                    sTP_DEV.fYfac = (float)(tft->height() - 2 * Mar_Val) /
                                    (int16_t)(XYpoint_Arr[2][0] - XYpoint_Arr[0][0]);

                    sTP_DEV.iXoff = (tft->width() -
                                     sTP_DEV.fXfac * (XYpoint_Arr[1][1] +
                                                      XYpoint_Arr[0][1])
                                    ) / 2;
                    sTP_DEV.iYoff = (tft->height() -
                                     sTP_DEV.fYfac * (XYpoint_Arr[2][0] +
                                                      XYpoint_Arr[0][0])
                                    ) / 2;
                } else {
                    printf("D2U_L2R\r\n");

                    sTP_DEV.fXfac = (float)(tft->width() - 2 * Mar_Val) /
                                    (int16_t)(XYpoint_Arr[0][1] -
                                              XYpoint_Arr[1][1]);
                    sTP_DEV.fYfac = (float)(tft->height() - 2 * Mar_Val) /
                                    (int16_t)(XYpoint_Arr[0][0] -
                                              XYpoint_Arr[2][0]);

                    sTP_DEV.iXoff = (tft->width() -
                                     sTP_DEV.fXfac * (XYpoint_Arr[0][1] +
                                                      XYpoint_Arr[1][1])
                                    ) / 2;
                    sTP_DEV.iYoff = (tft->height() -
                                     sTP_DEV.fYfac * (XYpoint_Arr[0][0] +
                                                      XYpoint_Arr[2][0])
                                    ) / 2;
                }
				
				printf("sTP_DEV.fXfac = %f \r\n", sTP_DEV.fXfac);
                printf("sTP_DEV.fYfac = %f \r\n", sTP_DEV.fYfac);
                printf("sTP_DEV.iXoff = %d \r\n", sTP_DEV.iXoff);
                printf("sTP_DEV.iYoff = %d \r\n", sTP_DEV.iYoff);
				
                //6.Calibration is successful
                //LCD_Clear(LCD_BACKGROUND);
                tft->fillRect(0, 0, tft->width(), tft->height(), WHITE);
                tft->setCursor(35, 110);
                tft->setTextColor(GREEN);
                tft->print("Touch Screen Adjust OK!");
                sleep_ms(1000);
                tft->fillRect(0, 0, tft->width(), tft->height(), BLACK);
                tft->setTextColor(WHITE);
                return;
                //Exception handling,Reset  Initial value
            default	:
                cnt = 0;
                DrawCross(tft->width() - Mar_Val, tft->height() - Mar_Val, WHITE);
                DrawCross(Mar_Val, Mar_Val, RED);
                tft->setCursor(40, 26);
                tft->setTextColor(RED);
                tft->print("TP Need readjust!");
                break;
            }
        }
    }
}


/*******************************************************************************
function:
		Use the default calibration factor
*******************************************************************************/
void cTouch::GetAdFac(void)
{
    sTP_DEV.fXfac = 0.0925;
    sTP_DEV.fYfac = 0.0765;
    sTP_DEV.iXoff = 32;
    sTP_DEV.iYoff = 28;
}

/*******************************************************************************
function:
		Touch pad initialization
*******************************************************************************/
void cTouch::Init(Adafruit_SPITFT *mytft )
{
    tft = mytft;
    gpio_put(TP_CS_PIN,1);

    Read_ADC_XY(&sTP_DEV.Xpoint, &sTP_DEV.Ypoint);
}
