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

#ifndef CELLUTILS_IMPL_H
#define CELLUTILS_IMPL_H

#include "SState.impl.h"
#include <iostream>

namespace sartparser
{
namespace impl
{

class CellUtils
{
public:
    static SCell* getCellByIndex(SCell* start, size_t index);

    static SCell* getNext(SCell* cell);

    static SCell* getPrev(SCell* cell);

    static void setNext(SCell* cell, SCell* prev);

    static void setPrev(SCell* cell, SCell* prev);

    static void destroyCells(SCell* start, bool backwards );

    static SCell* split(SCell* cell);

    static void dumpCell(SCell* cell, std::ostream& o);
private:
    static void dumpCellState(SCell*, std::ostream&, SState::Label );

    static void dumpState(State&, std::ostream& );
    static void dumpState(SState&, std::ostream& );
};


}// end of impl namespace
}// end of sartparser namespace

#endif // CELLUTILS_IMPL_H
