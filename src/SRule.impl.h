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

#ifndef __SRULE_HPP
#define __SRULE_HPP

#include "Common.h"
#include "Token.impl.h"

#include <algorithm>


namespace sartparser
{
namespace impl
{


class SRule
{
public:
    typedef std::vector<Token> Storage;
    typedef Storage::iterator Iterator;
    typedef Storage::const_iterator ConstIterator;

    SRule();
    ~SRule();
    SRule(const SRule &);
    SRule& operator=(const SRule&);

    void AddToken(const Token&);
    void AddTokenChain(const TokItem&);
    Status AddBeforeVar(const Token &);

    void SetProb(Real);

    TokItem GetFirst();
    TokItem GetLast();
    TokItem Get(size_t pos);
    KTokItem GetFirst() const;
    KTokItem GetLast() const;
    KTokItem Get(size_t pos) const;
    Real GetProb() const;
    bool SameTokens(const SRule&) const;
    bool IsUnit() const;

private:
    Storage data_;
    Real prob_;

    static bool isVar(const Token &);
};

class TokItem
{
public:
    TokItem();
    TokItem(unsigned int, SRule::Storage & );

    void SetToken(const Token &);

    TokenPtr GetToken();
    TokItem GetNext();
    TokItem GetPrevious();

    bool isValid() const;

    operator bool() const;

private:
    unsigned int pos_;
    SRule::Storage* container_;
};

class KTokItem
{
public:
    KTokItem();
    KTokItem(unsigned int, const SRule::Storage& );

    KTokenPtr GetToken() const;
    KTokItem GetNext() const;
    KTokItem GetPrevious() const;

    bool isValid() const;

    operator bool() const;

private:
    unsigned int pos_;
    const SRule::Storage* container_;
};

inline SRule::SRule()
    :data_()
    ,prob_(0.0)
{
}

inline SRule::~SRule()
{
}

inline SRule::SRule(const SRule& r)
    : data_(r.data_)
    , prob_(r.prob_)
{
}


inline SRule& SRule::operator=(const SRule& r)
{
    if (this == &r)
        return *this;

    //Copy data from other rule
    data_ = r.data_;

    //Copy probability
    prob_ = r.prob_;

    return *this;
}

inline void SRule::AddToken(const Token &t)
{
    data_.push_back(t);
}

inline void SRule::AddTokenChain(const TokItem& chain)
{
    for(TokItem ti = chain; ti.isValid(); ti = ti.GetNext() )
    {
        data_.push_back( *ti.GetToken() );
    }
}

inline Status SRule::AddBeforeVar(const Token& t)
{
    if (data_.empty() )
    {
        std::cerr << "ERROR: Variable state has an empty rule." << std::endl;
        return ERR_INVPARAM;
    }

    //Try to find var
    Iterator it;
    it = std::find_if( data_.begin(), data_.end(), SRule::isVar );

    if (it == data_.end() )
    {
        std::cerr << "ERROR: '?' not found in variable state." << std::endl;
        return ERR_INVPARAM;
    }

    //Insert at position (other elements get shifted)
    data_.insert(it, t);

    return OK;
}

inline void SRule::SetProb(Real p)
{
    prob_ = p;
}

inline TokItem SRule::GetFirst()
{
    return TokItem(0, data_);
}

inline TokItem SRule::GetLast()
{
    return TokItem(data_.size() -1, data_);
}

inline TokItem SRule::Get(size_t pos)
{
    return TokItem(pos, data_);
}

inline KTokItem SRule::GetFirst() const
{
    return KTokItem(0, data_);
}

inline KTokItem SRule::GetLast() const
{
    return KTokItem(data_.size() -1, data_);
}

inline KTokItem SRule::Get(size_t pos) const
{
    return KTokItem(pos, data_);
}

inline Real SRule::GetProb() const
{
    return prob_;
}

inline bool SRule::SameTokens(const SRule& r) const
{
    if (data_.size() != r.data_.size())
        return false;

    ConstIterator it = data_.begin();
    ConstIterator it2 = r.data_.begin();

    while (it != data_.end() && it2 != data_.end() )
    {
        if (! it->SameName(*it2) )
            return false;

        ++it;
        ++it2;
    }
    return true;
}

inline bool SRule::IsUnit() const
{
    return ( data_.size() == 1 && data_.at(0).GetType() == Token::NONTERMINAL );
}

inline bool SRule::isVar(const Token &t)
{
    return t.GetName() == "?";
}

inline TokItem::TokItem()
    : pos_(0)
    , container_(NULL)
{
}

inline TokItem::TokItem(unsigned int position, SRule::Storage &container)
    : pos_(position)
    , container_(&container)
{
}

inline void TokItem::SetToken(const Token& t)
{
    if ( isValid() )
    {
        container_->at(pos_) = t;
    }
}

inline TokenPtr TokItem::GetToken()
{
    return ( isValid() ) ? &container_->at(pos_) : NULL;
}

inline TokItem TokItem::GetNext()
{
    return TokItem(pos_+1, *container_);
}

inline TokItem TokItem::GetPrevious()
{
    return TokItem(pos_-1, *container_);
}

inline bool TokItem::isValid() const
{
    return (container_ != NULL) && (pos_ < container_->size() );
}

inline TokItem::operator bool() const
{
    return isValid();
}


inline KTokItem::KTokItem()
    : pos_(0)
    , container_(NULL)
{
}

inline KTokItem::KTokItem(unsigned int position, const SRule::Storage& container)
    : pos_(position)
    , container_(&container)
{
}

inline KTokenPtr KTokItem::GetToken() const
{
    return ( isValid() ) ? &container_->at(pos_) : NULL;
}

inline KTokItem KTokItem::GetNext() const
{
    return KTokItem(pos_+1, *container_);
}

inline KTokItem KTokItem::GetPrevious() const
{
    return KTokItem(pos_-1, *container_);
}

inline bool KTokItem::isValid() const
{
    return (container_ != NULL) && (pos_ < container_->size() );
}

inline KTokItem::operator bool() const
{
    return isValid();
}


}// end of impl namespace
}// end of sartparser namespace

#endif
