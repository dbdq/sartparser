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
 *
 *
 * Description:
 *       Production class is implementd as a pair LHS -> RHS,
 *       where LHS is a non-terminal token, and RHS is an array
 *       of Token Item Lists, each of which is a list of tokens,
 *       composing one right hand side production.
 */

#ifndef __PRODUCTION_HPP
#define __PRODUCTION_HPP

#include "Common.h"
#include "Array.impl.h"
#include "SRule.impl.h"

namespace sartparser
{
namespace impl
{

class Production
{
   public:

      Production();
      ~Production();

      RulePtr GetRule(size_t index);
      KSRulePtr GetRule(size_t index) const;
      size_t      GetRuleCount() const {return RHS.GetCount();}
      void     AddRule(RulePtr pList);

              KTokenPtr   GetLHS() const      {return &LHS;}
              const std::string& GetLHSName() const {return LHS.GetName();}
              void     SetLHS(KTokenPtr pT);


              void     SetLabel(const std::string& label);
              const std::string& GetLabel() const { return label_;}
   protected:
      Token   LHS;
      Array<SRule> RHS;
      std::string label_;
};


}// end of impl namespace
}// end of sartparser namespace

#endif




