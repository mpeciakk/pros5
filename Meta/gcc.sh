#!/bin/bash

firstString="$@"
secondString=
params="${firstString/"-arch arm64"/$secondString}"

firstString="$params"
secondString=
params="${firstString/"-Wl,-search_paths_first"/$secondString}"

/opt/homebrew/bin/i686-elf-g++ $params