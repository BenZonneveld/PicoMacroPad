#ifndef _MMC_SD_H_
#define _MMC_SD_H_	
#include <stdio.h>
#include <ff.h>

#define MAX_BMP_FILES 25
#define _READONLY	0	/* 1: Read-only mode */
#define _USE_IOCTL	1


#define SPI_MMC_BAUDRATE 48*1000*1000

#include "integer.h"

/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
    RES_OK = 0,		/* 0: Successful */
    RES_ERROR,		/* 1: R/W Error */
    RES_WRPRT,		/* 2: Write Protected */
    RES_NOTRDY,		/* 3: Not Ready */
    RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;

/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


/* Command code for disk_ioctrl() */

/* Generic command */
#define CTRL_SYNC			0	/* Mandatory for read/write configuration */
#define GET_SECTOR_COUNT	1	/* Mandatory for only f_mkfs() */
#define GET_SECTOR_SIZE		2
#define GET_BLOCK_SIZE		3	/* Mandatory for only f_mkfs() */
#define CTRL_POWER			4
#define CTRL_LOCK			5
#define CTRL_EJECT			6
/* MMC/SDC command */
#define MMC_GET_TYPE		10
#define MMC_GET_CSD			11
#define MMC_GET_CID			12
#define MMC_GET_OCR			13
#define MMC_GET_SDSTAT		14
/* ATA/CF command */
#define ATA_GET_REV			20
#define ATA_GET_MODEL		21
#define ATA_GET_SN			22
//#define _DISKIO

#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06	   
   
#define CMD0    0       
#define CMD1    1
#define CMD8    8       
#define CMD9    9       
#define CMD10   10      
#define CMD12   12      
#define CMD16   16      
#define CMD17   17      
#define CMD18   18      
#define CMD23   23      
#define CMD24   24      
#define CMD25   25      
#define CMD41   41      
#define CMD55   55      
#define CMD58   58      
#define CMD59   59     

#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF

#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF

//typedef unsigned char uint8_t;

extern uint8_t  SD_Type;

