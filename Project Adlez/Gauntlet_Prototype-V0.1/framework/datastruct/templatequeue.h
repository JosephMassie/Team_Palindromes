#pragma once

#include "../mem.h"

template <class DATA_TYPE>
class TemplateQueue
{
public:
	/** the linked-list node used by the queue */
	struct TemplateQueueNode
	{
		DATA_TYPE data;
		TemplateQueueNode * next;

		TemplateQueueNode(DATA_TYPE data):data(data),next(0){}
		TemplateQueueNode():next(0){}
	};
private:
	/** the front of the queue (has been waiting!!!) */
	TemplateQueueNode * head;
	/** the end of the queue (just que'd up!) */
	TemplateQueueNode * tail;
	/** how many elements in the queue */
	int m_size;
public:
	/** create an empty queue with a linked-list architecture */
	TemplateQueue():head(0),tail(0),m_size(0){}

	/** @return how many elements are in the queue */
	inline const int & size() const{
		return m_size;
	}

	/** look at the data at the top of the queue */
	inline DATA_TYPE * peekHead()const{
		if(!head)return 0;
		return &head->data;
	}

	/** @return popped-off head-node of the queue */
	TemplateQueueNode * dequeueNode(){
		if(!head)return 0;
		TemplateQueueNode * result = head;
		head = head->next;
		if(!head)
			tail = 0;
		--m_size;
		return result;
	}

	/** @return popped-off head of the queue */
	DATA_TYPE * dequeue(){
		if(!head)return 0;
		DATA_TYPE * result = &head->data;
		TemplateQueueNode * next = head->next;
		DELMEM(head);
		head = next;
		if(!head)
			tail = 0;
		--m_size;
		return result;
	}

	/** @param node added to the end of the queue */
	void queueNode(TemplateQueueNode * node){
		if(tail)	tail->next = node;
		else		head = node;
		tail = node;
		tail->next = 0;
		++m_size;
	}
	/** @param node added to the end of the queue as a new node */
	inline void queue(DATA_TYPE data){
		queueNode(NEWMEM(TemplateQueueNode(data)));
	}
};
