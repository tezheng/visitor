
#include <stdio.h>

#include "visitor.h"

#define VTRID_HLVisitor	1

USING_NAMESPACE(cys)

class HLVisitor: public Visitor
{
public:
	HLVisitor(): Visitor(VTRID_HLVisitor) {}
	virtual ~HLVisitor() {}
};

class World: public Visitee
{
	DEFINE_VISITEE(Visitee)

public:
	virtual void Accept(Visitor& visitor)
	{
		visitor.Visit(this);
	}

	void SaySomething()
	{
		printf("World\n");
	}
};

class Hello: public Visitee
{
	DEFINE_VISITEE(Visitee)

public:
	virtual void Accept(Visitor& visitor)
	{
		visitor.Visit(this);
	}

	virtual void Traverse(Visitor& visitor)
	{
		World* world = new World;
		world->Accept(visitor);
		delete world;
	}

	void SaySomething()
	{
		printf("Hello\n");
	}
};


struct HelloFunctor: public VisitFunctor
{
	void Visit(Visitee* object, Visitor& v)
	{
		Hello* hello = static_cast<Hello*>(object);

		hello->SaySomething();

		hello->Traverse(v);
	}
};

struct WorldFunctor: public VisitFunctor
{
	void Visit(Visitee* object, Visitor& v)
	{
		World* world = static_cast<World*>(object);

		world->SaySomething();

		world->Traverse(v);
	}
};


int main(int argc, char** argv)
{
	HLVisitor visitor;

	Hello* object = new Hello;
	object->Accept(visitor);
	delete object;

	return 0;
}


BEGIN_REG
	REG_VISITEE(Hello)
	REG_VISITEE(World)
	REG_VISITFUNCTOR(HLVisitor, Hello, HelloFunctor)
	REG_VISITFUNCTOR(HLVisitor, World, WorldFunctor)
END_REG
