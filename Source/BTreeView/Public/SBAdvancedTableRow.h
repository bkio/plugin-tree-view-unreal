/// MIT License, Copyright Burak Kara, burak@burak.io, https://en.wikipedia.org/wiki/MIT_License

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "SBTreeExpanderArrow.h"
#include "InputCoreTypes.h"
#include "Styling/SlateColor.h"
#include "Layout/Geometry.h"
#include "Input/Events.h"
#include "Input/DragAndDrop.h"
#include "Input/Reply.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Layout/Margin.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Layout/SBorder.h"
#include "Framework/Views/ITypedTableView.h"
#include "Widgets/Views/STableViewBase.h"
#include "Rendering/DrawElements.h"
#include "Types/SlateStructs.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Views/SExpanderArrow.h"
#include "Widgets/Views/SHeaderRow.h"
#include "Framework/Views/TableViewTypeTraits.h"

template <typename ItemType> class SListView;

/**
 * Where we are going to drop relative to the target item.
 */

template <typename ItemType> class SListView;

DECLARE_DELEGATE_OneParam(FOnTableRowDragEnter, FDragDropEvent const&);
DECLARE_DELEGATE_OneParam(FOnTableRowDragLeave, FDragDropEvent const&);
DECLARE_DELEGATE_RetVal_OneParam(FReply, FOnTableRowDrop, FDragDropEvent const&);

/**
 * The ListView is populated by Selectable widgets.
 * A Selectable widget is away of the ListView containing it (OwnerWidget) and holds arbitrary Content (Content).
 * A Selectable works with its corresponding ListView to provide selection functionality.
 */
template<typename ItemType>
class SBAdvancedTableRow : public ITableRow, public SBorder
{
	static_assert(TIsValidListItem<ItemType>::Value, "Item type T must be UObjectBase*, TSharedRef<>, or TSharedPtr<>.");

public:
	/** Delegate signature for querying whether this FDragDropEvent will be handled by the drop target of type ItemType. */
	DECLARE_DELEGATE_RetVal_ThreeParams(TOptional<EItemDropZone>, FOnCanAcceptDrop, const FDragDropEvent&, EItemDropZone, ItemType);
	/** Delegate signature for handling the drop of FDragDropEvent onto target of type ItemType */
	DECLARE_DELEGATE_RetVal_ThreeParams(FReply, FOnAcceptDrop, const FDragDropEvent&, EItemDropZone, ItemType);

