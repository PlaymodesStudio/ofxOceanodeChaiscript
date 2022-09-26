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
    virtual void loadBeforeConnections(ofJson &json) override;
    virtual void presetHasLoaded() override;
    
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
        for(int i = 0; i < getParameterGroup().size(); i++){
            parametersNames.push_back(getParameterGroup().get(i).getEscapedName());
        }
        for(int i = 0; i < parametersNames.size(); i++){
            bool paramHasToRemove = true;
            if(parametersNames[i] == "File") paramHasToRemove = false;
            else if(toCreateParameters.contains(parametersNames[i])){
                paramHasToRemove = false;
                if(getParameterGroup().get(parametersNames[i]).valueType() != toCreateParameters.get(parametersNames[i]).valueType()){
                    paramHasToRemove = true;
                }
                else if(getParameterGroup().get(parametersNames[i]).valueType() == typeid(float).name()){
                    auto &alreadyParam = getParameter<float>(parametersNames[i]);
                    auto &toCreateParam = toCreateParameters.getFloat(parametersNames[i]);
                    if(alreadyParam.getMin() != toCreateParam.getMin() || alreadyParam.getMax() != toCreateParam.getMax()){
                        alreadyParam.setMin(toCreateParam.getMin());
                        alreadyParam.setMax(toCreateParam.getMax());
                    }
                }
                else if(getParameterGroup().get(parametersNames[i]).valueType() == typeid(int).name()){
                    auto &alreadyParam = getParameter<int>(parametersNames[i]);
                    auto &toCreateParam = toCreateParameters.getInt(parametersNames[i]);
                    if(alreadyParam.getMin() != toCreateParam.getMin() || alreadyParam.getMax() != toCreateParam.getMax()){
                        alreadyParam.setMin(toCreateParam.getMin());
                        alreadyParam.setMax(toCreateParam.getMax());
                    }
                }
            }
            if(paramHasToRemove){
                getParameterGroup().remove(parametersNames[i]);
                storedFloatParameters.erase(parametersNames[i]);
                storedIntParameters.erase(parametersNames[i]);
                storedVecFloatParameters.erase(parametersNames[i]);
                storedBoolParameters.erase(parametersNames[i]);
                storedVoidParameters.erase(parametersNames[i]);
            }
        }
        for(int i = 0; i < toCreateParameters.size(); i++){
            auto &param = toCreateParameters.get(i);
            if(!getParameterGroup().contains(param.getName())){
                if(param.valueType() == typeid(float).name()){
                    storedFloatParameters[param.getName()] = ofParameter<float>();
                    addParameter(storedFloatParameters[param.getName()].set(param.getName(), param.cast<float>().get(), param.cast<float>().getMin(), param.cast<float>().getMax()));
                }
                else if(param.valueType() == typeid(int).name()){
                    storedIntParameters[param.getName()] = ofParameter<int>();
                    addParameter(storedIntParameters[param.getName()].set(param.getName(), param.cast<int>().get(), param.cast<int>().getMin(), param.cast<int>().getMax()));
                }
                else if(param.valueType() == typeid(vector<float>).name()){
                    storedVecFloatParameters[param.getName()] = ofParameter<vector<float>>();
                    addParameter(storedVecFloatParameters[param.getName()].set(param.getName(), param.cast<vector<float>>().get(), param.cast<vector<float>>().getMin(), param.cast<vector<float>>().getMax()));
                }
                else if(param.valueType() == typeid(bool).name()){
                    storedBoolParameters[param.getName()] = ofParameter<bool>();
                    addParameter(storedBoolParameters[param.getName()].set(param.getName(), param.cast<bool>().get()));
                }
                else if(param.valueType() == typeid(void).name()){
                    storedVoidParameters[param.getName()] = ofParameter<void>();
                    string paramName = param.getName();
                    listeners.push(storedVoidParameters[param.getName()].newListener([this, paramName](){
                        lastChangedParameterName = paramName;
                        try{
                            listenerFunc();
                        }
                        catch (std::exception &e){
                            ofLog() << e.what();
                        }
                    }));
                    addParameter(storedVoidParameters[param.getName()].set(paramName));
                }
            }
        }
        toCreateParameters.clear();
    }
    
    //Getters
    float getFloatParameter(string name){
        if(getParameterGroup().contains(name))
            return getParameter<float>(name).get();
        
        return 0;
    }
    
    int getIntParameter(string name){
        if(getParameterGroup().contains(name))
            return getParameter<int>(name).get();
        
        return 0;
    }
    
    bool getBoolParameter(string name){
        if(getParameterGroup().contains(name))
            return getParameter<bool>(name).get();
        
        return false;
    }
    
    vector<float> getVectorFloatParameter(string name){
        if(getParameterGroup().contains(name))
            return getParameter<vector<float>>(name);
        
        return {0};
    }
    
    //Setters
    void setFloatParameter(string name, float val){
        if(getParameterGroup().contains(name))
            getParameter<float>(name).set(val);
    }
    
    void setIntParameter(string name, int val){
        if(getParameterGroup().contains(name))
            getParameter<int>(name).set(val);
    }
    
    void setBoolParameter(string name, bool val){
        if(getParameterGroup().contains(name))
            getParameter<bool>(name).set(val);
    }
    
    void setVoidParameter(string name){
        if(getParameterGroup().contains(name))
            getParameter<void>(name).trigger();
    }
    
    void setVectorFloatParameter(string name, vector<float> val){
        if(getParameterGroup().contains(name))
            getParameter<vector<float>>(name).set(val);
    }
    
    string getLastChangedParameterName(){
        return lastChangedParameterName;
    }
    
private:
    void parametersListener(ofAbstractParameter &param);
    void loadFile();
    
    ofParameter<string> filename;
    std::filesystem::file_time_type fileLastChanged;
    
    chaiscript::ChaiScript chai;
    chaiscript::ChaiScript::State chaiInitState;
    
    std::function<bool()> listenerFunc;
    std::function<bool()> presetLoad;
    string lastChangedParameterName;
    
    bool hasValidFile;
    string fileDialogFilename;
    
    ofParameterGroup toCreateParameters;
    map<string, ofParameter<float>> storedFloatParameters;
    map<string, ofParameter<int>> storedIntParameters;
    map<string, ofParameter<vector<float>>> storedVecFloatParameters;
    map<string, ofParameter<bool>> storedBoolParameters;
    map<string, ofParameter<void>> storedVoidParameters;
    
    ofEventListeners listeners;
};

#endif /* scriptModule_h */
