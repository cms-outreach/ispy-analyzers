#!/bin/bash

set -ex

shopt -s expand_aliases
set +u && source ${CMS_PATH}/cmsset_default.sh; set -u
# CMSSW_RELEASE doesn't propagate to here and is "unbound". Fix.
cmsrel CMSSW_10_2_9
cd CMSSW_10_2_9/src
git clone https://github.com/cms-outreach/ispy-services.git ISpy/Services
cmsenv
mkdir -p ISpy/Analyzers
mv ${CI_PROJECT_DIR}/src ${CMSSW_BASE}/src/ISpy/Analyzers/
mv ${CI_PROJECT_DIR}/interface ${CMSSW_BASE}/src/ISpy/Analyzers/
mv ${CI_PROJECT_DIR}/python ${CMSSW_BASE}/src/ISpy/Analyzers/
mv ${CI_PROJECT_DIR}/BuildFile.xml ${CMSSW_BASE}/src/ISpy/Analyzers/
scram b
