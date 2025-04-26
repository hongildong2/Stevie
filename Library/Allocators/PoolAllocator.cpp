#include "pch.h"
#include "PoolAllocator.h"


struct Node
{
	Node* next;
};

PoolAllocator::PoolAllocator(const size_t blockSize, const size_t blockCount)
	: m_buffer(nullptr)
	, m_head(nullptr)
{
	m_buffer = malloc(blockSize * blockCount);
	MY_ASSERT(m_buffer != nullptr);

	// If assigned to one, all of them will be affected.
	union
	{
		void* as_void;
		char* as_char;
		Node* as_self;
	};

	// start from buffer's beginning
	as_void = m_buffer;
	Node* currentNode = as_self;

	for (unsigned int i = 0; i < blockCount; ++i)
	{
		// to next node(block)
		as_char += blockSize;

		currentNode->next = as_self;
		currentNode = as_self; // assign next block to current node.
	}

	currentNode->next = nullptr;
}

PoolAllocator::~PoolAllocator()
{
	free(m_buffer);
	m_buffer = nullptr;
	m_head = nullptr;
}

void* PoolAllocator::Allocate()
{
	// out of memory
	if (m_head == nullptr)
	{
		MY_ASSERT(false);
		return nullptr;
	}
	Node* userPtr = m_head;
	m_head = m_head->next;
	return userPtr;
}

void PoolAllocator::Free(void* const ptr)
{
	Node* returnedPtr = static_cast<Node*>(ptr);
	// TODO :: Range/Alignment Check?
	returnedPtr->next = m_head;

	m_head = returnedPtr;
}