	SLATE_BEGIN_ARGS(SBAdvancedTableRow< ItemType >)
		: _Style(&FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"))
		, _Padding(FMargin(0))
		, _ExpanderVisibility(true)
		, _ShowSelection(true)
		, _Content()
	{}

    SLATE_STYLE_ARGUMENT(FTableRowStyle, Style)
	SLATE_ARGUMENT(const struct FBExpandedArrowStyle*, ExpanderStyleSet)

	// High Level DragAndDrop

	/**
		* Handle this event to determine whether a drag and drop operation can be executed on top of the target row widget.
		* Most commonly, this is used for previewing re-ordering and re-organization operations in lists or trees.
		* e.g. A user is dragging one item into a different spot in the list or tree.
		*      This delegate will be called to figure out if we should give visual feedback on whether an item will
		*      successfully drop into the list.
		*/
	SLATE_EVENT(FOnCanAcceptDrop, OnCanAcceptDrop)

	/**
		* Perform a drop operation onto the target row widget
		* Most commonly used for executing a re-ordering and re-organization operations in lists or trees.
		* e.g. A user was dragging one item into a different spot in the list; they just dropped it.
		*      This is our chance to handle the drop by reordering items and calling for a list refresh.
		*/
	SLATE_EVENT(FOnAcceptDrop, OnAcceptDrop)

	// Low level DragAndDrop
	SLATE_EVENT(FOnDragDetected, OnDragDetected)
	SLATE_EVENT(FOnTableRowDragEnter, OnDragEnter)
	SLATE_EVENT(FOnTableRowDragLeave, OnDragLeave)
	SLATE_EVENT(FOnTableRowDrop, OnDrop)

	SLATE_ATTRIBUTE(FMargin, Padding)
	SLATE_ATTRIBUTE( bool, ExpanderVisibility)

	SLATE_ARGUMENT(bool, ShowSelection)

	SLATE_DEFAULT_SLOT(typename SBAdvancedTableRow<ItemType>::FArguments, Content)

	SLATE_END_ARGS()

	virtual void ConstructChildren(ETableViewMode::Type InOwnerTableMode, const TAttribute<FMargin>& InPadding,
			bool InExpanderVisibility, const TSharedRef<SWidget>& InContent)
	{
		this->Content = InContent;
		InnerContentSlot = nullptr;

		SHorizontalBox::FSlot* InnerContentSlotNativePtr = nullptr;
		if (InExpanderVisibility)
		{
			this->ChildSlot
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth().HAlign(HAlign_Right).VAlign(VAlign_Fill)
				[
					SNew(SBTreeExpanderArrow, SharedThis(this))
					.StyleSet(ExpanderStyleSet)
				]

			+ SHorizontalBox::Slot()
				.FillWidth(1)
				.Expose(InnerContentSlotNativePtr)
				.Padding(InPadding)
				[
					InContent
				]
				];

			InnerContentSlot = InnerContentSlotNativePtr;
		}
		else
		{
			this->ChildSlot
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth().HAlign(HAlign_Right).VAlign(VAlign_Fill)
				[
					SNew(SSpacer)
				]
			+ SHorizontalBox::Slot()
				.FillWidth(1)
				.Expose(InnerContentSlotNativePtr)
				.Padding(InPadding)
				[
					InContent
				]
				];

			InnerContentSlot = InnerContentSlotNativePtr;
		}
	}

	/**
		* Construct this widget
		*
		* @param	InArgs	The declaration data for this widget
		*/
	void Construct(const typename SBAdvancedTableRow<ItemType>::FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		/** Note: Please initialize any state in ConstructInternal, not here. This is because SBAdvancedTableRow derivatives call ConstructInternal directly to avoid constructing children. **/

		ConstructInternal(InArgs, InOwnerTableView);
		bool V = InArgs._ExpanderVisibility.Get();
		ConstructChildren(
			InOwnerTableView->TableViewMode,
			InArgs._Padding, V, 
			InArgs._Content.Widget
		);
	}

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
	{
		TSharedRef< ITypedTableView<ItemType> > OwnerWidget = OwnerTablePtr.Pin().ToSharedRef();
		const bool bIsActive = OwnerWidget->AsWidget()->HasKeyboardFocus();
		const ItemType* MyItem = OwnerWidget->Private_ItemFromWidget(this);
		if (bIsActive && OwnerWidget->Private_UsesSelectorFocus() && OwnerWidget->Private_HasSelectorFocus(*MyItem))
		{
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(),
				&Style->SelectorFocusedBrush,
				ESlateDrawEffect::None,
				Style->SelectorFocusedBrush.GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
			);
		}

		LayerId = SBorder::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

		if (ItemDropZone.IsSet())
		{
			// Draw feedback for user dropping an item above, below, or onto a row.
			const FSlateBrush* DropIndicatorBrush = [&]()
			{
				switch (ItemDropZone.GetValue())
				{
				case EItemDropZone::AboveItem: return &Style->DropIndicator_Above; break;
				default:
				case EItemDropZone::OntoItem: return &Style->DropIndicator_Onto; break;
				case EItemDropZone::BelowItem: return &Style->DropIndicator_Below; break;
				};
			}();

			FSlateDrawElement::MakeBox
			(
				OutDrawElements,
				LayerId++,
				AllottedGeometry.ToPaintGeometry(),
				DropIndicatorBrush,
				ESlateDrawEffect::None,
				DropIndicatorBrush->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
			);
		}

		return LayerId;
	}

	/**
	 * Called when a mouse button is double clicked.  Override this in derived classes.
	 *
	 * @param  InMyGeometry  Widget geometry.
	 * @param  InMouseEvent  Mouse button event.
	 * @return  Returns whether the event was handled, along with other possible actions.
	 */
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			TSharedRef< ITypedTableView<ItemType> > OwnerWidget = OwnerTablePtr.Pin().ToSharedRef();

			// Only one item can be double-clicked
			const ItemType* MyItem = OwnerWidget->Private_ItemFromWidget(this);

			// If we're configured to route double-click messages to the owner of the table, then
			// do that here.  Otherwise, we'll toggle expansion.
			const bool bWasHandled = OwnerWidget->Private_OnItemDoubleClicked(*MyItem);
			if (!bWasHandled)
			{
				ToggleExpansion();
			}

			return FReply::Handled();
		}
		return FReply::Unhandled();
	}

	/**
	 * See SWidget::OnMouseButtonDown
	 *
	 * @param MyGeometry The Geometry of the widget receiving the event.
	 * @param MouseEvent Information about the input event.
	 * @return Whether the event was handled along with possible requests for the system to take action.
	 */
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		TSharedPtr< ITypedTableView<ItemType> > OwnerWidget = OwnerTablePtr.Pin();
		bChangedSelectionOnMouseDown = false;

		check(OwnerWidget.IsValid());

		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			const ESelectionMode::Type SelectionMode = GetSelectionMode();
			if (SelectionMode != ESelectionMode::None)
			{
				const ItemType& MyItem = *OwnerWidget->Private_ItemFromWidget(this);
				const bool bIsSelected = OwnerWidget->Private_IsItemSelected(MyItem);

				if (SelectionMode == ESelectionMode::Multi)
				{
					if (MouseEvent.IsControlDown())
					{
						OwnerWidget->Private_SetItemSelection(MyItem, !bIsSelected, true);
						bChangedSelectionOnMouseDown = true;
					}
					else if (MouseEvent.IsShiftDown())
					{
						OwnerWidget->Private_SelectRangeFromCurrentTo(MyItem);
						bChangedSelectionOnMouseDown = true;
					}
				}

				if (!bIsSelected && !bChangedSelectionOnMouseDown)
				{
					OwnerWidget->Private_ClearSelection();
					OwnerWidget->Private_SetItemSelection(MyItem, true, true);
					bChangedSelectionOnMouseDown = true;
				}

				return FReply::Handled()
					.DetectDrag(SharedThis(this), EKeys::LeftMouseButton)
					.SetUserFocus(OwnerWidget->AsWidget(), EFocusCause::Mouse)
					.CaptureMouse(SharedThis(this));
			}
		}
		return FReply::Unhandled();
	}

	/**
	 * See SWidget::OnMouseButtonUp
	 *
	 * @param MyGeometry The Geometry of the widget receiving the event.
	 * @param MouseEvent Information about the input event.
	 * @return Whether the event was handled along with possible requests for the system to take action.
	 */
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		TSharedPtr< ITypedTableView<ItemType> > OwnerWidget = OwnerTablePtr.Pin();
		check(OwnerWidget.IsValid());

		// Requires #include "Widgets/Views/STableViewBase.h"
		TSharedRef< STableViewBase > OwnerTableViewBase = StaticCastSharedPtr< SListView<ItemType> >(OwnerWidget).ToSharedRef();

		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			FReply Reply = FReply::Unhandled().ReleaseMouseCapture();

			if (bChangedSelectionOnMouseDown)
			{
				Reply = FReply::Handled().ReleaseMouseCapture();
			}

			const bool bIsUnderMouse = MyGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition());
			if (HasMouseCapture())
			{
				if (bIsUnderMouse)
				{
					switch (GetSelectionMode())
					{
					case ESelectionMode::SingleToggle:
					{
						if (!bChangedSelectionOnMouseDown)
						{
							OwnerWidget->Private_ClearSelection();
							OwnerWidget->Private_SignalSelectionChanged(ESelectInfo::OnMouseClick);
						}

						Reply = FReply::Handled().ReleaseMouseCapture();
					}
					break;

					case ESelectionMode::Multi:
					{
						if (!bChangedSelectionOnMouseDown && !MouseEvent.IsControlDown() && !MouseEvent.IsShiftDown())
						{
							const ItemType* MyItem = OwnerWidget->Private_ItemFromWidget(this);
							check(MyItem != nullptr);

							const bool bIsSelected = OwnerWidget->Private_IsItemSelected(*MyItem);
							if (bIsSelected && OwnerWidget->Private_GetNumSelectedItems() > 1)
							{
								// We are mousing up on a previous selected item;
								// deselect everything but this item.

								OwnerWidget->Private_ClearSelection();
								OwnerWidget->Private_SetItemSelection(*MyItem, true, true);
								OwnerWidget->Private_SignalSelectionChanged(ESelectInfo::OnMouseClick);

								Reply = FReply::Handled().ReleaseMouseCapture();
							}
						}
					}
					break;
					}
				}

				const ItemType* MyItem = OwnerWidget->Private_ItemFromWidget(this);
				if (MyItem && OwnerWidget->Private_OnItemClicked(*MyItem))
				{
					Reply = FReply::Handled().ReleaseMouseCapture();
				}

				if (bChangedSelectionOnMouseDown)
				{
					OwnerWidget->Private_SignalSelectionChanged(ESelectInfo::OnMouseClick);
				}

				return Reply;
			}
		}
		else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton && !OwnerTableViewBase->IsRightClickScrolling())
		{
			// Handle selection of items when releasing the right mouse button, but only if the user isn't actively
			// scrolling the view by holding down the right mouse button.

			switch (GetSelectionMode())
			{
			case ESelectionMode::Single:
			case ESelectionMode::SingleToggle:
			case ESelectionMode::Multi:
			{
				// Only one item can be selected at a time
				const ItemType* MyItem = OwnerWidget->Private_ItemFromWidget(this);
				const bool bIsSelected = OwnerWidget->Private_IsItemSelected(*MyItem);

				// Select the item under the cursor
				if (!bIsSelected)
				{
					OwnerWidget->Private_ClearSelection();
					OwnerWidget->Private_SetItemSelection(*MyItem, true, true);
					OwnerWidget->Private_SignalSelectionChanged(ESelectInfo::OnMouseClick);
				}

				OwnerWidget->Private_OnItemRightClicked(*MyItem, MouseEvent);

				return FReply::Handled();
			}
			break;
			}
		}
		return FReply::Unhandled();
	}

	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override
	{
		bProcessingSelectionTouch = true;

		return
			FReply::Handled()
			// Drag detect because if this tap turns into a drag, we stop processing
			// the selection touch.
			.DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
	}

	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override
	{
		FReply Reply = FReply::Unhandled();

		if (bProcessingSelectionTouch)
		{
			bProcessingSelectionTouch = false;
			const TSharedPtr<ITypedTableView<ItemType>> OwnerWidget = OwnerTablePtr.Pin();
			if (const ItemType* MyItem = OwnerWidget->Private_ItemFromWidget(this))
			{
				ESelectionMode::Type SelectionMode = GetSelectionMode();
				if (SelectionMode != ESelectionMode::None)
				{
					const bool bIsSelected = OwnerWidget->Private_IsItemSelected(*MyItem);
					if (!bIsSelected)
					{
						if (SelectionMode != ESelectionMode::Multi)
						{
							OwnerWidget->Private_ClearSelection();
						}
						OwnerWidget->Private_SetItemSelection(*MyItem, true, true);
						OwnerWidget->Private_SignalSelectionChanged(ESelectInfo::OnMouseClick);

						Reply = FReply::Handled();
					}
					else if (SelectionMode == ESelectionMode::SingleToggle || SelectionMode == ESelectionMode::Multi)
					{
						OwnerWidget->Private_SetItemSelection(*MyItem, true, true);
						OwnerWidget->Private_SignalSelectionChanged(ESelectInfo::OnMouseClick);

						Reply = FReply::Handled();
					}
				}

				if (OwnerWidget->Private_OnItemClicked(*MyItem))
				{
					Reply = FReply::Handled();
				}
			}
		}
		return Reply;
	}

	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		if (bProcessingSelectionTouch)
		{
			// With touch input, dragging scrolls the list while selection requires a tap.
			// If we are processing a touch and it turned into a drag; pass it on to the 
			bProcessingSelectionTouch = false;
			return FReply::Handled().CaptureMouse(OwnerTablePtr.Pin()->AsWidget());
		}
		else if (HasMouseCapture() && bChangedSelectionOnMouseDown)
		{
			TSharedPtr< ITypedTableView<ItemType> > OwnerWidget = OwnerTablePtr.Pin();
			OwnerWidget->Private_SignalSelectionChanged(ESelectInfo::OnMouseClick);
		}

		if (OnDragDetected_Handler.IsBound())
		{
			return OnDragDetected_Handler.Execute(MyGeometry, MouseEvent);
		}
		else
		{
			return FReply::Unhandled();
		}
	}

	virtual void OnDragEnter(FGeometry const& MyGeometry, FDragDropEvent const& DragDropEvent) override
	{
		if (OnDragEnter_Handler.IsBound())
		{
			OnDragEnter_Handler.Execute(DragDropEvent);
		}
	}

	virtual void OnDragLeave(FDragDropEvent const& DragDropEvent) override
	{
		ItemDropZone = TOptional<EItemDropZone>();

		if (OnDragLeave_Handler.IsBound())
		{
			OnDragLeave_Handler.Execute(DragDropEvent);
		}
	}

	/** @return the zone (above, onto, below) based on where the user is hovering over within the row */
	EItemDropZone ZoneFromPointerPosition(FVector2D LocalPointerPos, float RowHeight)
	{
		const float VecticalZoneBoundarySu = FMath::Clamp(RowHeight * 0.25f, 3.0f, 10.0f);
		if (LocalPointerPos.Y < VecticalZoneBoundarySu)
		{
			return EItemDropZone::AboveItem;
		}
		else if (LocalPointerPos.Y > RowHeight - VecticalZoneBoundarySu)
		{
			return EItemDropZone::BelowItem;
		}
		else
		{
			return EItemDropZone::OntoItem;
		}
	}

	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override
	{
		if (OnCanAcceptDrop.IsBound())
		{
			const FVector2D LocalPointerPos = MyGeometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition());
			const EItemDropZone ItemHoverZone = ZoneFromPointerPosition(LocalPointerPos, MyGeometry.GetLocalSize().Y);

			ItemDropZone = [&]()
			{
				TSharedPtr< ITypedTableView<ItemType> > OwnerWidget = OwnerTablePtr.Pin();
				const ItemType* MyItem = OwnerWidget->Private_ItemFromWidget(this);
				return OnCanAcceptDrop.Execute(DragDropEvent, ItemHoverZone, *MyItem);
			}();

			return FReply::Handled();
		}
		else
		{
			return FReply::Unhandled();
		}

	}

	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override
	{
		const FReply Reply = [&]()
		{
			if (OnAcceptDrop.IsBound())
			{
				const TSharedRef< ITypedTableView<ItemType> > OwnerWidget = OwnerTablePtr.Pin().ToSharedRef();

				// A drop finishes the drag/drop operation, so we are no longer providing any feedback.
				ItemDropZone = TOptional<EItemDropZone>();

				// Find item associated with this widget.
				const ItemType* MyItem = OwnerWidget->Private_ItemFromWidget(this);

				// Which physical drop zone is the drop about to be performed onto?
				const FVector2D LocalPointerPos = MyGeometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition());
				const EItemDropZone HoveredZone = ZoneFromPointerPosition(LocalPointerPos, MyGeometry.GetLocalSize().Y);

				// The row gets final say over which zone to drop onto regardless of physical location.
				const TOptional<EItemDropZone> ReportedZone = OnCanAcceptDrop.IsBound()
					? OnCanAcceptDrop.Execute(DragDropEvent, HoveredZone, *MyItem)
					: HoveredZone;

				if (ReportedZone.IsSet())
				{
					FReply DropReply = OnAcceptDrop.Execute(DragDropEvent, ReportedZone.GetValue(), *MyItem);
					if (DropReply.IsEventHandled() && ReportedZone.GetValue() == EItemDropZone::OntoItem)
					{
						// Expand the drop target just in case, so that what we dropped is visible.
						OwnerWidget->Private_SetItemExpansion(*MyItem, true);
					}

					return DropReply;
				}
			}

			return FReply::Unhandled();
		}();

		// @todo slate : Made obsolete by OnAcceptDrop. Get rid of this.
		if (!Reply.IsEventHandled() && OnDrop_Handler.IsBound())
		{
			return OnDrop_Handler.Execute(DragDropEvent);
		}

		return Reply;
	}

	virtual void InitializeRow() override {}
	virtual void ResetRow() override {}

	virtual void SetIndexInList(int32 InIndexInList) override
	{
		IndexInList = InIndexInList;
	}

	virtual bool IsItemExpanded() const override
	{
		TSharedPtr< ITypedTableView<ItemType> > OwnerWidget = OwnerTablePtr.Pin();
		const ItemType* MyItem = OwnerWidget->Private_ItemFromWidget(this);
		const bool bIsItemExpanded = OwnerWidget->Private_IsItemExpanded(*MyItem);
		return bIsItemExpanded;
	}

	virtual void ToggleExpansion() override
	{
		TSharedPtr< ITypedTableView<ItemType> > OwnerWidget = OwnerTablePtr.Pin();

		const bool bItemHasChildren = OwnerWidget->Private_DoesItemHaveChildren(IndexInList);
		// Nothing to expand if row being clicked on doesn't have children
		if (bItemHasChildren)
		{
			ItemType MyItem = *(OwnerWidget->Private_ItemFromWidget(this));
			const bool bIsItemExpanded = bItemHasChildren && OwnerWidget->Private_IsItemExpanded(MyItem);
			OwnerWidget->Private_SetItemExpansion(MyItem, !bIsItemExpanded);
		}
	}

	virtual bool IsItemSelected() const override
	{
		TSharedPtr<ITypedTableView<ItemType>> OwnerTable = OwnerTablePtr.Pin();
		const ItemType& MyItem = *OwnerTable->Private_ItemFromWidget(this);
		return OwnerTable->Private_IsItemSelected(MyItem);
	}

	virtual int32 GetIndentLevel() const override
	{
		return OwnerTablePtr.Pin()->Private_GetNestingDepth(IndexInList);
	}

	virtual int32 DoesItemHaveChildren() const override
	{
		return OwnerTablePtr.Pin()->Private_DoesItemHaveChildren(IndexInList);
	}

	virtual TBitArray<> GetWiresNeededByDepth() const override
	{
		return OwnerTablePtr.Pin()->Private_GetWiresNeededByDepth(IndexInList);
	}

	virtual bool IsLastChild() const override
	{
		return OwnerTablePtr.Pin()->Private_IsLastChild(IndexInList);
	}

	virtual TSharedRef<SWidget> AsWidget() override
	{
		return SharedThis(this);
	}

	/** Set the entire content of this row, replacing any extra UI (such as the expander arrows for tree views) that was added by ConstructChildren */
	virtual void SetRowContent(TSharedRef< SWidget > InContent)
	{
		this->Content = InContent;
		InnerContentSlot = nullptr;
		SBorder::SetContent(InContent);
	}

	/** Set the inner content of this row, preserving any extra UI (such as the expander arrows for tree views) that was added by ConstructChildren */
	virtual void SetContent(TSharedRef< SWidget > InContent) override
	{
		this->Content = InContent;

		if (InnerContentSlot)
		{
			InnerContentSlot->AttachWidget(InContent);
		}
		else
		{
			SBorder::SetContent(InContent);
		}
	}

	/** Get the inner content of this row */
	virtual TSharedPtr<SWidget> GetContent() override
	{
		if (this->Content.IsValid())
		{
			return this->Content.Pin();
		}
		else
		{
			return TSharedPtr<SWidget>();
		}
	}

	virtual void Private_OnExpanderArrowShiftClicked() override
	{
		TSharedPtr< ITypedTableView<ItemType> > OwnerWidget = OwnerTablePtr.Pin();

		const bool bItemHasChildren = OwnerWidget->Private_DoesItemHaveChildren(IndexInList);
		// Nothing to expand if row being clicked on doesn't have children
		if (bItemHasChildren)
		{
			ItemType MyItem = *(OwnerWidget->Private_ItemFromWidget(this));
			const bool IsItemExpanded = bItemHasChildren && OwnerWidget->Private_IsItemExpanded(MyItem);
			OwnerWidget->Private_OnExpanderArrowShiftClicked(MyItem, !IsItemExpanded);
		}
	}

	/** @return The border to be drawn around this list item */
	const FSlateBrush* GetBorder() const
	{
		TSharedPtr< ITypedTableView<ItemType> > OwnerWidget = OwnerTablePtr.Pin();

		const bool bIsActive = OwnerWidget->AsWidget()->HasKeyboardFocus();

		static FName GenericWhiteBoxBrush("GenericWhiteBox");

		// @todo: Slate Style - make this part of the widget style
		const FSlateBrush* WhiteBox = FCoreStyle::Get().GetBrush(GenericWhiteBoxBrush);

		const ItemType* MyItem = OwnerWidget->Private_ItemFromWidget(this);

		check(MyItem);

		const bool bIsSelected = OwnerWidget->Private_IsItemSelected(*MyItem);

		if (bIsSelected && bShowSelection)
		{
			if (bIsActive)
			{
				return IsHovered()
					? &Style->ActiveHoveredBrush
					: &Style->ActiveBrush;
			}
			else
			{
				return IsHovered()
					? &Style->InactiveHoveredBrush
					: &Style->InactiveBrush;
			}
		}
		else
		{
			// Add a slightly lighter background for even rows
			const bool bAllowSelection = GetSelectionMode() != ESelectionMode::None;
			if (IndexInList % 2 == 0)
			{
				return (IsHovered() && bAllowSelection)
					? &Style->EvenRowBackgroundHoveredBrush
					: &Style->EvenRowBackgroundBrush;

			}
			else
			{
				return (IsHovered() && bAllowSelection)
					? &Style->OddRowBackgroundHoveredBrush
					: &Style->OddRowBackgroundBrush;

			}
		}
	}

	/**
	 * Callback to determine if the row is selected singularly and has keyboard focus or not
	 *
	 * @return		true if selected by owning widget.
	 */
	bool IsSelectedExclusively() const
	{
		TSharedPtr< ITypedTableView< ItemType > > OwnerWidget = OwnerTablePtr.Pin();

		if (!OwnerWidget->AsWidget()->HasKeyboardFocus() || OwnerWidget->Private_GetNumSelectedItems() > 1)
		{
			return false;
		}

		const ItemType* MyItem = OwnerWidget->Private_ItemFromWidget(this);
		return OwnerWidget->Private_IsItemSelected(*MyItem);
	}

	/**
	 * Callback to determine if the row is selected or not
	 *
	 * @return		true if selected by owning widget.
	 */
	bool IsSelected() const
	{
		TSharedPtr< ITypedTableView< ItemType > > OwnerWidget = OwnerTablePtr.Pin();

		const ItemType* MyItem = OwnerWidget->Private_ItemFromWidget(this);
		return OwnerWidget->Private_IsItemSelected(*MyItem);
	}

	/** By default, this function does nothing, it should be implemented by derived class */
	virtual FVector2D GetRowSizeForColumn(const FName& InColumnName) const override
	{
		return FVector2D::ZeroVector;
	}

	/** Protected constructor; SWidgets should only be instantiated via declarative syntax. */
	SBAdvancedTableRow()
		: IndexInList(0)
		, bShowSelection(true)
	{ }

