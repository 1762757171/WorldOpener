#include "SWorldOpener.h"
#include "SWoRow.h"
#include "WorldOpenerSettings.h"
#include "Widgets/Input/SSearchBox.h"

#define LOCTEXT_NAMESPACE "FWorldOpenerModule"

void SWorldOpener::Construct(const FArguments& InArgs)
{
    const TSharedRef<SScrollBar> VerticalScrollBar =
        SNew(SScrollBar)
            .Orientation(Orient_Vertical)
            .Thickness(FVector2D(12.0f, 12.0f))
    ;

    auto ReconstructLambda =
        [this](ULevel*, UWorld*)
        {
            Reconstruct();
        };
    
    Dh_Add = FWorldDelegates::LevelAddedToWorld.AddLambda(ReconstructLambda);
    Dh_Remove = FWorldDelegates::LevelRemovedFromWorld.AddLambda(ReconstructLambda);
    Dh_Change = FEditorDelegates::OnMapOpened.AddLambda
    (
        [this](const FString&, bool)
        {
            Reconstruct();
        }
    );
    

    //Construct level tree here
    ConstructTree();
    
    SAssignNew(Tree, STreeView<FWoRowBase::Ptr>)
        .TreeItemsSource(&Root)
        .HighlightParentNodesForSelection(true)
        .HeaderRow
        (
            SNew(SHeaderRow)
                +SHeaderRow::Column(SWoRow::Column_Load)
                    .DefaultLabel(LOCTEXT("LoadLable", ""))
                    .ManualWidth(16.0f)
                +SHeaderRow::Column(SWoRow::Column_Level)
                    .DefaultLabel(LOCTEXT("LevelLable", "Level name"))
                    .FillWidth(1.0f)
        )
        .OnGenerateRow_Lambda
        (
            [this](FWoRowBase::Ptr InRowDataPtr, const TSharedRef<STableViewBase>& OwnerTable)
            {
                return
                    SNew(SWoRow, OwnerTable)
                        .RowDataPtr(InRowDataPtr)
                        .OnDirty_Raw(this, &SWorldOpener::Reconstruct)
                ;
            }
        )
        .OnGetChildren_Lambda
        (
            [](FWoRowBase::Ptr Item, TArray<FWoRowBase::Ptr>& OutChildren)
            {
                Item->GetChildren(OutChildren);
            }
        )
        .ExternalScrollbar(VerticalScrollBar)
        .ConsumeMouseWheel(EConsumeMouseWheel::Always)
        .SelectionMode(ESelectionMode::Single)
        .AllowOverscroll(EAllowOverscroll::No)
    ;
    
    this->ChildSlot
    [
        SNew(SVerticalBox)
        +SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Fill)
        [
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
            .FillWidth(1.f)
            [
                SAssignNew(SearchBox, SSearchBox)
                    .OnTextChanged_Raw(this, &SWorldOpener::OnSearch)
            ]
            +SHorizontalBox::Slot()
            .AutoWidth()
            [
                SAssignNew(ViewConfigComboButton, SComboButton)
                    .OnGetMenuContent_Lambda
                    (
                        []
                        {
                            FMenuBuilder MenuBuilder(false, NULL);

                            MenuBuilder.BeginSection("ViewConfig", LOCTEXT("ShowHeading", "Show"));
                            {
                                MenuBuilder.AddMenuEntry(
                                    LOCTEXT("Config_ShowLine", "Show lines"),
                                    LOCTEXT("Config_ShowLine_Tooltip", "When enabled, display lines for clearer indentation."),
                                    FSlateIcon(),
                                    FUIAction(
                                    FExecuteAction::CreateLambda
                                    (
                                        []
                                        {
                                            UWoViewConfig* Cfg = GetMutableDefault<UWoViewConfig>();
                                            Cfg->bDrawLine = !Cfg->bDrawLine;
                                            Cfg->SaveConfig();
                                            
                                        }
                                    ),
                                    FCanExecuteAction(),
                                    FIsActionChecked::CreateLambda
                                    (
                                        []
                                        {
                                            return GetMutableDefault<UWoViewConfig>()->bDrawLine;
                                        }
                                    )
                                    ),
                                    NAME_None,
                                    EUserInterfaceActionType::ToggleButton
                                );
                            }
                            MenuBuilder.EndSection();

                            return MenuBuilder.MakeWidget();
                        }
                    )
                    .HAlign(HAlign_Center)
                    .HasDownArrow(false)
					.ButtonStyle( FEditorStyle::Get(), "ToggleButton" ) // Use the tool bar item style for this button
					.ForegroundColor_Lambda
					(
                        [this]
                        {
                            return
                            ViewConfigComboButton->IsHovered() ?
                                FEditorStyle::GetSlateColor("InvertedForeground") :
                                FEditorStyle::GetSlateColor("DefaultForeground")
                            ;
                        }
					)
                    .ButtonContent()
                    [
                        SNew(SImage)
                            .Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "SourceControl.Actions.ChangeSettings").GetIcon())
                    ]
                    .ToolTipText(LOCTEXT("Tooltip_Config", "View Options."))
            ]
        ]
        +SVerticalBox::Slot()
        .FillHeight(1.0f)
        [
            SNew(SHorizontalBox)
                +SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    Tree.ToSharedRef()
                ]
                +SHorizontalBox::Slot()
                .AutoWidth()
                [
                    VerticalScrollBar
                ]
        ]
    ];
}

