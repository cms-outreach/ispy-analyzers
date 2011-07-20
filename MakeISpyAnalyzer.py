#! /usr/bin/env python

import sys
import re
import string

source_skeleton = ['#include \"ISpy/Analyzers/interface/ISpyExample.h\"',
                   '#include \"ISpy/Analyzers/interface/ISpyService.h\"\n',
                   '#include \"FWCore/Framework/interface/Event.h\"',
                   '#include \"FWCore/Framework/interface/EventSetup.h\"',
                   '#include \"FWCore/Framework/interface/ESHandle.h\"',
                   '#include \"FWCore/Framework/interface/MakerMacros.h\"',
                   '#include \"FWCore/ParameterSet/interface/ParameterSet.h\"',
                   '#include \"FWCore/ServiceRegistry/interface/Service.h\"',
                   '#include \"FWCore/Utilities/interface/Exception.h\"',
                   '#include \"ISpy/Services/interface/IgCollection.h\"\n',
                   'using namespace edm::service;',
                   'using namespace edm;\n',
                   'ISpyExample::ISpyExample(const edm::ParameterSet& iConfig)',
                   ': inputTag_(iConfig.getParameter<edm::InputTag>(\"iSpyExampleTag\")){}\n',
                   'void ISpyExample::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)',
                   '{',
                   '  edm::Service<ISpyService> config;\n',    
                   '  if ( ! config.isAvailable() )', 
                   '  {',
                   '    throw cms::Exception (\"Configuration\")',
                   '      << \"ISpyExample requires the ISpyService\\n\"',
                   '     \"which is not present in the configuration file.\\n\"',
                   '     \"You must add the service in the configuration file\\n\"',
                   '     \"or remove the module that requires it\";',
                   '  }\n',
                   '  IgDataStorage *storage = config->storage();\n',
                   '#ifdef EXAMPLE',
                   '  edm::Handle<ExampleCollection> collection;',
                   '  event.getByLabel(inputTag_, collection);\n',
                   '  if ( collection.isValid() )',
                   '  {',	    	
                   '    std::string product = \"Examples \"',
                   '                           + edm::TypeID (typeid (ExampleCollection)).friendlyClassName() + \":\"', 
                   '                           + inputTag_.label() + \":\"',
                   '                           + inputTag_.instance() + \":\"', 
                   '                           + inputTag_.process();\n',
	           '     IgCollection& products = storage->getCollection(\"Products_V1\");',
 	           '     IgProperty PROD = products.addProperty(\"Product\", std::string ());',
 	           '     IgCollectionItem item = products.create();',
 	           '     item[PROD] = product;\n',
	           '     IgCollection& templates = storage->getCollection(\"Examples_V1\");\n',
	           '     IgProperty POS = templates.addProperty(\"position\", IgV3d());',
	           '     IgProperty E   = templates.addProperty(\"energy\", 0.0);\n',
	           '     for ( ExampleCollection::const_iterator t = collection->begin(); t != collection->end(); ++t )',
	           '     {',
                   '       IgCollectionItem item = templates.create();\n',	
	           '       item[POS] = IgV3d((*t).position().x(),',
		   '                         (*t).position().y(),',
		   '                         (*t).position().z());\n',
                   '       item[E] = (*t).energy();',
                   '     }',
                   '   }\n',
                   '   else', 
                   '   {',
                   '     std::string error = \"### Error: Examples \"',
                   '                          + edm::TypeID (typeid (ExampleCollection)).friendlyClassName() + \":\"', 
                   '                          + inputTag_.label() + \":\"',
                   '                          + inputTag_.instance() + \" are not found\";', 
                   '      config->error(error);',
                   '  }',
                   '#endif\n',
                   '}\n',
                   'DEFINE_FWK_MODULE(ISpyExample);']


interface_skeleton = ['#ifndef ANALYZER_ISPY_EXAMPLE_H',
                      '#define ANALYZER_ISPY_EXAMPLE_H\n',
                      '#include \"FWCore/Framework/interface/EDAnalyzer.h\"\n',
                      '#include \"FWCore/Utilities/interface/InputTag.h\"\n',
                      'class ISpyExample : public edm::EDAnalyzer',
                      '{',
                      'public:',
                      '    explicit ISpyExample(const edm::ParameterSet&);',
                      '    virtual ~ISpyExample(void){}',
                      '    virtual void analyze(const edm::Event&, const edm::EventSetup&);',
                      'private:',
                      '    edm::InputTag inputTag_;',
                      '};',
                      '#endif // ANALYZER_ISPY_EXAMPLE_H']

cfg_skeleton = ['import FWCore.ParameterSet.Config as cms\n',
                'ISpyExample = cms.EDAnalyzer(\'ISpyExample\',',
                '                            iSpyExampleTag = cms.InputTag(\"examples\")',
                '                           )']

def generateSource(class_name):

    source_file_name = './src/ISpy'+class_name+'.cc'

    try:
        source_file = open(source_file_name, 'w')

        for line in source_skeleton:
            if re.search('Example', line):
                line = string.replace(line, 'Example', class_name)
            source_file.write(line+'\n')

        print 'Wrote', source_file_name

    except IOError:
        print source_file_name, 'not found'
        print 'Check that you have a ./src directory'
        sys.exit()

def generateInterface(class_name):

    interface_file_name = './interface/ISpy'+class_name+'.h'

    try:
        interface_file = open(interface_file_name, 'w')

        for line in interface_skeleton:
            if re.search('_H', line) and re.search('EXAMPLE', line):
                line = string.replace(line, 'EXAMPLE', class_name.upper())
            if re.search('Example', line):
                line = string.replace(line, 'Example', class_name)
            interface_file.write(line+'\n')

        print 'Wrote', interface_file_name

    except IOError:
        print interface_file_name, 'not found'
        print 'Check that you have a ./interface directory'
        sys.exit()
                
def generateCfg(class_name):

    cfg_file_name = './python/ISpy'+class_name+'_cfi.py'

    try:
        cfg_file = open(cfg_file_name, 'w')

        for line in cfg_skeleton:
            if re.search('Example', line):
                line = string.replace(line, 'Example', class_name)
            cfg_file.write(line+'\n')

        print 'Wrote', cfg_file_name

    except IOError:
        print cfg_file_name, 'not found'
        print 'Check that you have a ./python directory'
        sys.exit() 

if len(sys.argv) != 2:
    print 'Usage: python MakeISpyAnalyzer.py [\"target\" of analyzer, e.g. EBRecHit]'
    sys.exit()

aname = sys.argv[1]

generateSource(aname)
generateInterface(aname)
generateCfg(aname)


