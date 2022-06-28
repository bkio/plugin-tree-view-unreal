/// MIT License, Copyright Burak Kara, burak@burak.io, https://en.wikipedia.org/wiki/MIT_License

#pragma once

#include "Layout/Margin.h"

typedef TSharedPtr< class BCustomTreeNode > TreeNodePtr;

/**
* The Data for a single node in the Directory Tree
*/
class BCustomTreeNode
{

private:

	/** Parent item or NULL if this is a root  */
	TWeakPtr< BCustomTreeNode > ParentDir;

	/** Full path of this directory in the tree */
	FString DirectoryPath;

	/** Display name of the category */
	FString DisplayName;

	int32 ParentID;

	int32 NodeID;

	FMargin TreeNodePadding;

	TArray<FString> ExtraStrings;
	/** Child categories */
	TArray< TreeNodePtr > SubDirectories;

public:

	/** @return Returns the parent or NULL if this is a root */
	const TreeNodePtr GetParentCategory()
	{
		return ParentDir.Pin();
	}

	/** @return the path on hard disk, read-only */
	const FString& GetDirectoryPath()
	{
		return DirectoryPath;
	}

	/** @return name to display in file tree view! read-only */
	const FString& GetDisplayName()
	{
		return DisplayName;
	}

	TArray<FString> GetExtraStrings()
	{
		return ExtraStrings;
	}

	const int32& GetParentID()
	{
		return ParentID;
	}

	const int32& GetNodeID()
	{
		return NodeID;
	}

	const FMargin& GetTreeNodePadding() 
	{
		return TreeNodePadding;
	}

	/** @return Returns all subdirectories, read-only */
	const TArray< TreeNodePtr >& GetSubDirectories() 
	{
		return SubDirectories;
	}

	/** @return Returns all subdirectories, read or write */
	TArray< TreeNodePtr >& AccessSubDirectories()
	{
		return SubDirectories;
	}

	/** Add a subdirectory to this node in the tree! */
	void AddSubDirectory(TreeNodePtr NewSubDir)
	{
		SubDirectories.Add(NewSubDir);
	}

public:

	/** Constructor for BCustomTreeNode */
	BCustomTreeNode(TreeNodePtr IN_ParentDir, FString IN_DirectoryPath, FString IN_DisplayName , int32 IN_NodeID, int32 IN_ParentID , FMargin IN_TreeNodePadding , TArray<FString> IN_ExtraStrings)
	{
		ParentDir = IN_ParentDir;
		DirectoryPath = IN_DirectoryPath;
		DisplayName = IN_DisplayName;
		ParentID = IN_ParentID;
		NodeID = IN_NodeID;
		TreeNodePadding = IN_TreeNodePadding;
		ExtraStrings = IN_ExtraStrings;
	}


};