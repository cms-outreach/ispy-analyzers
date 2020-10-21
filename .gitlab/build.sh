#!/bin/bash

set -ex

shopt -s expand_aliases
export BUILD_DIR=${PWD}
source ${CMS_PATH}/cmsset_default.sh
cd /home/cmsusr
export CMSSW_RELEASE=CMSSW_10_2_9
cmsrel ${CMSSW_RELEASE}
cd ${CMSSW_RELEASE}/src
git clone https://github.com/cms-outreach/ispy-services.git ISpy/Services
cmsenv
mkdir -p ISpy/Analyzers
mv ${BUILD_DIR}/src ISpy/Analyzers/
mv ${BUILD_DIR}/interface ISpy/Analyzers/
mv ${BUILD_DIR}/python ISpy/Analyzers/
mv ${BUILD_DIR}/BuildFile.xml ISpy/Analyzers/
scram b
