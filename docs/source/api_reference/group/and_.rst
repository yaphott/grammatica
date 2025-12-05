.. _api.group.and_:

===============
And objects
===============
.. currentmodule:: grammatica.grammar.group

Constructor
-----------
.. autosummary::
   :toctree: ../api/group/

   And

Attributes
----------

.. autosummary::
   :toctree: ../api/group/

   And.separator
   And.subexprs
   And.quantifier

Rendering
---------

.. autosummary::
   :toctree: ../api/group/

   And.render
   And.render_quantifier
   And.needs_wrapped
   And.as_string

Simplification
--------------

.. autosummary::
   :toctree: ../api/group/

   And.simplify
   And.simplify_subexprs

Utility
-----------------

.. autosummary::
   :toctree: ../api/group/

   And.copy
   And.equals

Metadata
--------

.. warning:: ``And.attrs_dict`` is considered experimental and may change without warning.

.. autosummary::
   :toctree: ../api/group/

   And.attrs_dict
