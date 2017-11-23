//////////////////////////////////////////////////////////////////////
//
// AkUnrealIOHookDeferred.cpp
//
// Default deferred low level IO hook (AK::StreamMgr::IAkIOHookDeferred) 
// and file system (AK::StreamMgr::IAkFileLocationResolver) implementation 
// on Windows.
// 
// AK::StreamMgr::IAkFileLocationResolver: 
// Resolves file location using simple path concatenation logic 
// (implemented in ../Common/CAkFileLocationBase). It can be used as a 
// standalone Low-Level IO system, or as part of a multi device system. 
// In the latter case, you should manage multiple devices by implementing 
// AK::StreamMgr::IAkFileLocationResolver elsewhere (you may take a look 
// at class CAkDefaultLowLevelIODispatcher).
//
// AK::StreamMgr::IAkIOHookDeferred: 
// Uses platform API for I/O. Calls to ::ReadFile() and ::WriteFile() 
// do not block because files are opened with the FILE_FLAG_OVERLAPPED flag. 
// Transfer completion is handled by internal FileIOCompletionRoutine function,
// which then calls the AkAIOCallback.
// The AK::StreamMgr::IAkIOHookDeferred interface is meant to be used with
// AK_SCHEDULER_DEFERRED_LINED_UP streaming devices. 
//
// Init() creates a streaming device (by calling AK::StreamMgr::CreateDevice()).
// AkDeviceSettings::uSchedulerTypeFlags is set inside to AK_SCHEDULER_DEFERRED_LINED_UP.
// If there was no AK::StreamMgr::IAkFileLocationResolver previously registered 
// to the Stream Manager, this object registers itself as the File Location Resolver.
//
// Copyright (c) 2006 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#include "AkAudioDevice.h"

// @todo:ak verify consistency of Wwise SDK alignment settings with unreal build settings on all platforms
//			Currently, Wwise SDK builds with the default alignment (8-bytes as per MSDN) with the VC toolchain.
//			Unreal builds with 4-byte alignment on the VC toolchain on both Win32 and Win64. 
//			This could (and currently does, on Win64) cause data corruption if the headers are not included with forced alignment directives as below.

#include "AkUnrealIOHookDeferred.h"
#include "Core.h"

// Device info.
#define DEFERRED_DEVICE_NAME		("UnrealIODevice")	// Default deferred device name.
CAkUnrealIOHookDeferred::AkDeferredIOInfo CAkUnrealIOHookDeferred::aPendingTransfers[AK_UNREAL_MAX_CONCURRENT_IO];
extern CAkUnrealIOHookDeferred g_lowLevelIO;

CAkUnrealIOHookDeferred::CAkUnrealIOHookDeferred() 
	: m_bCallbackRegistered(false)
	, m_deviceID(AK_INVALID_DEVICE_ID)
{
}

CAkUnrealIOHookDeferred::~CAkUnrealIOHookDeferred()
{
}

// Initialization/termination. Init() registers this object as the one and 
// only File Location Resolver if none were registered before. Then 
// it creates a streaming device with scheduler type AK_SCHEDULER_DEFERRED_LINED_UP.
AKRESULT CAkUnrealIOHookDeferred::Init(
	const AkDeviceSettings &	in_deviceSettings		// Device settings.
	)
{
	{
		FScopeLock ScopeLock(&CriticalSection);

		for(int32 i = 0; i < AK_UNREAL_MAX_CONCURRENT_IO; i++)
		{
			CAkUnrealIOHookDeferred::aPendingTransfers[i].Counter.Set(-1);
		}
	}
	if ( in_deviceSettings.uSchedulerTypeFlags != AK_SCHEDULER_DEFERRED_LINED_UP )
	{
		AKASSERT( !"CAkDefaultIOHookDeferred I/O hook only works with AK_SCHEDULER_DEFERRED_LINED_UP devices" );

		return AK_Fail;
	}
	
	// If the Stream Manager's File Location Resolver was not set yet, set this object as the 
	// File Location Resolver (this I/O hook is also able to resolve file location).
	if ( !AK::StreamMgr::GetFileLocationResolver() )
		AK::StreamMgr::SetFileLocationResolver( this );

	// Create a device in the Stream Manager, specifying this as the hook.
	m_deviceID = AK::StreamMgr::CreateDevice( in_deviceSettings, this );
	if ( m_deviceID != AK_INVALID_DEVICE_ID )
	{
		return AK_Success;
	}


	return AK_Fail;
}

