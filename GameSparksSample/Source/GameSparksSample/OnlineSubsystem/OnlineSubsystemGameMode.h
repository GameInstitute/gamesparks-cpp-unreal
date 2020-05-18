#pragma once

#include "GameFramework/GameMode.h"
#include "OnlineSubsystem.h"
#include "OnlineIdentityInterface.h"
#include "OnlineSubsystemGameMode.generated.h"

/*
 * For this sample to work you need to enable the OnlineSubsystemGameSparks via DefaultEngine.ini by adding
 * the following lines:
 *
 *  [OnlineSubsystemGameSparks]
 *  bEnabled=True
 *  ApiKey=<your-api-key>
 *  ApiSecret=<your-api-secret>
 *  bUsePreviewServer=True
 *
 *  Be aware that those settings override the Key and Secret used in BluePrints.
 * 
 * */

UCLASS()
class GAMESPARKSSAMPLE_API AOnlineSubsystemGameMode : public AGameMode {
        GENERATED_BODY()

        //Constructor
        AOnlineSubsystemGameMode();

        // BeginPlay
        virtual void BeginPlay() override;

        // EndPlay
        virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

        // Tick
        virtual void Tick(float DeltaSeconds) override;
    private:
        void OnConnectionStatusChanged(const FString& ServiceName, EOnlineServerConnectionStatus::Type LastConnectionState, EOnlineServerConnectionStatus::Type ConnectionState);
        FOnConnectionStatusChangedDelegate OnConnectionStatusChangedDelegate;
        FDelegateHandle OnConnectionStatusChangedDelegateHandle;

        void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
        FOnLoginCompleteDelegate OnLoginCompleteDelegate;
        FDelegateHandle OnLoginCompleteDelegateHandle;

        void OnLogoutComplete(int32 LocalUserNum, bool bWasSuccessful);
        FOnLogoutCompleteDelegate OnLogoutCompleteDelegate;
        FDelegateHandle OnLogoutCompleteDelegateHandle;
};
