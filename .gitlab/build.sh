#!/bin/bash

set -ex

shopt -s expand_aliases
set +u && source ${CMS_PATH}/cmsset_default.sh; set -u
cmsrel ${CMSSW_RELEASE}
cd ${CMSSW_RELEASE}/src
git clone https://github.com/cms-outreach/ispy-services.git ISpy/Services
cmsenv
mkdir -p ISpy/Analyzers
mv ${CI_PROJECT_DIR}/src ${CMSSW_BASE}/src/ISpy/Analyzers/
mv ${CI_PROJECT_DIR}/interface ${CMSSW_BASE}/src/ISpy/Analyzers/
mv ${CI_PROJECT_DIR}/python ${CMSSW_BASE}/src/ISpy/Analyzers/
mv ${CI_PROJECT_DIR}/BuildFile.xml ${CMSSW_BASE}/src/ISpy/Analyzers/
scram b