void CAkUnrealIOHookDeferred::Term()
{
	if ( m_bCallbackRegistered && FAkAudioDevice::Get() )
	{
		AK::SoundEngine::UnregisterGlobalCallback( GlobalCallback );
		m_bCallbackRegistered = false;
	}

	if ( AK::StreamMgr::GetFileLocationResolver() == this )
		AK::StreamMgr::SetFileLocationResolver( NULL );
	
	AK::StreamMgr::DestroyDevice( m_deviceID );
}

//
// IAkFileLocationAware implementation.
//-----------------------------------------------------------------------------

template<typename T>
AKRESULT CAkUnrealIOHookDeferred::PerformOpen( 
    T		        in_fileDescriptor,  // File ID or file Name.
    AkOpenMode      in_eOpenMode,       // Open mode.
    AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
	bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
    AkFileDesc &    out_fileDesc        // Returned file descriptor.
    )
{
	CleanFileDescriptor(out_fileDesc);
	if (AK_OpenModeReadWrite == in_eOpenMode)
	{
		return AK_NotImplemented;
	}
	else if (AK_OpenModeRead == in_eOpenMode)
	{
		io_bSyncOpen = true;
		FString* FilePath = new FString();

		// Get the full file path, using path concatenation logic.

		if (GetFullFilePath(in_fileDescriptor, in_pFlags, in_eOpenMode, FilePath) == AK_Success)
		{
			return FillFileDescriptorHelper(FilePath, out_fileDesc);
		}

		delete FilePath;
	}
	else // Write
	{
		FString FilePath;
		if (GetFullFilePath(in_fileDescriptor, in_pFlags, in_eOpenMode, &FilePath) == AK_Success)
		{
			io_bSyncOpen = true;
			IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			IFileHandle* FileHandle = PlatformFile.OpenWrite(*FilePath);
			if (FileHandle)
			{
				out_fileDesc.pCustomParam = (void*)FileHandle;
				out_fileDesc.uCustomParamSize = 1;
				return AK_Success;
			}
		}
	}

	return AK_Fail;
}

// Returns a file descriptor for a given file name (string).
AKRESULT CAkUnrealIOHookDeferred::Open( 
    const AkOSChar* in_pszFileName,     // File name.
    AkOpenMode      in_eOpenMode,       // Open mode.
    AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
	bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
    AkFileDesc &    out_fileDesc        // Returned file descriptor.
    )
{
	FString FileName(in_pszFileName);
	return PerformOpen(FileName, in_eOpenMode, in_pFlags, io_bSyncOpen, out_fileDesc);
}

// Returns a file descriptor for a given file ID.
AKRESULT CAkUnrealIOHookDeferred::Open( 
    AkFileID        in_fileID,          // File ID.
    AkOpenMode      in_eOpenMode,       // Open mode.
    AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
	bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
    AkFileDesc &    out_fileDesc        // Returned file descriptor.
    )
{
	return PerformOpen(in_fileID, in_eOpenMode, in_pFlags, io_bSyncOpen, out_fileDesc);
}

int32 CAkUnrealIOHookDeferred::GetFreeTransferIndex()
{
	for(int32 i = 0; i < AK_UNREAL_MAX_CONCURRENT_IO; i++)
	{
		if( CAkUnrealIOHookDeferred::aPendingTransfers[i].Counter.GetValue() == -1 )
		{
			return i;
		}
	}

	return -1;
}

int32 CAkUnrealIOHookDeferred::FindTransfer(void* pBuffer)
{
	for(int32 i = 0; i < AK_UNREAL_MAX_CONCURRENT_IO; i++)
	{
		if( CAkUnrealIOHookDeferred::aPendingTransfers[i].AkTransferInfo.pBuffer == pBuffer )
		{
			return i;
		}
	}

	return -1;
}


//
// IAkIOHookDeferred implementation.
//-----------------------------------------------------------------------------

