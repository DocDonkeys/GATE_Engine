#ifndef TREE_H
#define TREE_H

#include <map>
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
		N_TREE,
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

		void Intersects(std::vector<const GameObject*>& collector, const AABB& area);
		void Intersects(std::vector<const GameObject*>& collector, const Frustum& frustum);
		void Intersects(std::map<float, const GameObject*>& collector, const LineSegment& line, bool nearest = true);

		void Split();
		void Prune();

	private:
		void QuadSplit();
		void OctSplit();

	public:		// Members
		AABB aabb;
		NODE_TYPE nodeType;

		const Tree* tree = nullptr;
		TreeNode* branches = nullptr;
		uint numBranches = 0;
		std::vector<const GameObject*> nodeObjects;
	};
	// Tree Node END

public:	// Constructors
	Tree(TREE_TYPE type, AABB aabb, uint nodeSizeLimit);
	~Tree();

public: // Methods
	void Draw();

	void Create(const AABB& limits);
	void Reset();
	void Clear();

	bool Grow(const AABB& absorb);
	bool Shrink();

	bool Insert(const GameObject* obj);
	bool Remove(const GameObject* obj);

	//template<class Primitive>
	//void Intersects(std::vector<const GameObject*>& collector, const Primitive& area);	//IMPROVE: Use this instead of a method for every intersection type
	void Intersects(std::vector<const GameObject*>& collector, const AABB& area);
	void Intersects(std::vector<const GameObject*>& collector, const Frustum& frustum);
	void Intersects(std::map<float, const GameObject*>& collector, const LineSegment& line, bool nearest = true);

public:	// Members
	TREE_TYPE type = TREE_TYPE::NONE;
	TreeNode* rootNode;
	uint nodeSizeLimit;
	std::vector<const GameObject*> treeObjects;
};

#endif	//TREE_H	