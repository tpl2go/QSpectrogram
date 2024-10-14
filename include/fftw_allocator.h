#ifndef FFTW_ALLOCATOR_H
#define FFTW_ALLOCATOR_H
#include "fftw3.h"
#include <cstddef>
#include <vector>


template<class T>
struct FftwAllocator
{
    typedef T value_type;

    FftwAllocator() = default;

    template<class U>
    constexpr FftwAllocator(const FftwAllocator <U>&) noexcept {}

    [[nodiscard]] T* allocate(std::size_t n)
    {
        value_type* ptr = reinterpret_cast<value_type*>(fftw_malloc(n* sizeof(value_type)));
        if (!ptr)
        {
            throw std::bad_alloc();
        }
        return ptr;
    }

    void deallocate(T* p, std::size_t n) noexcept
    {
        fftw_free(p);
    }
};

template<class T, class U>
bool operator==(const FftwAllocator <T>&, const FftwAllocator <U>&) { return true; }

template<class T, class U>
bool operator!=(const FftwAllocator <T>&, const FftwAllocator <U>&) { return false; }


typedef std::vector<fftwf_complex, FftwAllocator<fftwf_complex>> fftwf_vector;
typedef std::vector<fftw_complex, FftwAllocator<fftw_complex>> fftw_vector;



#endif // FFTW_ALLOCATOR_H
