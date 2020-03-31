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

#include "SRule.impl.h"
#include "SState.impl.h"
#include "SCell.impl.h"
#include "SGrammar.impl.h"
#include "CellUtils.impl.h"

#include <cmath>

using namespace sartparser;
using namespace impl;


SCell::SCell(bool partial)
    : States(Array<StateType>::SHOULD_DELETE)
    , NextCell(NULL)
    , PrevCell(NULL)
    , I(0)
    , partial_(partial)
    , highMark_(0.0)
    , highMarkSet_(false)
{
}

SCell::~SCell()
{
}

void SCell::Init(SGrammar& g)
{
    if(partial_)
    {
        g.AddNonTerminal("?");
        g.SetPartial(true);
    }
    //Call const-version
    Init(const_cast<const SGrammar&>(g));
}


void SCell::Init(const SGrammar& g)
{
    SStatePtr pS = MakeNewState();

    pS->AddToken(*g.GetAxiom());
    pS->AddToken(*g.GetTerminal(""));

    pS->SetK(0);
    pS->SetDot(0);
    pS->SetLabel(SState::COMPLETED);
    States.Add(pS);

    // If need to track all the partial derivations,
    // seed with "0: ->.?" also.
    if(partial_)
    {
        KTokenPtr pNewT = g.GetNonTerminal("?");

        SStatePtr pS = MakeNewState();
        pS->AddToken(*pNewT);
        pS->SetK(0);
        pS->SetDot(0);
        pS->SetLabel(SState::COMPLETED);
        States.Add(pS);
    }
}


Status SCell::Predict(const SGrammar& G)
{

    size_t stateCount = States.GetCount();
    for(size_t i = 0; i < stateCount; i++)
    {
        SStatePtr pS = States.Get(i);

        // Get a token right after the dot.
        if(pS->IsFinished())//YAI || pS->IsPredicted())
            continue;

        TokItem pTI = pS->GetAfterDot();
        if(!pTI )
        {
            std::cerr << "ERROR: Invalid state " << I
                      << ": " <<  pS->GetK()
                      << " " << pS->GetLHS()->GetName() << ", Dot = "
                      << pS->GetDot() << std::endl;
            return ERR_NOTFOUND;
        }

        KTokenPtr pT = pTI.GetToken();
        // Skip it if it is a terminal
        if(pT->GetType() == Token::TERMINAL)
            continue;

        // Check if it is a "?"
        if((partial_) && ( pT->GetName() == "?"))
        {
            // Generate predictions for all non-terminals
            size_t NCount = G.GetNCount() - 1; // -1 for "?"
            KTokenPtr pT  = G.GetTerminal("");
            for(size_t j = 0; j < NCount; j++)
            {
                KTokenPtr pN = G.GetNByIndex(j);
                SStatePtr pNewS = MakeNewState();
                pNewS->AddToken( *pN );
                pNewS->SetProb(0.0);
                pNewS->SetDot(0);
                pNewS->SetK(I);
                pNewS->SetLowMark(highMark_);
                pNewS->SetHiMark(highMark_);
                pNewS->SetLHS( *pT );
                pNewS->SetLabel(SState::PREDICTED);

                Real NewAlpha = 0.0;
                pNewS->SetAlpha(NewAlpha);
                pNewS->SetGamma(1.0);
                pNewS->SetV( std::log(1) );

                // Do not need to update Gamma
                if(AddState(pNewS, true, false) == ERR_ALREADYEXISTS)
                    delete pNewS;
            }
            std::cerr << "WARNING: Handling '?' prediction" << std::endl;
            continue;
        }

        // At this point pT is a next nonterminal in the production.
        // We need to iterate through Rl's row corresponding to
        // pT and find all the nonterminals that are in LC relation
        // with it. Then we need to scan the grammar for possible
        // rules that have those non-terminals as LHS and stick them
        // into States computing new alphas and gammas.
        int ZIndex = G.GetNIndex(pT);
        size_t TableSize = G.GetNCount();
        if(partial_)
            TableSize--;

        for(size_t CIndex = 0; CIndex < TableSize; CIndex++)
        {
            Real ClosureProb = G.GetRl(ZIndex, CIndex);
            if(ClosureProb <= 0)	// Closure < 0 - numerical error, ignore
                continue;

            // Get the token which is in LC relationship with pT (pT itself
            // will also be included with weight of at least 1.0).
            KTokenPtr pTC = G.GetNByIndex(CIndex);

            // Find all the productions for it and stick them into
            // States with the dot at 0.
            KProductionPtr pP = G.GetProduction(pTC);
            if(!pP)
            {
                //Should never happen - it is checked while reading grammar
                std::cerr << "WARNING: Nonterminal " << pTC->GetName()
                          << "is missing production" << std::endl;
                continue;
            }

            for(size_t j = 0; j < pP->GetRuleCount(); j++)
            {
                KSRulePtr pR = pP->GetRule(j);
                SStatePtr pNewS = MakeNewState();
                pNewS->SetRule( *pR );
                pNewS->SetProb(pR->GetProb());
                pNewS->SetDot(0);
                pNewS->SetK(I);
                pNewS->SetLowMark(highMark_);
                pNewS->SetHiMark(highMark_);
                pNewS->SetLHS( *pTC );
                pNewS->SetLabel(SState::PREDICTED);

                Real NewAlpha =
                        pS->GetAlpha() * ClosureProb * pNewS->GetProb();
                pNewS->SetAlpha(NewAlpha);
                pNewS->SetGamma(pNewS->GetProb());
                pNewS->SetV( std::log(pNewS->GetProb()) );

                // Do not need to update Gamma
                if(AddState(pNewS, true, false) == ERR_ALREADYEXISTS)
                    delete pNewS;
            }
        }
    }

    return OK;
}

