.. _api.grammar:

========
Grammars
========

This page gives an overview of all public grammatica objects, functions and
methods. All classes and functions exposed in ``grammatica.*`` namespace are public.

The following subpackages are public.

- ``grammatica.builder.json_``: Classes and utilities for creating and manipulating grammars.
- ``grammatica.builder``: Classes and utilities for building grammars.
- ``grammatica.utils``: Utility functions used throughout the library.

..
    - ``grammatica.errors``: Custom exception and warnings classes that are raised by grammatica.

.. warning::

   The ``grammatica.base``, ``grammatica.grammar.base`` top-level modules are PRIVATE. Stable functionality in such modules is not guaranteed.

.. toctree::
   :maxdepth: 3

   string
   char_range
   group/and_
   group/or_
   derivation_rule
