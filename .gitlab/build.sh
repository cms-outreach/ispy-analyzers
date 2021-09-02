#!/bin/bash

set -ex
shopt -s expand_aliases
export BUILD_DIR=${PWD}
set +u && source ${CMS_PATH}/cmsset_default.sh; set -u
cmsrel ${CMSSW_RELEASE}
cd ${CMSSW_RELEASE}/src
git clone https://github.com/cms-outreach/ispy-services.git ISpy/Services
mkdir -p ISpy/Analyzers
mv ${BUILD_DIR}/src ISpy/Analyzers/
mv ${BUILD_DIR}/interface ISpy/Analyzers/
mv ${BUILD_DIR}/python ISpy/Analyzers/
mv ${BUILD_DIR}/BuildFile.xml ISpy/Analyzers/
cmsenv
scram b -v
