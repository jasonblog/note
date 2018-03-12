#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>

#define PAGE_MAP_FILE "/proc/self/pagemap"
#define PFN_MASK ((((uint64_t)1)<<55)-1)
#define PFN_PRESENT_FLAG (((uint64_t)1)<<63)

int mem_addr_vir2phy(unsigned long vir, unsigned long *phy)
{
    int fd;
    int page_size = getpagesize();
    unsigned long vir_page_idx = vir / page_size;
    unsigned long pfn_item_offset = vir_page_idx * sizeof(uint64_t);
    uint64_t pfn_item;

    fd = open(PAGE_MAP_FILE, O_RDONLY);
    if (fd < 0) {
        perror("open('" PAGE_MAP_FILE "')");
        return -1;
    }
    if ((off_t)-1 == lseek(fd, pfn_item_offset, SEEK_SET)) {
        perror("lseek('" PAGE_MAP_FILE "')");
        return -1;
    }
    if (sizeof(uint64_t) != read(fd, &pfn_item, sizeof(uint64_t))) {
        perror("read('" PAGE_MAP_FILE "')");
        return -1;
    }

    if (0==(pfn_item & PFN_PRESENT_FLAG)) {
        printf("page is not present");
        return -1;
    }
    *phy = (pfn_item & PFN_MASK)*page_size + vir % page_size;
    return 0;
}

int main(int argc, char **argv)
{
    int a = 0x12345678;
    unsigned long phy;

    mem_addr_vir2phy((unsigned long)&a, &phy);

    printf("vaddr = %p, phy = 0x%lx\n", &a, phy);
    printf("Enter any key to exit\n");
    fgetc(stdin);

    return 0;
}