// Returns a new Cell, with the Scanned set filled in
// and adds the cell to a chain.
SCellPtr SCell::Scan(const Line& tokens)
{
    SCellPtr pCell = new SCell(partial_);
    pCell->SetI(GetI() + 1);

    // For each Token in the input bank do the normal scan
    for(Line::const_iterator tok = tokens.begin(); tok != tokens.end(); ++tok)
    {
        if(tok->GetProb() <= 0.0)
            continue;


        if( ! tok->GetName().empty() )
        {
            Real HiMark = tok->GetHigh();
            if( !pCell->highMarkSet_ )
                pCell->SetHigh(HiMark);
        }

        if(Scan(*tok, pCell) != OK)
        {
            delete pCell;
            return NULL;
        }
    }

    CellUtils::setNext(this, pCell);
    CellUtils::setPrev(pCell, this);
    return pCell;
}

Status SCell::Complete(const SGrammar& sg)
{
    for(size_t i = 0; i < States.GetCount(); i++)
    {
        SStatePtr pS = States.Get(i);

        // For each complete state
        if(pS->IsFinished())// && !pS->IsUnit())
        {
#ifdef HEAVY_DEBUG
            printf("i = %d; USING ", i);
            pS->Dump(stdout);
            printf("to complete\n");
#endif 
            // pT is an LHS of a current complete state.
            KTokenPtr pT = pS->GetLHS();
            if(pT->GetName() == "")
                continue;

            // pC is a cell that has the dot at the
            // position, given by the beginning of the
            // current complete state.
            SCellPtr  pC = CellUtils::getCellByIndex( this, pS->GetK() );
            if(!pC)
            {
                std::cerr << "ERROR: Invalid dot position while completing "
                          << pS->GetLHS()->GetName()
                          << " at step " << I << std::endl;
                return ERR_INVPARAM;
            }
            // Iterate through the target parent state set backwards,
            // checking if the nonterminal on the left of the dot
            // can be reached from current state.
            int YIndex = sg.GetNIndex(pT);
            for(size_t j = 0; j < pC->States.GetCount(); j++)
            {
                SStatePtr pNewS = pC->States.Get(j);
                TokItem pNewTI = pNewS->GetAfterDot();
                if(!pNewTI)
                    continue;

                KTokenPtr pNewT = pNewTI.GetToken();
                if(pNewT->GetType() != Token::NONTERMINAL)
                    continue;

                /*
        T_REAL Penalty = Filter(G, pNewS, pS);
        if(Penalty == 0.0)
          continue;
            */

                if( pNewT->GetName() == "?" )
                {
                    SStatePtr pAddS = MakeNewState();
                    *pAddS = *pNewS;

                    // Add the pT to the end of the pAddS and advance the
                    // dot.
                    pAddS->AddBeforeVar( *pT );
                    pAddS->SetHiMark(pS->GetHiMark());
                    pAddS->AdvanceDot();
                    pAddS->SetLabel(SState::COMPLETED);

                    Real NewGamma =
                            // g * g'' * Ru
                            pNewS->GetGamma() * pS->GetGamma();

                    Real NewV =
                            // v + v''
                            pNewS->GetV() + pS->GetV();

                    pAddS->SetAlpha(0.0);
                    pAddS->SetGamma(NewGamma);
                    pAddS->SetV    (NewV    );
                    pAddS->AddChild(pS);



                    if(AddState(pAddS, true, true, true) == ERR_ALREADYEXISTS)
                        delete pAddS;
                    continue;
                }

                // We have the non-terminal
                // Check it against the Ru to see if it is reacheable
                // from pT.
                int UIndex = sg.GetNIndex(pNewT);
                Real UnitProb = sg.GetRu(UIndex, YIndex);
                if(UnitProb <= 0)
                    continue;

                Real Penalty = Filter(sg, pNewS, pS);
                if(Penalty == 0.0)
                    continue;

                // We found the non-terminal reacheable from pT
                // by Ru.
                SStatePtr pAddS = MakeNewState();
                *pAddS = *pNewS;
                // predicted states do not have valid marks
                if(pNewS->IsPredicted())
                    pAddS->SetLowMark(pS->GetLowMark());

                pAddS->SetHiMark(pS->GetHiMark());
                pAddS->AdvanceDot();
                pAddS->SetLabel(SState::COMPLETED);

                Real NewAlpha = 0.0;
                Real NewGamma = 0.0;
                if(!pS->IsUnit())
                {
                    NewAlpha =
                            // a * g'' * Ru * Penalty
                            pNewS->GetAlpha() * pS->GetGamma() * UnitProb * Penalty;
                    NewGamma =
                            // g * g'' * Ru * Penalty
                            pNewS->GetGamma() * pS->GetGamma() * UnitProb * Penalty;
                }

                Real NewV =
                        // v + v'' + log(Ru) + log(Penalty)
                        pNewS->GetV() + pS->GetV() + std::log(UnitProb) + std::log(Penalty);
                        // YAI Compute Length of the production to normalize
                        // int Length = I - pS->GetK();
                        // NewV =
                        // pNewS->GetV() + pS->GetV()/Length + Log(UnitProb);

                pAddS->SetAlpha(NewAlpha);
                pAddS->SetGamma(NewGamma);
                pAddS->SetV    (NewV    );
                if(Prune(pAddS))
                {
                    delete pAddS;
                    continue;
                }

                pAddS->AddChild(pS);

#ifdef HEAVY_DEBUG
                printf("Adding ");
                pAddS->Dump(stdout);
                printf("\n");
                printf("\tby ");
                pS->Dump(stdout);
                printf("\n");
#endif
                if(AddState(pAddS, true, true, true) == ERR_ALREADYEXISTS)
                    delete pAddS;
            }
        }
    }

#ifdef HEAVY_DEBUG
    // Dump the states and their children
    for(int jj = 0; jj < States.GetCount(); jj++)
    {
        SStatePtr pSS = (SStatePtr)States.Get(jj);
        printf("State ");
        pSS->Dump(stdout);
        printf("\n");
        printf("\tChildren:\n");
        for(int ii = 0; ii < ((SStatePtr)pSS)->GetChildCount(); ii++)
        {
            SStatePtr pCS = ((SStatePtr)pSS)->GetChild(ii);
            printf("\t");
            pCS->Dump(stdout);
            printf("\n");
        }
    }
#endif
    return OK;
}

