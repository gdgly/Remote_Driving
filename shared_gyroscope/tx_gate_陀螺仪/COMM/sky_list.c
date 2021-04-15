/*****************************************************************************/
/** 
* \file       sky_list.c
* \author     yangkun
* \date       2015/02/04
* \version    v 1.0
* \brief      双向链表数据结构
* \note       Copyright (c) 2000-2020  金龙客车制造有限公司
* \remarks    修改日志
******************************************************************************/
#include "sky_list.h"


SWORD32 __cos_list_key_cmp_string(VOID *new_key, VOID *list_node_key)
{
    SWORD32 ret = 0;

    if((NULL == new_key) || (NULL == list_node_key))
    {
        //COS_ASSERT(0);
        return -1;
    }

    ret = STRCMP((CHAR *)new_key, (CHAR *)list_node_key);

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

SWORD32 __cos_list_key_cmp_word32(VOID *new_key, VOID *list_node_key)
{
    if(*((WORD32 *)new_key) > *((WORD32 *)list_node_key))
    {
        return 1;
    }
    else if(*((WORD32 *)new_key) < *((WORD32 *)list_node_key))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

SWORD32 __cos_list_key_cmp_word64(VOID *new_key, VOID *list_node_key)
{
    if(*((WORD64 *)new_key) > *((WORD64 *)list_node_key))
    {
        return 1;
    }
    else if(*((WORD64 *)new_key) < *((WORD64 *)list_node_key))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

SWORD32 __cos_list_key_cmp_sword32(VOID *new_key, VOID *list_node_key)
{
    if(*((SWORD32 *)new_key) > *((SWORD32 *)list_node_key))
    {
        return 1;
    }
    else if(*((SWORD32 *)new_key) < *((SWORD32 *)list_node_key))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

/*****************************************************************************/
/** 
* \author      felix
* \date        2015/02/04
* \brief       将新节点插入到双向链表的指定位置中
* \param[out]  node    输入参数描述 
* \param[out]  prev    输入参数描述 
* \param[out]  next    输入参数描述 
* \return      返回值描述
* \remarks     其它信息
******************************************************************************/
static VOID __node_add(struct list_node * node,
                       struct list_node * prev,
                       struct list_node * next)
{
    if (!node || !prev || !next)
    {
        return;
    }

    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;

    return;
}

/*****************************************************************************/
/** 
* \author      felix
* \date        2015/02/04
* \brief       将节点的前后指针都指向自己，从而完成脱链操作
* \param[out]  prev    输入参数描述 
* \param[out]  next    输入参数描述 
* \return      返回值描述
* \remarks     其它信息
******************************************************************************/
static VOID __node_del(struct list_node * prev,
                       struct list_node * next)
{
    if (!prev || !next)
    {
        return;
    }

    next->prev = prev;
    prev->next = next;

    return;
}

/*****************************************************************************/
/** 
* \author      felix
* \date        2015/02/04
* \brief       在双向链表头部插入新节点
* \param[out]  node    输入参数描述 
* \param[out]  head    输入参数描述 
* \return      返回值描述
* \remarks     其它信息
******************************************************************************/
VOID list_add(struct list_node *node, struct list_node *head)
{
    if (!node || !head)
    {
        return;
    }

    __node_add(node, head, head->next);

    return;
}


/*****************************************************************************/
/** 
* \author      felix
* \date        2015/02/04
* \brief       在双向链表尾部插入新节点
* \param[out]  node    输入参数描述 
* \param[out]  head    输入参数描述 
* \return      返回值描述
* \remarks     其它信息
******************************************************************************/
VOID list_add_tail(struct list_node *node, struct list_node *head)
{
    if (!node || !head)
    {
        return;
    }

    __node_add(node, head->prev, head);

    return;
}


/*****************************************************************************/
/** 
* \author      felix
* \date        2015/02/04
* \brief       将节点从链表中删除
* \param[out]  node    输入参数描述 
* \return      返回值描述
* \remarks     其它信息
******************************************************************************/
VOID list_del(struct list_node *node)
{
    if (!node)
    {
        return;
    }

    __node_del(node->prev, node->next);
    LIST_NODE_INIT(node, node->pKey);

    return;
}

/*****************************************************************************/
/** 
* \author      felix
* \date        2015/02/04
* \brief       链表合并操作，将old_list挂接到new_list的尾部
* \param[out]  old_list    输入参数描述 
* \param[out]  new_list    输入参数描述 
* \return      返回值描述
* \remarks     其它信息
******************************************************************************/
VOID list_merge(struct list_node *old_list, struct list_node *new_list)
{
    struct list_node *first = NULL;
    struct list_node *last  = NULL;
    struct list_node *at    = NULL;

    /*入参判断*/
    if (!old_list || !new_list)
    {
        return;
    }
    
    first = old_list->next;
    last  = old_list->prev;
    at    = new_list->prev;

    if (first != old_list) 
    {
        LIST_NODE_INIT(old_list, old_list->pKey);
        
        first->prev = at;
        at->next = first;

        last->next = new_list;
        new_list->prev = last;
    }

    return;
}

/*****************************************************************************/
/** 
* \author      felix
* \date        2015/02/04
* \brief       判断双向链表是否为空
* \param[out]  node    输入参数描述 
* \return      返回值描述
* \remarks     其它信息
******************************************************************************/
SWORD32 list_is_alone(struct list_node *node)
{
    if (!node)
    {
        return 0;
    }

    if((node->next == node) && (node->prev == node))
        return 1;
    else
        return 0;
}

/*****************************************************************************/
/** 
* \author      felix
* \date        2015/02/04
* \brief       在双向链表中，从链表头开始搜索关键字为key的节点，并返回搜索结果
* \param[out]  list_head    输入参数描述 
* \param[out]  key          输入参数描述 
* \param[out]  pCmpKey      输入参数描述 
* \param[out]  list_key     输入参数描述 
* \param[out]  new_key      输入参数描述 
* \return      若找到，则返回该节点中list_node的指针，若没有找到，则返回比key大的第一个节点；
* \remarks     其它信息
******************************************************************************/
struct list_node *slist_search(struct list_node *list_head,
                                   VOID *key,
                                   SWORD32 (*pCmpKey)(VOID *new_key, VOID *list_key),
                                   SWORD32 *equal)
{
    struct list_node *pRtnList = NULL;/*返回的链表节点指针*/
    struct list_node *pListTmp = NULL;/*临时变量*/
    SWORD32          iCmpRtn   = 0;   /*关键字比较的结果*/
    
    /*入参检查*/
    if((NULL == list_head) || (NULL == key) || (NULL == pCmpKey) || (NULL == equal))
        return NULL;

    *equal   = 0; 
    pListTmp = list_head->next;
    while(list_head != pListTmp)
    {
        iCmpRtn = (*pCmpKey)(key, pListTmp->pKey);
        if(0 >= iCmpRtn)
        {
            if(0 == iCmpRtn)
                (*equal) = 1;
            pRtnList = pListTmp;
            break;
        }
        pListTmp = pListTmp->next;
    }

    return pRtnList;
}

/*****************************************************************************/
/** 
* \author      felix
* \date        2015/02/04
* \brief       从链表头部开始查找，直到找到第一个key值不比自己小的节点，然后将
*              新节点插到此节点前，由此得到一个以关键字作升序排列的队列
* \param[out]  list_head    输入参数描述 
* \param[out]  node         输入参数描述 
* \param[out]  pCmpKey      输入参数描述 
* \param[out]  list_key     输入参数描述 
* \param[out]  new_key      输入参数描述 
* \return      返回值描述
* \remarks     其它信息
******************************************************************************/
SWORD32 slist_insert(struct list_node *list_head,
                         struct list_node *node,
                         SWORD32 (*pCmpKey)(VOID *new_key, VOID *list_key))
{
    struct list_node *pListTmp = NULL;/*临时变量*/
    SWORD32          iCmpRtn   = 0;   /*关键字比较的结果*/
    
    /*入参检查*/
    if((NULL == list_head) || (NULL == node) || (NULL == pCmpKey))
        return 0;

    pListTmp = list_head->next;
    while(list_head != pListTmp)
    {
        iCmpRtn = (*pCmpKey)(node->pKey, pListTmp->pKey);
        if(0 > iCmpRtn)
        {
            /*找到第一个关键字不比自己小的节点,新节点在此结点前插入*/
            break;
        }
        pListTmp = pListTmp->next;
    }
    /*插入新节点*/
    list_add(node, pListTmp->prev);

    return 1;
}

