/*
 * Copyright (c) 2014 Miguel Sarabia
 * Imperial College London
 *
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

#include "CFGrammar.impl.h"

using namespace sartparser;
using namespace impl;


//==============================================================================
// RULE IMPLEMENTATION
//==============================================================================
Rule::Rule()
    : lhs()
    , rhs()
    , probability(0.0)
{
}

Rule::Rule(const std::string& lhs, const StringVector& rhs, Real probability)
    : lhs(lhs)
    , rhs(rhs)
    , probability(probability)
{
}

bool Rule::operator==(const Rule& r)
{
    return lhs == r.lhs && rhs == r.rhs && r.probability == r.probability;
}

//==============================================================================
// IMPL IMPLEMENTATION
//==============================================================================
CFGrammar::Impl::Impl()
    : sg()

{
}

//==============================================================================
// CFGRAMMAR IMPLEMENTATION
//==============================================================================
CFGrammar::CFGrammar()
    : pimpl_(new Impl() )
{
}

CFGrammar::~CFGrammar()
{
    delete pimpl_;
}

Status CFGrammar::addAxiom(const std::string &axiom)
{
    return pimpl_->sg.AddAxiom(axiom);
}

Status CFGrammar::addTerminal(const std::string &terminal)
{
    return pimpl_->sg.AddTerminal(terminal);
}

Status CFGrammar::addNonTerminal(const std::string &nonterminal)
{
    return pimpl_->sg.AddNonTerminal(nonterminal);
}

Status CFGrammar::addRule(
        const std::string &lhs,
        const StringVector &rhs,
        Real probability)
{
    return pimpl_->sg.AddRule(lhs, rhs, probability);
}

Status CFGrammar::addRule(const Rule& r)
{
    return pimpl_->sg.AddRule(r.lhs, r.rhs, r.probability);
}

Status CFGrammar::checkGrammar()
{
    //This will polymorphically dispatch to the correct function
    return pimpl_->sg.CheckGrammar();
}

StringVector CFGrammar::getTerminals() const
{
    StringVector result;

    for (size_t i = 0; i < pimpl_->sg.GetTCount(); ++i)
    {
        const std::string& terminal = pimpl_->sg.GetTByIndex(i)->GetName();
        if (terminal != "")
            result.push_back( terminal );
    }

    return result;
}

StringVector CFGrammar::getNonTerminals() const
{
    StringVector result;

    for (size_t i = 0; i < pimpl_->sg.GetNCount(); ++i)
        result.push_back( pimpl_->sg.GetNByIndex(i)->GetName() );

    return result;
}

const std::string& CFGrammar::getAxiom() const
{
    return pimpl_->sg.GetAxiom()->GetName();
}

Rules CFGrammar::getRules() const
{
    //Use alias for clearer code
    const SGrammar& grammar = pimpl_->sg;
    Rules result;
    for(size_t i = 0; i < pimpl_->sg.GetNCount(); i++)
    {
        KProductionPtr prod = grammar.GetProduction( grammar.GetNByIndex(i) );
        if (prod == NULL)
            continue;

        const std::string& lhs = prod->GetLHSName();

        for(size_t j = 0; j < prod->GetRuleCount(); j++)
        {
            Rule newRule;
            newRule.lhs = lhs;

            KSRulePtr currentRule = prod->GetRule(j);

            KTokItem tokItem = currentRule->GetFirst();
            while(tokItem)
            {
                newRule.rhs.push_back( tokItem.GetToken()->GetName() );
                tokItem = tokItem.GetNext();
            }

            newRule.probability = currentRule->GetProb();
            result.push_back(newRule);
        }
    }
    return result;
}
