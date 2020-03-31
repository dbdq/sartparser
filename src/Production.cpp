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

#include "Token.impl.h"
#include "Production.impl.h"

using namespace sartparser;
using namespace impl;

Production::Production()
    : LHS()
    , RHS(Array<SRule>::SHOULD_DELETE)
    , label_("")
{
}

Production::~Production()
{
}

void Production::SetLabel(const std::string& label)
{
   label_ = label;
}

RulePtr Production::GetRule(size_t index)
{
   return  RHS.Get(index);
}

KSRulePtr Production::GetRule(size_t index) const
{
    return const_cast<Production*>(this)->GetRule(index);
}

void Production::AddRule(RulePtr pList)
{
   // Blindly add.
   RHS.Add(pList);
}

void Production::SetLHS(KTokenPtr pT)
{
   LHS.SetData(pT->GetName(), pT->GetType());
}
