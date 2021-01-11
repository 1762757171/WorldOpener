#pragma once
#include "access_private.hpp"

ACCESS_PRIVATE_FIELD(ULevelStreaming, ULevelStreaming::ECurrentState, CurrentState);

namespace WoUtils
{
    //
    // Helper struct to temporally make specified UObject immune to dirtying
    //
    struct FAvoidModifyHelper
    {
        FAvoidModifyHelper(UObject* InObject)
            : ImmuneObject(InObject)
            , bDirty(InObject->GetOutermost()->IsDirty())
        {}
	
        ~FAvoidModifyHelper()
        {
            ImmuneObject->GetOutermost()->SetDirtyFlag(bDirty);
        }

    private:
        UObject*		ImmuneObject;
        bool			bDirty;
    };
    
    // You need call FlushWorld after this.
    void LoadLevel(ULevelStreaming* InLevelStreaming);
    // You need call FlushWorld after this.
    void UnloadLevel(ULevelStreaming* InLevelStreaming);

    void FlushWorld(const TArray<ULevelStreaming*>& InAdded);

    // Assumes level is loaded
    void MakeCurrent(ULevelStreaming* InLevelStreaming);
}
