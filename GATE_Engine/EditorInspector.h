#ifndef EDITORINSPECTOR_H
#define EDITORINSPECTOR_H

#include "EditorWindow.h"

class Component;
class ComponentTransform;
class ComponentMesh;
class ComponentMaterial;

class EditorInspector : public EditorWindow
{
public:
	EditorInspector(const char* name = "null", bool startEnabled = false, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	void Update();

public: //Methods
	//Since maybe we end up making executables from the engine, if all editor code remains in editor related classes, we will have a much more optimal and easy to process code on removing/disabling the editor related stuff
	//Pass a component to draw in editor 
	void DrawComponent(Component* c);

	void DrawComponentTransform(ComponentTransform* c);
	void DrawComponentMesh(ComponentMesh* c);
	void DrawComponentMaterial(ComponentMaterial* c);

private:
	bool startedEditing = false;
	char objNameBuffer[256];
};

#endif // !EditorInspector