SWorldOpener::~SWorldOpener()
{
    FWorldDelegates::LevelAddedToWorld.Remove(Dh_Add);
    FWorldDelegates::LevelRemovedFromWorld.Remove(Dh_Remove);
    FEditorDelegates::OnMapOpened.Remove(Dh_Change);
}

void SWorldOpener::ConstructTree()
{
    UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();

    FWoRow_Folder::Ptr TheRoot = MakeShareable(new FWoRow_Folder);
    TheRoot->FolderName = EditorWorld->GetMapName();
    Root = { TheRoot };

    for(const auto& l : EditorWorld->GetStreamingLevels())
    {
        // const FString& AssetName = l->GetWorldAssetPackageName();
        const FString& Path = l->GetFolderPath().ToString();
        TArray<FString> Folders;
        Path.ParseIntoArray(Folders, TEXT("/"));
        Folders.RemoveAll
        (
            [](const FString& item)
            {
                return item == TEXT("None");
            }
        );
        if(Folders.Num() != 0)
        {
            Folders[0] = TheRoot->FolderName / Folders[0];
            for(int32 i = 1; i < Folders.Num(); ++i)
            {
                Folders[i] = Folders[i - 1] / Folders[i] / FString();
            }
        }
        FWoRow_Folder::Ptr CurrentFolder = TheRoot;
        
        for(const auto& f : Folders)
        {
            FWoRowBase::Ptr* pFound =
            CurrentFolder->Content.FindByPredicate
            (
                [f](const FWoRowBase::Ptr& item)
                {
                    return item->IsFolderWithName(f);
                }
            );
            if(pFound)
            {
                CurrentFolder = StaticCastSharedPtr<FWoRow_Folder>( *pFound );
            }
            else
            {
                FWoRow_Folder::Ptr NewFolder = MakeShareable(new FWoRow_Folder);
                NewFolder->FolderName = f;
                CurrentFolder->Content.Add(NewFolder);
                CurrentFolder = NewFolder;
            }
        }
        
        FWoRow_Level::Ptr NewLevel = MakeShareable(new FWoRow_Level);
        NewLevel->SetStreamingLevel( l );
        CurrentFolder->Content.Add(NewLevel);
    }

    TheRoot->Sort();
}

void SWorldOpener::Reconstruct()
{
    TSet<FWoRowBase::Ptr> ItemsExpanded;
    Tree->GetExpandedItems(ItemsExpanded);
    ConstructTree();
    if(SearchBox)
    {
        const FText SearchText = SearchBox->GetText();
        OnSearch(SearchText);
    }
    Tree->RequestTreeRefresh();
    ExpandRecursive(ItemsExpanded);
}

void SWorldOpener::ExpandRecursive(const TSet<FWoRowBase::Ptr>& ItemsExpand,
                                   TOptional<TArray<FWoRowBase::Ptr>> InRoot) const
{
    const TArray<FWoRowBase::Ptr>& Array = InRoot.Get(Root);
    
    for(auto& r : Array)
        if(r->IsAnySameFolderNameInSet(ItemsExpand))
        {
            Tree->SetItemExpansion(r, true);
            TArray<FWoRowBase::Ptr> Children;
            r->GetChildren(Children);
            ExpandRecursive(ItemsExpand, Children);
        }
}

void SWorldOpener::AllExpand(bool bExpand) const
{
    Tree->SetItemExpansion(Root[0], bExpand);
    TArray<FWoRowBase::Ptr> Children;
    Root[0]->GetChildrenRecursive(Children);
    for(const auto& c : Children)
    {
        Tree->SetItemExpansion(c, bExpand);
    }
}

void SWorldOpener::OnSearch(const FText& InSearchText)
{
    Root[0]->SetFilterText(InSearchText);
    Tree->RequestTreeRefresh();
    AllExpand(!InSearchText.IsEmptyOrWhitespace());
}


#undef LOCTEXT_NAMESPACE