protected:

	/**
	 * An internal method to construct and setup this row widget (purposely avoids child construction).
	 * Split out from Construct() so that sub-classes can invoke super construction without invoking
	 * ConstructChildren() (sub-classes may want to constuct their own children in their own special way).
	 *
	 * @param  InArgs			Declaration data for this widget.
	 * @param  InOwnerTableView	The table that this row belongs to.
	 */
	void ConstructInternal(FArguments const& InArgs, TSharedRef<STableViewBase> const& InOwnerTableView)
	{
		bProcessingSelectionTouch = false;

		check(InArgs._Style);
		Style = InArgs._Style;

		check(InArgs._ExpanderStyleSet);
		ExpanderStyleSet = InArgs._ExpanderStyleSet;

		this->BorderImage = TAttribute<const FSlateBrush*>(this, &SBAdvancedTableRow::GetBorder);

		this->ForegroundColor = TAttribute<FSlateColor>(this, &SBAdvancedTableRow::GetForegroundBasedOnSelection);

		this->OnCanAcceptDrop = InArgs._OnCanAcceptDrop;
		this->OnAcceptDrop = InArgs._OnAcceptDrop;

		this->OnDragDetected_Handler = InArgs._OnDragDetected;
		this->OnDragEnter_Handler = InArgs._OnDragEnter;
		this->OnDragLeave_Handler = InArgs._OnDragLeave;
		this->OnDrop_Handler = InArgs._OnDrop;

		this->SetOwnerTableView(InOwnerTableView);

		this->bShowSelection = InArgs._ShowSelection;
	}

	void SetOwnerTableView(TSharedPtr<STableViewBase> OwnerTableView)
	{
		// We want to cast to a ITypedTableView.
		// We cast to a SListView<ItemType> because C++ doesn't know that
		// being a STableView implies being a ITypedTableView.
		// See SListView.
		this->OwnerTablePtr = StaticCastSharedPtr< SListView<ItemType> >(OwnerTableView);
	}

	FSlateColor GetForegroundBasedOnSelection() const
	{
		const TSharedPtr< ITypedTableView<ItemType> > OwnerWidget = OwnerTablePtr.Pin();
		const FSlateColor& NonSelectedForeground = Style->TextColor;
		const FSlateColor& SelectedForeground = Style->SelectedTextColor;

		if (!bShowSelection || !OwnerWidget.IsValid())
		{
			return NonSelectedForeground;
		}

		const ItemType* MyItem = OwnerWidget->Private_ItemFromWidget(this);
		const bool bIsSelected = OwnerWidget->Private_IsItemSelected(*MyItem);

		return bIsSelected
			? SelectedForeground
			: NonSelectedForeground;
	}

	virtual ESelectionMode::Type GetSelectionMode() const override
	{
		const TSharedPtr< ITypedTableView<ItemType> > OwnerWidget = OwnerTablePtr.Pin();
		return OwnerWidget->Private_GetSelectionMode();
	}

