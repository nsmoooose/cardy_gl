#!/bin/bash
[ -d analysis-cppcheck ] && rm analysis-cppcheck -rf
mkdir analysis-cppcheck
cppcheck . --enable=all --xml 2> analysis-cppcheck/errors.xml
cppcheck-htmlreport --file analysis-cppcheck/errors.xml --report-dir=analysis-cppcheck --source-dir=.
