#!/bin/bash
test_bin_path=../examples/simulator/out/

mkdir -p out
rm -rf out/*
mkdir -p out/coverage
cp $test_bin_path/*.gcno out/coverage
python ./nr_shell_test.py testcases $test_bin_path/nr_shell_db > out/test.log
mv $test_bin_path/*.gcda out/coverage
mv *.log out/
lcov --capture --directory ./out/coverage --output-file ./out/coverage/coverage.info
genhtml ./out/coverage/coverage.info --output-directory ./out/coverage/html
