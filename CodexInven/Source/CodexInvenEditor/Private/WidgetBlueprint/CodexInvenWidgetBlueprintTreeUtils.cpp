#include "WidgetBlueprint/CodexInvenWidgetBlueprintTreeUtils.h"

#include "Blueprint/WidgetTree.h"
#include "Components/NamedSlotInterface.h"
#include "Components/PanelWidget.h"
#include "Misc/PackageName.h"
#include "WidgetBlueprint.h"

namespace
{
	FString MakeWidgetBlueprintObjectPath(const FString& InPackagePath)
	{
		if (InPackagePath.Contains(TEXT(".")))
		{
			return InPackagePath;
		}

		return InPackagePath + TEXT(".") + FPackageName::GetLongPackageAssetName(InPackagePath);
	}
}

UWidgetBlueprint* FCodexInvenWidgetBlueprintTreeUtils::LoadWidgetBlueprint(const FString& InPackagePath)
{
	return LoadObject<UWidgetBlueprint>(nullptr, *MakeWidgetBlueprintObjectPath(InPackagePath));
}

UWidget* FCodexInvenWidgetBlueprintTreeUtils::FindWidget(const UWidgetBlueprint& InWidgetBlueprint, const FName InWidgetName)
{
	return InWidgetBlueprint.WidgetTree != nullptr ? InWidgetBlueprint.WidgetTree->FindWidget(InWidgetName) : nullptr;
}

bool FCodexInvenWidgetBlueprintTreeUtils::BuildWidgetTreeDescription(
	const UWidgetBlueprint& InWidgetBlueprint,
	FCodexInvenWidgetBlueprintTreeNode& OutRoot,
	FString& OutError)
{
	if (InWidgetBlueprint.WidgetTree == nullptr)
	{
		OutError = FString::Printf(TEXT("Widget blueprint %s has no widget tree."), *InWidgetBlueprint.GetName());
		return false;
	}

	if (InWidgetBlueprint.WidgetTree->RootWidget == nullptr)
	{
		OutError = FString::Printf(TEXT("Widget blueprint %s has an empty widget tree."), *InWidgetBlueprint.GetName());
		return false;
	}

	BuildWidgetTreeNode(*InWidgetBlueprint.WidgetTree->RootWidget, OutRoot);
	return true;
}

FString FCodexInvenWidgetBlueprintTreeUtils::FormatWidgetTree(const FCodexInvenWidgetBlueprintTreeNode& InRoot)
{
	FString FormattedTree;
	AppendFormattedNode(InRoot, 0, FormattedTree);
	return FormattedTree;
}

void FCodexInvenWidgetBlueprintTreeUtils::BuildWidgetTreeNode(
	const UWidget& InWidget,
	FCodexInvenWidgetBlueprintTreeNode& OutNode)
{
	OutNode.WidgetName = InWidget.GetFName();
	OutNode.WidgetClassName = InWidget.GetClass()->GetName();
	OutNode.Children.Reset();

	TArray<const UWidget*> Children;
	GatherChildWidgets(InWidget, Children);
	OutNode.Children.Reserve(Children.Num());

	for (const UWidget* ChildWidget : Children)
	{
		if (ChildWidget == nullptr)
		{
			continue;
		}

		FCodexInvenWidgetBlueprintTreeNode& ChildNode = OutNode.Children.AddDefaulted_GetRef();
		BuildWidgetTreeNode(*ChildWidget, ChildNode);
	}
}

void FCodexInvenWidgetBlueprintTreeUtils::GatherChildWidgets(const UWidget& InWidget, TArray<const UWidget*>& OutChildren)
{
	TSet<const UWidget*> SeenChildren;

	if (const UPanelWidget* PanelWidget = Cast<UPanelWidget>(&InWidget))
	{
		for (int32 ChildIndex = 0; ChildIndex < PanelWidget->GetChildrenCount(); ++ChildIndex)
		{
			if (const UWidget* ChildWidget = PanelWidget->GetChildAt(ChildIndex))
			{
				if (!SeenChildren.Contains(ChildWidget))
				{
					SeenChildren.Add(ChildWidget);
					OutChildren.Add(ChildWidget);
				}
			}
		}
	}

	if (const INamedSlotInterface* NamedSlotHost = Cast<INamedSlotInterface>(const_cast<UWidget*>(&InWidget)))
	{
		TArray<FName> SlotNames;
		NamedSlotHost->GetSlotNames(SlotNames);

		for (const FName SlotName : SlotNames)
		{
			if (const UWidget* SlotContent = NamedSlotHost->GetContentForSlot(SlotName))
			{
				if (!SeenChildren.Contains(SlotContent))
				{
					SeenChildren.Add(SlotContent);
					OutChildren.Add(SlotContent);
				}
			}
		}
	}
}

void FCodexInvenWidgetBlueprintTreeUtils::AppendFormattedNode(
	const FCodexInvenWidgetBlueprintTreeNode& InNode,
	const int32 InDepth,
	FString& OutText)
{
	OutText += FString::ChrN(InDepth * 2, TEXT(' '));
	OutText += FString::Printf(TEXT("%s [%s]\n"), *InNode.WidgetName.ToString(), *InNode.WidgetClassName);

	for (const FCodexInvenWidgetBlueprintTreeNode& ChildNode : InNode.Children)
	{
		AppendFormattedNode(ChildNode, InDepth + 1, OutText);
	}
}
