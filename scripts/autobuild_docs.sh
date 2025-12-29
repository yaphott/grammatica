#!/usr/bin/env bash
set -e

if [ -z "$(which inotifywait)" ]; then
    echo "inotifywait not installed. Please install inotify-tools."
    exit 1
fi

# File and directory paths are relative to the project root
REL_EXCLUDED_FILES=()
N_REL_EXCLUDED_FILES="${#REL_EXCLUDED_FILES[@]}"

ALWAYS_EXCLUDED_FILES=(
    "README.md"
    "CHANGELOG.md"
    "LICENSE"
    "environment.yml"
    "codecov.yml"
    "requirements.txt"
    "requirements-dev.txt"
    "requirements-docs.txt"
    "requirements-test.txt"
    "pyproject.toml"
    "setup.py"
    ".coverage"
    "test-data.xml"
    "Makefile"
    ".gitignore"
    ".DS_Store"
)
N_ALWAYS_EXCLUDED_FILES="${#ALWAYS_EXCLUDED_FILES[@]}"

REL_EXCLUDED_DIRS=(
    "scripts"
    "ci"
    "docs/build"
    "docs/source/api_reference/api"
)
N_REL_EXCLUDED_DIRS="${#REL_EXCLUDED_DIRS[@]}"

ALWAYS_EXCLUDED_DIRS=(
    ".github"
    ".git"
    ".venv"
    ".mypy_cache"
    ".pytest_cache"
    ".serena"
    ".vscode"
    "__pycache__"
)
N_ALWAYS_EXCLUDED_DIRS="${#ALWAYS_EXCLUDED_DIRS[@]}"

PROJECT_DIR="$(dirname "$(dirname "$(realpath "$0")")")"
cd "$PROJECT_DIR"

[[ -z "$DEBOUNCE_SECONDS" ]] && export DEBOUNCE_SECONDS=2

function kill_pid() {
    local target_pid="$1"
    local all_pids
    all_pids="$(ps -e | awk '{print $1}')"
    if echo "$all_pids" | grep -q "^${target_pid}$"; then
        kill "$target_pid" || echo "Unable to kill PID ${target_pid}"
    fi
}

cd docs
make clean && make html
cd ..

python3 ./scripts/serve_locally.py &
server_pid="$!"
echo "Server started with PID ${server_pid}"
trap 'kill_pid "$server_pid" || true' EXIT


rel_excluded_files_pattern=''
if [[ "$N_REL_EXCLUDED_FILES" -gt 0 ]]; then
    rel_excluded_files_pattern+='(^|\./)'
    if [[ "$N_REL_EXCLUDED_FILES" -ge 2 ]]; then
        rel_excluded_files_pattern+='('
    fi
    for i in "${!REL_EXCLUDED_FILES[@]}"; do
        if [[ "$i" -gt 0 ]]; then
            rel_excluded_files_pattern+="|"
        fi
        original_path="${REL_EXCLUDED_FILES[$i]}"
        escaped_path="$(echo "$original_path" | sed -E 's/([\.\?\*\+\|\{\(\[\}\)]|\])/\\\1/g')"
        rel_excluded_files_pattern+="$escaped_path"
    done
    if [[ "$N_REL_EXCLUDED_FILES" -ge 2 ]]; then
        rel_excluded_files_pattern+=')'
    fi
    rel_excluded_files_pattern+='$'
fi
echo "rel_excluded_files_pattern=${rel_excluded_files_pattern}"

always_excluded_files_pattern=''
if [[ "$N_ALWAYS_EXCLUDED_FILES" -gt 0 ]]; then
    always_excluded_files_pattern+='(^|/)'
    if [[ "$N_ALWAYS_EXCLUDED_FILES" -ge 2 ]]; then
        always_excluded_files_pattern+='('
    fi
    for i in "${!ALWAYS_EXCLUDED_FILES[@]}"; do
        if [[ "$i" -gt 0 ]]; then
            always_excluded_files_pattern+="|"
        fi
        original_path="${ALWAYS_EXCLUDED_FILES[$i]}"
        escaped_path="$(echo "$original_path" | sed -E 's/([\.\?\*\+\|\{\(\[\}\)]|\])/\\\1/g')"
        always_excluded_files_pattern+="$escaped_path"
    done
    if [[ "$N_ALWAYS_EXCLUDED_FILES" -ge 2 ]]; then
        always_excluded_files_pattern+=')'
    fi
    always_excluded_files_pattern+='$'
