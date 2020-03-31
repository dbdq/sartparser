/*
 * Copyright (c) 2014 Miguel Sarabia
 * Imperial College London
 *
 * Copyright (c) 1997 Yuri Ivanov
 * MIT Media Laboratory
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */


#ifndef __TIARRAY_HPP
#define __TIARRAY_HPP

#include "Common.h"

namespace sartparser
{
namespace impl
{


template<typename T>
class Array
{
public:
    const static bool NO_DELETE = false;
    const static bool SHOULD_DELETE = true;

    Array(bool AShouldDelete);
    ~Array();

    Status Add(T* pItem);
    Status AddAt(size_t nIndex, T* pItem);
    Status Delete(size_t nIndex);
    T* Remove(size_t nIndex);
    T* Get(size_t nIndex);
    const T* Get(size_t nIndex) const;
    size_t GetCount() const;

private:
    //Arrays cannot be copied  (for safety)
    Array(const Array&);
    Array& operator=(const Array& );

    const bool bShouldDelete;
    std::vector<T*> data;
};

template<typename T>
inline Array<T>::Array(bool AShouldDelete)
    :bShouldDelete(AShouldDelete)
{
}

template<typename T>
inline Array<T>::~Array()
{
    typedef typename std::vector<T*>::iterator Iterator;
    if (bShouldDelete)
    {
        for ( Iterator it = data.begin(); it != data.end(); ++it)
        {
            delete *it;
        }
    }
}

template<typename T>
inline Status Array<T>::Add(T* pItem)
{
    try
    {
        data.push_back(pItem);
    }
    catch(const std::bad_alloc&)
    {
        return ERR_OUTOFMEMORY;
    }

    return OK;
}

template<typename T>
inline Status Array<T>::AddAt(size_t nIndex, T *pItem)
{
    try
    {
        data.insert(data.begin() + static_cast<int>(nIndex), pItem);
    }
    catch(const std::bad_alloc&)
    {
        return ERR_OUTOFMEMORY;
    }

    return OK;
}

template<typename T>
inline Status Array<T>::Delete(size_t nIndex)
{
    T* ptr = Remove(nIndex);
    if (ptr == NULL)
        return ERR_NOTFOUND;

    return OK;
}


template<typename T>
inline T* Array<T>::Remove(size_t nIndex)
{
    if (nIndex >= data.size())
        return NULL;

    T* result = data.at(nIndex);
    data.erase(data.begin() + static_cast<int>(nIndex) );
    return result;
}

template<typename T>
inline T* Array<T>::Get(size_t nIndex)
{
    if (nIndex >= data.size() )
        return NULL;

    return data.at(nIndex);
}


template<typename T>
inline const T* Array<T>::Get(size_t nIndex) const
{
    if (nIndex >= data.size() )
        return NULL;

    return data.at(nIndex);
}

template<typename T>
inline size_t Array<T>::GetCount() const
{
    return data.size();
}


}// end of impl namespace
}// end of sartparser namespace

#endif //__TIARRAY_HPP
