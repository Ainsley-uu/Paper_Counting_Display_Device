#include "table_fft.h"
#include "stm32f10x.h"                  // Device header
#include "stm32_dsp.h"

#define NPT 256
uint32_t adc_buf[NPT]={0};
long lBufInArray[NPT];
long lBufOutArray[NPT/2];
long lBufMagArray[NPT/2];


void InitBufInArray()
{
    unsigned short i;
    float fx;
    for(i=0; i<NPT; i++)
    {
        fx = 1500 * sin(PI2 * i * 350.0 / Fs) +
             2700 * sin(PI2 * i * 8400.0 / Fs) +
             4000 * sin(PI2 * i * 18725.0 / Fs);
        lBufInArray[i] = ((signed short)fx) << 16;
    }
}

void GetPowerMag()
{
    signed short lX,lY;
    float X,Y,Mag;
    unsigned short i;
    for(i=0; i<NPT/2; i++)
    {
        lX  = (lBufOutArray[i] << 16) >> 16;
        lY  = (lBufOutArray[i] >> 16);
        
        // ??32768??65536????????????
        X = NPT * ((float)lX) / 32768;
        Y = NPT * ((float)lY) / 32768;
        Mag = sqrt(X * X + Y * Y) / NPT;
        if(i == 0)
            lBufMagArray[i] = (unsigned long)(Mag * 32768);
        else
            lBufMagArray[i] = (unsigned long)(Mag * 65536);
    }
}

int main(){
    // ????
	for(int i=0;i<NPT;i++)
		lBufInArray[i] = ((signed short)(adc_buf[i]-2048)) << 16;
	cr4_fft_256_stm32(lBufOutArray, lBufInArray, NPT);
}
