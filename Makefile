DOXYGEN_VERSION := $(shell doxygen --version 2>/dev/null)
ifdef DOXYGEN_VERSION
DOXYGEN_EXISTS = 1
else
DOXYGEN_EXISTS = 0
endif

.PHONY: 

doxy:
	@if [ ${DOXYGEN_EXISTS} = 1 ]; \
	then \
		doxygen WCSim_doxygen_config; \
	else\
		echo "Error: doxygen program not found in path. Exiting"; \
	fi

clean:
	echo "NEED TO CLEAN"
