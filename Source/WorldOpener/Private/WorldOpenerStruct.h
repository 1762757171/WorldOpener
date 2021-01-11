#pragma once

struct FWoRowBase : public TSharedFromThis<FWoRowBase>
{
    using Ptr = TSharedPtr<FWoRowBase>;

    virtual ~FWoRowBase() {}

    virtual TSharedRef<SWidget> GetRowWidget(TFunction<bool()>&& IsExpand) const = 0;

    virtual ECheckBoxState GetCheckState() const = 0;

    virtual void ChangeCheckState(ECheckBoxState NewState, TArray<ULevelStreaming*>& OutAdded) = 0;

    virtual bool IsFolderWithName(const FString& InName) { return false; }

    virtual bool IsAnySameFolderNameInSet(const TSet<Ptr>& InSet) const { return false; }

    virtual bool OnDoubleClick() { return false; }

    virtual bool IsDirty() { return false; }

    virtual FString GetNameForSort() const = 0;

    virtual FText GetCheckboxTooltip() const = 0;

    virtual FText GetElementTooltip() const = 0;

    virtual void Sort() {}

    virtual void SetFilterText(const FText& InText) = 0;

    virtual bool ShouldShow() const = 0;

    virtual void GetLevels(TArray<ULevelStreaming*>& OutLevels) = 0;

    virtual bool IsFolder() const { return false; }

    virtual void GetChildrenRecursive(TArray<FWoRowBase::Ptr>& OutChildren, TFunction<bool(FWoRowBase::Ptr)>&& FilterMethod = [](FWoRowBase::Ptr r){return !r->ShouldShow();}) const = 0;
    
    virtual void GetChildren(TArray<FWoRowBase::Ptr>& OutChildren, TFunction<bool(FWoRowBase::Ptr)>&& FilterMethod = [](FWoRowBase::Ptr r){return !r->ShouldShow();}) const = 0;

    virtual int32 GetLevelCount() const = 0;
    
};

struct FWoRow_Level : public FWoRowBase
{
    using Ptr = TSharedPtr<FWoRow_Level>;

    void SetStreamingLevel(class ULevelStreaming* InLevelStreaming);

protected:

    virtual TSharedRef<SWidget> GetRowWidget(TFunction<bool()>&& IsExpand) const override;

    virtual ECheckBoxState GetCheckState() const override;

    virtual void ChangeCheckState(ECheckBoxState NewState, TArray<ULevelStreaming*>& OutAdded) override;

    virtual void GetChildren(TArray<FWoRowBase::Ptr>& OutChildren, TFunction<bool(FWoRowBase::Ptr)>&& FilterMethod) const override;

    virtual void GetChildrenRecursive(TArray<FWoRowBase::Ptr>& OutChildren, TFunction<bool(FWoRowBase::Ptr)>&& FilterMethod) const override;

    virtual bool OnDoubleClick() override;

    virtual bool IsDirty() override;

    virtual FString GetNameForSort() const override;

    virtual FText GetCheckboxTooltip() const override;

    virtual FText GetElementTooltip() const override;

    virtual void SetFilterText(const FText& InText) override;

    virtual bool ShouldShow() const override;

    virtual void GetLevels(TArray<ULevelStreaming*>& OutLevels) override;
    
    virtual int32 GetLevelCount() const override;

private:
    
    class ULevelStreaming* LevelStreaming = nullptr;

    FName PrevFolder;

    FText Filter;
    
};

struct FWoRow_Folder : public FWoRowBase
{
    using Ptr = TSharedPtr<FWoRow_Folder>;
    
    FString FolderName;
    TArray<FWoRowBase::Ptr> Content;

    virtual TSharedRef<SWidget> GetRowWidget(TFunction<bool()>&& IsExpand) const override;

    virtual ECheckBoxState GetCheckState() const override;

    virtual void ChangeCheckState(ECheckBoxState NewState, TArray<ULevelStreaming*>& OutAdded) override;

    virtual void GetChildren(TArray<FWoRowBase::Ptr>& OutChildren, TFunction<bool(FWoRowBase::Ptr)>&& FilterMethod) const override;

    virtual void GetChildrenRecursive(TArray<FWoRowBase::Ptr>& OutChildren, TFunction<bool(FWoRowBase::Ptr)>&& FilterMethod) const override;

    virtual bool IsFolderWithName(const FString& InName) override;

    virtual bool IsAnySameFolderNameInSet(const TSet<FWoRowBase::Ptr>& InSet) const override;

    virtual FString GetNameForSort() const override;

    virtual void Sort() override;

    virtual FText GetCheckboxTooltip() const override;

    virtual FText GetElementTooltip() const override;

    virtual void SetFilterText(const FText& InText) override;

    virtual bool ShouldShow() const override;

    virtual void GetLevels(TArray<ULevelStreaming*>& OutLevels) override;

    virtual bool IsFolder() const override { return true; }
    
    virtual int32 GetLevelCount() const override;

private:

    FText Filter;
    
};

