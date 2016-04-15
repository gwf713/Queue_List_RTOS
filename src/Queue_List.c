/******************************************************
 *          Queue_List.c
 * @file    Queue_List.c
 * @brief   Source file of list queue module
 * @version 0.0.2
 * @date    09-September-2015
 * @author  Gwf
 *
 * Copyright(c) 2015, Gwf
 * All rights reserved
 *******************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "Queue_List.h"


/********** PRIVATE FUNCTION **********/
static void Copy_To_Node(ps_Item pItem, ps_Node pNode, size_t Size_Item);
static void Copy_To_Item(ps_Node pNode, ps_Item pItem, size_t Size_Item);
static void Init_Queue(ps_Queue	pq, size_t Size_Item, uint8_t Limit, OS_MUTEX *pMutex);

/********** PUBLIC FUNCTION **********/
/**
 * @brief  Creat new queue
 * @param  None
 * @retval pnew(ps_Queue)
 */
ps_Queue Create_Queue(size_t Size_Item, uint8_t Limit)
{
    ps_Queue pnew;
		OS_MUTEX  *pMutex;
		OS_ERR os_err;
	
    pnew = (ps_Queue)malloc(sizeof(ts_Queue));
		pMutex = (OS_MUTEX  *)malloc(sizeof(OS_MUTEX));
	
    if((pnew != NULL) && (pMutex != NULL))
    {
			/* Create Mutex */
			OSMutexCreate(pMutex, "Mutex of Queue", &os_err);
			Init_Queue(pnew, Size_Item, Limit, pMutex);
    }
    else
		{
			if(pnew != NULL)
			{
				free(pnew);
				pnew = NULL;
			}
			
			if(pMutex != NULL)
			{
				free(pMutex);
			}
		}
			
    return(pnew);
}

/**
 * @brief  Delete the queue
 * @param  pq(ps_Queue)
 * @retval None
 */
ErrorStatus Delete_Queue(ps_Queue pq)
{
    ErrorStatus Status = ERROR;
    OS_ERR os_err;
	
		OSMutexDel(pq ->pMutex, OS_OPT_DEL_ALWAYS, &os_err);
	
    Status = Empty_Queue(pq);
    
    if(Status == SUCCESS)
    {
				free(pq);
				
        return SUCCESS; 
    }
    else
    {
        return ERROR;
    }
}

/**
 * @brief  Check if the queue is full
 * @param  pq(ps_Queue)
 * @retval (Bool)
 */
FlagStatus Queue_Is_Full(const ps_Queue pq)
{
    return((pq ->Count_Node >= (pq ->Limit_Queue)) ? SET : RESET);
}

/**
 * @brief  check if the queue is empty
 * @param  pq(ps_queue)
 * @retval (bool)
 */
FlagStatus Queue_Is_Empty(const ps_Queue pq)
{
    return((pq ->Count_Node == 0) ? SET : RESET);
}

/**
 * @brief  Count the number in the queue
 * @param  pq(ps_queue)
 * @retval (uint8_t): Number of nodes in the queue
 */
uint8_t	Queue_Count(const ps_Queue pq)
{
    return(pq ->Count_Node);
}

/**
 * @brief  Add a new item to the queue
 * @param  pq(ps_queue)
 * @param  item(ts_Item)
 * @retval (ErrorStatus)
 */
