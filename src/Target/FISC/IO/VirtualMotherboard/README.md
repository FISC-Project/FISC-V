## **Directory:**  `< top >` / `Target` / `FISC` / `IO` / `VirtualMotherboard`  

### This directory contains the virtual 'physical' structure of the entire machine for this specific target.

### The devices present are:
> 1- Boot  
    1.1- EEPROM with Bootloader code / Firmware (aka BIOS)  
    1.2- CMOS Memory which holds Boot configurations  
> 2- Bus  
    2.1- AHCI - Advanced Host Controller Interface. Used for SATA devices  
    2.2- ATA / PATA - AT Attachment. Used for Hard Drive connections  
    2.3- ATAPI - ATA Packet Interface. Used for DVD and CD drives  
    2.4- DMA - Direct Memory Access. Used for IO devices to fetch memory without interrupting the CPU  
    2.5- PCI - Peripheral Component Interconnect. Used for many different various components, such as Graphics Cards, Network Cards and Sound Cards.  
    2.6- USB - Universal Serial Bus. You know what this is used for.  
> 3- Communication  
    3.1- Network - A generic network component which uses the host operating system's networking capabilities  
    3.2- Serial - Serial Communication Port which uses UART/USART  
> 4- Input  
    4.1- Keyboard  
    4.2- Mouse  
> 5- Interrupt Controller  
    5.1- 8259 PIC - Programmable Interrupt Controller. Used to connect some of the IO devices' interrupt signals together (Timer / Keyboard / Serial / ATA / Floppy / RTC / HDD) and is responsible for dispatching these signals to the CPU  
> 6- Sound  
    6.1- AC97 - Audio Codec 97. Used for producing high quality sound  
    6.2- Speaker - Used to produce very basic sounds through the host's speaker  
> 7- Storage  
    7.1- DVD-CD Disc  
    7.2- Floppy Disk  
    7.3- Hard Drive  
> 8- Time  
    8.1- RTC - Real Time Clock. Used to maintain the actual real physical time even after the Machine is shutdown  
    8.2 - Timer. Used to produce 'square-wave' / PWM like events on the CPU interrupt pins in order to execute tasks in a timely manner  
> 9- Video  
    9.1- VGA - Video Graphics Array. Used to produce video on a VGA screen. This module will simply use the operating system's video capabilities in order to simulate a virtual VGA screen  

## The main development focus for now will be on the following devices:  
> 1- VGA  
> 2- Keyboard  
> 3- Timer  