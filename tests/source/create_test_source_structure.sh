#!/bin/sh
# Since we need our directory structure to do the tests, and
# git doesn't support that, we create it through this script.
mkdir -p ./empty
mkdir -p ./old/directory_to_be_kept_as_is
mkdir -p ./old/directory_to_be_modified
mkdir -p ./old/empty_directory
mkdir -p ./old/empty_directory_to_be_deleted
mkdir -p ./old/non_empty_directory_to_be_deleted
mkdir -p ./new/directory_to_be_kept_as_is
mkdir -p ./new/directory_to_be_modified
mkdir -p ./new/empty_directory

