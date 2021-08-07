#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/STRUCTURE/RSEdge.h>
#include <BALL/STRUCTURE/RSFace.h>
#include <BALL/STRUCTURE/RSVertex.h>

namespace BALL
{

	RSVertex::RSVertex()
		
		:	GraphVertex< RSVertex,RSEdge,RSFace >(),
			atom_(-1)
	{
	}


	RSVertex::RSVertex(const RSVertex& rsvertex, bool deep)
		
		:	GraphVertex< RSVertex,RSEdge,RSFace >(rsvertex,deep),
			atom_(rsvertex.atom_)
	{
	}


	RSVertex::RSVertex(Index atom)
		
		:	GraphVertex< RSVertex,RSEdge,RSFace >(),
			atom_(atom)
	{
	}


	RSVertex::~RSVertex()
		
	{
	}


	void RSVertex::set(const RSVertex& rsvertex, bool deep)
		
	{
		if (this != &rsvertex)
		{
			GraphVertex< RSVertex,RSEdge,RSFace >::set(rsvertex,deep);
			atom_ = rsvertex.atom_;
		}
	}


	RSVertex& RSVertex::operator = (const RSVertex& rsvertex)
		
	{
		if (this != &rsvertex)
		{
			GraphVertex< RSVertex,RSEdge,RSFace >::operator = (rsvertex);
			atom_ = rsvertex.atom_;
		}
		return *this;
	}


	void RSVertex::setAtom(Index atom)
		
	{
		atom_ = atom;
	}


	Index RSVertex::getAtom() const
		
	{
		return atom_;
	}


	bool RSVertex::operator == (const RSVertex& rsvertex) const
		
	{
		if (atom_ != rsvertex.atom_)
		{
			return false;
		}
		HashSet<RSEdge*>::ConstIterator e;
		for (e = edges_.begin(); e != edges_.end(); e++)
		{
			if (rsvertex.edges_.has(*e) == false)
			{
				return false;
			}
		}
		for (e = rsvertex.edges_.begin(); e != rsvertex.edges_.end(); e++)
		{
			if (edges_.has(*e) == false)
			{
				return false;
			}
		}
		HashSet<RSFace*>::ConstIterator f;
		for (f = faces_.begin(); f != faces_.end(); f++)
		{
			if (rsvertex.faces_.has(*f) == false)
			{
				return false;
			}
		}
		for (f = rsvertex.faces_.begin(); f != rsvertex.faces_.end(); f++)
		{
			if (faces_.has(*f) == false)
			{
				return false;
			}
		}
		return true;
	}


	bool RSVertex::operator != (const RSVertex& rsvertex) const
		
	{
		return ( ! (*this == rsvertex) );
	}


	bool RSVertex::operator *= (const RSVertex& rsvertex) const
		
	{
		return (atom_ == rsvertex.atom_);
	}


	std::ostream& operator << (std::ostream& s, const RSVertex& rsvertex)
	{
		s << "RSVERTEX" << rsvertex.getIndex() << "("
			<< rsvertex.getAtom() << " [";
		RSVertex::ConstEdgeIterator e;
		for (e = rsvertex.beginEdge(); e != rsvertex.endEdge(); e++)
		{
			s << (*e)->getIndex() << ' ';
		}
		s << "] [";
		RSVertex::ConstFaceIterator f;
		for (f = rsvertex.beginFace(); f != rsvertex.endFace(); f++)
		{
			s << (*f)->getIndex() << ' ';
		}
		s << "])";
		return s;
	}


} // namespace BALL
