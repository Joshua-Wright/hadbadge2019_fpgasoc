#!/usr/bin/bash

dfu-util -d 1d50:614a,1d50:614b -a 0 -R -D soc.bit