/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-10-12     zmshahaha    the first version
 */

#ifndef __MM_BALLOONSLAB_H__
#define __MM_BALLOONSLAB_H__

#include <rtdef.h>

struct rt_balloonslab
{
    struct rt_list_node node;
    rt_slab_t slab;
};

rt_slab_t rt_balloonslab_init(void *begin_addr, rt_size_t size);
rt_err_t rt_balloonslab_detach(rt_slab_t slab);
void *rt_balloonslab_alloc(rt_size_t size);
void *rt_balloonslab_realloc(void *ptr, rt_size_t size);
void rt_balloonslab_free(void *ptr);
void rt_balloonslab_info(rt_size_t *total, rt_size_t *used, rt_size_t *max_used);

#endif /* __MM_BALLOONSLAB_H__ */
