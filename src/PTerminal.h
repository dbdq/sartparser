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

#ifndef __PTERMINAL_HPP
#define __PTERMINAL_HPP

#include "Common.h"

namespace sartparser
{

/// @file
/// @brief Contains definition of PTerminal.

/// @brief Struct to contain probabilisitc terminals.
///
/// Struct to represent the a probabilistic terminal. In other words, this
/// struct represents the _probability_ (or confidence) that _terminal_ has
/// actually been observed. ::PInput is a collection of PTerminals which defines
/// the probability over all grammar terminals for a parsing step.
/// This struct is copyable and assignable.
struct PTerminal
{
    std::string terminal; ///< The name of the terminal this object represents.
    Real probability; ///< The probability of this terminal actually occurring.
    Real highMark; ///< Used for filtering invalid states.
    Real lowMark; ///< Used for filtering invalid states.

    PTerminal();///< Default constructors. Set terminal to \"\" and values to 0.

    /// @brief Constructor, sets values of all members.
    ///
    /// @arg terminal The name of the terminal this objects represents.
    /// @arg probability The probability of this terminal actually occurring.
    /// @arg highMark Used for filtering invalid states. Defaults to 0.
    /// @arg lowMark Used for filtering invalid states. Defaults to 0.
    PTerminal(const std::string& terminal,
          Real probability,
          Real highMark = 0.0,
          Real lowMark = 0.0);

    bool operator==(const PTerminal& t) const; ///< Comparison operator
};

inline PTerminal::PTerminal()
    : terminal("")
    , probability(1.0)
    , highMark(0.0)
    , lowMark(0.0)
{
}

inline PTerminal::PTerminal(
        const std::string& word,
        Real probability,
        Real highMark,
        Real lowMark)
    : terminal(word)
    , probability(probability)
    , highMark(highMark)
    , lowMark(lowMark)
{
}

inline bool PTerminal::operator==(const PTerminal& t) const
{
    return ( terminal == t.terminal &&
             probability == t.probability &&
             t.highMark == t.highMark &&
             t.lowMark == t.lowMark);
}


}// end of namespace sartparser

#endif
