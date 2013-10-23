#!/bin/sh

#Path to closure compiler jar
CLOSURE_COMPILER_PATH=~/homemade/closure-compiler/compiler.jar

FILE="$(mktemp)"
echo "\"use strict\";(function(){" > "$FILE"
cat www/duetto.js www/nontetris.js www/webgl-util.js >> "$FILE"
echo "})();" >> "$FILE"

java -jar "$CLOSURE_COMPILER_PATH" --js="$FILE" --js_output_file=www/nontetris.min.js

rm -f "$FILE"
