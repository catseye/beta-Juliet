β-Juliet
========

β-Juliet is a minimal event-oriented language which, as of version 2.0, is
probably Turing-complete.

NOTE
----

I've decided that the languages β-Juliet and 2Iota are really too similar
to be seperate languages.  So, as of this repo, they've been merged like this:

* This language is called β-Juliet (a.k.a. beta-Juliet).
* The language previously referred to as β-Juliet is now β-Juliet 1.0.
* The language previously referred to as 2Iota is now β-Juliet 2.0.
* The reference interpreter for β-Juliet 2.0 is called `2iota`.
* The file extension of a β-Juliet source file is typically `.bj`,
  although you may see `.2i` used as well.  The latter suggests that
  the source relies on features only in version 2.0.
* The optional pre-processor for β-Juliet 1.0 is still called Portia.
  Portia may or may not work with β-Juliet 2.0; I don't know yet.

Further reorganization and stuff forthcoming.

Description
-----------

β-Juliet is a fairly minimal event-oriented language.  In β-Juliet, the world
is modelled as a set of _events_ which have no inherent organization or order.
Each event can be denoted with a _symbol_, such as `DominoFalls`, `CatMeows`,
or `SunSets`, or (in version 2.0) a string of symbols, such as `Address Line
Six Goes High` or `Greengrocer Falls Asleep on Subway`.

Each event can cause other events to occur — these are termed _consequences_
of the event.  In addition, this causation may be conditional, but the only
condition that is possible to check is: _given two events, which one happened
more recently?_

Basic Example
-------------

Basic Grammar
-------------

The basic grammar of β-Juliet is given here in EBNF.  Version 1.0 uses this
grammar as it stands; version 2.0 extends many of the productions.

    betaJuliet ::= Event {";" Event} ".".
    Event      ::= "event" Name {"," Property}.
    Property   ::= "caused" TimePrep Name {WhenTerm}
                 | "causes" Name ["immediately"] {WhenTerm}
                 | "duration" Number TimeUnit.
    TimePrep   ::= "before" | "after" | "by".
    TimeUnit   ::= "ms" | "s" | "m" | "h" | "d".
    WhenTerm   ::= "when" Name ">" Name.

Portia
------

*Portia* is a pre-processor language designed specifically for β-Juliet
version 1.0.  It is solely concerned with expanding parameterized events
into series of concrete events, for example creating events `DominoOneFalls`,
`DominoTwoFalls`, etc., from the generalized event schema
`Domino(N=Domino)Falls` where `Domino` is an _alphabet_ over the range of
dominoes.

This mechanism (in fact, an extended form of it) is included in version 2.0
of the β-Juliet language itself, so no pre-processor is needed for it.

Patterns
--------

β-Juliet version 2.0 introduces _event patterns_.  When the name of an event
is given by a string of symbols, some of those symbols may actually be
_parameters_ which act something like wildcards.  Each parameter ranges over
a specified _alphabet_.  When an event occurs which matches the name of an
event, the parameters in that name are bound to the literal symbols in the
occurring event.  These bound parameters may then be used in substitutions in
the consequences.

For example, if we have an alphabet called `Animal` that consists of the
symbols `Dog` `Cat` `Ferret`, we can have an event `(X=Animal) Licks Itself`
which has, as a consequence, `(X) Becomes Clean`.  Here `X` is a parameter.
This event will happen should some other event cause `Cat Licks Itself`, and
in this case, `X` will be bound to `Cat`, and this event will thus
subsequently cause the event `Cat Becomes Clean`.

(more to come)

Extended Grammar
----------------

β-Juliet version 2.0 explicitly reserves syntax for implementation-specific
pragmas and system events.

    betaJuliet      ::= Decl {";" Decl} ".".
    Decl            ::= Pragma | Alphabet | Event.

    Pragma          ::= "pragma" <<<implementation-specific>>>.

    Alphabet        ::= "alphabet" AlphabetName {"," SymbolName}.

    Event           ::= "event" EventDeclName {"," Property}.
    Property        ::= "causes" EventApplName ["after" TimeSpec] {WhenTerm}.

    TimeSpec        ::= RationalNumber TimeUnit.
    TimeUnit        ::= "ms" | "s" | "m" | "h" | "d".

    WhenTerm        ::= "when" EventName ">" EventName.

    EventDeclName   ::= EventDeclComp {EventDeclComp}.
    EventDeclComp   ::= SymbolName | "(" ParamName "=" MatchExpr ")".

    MatchExpr       ::= AlphabetName ["+"].

    EventApplName   ::= EventApplComp {EventApplComp}.
    EventApplComp   ::= SymbolName | "(" AlphabetExpr ")".

    AlphabetExpr    ::= AlphabetTerm {"|" AlphabetTerm}.
    AlphabetTerm    ::= "succ" ParamName
                      | "pred" ParamName
                      | "next" ParamName
                      | "prev" ParamName
                      | "first" AlphabetName
                      | "last" AlphabetName
                      | SymbolName
                      .

Implementations
---------------

There is a crude implementation of β-Juliet version 1.0 in the form of a
Perl 5 script.  It does not implement delays.

The reference implementation of β-Juliet version 2.0, called `2iota`, is
written in C.  It implements delays (when compiled as ANSI C they have
second granularity; when compiled as C99 with POSIX, they have millisecond
granularity.)  It does not, however, implement `caused after|before|by`
clauses.