Real SCell::Filter(const SGrammar& /*sg*/, SStatePtr pNewS, SStatePtr pS)
{
    bool test = pNewS->GetHiMark() - pS->GetLowMark() > 11;

    if( pS->GetLHS()->GetName() != "SKIP" && pNewS->GetLHS()->GetName() != "SKIP" && test )
    {
#ifdef HEAVY_DEBUG
        printf("Rejecting ");
        pNewS->Dump(stdout);
        printf(" for completion\n");
#endif
        return 0.0;
    }
    return 1.0;
}


bool SCell::Prune(SStatePtr /*pS*/)
{
    // Returns true if need to drop this state
    return false;
}

void SCell::SetPartial(bool partial)
{
    partial_ = partial;
}

bool SCell::GetPartial() const
{
    return partial_;
}

Real SCell::GetHigh() const
{
    return (highMarkSet_)? highMark_ : 0;
}

void SCell::SetHigh (Real high)
{
    highMarkSet_ = true;
    highMark_ = high;
}

size_t SCell::GetI() const
{
    return I;
}

void SCell::SetI(size_t index)
{
    I = index;
}

SStatePtr SCell::MakeNewState()
{

    SStatePtr pS = new SState();
    pS->SetV(0);
    return pS;
}

size_t SCell::GetStateCount() const
{
    return States.GetCount();
}

KSStatePtr SCell::GetState(size_t i) const
{
    return States.Get(i);
}


