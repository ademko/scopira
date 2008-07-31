
/**
  \page scopirasyspage Scopira Core Reference

  \section introsec Introduction

  Scopira is a C++ API and framework for application development. It is designed to be
  an efficient, modular and flexible toolkit for the development of graphical,
  visualization, distributed and/or scientific applications.

  This library provides reference counting, networking, IO, many utilities,
  core loop processing, Agents and basic numerics.
  It includes no graphical components.

  \section userssec User Documention

    - \subpage scopiraconfigpage

  \section tutsys Tutorials and Examples

   - \subpage basicexamplespage

  \section subsyssec Subsystem API Reference

  - Basics:
    - \subpage scopiratoolobject
    - \subpage scopiratooloutput
  - Utility:
    - \subpage scopiracoreserializationpage
    - \subpage scopiratoolflowpage
    - \subpage scopiracoreloop
    - \subpage scopiracoresidekick
  - Numerics:
    - \subpage scopirabasekitnarray
    - \subpage scopirabasekitdio
    - \subpage scopiratoolrandompage
    - \subpage scopiratoolarraypage
  - Parallel computing:
    - \subpage scopiraagentssyspage

  Namespaces that comprise this library are: scopira::tool, scopira::core
  and scopira::basekit

  \subsection othersubsyssec Other subsystems

  Scopira utilizes a modular architecture designed to allow application developers
  to only use the subsystems that they need. This also reduces the dependancy
  on unused 3rd party libraries.

  See the "Related Pages" tab for a full list of subsystem manuals.

*/

/**
  \page basicexamplespage Basic examples

  \section hellosec Full Hello World Example

  You may utilize the Scopira data objects and other algorithms without having to code up modules. The simplest example would be like this:

  \code
  #include <scopira/tool/output.h>
  #include <scopira/basekit/narray.h>
  //BBlibs scopira
  //BBtargets test.exe
  int main(void)
  {
    scopira::basekit::narray<double, 2> thematrix;
    thematrix.resize(4,4);
    thematrix.set_all(1);
    thematrix.diagonal_slice().set_all(5);
    OUTPUT << thematrix;
    return 0;
  }
  \endcode

  Save this as test.cpp. Run buildboss on it, make then run it:

  \verbatim
  buildboss test.cpp
  make
  ./test.cpp
  \endverbatim

  Your output should be:

  \verbatim
  autoinit: default flow for OUTPUT.
  Matrix, w=4 h=4:
                0:       1:       2:       3:
      0:      5.00     1.00     1.00     1.00
      1:      1.00     5.00     1.00     1.00
      2:      1.00     1.00     5.00     1.00
      3:      1.00     1.00     1.00     5.00
  \endverbatim

*/

