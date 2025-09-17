.. _api.grammar:

===============
Grammar objects
===============
.. currentmodule:: grammatica.grammar

Constructor
-----------
.. autosummary::
   :toctree: api/

   Grammar

Attributes
----------

.. autosummary::
   :toctree: api/

   Grammar.separator
   Grammar.subexprs
   Grammar.length_range

Rendering
---------

.. autosummary::
   :toctree: api/

   Grammar.render
   Grammar.render_quantifier
   Grammar.needs_wrapped
   Grammar.as_string

Simplification
--------------

.. autosummary::
   :toctree: api/

   Grammar.simplify
   Grammar.simplify_subexprs

Utility
-----------------

.. autosummary::
   :toctree: api/

   Grammar.copy
   Grammar.equals

Metadata
--------

.. warning:: ``Grammar.attrs_dict`` is considered experimental and may change without warning.

.. autosummary::
   :toctree: api/

   Grammar.attrs_dict
