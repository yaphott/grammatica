# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line, and also
# from the environment for the first two.
SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = source
BUILDDIR      = build

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

clean:
	@if [ -z "$(BUILDDIR)" ]; then \
		echo "Error: BUILDDIR variable is empty"; \
		exit 1; \
	fi
	@if [ -z "$(SOURCEDIR)" ]; then \
		echo "Error: SOURCEDIR variable is empty"; \
		exit 1; \
	fi
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" clean $(O)
	@if [ -d "${SOURCEDIR}/api_reference/api" ]; then \
		echo "Removing everything under '${SOURCEDIR}/api_reference/api'..."; \
		rm -rf "${SOURCEDIR}/api_reference/api"; \
	fi


# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
