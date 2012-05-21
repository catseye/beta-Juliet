β-Juliet
========

β-Juliet is a minimal event-oriented language which, as of version 2.0, is
probably Turing-complete.

NOTE
----

I've decided that the languages β-Juliet and 2Iota are really too similar
to be seperate languages.  So, as of this repo, they've been merged like this:

* This language is called β-Juliet (a.k.a. beta-Juliet).
* The language previously referred to as β-Juliet, is now β-Juliet 1.0.
* The language previously referred to as 2Iota is now β-Juliet 2.0.
* The reference interpreter for β-Juliet 2.0 is called `2iota`.
* The optional pre-processor for β-Juliet 1.0 is still called Portia.
* Portia may or may not work with β-Juliet 2.0; I don't know yet.

Further reorganization and stuff forthcoming.

Description
-----------

β-Juliet is a fairly minimal event-oriented language.  In β-Juliet, the world
is modelled as a set of _events_ which have no inherent organization or order.
Each event can be denoted with a symbol, such as `DominoFalls`, `CatMeows`, or
`SunSets`, or (in version 2.0) a string of symbols, such as `Address Line Six
Goes High` or `Greengrocer Falls Asleep on Subway`.

Each event can cause other events to occur — these are termed _consequences_
of the event.  In addition, this causation may be conditional, but the only
condition that is possible to check is: _given two events, which one happened
more recently?_

Example
-------

Grammar
-------

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
