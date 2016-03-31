#!/bin/bash

cd $WCSIMDIR
for f in testing/*mac; do echo $f; WCSim $f 2>&1 | grep 'Using dark'; done
