/* Linked: A light-weight linked list library
 * Copyright (C) 2013 Cyphar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#include "linked.h"

static void *pop_container = NULL;

link_t *link_init(void) {
	/* allocate new link */
	link_t *new = malloc(sizeof(link_t));

	/* allocate first link and set length */
	new->chain = malloc(sizeof(link_node));
	new->length = 1;

	/* initialise everything else to zero */
	new->chain->content = NULL;
	new->chain->contentlen = 0;

	new->chain->prev = NULL;
	new->chain->next = NULL;

	return new;
} /* link_init() */

void link_free(link_t *link) {
	if(!link) return;

	/* initialise parent and current links */
	link_node *parent = NULL, *current = link->chain;

	/* go to last link */
	while(current && current->next)
		current = current->next;

	/* go backwards through links and free them */
	while(current) {
		/* save parent */
		parent = current->prev;

		/* free the current link */
		free(current->content);
		free(current);

		current = parent;

		/* ensure no remaining links to freed link exist */
		if(current)
			current->next = NULL;
	}

	/* final setting to zero */
	link->chain = NULL;
	link->length = 0;

	free(link);

	/* freeing the pop_container (to keep valgrind happy) */
	free(pop_container);
	pop_container = NULL;
} /* link_free() */

int link_insert(link_t *link, int pos, void *content, int contentlen) {
	if(pos >= link->length || pos < 0 || !content || contentlen < 1)
		return 1;

	/* initialise current and next links */
	int position = pos;
	link_node *current = link->chain, *next = link->chain->next;

	/* find link to append new link to */
	while(position && current) {
		/* get next link */
		current = current->next;
		position--;
	}

	/* position not found or link->length is inaccurate */
	if(position || !current)
		return 1;

	/* allocate new link information */
	next = current->next;
	current->next = malloc(sizeof(link_node));

	/* allocate and copy content */
	current->next->content = malloc(contentlen);
	memcpy(current->next->content, content, contentlen);
	current->next->contentlen = contentlen;

	/* link up orphan and parent links */
	current->next->prev = current;
	current->next->next = next;

	/* update length */
	link->length++;
	return 0;
} /* link_insert() */

int link_remove(link_t *link, int pos) {
	if(pos >= link->length || pos < 0)
		return 1;

	/* initialise current, previous and next links */
	int position = pos;
	link_node *current = link->chain, *prev = NULL, *next = link->chain->next;

	/* find link to append new link to */
	while(position && current) {
		/* get next link */
		current = current->next;
		position--;
	}

	/* position not found or link->length is inaccurate */
	if(position || !current)
		return 1;

	/* set previous and next links */
	prev = current->prev;
	next = current->next;

	/* free the selected link */
	free(current->content);
	free(current);

	/* link up orphan links, if they exist */
	if(next)
		next->prev = prev;
	if(prev)
		prev->next = next;

	/* update length */
	link->length--;
	return 0;
} /* link_remove() */

void *link_get(link_t *link, int pos) {
	if(pos >= link->length || pos < 0)
		return NULL;

	/* initialise current chain */
	int position = pos;
	link_node *current = link->chain;

	/* find link to append new link to */
	while(position && current) {
		/* get next link */
		current = current->next;
		position--;
	}

	/* position not found or link->length is inaccurate */
	if(position || !current)
		return NULL;

	/* return the found content */
	return current->content;
} /* link_get() */

void *link_pop(link_t *link, int pos) {
	if(pos >= link->length || pos < 0)
		return NULL;

	/* initialise current chain */
	int position = pos;
	link_node *current = link->chain;

	/* find link to append new link to */
	while(position && current) {
		/* get next link */
		current = current->next;
		position--;
	}

	/* position not found or link->length is inaccurate */
	if(position || !current)
		return NULL;

	/* copy the link's content to a temporary variable */
	pop_container = realloc(pop_container, current->contentlen);
	memcpy(pop_container, current->content, current->contentlen);

	/* delete the link and return its content */
	link_remove(link, pos);
	return pop_container;
} /* link_pop() */

int link_truncate(link_t *link, int pos) {
	if(pos >= link->length || pos < 0)
		return 1;

	/* delete the position and everything after it */
	int position = pos;
	while(!link_remove(link, position));

	/* not all of the links where deleted */
	if(link->length >= position)
		return 1;

	return 0;
} /* link_truncate() */

int link_shorten(link_t *link, int num) {
	if(num >= link->length || num < 0)
		return 1;

	/* initialise the current and parent links */
	int left = num;
	link_node *current = link->chain, *parent = link->chain->prev;

	/* find the last link */
	while(current && current->next)
		current = current->next;

	/* go backwards and delete the given number of links */
	while(left && current) {
		/* save parent */
		parent = current->prev;

		/* free current link */
		free(current->content);
		free(current);

		/* update iterator and length */
		left--;
		link->length--;

		/* choose parent link and cut off freed link */
		current = parent;
		current->next = NULL;
	}

	/* not all links deleted */
	if(left)
		return 1;
	return 0;
} /* link_shorten() */

link_iter *link_iter_init(link_t *link) {
	if(!link)
		return NULL;

	/* allocate iterator */
	link_iter *iter = malloc(sizeof(link_iter));

	/* set everything to zero */
	iter->content = NULL;
	iter->internal.link = link;
	iter->internal.node = NULL;

	/* choose first item */
	link_iter_next(iter);
	return iter;
} /* link_iter_init() */

int link_iter_next(link_iter *iter) {
	if(!iter)
		return 1;

	/* select next link */
	if(!iter->internal.node)
		iter->internal.node = iter->internal.link->chain;
	else
		iter->internal.node = iter->internal.node->next;

	/* no links left */
	if(!iter->internal.node)
		return 1;

	/* updated link */
	iter->content = iter->internal.node->content;
	return 0;
} /* link_iter_next() */
