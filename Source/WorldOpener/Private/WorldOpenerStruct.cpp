#include "WorldOpenerStruct.h"

#include "EditorLevelUtils.h"
#include "WorldOpenerUtils.h"

#define LOCTEXT_NAMESPACE "FWorldOpenerModule"

void FWoRow_Level::SetStreamingLevel(ULevelStreaming* InLevelStreaming)
{
    LevelStreaming = InLevelStreaming;
    PrevFolder = LevelStreaming->GetFolderPath();
}

TSharedRef<SWidget> FWoRow_Level::GetRowWidget(TFunction<bool()>&& IsExpand) const
{
    return
    SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SImage)
                .Image_Lambda
                (
                    [this]
                    {
                        UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
                        const bool bEditing = EditorWorld->GetCurrentLevel() == LevelStreaming->GetLoadedLevel();
                        if(bEditing)
                            return FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.EditorModes").GetSmallIcon();
                        return FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.WorldProperties.Tab").GetIcon();
                    }
                )
        ]
        +SHorizontalBox::Slot()
        .FillWidth(1.f)
        [
            SNew(STextBlock)
                // .TextStyle(FEditorStyle::Get(), "ContentBrowser.PathText")
                .Text(FText::FromString(FPaths::GetBaseFilename( LevelStreaming->GetWorldAssetPackageName() )))
                .HighlightText_Lambda
                (
                    [this]
                    {
                        return Filter;
                    }
                )
        ]
    ;
}

ECheckBoxState FWoRow_Level::GetCheckState() const
{
    if (!LevelStreaming) return ECheckBoxState::Undetermined;
    return LevelStreaming->IsLevelLoaded() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void FWoRow_Level::ChangeCheckState(ECheckBoxState NewState, TArray<ULevelStreaming*>& OutAdded)
{
    switch (NewState)
    {
    case ECheckBoxState::Unchecked:
        {
            WoUtils::UnloadLevel(LevelStreaming);
        }
        break;
    case ECheckBoxState::Checked:
        {
            WoUtils::LoadLevel(LevelStreaming);
            OutAdded.Add(LevelStreaming);
        }
        break;
    default:
        break;
    }
}

void FWoRow_Level::GetChildren(TArray<FWoRowBase::Ptr>& OutChildren, TFunction<bool(FWoRowBase::Ptr)>&& FilterMethod) const
{
    OutChildren.Empty();
}

void FWoRow_Level::GetChildrenRecursive(TArray<FWoRowBase::Ptr>& OutChildren, TFunction<bool(FWoRowBase::Ptr)>&& FilterMethod) const
{
}

bool FWoRow_Level::OnDoubleClick()
{
    TArray<ULevelStreaming*> Added;
    ChangeCheckState(ECheckBoxState::Checked, Added);
    WoUtils::FlushWorld(Added);
    WoUtils::MakeCurrent(LevelStreaming);
    return true;
}

bool FWoRow_Level::IsDirty()
{
    FName Folder = LevelStreaming->GetFolderPath();
    if(Folder != PrevFolder)
    {
        Folder = PrevFolder;
        return true;
    }
    return false;
}

FString FWoRow_Level::GetNameForSort() const
{
    static const FString Prefix(TEXT("World_"));
    return Prefix + FPaths::GetBaseFilename( LevelStreaming->GetWorldAssetPackageName() );
}

FText FWoRow_Level::GetCheckboxTooltip() const
{
    const FText Format(LOCTEXT("Format_CheckboxTooltip_Level", "{0} level [{1}]."));
    FText LoadText(LOCTEXT("Label_Load", "Load"));
    FText UnloadText(LOCTEXT("Label_Unload", "Unload"));
    FText LevelPath = FText::FromString( LevelStreaming->GetWorldAssetPackageName() );
    return FText::FormatOrdered(Format, GetCheckState() == ECheckBoxState::Checked ? UnloadText : LoadText, LevelPath);
}

FText FWoRow_Level::GetElementTooltip() const
{
    const FText Format(LOCTEXT("Format_ElementTooltip_Level", "Double click to load level [{0}] and make it current level."));
    FText LevelPath = FText::FromString( LevelStreaming->GetWorldAssetPackageName() );
    return FText::FormatOrdered(Format, LevelPath);
}

void FWoRow_Level::SetFilterText(const FText& InText)
{
    Filter = InText;
}

bool FWoRow_Level::ShouldShow() const
{
    if(Filter.IsEmpty())
        return true;
    return LevelStreaming->GetWorldAssetPackageName().Contains(Filter.ToString());
}

void FWoRow_Level::GetLevels(TArray<ULevelStreaming*>& OutLevels)
{
    OutLevels.Add(LevelStreaming);
}

int32 FWoRow_Level::GetLevelCount() const
{
    return 1;
}

TSharedRef<SWidget> FWoRow_Folder::GetRowWidget(TFunction<bool()>&& IsExpand) const
{
    return
    SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            // SNew(STextBlock)
            //     .TextStyle(FEditorStyle::Get(), "ContentBrowser.TopBar.Font")
            //     .Font(FEditorStyle::Get().GetFontStyle("FontAwesome.11"))
            //     .Text(FText::FromString(FString(TEXT("\xf07c"))))
            SNew(SImage)
                .Image_Lambda
                (
                    [IsExpand]
                    {
                        return
                        FEditorStyle::Get().GetBrush(IsExpand() ?
                            "WorldBrowser.FolderOpen" :
                            "WorldBrowser.FolderClosed");
                    }
                )
        ]
        +SHorizontalBox::Slot()
        .FillWidth(1.f)
        [
            SNew(STextBlock)
                // .TextStyle(FEditorStyle::Get(), "ContentBrowser.PathText")
                .Text(FText::FromString(FPaths::GetPathLeaf( FolderName )))
                .HighlightText_Lambda
                (
                    [this]
                    {
                        return Filter;
                    }
                )
        ]
    ;
}

