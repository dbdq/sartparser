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

#include <fstream>
#include <stdexcept>

#include "../SParser.h"
#include "../CFGrammar.h"
#include "../Stream.h"
#include "../SParserUtils.h"
#include "../PTerminal.h"

#ifdef USE_CLPARSER
#include "../opencl/CLParser.h"
const bool clEnabled = true;
#else
const bool clEnabled = false;
#endif

#ifdef USE_CXX11
#include <chrono>

const bool chronoEnabled = true;

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<Clock> Timepoint;

inline Timepoint now()
{
    return Clock::now();
}

inline double getDuration(const Timepoint& start, const Timepoint& end)
{
    using namespace std::chrono;
    return duration_cast<nanoseconds>(end - start).count()/1e9;
}
#else
const bool chronoEnabled = false;
typedef void* Timepoint;

inline Timepoint now() { return NULL;}
inline double getDuration(const Timepoint&, const Timepoint&){ return 0; }
#endif

using namespace sartparser;
//==============================================================================
// CLASS TO STORE PROGRAM OPTIONS
//==============================================================================
class Options
{
public:

    Options();
    ~Options();

    void init(int argc, char** argv);
    std::istream& grammarStream();
    std::istream& dataStream();
    std::ostream& output();

    bool debug() const;
    bool predict() const;
    bool useCL() const;
    bool benchmark() const;

    const static std::string help;

private:

    //Forbid copying
    Options(const Options&);
    Options& operator=(const Options&);

    std::ifstream* grammarStream_;
    std::ifstream* dataStream_;
    std::ofstream* outputStream_;

    bool debug_;
    bool predict_;
    bool useCL_;
    bool benchmark_;

    void deletePtrs();
};

//==============================================================================
// OPTIONS IMPLEMENTATION
//==============================================================================
Options::Options()
    : grammarStream_(NULL)
    , dataStream_( NULL )
    , outputStream_( NULL)
    , debug_(false)
    , predict_(false)
    , useCL_(false)
    , benchmark_(false)
{
}

Options::~Options()
{
    deletePtrs();
}

void Options::init(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg( argv[i] );
        if (arg[0] == '-')
        {
            if (arg == "--debug")
            {
                debug_ = true;
            }
            else if (arg == "--predict")
            {
                predict_ = true;
            }
            else if (arg == "--cl")
            {
                if (clEnabled)
                {
                    useCL_ = true;
                }
                else
                {
                    throw std::runtime_error("OpenCL support was not built");
                }
            }
            else if (arg == "--benchmark")
            {
                if (chronoEnabled)
                {
                    benchmark_ = true;
                }
                else
                {
                    throw std::runtime_error(
                                "Benchmarking support was not built");
                }
            }
            else
            {
                throw std::runtime_error("Unknown option: " + arg);
            }
        }
        else if ( grammarStream_  == NULL )
        {
            grammarStream_ = new std::ifstream(arg.c_str() );
            if( !grammarStream_->is_open() )
            {
                deletePtrs();
                throw std::runtime_error("Error opening grammar file: " + arg);
            }
        }
        else if ( dataStream_ == NULL)
        {
            dataStream_ = new std::ifstream(arg.c_str());
            if( !dataStream_->is_open() )
            {
                deletePtrs();
                throw std::runtime_error("Error opening data file: " + arg );
            }
        }
        else if ( outputStream_ == NULL)
        {
            outputStream_ = new std::ofstream(arg.c_str());
            if (! outputStream_->is_open() )
            {
                deletePtrs();
                throw std::runtime_error("Error opening output file: " + arg );
            }
        }
        else
        {
            deletePtrs();
            throw std::runtime_error("Too many arguments provided");
        }
    }//End for loop through arguments

    if ( grammarStream_ == NULL )
    {
        deletePtrs();
        throw std::runtime_error("Required grammar file not specified");
    }
}


std::istream& Options::grammarStream()
{
    if ( !grammarStream_ )
        throw std::runtime_error("Missing grammar file information");
    return *grammarStream_;
}

std::istream& Options::dataStream()
{
    return (dataStream_)? *dataStream_ : std::cin;
}

