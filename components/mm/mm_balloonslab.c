/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-10-12     zmshahaha    the first version
 */

#include <rtdef.h>
#include <mm_page.h>
#include <mm_balloonslab.h>
#include <mmu.h>

#define unlikely(x)	__builtin_expect(!!(x), 0)

#ifndef RT_SLAB_GRAIN_SIZE
#define RT_SLAB_GRAIN_SIZE 0x200000
#endif

static struct rt_balloonslab *_current_slab;
static rt_size_t _init_slab_size_bits;
static rt_size_t _init_balloon_size;
static rt_size_t _init_slab_size;

rt_slab_t rt_balloonslab_init(void *begin_addr, rt_size_t size)
{
    _init_slab_size_bits = rt_page_bits(RT_SLAB_GRAIN_SIZE);
    begin_addr = rt_pages_alloc(_init_slab_size_bits);
    _init_balloon_size = 1 << (ARCH_PAGE_SHIFT + _init_slab_size_bits);
    _init_slab_size = _init_balloon_size - sizeof(struct rt_balloonslab);
    _current_slab = (struct rt_balloonslab *)begin_addr;

    rt_list_init(&(_current_slab->node));
    _current_slab->slab = 
        rt_slab_init("slab", begin_addr + sizeof(struct rt_balloonslab), _init_slab_size);
    RT_ASSERT(_current_slab->slab != RT_NULL);

    return _current_slab->slab;
}

rt_err_t rt_balloonslab_detach(rt_slab_t slab)
{
    struct rt_balloonslab *iter;

    rt_list_for_each_entry(iter, &(_current_slab->node), node)
    {
        rt_list_remove(&(iter->node));
        rt_slab_detach(iter->slab);
        rt_pages_free(iter, _init_slab_size_bits);
    }

    return RT_EOK;
}

void *rt_balloonslab_alloc(rt_size_t size)
{
    void *ret = RT_NULL;
    struct rt_balloonslab *iter;

    rt_list_for_each_entry(iter, &(_current_slab->node), node)
    {
        ret = rt_slab_alloc(_current_slab->slab, size);
        if (ret != RT_NULL)
        {
            _current_slab = iter;
            return ret;
        }
    }

    struct rt_balloonslab *new_slab = rt_pages_alloc(_init_slab_size_bits);
    if (new_slab == RT_NULL)
        return RT_NULL;

    rt_list_insert_after(&(_current_slab->node), &(new_slab->node));
    _current_slab = new_slab;
    rt_slab_init("slab", _current_slab->slab, _init_slab_size);

    return rt_slab_alloc(_current_slab->slab, size);
}

void *rt_slab_realloc(void *ptr, rt_size_t size)
{
    /**
     * buddy system would alloc slab begin with _init_balloon_size aligned address,
     * so we can find the corresponding slab according to the address.
     **/
    struct rt_balloonslab *balloonslab = 
        (struct rt_balloonslab *)RT_ALIGN_DOWN((rt_size_t)ptr, _init_balloon_size);

    return rt_slab_realloc(balloonslab->slab, size);
}

void rt_balloonslab_free(void *ptr)
{
    struct rt_balloonslab *balloonslab = 
        (struct rt_balloonslab *)RT_ALIGN_DOWN((rt_size_t)ptr, _init_balloon_size);

    rt_slab_free(balloonslab->slab, ptr);

    if (unlikely(balloonslab->slab->used == 0))
    {
        rt_list_remove(&(balloonslab->node));
        rt_slab_detach(balloonslab->slab);
        rt_pages_free(balloonslab, _init_slab_size_bits);
    }
}

void rt_balloonslab_info(rt_size_t *total, rt_size_t *used, rt_size_t *max_used)
{
    struct rt_balloonslab *iter;

    *total = *used = *max_used = 0;

    rt_list_for_each_entry(iter, &(_current_slab->node), node)
    {
        *total      += iter->slab->total;
        *used       += iter->slab->used;
        *max_used   += iter->slab->max;
    }
}