// Reads data from a file (asynchronous overload).
AKRESULT CAkUnrealIOHookDeferred::Read(
	AkFileDesc &			in_fileDesc,        // File descriptor.
	const AkIoHeuristics & in_heuristics,	// Heuristics for this data transfer (not used in this implementation).
	AkAsyncIOTransferInfo & io_transferInfo		// Asynchronous data transfer info.
	)
{
	FScopeLock ScopeLock(&CriticalSection);
	// in_fileDesc.uCustomParamSize == 0 if opened in read mode
	if( in_fileDesc.pCustomParam && in_fileDesc.uCustomParamSize == 0 )
	{
		// Register the global callback, if not already done
		if( !m_bCallbackRegistered && FAkAudioDevice::Get() )
		{
			if ( AK::SoundEngine::RegisterGlobalCallback( GlobalCallback ) != AK_Success )
			{
				AKASSERT( !"Failed registering to global callback" );
				return AK_Fail;
			}
			m_bCallbackRegistered = true;
		}

		int32 TransferIndex = GetFreeTransferIndex();
		AKASSERT( TransferIndex != -1 );

		CAkUnrealIOHookDeferred::aPendingTransfers[TransferIndex].pCustomParam = in_fileDesc.pCustomParam;
		CAkUnrealIOHookDeferred::aPendingTransfers[TransferIndex].AkTransferInfo = io_transferInfo;
		CAkUnrealIOHookDeferred::aPendingTransfers[TransferIndex].Counter.Set(1);

		{
			FIOSystem & IO = FIOSystem::Get();
			CAkUnrealIOHookDeferred::aPendingTransfers[TransferIndex].RequestIndex = IO.LoadData( 
				*((FString*)in_fileDesc.pCustomParam), 
				(int64) io_transferInfo.uFilePosition, 
				io_transferInfo.uRequestedSize, 
				io_transferInfo.pBuffer, 
				&CAkUnrealIOHookDeferred::aPendingTransfers[TransferIndex].Counter, 
				AIOP_High
				);
		}
		if( CAkUnrealIOHookDeferred::aPendingTransfers[TransferIndex].RequestIndex == 0 )
		{
			return AK_Fail;
		}

		return AK_Success;
	}

	return AK_NotImplemented;
}

AKRESULT CAkUnrealIOHookDeferred::Write(
	AkFileDesc &			in_fileDesc,        // File descriptor.
	const AkIoHeuristics & /*in_heuristics*/,	// Heuristics for this data transfer (not used in this implementation).
	AkAsyncIOTransferInfo & io_transferInfo		// Platform-specific asynchronous IO operation info.
	)
{
	// in_fileDesc.uCustomParamSize == 1 if opened in read mode
	if (in_fileDesc.pCustomParam && in_fileDesc.uCustomParamSize == 1)
	{
		IFileHandle* FileHandle = (IFileHandle*)in_fileDesc.pCustomParam;
		FileHandle->Seek(io_transferInfo.uFilePosition);
		bool result = FileHandle->Write((uint8*)io_transferInfo.pBuffer, io_transferInfo.uBufferSize);
		if (result)
		{
			if (io_transferInfo.pCallback)
			{
				io_transferInfo.pCallback(&io_transferInfo, AK_Success);
			}
			return AK_Success;
		}
	}
	return AK_Fail;
}


// Cancel transfer(s).
void CAkUnrealIOHookDeferred::Cancel(
	AkFileDesc &			in_fileDesc,		// File descriptor.
	AkAsyncIOTransferInfo & io_transferInfo,// Transfer info to cancel (this implementation only handles "cancel all").
	bool & io_bCancelAllTransfersForThisFile	// Flag indicating whether all transfers should be cancelled for this file (see notes in function description).
	)
{
	FScopeLock ScopeLock(&CriticalSection);
	if (in_fileDesc.uCustomParamSize == 0)
	{
		FIOSystem & IO = FIOSystem::Get();
		int32 TransferIndex = FindTransfer(io_transferInfo.pBuffer);

		if (TransferIndex != -1)
		{
			// Cancel the request. This decrements the thread-safe counter, so our global callback
			// will call the callback function automatically. The transfer will thus be handled correctly.
			IO.CancelRequests(&CAkUnrealIOHookDeferred::aPendingTransfers[TransferIndex].RequestIndex, 1);

			// We only cancelled one request, and not all of them.
			io_bCancelAllTransfersForThisFile = false;
		}
	}
}

// Close a file.
AKRESULT CAkUnrealIOHookDeferred::Close(
    AkFileDesc & in_fileDesc      // File descriptor.
    )
{
	FScopeLock ScopeLock(&CriticalSection);
	if (in_fileDesc.uCustomParamSize == 0)
	{
		FString* Filename = (FString*)in_fileDesc.pCustomParam;
		if (Filename)
		{
			FIOSystem & IO = FIOSystem::Get();
			IO.HintDoneWithFile(*Filename);
			delete (FString*)Filename;
			in_fileDesc.pCustomParam = nullptr;
		}
	}
	else if (in_fileDesc.uCustomParamSize == 1)
	{
		delete (IFileHandle*)in_fileDesc.pCustomParam;
		in_fileDesc.pCustomParam = nullptr;
	}

	return AK_Success;
}

