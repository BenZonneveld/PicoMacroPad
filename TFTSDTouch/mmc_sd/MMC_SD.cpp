//#include "DEV_Config.h"
#include "Adafruit_SPITFT.h"
#include <ff.h>
#include "MMC_SD.h"			   


unsigned char  SD_Type=0;  //version of the sd card

uint8_t MMC_SD::SPI4W_Write_Byte(uint8_t value)
{
	uint8_t rxDat;
	spi_write_read_blocking(spi1, &value, &rxDat, 1);
	return rxDat;
}

uint8_t MMC_SD::SPI4W_Read_Byte(uint8_t value)
{
	return SPI4W_Write_Byte(value);
}

//data: data to be written to sd card.
//return: data read from sd card.
unsigned char MMC_SD::SPI_ReadWriteByte(unsigned char CMD)
{
	return SPI4W_Write_Byte(CMD);
//	return SPI_Read_Byte();
}	  

//set spi in low speed mode.
void MMC_SD::SPI_SpeedLow(void)
{
//	SPI1->CR1&=0XFFC7; 
//	SPI1->CR1|=SPI_BaudRatePrescaler_256;
//	SPI_Cmd(SPI1,ENABLE); 
}


//set spi in high speed mode.
void MMC_SD::SPI_SpeedHigh(void)
{
//	SPI1->CR1&=0XFFC7; 
//	SPI1->CR1|=SPI_BaudRatePrescaler_32;
//	SPI_Cmd(SPI1,ENABLE); 
}


//released spi bus
void MMC_SD::DisSelect(void)
{
	gpio_put(SD_CS_PIN,1);
 	SPI_ReadWriteByte(0xff);//providing extra 8 clocks  
}

//pick sd card and waiting until until it's ready
//return: 0: succed 1: failure
unsigned char MMC_SD::Select(void)
{
	gpio_put(SD_CS_PIN,0);
	if(WaitReady()==0)return 0; 
	DisSelect();
	return 1;
}

//waiting for sd card until it's ready
unsigned char MMC_SD::WaitReady(void)
{
	unsigned int t=0;
	do{
		if(SPI_ReadWriteByte(0XFF) == 0XFF)
			return 0;
		t++;		  	
	}while(t<0XFFFFFF);
	return 1;
}

//waiting for response from sd card.
//Response: expect from sd card.
//return: succeed for 0, fail for other else 
//return: 0 for success, other for failure.   
unsigned char MMC_SD::GetResponse(unsigned char Response)
{
	unsigned short Count=0xFFFF;	   						  
	while ((SPI_ReadWriteByte(0XFF) != Response) && Count)
		Count--; 	  
	if (Count==0)
		return MSD_RESPONSE_FAILURE;  
	else 
		return MSD_RESPONSE_NO_ERROR;
}

//read a buffer from sd card.
//*buf: pointer to a buffer.
//len: length of the buffer.
//return: 0 for success, other for failure.   
unsigned char MMC_SD::RecvData(unsigned char *buf,unsigned short len)
{			  	  
	if(GetResponse(0xFE))
		return 1;//waiting for start command send back from sd card.
    while(len--){//receiving data...
        *buf = SPI_ReadWriteByte(0xFF);
        buf++;
    }

    //send 2 dummy write (dummy CRC)
    SPI_ReadWriteByte(0xFF);
    SPI_ReadWriteByte(0xFF);									  					    
    return 0;
}

//write a buffer containing 512 bytes to sd card.
//buf: data buffer
//cmd: command
//return: 0 for success, other for failure.   
unsigned char MMC_SD::SendBlock(unsigned char*buf,unsigned char cmd)
{	
	unsigned short t;		  	  
	if(WaitReady())return 1;
	SPI_ReadWriteByte(cmd);
	if(cmd!=0XFD){
		for(t=0;t<512;t++)
			SPI_ReadWriteByte(buf[t]);
	    SPI_ReadWriteByte(0xFF);//ignoring CRC
	    SPI_ReadWriteByte(0xFF);
		t = SPI_ReadWriteByte(0xFF);
		if((t&0x1F)!=0x05)
			return 2;								  					    
	}						 									  					    
    return 0;
}

