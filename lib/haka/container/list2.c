/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <haka/container/list2.h>

#include <assert.h>
#include <stddef.h>

#ifdef HAKA_DEBUG

static void _list2_check(list2_iter begin)
{
	list2_iter iter = begin;
	int end_found = 0;

	do {
		if (iter->next == iter->prev) {
			assert(iter->is_end || iter->next->is_end);
		}

		iter = list2_next(iter);

		if (iter->is_end) {
			++end_found;
			assert(end_found <= 1);
		}
	} while (iter != begin);

	assert(end_found == 1);
}

static void _list2_check2(list2_iter begin, list2_iter end)
{
	list2_iter iter = begin;

	while (iter != end) {
		assert(!iter->is_end);
		iter = list2_next(iter);
	}
}

#define CHECK_LIST(begin) _list2_check(begin)
#define CHECK_LIST2(begin, end) _list2_check2(begin, end)

#else

#define CHECK_LIST(begin)
#define CHECK_LIST2(begin, end)

#endif

void list2_init(struct list2 *list)
{
	list->head.next = list->head.prev = &list->head;
#ifdef HAKA_DEBUG
	list->head.is_end = true;
	list->head.is_elem = false;
#endif
}

list2_iter list2_insert(list2_iter iter, struct list2_elem *elem)
{
	assert(iter);
	assert(elem);
	assert(!elem->next && !elem->prev);
#ifdef HAKA_DEBUG
	assert(!elem->is_end);
#endif

	CHECK_LIST(iter);

	elem->next = iter;
	elem->prev = iter->prev;

	iter->prev->next = elem;
	iter->prev = elem;

	CHECK_LIST(elem);
	return elem;
}

list2_iter list2_insert_list(list2_iter iter, list2_iter begin, list2_iter end)
{
	list2_iter last;

	CHECK_LIST(iter);
	CHECK_LIST2(begin, end);

	if (begin == end) return iter;

#ifdef HAKA_DEBUG
	assert(!begin->is_end);
#endif

	last = end->prev;

	/* Remove [begin; end[ from the old list and move it inside the new one */
	begin->prev->next = end;
	end->prev = begin->prev;

	iter->prev->next = begin;
	begin->prev = iter->prev;

	last->next = iter;
	iter->prev = last;

	CHECK_LIST(iter);
	CHECK_LIST(end);
	return iter;
}

list2_iter list2_erase(list2_iter list)
{
	list2_iter next;

	CHECK_LIST(list);

	assert(list);

#ifdef HAKA_DEBUG
	assert( list->is_elem);
#endif

	next = list->next;
	list->next->prev = list->prev;
	list->prev->next = list->next;

	list->next = list->prev = NULL;

	if (next != list) CHECK_LIST(next);
	return next;
}

void list2_swap(struct list2 *a, struct list2 *b)
{
	struct list2 tmp;
	const bool a_empty = list2_empty(a);
	const bool b_empty = list2_empty(b);

	CHECK_LIST(list2_begin(a));
	CHECK_LIST(list2_begin(b));

	if (!a_empty) {
		a->head.next->prev = &b->head;
		a->head.prev->next = &b->head;
	}

	if (!b_empty) {
		b->head.next->prev = &a->head;
		b->head.prev->next = &a->head;
	}

	tmp = *b;

	if (!a_empty) *b = *a;
	else b->head.next = b->head.prev = &b->head;

	if (!b_empty) *a = tmp;
	else a->head.next = a->head.prev = &a->head;

	CHECK_LIST(list2_begin(a));
	CHECK_LIST(list2_begin(b));
}
