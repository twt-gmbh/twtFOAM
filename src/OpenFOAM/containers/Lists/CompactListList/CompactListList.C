/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2015 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "CompactListList.H"

// * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * * //

template<class T, class Container>
Foam::CompactListList<T, Container>::CompactListList(const List<Container>& ll)
:
    size_(ll.size()),
    offsets_(ll.size()+1)
{
    label sumSize = 0;
    offsets_[0] = 0;
    forAll(ll, i)
    {
        sumSize += ll[i].size();
        offsets_[i+1] = sumSize;
    }

    m_.setSize(sumSize);

    label k = 0;
    forAll(ll, i)
    {
        const Container& lli = ll[i];

        forAll(lli, j)
        {
            m_[k++] = lli[j];
        }
    }
}


template<class T, class Container>
Foam::CompactListList<T, Container>::CompactListList
(
    const labelUList& rowSizes
)
:
    size_(rowSizes.size()),
    offsets_(rowSizes.size()+1)
{
    label sumSize = 0;
    offsets_[0] = 0;
    forAll(rowSizes, i)
    {
        sumSize += rowSizes[i];
        offsets_[i+1] = sumSize;
    }

    m_.setSize(sumSize);
}


template<class T, class Container>
Foam::CompactListList<T, Container>::CompactListList
(
    const labelUList& rowSizes,
    const T& t
)
:
    size_(rowSizes.size()),
    offsets_(rowSizes.size()+1)
{
    label sumSize = 0;
    offsets_[0] = 0;
    forAll(rowSizes, i)
    {
        sumSize += rowSizes[i];
        offsets_[i+1] = sumSize;
    }

    m_.setSize(sumSize, t);
}


template<class T, class Container>
Foam::CompactListList<T, Container>::CompactListList
(
    const Xfer<CompactListList<T, Container> >& lst
)
{
    transfer(lst());
}


template<class T, class Container>
Foam::CompactListList<T, Container>::CompactListList
(
    CompactListList<T, Container>& lst,
    bool reUse
)
:
    size_(lst.size()),
    offsets_(lst.offsets_, reUse),
    m_(lst.m_, reUse)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T, class Container>
void Foam::CompactListList<T, Container>::setSize(const label nRows)
{
    if (nRows == 0)
    {
        clear();
    }
    if (nRows < size())
    {
        size_ = nRows;
        offsets_.setSize(nRows+1);
        m_.setSize(offsets_[nRows]);
    }
    else if (nRows > size())
    {
        FatalErrorInFunction
            << "Cannot be used to extend the list from " << offsets_.size()
            << " to " << nRows << nl
            << "    Please use one of the other setSize member functions"
            << abort(FatalError);
    }
}


template<class T, class Container>
void Foam::CompactListList<T, Container>::setSize
(
    const label nRows,
    const label nData
)
{
    size_ = nRows;
    offsets_.setSize(nRows+1);
    m_.setSize(nData);
}


template<class T, class Container>
void Foam::CompactListList<T, Container>::setSize
(
    const label nRows,
    const label nData,
    const T& t
)
{
    size_ = nRows;
    offsets_.setSize(nRows+1);
    m_.setSize(nData, t);
}


template<class T, class Container>
void Foam::CompactListList<T, Container>::setSize(const labelUList& rowSizes)
{
    size_ = rowSizes.size();
    offsets_.setSize(rowSizes.size()+1);

    label sumSize = 0;
    offsets_[0] = 0;
    forAll(rowSizes, i)
    {
        sumSize += rowSizes[i];
        offsets_[i+1] = sumSize;
    }

    m_.setSize(sumSize);
}


template<class T, class Container>
Foam::labelList Foam::CompactListList<T, Container>::sizes() const
{
    labelList rowSizes(size());

    if (rowSizes.size() > 0)
    {
        forAll(rowSizes, i)
        {
            rowSizes[i] = offsets_[i+1] - offsets_[i];
        }
    }
    return rowSizes;
}


template<class T, class Container>
void Foam::CompactListList<T, Container>::clear()
{
    size_ = 0;
    offsets_.clear();
    m_.clear();
}


template<class T, class Container>
void Foam::CompactListList<T, Container>::transfer
(
    CompactListList<T, Container>& a
)
{
    size_ = a.size_;
    offsets_.transfer(a.offsets_);
    m_.transfer(a.m_);
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class T, class Container>
Foam::List<Container> Foam::CompactListList<T, Container>::operator()()
const
{
    List<Container> ll(size());

    forAll(ll, i)
    {
        ll[i] = Container(operator[](i));
    }

    return ll;
}


// * * * * * * * * * * * * * * * *  IOStream operators * * * * * * * * * * * //

#include "CompactListListIO.C"

// ************************************************************************* //
