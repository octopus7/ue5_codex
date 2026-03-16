// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenInventoryTileView.h"

#include "CodexInvenInventoryTileEntryWidget.h"

UCodexInvenInventoryTileView::UCodexInvenInventoryTileView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EntryWidgetClass = UCodexInvenInventoryTileEntryWidget::StaticClass();
}
