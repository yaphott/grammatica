# Grammatica Documentation

This directory contains the Sphinx documentation source files for the Grammatica project.

## Requirements

The documentation is built using [Sphinx](https://www.sphinx-doc.org/en/master/) with the following dependencies:

- Python 3.10 or higher
- `sphinx`
- `pydata-sphinx-theme`
- `sphinx-autodoc-typehints`
- `sphinx-design`
- `sphinx-copybutton`

## Installation

Install the documentation dependencies using one of the following methods:

### Using pip

From the project root directory:

```bash
pip install -r requirements-doc.txt
```

### Using the Project Setup

From the project root directory:

```bash
pip install -e .[doc]
```

### Using Conda

Alternatively, create a full development environment using Conda from the project root directory:

```bash
conda env create -n grammatica-dev -f environment.yml
```

## Building the Documentation

The documentation uses a `Makefile` for building. All commands should be run from the `docs/` directory.

### Build HTML Documentation

To generate the HTML documentation:

```bash
cd docs
make html
```

The generated HTML files will be located in `docs/build/html/`. Open `docs/build/html/index.html` in your browser to view the documentation.

### Clean Build Files

To remove all generated documentation files:

```bash
cd docs
make clean
```

This will remove:

- All files in the `build/` directory
- Auto-generated API reference files in `source/api_reference/api/`

### Other Build Formats

Sphinx supports various output formats. To see all available options:

```bash
cd docs
make help
```

Common formats include:

- `make html` - HTML output (default)
- `make latexpdf` - PDF output via LaTeX
- `make epub` - EPUB output
- `make man` - Manual pages

## Development

### Preview Documentation Locally

After building the documentation, you can serve it locally using Python's built-in HTTP server:

```bash
cd docs/build/html
python -m http.server 9000
```

Then open <http://localhost:9000> in your browser.

Alternatively, use the provided script from the project root:

```bash
python scripts/serve_locally.py
```

This will serve the documentation at <http://127.0.0.1:9000> and automatically rebuild when you save changes.

## Environment Variables

The following environment variables affect documentation building:

- `RUNNING_CI`: Set to `1` when building in CI/CD (affects URL generation).
- `URL_PATH_PREFIX`: Prefix for all URLs (used for hosting in subdirectories).

## Troubleshooting

### Build Errors

If you encounter build errors:

1. Ensure all dependencies are installed: `pip install -r requirements-doc.txt`
2. Clean previous builds: `make clean`
3. Rebuild: `make html`

### Missing Modules

If Sphinx cannot find the `grammatica` module, ensure the package is installed: `pip install -e .` from the project root directory.

### API Documentation Not Updating

The API documentation is auto-generated. To force regeneration:

```bash
make clean
make html
```

## Contributing

When contributing to documentation:

1. Write clear, concise docstrings following the Google style guide.
2. Update relevant `.rst` files in `source/` if adding new sections.
3. Build and preview changes locally before submitting.
4. Ensure all Sphinx warnings are addressed.

## Links

- [Sphinx Documentation](https://www.sphinx-doc.org/en/master/)
- [PyData Sphinx Theme](https://pydata-sphinx-theme.readthedocs.io/en/stable/)
- [Grammatica Documentation](https://londowski.com/docs/grammatica/stable/)
- [Grammatica Repository](https://github.com/yaphott/grammatica)
