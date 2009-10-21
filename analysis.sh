#!/bin/bash
make clean

[ -d analysis ] && rm analysis -rf
mkdir analysis
cov-configure --comptype gcc --compiler /usr/bin/gcc --config analysis/config.xml
cov-build --config analysis/config.xml --dir analysis make cardy_gl
cov-analyze --config analysis/config.xml --dir analysis
cov-format-errors --dir analysis
