#!/bin/bash

pdflatex 250manual
pdflatex 250manual

rm -f 250manual.aux
rm -f 250manual.log
rm -f 250manual.out
rm -f 250manual.toc
rm -f 250manual.lof
