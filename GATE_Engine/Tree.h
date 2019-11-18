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
		TreeNode(AABB aabb, Tree* tree, NODE_TYPE node) : aabb(aabb), tree(tree), nodeType(node) {}

	public:
		void Draw();

		void Split();
		void Clear();

		bool Insert(GameObject* obj);
		bool Remove(GameObject* obj);

	private:	// Methods
		void QuadSplit();
		void OctSplit();

	public:		// Members
		AABB aabb;
		NODE_TYPE nodeType;

		Tree* tree;
		TreeNode* branches;
		uint numBranches = 0;
		std::vector<GameObject*> objects;
	};
	// Tree Node END

public:	// Constructors
	Tree(TREE_TYPE type, AABB aabb, uint nodeSizeLimit);

public: // Methods
	void Draw();

	void Create(AABB limits);
	void Clear();

	bool Insert(GameObject* obj);
	bool Remove(GameObject* obj);

	void Intersect(std::vector<GameObject*>&, AABB area);

public:	// Members
	TREE_TYPE type = TREE_TYPE::NONE;
	TreeNode* rootNode;
	uint nodeSizeLimit;
};

#endif	//TREE_H	