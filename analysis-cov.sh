#!/bin/bash
make clean

[ -d analysis-cov ] && rm analysis-cov -rf
mkdir analysis-cov
cov-configure --comptype gcc --compiler /usr/bin/gcc --config analysis-cov/config.xml
cov-build --config analysis-cov/config.xml --dir analysis-cov make cardy_gl
cov-analyze --config analysis-cov/config.xml --dir analysis-cov --all
cov-format-errors --dir analysis-cov
