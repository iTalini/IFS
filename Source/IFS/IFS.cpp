#include "IFS.h"
#include "GameplayTagsManager.h"

void FIFSModule::StartupModule()
{
    UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

    TagsManager.AddNativeGameplayTag(FName("Abilities.Fireplace.DecreaseFuel"), TEXT("Gameplay Ability that decreases fuel over time"));
    TagsManager.AddNativeGameplayTag(FName("Abilities.Fireplace.Start"), TEXT("Gameplay Ability that starts fire"));
    TagsManager.AddNativeGameplayTag(FName("Abilities.Fireplace.AddFuel"), TEXT("Gameplay Ability that adds fuel to the fire"));
}


IMPLEMENT_PRIMARY_GAME_MODULE(FIFSModule, IFS, "IFS");
