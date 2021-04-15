#ifndef _SKY_LIST_H_
#define _SKY_LIST_H_

#include "macro_def.h"

typedef struct list_node
{
    struct list_node *next, *prev;
    VOID   *pKey;
}LIST_NODE;

/* 使用COS_GET_STRUCT_PTR
#define list_entry(ptr, type, member) \
    ((type *)((CHAR *)(ptr)-(((ULONG)(&((type *)64)->member)) - 64))) */

#define LIST_NODE_INIT(ptr, pkey) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
    (ptr)->pKey = (pkey);\
} while (0)

#define LIST_SAFE_SCAN_START(LIST,NODE)     \
{                                           \
    LIST_NODE *_t_next_node_ = NULL;        \
    for(NODE = (LIST)->next;               \
        (NODE)&&((LIST) != NODE)&&((NODE)!= (NODE)->next)&&\
        ((_t_next_node_ = (NODE)->next),1); \
        NODE = _t_next_node_)               \
    {

#define LIST_SAFE_SCAN_END       }}


#define LIST_IS_EMPTY(ptr) ((ptr)->prev == (ptr)->next)

extern SWORD32 __cos_list_key_cmp_string(VOID *new_key, VOID *list_node_key);
extern SWORD32 __cos_list_key_cmp_word32(VOID *new_key, VOID *list_node_key);
extern SWORD32 __cos_list_key_cmp_word64(VOID *new_key, VOID *list_node_key);
extern SWORD32 __cos_list_key_cmp_sword32(VOID *new_key, VOID *list_node_key);
extern VOID list_add(struct list_node *node, struct list_node *head);
extern VOID list_add_tail(struct list_node *node, struct list_node *head);
extern VOID list_del(struct list_node *node);
extern SWORD32 list_is_alone(struct list_node *node);
extern VOID list_merge(struct list_node *old_list, struct list_node *new_list);
extern struct list_node *slist_search(struct list_node *list_head, VOID *key, SWORD32 (*pCmpKey)(VOID *list_key, VOID *new_key), SWORD32 *equal);
extern SWORD32 slist_insert(struct list_node *list_head, struct list_node *node, SWORD32 (*pCmpKey)(VOID *list_key, VOID *new_key));

#endif /*_COS_LIST_H_*/

