#ifndef HX_TRIE_H
#define HX_TRIE_H

#include "types.h"
#define TRIE_MAX_CHILDREN 26

typedef struct trie_node {
	struct trie_node *children[TRIE_MAX_CHILDREN];
	b8 isEnd;
} trie_node;

trie_node *trie_create(void);
void trie_insert(trie_node *root, const char *key);
b8 trie_search(trie_node *root, const char *key);

#endif
