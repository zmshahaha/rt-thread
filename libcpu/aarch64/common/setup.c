/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-10-16     zmshahaha    memory setup
 */

#include <rtthread.h>
#include <mmu.h>
#include <mm_memblock.h>
#include <drivers/ofw_fdt.h>
#include <drivers/ofw_raw.h>

#define SZ_128M 0x8000000UL

static rt_uint64_t initrd_ranges[3] = { };
static void* fdt_vaddr = RT_NULL;

void rt_hw_common_setup(void)
{
    extern char _head, _end;
    rt_size_t kernel_start = RT_ALIGN_DOWN((rt_size_t)rt_kmem_v2p(&_head), ARCH_PAGE_SIZE);
    rt_size_t kernel_end = RT_ALIGN((rt_size_t)rt_kmem_v2p(&_end), ARCH_PAGE_SIZE);
    rt_size_t fdt_start = RT_ALIGN_DOWN((rt_size_t)rt_kmem_v2p(&fdt_vaddr), ARCH_PAGE_SIZE);
    rt_size_t fdt_end = RT_ALIGN(fdt_start + fdt_totalsize(fdt_vaddr), ARCH_PAGE_SIZE);
    rt_size_t init_page_start = kernel_end;
    rt_size_t init_page_end = init_page_start + SZ_128M;
    rt_region_t init_page_reg = {
        .start = init_page_start,
        .end = init_page_end,
    };


    rt_fdt_prefetch(fdt_vaddr);
    rt_memblock_reserve_memory("fdt", fdt_start, fdt_end, MEMBLOCK_NONE);
    rt_memblock_reserve_memory("kernel", kernel_start, kernel_end, MEMBLOCK_NONE);
    rt_memblock_reserve_memory("init-page", init_page_start, init_page_end, MEMBLOCK_NONE);

    rt_page_init(init_page_reg);
    rt_system_heap_init(RT_NULL, RT_NULL);

    rt_fdt_scan_initrd(initrd_ranges);
    rt_fdt_scan_memory();
    rt_memblock_setup_memory_environment();
}
