/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-10-16     zmshahaha    memory setup
 */

#include <rtdef.h>

void rt_ofw_common_setup(void)
{
    extern char _start;
    rt_size_t kernel_start_aligned = RT_ALIGN_DOWN
}