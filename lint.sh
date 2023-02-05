#!/bin/bash

FILES="\
Mainboard_ESP32/main/main.c
Mainboard_ESP32/components
"

OPTIONS="\
--recursive
"

FILTERS="\
-build/include_subdir,\
-build/header_guard,\
-readability/casting
"

cpplint ${OPTIONS} --linelength=80 --filter=${FILTERS} ${FILES}