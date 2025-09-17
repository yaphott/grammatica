PYTHON=python3
APP_NAME=grammatica

install:
	${PYTHON} -m pip install .

install-dev:
	${PYTHON} -m pip install -e .

test:
	@echo 'Running tests'
	${PYTHON} -m pytest tests
	@echo 'Done'

lint:
	@echo 'Linting code'
	${PYTHON} -m pylint src
	${PYTHON} -m mypy src
	@echo 'Done'

format:
	@echo 'Formatting code'
	${PYTHON} -m isort --sp pyproject.toml src tests docs scripts
	${PYTHON} -m black --config pyproject.toml src tests docs scripts
	@echo 'Done'

build:
	@echo 'Building package'
	${PYTHON} -m hatch build
	@echo 'Done'
