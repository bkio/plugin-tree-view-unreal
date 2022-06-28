/// MIT License, Copyright Burak Kara, burak@burak.io, https://en.wikipedia.org/wiki/MIT_License

#pragma once

#include "SBCustomTreeView.h"
#include "BTreeViewWidgetStyle.h"
#include "UMGStyle.h"
#include "Blueprint/UserWidget.h"
#include "BCustomTreeView.generated.h"

USTRUCT(BlueprintType)
struct FBIconStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	UTexture2D* IconTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FVector2D IconSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FLinearColor IconColor;

	FBIconStyle()
	{
		IconSize = FVector2D(10.0f, 10.0f);
		IconColor = FLinearColor(0.72f, 0.72f, 0.72f, 1);
	}
};

USTRUCT(BlueprintType)
struct FBExpandedArrowStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FSlateBrush CollapsedStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FSlateBrush CollapsedHoveredStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FSlateBrush ExpandedStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FSlateBrush ExpandedHoveredStyle;
};

USTRUCT(BlueprintType)
struct FBTreeNode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "BCustomTreeNode")
	int32 NodeID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BCustomTreeNode")
	FString NodeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BCustomTreeNode")
	int32 ParentID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BCustomTreeNode")
	FMargin NodePadding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BCustomTreeNode")
	TArray<FString> ExtraStrings;		
};

USTRUCT(BlueprintType)
struct FBRowContentTypeByParent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	int32 ParentID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	TSubclassOf<class UUserWidget> RowContent;
};

USTRUCT(BlueprintType)
struct FBRowContentTypeById
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	int32 NodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	TSubclassOf<class UUserWidget> RowContent;
};

UCLASS(BlueprintType)
class UBCustomTreeView : public UWidget
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGenerateRowEvent, const FBTreeNode&, Row , class UUserWidget* ,  RowWidget, const TArray<FBTreeNode>&, Children);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelectionChangedEvent, const FBTreeNode&, Item, class UUserWidget*, RowWidget);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectionLostEvent);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnExpansionChangedEvent, const FBTreeNode&, Item, class UUserWidget*, RowWidget, const bool, ExpansionState);

	UBCustomTreeView();
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
	FGenerateRowEvent OnGenerateRow;

	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
	FOnSelectionChangedEvent OnSelectionChanged;

	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
	FOnSelectionLostEvent OnSelectionLost;
	
	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
	FOnExpansionChangedEvent OnExpansionChanged;

	UPROPERTY(EditAnyWhere , BlueprintReadWrite, Category = "TreeView")
	TArray<FBTreeNode> TreeNodes;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Style")
	FBTreeViewStyle TreeViewStyle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FBExpandedArrowStyle ArrowStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FMargin RowDefaultPadding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	bool ExpanderVisibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
	TArray<FBRowContentTypeByParent> RowContentsByParent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
	TArray<FBRowContentTypeById> RowContentsById;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
	TSubclassOf<class UUserWidget> DefaultRowContent;

	UFUNCTION(BlueprintCallable, Category = "TreeView")
	void CreateTree();

	UFUNCTION(BlueprintCallable, Category = "TreeView")
	void ExpandTreeItem(int32 NodeId);

	UFUNCTION(BlueprintCallable, Category = "TreeView")
	void CollapseTreeItem(int32 NodeId);

	UFUNCTION(BlueprintCallable, Category = "TreeView")
	void ToggleNodeExpansion(int32 NodeId);

	UFUNCTION(BlueprintCallable, Category = "TreeView")
	void SelectTreeItem(int32 NodeId);

	int32 GetRootIndex(int32 nodeindex);

	void HandleOnGenerateRow(TreeNodePtr Item, class UUserWidget* RowWidget, TArray<TreeNodePtr> Children);
	void HandleOnSelectionChanged(TreeNodePtr Item, class UUserWidget* RowWidget);
	void HandleOnSelectionLost();
	void HandleOnExpansionChanged(TreeNodePtr Item, class UUserWidget* RowWidget, bool ExpansionState);

protected:
	TSharedPtr<SBCustomTreeView > TreeViewWidget;
	TArray<TreeNodePtr> TreeStructure;
	TArray<TreeNodePtr> TempStructure;

	virtual TSharedRef<SWidget> RebuildWidget() override;

	void EnsureWidgetValidity();
};