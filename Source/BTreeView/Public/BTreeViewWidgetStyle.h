/// MIT License, Copyright Burak Kara, burak@burak.io, https://en.wikipedia.org/wiki/MIT_License

#pragma once

#include "SlateBasics.h"
#include "Styling/SlateTypes.h"
#include "Styling/SlateWidgetStyle.h"
#if WITH_EDITOR
#include "Editor/EditorStyle/Public/EditorStyle.h"
#endif
#include "BTreeViewWidgetStyle.generated.h"

USTRUCT(BlueprintType)
struct FBTreeViewStyle : public FSlateWidgetStyle
{
	GENERATED_BODY()

	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;

	static const FName TypeName;
	static const FTableRowStyle& GetNoHoverTableRowStyle();
	static const FBTreeViewStyle& GetDefault();

	UPROPERTY(EditAnywhere, Category = Appearance)
	bool EnableTableRowStyle;

	UPROPERTY(EditAnywhere, Category = Appearance , meta = (EditCondition = "EnableTableRowStyle"))
	FTableRowStyle TableRowStyle; 

	UPROPERTY(EditAnywhere, Category = Appearance)
	FTextBlockStyle RowTextStyle;

	UPROPERTY(EditAnywhere, Category = Appearance)
	FMargin TextPadding;

	UPROPERTY(EditAnywhere, Category = Appearance)
	FMargin TreeViewPadding;

	UPROPERTY(EditAnywhere, Category = Appearance)
	FScrollBarStyle VerticalScrollBarStyle;

	UPROPERTY(EditAnywhere, Category = Appearance)
	FVector2D VerticalScrollBarThickness;

	UPROPERTY(EditAnywhere, Category = Appearance)
	FScrollBoxStyle ScrollBoxStyle;

	FBTreeViewStyle()
	{
		TableRowStyle = FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");
		RowTextStyle = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
		VerticalScrollBarStyle = FCoreStyle::Get().GetWidgetStyle<FScrollBarStyle>("Scrollbar");
		EnableTableRowStyle = true;
		FSlateBrush DefaultScrollboxBrush = FSlateBrush();
		DefaultScrollboxBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
		ScrollBoxStyle = FScrollBoxStyle().SetTopShadowBrush(DefaultScrollboxBrush).SetBottomShadowBrush(DefaultScrollboxBrush).SetLeftShadowBrush(DefaultScrollboxBrush).SetRightShadowBrush(DefaultScrollboxBrush);
	}
};

UCLASS(BlueprintType, hidecategories = Object, MinimalAPI)
class UBTreeViewWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()
	
public:
	UBTreeViewWidgetStyle();

	UPROPERTY(Category = Appearance, EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
	FBTreeViewStyle TreeViewStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&TreeViewStyle);
	}
};