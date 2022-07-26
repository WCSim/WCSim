#!/bin/bash

echo "Getting current develop branch from WCSim/WCSim repository"
git clone -b develop --single-branch git@github.com:WCSim/WCSim.git WCSim-latest
cd WCSim-latest

echo "Copying gh-pages version of WCSim_doxygen_config to WCSim-latest"
echo " (it requires dot (part of graphvis), which isn't on every system by default)"
cp -f ../WCSim_doxygen_config .

echo "Running doxygen"
make doxy

echo "Getting current develop hash"
git rev-parse HEAD > hash

echo "Removing old version of docs in gh-pages branch"
cd ..
rm -rf *.html *.map *.png *.md5 search

echo "Copying new version of docs into gh-pages branch"
cp -r WCSim-latest/doc/doxygen/html/* .

echo "Committing new version of docs"
git commit -a -m "docs for WCSim hash `cat WCSim-latest/hash`"
git push origin gh-pages

echo "Cleaning up download of new repository"
rm -rf WCSim-latest
