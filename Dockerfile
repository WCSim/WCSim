### Created by Dr. Benjamin Richards (b.richards@qmul.ac.uk) 

### Download base image from cern repo on docker hub
FROM wcsim/wcsim:base

### Run the following commands as super user (root):
USER root

RUN cd /root/HyperK/WCSim \
    && source $HYPERKDIR/root/bin/thisroot.sh \
    && source $G4INSTALL/bin/geant4.sh \
    && source $G4INSTALL/share/Geant4-10.1.3/geant4make/geant4make.sh \
    && git pull \
    && make clean \
    && make rootcint \
    && make

### Open terminal
CMD ["/bin/bash"]
 
