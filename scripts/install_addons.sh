#!/bin/sh


cd ../../../addons

git clone git@github.com:jeffcrouse/ofxJSON.git

git clone git@github.com:andyinabox/ofxEventThreadedImageLoader.git

# stable branch works for 0.9.8 (at least right now it does)
git clone -b stable git@github.com:kylemcdonald/ofxCv.git

# git clone git@github.com:bakercp/ofxPS3EyeGrabber.git

# need to get a specific version of ofxFaceTracker for this to work
# with oF < 0.9.8
git clone git@github.com:kylemcdonald/ofxFaceTracker.git
cd ofxFaceTracker
git reset --hard 0aa42adeb9bb0517d9b9d063162c0160481ce965
cd ..

# listed as optional dependencies for ofxFaceTracker, not sure
# if these are necessary
git clone git@github.com:kylemcdonald/ofxTiming.git
git clone git@github.com:obviousjim/ofxDelaunay.git