protected:
	/** The list that owns this Selectable */
	TWeakPtr< ITypedTableView<ItemType> > OwnerTablePtr;

	/** Index of the corresponding data item in the list */
	int32 IndexInList;

	/** Whether or not to visually show that this row is selected */
	bool bShowSelection;

	/** Style used to draw this table row */
	const FTableRowStyle* Style;

	/** The slate style to use with the expander */
	const struct FBExpandedArrowStyle* ExpanderStyleSet;

	/** @see SBAdvancedTableRow's OnCanAcceptDrop event */
	FOnCanAcceptDrop OnCanAcceptDrop;

	/** @see SBAdvancedTableRow's OnAcceptDrop event */
	FOnAcceptDrop OnAcceptDrop;

	/** Are we currently dragging/dropping over this item? */
	TOptional<EItemDropZone> ItemDropZone;

	/** Delegate triggered when a user starts to drag a list item */
	FOnDragDetected OnDragDetected_Handler;

	/** Delegate triggered when a user's drag enters the bounds of this list item */
	FOnTableRowDragEnter OnDragEnter_Handler;

	/** Delegate triggered when a user's drag leaves the bounds of this list item */
	FOnTableRowDragLeave OnDragLeave_Handler;

	/** Delegate triggered when a user's drag is dropped in the bounds of this list item */
	FOnTableRowDrop OnDrop_Handler;

	/** The slot that contains the inner content for this row. If this is set, SetContent populates this slot with the new content rather than replace the content wholesale */
	FSlotBase* InnerContentSlot;

	/** The widget in the content slot for this row */
	TWeakPtr<SWidget> Content;

	bool bChangedSelectionOnMouseDown;

	/** Did the current a touch interaction start in this item?*/
	bool bProcessingSelectionTouch;
};