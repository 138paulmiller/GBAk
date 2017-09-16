#ifndef INTERRUPT_H
#define INTERRUPT_H
//Interrupt table, each element points to an interrupt callback
// interrupt callbacks
void interrupt_ignore( ) {
    // do nothing by default
}

//Table of function addresses where the GBA will jump to on interrupt
void (*const interrupt_table[13])( ) = {
    interrupt_ignore,   // V Blank interrupt 
    interrupt_ignore,   // H Blank interrupt 
    interrupt_ignore,   // V Counter interrupt 
    interrupt_ignore,   // Timer 0 interrupt 
    interrupt_ignore,   // Timer 1 interrupt 
    interrupt_ignore,   // Timer 2 interrupt 
    interrupt_ignore,   // Timer 3 interrupt 
    interrupt_ignore,   // Serial communication interrupt 
    interrupt_ignore,   // DMA 0 interrupt 
    interrupt_ignore,   // DMA 1 interrupt 
    interrupt_ignore,   // DMA 2 interrupt 
    interrupt_ignore,   // DMA 3 interrupt 
    interrupt_ignore,   // Key interrupt 
};




#endif //INTERRUPT_H
