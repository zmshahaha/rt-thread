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

#define SZ_128M 0x8000000UL

void rt_ofw_common_setup(void)
{
    extern char _head, _end;
    rt_size_t kernel_start_aligned = RT_ALIGN_DOWN((rt_size_t)rt_kmem_v2p(&_head), ARCH_PAGE_SIZE);
    rt_size_t kernel_end_aligned = RT_ALIGN((rt_size_t)rt_kmem_v2p(&_end), ARCH_PAGE_SIZE);
    rt_size_t init_page_start = kernel_end_aligned;
    rt_size_t init_page_end = init_page_start + SZ_128M;
    rt_region_t init_page_reg = {
        .start = init_page_start,
        .end = init_page_end,
    };
    
    rt_memblock_reserve_memory("kernel", kernel_start_aligned, kernel_end_aligned, MEMBLOCK_NONE);
    rt_memblock_reserve_memory("init-page", init_page_start, init_page_end, MEMBLOCK_NONE);

    rt_page_init(init_page_reg);
    rt_system_heap_init(RT_NULL, RT_NULL);

    rt_fdt_scan_memory();
}