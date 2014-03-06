#!/bin/sh
#
#*****************************************************************************
#
#	 Copyright (C) 2013  Daniele Di Proietto <d.diproietto@sssup.it>
#	 
#	 This file is part of nontetris.
#	 
#	 nontetris is free software: you can redistribute it and/or modify
#	 it under the terms of the GNU General Public License as published by
#	 the Free Software Foundation, either version 3 of the License, or
#	 (at your option) any later version.
#	 
#	 nontetris is distributed in the hope that it will be useful,
#	 but WITHOUT ANY WARRANTY; without even the implied warranty of
#	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	 GNU General Public License for more details.
#	 
#	 You should have received a copy of the GNU General Public License
#	 along with nontetris.  If not, see <http://www.gnu.org/licenses/>.
#
#*****************************************************************************
#
#Path to closure compiler jar
CLOSURE_COMPILER_PATH=/usr/share/java/closure-compiler.jar

FILE="$(mktemp)"
echo "\"use strict\";(function(){" > "$FILE"
cat www/nontetris-duetto.js www/webgl-util.js >> "$FILE"
echo "})();" >> "$FILE"

java -jar "$CLOSURE_COMPILER_PATH" --language_in=ECMASCRIPT5 --js="$FILE" --js_output_file=www/nontetris-duetto.min.js

rm -f "$FILE"
