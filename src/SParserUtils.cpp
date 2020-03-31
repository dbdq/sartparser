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

#include "SParserUtils.impl.h"
#include "SState.impl.h"

#include <cmath>

using namespace sartparser;
using namespace impl;


//==============================================================================
// PARSE PROBABILITY METHODS
//==============================================================================
ParseProbability::ParseProbability()
    : raw(0.0)
    , scaled(0.0)
    , scaleLength(0)
{
}


ParseProbability::ParseProbability(Real probability,
                                   int length,
                                   bool logSpace)
    : raw( (logSpace)? std::exp(probability) : probability )
    , scaled( (logSpace) ? std::exp(probability/length)
                         : std::pow(probability, 1.0/length) )
    , scaleLength( (length > 0) ? static_cast<size_t>(length) : 0 )
{
}

bool ParseProbability::isValid() const
{
    return scaleLength > 0;
}

//==============================================================================
// PARSE TREE IMPL
//==============================================================================
struct ParseTree::Impl
{
    std::string lhs;
    StringVector rhs;
    Real alpha;
    Real gamma;
    Real v;
    size_t k;
    Real highMark;
    Real lowMark;
    std::vector<ParseTree> children;
};


//==============================================================================
// PARSE TREE METHODS
//==============================================================================
ParseTree::ParseTree()
    :pimpl_(new Impl())
{
}


ParseTree::ParseTree(Impl *pimpl)
    :pimpl_(pimpl)
{
}

bool ParseTree::operator==(const ParseTree &other) const
{
    return ( pimpl_->lhs == other.pimpl_->lhs &&
             pimpl_->rhs == other.pimpl_->rhs  &&
             pimpl_->alpha == other.pimpl_->alpha &&
             pimpl_->v == other.pimpl_->v &&
             pimpl_->k == other.pimpl_->k &&
             pimpl_->highMark == other.pimpl_->highMark &&
             pimpl_->lowMark == other.pimpl_->lowMark &&
             pimpl_->children == other.pimpl_->children );
}

ParseTree::ParseTree(const ParseTree& p)
    : pimpl_(new Impl(*p.pimpl_))
{
}

ParseTree& ParseTree::operator=(const ParseTree& p)
{
    if (this == &p)
        return *this;

    delete pimpl_;
    pimpl_ = new Impl(*p.pimpl_);

    return *this;
}

ParseTree::~ParseTree()
{
    delete pimpl_;
}

const std::string& ParseTree::lhs() const
{
    return pimpl_->lhs;
}

const StringVector& ParseTree::rhs() const
{
    return pimpl_->rhs;
}

Real ParseTree::alpha() const
{
    return pimpl_->alpha;
}

Real ParseTree::gamma() const
{
    return pimpl_->gamma;
}

Real ParseTree::v() const
{
    return pimpl_->v;
}

size_t ParseTree::k() const
{
    return pimpl_->k;
}

Real ParseTree::highMark() const
{
    return pimpl_->highMark;
}

Real ParseTree::lowMark() const
{
    return pimpl_->lowMark;
}

const std::vector<ParseTree>& ParseTree::children() const
{
    return pimpl_->children;
}

//==============================================================================
// PARSE TREE FROM STATE
//==============================================================================
ParseTree ParseTreeUtil::ParseTreeFromState(const SState& s)
{
    ParseTree::Impl* pimpl = new ParseTree::Impl();

    pimpl->lhs = s.GetLHS()->GetName();
    pimpl->alpha = s.GetAlpha();
    pimpl->gamma = s.GetGamma();
    pimpl->v = s.GetV();
    pimpl->k = s.GetK();
    pimpl->highMark = s.GetHiMark();
    pimpl->lowMark = s.GetLowMark();

    KTokItem tok = s.GetFirst();
    while(tok)
    {
        pimpl->rhs.push_back( tok.GetToken()->GetName() );
        tok = tok.GetNext();
    }

    for( size_t i = 0; i < s.GetChildCount(); ++i)
    {
        pimpl->children.push_back( ParseTreeFromState( *s.GetChild(i)) );
    }

    return ParseTree(pimpl);
}

//==============================================================================
// VITERBI PARSE CONSTRUCTORS
//==============================================================================

ViterbiParse::ViterbiParse()
    : terminals()
    , probability()
    , parseTree()
{
}

ViterbiParse::ViterbiParse(const StringVector &symbols,
                           const sartparser::ParseProbability &probability,
                           const ParseTree& parseTree)
    : terminals(symbols)
    , probability(probability)
    , parseTree(parseTree)
{
}

