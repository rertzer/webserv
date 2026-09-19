#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: ./checkout.sh <mybranch>"
	exit 1
fi
echo "$1"

# Restore skip-tree files prior to git checkout

git update-index --no-skip-worktree conf/*.conf
git update-index --no-skip-worktree tests/conf_test/*.conf
git restore conf/*.conf
git restore tests/conf_test/*.conf
git checkout "$1"
git update-index --skip-worktree conf/*.conf
git update-index --skip-worktree tests/conf_test/*.conf
