#!/bin/bash
rm -r ../data/pipelines
mkdir ../data/pipelines
for numChars in 10000 1000000
do
    for stages in "100" "100 100" "100 100 100 100 100 100 100 100 100 100" "10 1000 10"
    do
        ../build/generate_pipeline -numChars $numChars -stages "$stages"
    done
done