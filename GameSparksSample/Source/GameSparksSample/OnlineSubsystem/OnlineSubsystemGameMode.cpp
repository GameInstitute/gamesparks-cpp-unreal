#include "GameSparksSample.h"
#include "OnlineSubsystemGameMode.h"
#include "OnlineFriendsInterface.h"
#include "Online.h"
#include <cassert>

AOnlineSubsystemGameMode::AOnlineSubsystemGameMode()
{
    // bind functions to delegates
    OnConnectionStatusChangedDelegate = FOnConnectionStatusChangedDelegate::CreateUObject(this, &AOnlineSubsystemGameMode::OnConnectionStatusChanged);
    OnLoginCompleteDelegate = FOnLoginCompleteDelegate::CreateUObject(this, &AOnlineSubsystemGameMode::OnLoginComplete);
    OnLogoutCompleteDelegate = FOnLogoutCompleteDelegate::CreateUObject(this, &AOnlineSubsystemGameMode::OnLogoutComplete);
}


void AOnlineSubsystemGameMode::BeginPlay()
{
    Super::BeginPlay();

    IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
    assert(OnlineSub);

    IOnlineIdentityPtr identity = OnlineSub->GetIdentityInterface();
    assert(identity);

    { // you can also get the identity-interface this way:
        IOnlineIdentityPtr const IdentityInterface = Online::GetIdentityInterface("GAMESPARKS");
        check(IdentityInterface.IsValid());
    }

    OnLoginCompleteDelegateHandle = identity->AddOnLoginCompleteDelegate_Handle(0, OnLoginCompleteDelegate);

    FOnlineAccountCredentials AccountCredentials;

    AccountCredentials.Type = "GSDevice"; // Device authentication Request

    identity->Login(0, AccountCredentials);
}


void AOnlineSubsystemGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
    assert(OnlineSub);

    OnlineSub->ClearOnConnectionStatusChangedDelegate_Handle(OnConnectionStatusChangedDelegateHandle);
}


void AOnlineSubsystemGameMode::Tick(float DeltaSeconds)
{
    AGameMode::Tick(DeltaSeconds);
}


void AOnlineSubsystemGameMode::OnConnectionStatusChanged(const FString& ServiceName, EOnlineServerConnectionStatus::Type LastConnectionState, EOnlineServerConnectionStatus::Type ConnectionState)
{
    if(ConnectionState == EOnlineServerConnectionStatus::Type::Connected)
    {
        IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
        IOnlineIdentityPtr identity = OnlineSub->GetIdentityInterface();
        assert(identity);

        FOnlineAccountCredentials AccountCredentials;

        AccountCredentials.Type = "GSDevice"; // Device authentication Request

        identity->Login(0, AccountCredentials);
    }
}


void AOnlineSubsystemGameMode::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
    IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
    assert(OnlineSub);

    IOnlineIdentityPtr identity = OnlineSub->GetIdentityInterface();
    assert(identity);

    identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, OnLoginCompleteDelegateHandle);


    if(bWasSuccessful)
    {
        auto playerId = identity->GetUniquePlayerId(LocalUserNum);
        auto user = identity->GetUserAccount(*playerId);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Logged in as ") + user->GetDisplayName());

        auto friendInterface = OnlineSub->GetFriendsInterface();

        FOnReadFriendsListComplete OnReadFriendsListComplete;
        OnReadFriendsListComplete.BindLambda([=](int32, bool, const FString&, const FString& ErrorStr){
            TArray<TSharedRef<FOnlineFriend> > friends;

            if(!friendInterface->GetFriendsList(LocalUserNum, TEXT(""), friends))
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GetFriendsList failed"));
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Your Friends:"));

                for(int i=0; i!= friends.Num(); ++i)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f + 20 * i, FColor::Yellow, friends[i]->GetDisplayName());
                }
            }
        });

        if(!friendInterface->ReadFriendsList(LocalUserNum, "", OnReadFriendsListComplete))
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ReadFriendsList failed"));
        }

        // and why not log the user out again ...
        OnLogoutCompleteDelegateHandle = identity->AddOnLogoutCompleteDelegate_Handle(LocalUserNum, OnLogoutCompleteDelegate);
        identity->Logout(LocalUserNum);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("log-in failed with error: ") + Error);
    }
}


void AOnlineSubsystemGameMode::OnLogoutComplete(int32 LocalUserNum, bool bWasSuccessful)
{
    IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
    assert(OnlineSub);

    IOnlineIdentityPtr identity = OnlineSub->GetIdentityInterface();
    assert(identity);

    identity->ClearOnLogoutCompleteDelegate_Handle(LocalUserNum, OnLogoutCompleteDelegateHandle);

    if(bWasSuccessful)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,   FString::Printf(TEXT("Logged out user %d"), LocalUserNum));

        // and log-in again
        OnLoginCompleteDelegateHandle = identity->AddOnLoginCompleteDelegate_Handle(0, OnLoginCompleteDelegate);
        FOnlineAccountCredentials AccountCredentials;
        AccountCredentials.Type = "GSDevice"; // Device authentication Request
        identity->Login(0, AccountCredentials);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("log-out failed"));
    }
}
