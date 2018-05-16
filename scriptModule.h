//
//  scriptModule.h
//  OceanodeLuaModule_Test
//
//  Created by Eduard Frigola on 09/05/2018.
//

#ifndef scriptModule_h
#define scriptModule_h

#include "ofxOceanodeNodeModel.h"
#include "chaiscript.hpp"
#include "chaiscript_stdlib.hpp"

class scriptModule : public ofxOceanodeNodeModel{
public:
    scriptModule();
    ~scriptModule(){};
    
    void update(ofEventArgs &a) override;

    virtual void presetSave(ofJson &json) override{
        ofSerialize(json, filename);
    };
    virtual void presetRecallBeforeSettingParameters(ofJson &json) override{
        ofDeserialize(json, filename);
    };
    
    std::vector<float> to_vector_float(const std::vector<chaiscript::Boxed_Value> &vs) {
        std::vector<float> vi;
        std::transform(vs.begin(), vs.end(), std::back_inserter(vi), [](const chaiscript::Boxed_Value &bv) { return chaiscript::Boxed_Number(bv).get_as<float>(); });
        return vi;
    }
    
    void addFloatParameter(string name, float val, float min, float max){
        if(!toCreateParameters.contains(name)){
            auto p = ofParameter<float>();
            toCreateParameters.add(p.set(name, val, min, max));
        }
    }
    
    void addIntParameter(string name, int val, int min, int max){
        if(!toCreateParameters.contains(name)){
            auto p = ofParameter<int>();
            toCreateParameters.add(p.set(name, val, min, max));
        }
    }
    
    void addBoolParameter(string name, bool val){
        if(!toCreateParameters.contains(name)){
            auto p = ofParameter<bool>();
            toCreateParameters.add(p.set(name, val));
        }
    }
    
    void addVoidParameter(string name){
        if(!toCreateParameters.contains(name)){
            auto p = ofParameter<void>();
            toCreateParameters.add(p.set(name));
        }
    }
    
    void addVectorFloatParameter(string name, vector<float> val, vector<float> min, vector<float> max){
        if(!toCreateParameters.contains(name)){
            auto p = ofParameter<vector<float>>();
            toCreateParameters.add(p.set(name, val, min, max));
        }
    }
    
    void updateParameters(){
        vector<string> parametersNames;
        for(int i = 0; i < parameters->size(); i++){
            parametersNames.push_back(parameters->get(i).getEscapedName());
        }
        for(int i = 0; i < parametersNames.size(); i++){
            if(parametersNames[i] != "Filename" && !toCreateParameters.contains(parametersNames[i])){
                parameters->remove(parametersNames[i]);
            }
        }
        for(int i = 0; i < toCreateParameters.size(); i++){
            if(!parameters->contains(toCreateParameters.get(i).getName())){
               parameters->add(toCreateParameters.get(i));
            }
        }
        toCreateParameters.clear();
    }
    
    //Getters
    float getFloatParameter(string name){
        return parameters->get(name).cast<float>().get();
    }
    
    int getIntParameter(string name){
        return parameters->get(name).cast<int>().get();
    }
    
    bool getBoolParameter(string name){
        return parameters->get(name).cast<bool>().get();
    }
    
    vector<float> getVectorFloatParameter(string name){
        return parameters->get(name).cast<vector<float>>().get();
    }
    
    //Setters
    void setFloatParameter(string name, float val){
        parameters->get(name).cast<float>().set(val);
    }
    
    void setIntParameter(string name, int val){
        parameters->get(name).cast<int>().set(val);
    }
    
    void setBoolParameter(string name, bool val){
        parameters->get(name).cast<bool>().set(val);
    }
    
    void setVoidParameter(string name){
        parameters->get(name).cast<void>().trigger();
    }
    
    void setVectorFloatParameter(string name, vector<float> val){
        parameters->get(name).cast<vector<float>>().set(val);
    }
    
    string getLastChangedParameterName(){
        return lastChangedParameterName;
    }
    
private:
    void parametersListener(ofAbstractParameter &param);
    
    ofParameter<string> filename;
    std::time_t fileLastChanged;
    
    chaiscript::ChaiScript chai;
    chaiscript::ChaiScript::State chaiInitState;
    
    std::function<bool()> listenerFunc;
    string lastChangedParameterName;
    
    bool hasValidFile;
    
    ofParameterGroup toCreateParameters;
    
    ofEventListeners listeners;
};

#endif /* scriptModule_h */
