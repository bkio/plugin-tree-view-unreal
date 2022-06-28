/// MIT License, Copyright Burak Kara, burak@burak.io, https://en.wikipedia.org/wiki/MIT_License

#include "SBCustomTreeView.h"
#include "BCustomTreeView.h"
#include "SlateOptMacros.h"
#include "SBAdvancedTableRow.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SBCustomTreeView::Construct(const FArguments& Args)
{
	TWidget = Args._TWidget;
	TStyle = Args._TStyle;
	ExpandedArrowStyle = Args._ExpandedArrowStyle;
	ExpanderVisibility = Args._ExpanderVisibility;

	ChildSlot
		[
			SNew(SScrollBox).Orientation(Orient_Vertical).Style(&TStyle->ScrollBoxStyle)
			.ScrollBarStyle(&TStyle->VerticalScrollBarStyle).ScrollBarThickness(TStyle->VerticalScrollBarThickness)
		+ SScrollBox::Slot().VAlign(VAlign_Fill).HAlign(HAlign_Fill)
		.Padding(TStyle->TreeViewPadding)
		[
			SAssignNew(TView, STView)
			.SelectionMode(ESelectionMode::Single).ExternalScrollbar(ExternalScrollbar())
		.ClearSelectionOnClick(false)
		.TreeItemsSource(&TreeStructure)
		.OnGenerateRow(this, &SBCustomTreeView::OnGenerateRow)
		.OnGetChildren(this, &SBCustomTreeView::OnGetChildren)
		.OnSelectionChanged(this, &SBCustomTreeView::OnSelectionChanged)
		.OnExpansionChanged(this, &SBCustomTreeView::OnExpansionChanged)
		]
		];
}

void SBCustomTreeView::ExpandTreeItem(TreeNodePtr Item)
{
	if (Item.IsValid())
	{
		TView->SetItemExpansion(Item, true);
		RefreshTree(TreeStructure);
	}
}

void SBCustomTreeView::OnGetChildren(TreeNodePtr Item, TArray< TreeNodePtr >& OutChildren)
{
	const auto& SubCategories = Item->GetSubDirectories();
	OutChildren.Append(SubCategories);
}

void SBCustomTreeView::CollapseTreeItem(TreeNodePtr Item)
{
	if (Item.IsValid())
	{
		TView->SetItemExpansion(Item, false);
		RefreshTree(TreeStructure);
	}
}

void SBCustomTreeView::ToggleNodeExpansion(TreeNodePtr Item)
{
	if (Item.IsValid())
	{
		if (TView->IsItemExpanded(Item))
		{
			TView->SetItemExpansion(Item, false);
		}
		else
		{
			TView->SetItemExpansion(Item, true);
		}
		RefreshTree(TreeStructure);
	}
}

TSharedPtr<SScrollBar> SBCustomTreeView::ExternalScrollbar()
{
	return SNew(SScrollBar).Style(&TStyle->VerticalScrollBarStyle).Thickness(TStyle->VerticalScrollBarThickness);
}