void CAkUnrealIOHookDeferred::GlobalCallback(AK::IAkGlobalPluginContext * in_pContext, AkGlobalCallbackLocation in_eLocation, void * in_pCookie)
{
	// Loop through all our pending transfers to see if some are done.
	FScopeLock ScopeLock(&g_lowLevelIO.CriticalSection);
	for (int32 i = 0; i < AK_UNREAL_MAX_CONCURRENT_IO; i++)
	{
		if( CAkUnrealIOHookDeferred::aPendingTransfers[i].Counter.GetValue() == 0 )
		{
			AKASSERT(CAkUnrealIOHookDeferred::aPendingTransfers[i].AkTransferInfo.pCallback);

			CAkUnrealIOHookDeferred::aPendingTransfers[i].AkTransferInfo.pCallback(&CAkUnrealIOHookDeferred::aPendingTransfers[i].AkTransferInfo, AK_Success);

			// Release the slot for this transfer. We are done for now.
			CAkUnrealIOHookDeferred::aPendingTransfers[i].Counter.Set(-1);
		}
	}
}


void CAkUnrealIOHookDeferred::CleanFileDescriptor( AkFileDesc& out_fileDesc )
{
	out_fileDesc.uSector			= 0;
	out_fileDesc.deviceID			= m_deviceID;
	out_fileDesc.uCustomParamSize	= 0;	// not used.
	out_fileDesc.pCustomParam		= NULL;
	out_fileDesc.iFileSize			= 0;
}

AKRESULT CAkUnrealIOHookDeferred::FillFileDescriptorHelper(const FString* in_szFullFilePath, AkFileDesc& out_fileDesc )
{
	if(in_szFullFilePath && !in_szFullFilePath->IsEmpty())
	{
		out_fileDesc.iFileSize	= IFileManager::Get().FileSize( **in_szFullFilePath);
		if( out_fileDesc.iFileSize > 0 )
		{
			out_fileDesc.uCustomParamSize	= 0; // 0 for read, 1 for write
			// We need the file name to pass UE4's IO module. Use the custom param to remember it.
			out_fileDesc.pCustomParam = (void*)in_szFullFilePath;

			return AK_Success;
		}
	}
	return AK_Fail;
}

// Returns the block size for the file or its storage device. 
AkUInt32 CAkUnrealIOHookDeferred::GetBlockSize(
    AkFileDesc &  /*in_fileDesc*/     // File descriptor.
    )
{
	// There is no limitation nor performance degradation with unaligned
	// seeking on any mount point with asynchronous cell fs API.
    return 1;
}

// Returns a description for the streaming device above this low-level hook.
void CAkUnrealIOHookDeferred::GetDeviceDesc(
    AkDeviceDesc & 
#ifndef AK_OPTIMIZED
	out_deviceDesc      // Description of associated low-level I/O device.
#endif
    )
{
#ifndef AK_OPTIMIZED
	// Deferred scheduler.
	out_deviceDesc.deviceID       = m_deviceID;
	out_deviceDesc.bCanRead       = true;
	out_deviceDesc.bCanWrite      = true;
	AK_CHAR_TO_UTF16( out_deviceDesc.szDeviceName, DEFERRED_DEVICE_NAME, AK_MONITOR_DEVICENAME_MAXLENGTH );
	out_deviceDesc.uStringSize   = (AkUInt32)AKPLATFORM::AkUtf16StrLen( out_deviceDesc.szDeviceName ) + 1;
#endif
}

// Returns custom profiling data: 1 if file opens are asynchronous, 0 otherwise.
AkUInt32 CAkUnrealIOHookDeferred::GetDeviceData()
{
	return 0;
}

