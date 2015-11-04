#!/bin/bash

> combined.lpp
( cd ./parts && for x in *.lpp; do cat $x >> ../combined.lpp; echo -e "\n\n\n" >> ../combined.lpp; done )

latexpp ./combined.lpp > ./combined.tex
pdflatex -shell-escape ./combined.tex && pdflatex -shell-escape ./combined.tex && chromium ./combined.pdf