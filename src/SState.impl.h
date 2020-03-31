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

#ifndef __SSTATE_HPP
#define __SSTATE_HPP

#include "Common.h"
#include "Array.impl.h"
#include "SRule.impl.h"

namespace sartparser
{
namespace impl
{

class SState
{
public:
    enum Label {UNKNOWN = 0, SCANNED, PREDICTED, COMPLETED};

    SState();
    SState(const SState& rS);

    SState& operator=(SState& rS);

    //Getters
    KTokItem   GetFirst()           const { return rule_.GetFirst(); }
    Label      GetLabel()           const { return label_; }
    size_t     GetK()               const { return k_; }
    size_t     GetDot()             const { return dot_; }
    KTokenPtr  GetLHS()             const { return &lhs_; }
    TokItem    GetAfterDot()              { return rule_.Get(dot_); }
    KTokItem   GetAfterDot()        const { return rule_.Get(dot_); }
    Real       GetProb()            const { return P;    }
    Real       GetAlpha()           const { return Alpha; }
    Real       GetBeta()            const { return Beta;  }
    Real       GetGamma( )          const { return Gamma; }
    Real       GetV()               const { return V;     }
    Real       GetLowMark()         const { return pLowMark; }
    Real       GetHiMark()          const { return pHiMark ; }
    size_t     GetChildCount()      const { return Children.GetCount(); }
    SStatePtr  GetChild( size_t i )       { return Children.Get(i); }
    KSStatePtr GetChild( size_t i ) const { return Children.Get(i); }

    // Is? methods
    bool IsUnit()      const { return rule_.IsUnit(); }
    bool IsFinished()  const { return finished_; }
    bool IsCompleted() const { return label_ == COMPLETED; }
    bool IsScanned()   const { return label_ == SCANNED; }
    bool IsPredicted() const { return label_ == PREDICTED; }
    bool IsKnown()     const { return label_ != UNKNOWN;  }

    // Setters
    void   SetLabel(Label label)      { label_ = label; }
    void   SetRule( const SRule& r )   { rule_ = r; }
    void   SetLHS( const Token& tok ) { lhs_ = tok; }
    void   SetK( size_t AK )          { k_ = AK; }
    Status SetDot( size_t ADot )      { dot_ = ADot; return CheckDot(); }
    void   SetProb( Real AP )         { P = AP; }
    void   SetAlpha( Real AnAlpha )   { Alpha = AnAlpha; }
    void   SetBeta( Real ABeta )      { Beta  = ABeta; }
    void   SetGamma( Real AGamma )    { Gamma = AGamma; }
    void   SetV( Real AV )            { V = AV;}
    void   SetLowMark( Real AMark )   { pLowMark = AMark; }
    void   SetHiMark( Real AMark  )   { pHiMark = AMark; }

    //RHS related functions
    void   AddToken( const Token& t )   { rule_.AddToken(t); }
    Status AddBeforeVar(const Token& t) { return rule_.AddBeforeVar(t); }

    //Dot related functions
    Status AdvanceDot() { dot_++; return CheckDot(); }
    Status CheckDot();
    Status ReplaceAfterDot(const Token &pT);

    //Children related functions
    Status    AddChild  (SStatePtr pS)           {return Children.Add(pS);}
    Status    AddChildAt(size_t i, SStatePtr pS) {return Children.AddAt(i, pS);}
    SStatePtr RemoveChild (size_t i)             {return Children.Remove(i);}
    void      RemoveChildren();

    //Comparison method
    bool sameKDotAndProd(const SState& rS) const
    {
        return (
            k_ == rS.k_ &&
            dot_ == rS.dot_ &&
            lhs_.SameName(rS.lhs_) &&
            rule_.SameTokens(rS.rule_) );
    }

private:
    SRule rule_;
    Token lhs_;
    size_t k_;
    size_t dot_;
    bool finished_;
    Label label_;
    Real P;
    Real Alpha;
    Real Beta;
    Real Gamma;
    Real V;

    Real pLowMark;
    Real pHiMark;

    Array<SState> Children;
};


}// end of impl namespace
}// end of sartparser namespace

#endif
