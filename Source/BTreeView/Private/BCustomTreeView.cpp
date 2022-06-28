/// MIT License, Copyright Burak Kara, burak@burak.io, https://en.wikipedia.org/wiki/MIT_License

#include "BCustomTreeView.h"

#define LOCTEXT_NAMESPACE "UMG"

UBCustomTreeView::UBCustomTreeView()
{
	bIsVariable = false;
	ExpanderVisibility = true;
	RowDefaultPadding = FMargin(4);
}

#if WITH_EDITOR
const FText UBCustomTreeView::GetPaletteCategory()
{
	return LOCTEXT("Views", "Views");
}
#endif

void UBCustomTreeView::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	TreeViewWidget.Reset();
}

TSharedRef<SWidget> UBCustomTreeView::RebuildWidget()
 {
	 TreeViewWidget = SNew(SBCustomTreeView).TWidget(this).TStyle(&TreeViewStyle).ExpandedArrowStyle(&ArrowStyle).ExpanderVisibility(ExpanderVisibility);
	 CreateTree();
	 return TreeViewWidget.ToSharedRef();
 }

void UBCustomTreeView::EnsureWidgetValidity()
{
	if (!TreeViewWidget.IsValid())
	{
		RebuildWidget();
	}
	ensure(TreeViewWidget.IsValid());
}

int32 UBCustomTreeView::GetRootIndex(int32 NodeIndex)
{
	if (TreeNodes[NodeIndex].ParentID == 0)
	{
		return NodeIndex;
	}
	return GetRootIndex(TreeNodes[NodeIndex].ParentID-1);
}
void UBCustomTreeView::ExpandTreeItem(int32 NodeId)
{
	EnsureWidgetValidity();
	if (TempStructure.Num() > NodeId)
	{
		TreeViewWidget->ExpandTreeItem(TempStructure[NodeId]);
	}
}

void UBCustomTreeView::SelectTreeItem(int32 NodeId)
{
	EnsureWidgetValidity();
	if (TempStructure.Num() > NodeId)
	{
		TreeViewWidget->SetOnMouseButtonDown(FPointerEventHandler());
		TreeViewWidget->SelectDirectory(TempStructure[NodeId]);
	}
}

void UBCustomTreeView::CollapseTreeItem(int32 NodeId)
{
	EnsureWidgetValidity();
	if (TempStructure.Num() > NodeId)
	{
		TreeViewWidget->CollapseTreeItem(TempStructure[NodeId]);
	}
}

void UBCustomTreeView::ToggleNodeExpansion(int32 NodeId)
{
	EnsureWidgetValidity();
	if (TempStructure.Num() > NodeId)
	{
		TreeViewWidget->ToggleNodeExpansion(TempStructure[NodeId]);
	}
}

void UBCustomTreeView::CreateTree()
{
	TreeStructure.Empty();
	TempStructure.Empty();
	for (int i = 0; i < TreeNodes.Num(); i++)
	{
		TreeNodes[i].NodeID = i;
		if (TreeNodes[i].ParentID == 0)
		{
			TSharedRef<BCustomTreeNode> RootDir = MakeShareable(new BCustomTreeNode(NULL, TreeNodes[i].NodeName, TreeNodes[i].NodeName , TreeNodes[i].NodeID , 0 , TreeNodes[i].NodePadding , TreeNodes[i].ExtraStrings));
			TreeStructure.Add(RootDir);
			TempStructure.Add(RootDir);
		}
		else
		{
			if (TempStructure.Num() >= TreeNodes[i].ParentID)
			{
				TreeNodePtr Parent = TempStructure[TreeNodes[i].ParentID - 1];
				
				TreeNodePtr Child = MakeShareable(new BCustomTreeNode(Parent, TreeNodes[i].NodeName, TreeNodes[i].NodeName, TreeNodes[i].NodeID , TreeNodes[i].ParentID , TreeNodes[i].NodePadding, TreeNodes[i].ExtraStrings));
				Parent->AddSubDirectory(Child);
				TempStructure.Add(Child);
			}
		}
	}

	EnsureWidgetValidity();
	TreeViewWidget->RefreshTree(TreeStructure);
}

void UBCustomTreeView::HandleOnGenerateRow(TreeNodePtr Item, class UUserWidget* RowWidget, TArray<TreeNodePtr> Children)
 {
	 if (Item.IsValid())
	 {
		 FBTreeNode Node;
		 Node.NodeID = Item->GetNodeID();
		 Node.NodeName = Item->GetDisplayName();
		 Node.ParentID = Item->GetParentID();
		 Node.ExtraStrings = Item->GetExtraStrings();
		 TArray<FBTreeNode> NodeChildren;

		 for (int i = 0; i < Children.Num(); i++)
		 {
			 FBTreeNode Childnode;
			 Childnode.NodeID = Item->GetNodeID();
			 Childnode.NodeName = Children[i]->GetDisplayName();
			 Childnode.ParentID = Children[i]->GetParentID();
			 Childnode.ExtraStrings = Children[i]->GetExtraStrings();
			 NodeChildren.Add(Childnode);
		 }

		 OnGenerateRow.Broadcast(Node, RowWidget, NodeChildren);
	 }
 }

void UBCustomTreeView::HandleOnSelectionChanged(TreeNodePtr Item, class UUserWidget* RowWidget)
{
	if (Item.IsValid())
	{
		FBTreeNode node;
		node.NodeID = Item->GetNodeID();
		node.NodeName = Item->GetDisplayName();
		node.ParentID = Item->GetParentID();
		node.ExtraStrings = Item->GetExtraStrings();
		OnSelectionChanged.Broadcast(node , RowWidget);
	}
}

void UBCustomTreeView::HandleOnSelectionLost()
{
	OnSelectionLost.Broadcast();
}

void UBCustomTreeView::HandleOnExpansionChanged(TreeNodePtr Item, class UUserWidget* RowWidget, bool ExpansionState)
{
	if (Item.IsValid())
	{
		FBTreeNode node;
		node.NodeID = Item->GetNodeID();
		node.NodeName = Item->GetDisplayName();
		node.ParentID = Item->GetParentID();
		node.ExtraStrings = Item->GetExtraStrings();
		OnExpansionChanged.Broadcast(node, RowWidget , ExpansionState);
	}
}

#undef LOCTEXT_NAMESPACE