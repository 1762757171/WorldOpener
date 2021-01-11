#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STableRow.h"
#include "WorldOpenerStruct.h"

class SWoRow : public SMultiColumnTableRow<FWoRowBase::Ptr>
{
public:

    SLATE_BEGIN_ARGS(SWoRow) {}

        SLATE_ARGUMENT(FWoRowBase::Ptr, RowDataPtr)

        SLATE_EVENT(FSimpleDelegate, OnDirty)
    
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    
    FWoRowBase::Ptr Data;

    FSimpleDelegate OnDirty;

    FKey Pressing;
    
public:

    static FName Column_Load;
    static FName Column_Level;
    
};