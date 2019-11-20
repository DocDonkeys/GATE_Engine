#include "Globals.h"
#include "Tree.h"
#include "GameObject.h"
#include "ComponentCamera.h"

#include "libs/SDL/include/SDL_assert.h"
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
}

Tree::~Tree()
{
	Clear();

	if (rootNode != nullptr) {
		delete rootNode;
	}
}

void Tree::Draw()
{
	rootNode->Draw();
}

void Tree::Create(const AABB& limits)
{
	Clear();
	rootNode->aabb = limits;
}

void Tree::Reset()
{
	rootNode->Clear();

	for (int i = 0; i < treeObjects.size(); i++)
		rootNode->Insert(treeObjects[i]);

	LOG("[Info]: Reseted Tree.")
}

void Tree::Clear()
{
	rootNode->Clear();
	treeObjects.clear();

	LOG("[Info]: Cleared Tree.")
}

bool Tree::Grow(const AABB& absorb)
{
	bool ret = false;

	if (!rootNode->aabb.Contains(absorb)) {

		if (rootNode->aabb.MaxX() < absorb.MaxX())
			rootNode->aabb.maxPoint.x = absorb.MaxX();
		if (rootNode->aabb.MaxY() < absorb.MaxY())
			rootNode->aabb.maxPoint.y = absorb.MaxY();
		if (rootNode->aabb.MaxZ() < absorb.MaxZ())
			rootNode->aabb.maxPoint.z = absorb.MaxZ();

		if (rootNode->aabb.MinX() > absorb.MinX())
			rootNode->aabb.minPoint.x = absorb.MinX();
		if (rootNode->aabb.MinY() > absorb.MinY())
			rootNode->aabb.minPoint.y = absorb.MinY();
		if (rootNode->aabb.MinZ() > absorb.MinZ())
			rootNode->aabb.minPoint.z = absorb.MinZ();

		Reset();

		LOG("[Info]: Tree grown to Min %f/%f/%f and Max %f/%f/%f.",
			rootNode->aabb.MinX(), rootNode->aabb.MinY(), rootNode->aabb.MinZ(),
			rootNode->aabb.MaxX(), rootNode->aabb.MaxY(), rootNode->aabb.MaxZ());

		ret = true;
	}

	return ret;
}

bool Tree::Shrink()
{
	bool ret = false;

	AABB orig = rootNode->aabb;
	for (int i = 0; i < treeObjects.size(); i++) {
		if (rootNode->aabb.MaxX() > treeObjects[i]->aabb.MaxX())
			rootNode->aabb.maxPoint.x = treeObjects[i]->aabb.MaxX();
		if (rootNode->aabb.MaxY() > treeObjects[i]->aabb.MaxY())
			rootNode->aabb.maxPoint.y = treeObjects[i]->aabb.MaxY();
		if (rootNode->aabb.MaxZ() > treeObjects[i]->aabb.MaxZ())
			rootNode->aabb.maxPoint.z = treeObjects[i]->aabb.MaxZ();

		if (rootNode->aabb.MinX() < treeObjects[i]->aabb.MinX())
			rootNode->aabb.minPoint.x = treeObjects[i]->aabb.MinX();
		if (rootNode->aabb.MinY() < treeObjects[i]->aabb.MinY())
			rootNode->aabb.minPoint.y = treeObjects[i]->aabb.MinY();
		if (rootNode->aabb.MinZ() < treeObjects[i]->aabb.MinZ())
			rootNode->aabb.minPoint.z = treeObjects[i]->aabb.MinZ();
	}

	if (orig.MaxX() != rootNode->aabb.MaxX()
		|| orig.MaxY() != rootNode->aabb.MaxY()
		|| orig.MaxZ() != rootNode->aabb.MaxZ()
		|| orig.MinX() != rootNode->aabb.MinX()
		|| orig.MinY() != rootNode->aabb.MinY()
		|| orig.MinZ() != rootNode->aabb.MinZ())
	{
		LOG("[Info]: Tree shrunk to Min %f/%f/%f and Max %f/%f/%f.",
			rootNode->aabb.MinX(), rootNode->aabb.MinY(), rootNode->aabb.MinZ(),
			rootNode->aabb.MaxX(), rootNode->aabb.MaxY(), rootNode->aabb.MaxZ());

		Reset();
		ret = true;
	}

	return ret;
}

bool Tree::Insert(const GameObject* obj)
{
	bool ret = false;

	for (int i = 0; i < treeObjects.size(); i++)
		if (treeObjects[i] == obj) {
			ret = true;	// Object is already inserted
			//LOG("[Error]: Inserted object already present in the tree.")
			break;
		}

	if (!ret) {
		if (!rootNode->Insert(obj)) {	// If object outside of bounds
			Grow(obj->aabb);
			SDL_assert(rootNode->Insert(obj));
		}

		treeObjects.push_back(obj);
		LOG("[Success]: Inserted object %s to tree.", obj->name.c_str())
	}

	return ret;
}

bool Tree::Remove(const GameObject* obj)
{
	bool ret = false;

	for (int i = 0; i < treeObjects.size(); i++)
		if (treeObjects[i] == obj) {
			ret = true;	// Object exists in the tree
			treeObjects.erase(treeObjects.begin() + i);
			break;
		}

	if (ret) {
		ret = rootNode->Remove(obj);
		SDL_assert(ret);	// If object is listed it should be somewhere inside the tree, otherwise we fucked up bad
		LOG("[Success]: Object %s removed from the tree succesfully.", obj->name.c_str());
	}
	else {
		//LOG("[Error]: Object to Remove not found in the tree.")
	}

	return ret;
}

void Tree::Intersects(std::vector<const GameObject*>& collector, const AABB& area)
{
	rootNode->Intersects(collector, area);
}

