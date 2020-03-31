/*
 * Copyright (c) 2014 Miguel Sarabia
 * Imperial College London
 *
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

#ifndef VITERBIPARSE_H
#define VITERBIPARSE_H

#include "Common.h"
#include <map>


/// @file
/// @brief Contains definitions for structures needed by SParser.

namespace sartparser
{

/// @brief Class to represent raw and scaled probabilities.
/// @remarks In *Python*, this class cannot be instantiated and its members are
/// read-only.
struct ParseProbability
{
    /// @brief Raw probability.
    Real raw;
    /// @brief Scaled probability (using scaleLength):
    /// \f$P_{\mbox{scaled}} =
    /// \left(P_{\mbox{raw}}\right)^\frac{1}{\textrm{length}}\f$.
    Real scaled;
    /// @brief Scaling factor to transform between raw and scaled.
    size_t scaleLength;

    /// @brief Default constructor. Set all fields to 0.
    ParseProbability();
    /// @brief Constructor
    /// @param probability raw probability
    /// @param length normalising constant
    /// @param logSpace if true set raw probability represented as a log
    ParseProbability(Real probability, int length, bool logSpace);

    /// @brief Check the fields make sense
    /// @return True if probabilities between 0-1 and scaledLength > 1.
    bool isValid() const;
};

/// @brief Class to contain the tree that yields the viterbi parse.
///
/// This class contains the set of rules which produce the most likely parse of
/// the inputs given to SParser.
/// @note This class cannot be constructed by users.
class ParseTree
{
private:
    class Impl;
    Impl* pimpl_;
    friend struct impl::ParseTreeUtil;

public:
    /// @brief Default constructor.
    ParseTree();
    /// @brief Copy constructor (deep copy).
    ParseTree(const ParseTree&);
    /// @brief Assignment operator (deep copy).
    ParseTree& operator=(const ParseTree&);
    /// @brief Destructor.
    ~ParseTree();
    /// @cond
    ParseTree(Impl* pimpl);
    /// @endcond

    /// @brief Comparison operator.
    bool operator==(const sartparser::ParseTree& other) const;

    /// @brief Get the left hand side of the rule. That is, the nonterminal
    /// which is to be expanded.
    /// @returns The name of the nonterminal in the left-hand side of this rule.
    const std::string& lhs() const;
    /// @brief Get the right hand side of the rule. That is, the set of
    /// terminals and nonterminals which correspond to the left hand side.
    /// @returns A vector with the names of the terminals and nonterminals which
    /// correspond to the left hand side.
    const StringVector& rhs() const;
    /// @brief Get the alpha value for this rule. The alpha value denotes the
    /// forward probability, that is the probablity of the string up to this
    /// point.
    /// @returns The alpha value.
    Real alpha() const;
    /// @brief Get the gamma value for this rule. The gamma value denotes the
    /// inner probability.
    /// @returns The gamma value.
    Real gamma() const;
    /// @brief Get the viterbi probability for this rule.
    /// @returns The natural log of the viterbi probability up to this state.
    Real v() const;
    /// @brief Get the step at which this rule was considered by the parser (k).
    /// @returns The k value.
    size_t k() const;
    /// @brief Get the high mark for this rule.
    Real highMark() const;
    /// @brief Get the low mark for this rule.
    Real lowMark() const;
    /// @brief Get the children of this rule. That is the set of rules which
    /// expand the non-terminals on the right hand side of this rule.
    /// @returns A vector containing a ParseTree for each non-terminal on the
    /// rhs().
    const std::vector<ParseTree>& children() const;
};

/// @brief Struct to contain the Viterbi Parse.
///
/// This struct contains the terminals, the probability (scaled and unscaled)
/// as well as the ParseTree that yielded this parse.
/// @remarks In *Python*, this class cannot be instantiated and its members are
/// read-only.
struct ViterbiParse
{
    /// @brief The ordered set of terminals that were parsed.
    StringVector terminals;
    /// @brief The probability of generating this parse.
    ParseProbability probability;
    /// @brief The ParseTree, set of rules, which originated this parse.
    ParseTree parseTree;

    /// @brief Default constructor.
    ViterbiParse();
    /// @brief Constructor.
    ViterbiParse(
            const StringVector& terminals,
            const ParseProbability& probability,
            const ParseTree& parseTree);
};

/// @brief Class to contain predictions about next parsing step.
/// @see sartparser::SParser::getPrediction().
/// @remarks In *Python*, this class cannot be instantiated and its members are
/// read-only.
struct Prediction
{
    /// @brief Map from terminals to their probabilities
    typedef std::map<std::string, Real> ProbabilityDistribution;

    /// @brief Normalised probabilities of a terminal appearing at next parsing
    /// step.
    ProbabilityDistribution terminalDistribution;

    /// @brief Contains max alpha values for parser, should terminalDistribution
    /// be used as input for the next step.
    ParseProbability probability;
};

} //end of sartparser namespace

#endif // VITERBIPARSE_H
