/*****************************************************************************/
/** 
* \file       sky_tavl.c
* \author     skywell
* \date       2020/03/11
* \version     V1.0
* \brief      文件描述
* \note       Copyright (c) 2000-2020  skywell
* \remarks    修改日志
******************************************************************************/
#include "sky_tavl.h"

SWORD32 __avl_key_cmp_string(VOID *new_key, VOID *avl_node_key)
{
    SWORD32 ret = 0;

    if((NULL == new_key) || (NULL == avl_node_key))
    {
        //ASSERT(0);
        return -1;
    }

    ret = STRCMP((CHAR *)new_key, (CHAR *)avl_node_key);

    if(ret < 0)
    {
        return -1;    
    }
    else if(ret > 0)
    {
        return 1;    
    }
    else
    {
        return ret;    
    }
    
}

SWORD32 __avl_key_cmp_word32(VOID *new_key, VOID *avl_node_key)
{
    if(*((WORD32 *)new_key) > *((WORD32 *)avl_node_key))
    {
        return 1;
    }
    else if(*((WORD32 *)new_key) < *((WORD32 *)avl_node_key))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

SWORD32 __avl_key_cmp_sword32(VOID *new_key, VOID *avl_node_key)
{
    if(*((SWORD32 *)new_key) > *((SWORD32 *)avl_node_key))
    {
        return 1;
    }
    else if(*((SWORD32 *)new_key) < *((SWORD32 *)avl_node_key))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

SWORD32 __avl_key_cmp_word64(VOID *new_key, VOID *avl_node_key)
{
    if(*((WORD64 *)new_key) > *((WORD64 *)avl_node_key))
    {
        return 1;
    }
    else if(*((WORD64 *)new_key) < *((WORD64 *)avl_node_key))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

SWORD32 __avl_key_cmp_macaddr(VOID *new_key, VOID *avl_node_key)
{
    SWORD32 rtn = 0;

    rtn = MEMCMP(new_key, avl_node_key, 6);
    if(rtn > 0)
    {
        return 1;    
    }
    else if(rtn < 0)
    {
        return -1;    
    }
    else
    {
        return 0;    
    }
}

/*
 * Consistency and balancing rules:
 * 1. tree->avl_height == 1+max(heightof(tree->avl_left),heightof(tree->avl_right))
 * 2. abs( heightof(tree->avl_left) - heightof(tree->avl_right) ) <= 1
 * 3. foreach node in tree->avl_left: node->key <= tree->key,
 *    foreach node in tree->avl_right: node->key >= tree->key.
 */

/*
 * Rebalance a tree.
 * After inserting or deleting a node of a tree we have a sequence of subtrees
 * nodes[0]..nodes[k-1] such that
 * nodes[0] is the root and nodes[i+1] = nodes[i]->{avl_left|avl_right}.
 */
static VOID tavl_rebalance(TAVL_NODE ***nodeplaces_ptr, SWORD32 count)
{
    for ( ; count > 0 ; count--) {
        TAVL_NODE ** nodeplace = *--nodeplaces_ptr;
        TAVL_NODE * node = *nodeplace;
        TAVL_NODE * nodeleft = node->avl_left;
        TAVL_NODE * noderight = node->avl_right;
        SWORD32 heightleft = heightof(nodeleft);
        SWORD32 heightright = heightof(noderight);
        if (heightright + 1 < heightleft) {
            /*                                                      */
            /*                            *                         */
            /*                          /   \                       */
            /*                       n+2      n                     */
            /*                                                      */
            TAVL_NODE * nodeleftleft = nodeleft->avl_left;
            TAVL_NODE * nodeleftright = nodeleft->avl_right;
            SWORD32 heightleftright = heightof(nodeleftright);
            if (heightof(nodeleftleft) >= heightleftright) {
                /*                                                        */
                /*                *                    n+2|n+3            */
                /*              /   \                  /    \             */
                /*           n+2      n      -->      /   n+1|n+2         */
                /*           / \                      |    /    \         */
                /*         n+1 n|n+1                 n+1  n|n+1  n        */
                /*                                                        */
                node->avl_left = nodeleftright; nodeleft->avl_right = node;
                nodeleft->avl_height = 1 + (node->avl_height = 1 + heightleftright);
                *nodeplace = nodeleft;
            } else {
                /*                                                        */
                /*                *                     n+2               */
                /*              /   \                 /     \             */
                /*           n+2      n      -->    n+1     n+1           */
                /*           / \                    / \     / \           */
                /*          n  n+1                 n   L   R   n          */
                /*             / \                                        */
                /*            L   R                                       */
                /*                                                        */
                nodeleft->avl_right = nodeleftright->avl_left;
                node->avl_left = nodeleftright->avl_right;
                nodeleftright->avl_left = nodeleft;
                nodeleftright->avl_right = node;
                nodeleft->avl_height = node->avl_height = heightleftright;
                nodeleftright->avl_height = heightleft;
                *nodeplace = nodeleftright;
            }
        }
        else if (heightleft + 1 < heightright) {
            /* similar to the above, just interchange 'left' <--> 'right' */
            TAVL_NODE * noderightright = noderight->avl_right;
            TAVL_NODE * noderightleft = noderight->avl_left;
            SWORD32 heightrightleft = heightof(noderightleft);
            if (heightof(noderightright) >= heightrightleft) {
                node->avl_right = noderightleft; noderight->avl_left = node;
                noderight->avl_height = 1 + (node->avl_height = 1 + heightrightleft);
                *nodeplace = noderight;
            } else {
                noderight->avl_left = noderightleft->avl_right;
                node->avl_right = noderightleft->avl_left;
                noderightleft->avl_right = noderight;
                noderightleft->avl_left = node;
                noderight->avl_height = node->avl_height = heightrightleft;
                noderightleft->avl_height = heightright;
                *nodeplace = noderightleft;
            }
        }
        else {
            SWORD32 height = (heightleft<heightright ? heightright : heightleft) + 1;
            if (height == node->avl_height)
                break;
            node->avl_height = height;
        }
    }
}


/**
 * 不允许关键字相同，如果相同，则返回关键字相同的节点。
 */
TAVL_NODE *tavl_insert(TAVL_HEAD *avl_head,
                       VOID *new_key,
                       TAVL_NODE *(*create_avl_node)(TAVL_HEAD *avl_head, VOID *new_key),
                       SWORD32 (*cmp_avl_key)(VOID *new_key, VOID *avl_node_key),
                       SWORD32 *is_same)
{
    TAVL_NODE **ptree     = NULL;
    TAVL_NODE **nodeplace = NULL;
    TAVL_NODE *new_node   = NULL;
    TAVL_NODE *node       = NULL;
    TAVL_NODE **stack[avl_maxheight];
    SWORD32 stack_count = 0;
    SWORD32 cmp = -1;
    TAVL_NODE ***stack_ptr = &stack[0]; /* = &stack[stackcount] */
    LIST_NODE *last_list_node = NULL;

    if (!avl_head || !is_same)
    {
        return NULL;
    }

    *is_same  = 0;
    ptree     = &avl_head->root;
    nodeplace = ptree;
    last_list_node = &avl_head->avl_node_list_head;
    
    for (;;) {
        node = *nodeplace;
        if (node == NULL)
        {
            new_node = (*create_avl_node)(avl_head, new_key);
            if(!new_node)
            {
                /*申请内存失败*/
                return NULL;
            }

            LIST_NODE_INIT(&new_node->avl_node_list, new_node->owner);
            
            if(0 > cmp)
                list_add(&new_node->avl_node_list, last_list_node->prev);
            else
                list_add(&new_node->avl_node_list, last_list_node);
            
            break;
        }
        *stack_ptr++ = nodeplace; stack_count++;
        last_list_node = &node->avl_node_list;
        
        cmp = (*cmp_avl_key)(new_key, node->key);
        if(0 > cmp)
            nodeplace = &node->avl_left;
        else if(0 == cmp)
        {
            *is_same = 1;
            return node;
        }
        else
            nodeplace = &node->avl_right;
    }
    new_node->avl_left = NULL;
    new_node->avl_right = NULL;
    new_node->avl_height = 1;
    *nodeplace = new_node;
    avl_head->avl_node_num++;
    tavl_rebalance(stack_ptr,stack_count);

    return new_node;
}


/*****************************************************************************/
/** 
* \author      felix
* \date        2015/02/05
* \brief       在AVL树中查找并移除关键字为delete_key的节点
*              !!!!注意:此函数中并不释放内存!!!!
* \param[out]  avl_head    输入参数描述 
* \param[out]  delete_key    输入参数描述 
* \param[out]  cmp_avl_key    输入参数描述 
* \param[out]  delete_key    输入参数描述 
* \param[out]  avl_node_key    输入参数描述 
* \return      返回值描述
* \remarks     其它信息
******************************************************************************/
TAVL_NODE *tavl_remove(TAVL_HEAD *avl_head,
                       VOID *delete_key, 
                       SWORD32 (*cmp_avl_key)(VOID *delete_key, VOID *avl_node_key))
{
    TAVL_NODE **ptree      = NULL;
    TAVL_NODE **nodeplace  = NULL;
    TAVL_NODE *node_to_delete = NULL;
    TAVL_NODE *node        = NULL;
    TAVL_NODE **stack[avl_maxheight];
    SWORD32 stack_count = 0;
    SWORD32 cmp = -1;
    TAVL_NODE ***stack_ptr = &stack[0]; /* = &stack[stackcount] */
    TAVL_NODE **nodeplace_to_delete = NULL;

    if (!avl_head)
    {
        return NULL;
    }
    
    ptree     = &avl_head->root;
    nodeplace = ptree;
    
    for (;;) {
        node = *nodeplace;
#ifdef AVL_DEBUG
        if (node == NULL) {
            /* what? node_to_delete not found in tree? */
            printf("avl_remove: node to delete not found in tree\n");
            return NULL;
        }
#endif
        *stack_ptr++ = nodeplace; stack_count++;
        
        cmp = (*cmp_avl_key)(delete_key, node->key);
        if (0 == cmp)
        {
            node_to_delete = node;
            break;
        }
        else if((-2 == cmp) || (-1 == cmp))
            nodeplace = &node->avl_left;
        else if((2 == cmp) || (1 == cmp))
            nodeplace = &node->avl_right;
        else
            return NULL;
    }
    nodeplace_to_delete = nodeplace;
    /* Have to remove node_to_delete = *nodeplace_to_delete. */
    if (node_to_delete->avl_left == NULL) {
        *nodeplace_to_delete = node_to_delete->avl_right;
        stack_ptr--; stack_count--;
    } else {
        TAVL_NODE ***stack_ptr_to_delete = stack_ptr;
        nodeplace = &node_to_delete->avl_left;
        for (;;) {
            node = *nodeplace;
            if (node->avl_right == NULL)
                break;
            *stack_ptr++ = nodeplace; stack_count++;
            nodeplace = &node->avl_right;
        }
        *nodeplace = node->avl_left;
        /* node replaces node_to_delete */
        node->avl_left = node_to_delete->avl_left;
        node->avl_right = node_to_delete->avl_right;
        node->avl_height = node_to_delete->avl_height;
        *nodeplace_to_delete = node; /* replace node_to_delete */
        *stack_ptr_to_delete = &node->avl_left; /* replace &node_to_delete->avl_left */
    }

    avl_head->avl_node_num--;
    list_del(&node_to_delete->avl_node_list);
    
    tavl_rebalance(stack_ptr,stack_count);

    return node_to_delete;
}

/*只删除AVL的节点关系,并不删除内存*/
TAVL_NODE *tavl_remove_node(TAVL_HEAD *avl_head,
                            TAVL_NODE *node_to_delete, 
                            SWORD32 (*cmp_avl_key)(VOID *delete_key, VOID *avl_node_key))
{
    TAVL_NODE **ptree      = NULL;
    VOID      *delete_key  = NULL;
    TAVL_NODE **nodeplace  = NULL;
    TAVL_NODE *node        = NULL;
    TAVL_NODE **stack[avl_maxheight];
    SWORD32 stack_count = 0;
    SWORD32 cmp = -1;
    TAVL_NODE ***stack_ptr = &stack[0]; /* = &stack[stackcount] */
    TAVL_NODE **nodeplace_to_delete = NULL;
    SWORD32   node_find    = 0;/*表示找到了与要删节点关键字相同的节点*/

    if (!avl_head || !node_to_delete)
    {
        return NULL;
    }

    delete_key = node_to_delete->key;
    ptree      = &avl_head->root;
    nodeplace  = ptree;
    
    for (;;) {
        node = *nodeplace;
#ifdef AVL_DEBUG
        if (node == NULL) {
            /* what? node_to_delete not found in tree? */
            printf("avl_remove: node to delete not found in tree\n");
            return NULL;
        }
#endif
        *stack_ptr++ = nodeplace; stack_count++;
        
        cmp = (*cmp_avl_key)(delete_key, node->key);
        if (0 == cmp)
        {
            if(node_to_delete == node)
            {
                node_to_delete = node;
                break;
            }
            else
            {
                node_find = 1;
                if(node_to_delete > node)
                    nodeplace = &node->avl_right;
                else
                    nodeplace = &node->avl_left;
            }
        }
        else if(((-2 == cmp) || (-1 == cmp)) && !node_find)
            nodeplace = &node->avl_left;
        else if(((2 == cmp) || (1 == cmp)) && !node_find)
            nodeplace = &node->avl_right;
        else
            return NULL;
    }
    nodeplace_to_delete = nodeplace;
    /* Have to remove node_to_delete = *nodeplace_to_delete. */
    if (node_to_delete->avl_left == NULL) {
        *nodeplace_to_delete = node_to_delete->avl_right;
        stack_ptr--; stack_count--;
    } else {
        TAVL_NODE ***stack_ptr_to_delete = stack_ptr;
        nodeplace = &node_to_delete->avl_left;
        for (;;) {
            node = *nodeplace;
            if (node->avl_right == NULL)
                break;
            *stack_ptr++ = nodeplace; stack_count++;
            nodeplace = &node->avl_right;
        }
        *nodeplace = node->avl_left;
        /* node replaces node_to_delete */
        node->avl_left = node_to_delete->avl_left;
        node->avl_right = node_to_delete->avl_right;
        node->avl_height = node_to_delete->avl_height;
        *nodeplace_to_delete = node; /* replace node_to_delete */
        *stack_ptr_to_delete = &node->avl_left; /* replace &node_to_delete->avl_left */
    }

    avl_head->avl_node_num--;
    list_del(&node_to_delete->avl_node_list);
    
    tavl_rebalance(stack_ptr,stack_count);

    return node_to_delete;
}
/**
 * 查找find_key所在的节点，
 * *rtn == 0 : 找到且返回找到的那个节点；
 * *rtn == -1: 没有找到且返回比关键字小的最接近的节点；
 * *rtn == 1 : 没有找到且返回比关键字大的最接近的节点；
 * 返回为NULL: 树空；
 */
TAVL_NODE *tavl_find(TAVL_HEAD *avl_head,
                     VOID *find_key,
                     SWORD32 (*cmp_avl_key)(VOID *new_key, VOID *avl_node_key),
                     SWORD32 *rtn)
{
    TAVL_NODE **ptree     = NULL;
    TAVL_NODE *node       = NULL;/*当前节点*/
    TAVL_NODE *node_last  = NULL;/*当前节点的前一个节点*/
    SWORD32   cmp         = -1;

    if (!avl_head || !rtn)
    {
        return NULL;
    }

    ptree     = &avl_head->root;
    node      = *ptree;
    node_last = node;
    *rtn      = 1;

    if(!node)
        return NULL;
    
    while(node && (0 != cmp))
    {
        node_last = node;
        
        cmp = (*cmp_avl_key)(find_key, node->key);
        if(!cmp)
            *rtn = 0;
        else if(0 > cmp)
            node = node->avl_left;
        else if(0 < cmp)
            node = node->avl_right;
    }

    if(!cmp)
        *rtn = 0;
    else if(0 < cmp)
        *rtn = -1;
    else
        *rtn = 1;
    
    return node_last;
}


VOID tavl_init_head(TAVL_HEAD *head,
                    VOID *owner)
{
    if (!head)
    {
        return;
    }

    head->owner = owner;
    head->root  = NULL;
    head->avl_node_num = 0;
    LIST_NODE_INIT(&head->avl_node_list_head, NULL);
}

SWORD32 tavl_get_node_num(TAVL_HEAD *head)
{
    if (!head)
    {
        return -1;
    }

    return head->avl_node_num;
}

SWORD32 tavl_is_null(TAVL_HEAD *head)
{
    if (!head)
    {
        return -1;
    }

    if(0 == head->avl_node_num)
    {
        return 1;
    }    
    else
    {
        return 0;
    }    
}

VOID *tavl_get_1st_node(TAVL_HEAD *head)
{
    LIST_NODE *avl_list_node = NULL;

    if (NULL == head)
    {
        return NULL;
    }
    
    if(tavl_is_null(head))
        return NULL;

    avl_list_node = head->avl_node_list_head.next;

    if (!avl_list_node)
    {
        return NULL;
    }
    return avl_list_node->pKey;
}

VOID *tavl_get_next_node(TAVL_HEAD *head,
                         TAVL_NODE *prev)
{
    LIST_NODE *avl_list_node = NULL;
    if (!head || !prev)
    {
        return NULL;
    }
    avl_list_node = &prev->avl_node_list;

    if(avl_list_node->next == &head->avl_node_list_head)
        return NULL;

    avl_list_node = avl_list_node->next;
    if (!avl_list_node)
    {
        return NULL;
    }
    return avl_list_node->pKey;
}

VOID *tavl_get_prev_node(TAVL_HEAD *head,
                         TAVL_NODE *next)
{
    LIST_NODE *avl_list_node = NULL;
    if (!head || !next)
    {
        return NULL;
    }
    avl_list_node = &next->avl_node_list;

    if(avl_list_node->prev == &head->avl_node_list_head)
        return NULL;

    avl_list_node = avl_list_node->prev;
    if (!avl_list_node)
    {
        return NULL;
    }
    return avl_list_node->pKey;
}


VOID *tavl_get_last_node(TAVL_HEAD *head)
{
    LIST_NODE *avl_list_node = NULL;
    if (!head)
    {
        return NULL;
    }
    if(tavl_is_null(head))
        return NULL;

    avl_list_node = head->avl_node_list_head.prev;
    if (!avl_list_node)
    {
        return NULL;
    }
    return avl_list_node->pKey;
}

