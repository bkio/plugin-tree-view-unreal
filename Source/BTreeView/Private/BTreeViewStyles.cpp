/// MIT License, Copyright Burak Kara, burak@burak.io, https://en.wikipedia.org/wiki/MIT_License

#include "BTreeViewStyles.h"
#include "Interfaces/IPluginManager.h"
#include "BCustomTreeView.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FBTreeViewStyles::InContent( RelativePath, ".png" ), __VA_ARGS__ )

FString FBTreeViewStyles::InContent(const FString& RelativePath, const ANSICHAR* Extension)
{
	FString Dir = IPluginManager::Get().FindPlugin(TEXT("TreeViewControl"))->GetBaseDir();
	FString Path = (Dir / RelativePath) + Extension;
	return Path;
}

TSharedPtr<FSlateStyleSet> FBTreeViewStyles::TreeViewStyleInstance = NULL;

void FBTreeViewStyles::Initialize()
{
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon10x10(10.0f, 10.0f);

	if (!TreeViewStyleInstance.IsValid())
	{
		TreeViewStyleInstance = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	}
	else
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*TreeViewStyleInstance);
	}

	TreeViewStyleInstance->Set("ClassIcon.TreeView", new IMAGE_BRUSH("Resources/TreeViewWidgetIcon", Icon10x10));

	FSlateStyleRegistry::RegisterSlateStyle(*TreeViewStyleInstance);
	FTableRowStyle NormalTableRowStyle;

	TreeViewStyleInstance->Set("TableView.DisabledView", FTableRowStyle(NormalTableRowStyle));
}

void FBTreeViewStyles::Shutdown()
{
	if (TreeViewStyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*TreeViewStyleInstance);
		ensure(TreeViewStyleInstance.IsUnique());
		TreeViewStyleInstance.Reset();
	}
}

FName FBTreeViewStyles::GetStyleSetName()
{
	static FName StyleSetName(TEXT("TreeViewStyles"));
	return StyleSetName;
}

const ISlateStyle& FBTreeViewStyles::Get()
{
	Initialize();
	return *TreeViewStyleInstance;
}