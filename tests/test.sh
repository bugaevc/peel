#! /bin/bash
set -e

mkdir -p actual
tests_dir="$( dirname -- "$BASH_SOURCE" )"
root_dir="$tests_dir/.."
peel_gen_path="$root_dir"/peel_gen/__init__.py
(cd actual && GI_GIR_PATH="$tests_dir" exec python3 -- "$peel_gen_path" --api-tweaks "$root_dir"/api-tweaks.txt Test 1)
exec diff --color=auto -ur -- "$tests_dir"/expected/ actual/
