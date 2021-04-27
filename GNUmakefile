
ifneq (,$(wildcard Makefile))
  include Makefile # CMake in-place build
else ifdef G4INSTALL
  include GNUmakefile_full # GNU make build
else
  include GNUmakefile_root # GNU make build
endif

