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

/** @file visitor.h
 *
 * Implement visitor pattern with dynamic binding support.
 *
 * @author Zheng Te
 *
 */

#ifndef CYBERY_CORE_VISITOR_H
#define CYBERY_CORE_VISITOR_H

#include <vector>

#include "tweak.h"


#define DEFINE_VISITEE(superclass) \
	public: \
		typedef superclass SUPER; \
		inline static VTEID GetVTEID() { return reinterpret_cast<uintptr_t>(&(visiteeID())); } \
		inline static unsigned GetVisiteeID() { return visiteeID(); } \
		inline static unsigned RegisterVisitee() { \
			visiteeID() = cys::Visitor::QueryVisiteeID(GetVTEID()); \
			return visiteeID(); \
		} \
	private: \
		inline static unsigned& visiteeID() { static unsigned id = static_cast<unsigned>(-1); return id; } \


BEGIN_NAMESPACE(cys)

class Visitor;
class Visitee;
class VisitFunctor;


struct delegate
{
	inline void operator()(Visitee* pObject, Visitor& visitor) const
	{
		return _call(_caller, pObject, visitor);
	}

public:
	VisitFunctor* _caller;

	delegate() : _caller(0) , _call(0) {}

	template <class T, void (T::*pfn)(Visitee*, Visitor&)>
	static delegate from_method(T* caller)
	{
		typedef void (*call_t)(VisitFunctor*, Visitee*, Visitor&);

		delegate d;
		d._caller = caller;
		d._call = (call_t)(&delegate_op<T, pfn>::call);

		return d;
	}

private:
	void (*_call)(VisitFunctor*, Visitee*, Visitor&);

	template <class T, void (T::*pfn)(Visitee*, Visitor&)>
	struct delegate_op
	{
		inline static void call(T* caller, Visitee* pObject, Visitor& visitor)
		{
			return (caller->*pfn)(pObject, visitor);
		}
	};
};
typedef std::vector<delegate> Delegates;


class VisitFunctor
{
public:
	enum Type
	{
		base,
		init,
		user,
	} _type;

public:
	VisitFunctor() : _type(user) { }
	VisitFunctor(Type type) : _type(type) { }
	virtual ~VisitFunctor() { }

public:
	void Visit(Visitee* pObject, Visitor& visitor);
};


class Visitor
{
	DECLARE_HELPER()

	VTRID _id;
	unsigned _mask;
	Delegates _functors;

public:
	Visitor(const VTRID&);
	virtual ~Visitor();

	template <class T>
	inline void Visit(T* pObject)
	{
		unsigned id = T::GetVisiteeID();
		CYS_ASSERT(id != static_cast<unsigned>(-1));

		delegate& fn = _functors[id];
		fn(pObject, *this);
	}

	// Set functor runtime
	template <class T, class F>
	inline bool AddFunctor(F& f)
	{
		unsigned id = T::GetVisiteeID();
		delegate d = delegate::from_method<F, &F::Visit>(&f);
		_functors[id] = d;

		return true;
	}

	inline void SetMask(unsigned mask)
	{
		_mask = mask;
	}

	inline unsigned GetMask() const
	{
		return _mask;
	}

	inline const VTRID& GetVTRID() const
	{
		return _id;
	}

private:
	class InitFunctor: public VisitFunctor
	{
	public:
		virtual void SetFunctors(Delegates& functors) = 0;
		virtual delegate GetParentFunctor() = 0;
	};

	template <class T>
	class ConcreteInitFunctor: public InitFunctor
	{
		Delegates* _pFunctors;

	public:
		ConcreteInitFunctor() : _pFunctors(0) { _type = VisitFunctor::init; }
		virtual ~ConcreteInitFunctor() {}
		virtual void SetFunctors(Delegates& functors) { _pFunctors = &functors; }
		virtual delegate GetParentFunctor() { return _pFunctors->operator[](T::SUPER::GetVisiteeID()); }

	public:
		void Visit(Visitee*, Visitor&)
		{
			delegate pFunctor = _pFunctors->operator[](T::SUPER::GetVisiteeID());
			while (static_cast<VisitFunctor*>(pFunctor._caller)->_type == VisitFunctor::init)
			{
				pFunctor = static_cast<InitFunctor*>(pFunctor._caller)->GetParentFunctor();
			}

			_pFunctors->operator[](T::GetVisiteeID()) = pFunctor;
		}
	};

public:
	template <class T>
	static bool RegisterVisitee()
	{
		T::RegisterVisitee();

		static ConcreteInitFunctor<T> t;
		delegate d = delegate::from_method<ConcreteInitFunctor<T>, &ConcreteInitFunctor<T>::Visit>(&t);
		RegisterBaseFunctor(T::GetVTEID(), d);

		return true;
	}

	template <class C, class F>
	static bool RegisterFunctor(const VTRID& clsid)
	{
		static F f;
		delegate d = delegate::from_method<F, &F::Visit>(&f);

		RegisterVisitFunctor(clsid, C::GetVTEID(), d);

		return true;
	}

	static unsigned QueryVisiteeID(const VTEID&);

private:
	static void RegisterBaseFunctor(const VTEID&, const delegate&);
	static void RegisterVisitFunctor(const VTRID& visitor, const VTEID& visitee, const delegate&);
};


class Visitee
{
	DEFINE_VISITEE(Visitee)

public:
	virtual void Accept(Visitor&) = 0;
	virtual void Traverse(Visitor&) {}
};

inline void VisitFunctor::Visit(Visitee* pObject, Visitor& visitor)
{
	pObject->Traverse(visitor);
}

template <>
inline bool Visitor::RegisterVisitee<Visitee>()
{
	Visitee::RegisterVisitee();
	
	static VisitFunctor visitfn(VisitFunctor::base);
	delegate d = delegate::from_method<VisitFunctor, &VisitFunctor::Visit>(&visitfn);
	RegisterBaseFunctor(Visitee::GetVTEID(), d);
	
	return true;
};

END_NAMESPACE(cys)


#define REG_VISITEE(obj) \
	cys::Visitor::RegisterVisitee<obj>();

#define REG_VISITFUNCTOR(vtr, vte, functor) \
	cys::Visitor::RegisterFunctor<vte, functor>(VTRID_##vtr);


#endif /* CYBERY_CORE_VISITOR_H */
