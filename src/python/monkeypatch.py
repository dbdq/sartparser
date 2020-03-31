# Copyright (c) 2014 Miguel Sarabia
# Imperial College London
#
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
#

from .pysartparser import Status, PTerminal

#TODO replace RuntimeErrors by own exception
def getTokens(fp):
    tokens = []
    while True:
        line = fp.readline()
        if not line:
            return tokens
        else:
            nocomments = line.split('#',1)[0]
            tokens += nocomments.split()

def separate(tokens):
    terminals = []
    nonterminals = []
    axioms = []
    productions = []
    
    indices = [i for i, x in enumerate(tokens) if x== "Section"]

    # Create a list of lists separated by "Section"
    partition = [ tokens[i:j] for i, j in zip( indices, indices[1:]+[None] )]
    
    #Send the lists of tokens to its appropriate place
    for section in partition:
        if ( len(section) < 2):
            raise RuntimeError("Empty Section")
        elif section[1] == "Terminals":
            terminals += section[2:]
        elif section[1] == "NonTerminals":
            nonterminals += section[2:]
        elif section[1] == "Axiom":
            axioms += section[2:]
        elif section[1] == "Productions":
            productions += splitProductions(section[2:])
        else:
            raise RuntimeError("Unknown section name")
            
    return (terminals, nonterminals, axioms, productions)
        
def splitProductions(production):
    productions = []
    prevLHS = lhs = prob = None
    rhs = []
    nextIsProb = False
    finished = False

    def complete():
        return lhs is not None and rhs and prob is not None

    def empty():
        return lhs is None and not rhs and prob is None


    for elem in production:
        if elem[-1] == ":" :
            if not empty():
                raise RuntimeError("Unexpected start of production")
            lhs = elem[:-1]

        elif elem == "|":
            if prevLHS is None or not empty() :
                raise RuntimeError("Unexpected | symbol")
            lhs = prevLHS

        elif elem == "[":
            nextIsProb = True

        elif nextIsProb and prob is None:
            prob = float(elem)

        elif elem[0] == "[" and elem[-1] and prob is None and not nextIsProb:
            prob = float(elem[1:-1])
            finished = True
            
        elif nextIsProb and elem == "]":
            if not complete():
                raise RuntimeError("Unexpected ']', production is not fully defined")
            finished = True

        elif nextIsProb:
            raise RuntimeError("Unexpected {}, expected ']'".format(elem))

        else:
            rhs.append(elem)
        
        if finished:
            productions.append( (lhs, rhs, prob) )
            prevLHS = lhs
            lhs = prob = None
            rhs = []
            nextIsProb = False
            finished = False

    #Need to check for partially defined productions
    if not empty():
        raise RuntimeError("Production unexpectedly finished")

    return productions


def loadGrammar(grammar, fp):
    tokens = getTokens(fp)

    if tokens[0] != "Section":
        raise RuntimeError("Unexpected {}, expected 'Section' keyword".format(tokens[0]))

    terminals, nonterminals, axioms, productions = separate(tokens)

    for terminal in terminals:
        err = grammar.addTerminal(terminal)
        if err != Status.OK:
            return err

    for nonterminal in nonterminals:
        err = grammar.addNonTerminal(nonterminal)
        if err != Status.OK:
            return err

    for axiom in axioms:
        err =  grammar.addAxiom(axiom)
        if err != Status.OK:
            return err;

    for (lhs, rhs, prob) in productions:
        err = grammar.addRule(lhs, rhs, prob)
        if err != Status.OK:
            return err

    return Status.OK

def parseLine(parser, src, simple = True):
    if type(src) is str:
        line = src
    else:
        line = src.readline()

    tokens = line.split()

    if not tokens:
        return None

    parserInput = []
    index = 0
    for terminal in parser.getGrammar().getTerminals():
        if not terminal:
            continue

        prob = float(tokens[index])
        index += 1

        if simple:
            highMark = 0
            lowMark = 0

        else:
            highMark = float(tokens[index])
            index += 1

            lowMark = highMark - float(tokens[index])
            index += 1

        parserInput.append( PTerminal(terminal, prob, highMark, lowMark) )

    return parser.__parse(parserInput)


def parse(parser, *input):
    newInput = []

    for item in input:

        if type(item) == PTerminal:
            newInput.append(item)

        else:
            length = len(item)

            if length < 1 or length > 4:
                raise RuntimeError("Invalid length of arguments for PTerminal")

            p = PTerminal()

            p.terminal = str( item[0] )
            p.probability = float( item[1] ) if length > 1 else 1.0
            p.highMark = float( item[2] ) if length > 2 else 0.0
            p.lowMark = float( item[3] ) if length > 3 else 0.0

            newInput.append(p)

    return parser.__parse(newInput)

def stringVector2String( sv ):
    return str(list(sv))

def rules2String(rules):
    return str([ str(x) for x in rules ])
