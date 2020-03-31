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

#ifndef __SPARSER_HPP
#define __SPARSER_HPP

#include <limits>
#include "Common.h"

/// @file
/// @brief Contains SParser definition.


namespace sartparser
{


/// @brief Class to perform stochastic grammar parsing.
///
/// This class performs stochastic grammar parsing. SParser requires a CFGrammar
/// in the constructor and a collection of PTerminals which represent concurrent
/// terminal symbols and their probabilities.
/// @note This class cannot be copied.
class SParser
{
public:
    /// @brief Constructor
    /// @param cfg The Stochastic Grammar which to be used for parsing.
    /// @throw std::invalid_argument If SCFGrammar::checkGrammar() fails.
    /// @note The grammar is passed by non-const reference and may be modified
    /// at any point. Users in multithreaded environments should be particularly
    /// careful about this.
    SParser(CFGrammar& cfg);

    /// @brief Destructor
    ~SParser();

    /// @brief Parse a terminal or set of concurrent terminals.
    /// @param input The probability of all grammar terminals for this parsing
    /// step. If a terminal is not present its probability will be assumed to be
    /// zero.
    /// @return sartparser::OK if everything went well. Another
    /// sartparser::Status otherwise.
    /// @remarks In *Python*, this method accepts **either** a list of PTerminal
    /// **or** a list of lists with two or four elements corresponding to the
    /// members of PTerminal.
    Status parse(const PInput& input);

    /// @brief Perform several parsing steps at once.
    /// @param inputs One or more sets of concurrent grammar terminals.
    /// @return sartparser::OK if everything went well. Another
    /// sartparser::Status otherwise.
    /// @remarks This method is **not** available in *Python*.
    Status parse(const PInputs& inputs);

    /// @brief Reset this parser.
    ///
    /// Discard all information from previous parse() calls and start from
    /// scratch.
    void reset();

    /// @brief Get the maximum alpha value of all candidate states
    /// @return The raw and normalised (with sequence length) maximum alpha
    /// value.
    ParseProbability getCurrentMaxAlpha() const;

    /// @brief Obtain the most likely next set of terminals, run a simulated
    /// step with them and obtain maximum alpha value of simulated step.
    /// @returns The probaility distribution of terminals and the raw and
    /// normalised max alpha values of the predicted step.
    Prediction getPrediction();

    /// @brief Obtain the Viterbi Parse (ie the most likely parse)
    /// @returns The viterbi parse, that is the sequence of terminals, the raw
    /// and scaled probability of the parse as well as the parse tree.
    ViterbiParse getViterbiParse();

    /// @brief Print debug information for all SParsers operations.
    /// @param debug The stream the information will be printed to.
    /// @remarks In *Python* this method does not take any arguments.
    void setDebug(std::ostream& debug);

    /// @brief Do not print debug information.
    void unsetDebug();

    /// @brief Get the grammar this parser is using.
    /// @returns A constant reference to the internal CFGrammar being used.
    const CFGrammar& getGrammar() const;

private:
    // Forbid copying
    SParser(const SParser&);
    SParser& operator=(const SParser& );

    class Impl;
    Impl* pimpl_;
};

} //end of sartparser namespace

#endif
