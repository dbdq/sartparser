# SARTParser Tutorial

SARTParser can be used from C++ and Python. The interfaces of both languages are
very similar (though not identical to account for idiomatic differences).

In the examples below we define a grammar with the following production rules:
  * `S: a b [0.5]`
  * `S: a S b [0.5]`

where `S` is the only non-terminal (and axiom) and `a` and `b` are the
terminals. We then proceed to input a terminal whose probability of being `a` or
`b` is 0.8 and 0.2 respectively (we denote this as `a[0.8] b[0.2]`).

After that, we input `a[0.3] b[0.7]` and request the viterbi parse (that is, the
most likely parse of the grammar).

## C++

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
#include <iostream>
#include <fstream>
#include <SARTParser/All.h>

int main()
{
    //Construct grammar
    sartparser::CFGrammar g;

    //Add terminals
    g.addTerminal("a");
    g.addTerminal("b");

    //Add non-terminals
    g.addNonTerminal("S");

    //Add axiom
    g.addAxiom("S");

    //Add production rules
    sartparser::Rule firstRule;
    firstRule.lhs = "S";
    firstRule.rhs.push_back("a");
    firstRule.rhs.push_back("b");
    firstRule.probability = 0.5;

    g.addRule(firstRule); //Adding S: a b [0.5]

    sartparser::Rule secondRule;
    secondRule.lhs = "S";
    secondRule.rhs.push_back("a");
    secondRule.rhs.push_back("S");
    secondRule.rhs.push_back("b");
    secondRule.probability = 0.5;

    g.addRule(secondRule); //Adding S: a S b [0.5]

    //Create parser
    sartparser::SParser p(g);

    //Parse one step
    sartparser::PInput firstStep;
    firstStep.push_back( sartparser::PTerminal("a", 0.8) );
    firstStep.push_back( sartparser::PTerminal("b", 0.2) );

    p.parse(firstStep); //Parsing a[0.8] b[0.2]

    //Parse another step
    sartparser::PInput secondStep;
    secondStep.push_back( sartparser::PTerminal("a", 0.3) );
    secondStep.push_back( sartparser::PTerminal("b", 0.7) );

    p.parse(secondStep); //Parsing a[0.3] b[0.7]

    //Get viterbi parse results
    std::cout << p.getViterbiParse() << std::endl;

    return 0;
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To compile this code you may want to call something like:

    g++ -o tutorial -I</path/to/SARTParser>/include -L</path/to/SARTParser>/lib -lSARTParser tutorial.cpp

If instead of defining the grammar by hand you wanted to read it from a file,
you could write this code:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
std::ifstream grammarFile("/path/to/grammar");
sartparser::loadGrammar(grammarFile, grammar);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can also read a parsing step with these commands:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
std::ifstream sequenceFile("/path/to/sequence");
sartparser::PInput input;
sartparser::loadSimpleParseLine(sequenceFile, input);
parser.parse(input);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


## Python

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.py}
#!/usr/bin/env python

import sartparser

def main():
    #Initialise grammar
    g = sartparser.CFGrammar()

    #Add terminals
    g.addTerminal("a")
    g.addTerminal("b")

    #Add non-terminals
    g.addNonTerminal("S")

    #Add axiom
    g.addAxiom("S")

    #Add production rules
    g.addRule("S", ["a", "b"], 0.5) #Adding S: a b [0.5]
    g.addRule("S", ["a", "S", "b"], 0.5) #Adding S: a S b [0.5]

    #Create parser
    p = sartparser.SParser(g)

    #Parse one step
    p.parse( ["a", 0.8], ["b", 0.2] ) #Parsing a[0.8] b[0.2]

    #Parse another step
    p.parse( ["a", 0.3], ["b", 0.7] ) #Parsing a[0.3] b[0.7]

    #Get viterbi parse results
    print( p.getViterbiParse() )


if __name__ == "__main__":
    main()

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As you can observe the Python interface is very similar to the C++ one. The main
lies in that sartparser.CFGrammar.addRule() wants a list (or tuple) of elements
rather than a sartparser.StringVector.

Similarly, sartparser.SParser.parse() requires a list of tuples with the
terminals and probabilities (though it also accepts a list of
sartparser.PTerminal).

If we wanted to load the grammar from a file we would issue the following
instructions (note grammar must already have been instantiated):

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.py}
with open("/path/to/grammar") as fp:
    grammar.load(fp)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Similarly, we can perform a parsing step with a sequence from a file:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.py}
with open("/path/to/sequence") as fp:
    for line in fp:
        parser.parseLine(line)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## Output

Whether you use C++ or Python the output of the program should be:

    Parse: a b     Probability = 0.28    Scaled probability = 0.5292

This means that the parser interpreted the first input as an `a`, and the second
as `b`.

The probability of this sequence being generated by the above grammar is 0.28.
The longer the parse sequence the smaller the generation probability will be,
that is why often we scale the probability with the length of the sequence, thus
obtaining the scaled probability.
