#!/usr/bin/env bash
set -e

PYTHON=python3
APP_NAME=grammatica
PYTEST_TARGET=tests

PROJECT_DIR="$(dirname "$(dirname "$(realpath "$0")")")"
cd "$PROJECT_DIR"

PYTHONHASHSEED=$(python -c 'import random; print(random.randint(1, 4294967295))')
export PYTHONHASHSEED

coverage_args="--cov-config=pyproject.toml --cov=$APP_NAME --cov-report=xml --cov-append"
pytest_cmd="PYTHONDEVMODE=1 PYTHONWARNDEFAULTENCODING=1 $PYTHON -m pytest -r fEs -s $coverage_args $PYTEST_TARGET"
echo "$pytest_cmd"
sh -c "$pytest_cmd"
