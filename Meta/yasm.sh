#!/bin/bash

firstString="$@"
secondString=
params="${firstString/"-arch arm64"/$secondString}"

/opt/homebrew/bin/yasm $params