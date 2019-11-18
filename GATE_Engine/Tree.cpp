#include "Globals.h"
#include "Tree.h"
#include "GameObject.h"
#include "libs/SDL/include/SDL_opengl.h"

#ifdef _DEBUG
#ifdef _MMGR_MEM_LEAK
#include "libs/mmgr/mmgr.h"
#endif
#endif

// Tree
Tree::Tree(TREE_TYPE type, AABB aabb, uint nodeSizeLimit) : type(type), nodeSizeLimit(nodeSizeLimit)
{
	rootNode = new TreeNode(aabb, this, TreeNode::NODE_TYPE::ROOT);

	rootNode->Split();								// The purpose of a tree is to split, so the first one is done here so that
	rootNode->nodeType = TreeNode::NODE_TYPE::ROOT;	// we can now reassign the original node as ROOT, avoiding unnecesary first-time checks on the split method
}

void Tree::Draw()
{
	rootNode->Draw();
}

void Tree::Create(AABB limits)
{
	Clear();

	rootNode->aabb = limits;

	rootNode->Split();
	rootNode->nodeType = TreeNode::NODE_TYPE::ROOT;
}

void Tree::Clear()
{
	rootNode->Clear();
}

bool Tree::Insert(GameObject* obj)
{
	return rootNode->Insert(obj);
}

bool Tree::Remove(GameObject* obj)
{
	return rootNode->Remove(obj);
}

// -----------------------------------------------------------------

// TreeNode
void Tree::TreeNode::Draw()
{
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glColor3f(0, 1, 0);

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	glEnd();

	// Draw Branches/Leafs
	if (nodeType != NODE_TYPE::LEAF)
		for (int i = 0; i < numBranches; ++i)
			branches[i].Draw();
}

void Tree::TreeNode::Split()
{
	nodeType = NODE_TYPE::BRANCH;

	switch (tree->type)
	{
	case TREE_TYPE::QUAD_TREE:
		QuadSplit();
		break;
	case TREE_TYPE::OC_TREE:
		OctSplit();
		break;
	}
}

void Tree::TreeNode::Clear()
{
	if (numBranches > 0) {
		for (int i = 0; i < numBranches; i++)
			branches[i].Clear();

		delete[] branches;
		branches = nullptr;
		numBranches = 0;
	}

	if (objects.size() > 0)
		objects.clear();
}

bool Tree::TreeNode::Insert(GameObject* obj)
{
	bool ret = false;

	if (aabb.Contains(obj->aabb.CenterPoint())) {
		if (numBranches > 0) {
			for (int i = 0; i < numBranches; i++) {
				if (branches[i].Insert(obj)) {
					ret = true;
					break;
				}
			}
		}
		else {
			objects.push_back(obj);

			if (objects.size() > tree->nodeSizeLimit) {
				Split();

				for (int i = 0; i < objects.size(); i++) {
					for (int j = 0; j < numBranches; j++) {
						if (branches[j].Insert(objects[i]))
							break;
					}
				}

				objects.clear();	// Saving objects only in leafs is better for RAM space, but having nodes know which objects are saved on their branches could be useful
			}
		}
	}

	return ret;
}

bool Tree::TreeNode::Remove(GameObject* obj)
{
	bool ret = false;

	if (aabb.Contains(obj->aabb.CenterPoint())) {
		if (numBranches > 0) {
			for (int i = 0; i < numBranches; i++) {
				if (branches[i].Remove(obj)) {
					ret = true;
					break;
				}
			}
		}
		else {
			for (int i = 0; i < objects.size(); i++) {
				if (objects[i] == obj) {
					objects.erase(objects.begin() + i);
					ret = true;
					break;
				}
			}
		}
	}
	
	return ret;
}

void Tree::TreeNode::QuadSplit()
{
	//Subdivide the AABB     x)
	AABB newAABBs[4];
	float3 maxPoint, minPoint;
	/*
	{maxx, maxy maxz)
		 ___________
		|	  |		|
		|_____|_____|
		|	  |		|
		|_____|_____|
					{minx, maxy minz)

	*/
	//NORTH-WEST subnode
	maxPoint = { aabb.MaxX(), aabb.MaxY(), aabb.MaxZ() };
	minPoint = { (aabb.MaxX() + aabb.MinX()) / 2 , aabb.MinY(), (aabb.MaxZ() + aabb.MinZ()) / 2 };
	newAABBs[0].minPoint = minPoint;
	newAABBs[0].maxPoint = maxPoint;

	//NORTH-EAST subnode
	maxPoint = { (aabb.MaxX() + aabb.MinX()) / 2, aabb.MaxY(), aabb.MaxZ() };
	minPoint = { aabb.MinX(), aabb.MinY(), (aabb.MaxZ() + aabb.MinZ()) / 2 };
	newAABBs[1].minPoint = minPoint;
	newAABBs[1].maxPoint = maxPoint;

	//SOUTH-WEST subnode
	maxPoint = { aabb.MaxX(), aabb.MaxY(),(aabb.MaxZ() + aabb.MinZ()) / 2 };
	minPoint = { (aabb.MaxX() + aabb.MinX()) / 2, aabb.MinY(), aabb.MinZ() };
	newAABBs[2].minPoint = minPoint;
	newAABBs[2].maxPoint = maxPoint;

	//SOUTH-EAST subnode
	maxPoint = { (aabb.MaxX() + aabb.MinX()) / 2, aabb.MaxY(),(aabb.MaxZ() + aabb.MinZ()) / 2 };
	minPoint = { aabb.MinX(), aabb.MinY(), aabb.MinZ() };
	newAABBs[3].minPoint = minPoint;
	newAABBs[3].maxPoint = maxPoint;

	branches = new TreeNode[4];
	numBranches = 4;

	for (int i = 0; i < 4; i++)
		branches[i] = TreeNode(newAABBs[i], tree, NODE_TYPE::LEAF);
}

