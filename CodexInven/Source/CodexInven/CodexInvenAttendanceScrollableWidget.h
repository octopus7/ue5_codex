#pragma once

#include "CodexInvenAttendanceWidgetBase.h"
#include "CodexInvenAttendanceScrollableWidget.generated.h"

UCLASS()
class CODEXINVEN_API UCodexInvenAttendanceScrollableWidget : public UCodexInvenAttendanceWidgetBase
{
	GENERATED_BODY()

public:
	UCodexInvenAttendanceScrollableWidget();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

private:
	class UScrollBox* GetDayScrollBox() const;
	bool IsPointerOverDayScrollBox(const FVector2D& InScreenSpacePosition) const;
	float ClampScrollOffset(float InScrollOffset) const;
	void ResetScrollDragState();

	UPROPERTY(EditDefaultsOnly, Category = "Attendance|Input", meta = (ClampMin = "1.0"))
	float MouseWheelScrollAmount = 120.0f;

	bool bIsDraggingScroll = false;
	FVector2D DragStartScreenPosition = FVector2D::ZeroVector;
	float DragStartScrollOffset = 0.0f;
};
