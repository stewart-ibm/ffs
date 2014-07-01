/*
 * Copyright (c) International Business Machines Corp., 2014
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "assert.h"
#include "misc.h"
#include "slab.h"
#include "heap.h"
#include "bb_trace.h"
#include "memory_leak_detection.h"

/* ======================================================================= */

static const char *__heap_msg[] = {
	"heap: unexpected NULL pointer"
	    "heap: unexpected cache and/or alloc size",
	"heap: unexpected alloc size",
};

#define HEAP_NULL               (__heap_msg[0])
#define HEAP_SIZE               (__heap_msg[1])
#define HEAP_ALLOC              (__heap_msg[2])

/* ======================================================================= */

void heap_dump(heap_t * self, FILE * out)
{
	if (self == NULL)
		return;

	size_t i;

	fprintf(out,
		"*******************************************************************\n");
	for (i = 0; i < self->slab_size; i++)
		if (self->slab[i] != NULL)
			slab_dump(self->slab[i], out);
	fprintf(out,
		"*******************************************************************\n");
}

heap_t *__heap_new(size_t alloc_size, size_t page_size, const char *file,
		   int line)
{
	alloc_size = max(__round_pow2(alloc_size), CACHE_ALLOC_MIN);
	page_size = max(__round_pow2(page_size), CACHE_SIZE_MIN);

	if (alloc_size < CACHE_ALLOC_MIN || CACHE_ALLOC_MAX < alloc_size)
		throw_unexpected(HEAP_SIZE);
	if (page_size < CACHE_SIZE_MIN || CACHE_SIZE_MAX < page_size)
		throw_unexpected(HEAP_SIZE);

	heap_t *self = (heap_t *) MALLOC(sizeof(*self) +
					 sizeof(*self->slab) * page_size /
					 alloc_size);
	assert(self != NULL);

	self->page_size = page_size;
	self->alloc_size = alloc_size;
	self->slab_size = page_size / alloc_size;

	memset(self->slab, 0, self->slab_size * sizeof(*self->slab));

	return self;
}

void __heap_delete(heap_t * self, const char *file, int line)
{
	if (unlikely(self == NULL))
		return;

	size_t i;
	for (i = 0; i < self->slab_size; i++) {
		if (self->slab[i] != NULL) {
			slab_delete(self->slab[i]);
			self->slab[i] = NULL;
		}
	}

	memset(self, 0, sizeof *self);
	FREE(self);
}

void *__heap_alloc(heap_t * self, size_t size, const char *file, int line)
{
	if (unlikely(self == NULL))
		throw_unexpected(HEAP_NULL);

	size = max(align(size, self->alloc_size), self->alloc_size);

	size_t slab_pos = size / self->alloc_size - 1;

	if (unlikely(self->slab_size < slab_pos))
		throw_unexpected(HEAP_ALLOC);

	if (unlikely(self->slab[slab_pos] == NULL))
		self->slab[slab_pos] = slab_new(size, 0);

	return slab_alloc(self->slab[slab_pos]);
}

void __heap_free(heap_t * self, void *ptr, const char *file, int line)
{
	if (unlikely(self == NULL))
		throw_unexpected(HEAP_NULL);

	void *data = (void *)((uint32_t) ptr & ~(self->page_size - 1));
	cache_t *c = (cache_t *) (data + self->page_size - sizeof(cache_t));
	cache_check(c);
	slab_free(cache_slab(c), ptr);
}

/* ======================================================================= */

static uint32_t page_size;

void heap_ctor(void) __constructor;
void heap_ctor(void)
{
	page_size = (const uint32_t)sysconf(_SC_PAGESIZE);
	assert(0 < page_size);
}

void heap_dtor(void) __destructor;
void heap_dtor(void)
{
}

/* ======================================================================= */
