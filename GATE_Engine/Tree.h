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
		TreeNode(AABB aabb, const Tree* tree, NODE_TYPE node) : aabb(aabb), tree(tree), nodeType(node) {}

	public:
		void Draw();

		bool Insert(const GameObject* obj);
		bool Remove(const GameObject* obj);
		void Clear();

		void Split();

	private:
		void QuadSplit();
		void OctSplit();

	public:		// Members
		AABB aabb;
		NODE_TYPE nodeType;

		const Tree* tree;
		TreeNode* branches;
		uint numBranches = 0;
		std::vector<const GameObject*> nodeObjects;
	};
	// Tree Node END

public:	// Constructors
	Tree(TREE_TYPE type, AABB aabb, uint nodeSizeLimit);
	~Tree();

public: // Methods
	void Draw();

	void Create(AABB limits);
	void Clear();
	void Reset();

	bool Insert(const GameObject* obj);
	bool Remove(const GameObject* obj);
	bool Grow(AABB newSize);

	void Intersect(std::vector<const GameObject*>&, AABB area);

public:	// Members
	TREE_TYPE type = TREE_TYPE::NONE;
	TreeNode* rootNode;
	uint nodeSizeLimit;
	std::vector<const GameObject*> treeObjects;
};

#endif	//TREE_H	