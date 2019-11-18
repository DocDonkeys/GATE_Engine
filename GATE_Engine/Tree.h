#ifndef TREE_H
#define TREE_H

#include <vector>
#include "libs/MathGeoLib/include/Geometry/AABB.h"

class GameObject;

// Space Partitioning
class Tree {
public:	// Nested classes
	// Tree Type
	enum class TREE_TYPE
	{
		NONE = -1,

		QUAD_TREE,
		OC_TREE,
		KD_TREE,

		MAX_TYPES
	};

	// Tree Node START
	class TreeNode {
	public:
		enum class NODE_TYPE
		{
			NONE = -1,

			ROOT,
			BRANCH,
			LEAF,

			MAX_TYPES
		};

	public:		// Constructors
		TreeNode() {}
		TreeNode(AABB aabb, TREE_TYPE tree, NODE_TYPE node) : aabb(aabb), treeType(tree), nodeType(node) {}

		void Split();
		void Draw();

	private:	// Methods
		void QuadSplit();
		void OctSplit();

	public:		// Members
		AABB aabb;

		NODE_TYPE nodeType;
		TREE_TYPE treeType;

		TreeNode* nodes;
		uint numNodes = 0;
		std::vector<GameObject*> objects;
	};
	// Tree Node END

public:	// Constructors
	Tree(TREE_TYPE type, AABB aabb);

public: // Methods
	void Draw();

	void Create(AABB limits);
	void Clear();

	void Insert(GameObject*);
	void Remove(GameObject*);

	//void Intersect(std::vector<GameObject*>&, PRIMITIVE);

public:	// Members
	TREE_TYPE type = TREE_TYPE::NONE;
	TreeNode* rootNode;
};

#endif	//TREE_H	