#ifndef _DYNAMICLIST_H_
#define _DYNAMICLIST_H_

#define MAX_WORD_SIZE 256

typedef struct {
	char **arr;
	int maxlen;
	int len;
} List;

List *new_list(int initial_size);
List *append(List *l, char *to_insert);
void free_list(List *target_list);
#endif


