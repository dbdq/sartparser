/*
 * Copyright (c) 2014 Miguel Sarabia, KyuHwa Lee
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


#include <cmath>
#include <stdexcept>
#include <map>

#include "SParser.h"
#include "PTerminal.h"
#include "SParserUtils.impl.h"
#include "CellUtils.impl.h"
#include "SCell.impl.h"
#include "CFGrammar.impl.h"


using namespace sartparser;
using namespace impl;


//==============================================================================
// IMPL DEFINITION
//==============================================================================
struct SParser::Impl
{
    Impl(CFGrammar &cfg, bool partial);
    ~Impl();

    Status ParseFinal();
    std::pair<SCellPtr, KSStatePtr> GetMostLikelyFinalState();
    void ExpandState(const SState &pS, StringVector& terminals) const;

    SCellPtr backtrack();
    ParseProbability GetViterbiProb(const SState &state) const;
    Status ParseLine(const Line &line, bool final = false);
    Line getPredictedLine() const;
    ParseProbability getPredictedAlpha(const Line& line);

    static ParseProbability getMaxAlpha(const SCell &cell);

    const CFGrammar& grammarWrapper_;
    const SGrammar& grammar_;
    SCell cellHead_;

    // This is to keep track
    // of current cell in the list
    SCellPtr currentCell_;
    bool partial_;

    // Output stream to send debug information
    std::ostream* debug_;
};

//==============================================================================
// IMPL IMPLEMENTATION
//==============================================================================
SParser::Impl::Impl(CFGrammar& cfg, bool partial)
    : grammarWrapper_( cfg )
    , grammar_( cfg.pimpl_->sg )
    , currentCell_( &cellHead_ )
    , partial_( partial )
    , debug_( NULL )
{

    if (cfg.checkGrammar() != OK)
        throw std::invalid_argument("Grammar check failed");

    cellHead_.SetPartial(partial_);

    //Note this call may modify sg (and consequently grammar_) !!!
    cellHead_.Init(cfg.pimpl_->sg);

    if ( cellHead_.Predict(grammar_) != OK)
        throw std::invalid_argument(
                "SParser failed to initialise (likely due to invalid grammar");

}


SParser::Impl::~Impl()
{
    CellUtils::destroyCells(&cellHead_, false);
}


Status SParser::Impl::ParseFinal()
{
    if(debug_)
    {
        *debug_ << "Parsing final symbol" << std::endl;
    }

    Line final;
    final.insert( *grammar_.GetTerminal("") );

    return ParseLine(final, true);
}


std::pair<SCellPtr, KSStatePtr> SParser::Impl::GetMostLikelyFinalState()
{
    KSStatePtr state = NULL;
    int maxIndex = -1;
    Real maxProb = -1;

    // Parse the final symbol ("")
    ParseFinal();

    // Now backtrack, but keep last cell for this function
    SCellPtr finalCell = backtrack();

    //if for whatever reason it's still invalid return NULL
    if (!finalCell)
    {
        return std::make_pair<SCellPtr, KSStatePtr>(NULL, NULL);
    }

    // Find the final state
    for(size_t i = 0; i < finalCell->GetStateCount(); i++)
    {
        state = finalCell->GetState(i);
        if( state->GetLHS()->GetName() == "" )
        {
            int length = finalCell->GetI() - state->GetK() - 1;
            Real prob = state->GetV()/length;
            if( state->GetV() != 0.0 && maxIndex == -1)
            {
                maxProb = prob;
                maxIndex = i;
            }
            else
            {
                if(maxProb < prob)
                {
                    maxProb = prob;
                    maxIndex = i;
                }
            }
        }
    }

    if(maxIndex == -1)
    {
        delete finalCell;
        return std::make_pair<SCellPtr, KSStatePtr>(NULL, NULL);
    }
    else
    {
        state = finalCell->GetState( static_cast<size_t>(maxIndex) );
        return std::make_pair(finalCell, state);
    }
}

void SParser::Impl::ExpandState(const SState& s, StringVector& terminals) const
{
    size_t childIndex = 0;
    KTokItem tokItem = s.GetFirst();

    while( tokItem )
    {
        const Token& t = *tokItem.GetToken();
        if (t.GetType() == Token::TERMINAL && t.GetName() != "" )
        {
            terminals.push_back( t.GetName() );
        }
        else
        {
            ExpandState( *s.GetChild( childIndex ), terminals );
            ++childIndex;

        }
        tokItem = tokItem.GetNext();
    }
}

SCellPtr SParser::Impl::backtrack()
{
    if ( currentCell_ != &cellHead_ )
    {
        currentCell_ = CellUtils::getPrev(currentCell_);
        return CellUtils::split(currentCell_);
    }
    else
    {
        return NULL;
    }
}

Status SParser::Impl::ParseLine(const Line& line, bool final)
{
    Status retCode;

    //If first time, print headCell before modifications
    if ( currentCell_ == &cellHead_ && debug_ && !final )
    {
        *debug_ << "Initial states" << std::endl;
        CellUtils::dumpCell(currentCell_, *debug_);
    }

    if(debug_ && !final)
    {
        *debug_ << "Reading" << std::endl;
        for( Line::const_iterator it = line.begin(); it != line.end(); ++it )
        {
            *debug_ << it->GetName() << " [" << it->GetProb() << "]   ";
        }
        *debug_ << std::endl;
    }

    currentCell_ = currentCell_->Scan(line);

    if(currentCell_)
    {
        retCode = currentCell_->Complete(grammar_);
        if( retCode == OK)
        {
            retCode = currentCell_->Predict(grammar_);
        }
        if ( retCode != OK )
            return retCode;

        if( debug_ )
            CellUtils::dumpCell(currentCell_, *debug_);
    }
    else
        return ERR_INVPARAM;

    return OK;
}

ParseProbability SParser::Impl::GetViterbiProb(const SState& state) const
{
    // Note state came from the cell after the current one
    int length = currentCell_->GetI() - state.GetK();

    return ParseProbability( state.GetV(), length, true);
}


Line SParser::Impl::getPredictedLine() const
{
    typedef std::map<std::string, Real> Storage;
    typedef Storage::const_iterator Iterator;

    Storage totalAlphas;

    //Initialise maxAlphas (each terminal has a starting 0.0 probability)
    for (size_t i = 0; i < grammar_.GetTCount(); ++i)
    {
        const std::string& token = grammar_.GetTByIndex(i)->GetName();
        if ( token != "" )
        {
            totalAlphas.insert( std::make_pair( token, 0.0 ) );
        }
    }


    //Go through all states and note maxAlpha for each terminal
    for (size_t i =0; i < currentCell_->GetStateCount(); ++i)
    {
        KSStatePtr state = currentCell_->GetState( i );
        KTokenPtr tok = state->GetAfterDot().GetToken();

        if ( !tok || tok->GetType() != Token::TERMINAL || tok->GetName() == "")
        {
            continue;
        }

        totalAlphas.at( tok->GetName() ) += state->GetAlpha();
    }

    //Get sum of all alphas
    Real sumAlphas = 0;
    for (Iterator it = totalAlphas.begin(); it != totalAlphas.end(); ++it)
    {
        sumAlphas += it->second;
    }

    Line result;
    for (Iterator it = totalAlphas.begin(); it != totalAlphas.end(); ++it)
    {
        const std::string& tokenName = it->first;
        Real normAlpha = it->second/sumAlphas;
        if (normAlpha > 0)
        {
            result.insert( Token(tokenName, Token::TERMINAL, normAlpha ) );
        }
    }

    return result;

}

ParseProbability SParser::Impl::getPredictedAlpha(const Line &line)
{
    //Parse predicted line
    ParseLine(line);

    ParseProbability result = getMaxAlpha(*currentCell_);

    //Before returning ensure we delete predictedCell
    SCellPtr predictedCell = backtrack();
    delete predictedCell;

    return result;
}

ParseProbability SParser::Impl::getMaxAlpha(const SCell& cell)
{

    KSStatePtr maxAlphaState = NULL;
    for(size_t i = 0; i < cell.GetStateCount(); ++i)
    {
        KSStatePtr candidate = cell.GetState(i);
        if ( candidate->IsFinished() )
        {
            continue;
        }
        else if ( maxAlphaState == NULL ||
                  candidate->GetAlpha() > maxAlphaState->GetAlpha()  )
        {
            maxAlphaState = candidate;
        }
    }

    int length = cell.GetI();

    if (maxAlphaState == NULL)
        return ParseProbability();
    else
        return ParseProbability( maxAlphaState->GetAlpha(), length, false );
}

//==============================================================================
// SPARSER IMPLEMENTATION
//==============================================================================
SParser::SParser(CFGrammar &cfg)
{
    pimpl_ = new Impl( cfg, false );
}

SParser::~SParser()
{
    delete pimpl_;
}

Status SParser::parse(const PInput &input)
{
    typedef PInput::const_iterator Iterator;

    Line line;

    for( Iterator it = input.begin(); it != input.end(); ++it)
    {
        KTokenPtr tok = pimpl_->grammar_.GetTerminal(it->terminal);
        if ( tok == NULL )
        {
            std::cerr << "Unkown terminal in " << it->terminal << std::endl;
            return ERR_NOTFOUND;
        }
        Token newToken(
                    it->terminal,
                    tok->GetType(),
                    it->probability,
                    it->highMark,
                    it->lowMark);

        line.insert(newToken);
    }

    pimpl_->ParseLine(line);

    return OK;
}



Status SParser::parse(const PInputs &inputs)
{
    Status errCode = OK;

    typedef PInputs::const_iterator Iterator;
    for (Iterator it = inputs.begin(); it!=inputs.end(); ++it)
    {
        //This parses a PInput
        errCode = parse( *it );
        if ( errCode != OK )
            return errCode;
    }

    return errCode;
}

void SParser::reset()
{
    pimpl_->currentCell_ = &pimpl_->cellHead_;
    CellUtils::destroyCells(pimpl_->currentCell_, false);
}

ParseProbability SParser::getCurrentMaxAlpha() const
{
    return Impl::getMaxAlpha( *pimpl_->currentCell_ );
}

Prediction SParser::getPrediction()
{
    //Compute what we need
    Line predicted = pimpl_->getPredictedLine();

    ParseProbability predictedProb = pimpl_->getPredictedAlpha(predicted);

    Prediction result;
    result.probability = predictedProb;

    //For each token in Line save its name and probability
    typedef Line::const_iterator Iterator;
    for (Iterator it = predicted.begin(); it != predicted.end(); ++it)
    {
        result.terminalDistribution.insert(
                    std::make_pair(it->GetName(), it->GetProb() ) );
    }

    return result;
}

ViterbiParse SParser::getViterbiParse()
{
    std::pair<SCellPtr, KSStatePtr> pair = pimpl_->GetMostLikelyFinalState();
    SCellPtr finalCell = pair.first;
    const SState& mostLikelyState = *pair.second;
    if (!finalCell)
    {
        return ViterbiParse();
    }

    StringVector symbols;
    pimpl_->ExpandState(mostLikelyState, symbols);

    //Prepare result
    ViterbiParse result (
                    symbols,
                    pimpl_->GetViterbiProb(mostLikelyState),
                    ParseTreeUtil::ParseTreeFromState(mostLikelyState) );

    // We need to clear this memory just before leaving this function
    // as it is the last time we'll use it
    delete finalCell;

    return result;
}

void SParser::setDebug(std::ostream& debug)
{
    pimpl_->debug_ = &debug;
}

void SParser::unsetDebug()
{
    pimpl_->debug_ = NULL;
}

const CFGrammar& SParser::getGrammar() const
{
    return pimpl_->grammarWrapper_;
}
