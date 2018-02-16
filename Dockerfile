### Created by Dr. Benjamin Richards (b.richards@qmul.ac.uk) 

### Download base image from cern repo on docker hub
FROM wcsim/wcsim:base

### Run the following commands as super user (root):
USER root

RUN cd /root/HyperK/WCSim \
    && git pull \
    && make clean \
    && make rootcint \
    && make

### Open terminal
CMD ["/bin/bash"]
 
