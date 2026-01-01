.. _api.group.or_:

==========
Or objects
==========
.. currentmodule:: grammatica.grammar.group

Constructor
-----------
.. autosummary::
   :toctree: ../api/group/

   Or

Attributes
----------

.. autosummary::
   :toctree: ../api/group/

   Or.separator
   Or.subexprs
   Or.quantifier

Rendering
---------

.. autosummary::
   :toctree: ../api/group/

   Or.render
   Or.render_quantifier
   Or.needs_wrapped
   Or.as_string

Simplification
--------------

.. autosummary::
   :toctree: ../api/group/

   Or.simplify
   Or.simplify_subexprs

Utility
-----------------

.. autosummary::
   :toctree: ../api/group/

   Or.copy
   Or.equals

Metadata
--------

.. warning:: ``Or.attrs_dict`` is considered experimental and may change without warning.

.. autosummary::
   :toctree: ../api/group/

   Or.attrs_dict