Status SCell::AddState(
        SStatePtr pState,
        bool AddAlpha,
        bool AddGamma,
        bool Sorted)
{
    size_t i, j;
    for(i = 0; i < States.GetCount(); i++)
    {
        SStatePtr pS = States.Get(i);
        if( pState->sameKDotAndProd( *pS ) )
        {
            if(AddAlpha)
                pS->SetAlpha(pS->GetAlpha() + pState->GetAlpha());
            if(AddGamma)
            {
                pS->SetGamma(pS->GetGamma() + pState->GetGamma());

                // Check if we just need to add the state to the
                // children chain.
                size_t LastChild = pState->GetChildCount() - 1;
                SStatePtr pCh = pState->GetChild(LastChild);

#ifdef HEAVY_DEBUG
                printf("Current state: ");
                pS->Dump(stdout);
                printf("\n");
                printf("Added state: ");
                pState->Dump(stdout);
                printf("\n");
                if(pCh)
                {
                    printf("Looking at children of ");
                    pCh->Dump(stdout);
                    printf("\n");
                }
#endif

                if(pCh->IsUnit())
                {
                    SStatePtr pCh2 = pCh->GetChild(0);
                    if(pCh2)
                    {
#ifdef HEAVY_DEBUG
                        printf("\tChild: ");
                        pCh2->Dump(stdout);
                        printf("\n");
#endif
                        SStatePtr pSCh = NULL;
                        size_t ChildCount = pS->GetChildCount();
                        for(j = 0; j < ChildCount; j++)
                        {
                            pSCh = pS->GetChild(j);
#ifdef HEAVY_DEBUG
                            printf("\tComparing with: ");
                            pSCh->Dump(stdout);
                            printf("\n");
#endif
                            if( pCh2->sameKDotAndProd(*pSCh) )
                                break;
                        }
                        if(j < ChildCount)
                        {
                            pSCh = pS->RemoveChild(j);
                            pS->AddChildAt(j, pCh);
                            return ERR_ALREADYEXISTS;
                        }
                    }
                }

                // Maximize Viterbi probability
                Real V = pState->GetV();
                if(V > pS->GetV())
                {
                    pS->SetV(V);
                    // Path probability maximisation
                    // Copy the predecessors (this is not right).
                    // Should only remove the predecessors with current K.
                    pS->RemoveChildren();
                    for(j = 0; j < pState->GetChildCount(); j++)
                        pS->AddChild (pState->GetChild(j) );
                }
                /*
        // In any case, copy over the predecessors of a new state
        for(j = 0; j < ((SStatePtr)pState)->GetChildCount(); j++)
           pS->AddChild(((SStatePtr)pState)->GetChild(j));
*/
            }
            // The dup is actually ok.
            return ERR_ALREADYEXISTS;
        }
    }

    if(!Sorted)
        return States.Add(pState);

    size_t K = pState->GetK();
    for(i = 0; i < States.GetCount(); i++)
    {
        SStatePtr pS = States.Get(i);
        if(pS->IsCompleted() && (pS->GetK() < K))
            break;
    }

#ifdef HEAVY_DEBUG
    printf("ADDING ");
    pState->Dump(stdout);
    printf("at %d position\n", i);
#endif

    return States.AddAt(i, pState);
}


Status SCell::Scan(const Token& pT, SCellPtr pCell)
{
    Real P = pT.GetProb();
    size_t i;
    Status nRetCode;
    // Go through the States set and find the appropriate
    // states, that have the token after the dot.
    for(i = 0; i < States.GetCount(); i++)
    {
        SStatePtr pS = States.Get(i);
        if(pS->IsFinished())
            continue;

        TokItem pTI = pS->GetAfterDot();

        KTokenPtr pNewT = pTI.GetToken();
        if( pT.SameName(*pNewT) )
        {
            // We only come here with TOKEN_TERMINALs,
            //so we need to set the high and low marks
            // operator= for SState will copy the
            // Alpha and Gamma over.
            SStatePtr pNewS = MakeNewState();
            *pNewS = *pS;

            //	 TokItem pTestTI = pNewS->GetAfterDot();
            //	 TokenPtr pTestT = pTestTI->GetToken();
            //	 pTestT->IsA();
            pNewS->ReplaceAfterDot( pT );
            //	 pTestTI = pNewS->GetAfterDot();
            //	 pTestT = pTestTI->GetToken();
            //	 pTestT->IsA();

            if( pT.GetLow() )
                //	This should be here, except a check for the skip state also needed
                // if(pNewS->IsPredicted())
                pNewS->SetLowMark( pT.GetLow());
            if(pT.GetHigh() )
                pNewS->SetHiMark(pT.GetHigh());
            pNewS->AdvanceDot();
            pNewS->SetLabel(SState::SCANNED);
            if(P != 1.0)
            {
                pNewS->SetAlpha(pNewS->GetAlpha() * P);
                pNewS->SetGamma(pNewS->GetGamma() * P);
                pNewS->SetV    (pNewS->GetV() + std::log(P));
            }

            // Do not update neither Alpha nor Gamma
            if((nRetCode = pCell->AddState(pNewS, false, false))
                    == ERR_ALREADYEXISTS)
            {
                std::cerr << "ERROR: Duplicate state scanned." <<  std::endl;
                delete pNewS;
                return nRetCode;
            }
        }
    }

    // If no states predicted - the grammar is not satisfied
    /*
   if(!pCell->States.GetCount())
   {
      printf("ERROR: String rejected at step %d\n", pCell->GetI());
      return ERR_INVPARAM;
   }
*/

    return OK;
}

