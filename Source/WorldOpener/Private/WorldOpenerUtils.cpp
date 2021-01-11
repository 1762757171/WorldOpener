#include "WorldOpenerUtils.h"
#include "EditorLevelUtils.h"
#include "FileHelpers.h"

void WoUtils::LoadLevel(ULevelStreaming* InLevelStreaming)
{
    WoUtils::FAvoidModifyHelper NoModifyWorld(InLevelStreaming);
    auto& CurrentState = access_private::CurrentState(*InLevelStreaming);
    CurrentState = ULevelStreaming::ECurrentState::Unloaded;
    InLevelStreaming->SetShouldBeVisibleInEditor(true);
}

void WoUtils::UnloadLevel(ULevelStreaming* InLevelStreaming)
{
    WoUtils::FAvoidModifyHelper NoModifyWorld(InLevelStreaming);
    auto& CurrentState = access_private::CurrentState(*InLevelStreaming);
    ULevel* LoadedLevel = InLevelStreaming->GetLoadedLevel();
    if (LoadedLevel)
    {
        GEditor->SelectNone(true, true);
        if(LoadedLevel->GetOutermost()->IsDirty())
            if (FEditorFileUtils::SaveDirtyPackages(true, true, false) == false)
                // The user pressed cancel. Abort the import so the user doesn't lose any changes
                return;
        InLevelStreaming->ClearLoadedLevel();
        EditorLevelUtils::RemoveLevelFromWorld(LoadedLevel);
    }
    CurrentState = ULevelStreaming::ECurrentState::FailedToLoad; 
}

void WoUtils::FlushWorld(const TArray<ULevelStreaming*>& InAdded)
{
    UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
    WoUtils::FAvoidModifyHelper NoModifyWorld(EditorWorld);
    
    EditorWorld->bRequestedBlockOnAsyncLoading = true;
    EditorWorld->PopulateStreamingLevelsToConsider();
    EditorWorld->FlushLevelStreaming();
    EditorWorld->RefreshStreamingLevels();
    EditorWorld->bRequestedBlockOnAsyncLoading = false;

    for(const auto& a : InAdded)
        if(ULevel* l = a->GetLoadedLevel())
            EditorLevelUtils::SetLevelVisibility(l, true, true, ELevelVisibilityDirtyMode::DontModify);
}

void WoUtils::MakeCurrent(ULevelStreaming* InLevelStreaming)
{
    UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
    WoUtils::FAvoidModifyHelper NoModifyWorld(EditorWorld);
    
    if (EditorWorld->SetCurrentLevel(InLevelStreaming->GetLoadedLevel()))
    {
        FEditorDelegates::NewCurrentLevel.Broadcast();
    }
}
