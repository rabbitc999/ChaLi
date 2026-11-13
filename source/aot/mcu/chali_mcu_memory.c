#include "chali_mcu_memory.h"
#include "chali_mcu_hal.h"

typedef struct
{
    uint64_t destination, source, size;
} ChaliDataSegment;

typedef struct ChaliBlock
{
    struct ChaliBlock *next;
    uint64_t address;
    size_t size;
    _Alignas(16) unsigned char data[];
} ChaliBlock;

extern const unsigned char chali_data_image[];
extern const ChaliDataSegment chali_data_segments[];
extern const uint64_t chali_data_segment_count, chali_data_size, chali_heap_size;
extern void chali_reset_slots(void);
extern void chali_reset_address_view(void);
extern _Noreturn void chali_panic(const char *);

static unsigned char *data;
static ChaliBlock *blocks;
static uint64_t nextAddress;
static uint64_t liveBytes;

/**
 * @brief 建立零填充数据区，装入稀疏片段并重置编译器槽。
 */
void chali_host_init(void)
{
    if (data || chali_data_size > SIZE_MAX) chali_panic("MCU: invalid data image");
    size_t size = (size_t)chali_data_size;
    data = chali_mcu_allocate(size ? size : 1);
    if (!data) chali_panic("MCU: data allocation failed");
    for (size_t i = 0; i < size; ++i) data[i] = 0;
    for (uint64_t i = 0; i < chali_data_segment_count; ++i)
    {
        const ChaliDataSegment *segment = &chali_data_segments[i];
        if (segment->destination > size || segment->size > size - segment->destination)
            chali_panic("MCU: invalid data segment");
        for (uint64_t j = 0; j < segment->size; ++j)
            data[segment->destination + j] = chali_data_image[segment->source + j];
    }
    nextAddress = (chali_data_size + 15u) & ~(uint64_t)15u;
    if (nextAddress == 0) nextAddress = 16;
    chali_reset_slots();
    chali_reset_address_view();
}

/**
 * @brief 清除地址缓存，释放全部仍存活的堆块和静态数据。
 */
void chali_host_finish(void)
{
    chali_reset_address_view();
    while (blocks) chali_host_drop((int64_t)blocks->address);
    chali_mcu_free(data);
    data = NULL;
}

/**
 * @brief 查找整个访问范围所在的存活数据区或堆块。
 * @param address 逻辑字节地址。
 * @param count 访问长度。
 * @return 地址对应的物理指针。
 */
unsigned char *chali_host_address(int64_t address, uint64_t count)
{
    if (!data || address < 0) chali_panic("MCU: invalid address");
    const uint64_t offset = (uint64_t)address;
    if (offset < chali_data_size && count <= chali_data_size - offset)
        return data + (size_t)offset;
    for (ChaliBlock *block = blocks; block; block = block->next)
    {
        if (offset < block->address) continue;
        const uint64_t within = offset - block->address;
        if (within <= block->size && count <= block->size - within)
            return block->data + (size_t)within;
    }
    chali_panic("MCU: invalid or expired range");
}

/**
 * @brief 通过板级堆分配存活块，逻辑基址在一次运行中不复用。
 * @param bytes 字节数，零按一个字节处理。
 * @return 16 字节对齐的逻辑基址。
 */
int64_t chali_host_allocate(uint64_t bytes)
{
    if (bytes == 0) bytes = 1;
    if (!data || bytes > SIZE_MAX - sizeof(ChaliBlock) ||
        bytes > chali_heap_size - liveBytes || nextAddress > INT64_MAX - 16 ||
        bytes > INT64_MAX - 16 - nextAddress)
        chali_panic("MCU: heap budget exceeded");
    ChaliBlock *block = chali_mcu_allocate(sizeof(*block) + (size_t)bytes);
    if (!block) chali_panic("MCU: heap allocation failed");
    block->next = blocks;
    block->address = nextAddress;
    block->size = (size_t)bytes;
    for (size_t i = 0; i < block->size; ++i) block->data[i] = 0;
    blocks = block;
    liveBytes += bytes;
    nextAddress += (bytes + 16u) & ~(uint64_t)15u;
    return (int64_t)block->address;
}

/**
 * @brief 撤销基址对应的块并归还板级堆。
 * @param address 块基址，零表示空句柄。
 */
void chali_host_drop(int64_t address)
{
    if (!address) return;
    for (ChaliBlock **link = &blocks; *link; link = &(*link)->next)
    {
        ChaliBlock *block = *link;
        if (block->address != (uint64_t)address) continue;
        *link = block->next;
        liveBytes -= block->size;
        chali_reset_address_view();
        chali_mcu_free(block);
        return;
    }
    chali_panic("MCU: invalid heap release");
}
