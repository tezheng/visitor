Visitor pattern with dynamic binding support.
=======

The code in this repository is something I wrote 5 years ago in 2007, a bit old and also not well polished, feel free to use/share :)
Suggestion and criticism is very welcome, contact me with tezheng1982<at>gmail.com
=======


Visitor is very useful pattern, some general information could be found here http://en.wikipedia.org/wiki/Visitor_pattern.

Here is a typical implementation of visitor pattern


class visitor
{

public:

    virtual void visit(shape&);

    virtual void visit(circle&);

    virtual void visit(rectangle&);

}


class shape
{

public:

    virtual void accept(visitor& v) { v.visit(*this); }

};


class circle: public shape
{

public:

    virtual void accept(visitor& v) { v.visit(*this); }

};


class rectangle: public shape
{

public:

    virtual void accept(visitor& v) { v.visit(*this); }

};


So, several shortcomings here
- What if a new class(square, e.g.) is defined?
  No way, have to modify visitor class, and all the existing subclasses if necessary.
- What if I subclass from shape and wanna inject it into the visitor system, but dont want to expose it?
  Not possible, you have to make it visible to visitor class
- What if I want to re-define visit method of certain visitee at runtime, or without touch the existing implementation?
  Not easy, you have to either rewrite the code, or subclass the visitor class, and override the visit function.


Well, this implementation is aimed at overcome these shortcomings, and makes it possible that
- No need touch the visitor class declaration when new visitee are defined, so as all the existing visitor implementations.
- Inject new visitee into the visitor system without expose it
- Make visit functor runtime customizable, which means one could hijack the existing behavior without touch anything.


Basic idea is
- Make visitor class as simple as possible

    class visitor
    {

    public:

        template <T>

        void visit(visitee* object) { functor.visit(visitee, *this); };

    };


- Define visit functor, which is the class to do the real work in

    class visitfunctor
    {

    public:

        void visit(visitee* object, visitor& visitor) { object->accept(); };

    };


- visitee is required

    class visitee
    {

        void accept(visitor& v) { visitor.visit<visitee>(this); }

    };


