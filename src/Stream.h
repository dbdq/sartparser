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

#ifndef STREAM_H
#define STREAM_H

#include"Common.h"

/// @file
/// @brief Contains streaming functions.

namespace sartparser
{
/// @brief Extract grammar information (terminals, non-terminals, axiom and
/// productions) from stream into grammar.
/// @param i The input stream.
/// @param cfg The destination grammar.
/// @returns sartparser::OK if everything went well, sartparser::ERR_READINGFILE
/// otherwise.
/// @remarks This function is **not** available in *Python*. Use
/// CFGrammar.load() instead.
Status loadGrammar(std::istream& i, CFGrammar& cfg);

/// @brief Load a set of concurrent terminals and their probabilities from
/// a stream.
/// @param i The input stream.
/// @param input The destination object, it can be passed to SParser::parse().
/// @param cfg Underlying grammar. Determines the order in which the terminals
/// are read.
/// @returns sartparser::OK if everything went well, sartparser::ERR_READINGFILE
/// otherwise.
/// @remarks This function is **not** available in *Python*. Use
/// SParser.parseLine() instead.
Status loadSimpleParseLine( std::istream& i,
                            PInput& input,
                            const CFGrammar& cfg);

/// @brief Call loadSimpleParseLine() until the end-of-file is reached.
/// @param i The input stream.
/// @param inputs The destination object, it can be passed to SParser::parse().
/// @param cfg Underlying grammar. Determines the order in which the terminals
/// are read.
/// @returns sartparser::OK if everything went well, sartparser::ERR_READINGFILE
/// otherwise.
/// @remarks This function is **not** available in *Python*. Use
/// SParser.parseLine() instead.
Status loadSimpleParseFile( std::istream& i,
                            PInputs& inputs,
                            const CFGrammar& cfg);

/// @brief Load a set of concurrent terminals, their probabilities, and their
/// high and low marks from a stream.
/// @param i The input stream.
/// @param input The destination object, it can be passed to SParser::parse().
/// @param cfg Underlying grammar. Determines the order in which the terminals
/// are read.
/// @returns sartparser::OK if everything went well, sartparser::ERR_READINGFILE
/// otherwise.
/// @remarks This function is **not** available in *Python*. Use
/// SParser.parseLine() instead.
Status loadParseLine( std::istream& i,
                      PInput& input,
                      const CFGrammar& cfg);

/// @brief Call loadParseLine() until the end-of-file is reached.
/// @param i The input stream.
/// @param inputs The destination object, it can be passed to SParser::parse().
/// @param cfg Underlying grammar. Determines the order in which the terminals
/// are read.
/// @returns sartparser::OK if everything went well, sartparser::ERR_READINGFILE
/// otherwise.
/// @remarks This function is **not** available in *Python*. Use
/// SParser.parseLine() instead.
Status loadParseFile( std::istream& i,
                      PInputs& inputs,
                      const CFGrammar &cfg );

/// @brief Dump the left corner, left corner closure, unit production and unit
/// production closure matrices from cfg into a stream.
/// @remarks This function is **not** available in *Python*.
void dumpGrammarMatrices( std::ostream& o, const CFGrammar &cfg);

/// @brief Stream-insertion operator for CFGrammar.
/// @remarks In *Python*, this overload is available through the str() function.
std::ostream& operator<<(std::ostream& o, const CFGrammar& cfg );

/// @brief Stream-insertion operator for PTerminal.
/// @remarks In *Python*, this overload is available through the str() function.
std::ostream& operator<<(std::ostream& o, const PTerminal& t);

/// @brief Stream-insertion operator for ParseTree.
/// @remarks In *Python*, this overload is available through the str() function.
std::ostream& operator<<(std::ostream& o, const ParseTree& t);

/// @brief Stream-insertion operator for ViterbiParse.
/// @remarks In *Python*, this overload is available through the str() function.
std::ostream& operator<<(std::ostream& o, const ViterbiParse& v);

/// @brief Stream-insertion operator for Prediction.
/// @remarks In *Python*, this overload is available through the str() function.
std::ostream& operator<<(std::ostream& o, const Prediction& p);

/// @brief Stream-insertion operator for ParseProbability.
/// @remarks In *Python*, this overload is available through the str() function.
std::ostream& operator<<(std::ostream& o, const ParseProbability& p);

/// @brief Stream-insertion operator for Rule.
/// @remarks In *Python*, this overload is available through the str() function.
std::ostream& operator<<(std::ostream& o, const Rule& r);

}// end of sartparser namespace

#endif // STREAM_H
