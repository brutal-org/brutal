#!/bin/bash

MARKDOWN_FILES="`find . | sort | grep ".md"`"

OUTPUT="brutal.pdf"
TEMPLATE="template.tex"
PANDOC_FLAGS="--from markdown \
		--toc \
		--template=template.tex \
		--variable subparagraph \
		-f markdown-implicit_figures \
		-V papersize:a4 \
		-V book \
		-V classoption=oneside \
		--top-level-division=chapter \
		--pdf-engine=xelatex \
		--highlight-style breezedark \
		--citeproc \
		--listings \
		--metadata ./metadata.yml"

pandoc $PANDOC_FLAGS -o "brutal.pdf" $MARKDOWN_FILES


