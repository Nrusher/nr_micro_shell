/**
 * @file      nr_micro_shell_thread.c
 * @author    Nrush
 * @version   V0.1
 * @date      28 Oct 2019
 * *****************************************************************************
 * @attention
 * 
 * MIT License
 * 
 * Copyright (C) 2019 Nrush. or its affiliates.  All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Includes ------------------------------------------------------------------*/
#include "nr_micro_shell.h"
#include <rtthread.h>

#ifdef USING_RT_THREAD

#ifndef RT_USING_POSIX

static struct rt_semaphore nr_shell_rx_sem;
static rt_device_t nr_shell_device;

static rt_err_t nr_shell_rx_ind(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&nr_shell_rx_sem);
	return RT_EOK;
}

void link_nr_shell_to_consle(void)
{
	nr_shell_device = rt_console_get_device();
	if (rt_device_open(nr_shell_device, RT_DEVICE_OFLAG_RDWR |
											RT_DEVICE_FLAG_INT_RX |
											RT_DEVICE_FLAG_STREAM) == RT_EOK)
	{
		rt_device_set_rx_indicate(nr_shell_device, nr_shell_rx_ind);
	}
}

#endif

static char nr_shell_getchar(void)
{
#ifdef RT_USING_POSIX
	return getchar();
#else
	char ch = 0;

	RT_ASSERT(nr_shell_device != RT_NULL);
	while (rt_device_read(nr_shell_device, -1, &ch, 1) != 1)
		rt_sem_take(&nr_shell_rx_sem, RT_WAITING_FOREVER);
	return ch;
#endif
}

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t nr_shell_stack[RT_NR_SHELL_STACKSIZE];
static struct rt_thread nr_shell_thread;
static void nr_shell_thread_entry(void *parameter)
{
	char ch;

#ifndef RT_USING_POSIX
	link_nr_shell_to_consle();
#endif

	rt_kprintf("nr micro shell init sucessfully.\r\n");

	shell_init();

	while (1)
	{
		ch = nr_shell_getchar();
		shell(ch);
	}
}

int rt_nr_shell_system_init(void)
{
	rt_err_t result = RT_EOK;
	rt_thread_t tid;

	tid = &nr_shell_thread;
	result = rt_thread_init(&nr_shell_thread,
							"nrshell",
							nr_shell_thread_entry, RT_NULL,
							&nr_shell_stack[0], sizeof(nr_shell_stack),
							RT_NR_SHELL_THREAD_PRIORITY, 10);
#ifndef RT_USING_POSIX
	rt_sem_init(&nr_shell_rx_sem, "nrrx", 0, 0);
#endif

	if (tid != NULL && result == RT_EOK)
	{
		rt_thread_startup(tid);
	}
	else
	{
		rt_kprintf("nr micro shell init failed.");
	}
	return 0;
}

INIT_APP_EXPORT(rt_nr_shell_system_init);
#endif

/******************* (C) COPYRIGHT 2019 Nrush *****END OF FILE*****************/
