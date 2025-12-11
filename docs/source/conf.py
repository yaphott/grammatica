from __future__ import annotations

import inspect
import os
import re
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import TYPE_CHECKING
from urllib.parse import quote

from sphinx.builders.html import StandaloneHTMLBuilder

if TYPE_CHECKING:
    from typing import Any

    from sphinx.application import Sphinx


if sys.version_info >= (3, 12):  # pragma: no cover
    from typing import override
else:  # pragma: no cover
    from typing_extensions import override

sys.path.insert(0, str(Path(__file__).parent.parent.parent / "src"))

import grammatica  # pylint: disable=C0413

RUNNING_CI: bool = os.environ.get("RUNNING_CI", "0") == "1"
URL_PATH_PREFIX: str = os.environ.get("URL_PATH_PREFIX", "")

URI_SUFFIX: str = "" if RUNNING_CI else "index.html"
REPO_URL: str = "https://github.com/yaphott/grammatica"


class CustomHTMLBuilder(StandaloneHTMLBuilder):
    name = "html"

    @override
    def get_target_uri(self, docname: str, typ: str | None = None) -> str:
        if docname == "index":
            return self.config["html_baseurl"] + URI_SUFFIX
        if docname.endswith("/index"):
            docname = docname.removesuffix("/index")
        if docname:
            docname = quote(docname)
        return self.config["html_baseurl"] + docname + "/" + URI_SUFFIX

    @override
    def get_output_path(self, page_name: str, /) -> Path:
        if page_name == "index":
            return Path(self.outdir, "index.html")
        if page_name.endswith("/index"):
            page_name = page_name.removesuffix("/index")
        return Path(self.outdir, page_name, "index.html")


def setup(app: Sphinx) -> dict[str, Any]:
    app.setup_extension("sphinx.builders.html")
    app.add_builder(CustomHTMLBuilder, override=True)
    return {
        "version": grammatica.__version__,
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }


def linkcode_resolve(domain, info):
    if domain != "py":
        return None

    submodule = sys.modules.get(info["module"])
    if submodule is None:
        return None

    obj = submodule
    for part in info["fullname"].split("."):
        try:
            obj = getattr(obj, part)
        except AttributeError:
            return None

    try:
        func_path = inspect.getsourcefile(obj)
    except TypeError:
        return None
    if func_path is None:
        return None
    func_path = Path(func_path).relative_to(Path(grammatica.__file__).parent)

    # Strip decorators
    obj = inspect.unwrap(obj)

    # Ignore re-exports
    module = inspect.getmodule(obj)
    if (module is not None) and not module.__name__.startswith(f"{grammatica.__name__}."):
        return None

    if ".dev" in grammatica.__version__:
        ref = "main"
    else:
        ref = "v" + ".".join(grammatica.__version__.split(".")[:3])

    source, line_n = inspect.getsourcelines(obj)
    if line_n:
        line_spec = f"#L{line_n}-L{line_n + len(source) - 1}"
    else:
        line_spec = ""

    return "/".join(
        (
            REPO_URL,
            "blob",
            ref,
            "src",
            "grammatica",
            func_path.as_posix() + line_spec,
        ),
    )


# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information ----------------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

start_year = 2025
end_year = datetime.now(tz=timezone.utc).year

project = "Grammatica"
author = "Nicholas Londowski"
copyright = f"{start_year}-{end_year}, {author}" if end_year > start_year else f"{start_year}, {author}"
version = re.sub(r"(\d+\.\d+)\.\d+(?:(\.dev\d+).*)?", r"\1\2", grammatica.__version__)
release = grammatica.__version__

# -- General configuration ----------------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

needs_sphinx = "8.2.3"
extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.autosummary",
    "sphinx.ext.intersphinx",
    "sphinx.ext.linkcode",
    "sphinx.ext.napoleon",
    "sphinx_autodoc_typehints",
    "sphinx_copybutton",
]

source_encoding = "utf-8"
master_doc = "index"
templates_path = ["_templates"]
exclude_patterns = ["build", "**/.git", "Thumbs.db", ".DS_Store"]
# nitpicky = True

# -- Options for HTML output ----------------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "pydata_sphinx_theme"
html_baseurl = "/" + URL_PATH_PREFIX
html_static_path = ["_static"]
html_css_files = ["grammatica.css"]
html_js_files = [
    ("custom-icons.js", {"defer": "defer"}),
]
html_theme_options = {
    "logo": {
        "image_light": "_static/logo.svg",
        "image_dark": "_static/logo-dark.svg",
    },
    "collapse_navigation": True,
    "header_links_before_dropdown": 4,
    "navbar_align": "left",
    "navbar_start": ["navbar-logo"],
    "navbar_center": ["version-switcher", "navbar-nav"],
    "navbar_end": ["theme-switcher", "navbar-icon-links"],
    "navbar_persistent": ["search-field"],
    "footer_start": ["copyright"],
    "footer_center": ["sphinx-version"],
    "switcher": {
        "version_match": version,
        "json_url": "https://londowski.com/docs/grammatica/versions.json",
    },
    "show_version_warning_banner": False,
    "icon_links": [
        {
            "name": "GitHub",
            "url": REPO_URL,
            "icon": "fa-brands fa-github",
        },
        {
            "name": "PyPI",
            "url": "https://pypi.org/project/grammatica/",
            "icon": "fa-custom fa-pypi",
        },
        {
            "name": "Londowski",
            "url": "https://londowski.com/",
            "icon": "fa-solid fa-l",
        },
    ],
}
html_title = f"{project} v{grammatica.__version__} Manual"
html_last_updated_fmt = "%b %d, %Y"
html_context = {"default_mode": "light"}
html_favicon = "_static/favicon.ico"
html_copy_source = False

# -- Options for Autodoc ---------------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/extensions/autodoc.html#configuration

autodoc_member_order = "bysource"
autodoc_typehints_description_target = "documented"
autodoc_use_type_comments = False

# -- Options for sphinx-autodoc-typehints --------------------------------------
# https://github.com/tox-dev/sphinx-autodoc-typehints?tab=readme-ov-file#options

typehints_use_rtype = False
typehints_document_rtype_none = False

# -- Options for sphinx-copybutton --------------------------
# https://sphinx-copybutton.readthedocs.io/en/latest/use.html

copybutton_prompt_text = r">>> |\.\.\. |\$ |In \[\d*\]: | {2,5}\.\.\.: | {5,8}: "
copybutton_prompt_is_regexp = True

# -- Options for Intersphinx -------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/extensions/intersphinx.html
intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
}

# -- Options for Autosummary -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/extensions/autosummary.html#configuration

autosummary_generate = True

# -- Options for Napoleon settings ------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/extensions/napoleon.html#configuration

napoleon_google_docstring = True
napoleon_numpy_docstring = False
napoleon_include_init_with_doc = True
always_use_bars_union = True
napoleon_use_ivar = False
napoleon_use_rtype = False
napoleon_use_param = False
napoleon_preprocess_types = True
napoleon_use_admonition_for_examples = False
napoleon_use_admonition_for_notes = True
napoleon_use_admonition_for_references = True
