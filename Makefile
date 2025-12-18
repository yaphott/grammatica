APP_NAME := grammatica
EXTENDED_LINE_LENGTH := 140

PYTHON ?= python3
PYTEST_TARGET ?= tests
PYTEST_WORKERS ?= logical

.PHONY: install install-dev test coverage lint format build

install:
	${PYTHON} -m pip install .

install-dev:
	${PYTHON} -m pip install -e .

test:
	@echo 'Running unit tests'
	PYTHONDEVMODE=1 PYTHONWARNDEFAULTENCODING=1 ${PYTHON} -m pytest -r fEs -s -n ${PYTEST_WORKERS} --dist=worksteal ${PYTEST_ARGS} ${PYTEST_TARGET}
	@echo 'Done'

coverage:
	@$(MAKE) test PYTEST_ARGS="--cov-config=pyproject.toml --cov=${APP_NAME} --cov-report=xml"
	@echo 'Generating coverage report'
	${PYTHON} -m coverage report -m
	${PYTHON} -m coverage html
	@echo 'Done'

doctest:
	@echo 'Running doctests'
	${PYTHON} scripts/run_doctests.py --raise-on-error -v
	@echo 'Done'

lint:
	@echo 'Linting code'
	${PYTHON} -m pylint src
	${PYTHON} -m mypy src
	@echo 'Done'

format:
	@echo 'Formatting code'
	${PYTHON} -m isort --sp pyproject.toml src
	${PYTHON} -m isort --sp pyproject.toml -l ${EXTENDED_LINE_LENGTH} tests docs scripts ci
	${PYTHON} -m black --config pyproject.toml src
	${PYTHON} -m black --config pyproject.toml -l ${EXTENDED_LINE_LENGTH} tests docs scripts ci
	@echo 'Done'

build:
	@echo 'Building package'
	${PYTHON} -m hatch build
	@echo 'Done'
