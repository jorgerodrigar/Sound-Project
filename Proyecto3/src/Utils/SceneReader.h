#ifndef JSON_READER_H_
#define JSON_READER_H_

#include <string>
#include <list>
#include <map>
using namespace std;

#include "jsonCore.h"

//-----------------------------READING JSON INFO-----------------------------------

// GOType = list of Gameobjects configurations (name, pos, etc) and comps cfgs
struct GOStruct {
	nap_json go_cfg;
	nap_json components_cfg;
};

// SceneType = scene's name and GOType
struct SceneStruct {
	string SceneName;
	bool PhysRequired;
	std::list<GOStruct> gameObjects;
};

//---------------------------------------------------------------------------------

// this class reads json files (singleton)
class SceneReader
{
public:
	static SceneReader* getSingleton();   // you can obtain/shutdown the instance of the singleton
	static void shutdownSingleton();

	// reads the file called "level" and return the scene information
	SceneStruct ReadLevel(string level, GOStruct * & player, GOStruct * & manager);

	// returns a copy of the prefab
	GOStruct getPrefab(string pref, bool & success);

private:
	static SceneReader* instance_; //singleton pattern
	SceneReader();
	~SceneReader();

	// levels files must be in "routeLevel" and prefabs in "routePrefabs"
	string routeLevel = ".\\Assets\\_Levels\\";
	string routePrefabs = ".\\Assets\\_Levels\\Prefabs";
	string routeMapsTypes = ".\\Assets\\_Levels\\__mapTypes.txt";

	// prefab preloading
	void preloadPrefabs();
	std::map<string, GOStruct*> prefabs;

	// reading and updating GO
	GOStruct* readGO(nap_json const & cfg);
	bool applyPrefab(nap_json const & cfg, GOStruct & go);
	void deepUpdateJson(nap_json & j, nap_json const & updater);
	void deepUpdateJson_rec(nap_json & j, nap_json const & updater, nap_json::json_pointer & ptr);

	// reading object (readGO + applyPrefab)
	GOStruct* ReadObject(nap_json const & player_cfg);

	// reading map
	void preloadMapTypes();
	std::map<char, string> mapTypes;
	std::list<GOStruct> ReadMap(string level);
	void setTilePosition(int r, int c, int i, int j, GOStruct & go);
};

#endif /* JSON_READER_H_ */

//not updated**
// Here is an example of reading a level (.json) and creating gameobjects from it.
// This must be in GameState in the future (the place where we will read levels and create the stuff)
	/*
	JsonReader* jsReader = JsonReader::getSingleton();
	Scene_Type* scene = jsReader->ReadLevel("Example00");

	ComponentFactory* factory = ComponentFactory::getSingleton();

	std::vector<GameObject*> gameObjects;
	for (int i = 0; i < scene->gameObjects.size(); i++) {
		gameObjects.push_back(new GameObject()); //IMPORTANTE: cuando cada entidad tenga parametros en la constructora se pondran aqui usando scene.gameObjects[i].GOParams
		for (int j = 0; j < scene->gameObjects[i].components.size(); j++) {
			gameObjects[i]->addComponent(factory->ParseComponent(scene->gameObjects[i].components[j].compName, scene->gameObjects[i].components[j].compParameters));
		}
	}

	for (int i = 0; i < scene->gameObjects.size(); i++) {
		gameObjects[i]->update(0);
		delete gameObjects[i];
	}

	scene = nullptr;

	ComponentFactory::shutdownSingleton();
	JsonReader::shutdownSingleton();
	*/