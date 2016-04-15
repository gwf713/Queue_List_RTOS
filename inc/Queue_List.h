/******************************************************
 *          Queue_List.h
 * @file    Queue_List.h
 * @brief   Head file of list queue module
 * @version 0.0.2
 * @date    09-September-2015
 * @author  Gwf
 *
 * Copyright(c) 2015, Gwf
 * All rights reserved
 *
 *******************************************************/
#ifndef	QUEUE_LIST_H
#define	QUEUE_LIST_H
    /********** INCLUDE FILES **********/
    /* Type define for platform */
    #include "stm32f4xx.h"
    #include "stdio.h"
		#include "os.h"
		
    /* Maxim items in queue */
    #define Node_Remove_EN

    typedef void *ps_Item;

    /*type define of node*/ 
    typedef	struct node{
        ps_Item         pItem; /* Item in node */
        struct	node    *pre; /* Previous node's pointer*/
        struct	node    *next;/* Next node's pointer */
    }ts_Node, *ps_Node;

    /*type define of queue*/
    typedef struct {
        ps_Node	Header;
        ps_Node	Tail;
        uint8_t	Count_Node;
        uint8_t Limit_Queue;
        size_t  Size_Item;
        OS_MUTEX  *pMutex;/* Mutex pointer */
    }ts_Queue, *ps_Queue;

    /********** PUBLIC FUNCTION **********/
    extern ps_Queue Create_Queue(size_t Size_Item, uint8_t Limit);
    extern ErrorStatus Delete_Queue(ps_Queue pq);
    extern FlagStatus Queue_Is_Full(const ps_Queue pq);
    extern FlagStatus Queue_Is_Empty(const ps_Queue pq);
    extern uint8_t Queue_Count(const ps_Queue pq);
    extern ErrorStatus Queue_Write(ps_Item pItem, ps_Queue pq);
    extern ErrorStatus Queue_Read(ps_Item pItem, ps_Queue pq);
    extern ErrorStatus Empty_Queue(ps_Queue pq);
    #ifdef Node_Remove_EN
        extern ErrorStatus Node_Remove(ps_Queue pq, ps_Node pnode);
        extern ps_Node Queue_Find_Node(const ps_Queue pq, FlagStatus (*pfun)(const ps_Node, const ps_Item), const ps_Item pItem);
    #endif /* Node_Remove_EN */
    extern void Queue_Traverse(ps_Queue pq, ErrorStatus (*pfun)(ps_Node pNode, ps_Item Item, ps_Queue pq), ps_Item pItem);
#endif	/*QUEUE_LIST_H*/

