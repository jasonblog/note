/*
* reference:
* http://stackoverflow.com/questions/6870712/beagleboard-bare-metal-programming
*/

/* rammap */
MEMORY
{
    ram : ORIGIN = 0x80000000, LENGTH = 0x10000
}
 
SECTIONS
{
    .text : { *(.text*) } > ram
}