std::ostream& Options::output()
{
    return (outputStream_) ? * outputStream_ : std::cout;
}

bool Options::debug() const
{
    return debug_;
}

bool Options::predict() const
{
    return predict_;
}

bool Options::useCL() const
{
    return useCL_;
}

bool Options::benchmark() const
{
    return benchmark_;
}

void Options::deletePtrs()
{
    delete grammarStream_;
    delete dataStream_;
    delete outputStream_;
}

const std::string Options::help =
        "Usage: grammar_file [data_file] [output_file]"
        "[--debug] [--predict] [--cl] [--benchmark] \n"
        "\nOptions:\n"
        "\tgrammar_file   Input grammar file\n"
        "\t[data_file]    Input sequence data"
        "(Optional, defaults to standard input)\n"
        "\t[output_file]  Output file"
        "(Optional, defaults to standard output)\n"
        "\t[--debug]      Print parsing debug information\n"
        "\t[--predict]    Print intermidiate predictions\n"
        "\t[--cl]         Use OpenCL accelerated parser\n"
        "\t[--benchmark]  Measure total parsing time\n";


template<typename T>
Status parse(CFGrammar& grammar, Options& options)
{
    Status retCode;
    try
    {
        T parser(grammar);
        double duration = 0;
        Timepoint start;

        if ( options.debug() )
        {
            parser.setDebug( options.output() );
        }

        if ( options.predict() )
        {
            options.output() << "Current max alpha: "
                             << parser.getCurrentMaxAlpha()
                             << "Next step predicition" << std::endl
                             << parser.getPrediction();
        }

        while ( true )
        {
            PInput line;
            retCode = loadSimpleParseLine(
                        options.dataStream(),
                        line,
                        grammar);

            //Check if we need to break (EOF is acceptable)
            if (retCode != OK)
            {
                if (retCode == ERR_EOF)
                {
                    retCode = OK;
                }
                break;
            }


            //Parse a new line
            if ( options.benchmark() )
            {
                start = now();
            }
            retCode = parser.parse(line);
            if ( options.benchmark() )
            {
                duration += getDuration( start, now() );
            }
            if (retCode != OK)
            {
                break;
            }

            if ( options.predict() )
            {
                options.output() << "Current max alpha: "
                                 << parser.getCurrentMaxAlpha()
                                 << "Next Step Predicition" << std::endl
                                 << parser.getPrediction();
            }
        }

        if( retCode != OK)
        {
            std::cerr << "Error encountered parsing sentence" << std::endl;
            return retCode;
        }
        else
        {
            ViterbiParse viterbiParse = parser.getViterbiParse();
            options.output() << "Done." << std::endl;
            if ( options.debug() )
            {
                options.output() << viterbiParse.parseTree << std::endl;
            }
            options.output() << viterbiParse;
        }
        if ( options.benchmark() )
        {
            options.output() << "Total parsing time: "
                             << duration << "s" << std::endl;
        }
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Failed to initialise parser: " << e.what() << std::endl;
        retCode = ERR_INVPARAM;
    }

    return retCode;

}

//==============================================================================
// MAIN()
//==============================================================================
int main(int argc, char **argv)
{
    Options options;
    try
    {
        options.init(argc, argv);
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << Options::help << std::endl;
        return -1;
    }

    Status retCode;

    CFGrammar grammar;
    retCode = sartparser::loadGrammar(options.grammarStream(), grammar);
    if (retCode != sartparser::OK)
    {
        std::cerr << "Error: Grammar file read error" << std::endl;
        return -2;
    }
    if ( options.debug() )
    {
        options.output() << grammar;
        dumpGrammarMatrices( options.output(), grammar);
    }

    //Do actual parsing
#ifdef USE_CLPARSER
    if ( options.useCL() )
    {
        retCode = parse<CLParser>(grammar, options);
    }
    else
    {
        retCode = parse<SParser>(grammar, options);
    }
#else
    retCode = parse<SParser>(grammar, options);
#endif

    if( retCode != OK )
    {
        return -3;
    }
    else
    {
        return 0;
    }
}

