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

#ifndef __TOKEN_HPP
#define __TOKEN_HPP

#include "Common.h"



namespace sartparser
{
namespace impl
{


class Token
{
public:
    enum Type {UNKNOWN = 0, TERMINAL, NONTERMINAL};

    Token(const std::string& name = "",
          Type type = Token::UNKNOWN,
          Real prob = 1.0,
          Real high = 0.0,
          Real low  = 0.0);
    Token(const Token& t);
    Token& operator=(const Token& t);
    ~Token();


    const std::string& GetName() const;
    Type GetType () const;
    Real GetProb() const;
    Real GetHigh() const;
    Real GetLow () const;

    void SetName(const std::string& name);
    void SetType (Type type );
    void SetProb(Real prob);
    void SetHigh(Real high);
    void SetLow(Real low);


    void SetData(const std::string& name,
            Type type = Token::UNKNOWN,
            Real prob = 1.0,
            Real high = 0.0,
            Real low  = 0.0);

    bool SameName(const Token& t) const;
    TokenPtr Clone() const;

private:
    std::string name_;
    Type type_;
    Real prob_;
    Real high_;
    Real low_;
};


inline Token::Token(const std::string& name,
                    Type type,
                    Real prob,
                    Real high,
                    Real low)
    : name_(name)
    , type_(type)
    , prob_(prob)
    , high_(high)
    , low_(low)
{
}

inline Token::Token(const Token& t)
    : name_(t.name_)
    , type_(t.type_)
    , prob_(t.prob_)
    , high_(t.high_)
    , low_(t.low_)
{
}

inline Token& Token::operator=(const Token& t)
{
    if (this != &t)
    {
        name_ = t.name_;
        type_ = t.type_;
        prob_ = t.prob_;
        high_ = t.high_;
        low_ =t.low_;
    }
    return *this;
}

inline Token::~Token()
{
}


inline const std::string& Token::GetName() const
{
    return name_;
}

inline Token::Type Token::GetType () const
{
    return type_;
}

inline Real Token::GetProb() const
{
    return prob_ ;
}

inline Real  Token::GetHigh() const
{
    return high_ ;
}

inline Real  Token::GetLow () const
{
    return low_ ;
}

inline void Token::SetName(const std::string& name)
{
    name_ = name;
}

inline void Token::SetType(Type type )
{
    type_  = type;
}

inline void Token::SetProb(Real prob)
{
    prob_ = prob;
}

inline void Token::SetHigh(Real high)
{
    high_ = high;
}

inline void Token::SetLow(Real low)
{
    low_ = low;
}


inline void Token::SetData(
        const std::string& name,
        Type type,
        Real prob,
        Real high,
        Real low)
{
    name_ = name;
    type_ = type;
    prob_ = prob;
    high_ = high;
    low_ = low;
}


inline bool Token::SameName(const Token& t) const
{
    return name_ == t.name_;
}

inline TokenPtr Token::Clone() const
{
    return new Token(name_, type_, prob_, high_, low_);
}


}// end of impl namespace
}// end of sartparser namespace

#endif
