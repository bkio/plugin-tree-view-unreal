/// MIT License, Copyright Burak Kara, burak@burak.io, https://en.wikipedia.org/wiki/MIT_License

#pragma once

#include "CoreMinimal.h"
#include "SlateCore.h"
#include "SlateBasics.h"

/**
 * Expander arrow and indentation component that can be placed in a TableRow
 * of a TreeView. Intended for use by TMultiColumnRow in TreeViews.
 */
class SBTreeExpanderArrow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SBTreeExpanderArrow )
		:_IndentAmount(10)
		, _BaseIndentLevel(0)
	{ }
		SLATE_ARGUMENT(const struct FBExpandedArrowStyle*, StyleSet)
		SLATE_ATTRIBUTE(float, IndentAmount)
		SLATE_ATTRIBUTE(EVisibility, ExpanderVisibility)
		SLATE_ATTRIBUTE(int32, BaseIndentLevel)
	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs, const TSharedPtr<class ITableRow>& TableRow );

protected:
	/** Invoked when the expanded button is clicked (toggle item expansion) */
	FReply OnArrowClicked();

	/** @return Visible when has children; invisible otherwise */
	EVisibility GetExpanderVisibility() const;

	/** @return the margin corresponding to how far this item is indented */
	FMargin GetExpanderPadding() const;

	/** @return the name of an image that should be shown as the expander arrow */
	const FSlateBrush* GetExpanderImage() const;

	TWeakPtr<class ITableRow> OwnerRowPtr;

	/** A reference to the expander button */
	TSharedPtr<SButton> ExpanderArrow;

	/** The slate style to use */
	const struct FBExpandedArrowStyle* StyleSet;

	/** The amount of space to indent at each level */
	TAttribute<float> IndentAmount;

	/** The level in the tree that begins the indention amount */
	TAttribute<int32> BaseIndentLevel;
};