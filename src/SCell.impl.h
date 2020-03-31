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

#ifndef __SCELL_HPP
#define __SCELL_HPP

#include "Common.h"
#include "SState.impl.h"
#include <set>

namespace sartparser
{
namespace impl
{

//Support for SCell Scan function
struct TokenSorter:
        public std::binary_function<const Token&, const Token&, bool>
{
    bool operator()(const Token& a, const Token& b)
    {
        return a.GetName() < b.GetName();
    }
};

typedef std::set<Token, TokenSorter> Line;

class SCell
{
public:
    typedef SState StateType;

    SCell(bool partial = false);
    virtual ~SCell();

    void Init(SGrammar &g);
    void Init(const SGrammar &g);

    Status Predict(const SGrammar &G);
    SCellPtr Scan(const Line& tokens);
    Status Complete(const SGrammar& sg);
    Real Filter(const SGrammar &G, SStatePtr pNewS, SStatePtr pS);
    bool Prune(SStatePtr pS);

    void SetPartial (bool partial);
    bool GetPartial () const;

    Real GetHigh () const;
    void  SetHigh (Real high);

    size_t GetI() const;
    void  SetI(size_t index);

    SStatePtr MakeNewState();

    size_t GetStateCount() const;
    KSStatePtr GetState(size_t i) const;
    Status AddState(
            SStatePtr pState,
            bool AddAlpha = false,
            bool AddGamma = false,
            bool Sorted = false);

private:
    Status Scan(const Token &pT, SCellPtr pCell);


    Array<StateType> States;

    SCellPtr NextCell;
    SCellPtr PrevCell;
    size_t I;

    bool partial_;
    Real highMark_;
    bool highMarkSet_;

    friend class CellUtils;
};



}// end of impl namespace
}// end of sartparser namespace
#endif
