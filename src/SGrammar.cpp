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

#include "SGrammar.impl.h"

using namespace sartparser;
using namespace impl;

Status SGrammar::AddRule(
        const std::string& lhs,
        const StringVector& rhs,
        Real probability,
        const std::string& label)
{
    Status ret = Grammar::AddRule(lhs, rhs);

    if (ret != OK)
        return ret;

    ProductionPtr pProd = GetProduction( lhs );
    //Change the probability of the last added rule
    pProd->GetRule( pProd->GetRuleCount() -1 )->SetProb(probability);
    pProd->SetLabel( label );

    return OK;
}

Status SGrammar::CheckGrammar()
{

   Status nRetCode = Grammar::CheckGrammar();

   if (nRetCode != OK)
      return nRetCode;

   // Check that sum of probabilities of rules for
   // each nonterminal adds up to 1.0
   for(unsigned int i = 0; i < P.GetCount(); i++)
   {
       Real prob = 0.0;
       ProductionPtr pP = P.Get(i);
       for(size_t j = 0; j < pP->GetRuleCount(); j++)
       {
           RulePtr pR = pP->GetRule(j);
           prob += pR->GetProb();
       }

       if( prob < Real(0.9999) || prob > Real(1.0001) )
       {
           std::cerr << "ERROR: Probability of " << pP->GetLHSName()
                     << " [" << prob <<"] is not 1" << std::endl;
           return ERR_INVPARAM;
       }
   }

   nRetCode = ComputeClosures();
   if (nRetCode != OK)
       return nRetCode;

   return OK;
}


Status SGrammar::ComputeClosures()
{
   int size = N.GetCount();
   size_t PCount = P.GetCount();

   // It assumes all the matrices have been allocated.
   // Clear the matrix

   // Create a matrix of zeros of the right size
   Matrix zeros = Matrix::Zero(size, size);
   Pl = zeros;
   Pu = zeros;

   for(size_t i = 0; i < PCount; i++)
   {
       ProductionPtr pProd = P.Get(i);
       KTokenPtr pT = pProd->GetLHS();
       int LHSIndex = GetNIndex(pT);
       if(LHSIndex == -1)
       {
           std::cerr << "ERROR: LHS " << pT->GetName()
                     << " not found among nonterminals" << std::endl;
           return ERR_NOTFOUND;
       }
       size_t RCount = pProd->GetRuleCount();

       for(size_t j = 0; j < RCount; j++)
       {
           RulePtr pR = pProd->GetRule(j);
           TokItem pTI = pR->GetFirst();
           KTokenPtr pNewT = pTI.GetToken();
           if(pNewT->GetType() == Token::NONTERMINAL)
           {
               int Index = GetNIndex(pNewT);
               if(Index == -1)
               {
                   std::cerr << "ERROR: " << pNewT->GetName()
                             << " not found among nonterminals" << std::endl;
                   return ERR_NOTFOUND;
               }
               Real Pr = pR->GetProb();

               // At this point, each Production should be for a
               // distinct nonterminal.
               // Pl accumulates probability for all left corner
               // relations between LHS and each one of the first
               // nonterminals of each RHS.
               Pl(LHSIndex, Index) += Pr;

               // Check if it is a unit production.
               // If it is then it is a part of Pu.
               if( !pTI.GetNext() )
                   Pu(LHSIndex, Index) += Pr;
           }
       }
   }

   // Rl, as a matrix of transitive reflexive closures of the
   // corresponding Pl can be computed in closed form as 
   // Rl = (I - Pl)^-1
   MakeR(Pl, Rl);

   // Ru, as a matrix of transitive reflexive closures of the
   // corresponding Pu can be computed in closed form as 
   // Ru = (I - Pu)^-1
   MakeR(Pu, Ru);

   return OK;
}

void SGrammar::MakeR(const Matrix &aP, Matrix &aR)
{
   int size = N.GetCount();
   Matrix identity = Matrix::Identity(size, size);

   // aR = (I - aP)^-1
   aR = (identity - aP).inverse();
}