void Tree::Intersects(std::vector<const GameObject*>& collector, const Frustum& frustum)
{
	rootNode->Intersects(collector, frustum);
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

	glColor3f(1, 1, 1);
	glEnd();

	// Draw Branches/Leafs
	if (numBranches > 0)
		for (int i = 0; i < numBranches; ++i)
			branches[i].Draw();
}

void Tree::TreeNode::Split()
{
	if (nodeType != NODE_TYPE::ROOT)
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

void Tree::TreeNode::Prune()	// WARNING: This is only called in the context that a branch is left with 4 leafs which have no objects
{
	SDL_assert(numBranches > 0);
		
	delete[] branches;
	branches = nullptr;
	numBranches = 0;

	nodeType = NODE_TYPE::LEAF;
}

void Tree::TreeNode::Prune(std::vector<const GameObject*> leafObjs)	// WARNING: This is only called in the context that a branch is left with 4 leafs which together don't overflow the parent's node limit
{
	SDL_assert(numBranches > 0);

	delete[] branches;
	branches = nullptr;
	numBranches = 0;

	nodeType = NODE_TYPE::LEAF;
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

	if (nodeObjects.size() > 0)
		nodeObjects.clear();
}

void Tree::TreeNode::Intersects(std::vector<const GameObject*>& collector, const AABB& area)
{
	if (aabb.Intersects(area)) {
		for (int i = 0; i < nodeObjects.size(); i++)
			collector.push_back(nodeObjects[i]);

		if (numBranches > 0)
			for (int i = 0; i < numBranches; i++)
				branches[i].Intersects(collector, area);
	}
}

void Tree::TreeNode::Intersects(std::vector<const GameObject*>& collector, const Frustum& frustum)
{
	if (ComponentCamera::Intersects(frustum, aabb)) {
		for (int i = 0; i < nodeObjects.size(); i++)
			collector.push_back(nodeObjects[i]);

		if (numBranches > 0)
			for (int i = 0; i < numBranches; i++)
				branches[i].Intersects(collector, frustum);
	}
}

bool Tree::TreeNode::Insert(const GameObject* obj)
{
	bool ret = false;

	if (aabb.Contains(obj->aabb)) {	// If obj AABB inside node's AABB
		if (numBranches > 0) {	// If I'm a branch
			for (int i = 0; i < numBranches; i++) {
				if (branches[i].Insert(obj)) {	// If inserted succesfully we can leave
					ret = true;
					break;
				}
			}

			if (!ret) {	// If none of the leafs fully contain the obj, the parent branch will
				nodeObjects.push_back(obj);
				ret = true;
			}
		}
		else {	// If I'm a leaf
			nodeObjects.push_back(obj);
			ret = true;

			if (nodeObjects.size() > tree->nodeSizeLimit) {	// If node's size surpassed limit, subdivide itself and spread it's objects to the new leafs
				Split();

				std::vector<const GameObject*> remainers;	// We will collect all objects that aren't assigned on children because they don't fully fit in any
				bool assigned = false;

				for (int i = 0; i < nodeObjects.size(); i++) {
					for (int j = 0; j < numBranches; j++) {
						if (branches[j].Insert(nodeObjects[i])) {	// If an object is succesfully inserted in a leaf
							assigned = true;
							break;
						}
					}

					if (!assigned)	// If no leaf could take this object, collect it
						remainers.push_back(nodeObjects[i]);
					else
						assigned = false;
				}

				if (remainers.size() == nodeObjects.size())	// If no objects have been assigned on children nodes, prune them
					Prune();
				else	// Else, copy remaining objects to node branch so that assigned ones are erased
					nodeObjects = remainers;
			}
		}
	}

	return ret;
}

bool Tree::TreeNode::Remove(const GameObject* obj)
{
	bool ret = false;

	if (aabb.Contains(obj->aabb)) {	// If obj is contained inside this node
		if (numBranches > 0) {	// If I'm a branch
			for (int i = 0; i < numBranches; i++) {
				if (branches[i].Remove(obj)) {	// If obj has been removed from one of the children nodes
					ret = true;

					std::vector<const GameObject*> leafCollector;
					for (i = 0; i < numBranches; i++) {
						if (branches[i].numBranches > 0) {	// If a children isn't leaf, break immediately
							break;
						}
						else {								// Else, if the leaf has objects at all add them to the collector
							int leafObjSize = branches[i].nodeObjects.size();
							if (leafObjSize > 0) {
								for (int j = 0; j < leafObjSize; j++)
									leafCollector.push_back(branches[i].nodeObjects[j]);

								if (nodeObjects.size() + leafCollector.size() > tree->nodeSizeLimit)	// If parent node's objects + leaf objects overflows limit, break immediately
									break;
							}
						}
					}

					if (i == numBranches) {	// If previous procedure was completed without breaks (no branches, no overflow of object limit)
						if (!leafCollector.empty())	// If collector isn't empty
							for (int k = 0; k < leafCollector.size(); k++)	// Add collector objects to parent node's list
								nodeObjects.push_back(leafCollector[k]);

						Prune();	// Eliminate children (which are either fully empty or their objects have been moved to the parent node)
					}

					break;
				}
			}

			if (!ret) {	// If obj was not found in any of the children nodes, yet I contain it, it means I, the parent branch, hold it and must remove it myself
				for (int i = 0; i < nodeObjects.size(); i++) {
					if (nodeObjects[i] == obj) {
						nodeObjects.erase(nodeObjects.begin() + i);
						ret = true;
						break;
					}
				}

				SDL_assert(ret);
			}
		}
		else {	// If I'm a leaf
			for (int i = 0; i < nodeObjects.size(); i++) {
				if (nodeObjects[i] == obj) {
					nodeObjects.erase(nodeObjects.begin() + i);
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