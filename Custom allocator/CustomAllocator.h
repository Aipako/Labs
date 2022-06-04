#pragma once
#define CUSTOM_ALLOCATOR_DEBUG
//#define CHUNKS_PER_POOL 2
#define BASE_SIZE 1024*1024
//Использование аллокаторов в контейнерах

//CustomAllocator -> std::allocator_traits -> STLcontainer
//								^					
//								|
//					  std::pointer_traits



template<typename T>
class CustomAllocator
{
public:
	struct Chunk
	{
		size_t chunkSize = 0;
		T* chunkBod = nullptr;
		Chunk* next = nullptr;
		Chunk* prev = nullptr;
		bool isFree = true;
	};
private:
	//Указатель на начало пула
	Chunk* poolStartPtr = nullptr;
	//
	friend class CustomAllocator;
	bool isConstructed = false;
public:
	static_assert(!std::is_same_v<T, void>, "Allocator type can't be void");
	//Информация для std::allocator_traits
	using value_type = T;
	//using propagate_on_container_move_assigment = std::true_type;
	//using propagate_on_container_copy_assigment = std::true_type;
	//using propagate_on_container_swap = std::true_type;



	//аллокатор не имеет типа
	using is_always_equal = std::true_type;

	//конструктор аллокатора
	
	CustomAllocator()
	{
		if (!isConstructed)
		{
			poolStartPtr = allocatePool(BASE_SIZE);
			isConstructed = true;
		}
			
	}

	template<typename T>
	CustomAllocator(const CustomAllocator& prevAlloc) noexcept 
	{
		poolStartPtr = prevAlloc.poolStartPtr;
	}
	
	
	//преобразование из другого аллокатора

	template<class U>
	CustomAllocator(const CustomAllocator<U>& prevAlloc) noexcept 
	{
		poolStartPtr = reinterpret_cast<Chunk*>(const_cast<CustomAllocator<U>::Chunk*>(prevAlloc.poolStartPtr));
	}
	

	template <class U> struct rebind 
	{
		typedef CustomAllocator<U> other;
	};
	//деструктор аллокатора
	//~CustomAllocator();

	//Базовая функция
	T* allocate(size_t size)
	{
		
		if (poolStartPtr == nullptr)
		{
			//throw std::bad_alloc;
		}
		return getPoolChunk(size);
	}
	//Базовая функция
	void deallocate(T* ptr, size_t size)
	{
		flushPoolChunk(ptr, size);
	}
	//функции Construct и Deconstruct не используются, и std::allocator_traits замещает их прямым вызовом конструктора и деструктора
	
private:



	
	T* getPoolChunk(size_t size)
	{
		Chunk* currentChunk = poolStartPtr;
		while (currentChunk != nullptr)
		{
			if (size > currentChunk->chunkSize || !currentChunk->isFree)
			{
				currentChunk = currentChunk->next;
			}
			else
			{
				break;
			}
		}
		//if(currentChunk == nullptr)
			//throw std::bad_alloc;
		
		//страшная конструкция из указателей и кастов

		//дробим чанк только если мы можем вместить в него ещё один заголовок чанка
		if (currentChunk->chunkSize > size * sizeof(T) + sizeof(Chunk) + 1)
		{
			Chunk* nextChunk = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(currentChunk) + sizeof(Chunk) + size * sizeof(T));
			nextChunk->next = currentChunk->next;
			currentChunk->next = nextChunk;
			nextChunk->chunkSize = currentChunk->chunkSize + size * sizeof(T) + 2 * sizeof(Chunk);
			currentChunk->chunkSize = size * sizeof(T);
			currentChunk->chunkBody = reinterpret_cast<T*>(reinterpret_cast<char*>(currentChunk) + sizeof(Chunk));
			nextChunk->prev = currentChunk;
			currentChunk->isFree = false;
		}
		else
		{
			currentChunk->isFree = false;
		}
		

		#ifdef CUSTOM_ALLOCATOR_DEBUG
		std::cout << "Allocated size: " << size * sizeof(T) << " bytes at adress " << currentChunk << std::endl;
		#endif

		return reinterpret_cast<T*>(currentChunk->chunkBody);
	}

	void flushPoolChunk(T* ptr, size_t size)
	{
		Chunk* currentChunk = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(ptr) - sizeof(Chunk));
		currentChunk->isFree = true;
		//Оптимизация хранения чанка в пуле с следующим чанком
		if (currentChunk->next != nullptr && currentChunk->next->isFree == true)
		{
			
			
			currentChunk->chunkSize += sizeof(Chunk) + currentChunk->next->chunkSize;
			if (currentChunk->next->next != nullptr)
				currentChunk->next->next->prev = currentChunk;
			currentChunk->next = currentChunk->next->next;
		}
		
		//Оптимизация хранения чанка в пуле с предыдущим чанком
		if (currentChunk->prev != nullptr && currentChunk->prev->isFree == true)
		{
			Chunk* prevChunk = currentChunk->prev;

			prevChunk->next = currentChunk->next;
			prevChunk->chunkSize += sizeof(Chunk) + currentChunk->chunkSize;
			if (currentChunk->next != nullptr)
				currentChunk->next->prev = prevChunk;
		}

		#ifdef CUSTOM_ALLOCATOR_DEBUG
			std::cout << "Deallocated size: " << size * sizeof(T) << " bytes at adress " << reinterpret_cast<Chunk*>(reinterpret_cast<char*>(ptr) - sizeof(Chunk)) << std::endl;
		#endif
	}

	
	Chunk* allocatePool(size_t poolSize)
	{

		//Указатель на первый чанк пула - он же сам пул
		Chunk* poolBegin = reinterpret_cast<Chunk*>(malloc(poolSize + sizeof(Chunk)));
		poolBegin->chunkSize = poolSize;
		poolBegin->next = nullptr;
		poolBegin->prev = nullptr;
		poolBegin->chunkBody = reinterpret_cast<T*>(reinterpret_cast<char*>(poolBegin) + sizeof(Chunk));
		return poolBegin;
	}


};

//Базовая функция сравнения
template<class T>
bool operator ==(CustomAllocator<T> const&, CustomAllocator<T> const&)
{
	return true;
}

//Базовая функция сравнения
template<class T>
bool operator !=(CustomAllocator<T> const&&, CustomAllocator<T> const&)
{
	return false;
}

