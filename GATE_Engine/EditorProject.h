#ifndef EDITORPROJECT_H
#define EDITORPROJECT_H

#include "EditorWindow.h";

struct AbstractDir;
class ResourceTexture;
class EditorProject : public EditorWindow
{
public:
	EditorProject(const char* name = "null", bool startEnabled = false, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	
	bool Start();
	void Update();
	void DrawAssetsLayout(AbstractDir* selected_dir);

	void PrintAssetsHierarchy(AbstractDir* abs_dir, int& treenode_id);

public:
	int dragged_file = -1;
	bool finished_dragndrop = false;
	bool was_dragndroping = false;
	bool dragndroping = false;

private:
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool selected_dir_already = false;
	float initial_width = 200.0f;

	ResourceTexture* folder_tex = nullptr;
	ResourceTexture* object_tex = nullptr;
};
#endif // !EDITORPROJECT_H