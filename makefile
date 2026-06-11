MODULES = hal
BOARD ?= edu-ciaa-nxp
VERBOSE=y
MUJU ?= ./muju
DOC_DIR = ./build/doc

include $(MUJU)/module/base/makefile

doc:
	mkdir -p $(DOC_DIR)
	doxygen Doxyfile
