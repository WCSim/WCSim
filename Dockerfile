### Created by Dr. Benjamin Richards (b.richards@qmul.ac.uk) 

### Download base image from cern repo on docker hub
FROM wcsim/wcsim:base

USER nu

### Compile an up-to-date copy of the WCSim code
WORKDIR $WCSIMDIR
RUN source ../env-WCSim.sh \
    && git pull \
    && make clean \
    && make rootcint \
    && make \
    && make

### Open terminal
ENTRYPOINT source $HYPERKDIR/env-WCSim.sh && /bin/bash
 