class MMC_SD {
public:
    MMC_SD() {};
    void Init(void);
    DSTATUS disk_initialize(BYTE);
    DSTATUS disk_status(BYTE);
    DRESULT disk_read(BYTE, BYTE*, DWORD, BYTE);
#if	_READONLY == 0
    DRESULT disk_write(BYTE, const BYTE*, DWORD, BYTE);
#endif
    DRESULT disk_ioctl(BYTE, BYTE, void*);
    void	disk_timerproc(void);
//    DWORD get_fattime(void);
    uint8_t SPI4W_Write_Byte(uint8_t value);
    uint8_t SPI4W_Read_Byte(uint8_t value);
    FRESULT f_open(FIL* fp, const TCHAR* path, BYTE mode);				/* Open or create a file */
    FRESULT f_close(FIL* fp);											/* Close an open file object */
    FRESULT f_read(FIL* fp, void* buff, UINT btr, UINT* br);			/* Read data from a file */
    FRESULT f_write(FIL* fp, const void* buff, UINT btw, UINT* bw);	/* Write data to a file */
    FRESULT f_forward(FIL* fp, UINT(*func)(const BYTE*, UINT), UINT btf, UINT* bf);	/* Forward data to the stream */
    FRESULT f_lseek(FIL* fp, DWORD ofs);								/* Move file pointer of a file object */
    FRESULT f_truncate(FIL* fp);										/* Truncate file */
    FRESULT f_sync(FIL* fp);											/* Flush cached data of a writing file */
    FRESULT f_opendir(DIR* dp, const TCHAR* path);						/* Open a directory */
    FRESULT f_closedir(DIR* dp);										/* Close an open directory */
    FRESULT f_readdir(DIR* dp, FILINFO* fno);							/* Read a directory item */
    FRESULT f_mkdir(const TCHAR* path);								/* Create a sub directory */
    FRESULT f_unlink(const TCHAR* path);								/* Delete an existing file or directory */
    FRESULT f_rename(const TCHAR* path_old, const TCHAR* path_new);	/* Rename/Move a file or directory */
    FRESULT f_stat(const TCHAR* path, FILINFO* fno);					/* Get file status */
    FRESULT f_chmod(const TCHAR* path, BYTE value, BYTE mask);			/* Change attribute of the file/dir */
    FRESULT f_utime(const TCHAR* path, const FILINFO* fno);			/* Change times-tamp of the file/dir */
    FRESULT f_chdir(const TCHAR* path);								/* Change current directory */
    FRESULT f_chdrive(const TCHAR* path);								/* Change current drive */
    FRESULT f_getcwd(TCHAR* buff, UINT len);							/* Get current directory */
    FRESULT f_getfree(const TCHAR* path, DWORD* nclst, FATFS** fatfs);	/* Get number of free clusters on the drive */
    FRESULT f_getlabel(const TCHAR* path, TCHAR* label, DWORD* sn);	/* Get volume label */
    FRESULT f_setlabel(const TCHAR* label);							/* Set volume label */
    FRESULT f_mount(FATFS* fs, const TCHAR* path, BYTE opt);			/* Mount/Unmount a logical drive */
    FRESULT f_mkfs(const TCHAR* path, BYTE sfd, UINT au);				/* Create a file system on the volume */
    FRESULT f_fdisk(BYTE pdrv, const DWORD szt[], void* work);			/* Divide a physical drive into some partitions */
    void SPI_SpeedLow(void);
    void SPI_SpeedHigh(void);
    uint8_t ReadDisk(uint8_t* buf, uint32_t sector, uint8_t cnt);
    uint8_t WriteDisk(uint8_t* buf, uint32_t sector, uint8_t cnt);
    uint32_t GetSectorCount(void);
protected:
    WCHAR ff_convert(WCHAR wch, UINT dir);
    WCHAR ff_wtoupper(WCHAR wch);
    int cmp_lfn(WCHAR* lfnbuf,BYTE* dir);
    void gen_numname(BYTE* dst,const BYTE* src,const WCHAR* lfn,WORD seq);
    BYTE sum_sfn(const BYTE* dir);
    void fit_lfn(const WCHAR* lfnbuf,BYTE* dir,BYTE ord,BYTE sum);
    int pick_lfn(WCHAR* lfnbuf,BYTE* dir);
    uint8_t SPI_ReadWriteByte(uint8_t data);
    uint8_t WaitReady(void);
    uint8_t GetResponse(uint8_t Response);
    uint8_t Initialize(void);
    uint8_t GetCID(uint8_t* cid_data);
    uint8_t GetCSD(uint8_t* csd_data);
    void DisSelect(void);
    unsigned char Select();
    unsigned char RecvData(unsigned char* buf, unsigned short len);
    unsigned char SendBlock(unsigned char* buf, unsigned char cmd);
    unsigned char SendCmd(unsigned char cmd, unsigned int arg, unsigned char crc);
    /*--------------------------------------------------------------*/
    /* FatFs module application interface                           */
    static void mem_cpy(void* dst, const void* src, UINT cnt);
    static void mem_set(void* dst, int val, UINT cnt);
    static int mem_cmp(const void* dst, const void* src, UINT cnt);
    static int chk_chr(const char* str, int chr);
    int f_putc(TCHAR c, FIL* fp);										/* Put a character to the file */
    int f_puts(const TCHAR* str, FIL* cp);								/* Put a string to the file */
    int f_printf(FIL* fp, const TCHAR* str, ...);						/* Put a formatted string to the file */
    TCHAR* f_gets(TCHAR* buff, int len, FIL* fp);						/* Get a string from the file */

    FRESULT sync_window(FATFS* fs);
    FRESULT move_window(FATFS* fs,DWORD sector);
    FRESULT sync_fs(FATFS* fs);
    DWORD clust2sect(FATFS* fs, DWORD clst);
    DWORD get_fat(FATFS* fs,DWORD clst);
    FRESULT put_fat(FATFS* fs, DWORD clst, DWORD val);
    FRESULT remove_chain(FATFS* fs, DWORD clst);
    DWORD create_chain(FATFS* fs, DWORD clst);
    DWORD clmt_clust(FIL* fp,DWORD ofs);
    FRESULT dir_sdi(DIR* dp,WORD idx);
    FRESULT dir_next(DIR* dp,int stretch);
    FRESULT dir_alloc(DIR* dp,UINT nent);
    DWORD ld_clust(FATFS* fs,BYTE* dir);
    void st_clust(BYTE* dir,DWORD cl);
    FRESULT dir_find(DIR* dp);
    FRESULT dir_read(DIR* dp,int vol);
    FRESULT dir_register(DIR* dp);
    FRESULT dir_remove(DIR* dp);
    FRESULT create_name(DIR* dp,const TCHAR** path);
    void get_fileinfo(DIR* dp,FILINFO* fno);
    int get_ldnumber(const TCHAR** path);
    FRESULT follow_path(DIR* dp,const TCHAR* path);
    BYTE check_fs(FATFS* fs,DWORD sect);
    FRESULT find_volume(FATFS** rfs,const TCHAR** path,BYTE wmode);
    FRESULT validate(void* obj);
    void putc_bfd(putbuff* pb,TCHAR c);
    /* RTC function */
#if !_FS_READONLY
    DWORD get_fattime(void);
#endif

protected:
    FATFS* microSDFatFs;
    uint   spi_baudrate;
public:
    FRESULT f_res;
    char* pDirectoryFiles[MAX_BMP_FILES];
    uint8_t str[20];
    char file_name[25][12];
};

#endif