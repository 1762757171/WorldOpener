#include "SWoRow.h"
#include "WorldOpener.h"
#include "WorldOpenerSettings.h"
#include "WorldOpenerUtils.h"

#define LOCTEXT_NAMESPACE "FWorldOpenerModule"

FName SWoRow::Column_Load("Column_Load");
FName SWoRow::Column_Level("Column_Level");

void SWoRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
    Data = InArgs._RowDataPtr;
    OnDirty = InArgs._OnDirty;
    
    FSuperRowType::Construct(
      FSuperRowType::FArguments()
          .Style(FEditorStyle::Get(), "SceneOutliner.TableViewRow"),

      InOwnerTableView
    );
}

void SWoRow::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    STableRow::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    
    if(Data->IsDirty())
    {
        (void)OnDirty.ExecuteIfBound();
    }
}

TSharedRef<SWidget> SWoRow::GenerateWidgetForColumn(const FName& ColumnName)
{
    if(ColumnName == Column_Load)
    {
        return
            SNew(SCheckBox)
                .ToolTipText_Lambda
                (
                    [this]
                    {
                        return Data->GetCheckboxTooltip();
                    }
                )
                .IsChecked_Lambda
                (
                    [this]
                    {
                        return Data->GetCheckState();
                    }
                )
                .OnCheckStateChanged_Lambda
                (
                    [this](ECheckBoxState NewState)
                    {
                        UWorld* PersistentWorld = GEditor->GetEditorWorldContext().World();
                        WoUtils::FAvoidModifyHelper NoModifyWorld(PersistentWorld);

                        TArray<ULevelStreaming*> Added;
                        Data->ChangeCheckState(NewState, Added);
                        
                        WoUtils::FlushWorld(Added);
                        
                    }
                )
        ;
    }
    if(ColumnName == Column_Level)
    {
        return
			SNew(SHorizontalBox)
            .ToolTipText_Lambda
            (
                [this]
                {
                    return Data->GetElementTooltip();
                }
            )
            +SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SExpanderArrow, SharedThis(this))
                    .ShouldDrawWires_Lambda
                    (
                        []
                        {
                            return GetMutableDefault<UWoViewConfig>()->bDrawLine;
                        }
                    )
            ]
            +SHorizontalBox::Slot()
            .FillWidth(1.f)
            [
                Data->GetRowWidget
                    (
                    [this]()
                        {
                            return
                            StaticCastSharedPtr<STreeView<FWoRowBase::Ptr>>( OwnerTablePtr.Pin() )->IsItemExpanded(Data);
                        }
                    )
            ]
        ;
    }
    return SNullWidget::NullWidget;
}

FReply SWoRow::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
    if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
        if(Data->OnDoubleClick())
            return FReply::Handled();

    return STableRow::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
}

FReply SWoRow::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if(MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        Pressing = EKeys::RightMouseButton;
        return FReply::Handled();
    }
    return STableRow::OnMouseButtonDown(MyGeometry, MouseEvent);
}

FReply SWoRow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if(Pressing == EKeys::RightMouseButton && MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        Pressing = EKeys::Invalid;

        // Summon context menu
        FWorldOpenerModule& WoModule = FModuleManager::GetModuleChecked<FWorldOpenerModule>("WorldOpener");
        FMenuBuilder MenuBuilder(true, WoModule.GetCommandList());
        TArray<ULevelStreaming*> Levels;
        Data->GetLevels(Levels);
        WoModule.OnBuildMenu.Broadcast(MenuBuilder, Levels, Data->IsFolder());
        const TSharedPtr<SWidget> MenuWidget = MenuBuilder.MakeWidget();

        const FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();

        FSlateApplication::Get().PushMenu(
            AsShared(),
            WidgetPath,
            MenuWidget.ToSharedRef(),
            MouseEvent.GetScreenSpacePosition(),
            FPopupTransitionEffect( FPopupTransitionEffect::ContextMenu )
            );

    }
    return STableRow::OnMouseButtonUp(MyGeometry, MouseEvent);
}

#undef LOCTEXT_NAMESPACE
