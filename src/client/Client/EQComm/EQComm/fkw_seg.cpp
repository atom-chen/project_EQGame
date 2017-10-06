/**
 * Word Segmentation
 */
#include "EQCommPCH.h"
#include "fkw_util.h"
#include "fkw_seg.h"

static fkw_atom_iter_t *
atom_iter_init(unsigned int size)
{
	int i;

	fkw_atom_t *atom;
	fkw_atom_iter_t *atom_iter;	

	mc_collector_t *mc;

	mc = NULL;

	atom_iter = (fkw_atom_iter_t *) mc_calloc(&mc, sizeof(fkw_atom_iter_t) );

	atom_iter->size = size;

	atom_iter->worker_list     = NULL;
	atom_iter->worker_list_end = NULL;
	atom_iter->free_list       = NULL;
	atom_iter->mc = mc;

	i = 0;	
	while ((uint)i < size) {
		atom = (fkw_atom_t *) mc_calloc(&(atom_iter->mc), sizeof(fkw_atom_t) );

		atom->value.data = NULL;
		atom->value.len  = 0;
		atom->next       = NULL;

		if (atom_iter->free_list == NULL){
			atom_iter->free_list = atom;
		} else {
			atom->next = atom_iter->free_list;
			atom_iter->free_list = atom;
		}

		i++;
	}

	return atom_iter;
}

static void 
atom_iterator_add(fkw_atom_iter_t *atom_iter, fkw_str_t *atom_value)
{
	fkw_atom_t *atom;
	if (atom_iter->free_list == NULL){
		atom_iter->free_list   = atom_iter->worker_list;
		atom_iter->worker_list = atom_iter->worker_list->next;

		atom_iter->free_list->next = NULL;
	}

	atom = atom_iter->free_list;
	atom_iter->free_list = atom_iter->free_list->next;

	atom->next = NULL;
	atom->value.data = atom_value->data;
	atom->value.len  = atom_value->len;
	atom->value.head_len  = atom_value->head_len;

	if (atom_iter->worker_list == NULL) {
		atom_iter->worker_list     = atom;
		atom_iter->worker_list_end = atom;
	} else {
		atom_iter->worker_list_end->next = atom;
		atom_iter->worker_list_end = atom;
	}
}

static fkw_atom_t *
fkw_atom_iter_pop(fkw_atom_iter_t *atom_iter)
{
	fkw_atom_t *atom;

	if (atom_iter->worker_list == NULL){
		return NULL;
	}

	atom = atom_iter->worker_list;
	atom_iter->worker_list = atom->next;

	atom->next = atom_iter->free_list;
	atom_iter->free_list = atom;

	return atom;
}
//---------以下注释勿删----------------------以下注释勿删-----------勿删-------勿删---------勿删----------勿删------

//--小端匹配（从短到长匹配）
// void fkw_do_seg(fkw_dict_array_t *dict_array, fkw_atom_iter_t *atom_iter, fkw_str_t **text)
// {
// 	unsigned int size, len;
//     unsigned char *string = (*text)->data;
// 	fkw_atom_t *atom;
// 	fkw_word_node_t word_node;
// 	atom = atom_iter->worker_list;
// 	if (!(atom && atom->next)){
// 		//return;
// 	}
// 
// 	size = 1;
// 	len  = atom->value.len; 
// 
// 	size_t head_len = atom->value.head_len;
// 	int i = 0;
// 	while (i <= MAX_WORD_SIZE)
// 	{
// 		if (!atom->next)
// 		{
// 			break;
// 		}
// 		word_node.word = (const char *)atom_iter->worker_list->value.data;
// 		word_node.size = 1;
// 		word_node.len  = atom->value.len;
// 		word_node.hash_key = create_hash_key(word_node.word, len);
// 
// 		if (fkw_dict_search_word(dict_array, &word_node)){
// 			memset(string + head_len, 42, word_node.len);
// 		}
// 		atom = atom->next;
// 	}
// 
// 	atom = atom_iter->worker_list;
// 
// 	while (atom->next){
// 		atom = atom->next;
// 		size += 1;
// 		len  += atom->value.len;
// 
// 		word_node.word = (const char *)atom_iter->worker_list->value.data;
// 		word_node.size = size;
// 		word_node.len  = len;
// 		word_node.hash_key = create_hash_key(word_node.word, len);
// 
//         if (fkw_dict_search_word(dict_array, &word_node)){
//             memset(string + head_len, 42, word_node.len);
// 		}
// 	}
// }

//-------------------------------------------------------------------------------------------
//--大端匹配字符（从长到短匹配）
void fkw_do_seg(fkw_dict_array_t *dict_array, fkw_atom_iter_t *atom_iter, fkw_str_t **text)
{
	unsigned int size, len;
	unsigned char *string = (*text)->data;
	fkw_atom_t *atom;
	fkw_word_node_t word_node;
	atom = atom_iter->worker_list;
	if (!(atom && atom->next)){
		//return;
	}

	int atomLen[MAX_WORD_SIZE];//数组存放的是工作链表中前N个结点的字符的总长
	for (int i = MAX_WORD_SIZE; i > 0; --i)
	{
		atomLen[MAX_WORD_SIZE-i] = get_worklist_atom_len(atom_iter,i);
	}

	size = 1;
	len  = 0; 
	size_t head_len = atom->value.head_len;

	for(int i = 0; i < MAX_WORD_SIZE; ++i)
	{
		size = MAX_WORD_SIZE - i;
		len = atomLen[i];
		word_node.word = (const char *)atom_iter->worker_list->value.data;
		word_node.size = size;
		word_node.len  = len;
		word_node.hash_key = create_hash_key(word_node.word, len);

		if (fkw_dict_search_word(dict_array, &word_node)){
			memset(string + head_len, 42, word_node.len);
		}
	}
}

void fkw_full_seg(fkw_str_t *text, fkw_dict_array_t *dict_array)
{
	unsigned char *start;
	fkw_str_t atom_value;
	fkw_atom_iter_t *atom_iter;
	mc_collector_t *mc;
	atom_iter = atom_iter_init(MAX_WORD_SIZE);

	mc = NULL;

    size_t head_len;

	start = text->data;
	while ((head_len = start - text->data) < text->len) {
		atom_value.data = start;
		atom_value.len  = get_mblen((unsigned char)start[0]);
		atom_value.head_len  = head_len;
		atom_iterator_add(atom_iter, &atom_value);
		
		if (atom_iter->free_list == NULL) {
			fkw_do_seg(dict_array, atom_iter, &text);
		}

		start = start + atom_value.len;
	}

	fkw_atom_iter_pop(atom_iter);

	// free atom iter's memory
	mc_destory(atom_iter->mc);
}

void fkw_free_word_list(fkw_word_list_t **word_list)
{
	mc_destory((*word_list)->mc);
	*word_list = NULL;
}

int get_worklist_atom_len(fkw_atom_iter_t *atom_iter,int size)
{
	int lenth = 0;
	fkw_atom_t* atom = atom_iter->worker_list;
	for (int i = 0; i < size; ++i)
	{
		lenth += atom->value.len;
		atom = atom->next;
	}

	return lenth;
}