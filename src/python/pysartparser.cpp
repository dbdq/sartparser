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

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <iomanip>

#include "../All.h"


using namespace sartparser;
namespace py = boost::python;

//==============================================================================
// LIST EXTRACTOR FOR VECTOR
//==============================================================================
template<typename T>
std::vector<T> getVector(const py::object& o)
{
    //Otherwise extract a list and convert all members
    std::vector<T> result;
    for (int i =0; i< len(o); ++i)
    {
        result.push_back( py::extract<T>( o[i]) );
    }
    return result;
}

//==============================================================================
// STRING METHODS
//==============================================================================
template<typename T>
std::string toString(const T& t)
{
    std::ostringstream oss;
    oss << t;
    return oss.str();
}

//==============================================================================
// WRAPPERS FOR PYTHON MODULE (mainly dealing with native python lists)
//==============================================================================
Status addRuleWrapper(
        CFGrammar& cfg,
        const std::string& lhs,
        const py::object& rhs,
        Real prob)
{
    StringVector rhs2 = getVector<std::string>(rhs);
    return cfg.addRule(lhs, rhs2, prob);
}

Status parseWrapper( SParser& parser, const py::object& input)
{
    PInput input2 = getVector<PTerminal>(input);
    return parser.parse(input2);
}

void setDebugWrapper(SParser& parser)
{
    parser.setDebug(std::cout);
}

py::dict distribution2Dict(const Prediction& p)
{
    py::dict result;
    //Iterate through map and load values in a python dictionary
    typedef Prediction::ProbabilityDistribution::const_iterator Iterator;
    for( Iterator it = p.terminalDistribution.begin();
         it!= p.terminalDistribution.end();
         ++it )
    {
        result[it->first] = it->second;
    }
    return result;
}


//==============================================================================
// MODULE DEFINITION
//==============================================================================
BOOST_PYTHON_MODULE(pysartparser)
{
    py::enum_<Status>("Status")
            .value("OK", OK)
            .value("ERR_INVPARAM", ERR_INVPARAM)
            .value("ERR_READINGFILE", ERR_READINGFILE)
            .value("ERR_OUTOFMEMORY", ERR_OUTOFMEMORY)
            .value("ERR_OUTOFBOUNDS", ERR_OUTOFBOUNDS)
            .value("ERR_NOTFOUND", ERR_NOTFOUND)
            .value("ERR_ALREADYEXISTS", ERR_ALREADYEXISTS)
            .value("ERR_EOF", ERR_EOF)
            .value("ERR_REJECTED", ERR_REJECTED);

    py::class_<StringVector>("StringVector", py::no_init)
            .def( py::vector_indexing_suite<StringVector>() );

    py::class_<PInput>("PInput", py::no_init)
            .def( py::vector_indexing_suite<PInput>() );

    py::class_<std::vector<ParseTree> > ("ParseTrees", py::no_init)
            .def(py::vector_indexing_suite<std::vector<ParseTree> >());

    py::class_<Rules>("Rules", py::no_init)
            .def(py::vector_indexing_suite<Rules>());

    py::class_<PTerminal>("PTerminal")
            .def( py::init<std::string, Real, py::optional<Real, Real> >() )
            .def_readwrite("terminal", &PTerminal::terminal)
            .def_readwrite("probability", &PTerminal::probability)
            .def_readwrite("highMark", &PTerminal::highMark)
            .def_readwrite("lowMark", &PTerminal::lowMark)
            .def("__str__", &toString<PTerminal>);

    py::class_<ParseProbability>("ParseProbability", py::no_init)
            .def("isValid", &ParseProbability::isValid )
            .def_readonly("raw", &ParseProbability::raw )
            .def_readonly("scaled", &ParseProbability::scaled )
            .def_readonly("scaleLength", &ParseProbability::scaleLength )
            .def( "__str__", &toString<ParseProbability> );

    // Define the return policy for const refs, if I'm not mistaken this should
    // link the lifetimes of the objects and return a straight reference of
    // the object.
    typedef py::return_internal_reference<1,
            py::return_value_policy<py::reference_existing_object> >
            ReturnPolicy;

    py::class_<ParseTree> ("ParseTree", py::no_init)
            .def("lhs", &ParseTree::lhs, ReturnPolicy())
            .def("rhs", &ParseTree::rhs, ReturnPolicy())
            .def("alpha", &ParseTree::alpha)
            .def("gamma", &ParseTree::gamma)
            .def("v", &ParseTree::v)
            .def("k", &ParseTree::k)
            .def("highMark", &ParseTree::highMark)
            .def("lowMark", &ParseTree::lowMark)
            .def("children", &ParseTree::children, ReturnPolicy() )
            .def("__str__", &toString<ParseTree> );

    py::class_<ViterbiParse>("ViterbiParse", py::no_init)
            .def_readonly("terminals", &ViterbiParse::terminals)
            .def_readonly("probability", &ViterbiParse::probability)
            .def_readonly("parseTree", &ViterbiParse::parseTree)
            .def( "__str__", &toString<ViterbiParse> );

    py::class_<Prediction>("Prediction", py::no_init)
            .add_property( "terminalDistribution", &distribution2Dict)
            .def_readonly( "probability", &Prediction::probability)
            .def("__str__", &toString<Prediction> );

    py::class_<Rule>("Rule", py::no_init)
            .def_readonly("lhs", &Rule::lhs)
            .def_readonly("rhs", &Rule::rhs)
            .def_readonly("probability", &Rule::probability)
            .def("__str__", &toString<Rule>);

    //Copy value rather than returning an internal
    typedef py::return_value_policy<py::copy_const_reference>
            CopyReturnPolicy;

    py::class_<CFGrammar, boost::noncopyable>( "CFGrammar" )
            .def("addAxiom", &CFGrammar::addAxiom )
            .def("addTerminal", &CFGrammar::addTerminal )
            .def("addNonTerminal", &CFGrammar::addNonTerminal )
            .def("addRule", &addRuleWrapper)
            .def("checkGrammar", &CFGrammar::checkGrammar )
            .def("getTerminals", &CFGrammar::getTerminals )
            .def("getNonTerminals", &CFGrammar::getNonTerminals )
            .def("getAxiom", &CFGrammar::getAxiom, CopyReturnPolicy() )
            .def("getRules", &CFGrammar::getRules)
            .def("__str__", &toString<CFGrammar> );

    //Link lifetimes of CFGrammar and SParser
    typedef py::with_custodian_and_ward<1,2> ConstructorPolicy;

    py::class_<SParser, boost::noncopyable>
            ("SParser", py::init<CFGrammar&>()[ConstructorPolicy()])
            .def("__parse", parseWrapper )
            .def("reset", &SParser::reset)
            .def("getCurrentMaxAlpha", &SParser::getCurrentMaxAlpha)
            .def("getPrediction", &SParser::getPrediction)
            .def("getViterbiParse", &SParser::getViterbiParse)
            .def("setDebug", setDebugWrapper )
            .def("unsetDebug", &SParser::unsetDebug )
            .def("getGrammar", &SParser::getGrammar, ReturnPolicy() );
}
