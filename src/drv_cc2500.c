#include "board.h"

static uint8_t ccData[27];
static uint8_t ccLen;
static uint8_t channr;

uint8_t txid[2];

uint8_t calData[60];	 
uint8_t hopData[60];

uint8_t listLength;

int count=0;
static uint8_t bind_jumer=0;


void cc2500_writeReg(uint8_t address, uint8_t data);
void cc2500_resetChip(void);
void cc2500_strobe(uint8_t address);
unsigned char cc2500_readReg(unsigned char address);
void RegistersInit(int bind);
void CC2500tunning();
void bindFrsky();

void cc2500spiDetect(void)
{
    uint8_t in[4];

    spiSelect(true);
    spiTransferByte(0x30 | 0xC0);
    in[0] = spiTransferByte(0xff);
    spiSelect(false);
}

void cc2500spiInit(void)
{
    RegistersInit(1);
    bindFrsky();
    //RegistersInit(0);
    cc2500_writeReg(CC2500_0A_CHANNR, hopData[channr]);//0A-hop
    cc2500_writeReg(CC2500_23_FSCAL3,0x89);//23-89
    cc2500_strobe(CC2500_SRX);
}

void RegistersInit(int bind)
{
    cc2500_resetChip();
    cc2500_writeReg(CC2500_02_IOCFG0,   0x01);    // reg 0x02: RX complete interrupt(GDO0)
    cc2500_writeReg(CC2500_17_MCSM1,    0x0C);    // reg 0x17
    cc2500_writeReg(CC2500_18_MCSM0,    0x18);    // reg 0x18
    cc2500_writeReg(CC2500_06_PKTLEN,   0x19);    // Leave room for appended status bytes
    cc2500_writeReg(CC2500_08_PKTCTRL0, 0x05);    // reg 0x08
    cc2500_writeReg(CC2500_3E_PATABLE,  0xFF);    //
    cc2500_writeReg(CC2500_0B_FSCTRL1,  0x08);    // reg 0x0B
    cc2500_writeReg(CC2500_0C_FSCTRL0,  0x00);    // reg 0x0C
    cc2500_writeReg(CC2500_0D_FREQ2,    0x5C);    // reg 0x0D
    cc2500_writeReg(CC2500_0E_FREQ1,    0x76);    // reg 0x0E
    cc2500_writeReg(CC2500_0F_FREQ0,    0x27);    // reg 0x0F
    cc2500_writeReg(CC2500_10_MDMCFG4,  0xAA);    // reg 0x10
    cc2500_writeReg(CC2500_11_MDMCFG3,  0x39);    // reg 0x11
    cc2500_writeReg(CC2500_12_MDMCFG2,  0x11);    // reg 0x12
    cc2500_writeReg(CC2500_13_MDMCFG1,  0x23);    // reg 0x13
    cc2500_writeReg(CC2500_14_MDMCFG0,  0x7A);    // reg 0x14
    cc2500_writeReg(CC2500_15_DEVIATN,  0x42);    // reg 0x15
    cc2500_writeReg(CC2500_19_FOCCFG,   0x16);    // reg 0x16
    cc2500_writeReg(CC2500_1A_BSCFG,    0x6C);    // reg 0x1A
    cc2500_writeReg(CC2500_1B_AGCCTRL2, 0x03);    // reg 0x1B
    cc2500_writeReg(CC2500_1C_AGCCTRL1, 0x40);    // reg 0x1C
    cc2500_writeReg(CC2500_1D_AGCCTRL0, 0x91);    // reg 0x1D
    cc2500_writeReg(CC2500_21_FREND1,   0x56);    // reg 0x21
    cc2500_writeReg(CC2500_22_FREND0,   0x10);    // reg 0x22
    cc2500_writeReg(CC2500_23_FSCAL3,   0xA9);    // reg 0x23
    cc2500_writeReg(CC2500_24_FSCAL2,   0x05);    // reg 0x24
    cc2500_writeReg(CC2500_25_FSCAL1,   0x00);    // reg 0x25
    cc2500_writeReg(CC2500_26_FSCAL0,   0x11);    // reg 0x26
    cc2500_writeReg(CC2500_29_FSTEST,   0x59);    // reg 0x29
    cc2500_writeReg(CC2500_2C_TEST2,    0x88);    // reg 0x2C
    cc2500_writeReg(CC2500_2D_TEST1,    0x31);    // reg 0x2D
    cc2500_writeReg(CC2500_2E_TEST0,    0x0B);    // reg 0x2E
    cc2500_writeReg(CC2500_03_FIFOTHR,  0x0F);	  // reg 0x03
    //cc2500_writeReg(CC2500_09_ADDR, bind ? 0x03 : txid[0]);
    cc2500_writeReg(CC2500_09_ADDR, bind ? 0x03 : 0x42); //MY SPECIFIC TXID
    cc2500_strobe(CC2500_SIDLE);	// Go to idle...
    
    cc2500_writeReg(CC2500_07_PKTCTRL1,0x0D);	// reg 0x07 hack: Append status, filter by address, auto-flush on bad crc, PQT=0
    cc2500_writeReg(CC2500_0C_FSCTRL0,bind ? 0x00 : count);	// Frequency offset hack
    cc2500_writeReg(CC2500_0A_CHANNR, 0x00);
}