//send a command to sd card 
//cmd£∫command
//arg: parameter
//crc: crc
//return: response sent back from sd card.
unsigned char MMC_SD::SendCmd(unsigned char cmd, unsigned int arg, unsigned char crc)
{
    unsigned char r1;	
	unsigned char Retry=0; 
	DisSelect();
	if(Select())return 0XFF;

    SPI_ReadWriteByte(cmd | 0x40);
    SPI_ReadWriteByte(arg >> 24);
    SPI_ReadWriteByte(arg >> 16);
    SPI_ReadWriteByte(arg >> 8);
    SPI_ReadWriteByte(arg);	  
    SPI_ReadWriteByte(crc); 
	if(cmd == CMD12)SPI_ReadWriteByte(0xff); //Skip a stuff byte when stop reading
	Retry = 0X1F;
	do{
		r1 = SPI_ReadWriteByte(0xFF);
	}while((r1&0X80) && Retry--);	 

    return r1;
}


//obtain CID including manufacturer informationfrom sd card  
//*cid_dat: pointer to the buffer storing CID, at least 16 bytes.
//return: 0 no error  1 error
unsigned char MMC_SD::GetCID(unsigned char *cid_data)
{
    unsigned char r1;	   

    r1=SendCmd(CMD10,0,0x01);
    if(r1 == 0x00){
		r1=RecvData(cid_data,16);	 
    }
	DisSelect();
	if(r1)return 1;
	else return 0;
}																				  

//obtain CSD including storage and speed.
//*csd_data : pointer to the buffer storing CSD, at least 16 bytes.
//return: 0 no error  1 error
unsigned char MMC_SD::GetCSD(unsigned char *csd_data)
{
    unsigned char r1;	 
    r1 = SendCmd(CMD9,0,0x01);//∑¢CMD9√¸¡Ó£¨∂¡CSD send CMD9 in order to get CSD
    if(r1 == 0)	{
    	r1=RecvData(csd_data, 16);
    }
	DisSelect();
	if(r1)return 1;
	else return 0;
}  

//obtian the totals of sectors of sd card.
//return: 0 error, other else for storage of sd card.
//numbers of bytes of each sector must be 512, otherwise fail to initialization.  
uint32_t MMC_SD::GetSectorCount(void)
{
    unsigned char csd[16];
    unsigned int Capacity;  
    unsigned char n;
	unsigned short csize;  					    
	
    if(GetCSD(csd)!=0) return 0;	    
    //calculation for SDHC below
    if((csd[0]&0xC0)==0x40)	 //V2.00
    {	
		csize = csd[9] + ((unsigned short)csd[8] << 8) + 1;
		Capacity = (unsigned int)csize << 10;  //totals of sectors 		   
    }else//V1.XX
    {	
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((unsigned short)csd[7] << 2) + ((unsigned short)(csd[6] & 3) << 10) + 1;
		Capacity= (unsigned int)csize << (n - 9); 
    }
    return Capacity;
}

