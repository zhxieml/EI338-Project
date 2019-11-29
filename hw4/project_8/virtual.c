#include <stdlib.h>
#include <stdio.h>
#include "virtual.h"

PTE pt[PAGE_TABLE_ENTRY_NUM];
TLBE tlb[TLB_ENTRY_NUM];
char memory[FRAME_NUM * FRAME_SIZE]; // the char data type occupies a byte of storage
unsigned frame_to_page[FRAME_NUM];
int free_frame_index;
int victim_frame_index; // index of vistim frame in page replacement using the FIFO strategy
int tlb_first_index;
int tlb_capacity;

// statistics
float page_fault_num, page_refer_num, TLB_hit_num, TLB_refer_num;

FILE *backing_store_fp;

unsigned extract_page_num(unsigned logical_addr) {
    return (logical_addr & PAGE_MASK) >> PAGE_NUM_BIT; 
}

unsigned extract_offset(unsigned logical_addr) {
    return logical_addr & OFFSET_MASK;
}

unsigned get_frame_num_from_pt(unsigned page_num) {
    ++page_refer_num;
    
    if (pt[page_num].valid_bit)
        return pt[page_num].frame_num;

    // a page fault occurs
    ++ page_fault_num;

    unsigned frame_num;

    // find free frame
    if (free_frame_index < FRAME_NUM) {
        pt[page_num].frame_num = frame_num = free_frame_index++;
        pt[page_num].valid_bit = 1;
        frame_to_page[frame_num] = page_num;
    }

    // replace a page
    else {
        pt[page_num].frame_num = frame_num = victim_frame_index;
        pt[page_num].valid_bit = 1;
        pt[frame_to_page[frame_num]].valid_bit = 0; // erase
        victim_frame_index = (victim_frame_index + 1) % FRAME_NUM; // realized by a circular array
        frame_to_page[frame_num] = page_num;
    }

    // swap
    fseek(backing_store_fp, page_num * PAGE_SIZE, SEEK_SET);
    fread(memory + frame_num * FRAME_SIZE, FRAME_SIZE, 1, backing_store_fp);

    return frame_num;
}

unsigned get_frame_num_from_TLB(unsigned page_num) {
    ++TLB_refer_num;
    
    for (int i = 0; i < tlb_capacity; ++i) {
        if (tlb[i].page_num == page_num) {
            ++TLB_hit_num;
            
            return tlb[i].frame_num;
        }
    }

    // a TLB miss occurs
    unsigned frame_num = get_frame_num_from_pt(page_num);

    // TLB is not full
    if (tlb_capacity < TLB_ENTRY_NUM) {
        tlb[tlb_capacity].frame_num = frame_num;
        tlb[tlb_capacity].page_num = page_num;

        ++tlb_capacity;
    }

    // TLB is full
    else {
        tlb[tlb_first_index].frame_num = frame_num;
        tlb[tlb_first_index].page_num = page_num;

        tlb_first_index = (tlb_first_index + 1) % TLB_ENTRY_NUM;
    }

    return frame_num;
}

void update_TLB();

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid input!\n");

        return 1;
    }

    FILE *logical_addrs_fp = fopen(argv[1], "r"); 
    FILE *ground_truth_fp = fopen(argv[2], "r");
    unsigned logical_addr, physical_addr;
    int ground_truth, sample;
    unsigned page_num, frame_num, offset;

    backing_store_fp = fopen("BACKING_STORE.bin", "rb");

    while (fscanf(logical_addrs_fp, "%d", &logical_addr) != EOF) {
        page_num = extract_page_num(logical_addr);
        offset = extract_offset(logical_addr);
        frame_num = get_frame_num_from_TLB(page_num);
        physical_addr = (frame_num << FRAME_NUM_BIT) + offset;
        sample = memory[physical_addr];

        printf("Virtual address: %d Physical address: %d Value: %d\n", logical_addr, physical_addr, sample);
    }

    printf("----------------- Statistics -----------------\n");
    printf("Number of page table entries\t%d\n", PAGE_TABLE_ENTRY_NUM);
    printf("Page/frame size\t\t\t%d\n", PAGE_SIZE);
    printf("Number of TLB entries\t\t%d\n", TLB_ENTRY_NUM);
    printf("Number of frames\t\t%d\n", FRAME_NUM);
    printf("Page-falut rate\t\t\t%5.3f\n", page_fault_num / page_refer_num);
    printf("TLB hit rate\t\t\t%5.3f\n", TLB_hit_num / TLB_refer_num);
    printf("----------------------------------------------\n");

    fclose(logical_addrs_fp);
    fclose(backing_store_fp);

    return 0;
}
