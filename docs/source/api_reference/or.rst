.. _api.or:

==========
Or objects
==========
.. currentmodule:: grammatica.grammar

Constructor
-----------
.. autosummary::
   :toctree: api/

   Or

Attributes
----------

.. autosummary::
   :toctree: api/

   Or.separator
   Or.subexprs
   Or.quantifier

Rendering
---------

.. autosummary::
   :toctree: api/

   Or.render
   Or.render_quantifier
   Or.needs_wrapped
   Or.as_string

Simplification
--------------

.. autosummary::
   :toctree: api/

   Or.simplify
   Or.simplify_subexprs

Utility
-----------------

.. autosummary::
   :toctree: api/

   Or.copy
   Or.equals

Metadata
--------

.. warning:: ``Or.attrs_dict`` is considered experimental and may change without warning.

.. autosummary::
   :toctree: api/

   Or.attrs_dict