// Writes data to a file (asynchronous overload).
AKRESULT CAkUnrealIOHookDeferred::GetFullFilePath(
	const FString&		in_szFileName,		// File name.
	AkFileSystemFlags * in_pFlags,			// Special flags. Can be NULL.
	AkOpenMode			in_eOpenMode,		// File open mode (read, write, ...).
	FString*			out_szFullFilePath // Full file path.
	)
{
	if (in_szFileName.IsEmpty())
	{
		AKASSERT(!"Invalid file name");
		return AK_InvalidParameter;
	}

	// Prepend string path (basic file system logic).
	int32 uiPathSize = in_szFileName.Len();
	if (in_szFileName.Len() >= AK_MAX_PATH)
	{
		AKASSERT(!"Input string too large");
		return AK_InvalidParameter;
	}

	*out_szFullFilePath = m_szBasePath;

	if (in_pFlags
		&& in_eOpenMode == AK_OpenModeRead)
	{
		// Add language directory name if needed.
		if (in_pFlags->bIsLanguageSpecific)
		{
			size_t uLanguageStrLen = AKPLATFORM::OsStrLen(AK::StreamMgr::GetCurrentLanguage());
			if (uLanguageStrLen > 0)
			{
				uiPathSize += (uLanguageStrLen + 1);
				if (uiPathSize >= AK_MAX_PATH)
				{
					AKASSERT(!"Path is too large");
					return AK_Fail;
				}
				FString CurrentLanguage(AK::StreamMgr::GetCurrentLanguage());
				*out_szFullFilePath = FPaths::Combine(**out_szFullFilePath, *CurrentLanguage);
				out_szFullFilePath->Append(FGenericPlatformMisc::GetDefaultPathSeparator());
			}
		}
	}

	// Append file title.
	uiPathSize += out_szFullFilePath->Len();
	if (uiPathSize >= AK_MAX_PATH)
	{
		AKASSERT(!"File name string too large");
		return AK_Fail;
	}

	out_szFullFilePath->Append(in_szFileName);
	return AK_Success;
}

#define MAX_NUMBER_STRING_SIZE      (10)    // 4G
#define MAX_EXTENSION_SIZE          (4)     // .xxx
#define MAX_FILETITLE_SIZE          (MAX_NUMBER_STRING_SIZE+MAX_EXTENSION_SIZE+1)   // null-terminated
AKRESULT CAkUnrealIOHookDeferred::GetFullFilePath(
	AkFileID			in_fileID,		// File name.
	AkFileSystemFlags * in_pFlags,			// Special flags. Can be NULL.
	AkOpenMode			in_eOpenMode,		// File open mode (read, write, ...).
	FString*			out_szFullFilePath // Full file path.
	)
{
	if (!in_pFlags ||
		!(in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC || in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC_EXTERNAL))
	{
		AKASSERT(!"Unhandled file type");
		return AK_InvalidParameter;
	}

	// Compute file name with file system paths.
	size_t uiPathSize = m_szBasePath.Len();

	// Copy base path. 
	*out_szFullFilePath = m_szBasePath;

	// Add language directory name if needed.
	if (in_pFlags->bIsLanguageSpecific)
	{
		size_t uLanguageStrLen = AKPLATFORM::OsStrLen(AK::StreamMgr::GetCurrentLanguage());
		if (uLanguageStrLen > 0)
		{
			uiPathSize += (uLanguageStrLen + 1);
			if (uiPathSize >= AK_MAX_PATH)
			{
				AKASSERT(!"Path is too large");
				return AK_InvalidParameter;
			}
			out_szFullFilePath->Append(AK::StreamMgr::GetCurrentLanguage());
			out_szFullFilePath->Append(FGenericPlatformMisc::GetDefaultPathSeparator());
		}
	}

	// Append file title.
	if ((uiPathSize + MAX_FILETITLE_SIZE) <= AK_MAX_PATH)
	{
			out_szFullFilePath->Append(FString::FromInt(in_fileID));
		if (in_pFlags->uCodecID == AKCODECID_BANK)
			out_szFullFilePath->Append(TEXT(".bnk"));
		else
			out_szFullFilePath->Append(TEXT(".wem"));
	}
	else
	{
		AKASSERT(!"String buffer too small");
		return AK_InvalidParameter;
	}

	return AK_Success;
}


AKRESULT CAkUnrealIOHookDeferred::SetBasePath(
	const FString&   in_szBasePath
	)
{
	if (in_szBasePath.Len() + AKPLATFORM::OsStrLen(AK::StreamMgr::GetCurrentLanguage()) + 1 >= AK_MAX_PATH)
	{
		return AK_InvalidParameter;
	}

	m_szBasePath = in_szBasePath;
	if (!m_szBasePath.EndsWith(FGenericPlatformMisc::GetDefaultPathSeparator()))
	{
		m_szBasePath.Append(FGenericPlatformMisc::GetDefaultPathSeparator());
	}

	if (!FPaths::DirectoryExists(m_szBasePath))
	{
		return AK_PathNotFound;
	}

	return AK_Success;
}


