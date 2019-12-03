#ifndef GO_FACTORY_H_
#define GO_FACTORY_H_

#include <list>
#include "SceneReader.h"

class Component;
class GameObject;

// macro to register each component
// it needs to be called in each "component".cpp
#define REGISTER_TYPE(klass) \
    class klass##Factory : public GOFactory { \
    public: \
        klass##Factory() \
        { \
            GOFactory::registerType(#klass, this); \
        } \
        virtual Component* create(nap_json const & cfg, GameObject* owner) { \
            return new klass(cfg, owner); \
        } \
    }; \
    static klass##Factory global_##klass##Factory;

//all pointers so no .h needed
class GOFactory
{
public:
	GOFactory() {}
	inline static void cleanFactories() {
		factories->clear();
		delete factories;
	}

	// parses a GOType into a GO
	static GameObject* ParseGO(GOStruct & cfg);

	// gets the GO parsed of a prefab
	static GameObject* GetGOPrefab(std::string const & name);

	// parses a nap_json components_cfg into a list of configured components*
	static std::list<Component*> ParseComponents(GameObject *o, nap_json & components_cfg);

protected:
	// used to automate components registration
	static void registerType(const string& name, GOFactory* factory);
	static std::map<string, GOFactory*>* factories;
	virtual Component* create(nap_json const & cfg, GameObject* owner) = 0;
};

#endif /* GO_FACTORY_H_ */
