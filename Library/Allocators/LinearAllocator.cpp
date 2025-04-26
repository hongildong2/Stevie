#include "pch.h"
#include "LinearAllocator.h"

LinearAllocator::LinearAllocator(size_t sizeInBytes)
	: m_sizeInBytes(sizeInBytes)
	, m_basePtr(nullptr)
	, m_currentPtr(nullptr)
{
	m_basePtr = malloc(sizeInBytes);
	MY_ASSERT(m_basePtr != nullptr);
	m_currentPtr = m_basePtr;
}

LinearAllocator::~LinearAllocator()
{
	free(m_basePtr);
	m_basePtr = nullptr;
	m_currentPtr = nullptr;
}

void* LinearAllocator::Alloc(const size_t sizeInBytes)
{
	char* currentPtr = static_cast<char*>(m_currentPtr);
	void* const userPtr = currentPtr;

	// allocate
	currentPtr += sizeInBytes;

	// out of memory
	if (currentPtr > (static_cast<char*>(m_basePtr) + m_sizeInBytes))
	{
		MY_ASSERT(false);
		return nullptr;
	}

	m_currentPtr = currentPtr;
	return userPtr;
}

void LinearAllocator::Free(void* const ptr)
{
}



void LinearAllocator::Reset()
{
	m_currentPtr = m_basePtr;
}
