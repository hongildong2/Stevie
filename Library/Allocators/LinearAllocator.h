#pragma once
class LinearAllocator final
{
public:
	LinearAllocator(size_t sizeInBytes);
	~LinearAllocator();

	LinearAllocator(LinearAllocator&&) = delete;
	LinearAllocator& operator=(LinearAllocator&&) = delete;

	LinearAllocator(LinearAllocator const&) = delete;
	LinearAllocator& operator=(LinearAllocator const&) = delete;

	void* Alloc(const size_t sizeInBytes);
	void Free(void* const ptr);
	void Reset();


private:
	size_t m_sizeInBytes;
	void* m_basePtr;
	void* m_currentPtr;
};

