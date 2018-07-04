#!/bin/bash

mkdir -p bin/data/helen-images/
aws s3 sync s3://helen-images bin/data/helen-images

