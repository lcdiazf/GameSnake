#include <generated/csr.h>
#include "globalvar.h"

static uint8_t SPI_Write(uint8_t DATA, uint8_t CS){
    if(idleState==0x1)spi_sd_config_write(0x50500000+(8*CS));
    if(idleState==0x0)spi_sd_config_write(0x00000000+(8*CS));
    spi_sd_mosi_data_write(DATA<<24);
    spi_sd_start_write(1);
    while(spi_sd_active_read()==1);
    return spi_sd_miso_data_read();
}

static void dummy(int times){
        for(int i=0;i<times;i++)
            {
                SPI_Write(0xff,1);
            }
}

static uint8_t SD_Command(uint8_t cmd, uint32_t arg, uint8_t crc) {	
	unsigned char retry;
    uint8_t res;

	SPI_Write(cmd | 0x40,0);		//SD Spec: 1st bit always zero, 2nd always 1
	SPI_Write(arg>>24,0);			//first of the 4 bytes address
	SPI_Write(arg>>16,0);			//second
	SPI_Write(arg>>8,0);			//third
	SPI_Write(arg,0);				//fourth
	SPI_Write(crc,0);				//CRC and last bit 1

	retry = 0;
	while((res = SPI_Write(0xff,0)) == 0xff) {	//wait for ack.
		if(retry++ > 100) {
            printf("CMD-REJECTED\n");		    //overtime exit.
			return 0x11;  			
		}
	}
	return res;	  
}

static void initConfig(void){    
    spi_sd_xfer_write(0x04040001);
    idleState=0x1;
    int dum=8;
    dummy(10);
    if(SD_Command(0, 0x0, 0x95)==0x01){//printf("Listo-CMD0\n");  //Start idle
    }else{printf("waiting SD\n");
    return;}
    dummy(dum);
    uint8_t retry=0;
    while(SD_Command(1, 0x0, 0xff)!=0x0){
        retry++;    
        dummy(dum);
        printf("waiting SD\n");
        if(retry>250){
            printf("Memory SD not found\n");
            return;
        }
    }
    //printf("Listo-CMD1\n");
    while(SD_Command(59, 0x0, 0xff)!=0x0){
    dummy(dum);
        printf("waiting SD\n");
    }
    //printf("Listo-CMD59\n");
    while(SD_Command(16, 0x10, 0xff)!=0x0){
    dummy(dum);
        printf("waiting SD\n");
    }
    //printf("Listo-CMD16\n");
    printf("SD Ready\n");
    idleState=0x0;
}

static uint32_t readSingle(uint32_t addr, uint32_t len){	
	uint8_t retry=0;
    uint16_t block=0;

    while(SD_Command(16, len, 0xff)!=0x0){
        printf("waiting SD\n");
        if(retry++ > 100) {
            printf("CMD16-Rejected\n");						//overtime exit.
            return 0x11;
		}
    }

    while(SD_Command(17, addr, 0xff)!=0x0){
        printf("waiting SD\n");
        if(retry++ > 100) {
            printf("CMD17-Rejected\n");						//overtime exit.
            return 0x11;
		}
    }

	retry = 0;

	while((SPI_Write(0xff,0)) != 0xfe) {	//wait for ack.
		if(retry++ > 100) {						//overtime exit.
            printf("Overtime-Token\n");
            return 0xFF;//*******
		}
	}
//    printf("Token verified\n");
    uint8_t data=0;
    for(uint8_t i=0;i<2;i++){
        data=SPI_Write(0xff,0);
//        printf("Block: %X Data:%X\n",block,data);
        block=block<<8;
        block |= data;
    }
    block |= data;
//    printf("Lectura completada\n");
    return block;
}
