#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Globals.h"
#include "JsonLoader.h"
#include "Resource.h"
#include <string>

class Resource 
{
public:
	enum Type
	{
		UNKNOWN = -1,
		MESH = 0,
		TEXTURE,
		SCENE,
		MODEL,
		SCRIPT,
	};

	Resource(uint32 uid, Resource::Type type); 
	virtual ~Resource(); 

public:
	Resource::Type GetType() const; 
	uint32 GetUID() const; 
	const char* GetFile() const; 
	const char* GetImportedFile() const; 
	const char* GetName() const;
	bool IsLoadedToMemory() const; 
	bool LoadToMemory(); 
	uint CountReferences() const;
	void AddReference() { references++; };
	void RemoveReference() { references--; };

	//Virtual methods
	virtual void Save(json& config) const; 
	virtual void Load(const json& config); 
	virtual bool LoadInMemory() = 0;

protected: 
	uint32 UID = 0; 
	std::string name;
	std::string file; 
	std::string imported_file;
	Type type = UNKNOWN;
	uint loaded = 0;
	uint references = 0;
};
#endif
