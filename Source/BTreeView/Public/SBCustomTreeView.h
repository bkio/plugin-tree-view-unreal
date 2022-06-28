/// MIT License, Copyright Burak Kara, burak@burak.io, https://en.wikipedia.org/wiki/MIT_License

#pragma once

#include "BCustomTreeNode.h"
#include "SlateCore.h"
#include "Engine.h"
#include "BTreeViewStyles.h"

typedef STreeView<TreeNodePtr> STView;

struct FRow
{
	int32 NodeId;
	class UUserWidget* RowWidget;
};

class SBCustomTreeView : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SBCustomTreeView)
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<class UBCustomTreeView>, TWidget)
	SLATE_ARGUMENT(const struct FBTreeViewStyle*, TStyle)
	SLATE_ARGUMENT(bool , ExpanderVisibility)

	SLATE_ARGUMENT(const struct FBExpandedArrowStyle*, ExpandedArrowStyle)
	//SLATE_ARGUMENT(TArray<const struct FRowContentType>*, RowContents)
	SLATE_END_ARGS()

	/** Widget constructor */
	void Construct(const FArguments& InArgs);

	/** @return Returns the currently selected category item */
	TreeNodePtr GetSelectedDirectory() const;

	/** Selects the specified category */
	void SelectDirectory(const TreeNodePtr& CategoryToSelect);

	/** @return Returns true if the specified item is currently expanded in the tree */
	bool IsItemExpanded(const TreeNodePtr Item) const;

	void RefreshTree(TArray< TreeNodePtr > structure);
	void ExpandTreeItem(TreeNodePtr Item);
	void CollapseTreeItem(TreeNodePtr Item);
	void ToggleNodeExpansion(TreeNodePtr Item);
	
	TWeakObjectPtr<class UBCustomTreeView> TWidget;
	const struct FBTreeViewStyle* TStyle;
	const struct FBExpandedArrowStyle* ExpandedArrowStyle;
	bool ExpanderVisibility;
	TArray<FRow> Rows;

protected:
	TSharedRef<ITableRow> OnGenerateRow(TreeNodePtr Item, const TSharedRef<STableViewBase>& OwnerTable);

	TSharedPtr<SScrollBar> ExternalScrollbar();

	void OnGetChildren(TreeNodePtr Item, TArray< TreeNodePtr >& OutChildren);

	void OnSelectionChanged(TreeNodePtr Item, ESelectInfo::Type SelectInfo);

	void OnExpansionChanged(TreeNodePtr Item, bool ExpansionState);

	/** SWidget overrides */
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	TArray< TreeNodePtr > TreeStructure;
	/** The tree view widget*/
	TSharedPtr< STView > TView;
	TSharedPtr< SScrollBox > verticalscrollbox;
	FGeometry CachedGeometry;
	float currentscrolldisremaining;
};
