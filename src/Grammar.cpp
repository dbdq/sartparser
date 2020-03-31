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

#include "Grammar.impl.h"

using namespace sartparser;
using namespace impl;

Grammar::Grammar() : Axiom(),
    T(Array<Token>::SHOULD_DELETE),
    N(Array<Token>::SHOULD_DELETE),
    P(Array<Token>::SHOULD_DELETE)
{
}

Grammar::~Grammar()
{
}

Status Grammar::AddAxiom(const std::string& word)
{

    if(Axiom.GetType() != Token::UNKNOWN)
    {
        std::cerr << "ERROR: Axiom already defined" << std::endl;
        return ERR_ALREADYEXISTS;
    }

    KTokenPtr pT = GetNonTerminal( word );
    if( pT )
    {
        //Axiom was already defined as a non-terminal
        Axiom.SetData(pT->GetName(), pT->GetType());
        return OK;
    }
    else
    {
        //Define a new non-terminal
        Axiom.SetData(word, Token::NONTERMINAL);
        return OK;
    }
}

KTokenPtr Grammar::GetTerminal(const std::string& word) const
{
    KTokenPtr result;
    for( unsigned int i = 0; i < T.GetCount(); ++i )
    {
        result = T.Get(i);
        if( word ==  result->GetName() )
            return result;
    }
    return NULL;
}

KTokenPtr Grammar::GetNonTerminal(const std::string& word) const
{
    KTokenPtr result;
    for(size_t i = 0; i < N.GetCount(); ++i)
    {
        result = N.Get(i);
        if( word == result->GetName() )
            return result;
    }
    return NULL;
}


KTokenPtr Grammar::GetAxiom() const
{
   return &Axiom;
}

ProductionPtr Grammar::GetProduction(size_t index)
{
   if(index < P.GetCount())
      return P.Get(index);
   else
      return NULL;
}

ProductionPtr Grammar::GetProduction(const std::string& word)
{
    for(size_t i = 0; i < P.GetCount(); i++)
    {
        ProductionPtr pProd = GetProduction(i);
        KTokenPtr pLHS = pProd->GetLHS();
        if( word == pLHS->GetName() )
            return pProd;
    }
    return NULL;
}

ProductionPtr Grammar::GetProduction(KTokenPtr pT)
{
   if(pT)
      return GetProduction(pT->GetName());
   return NULL;
}


KProductionPtr Grammar::GetProduction(size_t index) const
{
    return const_cast<Grammar*>(this)->GetProduction(index);
}

KProductionPtr Grammar::GetProduction(const std::string& word) const
{
    return const_cast<Grammar*>(this)->GetProduction(word);
}

KProductionPtr Grammar::GetProduction(KTokenPtr pT) const
{
    return const_cast<Grammar*>(this)->GetProduction(pT);
}


Status Grammar::AddTerminal(const std::string& word)
{
    if( GetTerminal( word ) )
    {
        // Terminal already exists - just ignore it
        std::cerr<< "WARNING: Duplicate terminal - " << word << std::endl;
        return OK;
    }
    TokenPtr pT = new Token(word, Token::TERMINAL);
    T.Add(pT);
    return OK;
}

Status Grammar::AddNonTerminal(const std::string& word)
{
    TokenPtr pT;
    if( GetNonTerminal(word) )
    {
        // Nonterminal already exists - just ignore it
        std::cerr << "WARNING: Duplicate nonterminal -" << word << std::endl;
        return OK;
    }

    //If axiom and current symbols are the same
    if( Axiom.GetName() == word)
    {
        pT = new Token(Axiom.GetName(), Axiom.GetType());
    }
    else
    {
        pT = new Token(word, Token::NONTERMINAL);
    }
    N.Add(pT);
    return OK;
}

