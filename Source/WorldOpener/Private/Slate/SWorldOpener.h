#pragma once

#include "CoreMinimal.h"

#include "WorldOpenerStruct.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class SWorldOpener : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SWorldOpener) {}
    
    SLATE_END_ARGS()

public:
    
    void Construct(const FArguments& InArgs);
    ~SWorldOpener();

protected:

    void ConstructTree();

    void Reconstruct();

    void ExpandRecursive(const TSet<FWoRowBase::Ptr>& ItemsExpand,
        TOptional<TArray<FWoRowBase::Ptr>> InRoot = TOptional<TArray<FWoRowBase::Ptr>>()) const;

    void AllExpand(bool bExpand = true) const;

    void OnSearch(const FText& InSearchText);

private:

    TArray<FWoRowBase::Ptr> Root;

    TSharedPtr<STreeView<FWoRowBase::Ptr>> Tree;

    TSharedPtr<SComboButton> ViewConfigComboButton;
    TSharedPtr<SSearchBox> SearchBox;

    FDelegateHandle Dh_Add;
    FDelegateHandle Dh_Remove;
    FDelegateHandle Dh_Change;

};
