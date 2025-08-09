<picture align="center">
  <source media="(prefers-color-scheme: dark)" srcset="https://londowski.com/docs/grammatica/_static/grammatica_logo_dark.svg">
  <img alt="Grammatica Logo" src="https://londowski.com/docs/grammatica/_static/grammatica_logo.svg">
</picture>

# Grammatica: GBNF grammar building toolkit

## What is it?

[Grammatica](https://github.com/yaphott/grammatica) is a [GBNF (GGML BNF)](https://github.com/ggml-org/llama.cpp/blob/master/grammars/README.md) grammar builder that exposes the GBNF notation system using a syntax that reflects the resulting grammar. These grammars can be used to restrict the output of LLMs using interface engines such as [llama.cpp](https://github.com/ggml-org/llama.cpp).

## Requirements

**Grammatica** requires Python 3.10 or higher and is platform independent.

> [`typing-extensions`](https://github.com/python/typing_extensions) is required for Python 3.11 and lower.

## Issue Reporting

If you discover an issue with Grammatica, please report it at [https://github.com/yaphott/grammatica/issues](https://github.com/yaphott/grammatica/issues).

## Installing

Install from [**PyPi**](https://pypi.org/project/grammatica/):

[![PyPI](https://img.shields.io/pypi/v/grammatica?style=flat-square)](https://pypi.org/project/grammatica/)
[![PyPI - Python Version](https://img.shields.io/pypi/pyversions/grammatica?style=flat-square)](https://pypi.org/project/grammatica/)
[![PyPI - License](https://img.shields.io/pypi/l/grammatica?style=flat-square)](https://pypi.org/project/grammatica/)

```bash
pip install grammatica
```

<!--
Install from [**Conda**](https://anaconda.org/yaphott/grammatica):

[![Anaconda-Cloud](https://img.shields.io/conda/v/conda-forge/grammatica?style=flat-square)](https://anaconda.org/yaphott/grammatica)
[![Anaconda-Cloud - Python Version](https://img.shields.io/conda/pn/conda-forge/grammatica?style=flat-square)](https://anaconda.org/yaphott/grammatica)
[![Anaconda-Cloud - License](https://img.shields.io/conda/l/conda-forge/grammatica?style=flat-square)](https://anaconda.org/yaphott/grammatica)

```bash
conda install -c yaphott grammatica
```
-->

Install from [**GitHub**](https://github.com/yaphott/grammatica):

![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/yaphott/grammatica/ubuntu.yml?style=flat-square)
![Codecov](https://img.shields.io/codecov/c/github/yaphott/grammatica/main?flag=unittests&style=flat-square&token=NOT_YET_CONFIGURED)
![GitHub](https://img.shields.io/github/license/yaphott/grammatica?style=flat-square)

```bash
git clone git+https://github.com/yaphott/grammatica.git
cd grammatica
pip install -e .
```

<!--
## Usage

```python
from grammatica.grammar import CharRange, String, Grammar, Or

# Create a simple character range expression
digits = CharRange([("0", "9")])

# Create a string literal expression
hello = String("Hello")

# Compose grammars using AND/OR operations
greeting = Grammar([hello, String(" "), Or([String("World"), String("Universe")]), digits])

# Render the grammar expression
pattern = greeting.render()
print(pattern)  # "Hello" " " ("World" | "Universe") [0-9]

simplified = greeting.simplify()
pattern = simplified.render()
print(pattern)  # "Hello " ("World" | "Universe") [0-9]
```
-->

## Useful Links

- [Grammatica Documentation](https://londowski.com/docs/grammatica/)
- [Introduction to GGML](https://huggingface.co/blog/introduction-to-ggml) (Hugging Face)
- [GBNF Guide](https://github.com/ggml-org/llama.cpp/blob/master/grammars/README.md) (llama.cpp)

---

[Go to Top](#Grammatica)
