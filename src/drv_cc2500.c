#include "board.h"

void cc2500spiDetect(void)
{
    uint8_t in[4];

    spiSelect(true);
    spiTransferByte(0x30 | 0xC0);
    in[0] = spiTransferByte(0xff);
    spiSelect(false);
}
