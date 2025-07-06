#pragma once

struct Node;
class PoolAllocator final
{
public:
	PoolAllocator(const size_t blockSize, const size_t blockCount);
	~PoolAllocator();

	PoolAllocator(PoolAllocator&&) = delete;
	PoolAllocator& operator=(PoolAllocator&&) = delete;

	PoolAllocator(PoolAllocator const&) = delete;
	PoolAllocator& operator=(PoolAllocator&) = delete;

	void* Allocate();
	void Free(void* const ptr);

private:
	void* m_buffer;
	Node* m_head;
};

