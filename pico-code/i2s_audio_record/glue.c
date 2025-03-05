
#include "stdio.h"
#include "pico/stdlib.h"
#include "stdlib.h"
#include "ff.h"
#include "diskio.h"
#include "pico_storage.h"
#include "msc_storage_driver.h"
#include "spi_sdmmc.h"
#include "hardware/rtc.h"
#include "inttypes.h"
#include "hardware/gpio.h"


#define SDMMC_DRV_0     0
sdmmc_data_t *pSDMMC=NULL;

//==================//
DSTATUS disk_initialize (BYTE drv){
    DSTATUS stat;
    switch (drv) {
        case SDMMC_DRV_0:
            if (pSDMMC == NULL) {
                pSDMMC = (sdmmc_data_t*)malloc(sizeof(sdmmc_data_t));
                pSDMMC->csPin = SDMMC_PIN_CS;
                pSDMMC->spiPort = SDMMC_SPI_PORT;
                pSDMMC->spiInit=false;
                pSDMMC->sectSize=512;
#ifdef __SPI_SDMMC_DMA
                pSDMMC->dmaInit=false;
#endif
             stat = sdmmc_disk_initialize(pSDMMC);
            return stat;
        } else {
            return RES_OK;
        }
           
        break;
        
    }
    return STA_NOINIT;
 }
/*-----------------------------------------------------------------------*/
/* Get disk status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (BYTE drv) {
    DSTATUS stat;
    switch (drv) {
        case SDMMC_DRV_0:
            stat=  sdmmc_disk_status(pSDMMC); /* Return disk status */
            return stat;
        break;
        
    }
    return RES_PARERR;
	
}

/*-----------------------------------------------------------------------*/
/* Read sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE drv,		/* Physical drive number (0) */
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	LBA_t sector,	/* Start sector number (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
    DSTATUS stat;
    switch (drv) {
        case SDMMC_DRV_0:
            stat = sdmmc_disk_read(buff, sector, count, pSDMMC);
            return stat;
        break;
        
    }
	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if FF_FS_READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0) */
	const BYTE *buff,	/* Ponter to the data to write */
	LBA_t sector,		/* Start sector number (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
    DSTATUS stat = STA_NODISK;
    switch (drv) {
        case SDMMC_DRV_0:
            stat = sdmmc_disk_write(buff, sector, count, pSDMMC);
            return stat;
        break;
        
    }
	return RES_PARERR;

}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous drive controls other than data read/write               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive number (0) */
	BYTE cmd,		/* Control command code */
	void *buff		/* Pointer to the conrtol data */
)
{
    DSTATUS stat;
    switch (drv) {
        case SDMMC_DRV_0:
            stat = sdmmc_disk_ioctl(cmd, buff, pSDMMC);
            return stat;
        break;
        
    }
	return RES_PARERR;
}

DWORD get_fattime(void) {
    datetime_t t ;
    bool rc = rtc_get_datetime(&t);
    if (!rc) return 0;

    DWORD fattime = 0;
    // bit31:25
    // Year origin from the 1980 (0..127, e.g. 37 for 2017)
    uint32_t yr = t.year - 1980;
    fattime |= (yr & 0b01111111) << 25;
    // bit24:21
    // Month (1..12)
    uint32_t mo = t.month;
    fattime |= ( mo & 0b00001111) << 21;
    // bit20:16
    // Day of the month (1..31)
    uint32_t da = t.day;
    fattime |= (da & 0b00011111) << 16;
    // bit15:11
    // Hour (0..23)
    uint32_t hr = t.hour;
    fattime |= (hr & 0b00011111) << 11;
    // bit10:5
    // Minute (0..59)
    uint32_t mi = t.min;
    fattime |= ( mi&0b00111111) << 5;
    // bit4:0
    // Second / 2 (0..29, e.g. 25 for 50)
    uint32_t sd = t.sec / 2;
    fattime |= (sd&0b00011111);
    return fattime;
}

void led_blinking(void)
{
    static absolute_time_t  t1;
    static bool state=false;
        
    // Blink every interval ms
    if ( absolute_time_diff_us(t1, get_absolute_time()) < 100000) return; // not enough time
    t1 = get_absolute_time();
    gpio_put(LED_BLINKING_PIN, state);
    state = !state;
}

void led_blinking_off(void) {
    gpio_put(LED_BLINKING_PIN, false);
}

