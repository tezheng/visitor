/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2009-2012 Zheng Te.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/** @file visitor.cpp
 *
 * Register for visitor/visitee/visitfunctor.
 *
 * @author Zheng Te
 *
 */

#include <algorithm>
#include <map>

#include "visitor.h"


USING_NAMESPACE(cys)


typedef std::map<unsigned int, delegate> DELEGATEMAP;
static DELEGATEMAP& GetBaseDelegates()
{
	static DELEGATEMAP s_mapBaseDelegates;
	return s_mapBaseDelegates;	
}

typedef std::map<VTRID, DELEGATEMAP> VISITORDELEGATEMAP;
static VISITORDELEGATEMAP& GetVisiteeDelegates()
{
	static VISITORDELEGATEMAP s_mapDelegates;
	return s_mapDelegates;
}


static void GetBaseVisitFunctors(const VTRID& id, Delegates& delegates)
{
	delegates.clear();
	delegates.reserve(GetBaseDelegates().size());

	for (DELEGATEMAP::iterator itr = GetBaseDelegates().begin();
		 itr != GetBaseDelegates().end(); ++itr)
	{
		delegates.push_back(itr->second);
	}

	DELEGATEMAP& visitormap = GetVisiteeDelegates()[id];
	for (DELEGATEMAP::iterator itr = visitormap.begin();
		 itr != visitormap.end(); ++itr)
	{
		delegates[itr->first] = itr->second;
	}
}


BEGIN_HELPER(Visitor)

struct opSetFunctors
{
	opSetFunctors(Delegates& functors)
	: _functors(functors)
	{

	}

	void operator()(delegate& rhs) const
	{
		if (rhs._caller->_type == VisitFunctor::init)
			(static_cast<InitFunctor*>(rhs._caller))->SetFunctors(_functors);
	}

	Delegates& _functors;
};

struct opBuildFunctors
{
	void operator()(delegate& rhs) const
	{
		if (rhs._caller->_type == VisitFunctor::init)
			rhs(0, *((Visitor*)0));
	}
};

END_HELPER(Visitor)


Visitor::Visitor(const VTRID& id)
: _id(id)
, _mask(unsigned(0))
{
	GetBaseVisitFunctors(_id, _functors);

	Helper::opSetFunctors fn(_functors);
	Helper::opBuildFunctors bfn;
	std::for_each(_functors.begin(), _functors.end(), fn);
	std::for_each(_functors.begin(), _functors.end(), bfn);
}

Visitor::~Visitor()
{

}

unsigned Visitor::QueryVisiteeID(const VTEID& vteid)
{
	typedef std::map<VTEID, unsigned int> TYPEMAP;
	static TYPEMAP s_mapType;

	TYPEMAP::const_iterator itr = s_mapType.find(vteid);
	if (itr != s_mapType.end())
	{
		return itr->second;
	}
	else
	{
		unsigned int id = (unsigned int)s_mapType.size();
		s_mapType.insert(std::make_pair(vteid, id));

		return id;
	}
}

void Visitor::RegisterVisitFunctor(const VTRID& clsid, const VTEID& vteid, const delegate& d)
{
	GetVisiteeDelegates()[clsid][QueryVisiteeID(vteid)] = d;
}

void Visitor::RegisterBaseFunctor(const VTEID& vteid, const delegate& d)
{
	GetBaseDelegates()[QueryVisiteeID(vteid)] = d;
}


BEGIN_REG
	cys::Visitor::RegisterVisitee<cys::Visitee>();
END_REG
