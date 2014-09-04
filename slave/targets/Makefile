TARGETS = $(dir $(wildcard */Makefile))

.PHONY: default ${TARGETS}

default: ${TARGETS}

${TARGETS}: %:
	make -C $@