//initialize sd card 
unsigned char MMC_SD::Initialize(void)
{
    unsigned char r1;      
    unsigned short retry;  
    unsigned char buf[4];  
	unsigned short i;
   	
	gpio_put(SD_CS_PIN,1);
 	SPI_SpeedLow();	
 	for(i=0;i<10;i++)SPI_ReadWriteByte(0XFF);
	retry=20;
	do
	{
		r1=SendCmd(CMD0,0,0x95);//enter to idle state
	}while((r1!=0X01) && retry--);
 	SD_Type=0;
    
	if(r1==0X01)
	{
		if(SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
		{
			for(i=0;i<4;i++)buf[i]=SPI_ReadWriteByte(0XFF);	//Get trailing return value of R7 resp
			if(buf[2]==0X01&&buf[3]==0XAA)//is it support of 2.7~3.6V 
			{
				retry=0XFFFE;
				do
				{
					SendCmd(CMD55,0,0X01);	
					r1=SendCmd(CMD41,0x40000000,0X01);
				}while(r1&&retry--);
				if(retry&&SendCmd(CMD58,0,0X01)==0) //start to identify the SD2.0 version of sd card.
				{
					for(i=0;i<4;i++)buf[i]=SPI_ReadWriteByte(0XFF);//get OCR
					if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC;    //check CCS
					else SD_Type=SD_TYPE_V2;   
				}
			}
		}else//SD V1.x/ MMC	V3
		{
			SendCmd(CMD55,0,0X01);		
			r1=SendCmd(CMD41,0,0X01);	
			if(r1<=1)
			{		
				SD_Type=SD_TYPE_V1;
				retry=0XFFFE;
				do //exit idle state
				{
					SendCmd(CMD55,0,0X01);	
					r1=SendCmd(CMD41,0,0X01);
				}while(r1&&retry--);
			}else
			{
				SD_Type=SD_TYPE_MMC;//MMC V3
				retry=0XFFFE;
				do 
				{											    
					r1=SendCmd(CMD1,0,0X01);
				}while(r1&&retry--);  
			}
			if(retry==0||SendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;
		}
	}
	DisSelect();
	SPI_SpeedHigh();
	if(SD_Type)return 0;
	else if(r1)return r1; 	   
	return 0xaa;
}


//read SD card
//buf: data buffer
//sector: sector
//cnt: totals of sectors]
//return: 0 ok, other for failure
uint8_t MMC_SD::ReadDisk(uint8_t *buf,uint32_t sector,uint8_t cnt)
{
	unsigned char r1;
	if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;
	if(cnt==1)
	{
		r1=SendCmd(CMD17,sector,0X01);
		if(r1==0)
		{
			r1=RecvData(buf,512);   
		}
	}else
	{
		r1=SendCmd(CMD18,sector,0X01);
		do
		{
			r1=RecvData(buf,512);
			buf+=512;  
		}while(--cnt && r1==0); 	
		SendCmd(CMD12,0,0X01);	
	}   
	DisSelect();
	return r1;//
}


//write sd card 
//buf: data buffer
//sector: start sector
//cnt: totals of sectors]
//return: 0 ok, other for failure
uint8_t MMC_SD::WriteDisk(uint8_t *buf,uint32_t sector,uint8_t cnt)
{
	unsigned char r1;
	if(SD_Type!=SD_TYPE_V2HC)sector *= 512;
	if(cnt==1)
	{
		r1=SendCmd(CMD24,sector,0X01);
		if(r1==0)
		{
			r1=SendBlock(buf,0xFE); 
		}
	}else
	{
		if(SD_Type!=SD_TYPE_MMC)
		{
			SendCmd(CMD55,0,0X01);	
			SendCmd(CMD23,cnt,0X01);	
		}
 		r1=SendCmd(CMD25,sector,0X01);
		if(r1==0)
		{
			do
			{
				r1=SendBlock(buf,0xFC); 
				buf+=512;  
			}while(--cnt && r1==0);
			r1=SendBlock(0,0xFD);
		}
	}   
	DisSelect();
	return r1;
}	   

void MMC_SD::Init(void) {

	gpio_put(SD_CS_PIN, 1);
	gpio_put(LCD_CS_PIN, 1);
	gpio_put(TP_CS_PIN, 1);

	microSDFatFs = (FATFS*)malloc(sizeof(FATFS));

	int counter = 0;
	//Check the mounted device
	f_res = f_mount(microSDFatFs, (TCHAR const*)"/", 1);
	if (f_res != FR_OK) {
		printf("SD card mount file system failed ,error code :(%d)\r\n", f_res);
		//		GUI_DisString_EN(0,20,"FATFS_NOT_MOUNTED",&Font16,LCD_BACKGROUND,RED);
		//		while(1);
	}
	else {
		printf("SD card mount file system success!! \r\n");
		for (counter = 0; counter < MAX_BMP_FILES; counter++) {
			pDirectoryFiles[counter] = file_name[counter];
		}
	}
}
