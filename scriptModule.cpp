//
//  scriptModule.cpp
//  OceanodeLuaModule_Test
//
//  Created by Eduard Frigola on 09/05/2018.
//

#include "scriptModule.h"

scriptModule::scriptModule() : ofxOceanodeNodeModel("Script Module"){
    parameters->add(filename.set("Filename", ""));
    hasValidFile = false;
    //filename = "button_gate.chai";
    //fileLastChanged = std::filesystem::last_write_time(ofToDataPath(filename));
    
    listeners.push(parameters->parameterChangedE().newListener(this, &scriptModule::parametersListener));
    
    chai.add(chaiscript::fun(&scriptModule::addFloatParameter), "addFloatParameter");
    chai.add(chaiscript::fun(&scriptModule::addIntParameter), "addIntParameter");
    chai.add(chaiscript::fun(&scriptModule::addBoolParameter), "addBoolParameter");
    chai.add(chaiscript::fun(&scriptModule::addVoidParameter), "addVoidParameter");
    chai.add(chaiscript::fun(&scriptModule::addVectorFloatParameter), "addVectorFloatParameter");
    
    chai.add(chaiscript::fun(&scriptModule::getFloatParameter), "getFloatParameter");
    chai.add(chaiscript::fun(&scriptModule::getIntParameter), "getIntParameter");
    chai.add(chaiscript::fun(&scriptModule::getBoolParameter), "getBoolParameter");
    chai.add(chaiscript::fun(&scriptModule::getVectorFloatParameter), "getVectorFloatParameter");
    
    chai.add(chaiscript::fun(&scriptModule::setFloatParameter), "setFloatParameter");
    chai.add(chaiscript::fun(&scriptModule::setIntParameter), "setIntParameter");
    chai.add(chaiscript::fun(&scriptModule::setBoolParameter), "setBoolParameter");
    chai.add(chaiscript::fun(&scriptModule::setVoidParameter), "setVoidParameter");
    chai.add(chaiscript::fun(&scriptModule::setVectorFloatParameter), "setVectorFloatParameter");
    
    chai.add(chaiscript::fun(&scriptModule::getLastChangedParameterName), "getLastChangedParameterName");
    
    chai.add(chaiscript::user_type<scriptModule>(), "scriptModule");
    chai.add(chaiscript::bootstrap::standard_library::vector_type<vector<float>>("FloatVector"));
    chai.add(chaiscript::type_conversion<std::vector<chaiscript::Boxed_Value>, std::vector<float>>(
        [&](const std::vector<chaiscript::Boxed_Value> &t_bvs) {
            return to_vector_float(t_bvs);
        }));
    chai.add_global(chaiscript::var(this), "this");
    
    chaiInitState = chai.get_state();
}


void scriptModule::update(ofEventArgs &a){
    if(hasValidFile){
        auto currentLocale = std::filesystem::last_write_time(ofToDataPath("scripts/" + filename.get()));
        if(currentLocale != fileLastChanged){
            //parameters->clear();
            chai.set_state(chaiInitState);
            try{
                chai.eval_file(ofToDataPath("scripts/" + filename.get()));
            }catch (std::exception &e){
                ofLog() << e.what();
            }
            try{
                listenerFunc = chai.eval<std::function<bool()>>("listenerFunc");
            }catch (std::exception &e){
                ofLog() << e.what();
            }
            updateParameters();
            ofNotifyEvent(parameterGroupChanged);
            fileLastChanged = currentLocale;
            ofLog() <<"File changed  " <<  ofGetTimestampString();
        }
    }
}

//TODO: Have to listen to void parameters separately
void scriptModule::parametersListener(ofAbstractParameter &param){
    if(param.getName() == filename.getName()){
        ofFile file;;
        hasValidFile = file.doesFileExist(ofToDataPath("scripts/" + filename.get()));
    }
    else{
        lastChangedParameterName = param.getName();
        try{
            listenerFunc();
        }
        catch (std::exception &e){
            ofLog() << e.what();
        }
    }
}

