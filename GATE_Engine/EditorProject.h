#ifndef EDITORPROJECT_H
#define EDITORPROJECT_H

#include "EditorWindow.h"

struct AbstractDir;
class EditorProject : public EditorWindow
{
public:
	EditorProject(const char* name = "null", bool startEnabled = false, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	void Update();
	void DrawAssetsLayout();

	void PrintAssetsHierarchy(AbstractDir* abs_dir, int& treenode_id);

private:
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool selected_dir_already = false;
	float initial_width = 200.0f;
};
#endif // !EDITORPROJECT_H