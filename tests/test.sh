#! /bin/bash
set -e

mkdir -p actual
tests_dir="$( dirname -- "$BASH_SOURCE" )"
root_dir="$tests_dir/.."
peel_gen_path="$root_dir"/peel-gen.py
(cd actual && GI_GIR_PATH="$tests_dir" exec "$peel_gen_path" Test 1 --api-tweaks "$tests_dir"/api-tweaks.txt)
exec diff --color=auto -ur -- "$tests_dir"/expected/ actual/
