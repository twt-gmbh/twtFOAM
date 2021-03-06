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

#include "writer.H"
#include "coordSet.H"
#include "OFstream.H"
#include "OSspecific.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

template<class Type>
Foam::autoPtr< Foam::writer<Type> > Foam::writer<Type>::New
(
    const word& writeType
)
{
    typename wordConstructorTable::iterator cstrIter =
        wordConstructorTablePtr_->find(writeType);

    if (cstrIter == wordConstructorTablePtr_->end())
    {
        FatalErrorInFunction
            << "Unknown write type "
            << writeType << nl << nl
            << "Valid write types : " << endl
            << wordConstructorTablePtr_->sortedToc()
            << exit(FatalError);
    }

    return autoPtr<writer<Type> >(cstrIter()());
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Type>
Foam::fileName Foam::writer<Type>::getBaseName
(
    const coordSet& points,
    const wordList& valueSets
) const
{
    fileName fName(points.name());

    forAll(valueSets, i)
    {
        fName += '_' + valueSets[i];
    }

    return fName;
}


template<class Type>
void Foam::writer<Type>::writeCoord
(
    const coordSet& points,
    const label pointI,
    Ostream& os
) const
{
    if (points.hasVectorAxis())
    {
        write(points.vectorCoord(pointI), os);
    }
    else
    {
        write(points.scalarCoord(pointI), os);
    }
}


template<class Type>
void Foam::writer<Type>::writeTable
(
    const coordSet& points,
    const List<Type>& values,
    Ostream& os
) const
{
    forAll(points, pointI)
    {
        writeCoord(points, pointI, os);
        writeSeparator(os);
        write(values[pointI], os);
        os << nl;
    }
}


template<class Type>
void Foam::writer<Type>::writeTable
(
    const coordSet& points,
    const List<const List<Type>*>& valuesPtrList,
    Ostream& os
) const
{
    forAll(points, pointI)
    {
        writeCoord(points, pointI, os);

        forAll(valuesPtrList, i)
        {
            writeSeparator(os);

            const List<Type>& values = *valuesPtrList[i];
            write(values[pointI], os);
        }
        os << nl;
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
Foam::writer<Type>::writer()
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class Type>
Foam::writer<Type>::~writer()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void Foam::writer<Type>::write
(
    const coordSet& points,
    const wordList& valueSetNames,
    const List<Field<Type> >& valueSets,
    Ostream& os
) const
{
    List<const Field<Type>*> valueSetPtrs(valueSets.size());
    forAll(valueSetPtrs, i)
    {
        valueSetPtrs[i] = &valueSets[i];
    }
    write(points, valueSetNames, valueSetPtrs, os);
}


template<class Type>
Foam::Ostream& Foam::writer<Type>::write
(
    const scalar value,
    Ostream& os
) const
{
    return os << value;
}


template<class Type>
template<class VSType>
Foam::Ostream& Foam::writer<Type>::writeVS
(
    const VSType& value,
    Ostream& os
) const
{
    for (direction d=0; d<VSType::nComponents; d++)
    {
        if (d > 0)
        {
            writeSeparator(os);
        }

        os << value.component(d);
    }
    return os;
}


template<class Type>
void Foam::writer<Type>::writeSeparator
(
    Ostream& os
) const
{
    os << token::SPACE << token::TAB;
}


template<class Type>
Foam::Ostream& Foam::writer<Type>::write
(
    const vector& value,
    Ostream& os
) const
{
    return writeVS(value, os);
}


template<class Type>
Foam::Ostream& Foam::writer<Type>::write
(
    const sphericalTensor& value,
    Ostream& os
) const
{
    return writeVS(value, os);
}


template<class Type>
Foam::Ostream& Foam::writer<Type>::write
(
    const symmTensor& value,
    Ostream& os
) const
{
    return writeVS(value, os);
}


template<class Type>
Foam::Ostream& Foam::writer<Type>::write
(
    const tensor& value,
    Ostream& os
) const
{
    return writeVS(value, os);
}


// ************************************************************************* //
