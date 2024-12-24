# Name of your LaTeX project (without extension)
PROJECT = report

# List of intermediate/auxiliary files to clean
AUX_FILES = $(PROJECT).aux $(PROJECT).bbl $(PROJECT).bcf $(PROJECT).blg $(PROJECT).log $(PROJECT).out $(PROJECT).toc $(PROJECT).run.xml 

# Default target: Compile the PDF
all: $(PROJECT).pdf

# Rule to generate the PDF
$(PROJECT).pdf: $(PROJECT).tex $(PROJECT).bib
	pdflatex $(PROJECT).tex     # First pass
	biber $(PROJECT)            # Run biber for bibliography
	pdflatex $(PROJECT).tex     # Second pass for cross-references
	pdflatex $(PROJECT).tex     # Third pass for final output

# Clean all intermediate files
clean:
	rm -f $(AUX_FILES)

# Clean everything, including the compiled PDF
cleanall: clean
	rm -f $(PROJECT).pdf