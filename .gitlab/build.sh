#!/bin/bash

set -ex

shopt -s expand_aliases
export BUILD_DIR=${PWD}
source ${CMS_PATH}/cmsset_default.sh
# CMSSW_RELEASE doesn't propagate to here and is "unbound". Fix.
cmsrel CMSSW_10_2_9
cd CMSSW_10_2_9/src
git clone https://github.com/cms-outreach/ispy-services.git ISpy/Services
cmsenv
mkdir -p ISpy/Analyzers
mv ${BUILD_DIR}/src CMSSW_10_2_9/src/ISpy/Analyzers/
mv ${BUILD_DIR}/interface CMSSW_10_2_9/src/ISpy/Analyzers/
mv ${BUILD_DIR}/python CMSSSW_10_2_9/src/ISpy/Analyzers/
mv ${BUILD_DIR}/BuildFile.xml CMSSW_10_2_9/src/ISpy/Analyzers/
scram b