ErrorStatus Queue_Write(ps_Item pItem, ps_Queue	pq)
{
    ps_Item pNewItem = NULL;
    ps_Node pnew = NULL;
	  CPU_TS ts;
		OS_ERR  os_err;
		
		OSMutexPend(pq ->pMutex, 0, OS_OPT_PEND_BLOCKING, &ts, &os_err);
	
    if(os_err == OS_ERR_NONE)/* Mutex pend */
    {
        /* If the queue is full, then return false. */
        if(Queue_Is_Full(pq))
        {
            OSMutexPost(pq ->pMutex, OS_OPT_POST_NONE, &os_err);/* Mutex post */
            return ERROR;
        }

        pNewItem = (ps_Item)malloc(pq ->Size_Item);

        if(pNewItem == NULL)
        {
            OSMutexPost(pq ->pMutex, OS_OPT_POST_NONE, &os_err);/* Mutex post */
            return ERROR;
        }

        /* Allocate memory for new node. If no memory is available for node, return false. */
        pnew = (ps_Node)malloc(sizeof(ts_Node));

        if(pnew == NULL)
        {
            free(pNewItem);
            OSMutexPost(pq ->pMutex, OS_OPT_POST_NONE, &os_err);/* Mutex post */
            return	ERROR;
        }
        else
        {
            pnew ->pItem = pNewItem;
        }

        /* Copy item to node. */
        Copy_To_Node(pItem, pnew, pq ->Size_Item);

        /* Insert new node to the queue. */
        pnew ->pre = NULL;

        if(Queue_Is_Empty(pq))
        {
            pq ->Tail = pnew;
            pnew ->next = NULL;
        }
        else
        {
            pq ->Header ->pre = pnew;
            pnew ->next = pq ->Header;
        }

        pq ->Header = pnew;
        pq ->Count_Node++;

        /* Enable queue module */
        OSMutexPost(pq ->pMutex, OS_OPT_POST_NONE, &os_err);/* Mutex post */

        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}

/**
 * @brief  Read a item from a queue
 * @param  pq(ps_queue)
 * @param  pitem(ps_Item): Where the item will be moved to.
 * @retval (bool)
 */
ErrorStatus Queue_Read(ps_Item pitem, ps_Queue pq)
{
    ps_Node pt;
		CPU_TS ts;
		OS_ERR  os_err;
		
		OSMutexPend(pq ->pMutex, 0, OS_OPT_PEND_BLOCKING, &ts, &os_err);
	
    if(os_err == OS_ERR_NONE)/* Mutex pend */
    {
        /* Check if queue is empty. Return false, if Empty. */
        if(Queue_Is_Empty(pq))
        {
            OSMutexPost(pq ->pMutex, OS_OPT_POST_NONE, &os_err);/* Mutex post */
            return ERROR;
        }

        /* Copy item to the destination item. */
        Copy_To_Item(pq ->Tail, pitem, pq ->Size_Item);

        /* The pre node is the new tail node, and free the read node memory. */
        pt = pq ->Tail;
        free(pt ->pItem);
        free(pt);

        pq ->Tail = pq ->Tail ->pre;
        if((pq ->Tail) != NULL)
            pq ->Tail ->next = NULL;/* If the pq ->Tail is Null, what will happen? */

        /* Reduce the count of nodes. */
        pq ->Count_Node--;

        /* If the count of nodes, then clear the header pointer of the queue. */
        if(pq ->Count_Node == 0)
            pq ->Header = NULL;

        OSMutexPost(pq ->pMutex, OS_OPT_POST_NONE, &os_err);/* Mutex post */

        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}

/**
 * @brief  Empty the queue's nodes
 * @param  pq(ps_queue)
 * @retval None
 */
ErrorStatus Empty_Queue(ps_Queue pq)
{
    ps_Item dummy;

    dummy = (ps_Item)malloc(pq ->Size_Item);

    if(dummy == NULL)
    {
        return ERROR;
    }

    /*Clear the nodes with functions "Queue_Read()"*/
    while(!Queue_Is_Empty(pq))
    {
        if(Queue_Read(dummy, pq) == ERROR)
        {
            free(dummy);
            return ERROR;
        }
    }
        
    free(dummy);
    return SUCCESS;
}

#ifdef Node_Remove_EN
    /**
     * @brief  Remove the node with item as same as parameter of item
     * @param  pq(ps_queue)
     * @param  item(ts_Item)
     * @retval None
     */
    ErrorStatus Node_Remove(ps_Queue pq, ps_Node pnode)
    {
        OS_ERR  os_err;
				CPU_TS ts;
			
				OSMutexPend(pq ->pMutex, 0, OS_OPT_PEND_BLOCKING, &ts, &os_err);
	
				if(os_err == OS_ERR_NONE)/* Mutex pend */
        {
            if(pq ->Header == pnode)
            {
                pq ->Header = pnode ->next;
            }

            if(pq ->Tail == pnode)
            {
                pq ->Tail = pnode ->pre;
            }

            /*Delete the node from queue*/
            if(pnode ->pre != NULL)
                pnode ->pre ->next = pnode ->next;/* If there is no previous node, what will happen? */
            if(pnode ->next != NULL)
                pnode ->next ->pre = pnode ->pre;/* If there is no next node, what will happen? */

            free(pnode ->pItem);
            free(pnode);

            (pq ->Count_Node)--;

            OSMutexPost(pq ->pMutex, OS_OPT_POST_NONE, &os_err);/* Mutex post */

            return(SUCCESS);
        }
        else
        {
            return(ERROR);
        }
    }

    /**
     * @brief  Find node in the queue, compare in pfun, 
     *         return the point to the node
     * @param  pq(ps_queue)
     * @param  pfun: compare the node in this function
     * @param  pfun(ps_Item)
     * @retval None
     */
    ps_Node Queue_Find_Node(const ps_Queue pq, FlagStatus (*pfun)(const ps_Node, const ps_Item), const ps_Item pItem)
    {
        ps_Node pnode = pq ->Header;

        if(pnode != NULL)
        {
            if(pfun(pnode, pItem) == SET)
            {
                return pnode;
            }
            else
            {
                pnode = pnode ->next;
            }

            while(pnode != NULL)
            {
                if(pfun(pnode, pItem) == SET)
                {
                    break;
                }
                else
                {
                    pnode = pnode ->next;
                }
            }
        }

        return pnode;
    }
#endif /* Node_Remove_EN */

/**
 * @brief  Traverse the whole queue
 * @param  pq(ps_queue)
 * @param  pfun: the function's pointer this function will call
 * @retval None
 */
void Queue_Traverse(ps_Queue pq, ErrorStatus (*pfun)(ps_Node pNode, ps_Item pItem, ps_Queue pq), ps_Item pItem)
{
    ps_Node pnode = pq ->Header;
    ErrorStatus Result_Function = ERROR;

    while(pnode ->next != NULL)
    {
        Result_Function = (* pfun)(pnode, pItem, pq);
        Result_Function = Result_Function;
        pnode = pnode ->next;
    }
}

/****************	PRIVATE FUNCTION	********************/ 
/**
 * @brief  Initialize the queue
 * @param  pq(ps_Queue)
 * @retval None
 */
static void Init_Queue(ps_Queue	pq, size_t Size_Item, uint8_t Limit, OS_MUTEX *pMutex)
{
		OS_ERR  os_err;
		CPU_TS ts;
	
	  pq ->pMutex = pMutex;
	  OSMutexPend(pq ->pMutex, 0, OS_OPT_PEND_BLOCKING, &ts, &os_err);
  	pq ->Header = pq ->Tail = NULL;
    pq ->Count_Node = 0;
    pq ->Size_Item = Size_Item;
    pq ->Limit_Queue = Limit;
    OSMutexPost(pq ->pMutex, OS_OPT_POST_NONE, &os_err);/* Mutex post */
}

/**
 * @brief  Copy the item to the node
 * @param  item(ts_Item)
 * @param  pNode(ps_Node): Node's pointer
 * @retval None
 */
static void Copy_To_Node(ps_Item pItem, ps_Node pNode, size_t Size_Item)
{
    memcpy(pNode ->pItem, pItem, Size_Item);
}

/**
 * @brief  Copy the node's item to the item
 * @param  pItem(ps_Item): Item's pointer
 * @param  pNode(ps_Node): Node's pointer
 * @retval None
 */
static void Copy_To_Item(ps_Node pNode, ps_Item pItem, size_t Size_Item)
{
    memcpy(pItem, pNode ->pItem, Size_Item);
}
