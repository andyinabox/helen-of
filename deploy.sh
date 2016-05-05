#!/bin/bash

TARGET=/Volumes/Baal/

mkdir -p $TARGET/helen/helen1/bin
mkdir -p $TARGET/helen/shared

rsync -avz ./bin/ $TARGET/helen/helen1/bin/ --exclude=".DS_Store"
rsync -avz ../shared/ $TARGET/helen/shared/ --exclude=".DS_Store"

echo "Done!";
