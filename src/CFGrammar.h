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

#ifndef CFGRAMMAR_H
#define CFGRAMMAR_H

#include "Common.h"

/// @file
/// @brief Contains CFGrammar definition.

namespace sartparser
{

/// @brief Struct to represent a production rule
/// @remarks In *Python*, this class cannot be instantiated and its members are
/// read-only.
struct Rule
{
    /// @brief The left-hand side of the production rule. It should contain a
    /// non-terminal which was already declared with addNonTerminal().
    std::string lhs;
    /// @brief A vector with the right-hand side of the production rule. It
    /// should contain one or more terminals or non-terminals already declared
    /// with addTerminal() and addNonTerminal().
    StringVector rhs;
    /// The transition probability for this production rule.
    Real probability;

    /// @brief Default Constructor
    Rule();
    /// @brief Constructor
    Rule(const std::string& lhs, const StringVector& rhs, Real probability);
    /// @brief Comparison operator
    bool operator==(const Rule& r);
};


/// @brief Class to represent Context Free Grammars.
///
/// This class is represents a stochastic Context-Free Grammar (CFG). A CFG is
/// defined by a finite set of terminals, a finite set non-terminals (or
/// variables), a set of production rules (from non-terminals to a combination
/// of non-terminals and terminals) and a starting variable (axiom).
/// Additionally for stochastic grammars, the transition probability for each
/// production is required.
/// This class provides methods to specify all these elements.
/// @note This class cannot be copied.
class CFGrammar
{
public:

    /// @brief Constructor
    explicit CFGrammar();
    /// @brief Destructor
    ~CFGrammar();

    /// @brief Define axiom (starting non-terminal)
    /// @param axiom Name of the axiom to be defined. If the axiom is not
    /// regcognised as a known non-terminal, the function will call
    /// addNonTerminal() with axiom.
    /// @returns sartparser::OK if everything went well.
    /// sartparser::ERR_ALREADYEXISTS if axiom previously defined.
    Status addAxiom(const std::string& axiom);

    /// @brief Add a terminal to the grammar.
    /// @param terminal Name of the terminal to be added. It must be different
    /// from any other non-terminals and terminals.
    /// @returns sartparser::OK if everything went well.
    /// @note function will ignore duplicate terminals (but still return
    /// sartparser::OK).
    Status addTerminal(const std::string& terminal);

    /// @brief Add a non-terminal to the grammar.
    /// @param nonterminal Name of the variable to be added. It must be
    /// different from any other non-terminals and terminals.
    /// @returns sartparser::OK if everything went well.
    /// @note function will ignore duplicate variables (but still return
    /// sartparser::OK).
    Status addNonTerminal(const std::string& nonterminal);

    /// @brief Add a production rule to the grammar.
    /// @param lhs The left-hand side of the production rule. It should contain
    /// a non-terminal which was already declared with addNonTerminal().
    /// @param rhs A vector with the right-hand side of the production rule. It
    /// should contain one or more terminals or non-terminals already declared
    /// with addTerminal() and addNonTerminal().
    /// @param probability The transition probability for this production rule.
    /// @returns sartparser::OK if everything went well.
    /// sartparser::ERR_NOTFOUND if lhs wasn't previously declared.
    /// sartparser::ERR_INVPARAM if rhs contains unknown terminals or
    /// non-terminals.
    /// @remarks In *Python*, rhs should be a list or a tuple containing
    /// only strings.
    Status addRule( const std::string& lhs,
                    const StringVector& rhs,
                    Real probability = 1.0 );

    /// @brief Add a production rule to the grammar.
    /// @param r Rule to be added.
    /// @returns sartparser::OK if everything went well.
    /// sartparser::ERR_NOTFOUND if lhs wasn't previously declared.
    /// sartparser::ERR_INVPARAM if rhs contains unknown terminals or
    /// non-terminals.
    /// @remarks This method is **not** available in *Python*.
    Status addRule( const Rule& r );

    /// @brief Check grammar is well formed.
    ///
    /// Checks all the elements required for the grammar have been defined.
    /// In particular check that:
    ///  * Terminals, non-terminals, productions and axiom have been defined.
    ///  * Terminals and non-terminals do not share an identifier.
    ///  * All non-terminals have at least one expansion rule.
    ///  * No terminals are present on the LHS of a production.
    ///  * The rules for each non-terminal add up to 1.
    ///
    /// @returns sartparser::OK if all checks passed,
    /// sartparser::ERR_INVPARAM otherwise.
    Status checkGrammar();

    /// @brief Get list of terminals already defined in the grammar.
    /// @returns A StringVector with all terminals already defined.
    StringVector getTerminals() const;

    /// @brief Get list of non-terminals already defined in the grammar.
    /// @returns A StringVector with all non-terminals already defined.
    StringVector getNonTerminals() const;

    /// @brief Get axiom defined in the grammar.
    /// @returns The axiom (will be empty if undefined.
    const std::string& getAxiom() const;

    /// @brief Get all the rules declared in this grammar.
    /// @returns A vector of Rule that the current grammar contains.
    Rules getRules() const;

private:
    //Forbid copying
    CFGrammar(const CFGrammar& );
    CFGrammar& operator=(const CFGrammar&);

    class Impl;
    Impl* pimpl_;

    friend class SParser;
    friend class Stream;

};

} // end of sartparser

#endif // CFGRAMMAR_H