void cc2500spiMain(void)
{
    //uint8_t in[4];

    //spiSelect(true);
    //spiTransferByte(0x31 | 0xC0);
    //in[0] = spiTransferByte(0xff);
    //spiSelect(false);

}

void _spi_write(uint8_t command) 
{
    spiTransferByte(command | 0x80);
}

void cc2500_writeReg(uint8_t address, uint8_t data) 
{//same as 7105
    spiSelect(true);
    _spi_write(address); 
    _spi_write(data);  
    spiSelect(false);
} 

void cc2500_resetChip(void)
{
    // Toggle chip select signal
    spiSelect(false);
    delayMicroseconds(30);
    spiSelect(true);
    delayMicroseconds(30);
    spiSelect(false);
    delayMicroseconds(45);
    cc2500_strobe(CC2500_SRES);
    delayMicroseconds(100);
}

void cc2500_strobe(uint8_t address) 
{
    spiSelect(true);
    _spi_write(address);
    spiSelect(false);
}

unsigned char cc2500_readReg(unsigned char address) 
{ 
  uint8_t result;
  spiSelect(true);
  address |=0x80; //bit 7 =1 for reading
  _spi_write(address);
  result = spiTransferByte(0xff);  
  spiSelect(false);
  return(result); 
} 



static void ReadRegisterMulti(uint8_t address, uint8_t data[], uint8_t length)
{
    unsigned char i;

    spiSelect(true);
    _spi_write(address);
    for(i = 0; i < length; i++)
    {
        data[i] = spiTransferByte(0xff); 
    }
    spiSelect(false);
}

void cc2500_readFifo(uint8_t *dpbuffer, int len)
{
    ReadRegisterMulti(CC2500_3F_RXFIFO | CC2500_READ_BURST, dpbuffer, len);
}

void bindFrsky(){
		//LED_ON;
		//CC2500tunning();
		cc2500_writeReg(CC2500_0C_FSCTRL0,count);
		int adr=100;
		//EEPROM.write(adr+101,count);
		getBind();

}

void CC2500tunning(){
cc2500_strobe(CC2500_SRX);//enter in rx mode
int count1=0;
while(1){
  delay(100);
count1++;
if (count>=250){
count=0;
}
if(count1>3000){
count1=0;
cc2500_writeReg(CC2500_0C_FSCTRL0,count);	// Frequency offset hack
count=count+10;
}
			ccLen = cc2500_readReg(CC2500_3B_RXBYTES | CC2500_READ_BURST) & 0x7F;
			if (ccLen >= 18) {						
				//cc2500_readFifo((uint8_t *)ccData, ccLen);
				if(ccData[ccLen-1] & 0x80 && ccData[2]==0x01 && ccData[5]==0x00) {	
          
							break;
				}
			}
		}	
}

void getBind(void)
{
	cc2500_strobe(CC2500_SRX);//enter in rx mode
	listLength = 0;
	bool eol = false;	
	//           len|bind |tx id|idx|h0|h1|h2|h3|h4|00|00|00|00|00|00|01
	// Start by getting bind packet 0 and the txid
	//        0  1   2  txid0(3) txid1()4    5  6  7   8  9 10 11 12 13 14 15 16 17
//ccdata	//11 03 01  d7       2d          00 00 1e 3c 5b 78 00 00 00 00 00 00 01
            //11 03 01  19       3e          00 02 8e 2f bb 5c 00 00 00 00 00 00 01
	while (1) {
			ccLen = cc2500_readReg(CC2500_3B_RXBYTES | CC2500_READ_BURST) & 0x7F;
			if (ccLen >= 18) {						
				cc2500_readFifo((uint8_t *)ccData, ccLen);
				if(ccData[ccLen-1] & 0x80) {	
					if(ccData[2]==0x01) {	
						if(ccData[5]==0x00) {	
							txid[0] = ccData[3];
							txid[1]= ccData[4];							
							for (uint8_t n=0;n<5;n++) {
								hopData[ccData[5]+n] = ccData[6+n];
							}
							break;
						}
					}
				}
			}
	}

	for (uint8_t bindIdx=0x05; bindIdx<=120;bindIdx+=5) {
		while (1) {
				ccLen = cc2500_readReg(CC2500_3B_RXBYTES | CC2500_READ_BURST) & 0x7F;	
				if (ccLen >= 18) {						
					cc2500_readFifo((uint8_t *)ccData, ccLen);
					if(ccData[ccLen-1] & 0x80) {	
						if(ccData[2]==0x01) {														
							if((ccData[3]==txid[0]) && (ccData[4]==txid[1])) {	
								if(ccData[5]==bindIdx) {							
									for (uint8_t n=0;n<5;n++) {
										if (ccData[6+n] == ccData[ccLen-3]) {	
											eol = true;
											listLength = ccData[5]+n;
											break;
										}
										hopData[ccData[5]+n] = ccData[6+n];
									}
									break;	
								}
							}
						}
					}
				}
		}
		if (eol) break;	// End of list found, stop!
	}
	//Store_bind();	
	cc2500_strobe(CC2500_SIDLE);	// Back to idle
}	
