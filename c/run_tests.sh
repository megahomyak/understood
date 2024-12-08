#!/bin/sh

for module in parser; do
    gcc $module/test.c && (./a.out; rm a.out)
done
