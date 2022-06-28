/// MIT License, Copyright Burak Kara, burak@burak.io, https://en.wikipedia.org/wiki/MIT_License

#pragma once

class FBTreeViewStyles
{

public:
	static void Initialize();

	static void Shutdown();

	static const class ISlateStyle& Get();

	static FName GetStyleSetName();
	static void SetExpandedArrowStyle(const struct FBExpandedArrowStyle* style);

private:
	// Creates the Style Set.
	static TSharedRef<class FSlateStyleSet> Create();

	// Singleton instance used for our Style Set.
	static TSharedPtr<class FSlateStyleSet> TreeViewStyleInstance;
	
	static FString InContent(const FString& RelativePath, const ANSICHAR* Extension);
};