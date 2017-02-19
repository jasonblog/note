#define NUM_PAGE_TABLE_ENTRIES 4096 /* 1 entry per 1MB, so this covers 4G address space */
#define CACHE_DISABLED    0x12
#define SDRAM_START       0x80000000
#define SDRAM_END         0x8fffffff
#define CACHE_WRITEBACK   0x1e

#define U32 unsigned int

void enable_mmu2(void)
{
    static unsigned int *page_table=(unsigned int *const)0x14000;

    int i;
    U32 reg;

    for (i = 0; i < NUM_PAGE_TABLE_ENTRIES; i++)
   {
		/* map 0x20000000~0x210000000 to 0xa0000000~0xa1000000 */
		if (i >= (0xa0000000 >> 20) && i <= ( 0xa1000000 >> 20))
		{
			page_table[i] = (i-2048)<<20 | 0x0410 | 2;
			printk("page_table[%d]addr=%x, section=%x \n",i,&page_table[i],page_table[i]);
		}
		else
                       /*not mapping yet*/
			page_table[i]= i<<20| 0x0410|2 ;
   }
    printk("page_talble base=%x page_table409=%x \n",page_table,&page_table[4095]);
    /* Set up an identity-mapping for all 4GB, rw for everyone */
    /* Copy the page table address to cp15 */
    asm volatile("mcr p15, 0, %0, c2, c0, 0"
            : : "r" (page_table) : "memory");
    /* Set the access control to all-supervisor */
    asm volatile("mcr p15, 0, %0, c3, c0, 0" : : "r" (~0));

    /* Enable the MMU */
    asm("mrc p15, 0, %0, c1, c0, 0" : "=r" (reg) : : "cc");
    reg|=0x1;
    asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r" (reg) : "cc");
}

void enable_mmu(void)
{
    static U32 __attribute__((aligned(16384))) page_table[NUM_PAGE_TABLE_ENTRIES];
    int i;
    U32 reg;

    /* Set up an identity-mapping for all 4GB, rw for everyone */
    for (i = 0; i < NUM_PAGE_TABLE_ENTRIES; i++)
        page_table[i] = i << 20 | (3 << 10) | CACHE_DISABLED;
    /* Then, enable cacheable and bufferable for RAM only */
    for (i = SDRAM_START >> 20; i < SDRAM_END >> 20; i++)
    {
        page_table[i] = i << 20 | (3 << 10) | CACHE_WRITEBACK;
    }

    /* Copy the page table address to cp15 */
    asm volatile("mcr p15, 0, %0, c2, c0, 0"
            : : "r" (page_table) : "memory");
    /* Set the access control to all-supervisor */
    asm volatile("mcr p15, 0, %0, c3, c0, 0" : : "r" (~0));

    /* Enable the MMU */
    asm("mrc p15, 0, %0, c1, c0, 0" : "=r" (reg) : : "cc");
    reg|=0x1;
    asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r" (reg) : "cc");
}