fi
echo "always_excluded_files_pattern=${always_excluded_files_pattern}"

rel_excluded_dirs_pattern=''
if [[ "$N_REL_EXCLUDED_DIRS" -gt 0 ]]; then
    rel_excluded_dirs_pattern+='(^|\./)'
    if [[ "$N_REL_EXCLUDED_DIRS" -ge 2 ]]; then
        rel_excluded_dirs_pattern+='('
    fi
    for i in "${!REL_EXCLUDED_DIRS[@]}"; do
        if [[ "$i" -gt 0 ]]; then
            rel_excluded_dirs_pattern+="|"
        fi
        original_path="${REL_EXCLUDED_DIRS[$i]}"
        escaped_path="$(echo "$original_path" | sed -E 's/([\.\?\*\+\|\{\(\[\}\)]|\])/\\\1/g')"
        rel_excluded_dirs_pattern+="$escaped_path"
    done
    if [[ "$N_REL_EXCLUDED_DIRS" -ge 2 ]]; then
        rel_excluded_dirs_pattern+=')'
    fi
    rel_excluded_dirs_pattern+='(/|$)'
fi
echo "rel_excluded_dirs_pattern=${rel_excluded_dirs_pattern}"

always_excluded_dirs_pattern=''
if [[ "$N_ALWAYS_EXCLUDED_DIRS" -gt 0 ]]; then
    always_excluded_dirs_pattern+='(^|/)'
    if [[ "$N_ALWAYS_EXCLUDED_DIRS" -ge 2 ]]; then
        always_excluded_dirs_pattern+='('
    fi
    for i in "${!ALWAYS_EXCLUDED_DIRS[@]}"; do
        if [[ "$i" -gt 0 ]]; then
            always_excluded_dirs_pattern+="|"
        fi
        original_path="${ALWAYS_EXCLUDED_DIRS[$i]}"
        escaped_path="$(echo "$original_path" | sed -E 's/([\.\?\*\+\|\{\(\[\}\)]|\])/\\\1/g')"
        always_excluded_dirs_pattern+="$escaped_path"
    done
    if [[ "$N_ALWAYS_EXCLUDED_DIRS" -ge 2 ]]; then
        always_excluded_dirs_pattern+=')'
    fi
    always_excluded_dirs_pattern+='(/|$)'
fi
echo "always_excluded_dirs_pattern=${always_excluded_dirs_pattern}"

patterns=(
    "$rel_excluded_files_pattern"
    "$always_excluded_files_pattern"
    "$rel_excluded_dirs_pattern"
    "$always_excluded_dirs_pattern"
)
n_patterns="${#patterns[@]}"

excluded_pattern=''
for pattern in \
    "$rel_excluded_files_pattern" \
    "$always_excluded_files_pattern" \
    "$rel_excluded_dirs_pattern" \
    "$always_excluded_dirs_pattern"; do
    if [[ -n "$pattern" ]]; then
        if [[ -n "$excluded_pattern" ]]; then
            excluded_pattern+="|"
        fi
        if [[ "$n_patterns" -gt 1 ]]; then
            excluded_pattern+='('
        fi
        excluded_pattern+="$pattern"
        if [[ "$n_patterns" -gt 1 ]]; then
            excluded_pattern+=')'
        fi
    fi
done
echo "excluded_pattern=${excluded_pattern}"

inotifywait --recursive \
    --monitor \
    --format '%e %w%f' \
    --event create,delete,modify,move ./docs/ \
    --exclude "$excluded_pattern" \
    | while read changed; do
    echo "$changed"
    while read -t "$DEBOUNCE_SECONDS" changed; do
        echo "$changed"
    done
    echo "No more changes detected"
    kill_pid "$server_pid"
    trap - EXIT

    cd docs
    make clean && make html
    cd ..
    python3 ./scripts/serve_locally.py &
    server_pid="$!"
    echo "Server started (PID: ${server_pid})"
    trap 'kill_pid "$server_pid" || true' EXIT
done
