/* 2^8 entries in the page table
 * Page size of 2^8 bytes
 * 16 entries in the TLB
 * Frame size of 2^8 bytes
 * 256 frames
 * Physical memory of 65,536 bytes (256 frames × 256-byte frame size)
 */
#include <stdbool.h>

#define PAGE_MASK 0xFF00
#define PAGE_NUM_BIT 8
#define FRAME_NUM_BIT 8
#define OFFSET_MASK 0xFF

#define PAGE_TABLE_ENTRY_NUM 256
#define PAGE_SIZE 0x100
#define TLB_ENTRY_NUM 16
#define FRAME_SIZE 0x100
#define FRAME_NUM 128

typedef struct page_table_entry {
    unsigned frame_num;
    bool valid_bit;
} PTE;

typedef struct tlb_entry {
    unsigned page_num;
    unsigned frame_num;
} TLBE;