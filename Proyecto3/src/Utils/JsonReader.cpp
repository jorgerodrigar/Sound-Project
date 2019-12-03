#include "SceneReader.h"

#include "LogSystem.h"
#include "FileSystemUtils.h"

#include <fstream>

#define undefLevelName "undefinedName"

SceneReader* SceneReader::instance_ = nullptr;
SceneReader* SceneReader::getSingleton() {
	if (instance_ == nullptr) {
		instance_ = new SceneReader();
	}

	return instance_;
}

void SceneReader::shutdownSingleton() {
	if (instance_ == nullptr) return;

	delete instance_;
	instance_ = nullptr;
}

SceneReader::SceneReader() { preloadPrefabs(); preloadMapTypes(); }
SceneReader::~SceneReader() {
	//delete the GOStructs
	for (auto itr = prefabs.begin(); itr != prefabs.end(); itr++) {
		delete (itr->second);
	}
	prefabs.clear();
	mapTypes.clear();
}

///////////////////////////////////////////////////////////////////////////////

//preload all the prefabs in folder "routePrefabs"
void SceneReader::preloadPrefabs() {
	LogSystem::Log("Precargando prefabs...", LogSystem::JSON);
	size_t n = 0;

	//get all names using FileSystemUtils
	auto entries = FileSystemUtils::retrieveDirEntries(routePrefabs);

	for (auto & ent : entries) {
		//open the file
		ifstream file(ent);
		if (!file.is_open()) {
			LogSystem::Log("El archivo " + ent + " no se pudo abrir... ignorando", LogSystem::JSON);
			continue;
		}

		//parse the json
		nap_json j;
		file >> j;
		n++;

		//json can have just one prefab or a list
		if (FIND(j, "Prefabs")) { //list of prefabs
			for (size_t i = 0; i < j["Prefabs"].size(); i++) {
				//parse and add it to the map
				GOStruct* go = readGO(j["Prefabs"][i]);
				prefabs[go->go_cfg["id"]["type"]] = go;
			}
		}
		else { //just one prefab
			//parse and add it to the map
			GOStruct* go = readGO(j);
			prefabs[go->go_cfg["id"]["type"]] = go;
		}

		file.close();
	}

	LogSystem::Log("Precargados " + to_string(n) + " archivos de prefabs con exito", LogSystem::JSON);
}

GOStruct SceneReader::getPrefab(string pref, bool & success) {
	if (prefabs.count(pref) > 0) {
		success = true;
		return *prefabs[pref];
	}
	else {
		LogSystem::Log("El prefab " + pref + "no fue encontrado... abortando parseo del GO", LogSystem::JSON);
		success = false;
		return GOStruct();
	}
}

///////////////////////////////////////////////////////////////////////////////

GOStruct* SceneReader::readGO(nap_json const & cfg) {
	auto go = new GOStruct();
	go->go_cfg = cfg;

	//existance of comps
	if (FIND(cfg, "Components")) {
		go->go_cfg.erase("Components");
		go->components_cfg = cfg["Components"];
	}

	return go;
}

bool SceneReader::applyPrefab(nap_json const & cfg, GOStruct & go) {
	bool success = true;

	//check if the object has id
	if (FIND(cfg, "id")) {
		//check if the object uses a prefab as base
		if (FIND(cfg["id"], "type")) {

			//check the prefab exists and load / throw error and continue
			GOStruct prefab_go = getPrefab(cfg["id"]["type"], success);

			if (success) {
				//update all the prefab values with raw GO
				//using recursive deep update
				deepUpdateJson(prefab_go.go_cfg, go.go_cfg);
				deepUpdateJson(prefab_go.components_cfg, go.components_cfg);
				go = prefab_go;
			}
		}
	}

	return success;
}

