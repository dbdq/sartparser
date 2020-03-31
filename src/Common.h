/*
 * Copyright (c) 2014 Miguel Sarabia
 * Imperial College London
 *
 * Copyright (c) 1995, 1997 Yuri Ivanov
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

#ifndef __GLOBDEFS_H__
#define __GLOBDEFS_H__

/// @file
/// @brief Common include and definitions for the SARTParser library.

#include <cstddef>
#include <string>
#include <vector>
#include <iostream>

///@brief Namespace for all functions and classes in SARTParser library
namespace sartparser
{

/// Error code for SARTParser library
enum Status
{
    OK = 0, ///< Operation succeded. No errors.
    ERR_INVPARAM, ///< Invalid parameter.
    ERR_READINGFILE,///< Error reading file.
    ERR_OUTOFMEMORY,///< Error allocating memory.
    ERR_OUTOFBOUNDS,///< Requested item outside of container bounds.
    ERR_NOTFOUND,///< Requested item not found in container.
    ERR_ALREADYEXISTS,///< Requested item already exists in container.
    ERR_EOF,///< End-of-file found whilst processing file.
    ERR_REJECTED///< Input was rejected by Parser
};

//Definition of Real
/// @typedef sartparser::Real
/// @brief Typedef to represent real numbers in SARTParser, it may be
/// mapped to either double or float depending on the compile configuration.
/// @remarks This type is **not** available in *Python*.
#ifdef USE_FLOAT
typedef float Real;
#else
typedef double Real;
#endif


//Forward definitions
class PTerminal;
class SParser;
class ParseTree;
class Prediction;
class ViterbiParse;
class CFGrammar;
class ParseProbability;
class Rule;

//Typedefs
/// @brief A collection of Rule.
/// @remarks In *Python*, this class cannot be instantiated as it is only used
/// for reading. It behaves as (and can be converted to) a list.
typedef std::vector<Rule> Rules;

/// @brief A collection of std::string.
/// @remarks In *Python*, this class cannot be instantiated as it is only used
/// for reading. It behaves as (and can be converted to) a list.
typedef std::vector<std::string> StringVector;

/// @brief A collection of PTerminal.
///
/// Contains the terminal probability distribution for a certain step.
///
/// @remarks In *Python*, this class cannot be instantiated as it is only used
/// for reading. It behaves as (and can be converted to) a list.
typedef std::vector<PTerminal> PInput;


/// @brief A collection of ::PInput.
///
/// Contains several steps for a SParser to parse serially. Each step may
/// contain a different terminal probability distribution.
///
/// @remarks This type is **not** available in *Python*.
typedef std::vector<PInput> PInputs;

namespace impl
{
//Internal forward definitions
template<typename T>
class Array;

class Cell;
class CellUtils;
class Grammar;
class ParseTreeUtil;
class Production;
class SRule;
class SCell;
class SGrammar;
class SState;
class State;
class Token;
class Token;
class TokItem;
class KTokItem;

//Typedef pointer to classes
typedef Grammar* GrammarPtr;
typedef Production* ProductionPtr;
typedef const Production* KProductionPtr;
typedef Real *RealPtr;
typedef SRule* RulePtr;
typedef const SRule* KSRulePtr;
typedef SCell* SCellPtr;
typedef const SCell* KSCellPtr;
typedef SGrammar* SGrammarPtr;
typedef SParser* SParserPtr;
typedef SState* SStatePtr;
typedef const SState* KSStatePtr;
typedef Token* TokenPtr;
typedef const Token* KTokenPtr;

} // end of impl namespace

} // end of sartparser namespace

#endif /*__GLOBDEFS_H__*/