Status Grammar::AddRule(const std::string& lhs, const StringVector& rhs)
{
    // Productions are only defined after all the terminals and
    // nonterminals involved are defined. The pieces can be interleaved
    // but the production checks if it is reading correct symbols while
    // reading words from the file.
    bool newProduction = false;

    KTokenPtr plhs = GetNonTerminal(lhs);
    if(!plhs)
    {
        std::cerr << "ERROR: LHS of a production is not a nonterminal - "
                  << lhs << std::endl;
        return ERR_NOTFOUND;
    }

    ProductionPtr pProd = GetProduction(plhs);
    if(!pProd)
    {
        newProduction = true;
        pProd = new Production();
        pProd->SetLHS(plhs);
    }

    RulePtr pR = new SRule();
    for(StringVector::const_iterator it = rhs.begin(); it != rhs.end(); ++it)
    {
        const std::string& word = *it;

        KTokenPtr pT = GetTerminal(word);
        if(!pT)
            pT = GetNonTerminal(word);
        if(!pT)
        {
            std::cerr << "ERROR: RHS for " << pProd->GetLHSName() <<
                         " contains undefined symbol" << std::endl;
            return ERR_INVPARAM;
        }
        pR->AddToken( *pT);
        pR->SetProb(1.0);
    }

    //Add rule to production
    pProd->AddRule(pR);

    //if production is new, add it to list of productions
    if( newProduction )
        P.Add(pProd);

    return OK;
}

Status Grammar::CheckGrammar()
{
   KTokenPtr pT;
   std::string Name;

   // First check if we terminals, nonterminals and productions have been 
   // defined, and T and N are non-overlapping sets
   size_t TCount = T.GetCount();
   size_t NCount = N.GetCount();
   size_t PCount = P.GetCount();


   if(!TCount)
   {
      std::cerr << "ERROR: Alphabet is empty" << std::endl;
      return ERR_INVPARAM;
   }

   if(!NCount)
   {
      std::cerr << "ERROR: Set of nonterminals is empty."
                << "At least the Axiom has to be defined." << std::endl;
      return ERR_INVPARAM;
   }

   if(!PCount)
   {
      std::cerr << "ERROR: No production rules defined." << std::endl;
      return ERR_INVPARAM;
   }

   // Check if axiom is defined
   Name = Axiom.GetName();
   if( Name == "" || Axiom.GetType() == Token::UNKNOWN)
   {
      std::cerr << "ERROR: Axiom is not defined" << std::endl;
      return ERR_INVPARAM;
   }

   for(size_t i = 0; i < T.GetCount(); i++) // Iterate through a
   {				// list of terminals making sure they are
      KTokenPtr pNewT;		// not defined in both sets
      pT = T.Get(i);
      Name = pT->GetName();
      pNewT = GetNonTerminal(Name); // it should be NULL
      if(pNewT)
      {
      std::cerr << "ERROR: Name " << Name
                << " is defined as both terminal and nonterminal" << std::endl;
	 return ERR_INVPARAM;
      }

   }

   // Check if Axiom is a part of N.
   pT = GetNonTerminal(Axiom.GetName());
   if(!pT)
   {
      // Not found
      std::cerr << "ERROR: Axiom " << Axiom.GetName()
                << " is not a nonterminal" << std::endl;
      return ERR_INVPARAM;
   }

   // See if all the nonterminals have at least
   // one expansion rule.
   for(size_t i = 0; i < NCount; i++)
   {
      pT = N.Get(i);
      if(!GetProduction(pT))
      {
     std::cerr << "ERROR: No production for " << pT->GetName()
               << " defined" << std::endl;
	 return ERR_INVPARAM;
      }
   }
    
   // Now, check that there are no terminals on the LHS of any production.
   ProductionPtr pProd;
   for(size_t i = 0; i < PCount; i++)
   {
      pProd = P.Get(i);
      pT = pProd->GetLHS();
      if(pT->GetType() != Token::NONTERMINAL)
      {
     std::cerr << "ERROR: " << pT->GetName() <<
                  ", defined as terminal is used as LHS" << std::endl;
	 return ERR_INVPARAM;
      }
   }

   // Add an end of string terminal and end of string token
   // to the productions for the axiom.
   if ( GetTerminal("")  == NULL)
   {
       TokenPtr terminal = new Token("", Token::TERMINAL);
       T.Add(terminal);
   }

   return OK;
}

int Grammar::GetNIndex(KTokenPtr pT) const
{
   for(size_t i = 0; i < N.GetCount(); i++)
   {
      KTokenPtr pNewT = N.Get(i);
      if(pT->SameName(*pNewT))
	 return i;
   }
   return -1;
}

int Grammar::GetTIndex(KTokenPtr pT) const
{
   for(size_t i = 0; i < T.GetCount(); i++)
   {
      KTokenPtr pNewT = T.Get(i);
      if(pT->SameName(*pNewT) )
	 return i;
   }
   return -1;
}
