#include "IFS.h"
#include "GameplayTagsManager.h"

void FIFSModule::StartupModule()
{
    UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

    TagsManager.AddNativeGameplayTag(FName("Ability.Fire.DecreaseFuel"), TEXT("Gameplay Ability that decreases fuel over time"));
    TagsManager.AddNativeGameplayTag(FName("Ability.Fire.Start"), TEXT("Gameplay Ability that starts fire"));
    TagsManager.AddNativeGameplayTag(FName("Ability.Fire.AddFuel"), TEXT("Gameplay Ability that adds fuel to the fire"));
}


IMPLEMENT_PRIMARY_GAME_MODULE(FIFSModule, IFS, "IFS");