TSharedRef<ITableRow> SBCustomTreeView::OnGenerateRow(TreeNodePtr Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!Item.IsValid())
	{
		return SNew(SBAdvancedTableRow<TreeNodePtr>, OwnerTable)
			[
				SNew(STextBlock)
				.Text(FText::FromString("EMPTY"))
			];
	}
	
	const auto& SubCategories = Item->GetSubDirectories();
	TArray< TreeNodePtr > OutChildren;
	OutChildren.Append(SubCategories);

	TreeNodePtr Parent = Item->GetParentCategory();
	int32 Iteration = 0;
	while (Parent.IsValid())
	{
		Iteration++;
		Parent = Parent->GetParentCategory();
	}

	FMargin RowPadding = TStyle->TextPadding +  FMargin(Iteration) * (TWidget->RowDefaultPadding + Item->GetTreeNodePadding());

	FRow Row = FRow();
	Row.NodeId = Item->GetNodeID();

	if (GEngine->GameViewport)
	{
		UWorld* world = GEngine->GameViewport->GetWorld();
		bool isrowcontentvalid = false;
		TSubclassOf<class UUserWidget> CurrentRowContent;
		if (TWidget->DefaultRowContent)
		{
			isrowcontentvalid = true;
			CurrentRowContent = TWidget->DefaultRowContent;
		}

		for (int i = 0; i < TWidget->RowContentsByParent.Num(); i++)
		{
			if (TWidget->RowContentsByParent[i].RowContent && TWidget->RowContentsByParent[i].ParentID == Item->GetParentID())
			{
				isrowcontentvalid = true;
				CurrentRowContent = TWidget->RowContentsByParent[i].RowContent;
				break;
			}
		}

		for (int i = 0; i < TWidget->RowContentsById.Num(); i++)
		{
			if (TWidget->RowContentsById[i].RowContent && TWidget->RowContentsById[i].NodeId == Item->GetNodeID())
			{
				isrowcontentvalid = true;
				CurrentRowContent = TWidget->RowContentsById[i].RowContent;
				break;
			}
		}

		if (world && isrowcontentvalid)
		{
			Row.RowWidget = CreateWidget<UUserWidget>(world, CurrentRowContent);
			Rows.Add(Row);
			TWidget->HandleOnGenerateRow(Item, Row.RowWidget, OutChildren);

			if (TStyle->EnableTableRowStyle)
			{
				return SNew(SBAdvancedTableRow<TreeNodePtr>, OwnerTable).Style(&TStyle->TableRowStyle)
					.ExpanderStyleSet(ExpandedArrowStyle).Padding(RowPadding)
					.ExpanderVisibility(ExpanderVisibility)
					[
						Row.RowWidget->TakeWidget()

					];
			}
			else
			{

				return SNew(SBAdvancedTableRow<TreeNodePtr>, OwnerTable).Style(&TStyle->GetNoHoverTableRowStyle())
					.ExpanderStyleSet(ExpandedArrowStyle).Padding(RowPadding)
					.ExpanderVisibility(ExpanderVisibility)
					[
						Row.RowWidget->TakeWidget()

					];
			}
		}
		else
		{
			Rows.Add(Row);
			TWidget->HandleOnGenerateRow(Item, Row.RowWidget, OutChildren);

			if (TStyle->EnableTableRowStyle)
			{
				return SNew(SBAdvancedTableRow<TreeNodePtr>, OwnerTable).Style(&TStyle->TableRowStyle)
					.ExpanderStyleSet(ExpandedArrowStyle).Padding(RowPadding)
					.ExpanderVisibility(ExpanderVisibility)

					[
						SNew(STextBlock).TextStyle(&TStyle->RowTextStyle)
						.Text(FText::FromString(Item->GetDisplayName()))

					];
			}
			else
			{
				return SNew(SBAdvancedTableRow<TreeNodePtr>, OwnerTable).Style(&TStyle->GetNoHoverTableRowStyle())
					.ExpanderStyleSet(ExpandedArrowStyle).Padding(RowPadding)
					.ExpanderVisibility(ExpanderVisibility)

					[
						SNew(STextBlock).TextStyle(&TStyle->RowTextStyle)
						.Text(FText::FromString(Item->GetDisplayName()))

					];
			}
		}
	}
	else
	{
		TWidget->HandleOnGenerateRow(Item, Row.RowWidget, OutChildren);
		Rows.Add(Row);

		if (TStyle->EnableTableRowStyle)
		{
			return SNew(SBAdvancedTableRow< TreeNodePtr >, OwnerTable).Style(&TStyle->TableRowStyle)
				.ExpanderStyleSet(ExpandedArrowStyle).Padding(RowPadding)
				.ExpanderVisibility(ExpanderVisibility)

				[
					SNew(STextBlock).TextStyle(&TStyle->RowTextStyle)
					.Text(FText::FromString(Item->GetDisplayName()))

				];
		}
		else
		{
			return SNew(SBAdvancedTableRow< TreeNodePtr >, OwnerTable).Style(&TStyle->GetNoHoverTableRowStyle())
				.ExpanderStyleSet(ExpandedArrowStyle).Padding(RowPadding)
				.ExpanderVisibility(ExpanderVisibility)

				[
					SNew(STextBlock).TextStyle(&TStyle->RowTextStyle)
					.Text(FText::FromString(Item->GetDisplayName()))

				];
		}
	}
}

void SBCustomTreeView::OnSelectionChanged(TreeNodePtr Item, ESelectInfo::Type SelectInfo)
{
	if (Item.IsValid())
	{
		class UUserWidget* SelectedRowWidget;
		for (int i = 0; i < Rows.Num(); i++)
		{
			if (Item->GetNodeID() == Rows[i].NodeId)
			{
				SelectedRowWidget = Rows[i].RowWidget;
				TWidget->HandleOnSelectionChanged(Item, SelectedRowWidget);
				break;
			}
		}
	}
	else
	{
		TWidget->HandleOnSelectionLost();
	}
}

void SBCustomTreeView::OnExpansionChanged(TreeNodePtr Item, bool ExpansionState)
{
	if (Item.IsValid())
	{
		class UUserWidget* ExpandedRowWidget;
		for (int i = 0; i < Rows.Num(); i++)
		{
			if (Item->GetNodeID() == Rows[i].NodeId)
			{
				ExpandedRowWidget = Rows[i].RowWidget;
				TWidget->HandleOnExpansionChanged(Item, ExpandedRowWidget, ExpansionState);
				break;
			}
		}
	}
}

void SBCustomTreeView::RefreshTree(TArray< TreeNodePtr > structure)
{
	TreeStructure = structure;
	if (TView.IsValid())
	{
		TView->RequestTreeRefresh();
	}
}

TreeNodePtr SBCustomTreeView::GetSelectedDirectory() const
{
	if (TView.IsValid())
	{
		auto SelectedItems = TView->GetSelectedItems();
		if (SelectedItems.Num() > 0)
		{
			const auto& SelectedCategoryItem = SelectedItems[0];
			return SelectedCategoryItem;
		}
	}
	return NULL;
}

void SBCustomTreeView::SelectDirectory(const TreeNodePtr& CategoryToSelect)
{
	if (ensure(TView.IsValid()))
	{
		TView->SetSelection(CategoryToSelect);
	}
}

bool SBCustomTreeView::IsItemExpanded(const TreeNodePtr Item) const
{
	return TView->IsItemExpanded(Item);
}

void SBCustomTreeView::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION