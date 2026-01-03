#!/usr/bin/env bash
set -e

if [ -z "$(which inotifywait)" ]; then
    echo "inotifywait not installed. Please install inotify-tools."
    exit 1
fi

PROJECT_DIR="$(dirname "$(dirname "$(realpath "$0")")")"
cd "$PROJECT_DIR"

[[ -z "$DEBOUNCE_SECONDS" ]] && export DEBOUNCE_SECONDS=1.0

TARGET_REL_DIRS=(
    "src"
    "docs"
)
TARGET_DIRS=()
for rel_dir in "${TARGET_REL_DIRS[@]}"; do
    TARGET_DIRS+=("$PROJECT_DIR/$rel_dir")
done

EXCLUDE_REL_FILES=()
EXCLUDE_FILES=()
for rel_file in "${EXCLUDE_REL_FILES[@]}"; do
    EXCLUDE_FILES+=("$PROJECT_DIR/$rel_file")
done
EXCLUDE_FILE_NAMES=(
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
EXCLUDE_REL_DIRS=(
    "scripts"
    "ci"
    "docs/build"
    "docs/source/api_reference/api"
)
EXCLUDE_DIRS=()
for rel_dir in "${EXCLUDE_REL_DIRS[@]}"; do
    EXCLUDE_DIRS+=("$PROJECT_DIR/$rel_dir")
done
EXCLUDE_DIR_NAMES=(
    ".github"
    ".git"
    ".venv"
    ".mypy_cache"
    ".pytest_cache"
    ".serena"
    ".vscode"
    "__pycache__"
)

function kill_pid() {
    local target_pid="$1"
    local all_pids
    all_pids="$(ps -e | awk '{print $1}')"
    if echo "$all_pids" | grep -q "^${target_pid}$"; then
        kill "$target_pid" || echo "Unable to kill PID ${target_pid}"
    fi
}

function path_pattern() {
    local is_full_path="$1"
    local is_dir="$2"
    shift 2
    local paths=("$@")
    local n="${#paths[@]}"
    local pattern=''
    if [[ "$n" -gt 0 ]]; then
        pattern+="^"
        if [[ "$is_full_path" != 'true' ]]; then
            pattern+='/([^/]+/)*'
        fi
        if [[ "$n" -ge 2 ]]; then
            pattern+='('
        fi
        for i in "${!paths[@]}"; do
            if [[ "$i" -gt 0 ]]; then
                pattern+="|"
            fi
            escaped_path="$(echo "${paths[$i]}" | sed -E 's/([\.\?\*\+\|\{\(\[\}\)]|\])/\\\1/g')"
            pattern+="$escaped_path"
        done
        if [[ "$n" -ge 2 ]]; then
            pattern+=')'
        fi
        if [[ "$is_dir" == 'true' ]]; then
            pattern+='(/[^/]+)*'
        fi
        pattern+='$'
    fi
    echo "$pattern"
}

exclude_files_pattern="$(path_pattern true false "${EXCLUDE_FILES[@]}")"
echo "exclude_files_pattern=${exclude_files_pattern}"
exclude_file_names_pattern="$(path_pattern false false "${EXCLUDE_FILE_NAMES[@]}")"
echo "exclude_file_names_pattern=${exclude_file_names_pattern}"
exclude_dirs_pattern="$(path_pattern true true "${EXCLUDE_DIRS[@]}")"
echo "exclude_dirs_pattern=${exclude_dirs_pattern}"
exclude_dir_names_pattern="$(path_pattern false true "${EXCLUDE_DIR_NAMES[@]}")"
echo "exclude_dir_names_pattern=${exclude_dir_names_pattern}"
excluded_pattern=''
patterns=(
    "$exclude_files_pattern"
    "$exclude_file_names_pattern"
    "$exclude_dirs_pattern"
    "$exclude_dir_names_pattern"
)
n_patterns="${#patterns[@]}"
for pattern in "${patterns[@]}"; do
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

server_pid=''
cd docs
if make clean && make html; then
    python3 ../scripts/serve_locally.py &
    server_pid="$!"
    echo "Server started (PID: ${server_pid})"
    trap 'kill_pid "$server_pid" || true' EXIT
else
    echo 'Failed to build documentation'
fi
cd ..

FORMATTED_TARGET_DIRS=()
for directory in "${TARGET_DIRS[@]}"; do
    FORMATTED_TARGET_DIRS+=("${directory}/")
done
inotifywait --recursive \
    --monitor \
    --format '%e %w%f' \
    --event create,delete,modify,move "${FORMATTED_TARGET_DIRS[@]}" \
    --exclude "$excluded_pattern" \
 | while read -r changed; do
    echo "$changed"
    while read -t "$DEBOUNCE_SECONDS" -r changed; do
        echo "$changed"
    done
    echo "No additional changes detected"

    if [[ -n "$server_pid" ]]; then
        kill_pid "$server_pid"
        server_pid=''
    fi
    trap - EXIT

    cd docs
    if make clean && make html; then
        python3 ../scripts/serve_locally.py &
        server_pid="$!"
        echo "Server started (PID: ${server_pid})"
        trap 'kill_pid "$server_pid" || true' EXIT
    else
        echo 'Failed to build documentation'
    fi
    cd ..
done
