/// MIT License, Copyright Burak Kara, burak@burak.io, https://en.wikipedia.org/wiki/MIT_License

#include "BTreeViewWidgetStyle.h"

const FName FBTreeViewStyle::TypeName = TEXT("FBTreeViewStyle");

const FTableRowStyle& FBTreeViewStyle::GetNoHoverTableRowStyle()
{
	static FTableRowStyle Default;

#if WITH_EDITOR
	return FEditorStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.NoHoverTableRow");
#else
	return Default;
#endif
}

const FBTreeViewStyle& FBTreeViewStyle::GetDefault()
{
	static FBTreeViewStyle Default;
	return Default;
}

void FBTreeViewStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const {}

UBTreeViewWidgetStyle::UBTreeViewWidgetStyle() {}