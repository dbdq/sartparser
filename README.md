(This is a full clone of Miguel SDC's [repo](https://bitbucket.org/miguelsdc/sartparser) for backup purpose only.)

SARTParser is an open source library which can parse Stochastic Context Free
Grammars.

It was originally created by _Yuri Ivanov_ in 1997 and it was recently cleaned up
by _Miguel Sarabia_ (with some help from _KyuHwa Lee_).

### Related bibliography:

 * Lee K, Su Y, Kim TK, Demiris Y (2013) [A Syntactic Approach to Robot Imitation Learning using Probabilistic Activity Grammars](http://dx.doi.org/10.1016/j.robot.2013.08.003), Robotics and Autonomous Systems, 61(12), pp:1323-1334
 * Ivanov Y, Bobick AF (2000) [Recognition of Visual Activities and Interactions by Stochastic Parser](http://dx.doi.org/10.1109/34.868686), IEEE Transactions on Pattern Analysis and Machine Intelligence, 22(8), pp:852-872.
 * Stolcke A (1995) [An Efficient Probabilistic Context-Free Parsing Algorithm that Computes Prefix Probabilities](http://dl.acm.org/citation.cfm?id=211197), Computational Linguistics, 21(2), pp:165-201.

### Installation instructions

SARTParser depends only on `Eigen` (> 3.0.0) and requires `CMake` (> 2.8.3). `Doxygen` (> 1.8.6) is needed to generate the documentation. For the python bindings `Boost.Python` and the devel files for Python should be available as well.

On ubuntu, you can install these dependencies with:

    sudo apt-get install libeigen3-dev cmake doxygen libboost-python-dev python-dev

If you're reading this you probably already have the source code, but otherwise you can download it from [here](https://bitbucket.org/miguelsdc/sartparser/get/tip.zip). Alternatively, if you have `mercurial` installed you may clone the project:

    hg clone https://bitbucket.org/miguelsdc/sartparser SARTParser

Compilation follows the normal template of CMake projects; that is:

    cd SARTParser
    mkdir build
    cmake ../ -DCMAKE_BUILD_TYPE=Release
    make
    make install

If you want to install the Python bindings in your system, issue:

    make install_python

Optionally, to generate the documentation execute:

    make doc


### Q&A:

**Q: Can I browse the documentation online?**

_A_: Sure thing, just follow this [link](http://miguelsdc.bitbucket.org/SARTParser).

---

**Q: Is there a tutorial to get me started?**

_A_: Yup, [online](http://miguelsdc.bitbucket.org/SARTParser/md_TUTORIAL.html) and under the root of the project (`TUTORIAL.md`).

---

**Q: Is this thing fast?**

_A_: Reasonably so. The library is written in C++, and has been profiled and sped-up several times. However, stochastic grammars have an exponential parsing complexity, so if you give it a very complex grammar or too many inputs it will eventually grind your computer to a halt.

To get a feeling for it, you may want to run a few of the examples.

---

**Q: What can you tell me about multi-threading?**

_A_: The library does not use threads internally and the different classes are
_not_ thread-safe. If using in a multithreaded environment you should ensure
single-access before calling any non-const methods.

However, the library does not have static instances and there is nothing to stop
you from running several different parsers concurrently.

---

**Q: Why does the code look so insconsistent?**

_A_: Some parts of the code go back to 1997 (that is, before C++ was an ISO standard!). And there have been different authors with different coding styles. Patches are always welcome though.

---

**Q: Is there any support for Python3?**

_A_: The actual bindings are bilingual (courtesy of `Boost.Python`). However, in order to keep the CMake files sane, only the version of Boost.Python supports _by default_ will be used. In most machines that will be Python2. But in some it will be Python3. CMake will tell you for which version it's compiling the bindings.

---

**Q: But I want to use Python <X.Y>?!**

_A_: Your best bet is to compile your own `Boost.Python` which defaults to the correct version of Python. Alternatively, you may want to hack `src/python/CMakeLists.txt` to make it bow to your wishes.
