#pragma once

#include "CoreMinimal.h"

class UWidget;
class UWidgetBlueprint;

struct FCodexInvenWidgetBlueprintTreeNode
{
	FName WidgetName;
	FString WidgetClassName;
	TArray<FCodexInvenWidgetBlueprintTreeNode> Children;
};

class FCodexInvenWidgetBlueprintTreeUtils final
{
public:
	static UWidgetBlueprint* LoadWidgetBlueprint(const FString& InPackagePath);
	static UWidget* FindWidget(const UWidgetBlueprint& InWidgetBlueprint, FName InWidgetName);
	static bool BuildWidgetTreeDescription(const UWidgetBlueprint& InWidgetBlueprint, FCodexInvenWidgetBlueprintTreeNode& OutRoot, FString& OutError);
	static FString FormatWidgetTree(const FCodexInvenWidgetBlueprintTreeNode& InRoot);

private:
	static void BuildWidgetTreeNode(const UWidget& InWidget, FCodexInvenWidgetBlueprintTreeNode& OutNode);
	static void GatherChildWidgets(const UWidget& InWidget, TArray<const UWidget*>& OutChildren);
	static void AppendFormattedNode(const FCodexInvenWidgetBlueprintTreeNode& InNode, int32 InDepth, FString& OutText);
};