void Tree::TreeNode::OctSplit()
{
	//Subdivide the AABB     x)
	AABB newAABBs[8];
	float3 maxPoint, minPoint;
	/*
	{maxx, maxy, maxz)
		 ___________
		|	  |		|
		|_____|_____|
		|	  |		|
		|_____|_____|
					{minx, maxy, minz)

	*/
	//NORTH-WEST-TOP subnode
	maxPoint = { aabb.MaxX(), aabb.MaxY(), aabb.MaxZ() };
	minPoint = { (aabb.MaxX() + aabb.MinX()) / 2 , (aabb.MinY() + aabb.MaxY()) / 2, (aabb.MaxZ() + aabb.MinZ()) / 2 };
	newAABBs[0].minPoint = minPoint;
	newAABBs[0].maxPoint = maxPoint;

	//NORTH-EAST-TOP subnode
	maxPoint = { (aabb.MaxX() + aabb.MinX()) / 2, aabb.MaxY(), aabb.MaxZ() };
	minPoint = { aabb.MinX(), (aabb.MinY() + aabb.MaxY()) / 2, (aabb.MaxZ() + aabb.MinZ()) / 2 };
	newAABBs[1].minPoint = minPoint;
	newAABBs[1].maxPoint = maxPoint;

	//SOUTH-WEST-TOP subnode
	maxPoint = { aabb.MaxX(), aabb.MaxY(),(aabb.MaxZ() + aabb.MinZ()) / 2 };
	minPoint = { (aabb.MaxX() + aabb.MinX()) / 2, (aabb.MinY() + aabb.MaxY()) / 2, aabb.MinZ() };
	newAABBs[2].minPoint = minPoint;
	newAABBs[2].maxPoint = maxPoint;

	//SOUTH-EAST-TOP subnode
	maxPoint = { (aabb.MaxX() + aabb.MinX()) / 2, aabb.MaxY(),(aabb.MaxZ() + aabb.MinZ()) / 2 };
	minPoint = { aabb.MinX(), (aabb.MinY() + aabb.MaxY()) / 2, aabb.MinZ() };
	newAABBs[3].minPoint = minPoint;
	newAABBs[3].maxPoint = maxPoint;

	//NORTH-WEST-BOTTOM subnode
	maxPoint = { aabb.MaxX(), (aabb.MinY() + aabb.MaxY()) / 2, aabb.MaxZ() };
	minPoint = { (aabb.MaxX() + aabb.MinX()) / 2 ,aabb.MinY(), (aabb.MaxZ() + aabb.MinZ()) / 2 };
	newAABBs[4].minPoint = minPoint;
	newAABBs[4].maxPoint = maxPoint;

	//NORTH-EAST-BOTTOM subnode
	maxPoint = { (aabb.MaxX() + aabb.MinX()) / 2,(aabb.MinY() + aabb.MaxY()) / 2, aabb.MaxZ() };
	minPoint = { aabb.MinX(),aabb.MinY(), (aabb.MaxZ() + aabb.MinZ()) / 2 };
	newAABBs[5].minPoint = minPoint;
	newAABBs[5].maxPoint = maxPoint;

	//SOUTH-WEST-BOTTOM subnode
	maxPoint = { aabb.MaxX(), (aabb.MinY() + aabb.MaxY()) / 2,(aabb.MaxZ() + aabb.MinZ()) / 2 };
	minPoint = { (aabb.MaxX() + aabb.MinX()) / 2, aabb.MinY(), aabb.MinZ() };
	newAABBs[6].minPoint = minPoint;
	newAABBs[6].maxPoint = maxPoint;

	//SOUTH-EAST-BOTTOM subnode
	maxPoint = { (aabb.MaxX() + aabb.MinX()) / 2,(aabb.MinY() + aabb.MaxY()) / 2,(aabb.MaxZ() + aabb.MinZ()) / 2 };
	minPoint = { aabb.MinX(), aabb.MinY(), aabb.MinZ() };
	newAABBs[7].minPoint = minPoint;
	newAABBs[7].maxPoint = maxPoint;

	branches = new TreeNode[8];
	numBranches = 8;

	for (int i = 0; i < 8; ++i)
		branches[i] = TreeNode(newAABBs[i], tree, NODE_TYPE::LEAF);

}