//update all nested json objects
void SceneReader::deepUpdateJson(nap_json & j, nap_json const & updater) {
	//json.update(updater); //not recursive!!!!!!
	nap_json::json_pointer ptr;
	deepUpdateJson_rec(j, updater, ptr); //recursive part uses a json pointer
}
void SceneReader::deepUpdateJson_rec(nap_json & j, nap_json const & updater, nap_json::json_pointer & ptr) {
	//iterate objects layer by layer
	for (auto it = updater.begin(); it != updater.end(); ++it) {
		if (it->is_structured()) {
			ptr.push_back(it.key());			//keep track of the pointer
			deepUpdateJson_rec(j, *it, ptr);	//nested object
			ptr.pop_back();						//back from nested
		}
		else {
			ptr.push_back(it.key());	//add key to ptr
			j[ptr] = it.value();		//update the value of j
			ptr.pop_back();				//back ptr from the key
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

// returns a Scene_type with all the information read
// from "level.json" (GameObjects) and "level.txt" (map/tiles)
SceneStruct SceneReader::ReadLevel(string level, GOStruct * & player, GOStruct * & manager) {
	LogSystem::Log("Reading level " + level + " ...", LogSystem::JSON);
	size_t n = 0;

	SceneStruct scene;

	//read "level.json"
	ifstream file(routeLevel + level + ".json");
	if (!file.is_open()) {
		LogSystem::Log("El archivo " + routeLevel + level + ".json no fue encontrado... abortando parseo", LogSystem::JSON);
		return scene;
	}

	//parse it
	nap_json j;
	file >> j;

	//read the scene name
	string s = FINDnRETURN_s(j, "stateID_", undefLevelName);
	scene.SceneName = s;

	//read phys required
	bool b = FINDnRETURN(j, "physRequired", bool, false);
	scene.PhysRequired = b;

	//read the map/tiles and dump all the GOs IF FOUND
	if (FIND(j, "mapFile")) {
		string mapFile = j["mapFile"];
		scene.gameObjects = ReadMap(mapFile);
	}
	else LogSystem::Log("La escena " + scene.SceneName + " no contiene mapa...", LogSystem::JSON);

	//read the player
	if (FIND(j, "Player")) {
		player = ReadObject(j["Player"]);
		LogSystem::Log("Leido Player con exito", LogSystem::JSON);
	}
	else LogSystem::Log("La escena " + scene.SceneName + " no contiene player...", LogSystem::JSON);

	//read the manager of the scene
	if (FIND(j, "Manager")) {
		manager = ReadObject(j["Manager"]);
		LogSystem::Log("Leido Manager con exito", LogSystem::JSON);
	}
	else LogSystem::Log("La escena " + scene.SceneName + " no contiene manager...", LogSystem::JSON);

	//read the gameobjects
	if (!(FIND(j, "GameObjects")) || j["GameObjects"].size() == 0) {
		LogSystem::Log("El archivo " + routeLevel + level + ".json no tiene GOs...", LogSystem::JSON);
		return scene;
	}

	//one by one
	for (size_t i = 0; i < j["GameObjects"].size(); i++) {
		nap_json cfg = j["GameObjects"][i];

		//raw read the GO
		GOStruct go = *readGO(cfg);

		//apply or not a prefab (continue if error)
		if (!applyPrefab(cfg, go)) continue;

		scene.gameObjects.push_back(go); //add the go to the scene
		n++;
	}

	file.close();
	LogSystem::Log("Leidos " + to_string(n) + " Gameobjects con exito", LogSystem::JSON);
	return scene;
}

GOStruct* SceneReader::ReadObject(nap_json const & object_cfg) {
	//raw read the GO
	GOStruct * object = readGO(object_cfg);

	//apply or not a prefab (return if error)
	if (!applyPrefab(object_cfg, *object)) LogSystem::Log("Error al leer el objeto", LogSystem::JSON);
	return object;
}

///////////////////////////////////////////////////////////////////////////////

#define non "non"
void SceneReader::preloadMapTypes() {
	LogSystem::Log("Reading map types file" + routeMapsTypes + " ...", LogSystem::JSON);
	size_t n = 0;

	//check if openned correctly
	ifstream file(routeMapsTypes);
	if (!file.is_open()) {
		LogSystem::Log("El archivo " + routeMapsTypes + " no fue encontrado... abortando parseo", LogSystem::JSON);
		return;
	}

	//read the pairs untill the end
	char c; string s;
	while (file >> c >> s) {

		//check the prefab exists and save / throw error and continue
		if (s != non && prefabs.count(s) == 0) {
			LogSystem::Log("El prefab " + s + " no fue encontrado... abortando parseo del mapType", LogSystem::JSON);
			continue;
		}

		mapTypes[c] = s;
		n++;

	}

	file.close();
	LogSystem::Log("Leidos " + to_string(n) + " tipos con exito", LogSystem::JSON);
}

// it adds to the scene the GameObjects (tiles) that will form the floor
std::list<GOStruct> SceneReader::ReadMap(string level) {
	LogSystem::Log("Reading map " + level + " ...", LogSystem::JSON);
	size_t n = 0;

	std::list<GOStruct> map;

	//check if openned correctly
	ifstream file(routeLevel + level);
	if (!file.is_open()) {
		LogSystem::Log("El mapa " + routeLevel + level + " no fue encontrado... abortando parseo", LogSystem::JSON);
		return map;
	}

	//read rows and cols
	int r, c;
	file >> r >> c;

	//read tiles
	for (int i = 0; i < r; i++) {
		string row; file >> row;

		for (int j = 0; j < c; j++) {
			// the map is composed of chars
			// the chars are mapped to the prefab name on mapTypes
			// mapTypes is preloaded from file "routeMapsTypes" in preloadMapTypes()

			//check if mapType exists
			if (mapTypes.count(row[j]) == 0) {
				string s = " "; s[0] = row[j];
				LogSystem::Log("El mapType '" + s + "' no fue encontrado... abortando parseo de tile", LogSystem::JSON);
				continue;
			}

			string prefName = mapTypes[row[j]];
			if (prefName == non) continue; //some are ignored but added to the map to not proc errors messages

			GOStruct go = *prefabs[prefName];

			//temporal way of adding suffix to entity name. Ogre dosent allow same name. //FIXED
			//go.components_cfg["tile_rend"]["shape"]["entity_name"] = "tile_" + to_string(n);

			//configue its position and push it
			setTilePosition(r, c, i, j, go);
			map.push_back(go);
			n++;
		}
	}

	file.close();
	LogSystem::Log("Leidos " + to_string(n) + " Gameobjects (tiles del map) con exito", LogSystem::JSON);
	return map;
}

#include "Transforms.h"
// set the physical position of the tile parsing its logic position (i, j)
void SceneReader::setTilePosition(int r, int c, int i, int j, GOStruct & go) {
	//at some point moved to and external cfg file
	const float tile_w = 5.0f, tile_h = 5.0f, level_y = -5.0, GAP = 0.0;

	//calculate positions with offsets
	float x, y, z;
	x = -(c / 2.0f * tile_w) + j * tile_w + j * GAP;
	y = level_y;
	z = -(r / 2.0f * tile_h) + i * tile_h + i * GAP;

	go.go_cfg["pos"] = nap_vector3(x, y, z).json();
}