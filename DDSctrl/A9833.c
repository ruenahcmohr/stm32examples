
#include "A9833.h"



void Ad9833SetFreq ( double f, uint8_t channel )  {

//  unsigned long  div = f * 10.73741824;      // make freq register from frequency (25Mhz) (for 20Mhz use 13.4217728);

  uint32_t  div = (f * 5497.558f);      // *512 for SIN lookup
  
  uint16_t MSW = ((div>>14) & 0x3FFF) | 0x4000;    // build MSW (and set freq flag)
  uint16_t LSW = (div & 0x3FFF)       | 0x4000;	 // build LSW

  if (channel) {
    AD9833Write16(0x2868);                                 // set  28bit word for freq 1
  } else {
    AD9833Write16(0x2068);                                 // set  28bit word for freq 0
  } 

  AD9833Write16(LSW);					 // send the LSW first
  AD9833Write16(MSW);					 // send the MSW last
}


void AD9833Init (void) {

  AD9833Write16(0x2100);		// control word, "reset", which resets nothing but is really disable. output to mid value voltage 

  AD9833Write16(0x7200);		// Freq0 Freq MSW  
  AD9833Write16(0x4000);		// Freq0 Freq LSW  
  AD9833Write16(0xC000);		// Freq0 Phase = 0

  AD9833Write16(0xAC00);		// Freq1 Freq MSW  
  AD9833Write16(0x8000); 		// Freq1 Freq LSW  
  AD9833Write16(0xE000);		// Freq1 Phase = 0

  AD9833Write16(0x2068);                // 2068 control word, re-enable,  output = MSB, using freq0 and phase0

}


void AD9833Write16 (uint16_t data)    {

  uint16_t temp;  
			
  A9833StrobeL();                // Fsync Low --> begin frame
  
  for( temp = 0x8000; temp != 0; temp >>= 1) {    
    if ( (data & temp) != 0 ) {    A9833SendOne();
    } else {                       A9833SendZero();
    }    
  } 

  A9833StrobeH();                  // Fsync High --> End of frame
} 


void AD9833Delay (uint16_t d) {
  for(; d ; d--)  NOP();
}
