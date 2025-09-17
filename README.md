<picture align="center">
  <source media="(prefers-color-scheme: dark)" srcset="https://londowski.com/docs/grammatica/stable/_static/logo-dark.svg">
  <img alt="Grammatica Logo" src="https://londowski.com/docs/grammatica/stable/_static/logo.svg">
</picture>

---

# Grammatica: GBNF grammar building toolkit

|         |                                                                                                                                                                                                                                                                                                       |
| ------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Testing | [![Ubuntu](https://github.com/yaphott/grammatica/actions/workflows/ubuntu.yml/badge.svg?branch=main)](https://github.com/yaphott/grammatica/actions/workflows/ubuntu.yml) [![codecov](https://codecov.io/gh/yaphott/grammatica/graph/badge.svg?token=TODO)](https://codecov.io/gh/yaphott/grammatica) |
| Package | [![PyPI - Version](https://img.shields.io/pypi/v/grammatica)](https://pypi.org/project/grammatica/) [![Conda Version](https://img.shields.io/conda/vn/yaphott/grammatica)](https://anaconda.org/yaphott/grammatica)                                                                                   |
| Meta    | [![PyPI - License](https://img.shields.io/pypi/l/grammatica)](https://github.com/yaphott/grammatica/blob/main/LICENSE)                                                                                                                                                                                |

## What is it?

[Grammatica](https://github.com/yaphott/grammatica) is a [GBNF (GGML BNF)](https://github.com/ggml-org/llama.cpp/blob/master/grammars/README.md) grammar builder that exposes the GBNF notation system using a syntax that reflects the resulting grammar. These grammars can be used to restrict the output
of LLMs using interface engines such as [llama.cpp](https://github.com/ggml-org/llama.cpp).

## Table of Contents

- [Requirements](#requirements)
- [Issue Reporting](#issue-reporting)
- [Installing](#installing)
- [Useful Links](#useful-links)

## Requirements

**Grammatica** requires Python 3.10 or higher and is platform independent.

> [!IMPORTANT]
> [`typing-extensions`](https://github.com/python/typing_extensions) is required for Python 3.11 and lower.

## Issue Reporting

If you discover an issue with Grammatica, please report it at [https://github.com/yaphott/grammatica/issues](https://github.com/yaphott/grammatica/issues).

## Installing

Install from [**PyPi**](https://pypi.org/project/grammatica/):

```bash
pip install grammatica
```

Install from [**Conda**](https://anaconda.org/yaphott/grammatica):

```bash
conda install -c yaphott grammatica
```

Install from [**GitHub**](https://github.com/yaphott/grammatica):

```bash
git clone git+https://github.com/yaphott/grammatica.git
cd grammatica
pip install -e .
```

## Useful Links

- [Grammatica Documentation](https://londowski.com/docs/grammatica/stable/)
- [Introduction to GGML](https://huggingface.co/blog/introduction-to-ggml) (Hugging Face)
- [GBNF Guide](https://github.com/ggml-org/llama.cpp/blob/master/grammars/README.md) (llama.cpp)

---

[Go to Top](#table-of-contents)
