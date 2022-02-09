/*
 * dynamiclist.c
 *
 * Dynamically sized array that doubles ever expansions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynamic_list.h"

List *new_list(int initial_size) {
    List *l = malloc(sizeof *l);
    l -> maxlen = initial_size;
    l -> len = 0;
    l->arr = malloc(l->maxlen * sizeof(char *));

    for(int i = 0; i < l->maxlen;i++)
        l->arr[i] = malloc(MAX_WORD_SIZE * sizeof(char));
    return l;
}

List *append(List *l, char *to_insert) {

    if(l->len >= l->maxlen) {
        l->maxlen = 2 * l->maxlen;
        l->arr = realloc(l->arr, l->maxlen * sizeof(char *));
        for(int i = l->len; i <l->maxlen; i++) {
            l->arr[i] = malloc(MAX_WORD_SIZE * sizeof(char));
        }
    }
    l->arr[l->len] = to_insert;
    l->len++;

    return l;
}

void free_list(List *target_list)
{
	for(int i = 0; i < target_list ->len; i++) {
		free(target_list->arr[i]);
	}
	free(target_list);	
}
