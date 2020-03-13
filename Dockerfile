### Created by Dr. Benjamin Richards (b.richards@qmul.ac.uk) 

### Download base image from cern repo on docker hub
FROM wcsim/wcsim:base

USER nu

### Get and build WCSim
WORKDIR $HYPERKDIR
RUN git clone https://github.com/WCSim/WCSim
WORKDIR $WCSIMDIR
RUN source ../env-WCSim.sh \
    && make clean \
    && make rootcint \
    && make \
    && make
# sometimes a second `make` is required for the library to link correctly on a fresh clone. I don't understand why

### Open terminal
ENTRYPOINT source $HYPERKDIR/env-WCSim.sh && /bin/bash
 