ECheckBoxState FWoRow_Folder::GetCheckState() const
{
    const int32 Count = Content.Num();
    int32 Loaded = 0;

    for (const auto& l : Content)
    {
        const ECheckBoxState State = l->GetCheckState();
        if(State == ECheckBoxState::Undetermined)
            return ECheckBoxState::Undetermined;
        if (State == ECheckBoxState::Checked)
            ++Loaded;
    }

    if (Loaded == Count)
        return ECheckBoxState::Checked;
    if (Loaded == 0)
        return ECheckBoxState::Unchecked;
    return ECheckBoxState::Undetermined;
}

void FWoRow_Folder::ChangeCheckState(ECheckBoxState NewState, TArray<ULevelStreaming*>& OutAdded)
{
    if (NewState == ECheckBoxState::Undetermined) return;
    
    if(NewState == ECheckBoxState::Checked)
    {
        const int LevelCount = GetLevelCount();
        if(LevelCount > 20)
        {
            const FText Format = LOCTEXT("Prompt_ManyLevels", "You are loading {0} levels. Are you sure?");
            FText Text = FText::FormatOrdered(Format, LevelCount);
            if(EAppReturnType::Ok != FMessageDialog::Open(EAppMsgType::OkCancel, Text))
                return;
        }
    }

    for (const auto& l : Content)
        l->ChangeCheckState(NewState, OutAdded);
}

void FWoRow_Folder::GetChildren(TArray<FWoRowBase::Ptr>& OutChildren, TFunction<bool(FWoRowBase::Ptr)>&& FilterMethod) const
{
    OutChildren = Content;
    OutChildren.RemoveAll(FilterMethod);
}

void FWoRow_Folder::GetChildrenRecursive(TArray<FWoRowBase::Ptr>& OutChildren, TFunction<bool(FWoRowBase::Ptr)>&& FilterMethod) const
{
    for(const auto& c : Content)
    {
        OutChildren.Add(c);
        c->GetChildrenRecursive(OutChildren, [](FWoRowBase::Ptr){return false;});
    }

    OutChildren.RemoveAll(FilterMethod);
}

bool FWoRow_Folder::IsFolderWithName(const FString& InName)
{
    return FolderName == InName;
}

bool FWoRow_Folder::IsAnySameFolderNameInSet(const TSet<FWoRowBase::Ptr>& InSet) const
{
    for(const auto& s : InSet)
        if(s->IsFolderWithName(FolderName))
            return true;

    return false;
}

FString FWoRow_Folder::GetNameForSort() const
{
    static const FString Prefix(TEXT("Folder_"));
    return Prefix + FolderName;
}

void FWoRow_Folder::Sort()
{
    Content.Sort
    (
        [](const FWoRowBase::Ptr& lhs, const FWoRowBase::Ptr& rhs)
        {
            return lhs->GetNameForSort() < rhs->GetNameForSort();
        }
    );
    for(auto c : Content)
    {
        c->Sort();
    }
}

FText FWoRow_Folder::GetCheckboxTooltip() const
{
    const FText Format(LOCTEXT("Format_CheckboxTooltip_Folder", "{0} all levels under folder [{1}]."));
    FText LoadText(LOCTEXT("Label_Load", "Load"));
    FText UnloadText(LOCTEXT("Label_Unload", "Unload"));
    FText FolderPath = FText::FromString( FolderName );
    return FText::FormatOrdered(Format, GetCheckState() == ECheckBoxState::Checked ? UnloadText : LoadText, FolderPath);
}

FText FWoRow_Folder::GetElementTooltip() const
{
    const FText Format(LOCTEXT("Format_ElementTooltip_Folder", "Double click to expand folder [{0}]."));
    FText LevelPath = FText::FromString( FolderName );
    return FText::FormatOrdered(Format, LevelPath);
}

void FWoRow_Folder::SetFilterText(const FText& InText)
{
    Filter = InText;
    for(const auto& c : Content)
        c->SetFilterText(InText);
}

bool FWoRow_Folder::ShouldShow() const
{
    if(Filter.IsEmpty())
        return true;
    
    if(FolderName.Contains(Filter.ToString()))
        return true;
    
    for(const auto& c : Content)
        if(c->ShouldShow())
            return true;
    return false;
}

void FWoRow_Folder::GetLevels(TArray<ULevelStreaming*>& OutLevels)
{
    for(const auto& c : Content)
    {
        c->GetLevels(OutLevels);
    }
}

int32 FWoRow_Folder::GetLevelCount() const
{
    int32 LevelCnt = 0;
    for(const auto& c : Content)
        LevelCnt += c->GetLevelCount();
    return LevelCnt;
}

#undef LOCTEXT_NAMESPACE
