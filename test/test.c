/* Linked: A light-weight linked list library
 * Copyright (c) 2013 Aleksa Sarai
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>

#include <linked.h>

int main(void) {
	struct link_t *table = link_init();
	struct link_iter *ii = NULL;

	/* generate a list */
	printf("generating list\n");
	int to = 100;
	while(to--)
		link_append(table, &to, sizeof(int));

	printf("print list of length %d\n", table->length - 1);
	ii = link_iter_init(table);
	do {
		if(ii->content)
			printf("%d ", *(int *) ii->content);
	} while(!link_iter_next(ii));
	printf("\n");
	link_iter_free(ii);

	printf("shorten list by 25\n");
	link_shorten(table, 25);

	printf("print list of length %d\n", table->length - 1);
	ii = link_iter_init(table);
	do {
		if(ii->content)
			printf("%d ", *(int *) ii->content);
	} while(!link_iter_next(ii));
	printf("\n");
	link_iter_free(ii);

	printf("truncate list to 56\n");
	link_truncate(table, 56);

	printf("print list of length %d\n", table->length - 1);
	ii = link_iter_init(table);
	do {
		if(ii->content)
			printf("%d ", *(int *) ii->content);
	} while(!link_iter_next(ii));
	printf("\n");
	link_iter_free(ii);

	printf("setting everything to 42\n");
	int i;
	for(i = 0; i < table->length; i++) {
		int num = 42;
		link_set(table, i, &num, sizeof(int));
	}

	printf("print list of length %d\n", table->length - 1);
	ii = link_iter_init(table);
	do {
		if(ii->content)
			printf("%d ", *(int *) ii->content);
	} while(!link_iter_next(ii));
	printf("\n");
	link_iter_free(ii);

	link_free(table);
	return 0;
}
