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

#include <limits>
#include <sstream>
#include <iomanip>

#include "Stream.h"
#include "PTerminal.h"
#include "SParserUtils.h"
#include "CFGrammar.impl.h"

using namespace sartparser;
using namespace impl;

namespace sartparser
{
struct Stream
{
    static Status readNextWord(std::istream& i, std::string& s)
    {
        while (true)
        {
            i >> s;
            //return ERR_EOF if we finished the file
            if ( i.eof() )
                return ERR_EOF;

            if ( s.at(0) != '#' )
                return OK;
            else
            {
                // Discard the comment
                i.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    enum Section{UNKNOWN, TERMINALS, NONTERMINALS, PRODUCTIONS, AXIOM};

    static Section getSectionTag(const std::string& word)
    {
        if (word == "Terminals")
            return TERMINALS;
        else if (word == "NonTerminals")
            return NONTERMINALS;
        else if (word == "Productions")
            return PRODUCTIONS;
        else if (word == "Axiom")
            return AXIOM;
        else
            return UNKNOWN;
    }


    static std::string loadDelimited(
            std::istream& i,
            const std::string& word,
            char begin,
            char end )
    {
        if (word.at(0) != begin)
            return "";

        //If word contains ending character
        if (word.at(word.length() -1 ) == end)
        {
            //Return string without delimiters
            return word.substr(1, word.length() -2 );
        }
        else
        {
            std::string temp;
            std::getline(i, temp, end);
            //Return the original word (without delimiters) plus what we've
            //just extracted
            return  word.substr(1, word.length() -1) + temp;
        }
    }

    static bool isEndOfProduction(const std::string& word)
    {
        // Evaluates to true if word is beginning of new section,
        // or a new production rule (but not part of a label)
        return ((word.at(0) != '<') && (word.at(word.length() - 1) == ':')) ||
                word == "Section";
    }

    static Status loadProduction(std::istream& i, std::string& word, SGrammar& g)
    {
        // Productions are only defined after all the terminals and
        // nonterminals involved are defined. The pieces can be interleaved
        // but the production checks if it is reading correct symbols while
        // reading words from the file.
        Status errCode;
        bool closed = false;

        //Check lhs is finished by colon and delete it
        if( word.at(word.length() - 1) == ':' )
            word.resize(word.length() -1 );
        else
        {
            std::cerr << "WARNING: Colon missing in production for "
                      << word << std::endl;
        }
        std::string lhs = word;
        StringVector rhs;
        Real probability;
        std::string label = "";

        while(true)
        {
            errCode = readNextWord(i, word);

            if( errCode == ERR_EOF || isEndOfProduction(word) )
            {
                if(closed)
                {
                    g.AddRule(lhs, rhs, probability, label);
                    return errCode;
                }
                else
                {
                    std::cerr << "ERROR: Incomplete rule for " << lhs << std::endl;
                    return ERR_INVPARAM;
                }
            }

            else if (closed)
            {
                std::string tryLabel = loadDelimited(i, word, '<', '>');
                if( !tryLabel.empty() )
                {
                    //if we reached the eof whilst trying to read the label
                    if (i.eof())
                    {
                        std::cerr << "ERROR: Unrecognized sequence " << word
                                  << " in production for " << lhs << std::endl;
                        return ERR_INVPARAM;
                    }
                    //If we've successfully extracted a label
                    label = tryLabel;
                }
                else if(word.at(0) == '|')
                {
                    g.AddRule(lhs, rhs, probability, label);
                    label.clear();
                    rhs.clear();
                    closed = false;
                }
            }

            else
            {
                std::string tryProb = loadDelimited(i, word, '[', ']');
                if( !tryProb.empty() )
                {
                    //If we got the end-of-file when trying to read probability
                    if ( i.eof() )
                    {
                        std::cerr << "ERROR: Invalid probability " << word
                                  << " in production for " << lhs << std::endl;
                        return ERR_INVPARAM;
                    }

                    //Convert string to real
                    std::stringstream ss;
                    ss << tryProb;
                    ss >> probability;
                    closed = true;
                }
                else
                {
                    //This is just a normal token to add to our rhs
                    rhs.push_back(word);
                }
            }
        } //end while loop
        return OK;
    }

    static Status loadGrammar(std::istream&i, CFGrammar& cfg)
    {
        //Get hold of internal grammar
        SGrammar& g = cfg.pimpl_->sg;

        Status errCode;
        Section currentSection = UNKNOWN;
        std::string word;


        while(true)
        {
            // AddProduction returns the next word in
            // its input parameter, so, only read
            // the next word if reading other sections
            if(currentSection != PRODUCTIONS)
                if(readNextWord(i, word) == ERR_EOF)
                    return g.CheckGrammar();

            //Word contains next word from the file
            if(word == "Section")
            {
                if(readNextWord(i, word) == ERR_EOF)
                {
                    std::cerr << "ERROR: Unexpected EOF reading section name"
                              << std::endl;
                    return ERR_READINGFILE;
                }
                currentSection = getSectionTag(word);
                if(currentSection == UNKNOWN)
                {
                    std::cerr << "ERROR: Invalid section name - "
                              << word << std::endl;
                    return ERR_READINGFILE;
                }
                if(readNextWord(i, word) == ERR_EOF)
                {
                    std::cerr << "ERROR: Unexpected EOF reading section"
                              << std::endl;
                    return ERR_READINGFILE;
                }
            }

            switch(currentSection)
            {
            case AXIOM:
                errCode = g.AddAxiom(word);
                if( errCode != OK)
                    return errCode;
                break;
            case TERMINALS:
                errCode = g.AddTerminal(word);
                if( errCode != OK)
                    return errCode;
                break;
            case NONTERMINALS:
                errCode = g.AddNonTerminal(word);
                if( errCode != OK)
                    return errCode;
                break;
            case PRODUCTIONS:
                errCode = loadProduction(i, word, g);
                if( errCode == ERR_EOF )
                    return g.CheckGrammar();
                else if ( errCode != OK )
                    return errCode;
                break;
            case UNKNOWN:
            default:
                //Should never happen
                std::cerr << "ERROR: Invalid section name - "
                          <<  word << std::endl;
                return ERR_READINGFILE;
            } //End switch statement
        }// end while loop

        return OK;
    }


    static void dumpGrammar( std::ostream& o, const CFGrammar& cfg )
    {
        SGrammar& g = cfg.pimpl_->sg;

        o << "Section Terminals" << std::endl;

        for(size_t i = 0; i < g.GetTCount(); i++)
        {
            KTokenPtr tok = g.GetTByIndex(i);
            if ( tok->GetName() != "" )
            {
                o << "\t" << tok->GetName() << std::endl;
            }
        }

        o << "Section NonTerminals" << std::endl;
        for(size_t i = 0; i < g.GetNCount(); i++)
        {
            KTokenPtr tok = g.GetNByIndex(i);
            o << "\t" << tok->GetName() << std::endl;
        }

        o << "Section Axiom" << std::endl;
        o << "\t" << g.GetAxiom()->GetName() << std::endl;

        o << "Section Productions"  << std::endl;
        for(size_t i = 0; i < g.GetNCount(); i++)
        {

            KProductionPtr prod = g.GetProduction( g.GetNByIndex(i) );
            if (prod == NULL)
                continue;

            o << "\t" << prod->GetLHSName() << ":";

            for(size_t j = 0; j < prod->GetRuleCount(); j++)
            {
                KSRulePtr rule = prod->GetRule(j);
                if (j != 0 )
                    o << "\t\t| ";
                else
                    o << "\t ";

                KTokItem tokItem = rule->GetFirst();
                while(tokItem)
                {
                    o << tokItem.GetToken()->GetName() << " ";
                    tokItem = tokItem.GetNext();
                }

                o << "\t["
                  << std::setw(2)
                  << std::setprecision(2)
                  << rule->GetProb()
                  << "]" << std::endl;
            }
        }
        o.unsetf(std::ios_base::floatfield);
    }



    static void dumpSGrammarMatrix(
            std::ostream& o,
            const SGrammar& sg,
            //Below is a ptr to member function, syntax is nasty, use with care!
            Real (SGrammar::*getter)(int, int) const )
    {
        size_t n = sg.GetNCount();

        if( sg.GetPartial() )
            --n;

        o << "         ";
        for(size_t i = 0; i < n; i++)
            o << std::setw(9) << sg.GetNByIndex(i)->GetName();
        o << std::endl;

        for(size_t i = 0; i < n; i++)
        {
            o << std::setw(9) << sg.GetNByIndex(i)->GetName();

            for(size_t j = 0; j < n; j++)
            {
                o << std::setw(9) << (sg.*getter)(i, j); //Yuck!
            }
            o << std::endl;
        }
    }

    static void dumpSGrammarMatrices(std::ostream& o, const CFGrammar& cfg)
    {
        SGrammar& sg = cfg.pimpl_->sg;
        o <<  std::endl << " Pl:" << std::endl;
        dumpSGrammarMatrix(o, sg, &SGrammar::GetPl);

        o <<  std::endl << " Rl:" << std::endl;
        dumpSGrammarMatrix(o, sg, &SGrammar::GetRl);

        o <<  std::endl << " Pu:" << std::endl;
        dumpSGrammarMatrix(o, sg, &SGrammar::GetPu);

        o <<  std::endl << " Ru:" << std::endl;
        dumpSGrammarMatrix(o, sg, &SGrammar::GetRu);
    }


    static void toDouble(const std::string& str, Real& d)
    {
        std::stringstream buffer;
        buffer << str;
        buffer >> d;
    }

    static Status loadLine(
            std::istream& i,
            PInput& input,
            const CFGrammar& cfg,
            bool simple)
    {

        SGrammar& sg = cfg.pimpl_->sg;

        Status errCode;
        std::string str;

        for(size_t j = 0; j < sg.GetTCount(); j++)
        {
            KTokenPtr tok = sg.GetTByIndex(j);
            if( tok->GetName().empty() )
                continue;

            PTerminal pword;

            pword.terminal = tok->GetName();

            errCode = readNextWord(i, str);
            if (errCode == ERR_EOF && j ==0 )
                return ERR_EOF;
            else if (errCode != OK)
                return ERR_READINGFILE;

            toDouble(str, pword.probability );

            if (simple)
            {
                pword.highMark = 0.0;
                pword.lowMark = 0.0;
            }
            else
            {
                errCode = readNextWord(i, str);
                if (errCode != OK)
                    return ERR_READINGFILE;
                toDouble(str, pword.highMark);

                errCode = readNextWord(i, str);
                if (errCode != OK)
                    return ERR_READINGFILE;

                Real length;
                toDouble(str, length);
                pword.lowMark = pword.highMark - length;
            }

            input.push_back(pword);
        }
        return OK;
    }

    static Status loadPTFile(
            std::istream &i,
            PInputs& inputs,
            const CFGrammar& cfg,
            bool simple )
    {
        Status errCode;

        while( true )
        {
            PInput pinput;
            errCode = loadLine(i, pinput, cfg, simple);

            if (errCode == ERR_EOF && inputs.size() > 0)
            {
                return OK;
            }
            else if (errCode != OK)
            {
                return errCode;
            }
            inputs.push_back(pinput);
        }
    }

    static void dumpParseTree(
            std::ostream& o,
            const ParseTree& p,
            unsigned int level )
    {
        for (unsigned int i = 0; i < level; ++i)
            o << "   ";


        o << std::fixed << p.k() << " " << p.lhs() << " -> ";

        //Print rhs
        const StringVector& rhs = p.rhs();
        for (StringVector::const_iterator it=rhs.begin(); it!= rhs.end(); ++it)
        {
            o << *it << " ";
        }

        o << "\t[" << std::scientific << p.alpha()
          << ", " << p.gamma()
          << ", " << p.v();

        o.unsetf(std::ios_base::floatfield);

        o << "] ["  << p.lowMark()
          << ", " << p.highMark()
          << "]" << std::endl;

        //Print children
        typedef std::vector<ParseTree>::const_iterator ParseTreeIterator;
        const std::vector<ParseTree>& children = p.children();
        for (ParseTreeIterator it = children.begin(); it!= children.end(); ++it)
        {
            dumpParseTree(o, *it, level+1);
        }
    }

}; //End of Stream struct


Status loadGrammar(std::istream &i, CFGrammar &cfg)
{
    return Stream::loadGrammar(i, cfg);
}


void dumpGrammarMatrices(std::ostream &o, const CFGrammar& cfg)
{
    Stream::dumpSGrammarMatrices(o, cfg);
}

Status loadSimpleParseLine(
        std::istream &i,
        PInput &input,
        const CFGrammar &cfg )
{
    return Stream::loadLine(i, input, cfg, true);
}

Status loadSimpleParseFile(
        std::istream &i,
        PInputs &inputs,
        const CFGrammar &cfg )
{
    return Stream::loadPTFile(i, inputs, cfg, true);
}

Status loadParseLine(
        std::istream &i,
        PInput &input,
        const CFGrammar& cfg)
{
    return Stream::loadLine(i, input, cfg, false);
}

Status loadParseFile(
        std::istream &i,
        PInputs &inputs,
        const CFGrammar& cfg)
{
    return Stream::loadPTFile(i, inputs, cfg, false);
}

std::ostream& operator<<(std::ostream &o, const CFGrammar& cfg)
{
    //Stochastic grammar
    Stream::dumpGrammar(o, cfg);
    return o;
}


std::ostream& operator<<(std::ostream &o, const ParseTree &t)
{
    Stream::dumpParseTree(o, t, 0);
    return o;
}

std::ostream& operator<<(std::ostream &o, const ViterbiParse &v)
{
    o << "Parse: ";

    typedef StringVector::const_iterator Iterator;
    for (Iterator it = v.terminals.begin(); it!= v.terminals.end(); ++it)
    {
        o << *it << " ";
    }

    o << "    " << v.probability;
    return o;
}

std::ostream& operator<<(std::ostream &o, const Prediction &p)
{
    o << "Prediction: " << std::setprecision(4);

    typedef Prediction::ProbabilityDistribution::const_iterator Iterator;
    for ( Iterator it = p.terminalDistribution.begin();
          it!= p.terminalDistribution.end();
          ++it )
    {
        o << it->first << " [" << it->second << "] ";
    }

    o << "    " << p.probability;
    return o;
}


std::ostream& operator<<(std::ostream &o, const ParseProbability &p)
{
    o << std::setprecision(4)
      << "Probability = " << p.raw
      << "    Scaled probability = " << p.scaled << std::endl;

    //Reset precision
    o.unsetf(std::ios_base::floatfield);

    return o;
}

std::ostream& operator<<(std::ostream& o, const PTerminal &t)
{
    o << std::setprecision(4) << t.terminal << " [" << t.probability;
    o.unsetf(std::ios_base::floatfield);
    o << "] ["  << t.lowMark << ", " << t.highMark << "]" << std::endl;

    return o;
}

std::ostream& operator<<(std::ostream& o, const Rule& r )
{

    o << r.lhs << ": ";

    for(StringVector::const_iterator it= r.rhs.begin(); it!= r.rhs.end(); ++it)
    {
        o << *it << " ";
    }
    o << "  ["  << std::setw(2) << std::setprecision(2) << r.probability << "]";

    o.unsetf(std::ios_base::floatfield);

    return o;
}

}//end of sartparser namespace
