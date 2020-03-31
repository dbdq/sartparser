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

#ifndef __SGRAMMAR_HPP
#define __SGRAMMAR_HPP

#include "Common.h"
#include "Grammar.impl.h"
#include "SRule.impl.h"
#include <Eigen/LU>

namespace sartparser
{
namespace impl
{


class SGrammar : public Grammar
{
   public:
      SGrammar() : Grammar()
         {partial_ = false;}
      virtual ~SGrammar() {}

      Status AddRule(
              const std::string& lhs,
              const StringVector& rhs,
              Real probability,
              const std::string& label = "");

      virtual Status CheckGrammar();
              Real GetPl(int i, int j) const {return Pl(i,j);}
              Real GetPu(int i, int j) const {return Pu(i,j);}
              Real GetRl(int i, int j) const {return Rl(i,j);}
              Real GetRu(int i, int j) const {return Ru(i,j);}

      void SetPartial(bool partial){partial_ = partial;}
      bool GetPartial() const { return partial_; }
   private:
      typedef Eigen::Matrix<Real, Eigen::Dynamic, Eigen::Dynamic> Matrix;
      Status ComputeClosures();

      void MakeR(const Matrix &aP, Matrix &aR);
      
      // Left corner, left corner closure, 
      // unit production and unit production closure 
      // matrices. Their size is N.GetCount() x N.GetCount()
      Matrix Pl, Rl, Pu, Ru;
      
   private:
      bool partial_;
};


}// end of impl namespace
}// end of sartparser namespace

#endif
