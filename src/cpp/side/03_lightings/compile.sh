#!/bin/bash

# check working directory 
echo "$PWD"

# Define the emsdk repo variables
name=~/Projects/Graphics/emsdk/

echo "\nMake the "latest" SDK "active" for the current user. (writes .emscripten file)"
$name/./emsdk activate latest

echo "\nActivate PATH and other environment variables in the current terminal"
source $name/./emsdk_env.sh

echo "\nCheck Emscripten Version"
emcc -v

make