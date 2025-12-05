#!/usr/bin/env bash
set -e

PYTHONHASHSEED=$(python -c 'import random; print(random.randint(1, 4294967295))')
export PYTHONHASHSEED
echo "PYTHONHASHSEED=$PYTHONHASHSEED"

# If no X server is found, we use xvfb to emulate it
if [[ $(uname) == "Linux" && -z $DISPLAY ]]; then
    export DISPLAY=":0"
    xvfb="xvfb-run "
fi

pytest_target=tests
pytest_cmd="${xvfb}pytest -r fEs -s --cov=src --cov-report=xml --cov-append $pytest_target"

echo "$pytest_cmd"
sh -c "$pytest_cmd"
