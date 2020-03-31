/*
 * Copyright (c) 2014 Miguel Sarabia del Castillo
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

#include "CellUtils.impl.h"
#include "SCell.impl.h"

using namespace sartparser;
using namespace impl;

SCell* CellUtils::getCellByIndex(SCell* start, size_t index )
{

    if( start->I == index )
        return start;

    SCell* cell = start;
    if( index < start->I )
    {
        while(cell && cell->I != index)
            cell = cell->PrevCell;
    }
    else
    {
        while(cell && cell->I != index)
            cell = cell->NextCell;
    }
    return cell;
}

SCell* CellUtils::getNext(SCell* cell)
{
    return cell->NextCell;
}

SCell* CellUtils::getPrev(SCell* cell)
{
    return cell->PrevCell;
}

void CellUtils::setNext(SCell* cell, SCell* next)
{
    if( cell->NextCell )
    {
        destroyCells(cell, false); // forward
    }
    cell->NextCell = next;
}

void CellUtils::setPrev(SCell* cell, SCell* prev)
{
    if(cell->PrevCell)
        destroyCells(cell, true);  // bacwards
    cell->PrevCell = prev;
}

void CellUtils::destroyCells(SCell* start, bool backwards )
{
    SCell* cell = NULL;
    if(backwards)
    {
        cell = start->PrevCell;
        start->PrevCell = NULL;
    }
    else
    {
        cell = start->NextCell;
        start->NextCell = NULL;
    }

    while(cell)
    {
        SCell* newCell;
        if(backwards)
            newCell = cell->PrevCell;
        else
            newCell = cell->NextCell;

        delete cell;
        cell = newCell;
    }
}

SCell* CellUtils::split(SCell* cell)
{
    //This method only works on the last cell (and if there's more than 1 cell)
    if ( !cell || cell->NextCell == NULL )
        return NULL;

    SCell* result = cell->NextCell;
    result->PrevCell = NULL;
    cell->NextCell = NULL;

    return result;
}

void CellUtils::dumpCell(SCell* cell, std::ostream& o)
{
    o << "States:" << std::endl;

    o << "\tScanned:" << std::endl;
    dumpCellState(cell, o, SState::SCANNED);

    o << "\tCompleted:" << std::endl;
    dumpCellState(cell, o, SState::COMPLETED);

    o << "\tPredicted:" << std::endl;
    dumpCellState(cell, o, SState::PREDICTED);
}

void CellUtils::dumpCellState(SCell* cell, std::ostream& o, SState::Label lbl)
{

    for (unsigned int i = 0; i < cell->States.GetCount(); ++i)
    {
        SStatePtr state = cell->States.Get(i);
        if (state->GetLabel() == lbl)
        {
            o << "\t\t" << cell->I << ": ";
            dumpState(*state, o);
            o << std::endl;
        }
    }
}


inline void CellUtils::dumpState(SState& state, std::ostream &o)
{
    o << state.GetK() << " " << state.GetLHS()->GetName() << " -> ";

    KTokItem tok = state.GetFirst();
    size_t count = 0;
    while(tok)
    {
        if(!state.IsFinished() && ( state.GetDot() == count++))
            o << ". ";

        o << tok.GetToken()->GetName() << " ";
        tok = tok.GetNext();
    }
    if( state.IsFinished() )
        o << ".";

    o << "\t[" << std::scientific << state.GetAlpha()
      << ", " << state.GetGamma()
      << ", " << state.GetV();

    o.unsetf(std::ios_base::floatfield);

    o << "] ["  << state.GetLowMark()
      << ", " << state.GetHiMark() << "]";
}
