#ifndef _SKY_AVL_H_
#define _SKY_AVL_H_

#include "macro_def.h"
#include "sky_list.h"

/*
 * task->mm->mmap_avl is the AVL tree corresponding to task->mm->mmap
 * or, more exactly, its root.
 * A vm_area_struct has the following fields:
 *   vm_avl_left     left son of a tree node
 *   vm_avl_right    right son of a tree node
 *   vm_avl_height   1+max(heightof(left),heightof(right))
 * The empty tree is represented as NULL.
 */

/* Since the trees are balanced, their height will never be large. */
#define avl_maxheight     41    /* why this? a small exercise */
#define heightof(tree)    ((tree) == NULL ? 0 : (tree)->avl_height)

typedef struct tavl_node
{
    VOID                *key;
    VOID                *owner;/*the owner of this node*/    
    SWORD32             avl_height;    
    struct tavl_node    *avl_left;
    struct tavl_node    *avl_right;
    LIST_NODE           avl_node_list;/*the pKey is owner*/
}TAVL_NODE;
    
typedef struct tavl_head
{
    VOID         *owner;/*the owner of this node*/
    TAVL_NODE    *root;    
    SWORD32      avl_node_num;
    LIST_NODE    avl_node_list_head;/*the pKey is NULL*/
}TAVL_HEAD;

extern SWORD32 __avl_key_cmp_string(VOID *new_key, VOID *avl_node_key);
extern SWORD32 __avl_key_cmp_word32(VOID *new_key, VOID *avl_node_key);
extern SWORD32 __avl_key_cmp_word32(VOID *new_key, VOID *avl_node_key);
extern SWORD32 __avl_key_cmp_word64(VOID *new_key, VOID *avl_node_key);
extern SWORD32 __avl_key_cmp_macaddr(VOID *new_key, VOID *avl_node_key);

extern TAVL_NODE *tavl_insert(TAVL_HEAD *avl_head,
                              VOID *new_key,
                              TAVL_NODE *(*create_avl_node)(TAVL_HEAD *avl_head, VOID *new_key),
                              SWORD32 (*cmp_avl_key)(VOID *new_key, VOID *avl_node_key),
                              SWORD32 *is_same);
extern TAVL_NODE *tavl_remove(TAVL_HEAD *avl_head,
                              VOID *delete_key, 
                              SWORD32 (*cmp_avl_key)(VOID *delete_key, VOID *avl_node_key));
extern TAVL_NODE *tavl_remove_node(TAVL_HEAD *avl_head,
                                   TAVL_NODE *node_to_delete, 
                                   SWORD32 (*cmp_avl_key)(VOID *delete_key, VOID *avl_node_key));
extern TAVL_NODE *tavl_find(TAVL_HEAD *avl_head,
                            VOID *find_key,
                            SWORD32 (*cmp_avl_key)(VOID *new_key, VOID *avl_node_key),
                            SWORD32 *rtn);

extern VOID tavl_init_head(TAVL_HEAD *head,
                           VOID *owner);

extern SWORD32 tavl_get_node_num(TAVL_HEAD *head);

extern SWORD32 tavl_is_null(TAVL_HEAD *head);

extern VOID *tavl_get_1st_node(TAVL_HEAD *head);

extern VOID *tavl_get_next_node(TAVL_HEAD *head,
                                TAVL_NODE *prev);

extern VOID *tavl_get_prev_node(TAVL_HEAD *head,
                                TAVL_NODE *next);

extern VOID *tavl_get_last_node(TAVL_HEAD *head);

#endif/*_COS_AVL_H_*/

