// Copyright (c) 2006-2017 Audiokinetic Inc. / All Rights Reserved

#pragma once

#include "AkAudioDevice.h"


class IAkUserEventCallbackPackage
{
public:
	/** Copy of the user callback flags, for use in our own callback */
	uint32 uUserFlags;

	IAkUserEventCallbackPackage()
		: uUserFlags(0)
	{}

	IAkUserEventCallbackPackage(uint32 in_Flags)
		: uUserFlags(in_Flags)
	{}

	virtual ~IAkUserEventCallbackPackage() {}

	virtual void HandleAction(AkCallbackType in_eType, AkCallbackInfo* in_pCallbackInfo) = 0;
};

class FAkFunctionPtrEventCallbackPackage : public IAkUserEventCallbackPackage
{
public:
	FAkFunctionPtrEventCallbackPackage(AkCallbackFunc CbFunc, void* Cookie, uint32 Flags)
		: IAkUserEventCallbackPackage(Flags)
		, pfnUserCallback(CbFunc)
		, pUserCookie(Cookie)
	{}

	virtual void HandleAction(AkCallbackType in_eType, AkCallbackInfo* in_pCallbackInfo) override;

private:
	/** Copy of the user callback, for use in our own callback */
	AkCallbackFunc pfnUserCallback;

	/** Copy of the user cookie, for use in our own callback */
	void* pUserCookie;

};

class FAkBlueprintDelegateEventCallbackPackage : public IAkUserEventCallbackPackage
{
public:
	FAkBlueprintDelegateEventCallbackPackage(FOnAkPostEventCallback PostEventCallback, uint32 Flags)
		: IAkUserEventCallbackPackage(Flags)
		, BlueprintCallback(PostEventCallback)
	{}

	virtual void HandleAction(AkCallbackType in_eType, AkCallbackInfo* in_pCallbackInfo) override;

private:
	FOnAkPostEventCallback BlueprintCallback;
};

class FAkLatentActionEventCallbackPackage : public IAkUserEventCallbackPackage
{
public:
	FAkLatentActionEventCallbackPackage(FWaitEndOfEventAction* LatentAction)
		: IAkUserEventCallbackPackage(AK_EndOfEvent)
		, EndOfEventLatentAction(LatentAction)
	{}

	virtual void HandleAction(AkCallbackType in_eType, AkCallbackInfo* in_pCallbackInfo) override;

private:
	FWaitEndOfEventAction* EndOfEventLatentAction;
};

class FAkComponentCallbackManager
{
public:
	static FAkComponentCallbackManager* GetInstance();

	static FAkComponentCallbackManager* Instance;

	/** Our own event callback */
	static void AkComponentCallback(AkCallbackType in_eType, AkCallbackInfo* in_pCallbackInfo);

	FAkComponentCallbackManager();
	~FAkComponentCallbackManager();

	IAkUserEventCallbackPackage* CreateCallbackPackage(AkCallbackFunc in_cbFunc, void* in_Cookie, uint32 in_Flags, AkGameObjectID in_gameObjID);
	IAkUserEventCallbackPackage* CreateCallbackPackage(FOnAkPostEventCallback BlueprintCallback, uint32 in_Flags, AkGameObjectID in_gameObjID);
	IAkUserEventCallbackPackage* CreateCallbackPackage(FWaitEndOfEventAction* LatentAction, AkGameObjectID in_gameObjID);
	void RemoveCallbackPackage(IAkUserEventCallbackPackage* in_Package, AkGameObjectID in_gameObjID);

	void RegisterGameObject(AkGameObjectID in_gameObjID);
	void UnregisterGameObject(AkGameObjectID in_gameObjID);

	bool HasActiveEvents(AkGameObjectID in_gameObjID);

private:
	typedef TSet<IAkUserEventCallbackPackage*> PackageSet;

	void RemovePackageFromSet(PackageSet* in_pPackageSet, IAkUserEventCallbackPackage* in_pPackage, AkGameObjectID in_gameObjID);

	FCriticalSection CriticalSection;

	typedef AkGameObjectIdKeyFuncs<PackageSet, false> PackageSetGameObjectIDKeyFuncs;
	TMap<AkGameObjectID, PackageSet, FDefaultSetAllocator, PackageSetGameObjectIDKeyFuncs> GameObjectToPackagesMap;
};
