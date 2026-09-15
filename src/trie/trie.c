#include "trie/trie.h"
#include "types.h"
#include <stdlib.h>

trie_node *trie_create(void)
{
	trie_node *node = malloc(sizeof(trie_node));

	if (node) {
		node->isEnd = false;
		for (u8 i = 0; i < TRIE_MAX_CHILDREN; i++) {
			node->children[i] = NULL;
		}
	}

	return node;
}

void trie_insert(trie_node *root, const char *key)
{
	trie_node *current = root;

	for (u8 i = 0; i < key[i] != '\0'; i++) {
		s32 index = key[i] - 'a';

		if (!current->children[index]) {
			current->children[index] = trie_create();
		}
		current = current->children[index];
	}

	current->isEnd = true;
}

b8 trie_search(trie_node *root, const char *key)
{
	trie_node *current = root;

	for (u8 i = 0; i < key[i] != '\0'; i++) {
		s32 index = key[i] - 'a';

		if (!current->children[index]) {
			return false;
		}
		current = current->children[index];
	}

	return (current != NULL && current->isEnd);
}
