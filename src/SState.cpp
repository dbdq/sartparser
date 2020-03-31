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

#include "SState.impl.h"


using namespace sartparser;
using namespace impl;

SState::SState()
    : rule_()
    , lhs_()
    , k_(0)
    , dot_(0)
    , finished_(false)
    , label_(UNKNOWN)
    , P (1.0)
    , Alpha(1.0)
    , Beta(1.0)
    , Gamma(1.0)
    , V (1.0)
    , pLowMark(0.0)
    , pHiMark(0.0)
    , Children(Array<SState>::NO_DELETE)
{
}


SState::SState(const SState& rS)
    : rule_( rS.rule_ )
    , lhs_( rS.lhs_ )
    , k_ ( rS.k_ )
    , dot_( rS.dot_ )
    , finished_( rS.finished_ )
    , label_ (rS.label_)
    , P (rS.P)
    , Alpha(rS.Alpha)
    , Beta(rS.Beta)
    , Gamma(rS.Gamma)
    , V (rS.V)
    , pLowMark(rS.pLowMark)
    , pHiMark(rS.pHiMark)
    , Children(Array<SState>::NO_DELETE)
{
}


SState& SState::operator=(SState& rS)
{
    if ( this == &rS)
        return *this;

    rule_ = rS.rule_;
    lhs_ = rS.lhs_;
    k_ = rS.k_;
    dot_ = rS.dot_;
    label_ = rS.label_;
    P       = rS.P;
    Alpha   = rS.Alpha;
    Beta    = rS.Beta;
    Gamma   = rS.Gamma;
    V       = rS.V;
    pLowMark = rS.pLowMark;
    pHiMark  = rS.pHiMark;

    RemoveChildren();

    for(size_t i = 0; i < rS.GetChildCount(); i++)
        Children.Add(rS.GetChild(i));

    return *this;
}

void SState::RemoveChildren()
{
   for(int i = Children.GetCount() - 1; i >= 0; --i)
      Children.Remove(static_cast<size_t>(i));
}

Status SState::CheckDot()
{
   // Check if at the end of rule
   TokItem pTI = rule_.GetFirst();
   size_t count = 0;
   while(pTI)
   {
      pTI = pTI.GetNext();
      count++;
   }

   if(dot_ > count)
   {
      std::cerr << "ERROR: Dot advanced beyond boundary in "
                << lhs_.GetName() << std::endl;
      return ERR_OUTOFBOUNDS;
   }

   finished_ = (count == dot_);
   return OK;
}

Status SState::ReplaceAfterDot(const Token& pT)
{
   TokItem pTI = rule_.Get(dot_);

   if (!pTI)
   {
       return ERR_NOTFOUND;
   }
   else
   {
       pTI.SetToken(pT);
       return OK;
   }
}

