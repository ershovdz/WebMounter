#ifndef LVFSDRIVER_H
#define LVFSDRIVER_H

//Windows specific headers
#include <windows.h>
#include <winbase.h>

//Dokan driver header
#include <dokan.h>

//QT headers
#include <QThread>
#include <QMutex>

//WebMounter headers
#include "lvfs_driver.h"
#include "file_proxy.h"
//#include "NotificationDevice.h"
#include "data.h"

//#include "control_panel.h"
#include "common_stuff.h"

namespace LocalDriver
{
	using Common::FileProxy;
	using Ui::NotificationDevice;
	using Ui::Notification;

	class LVFSDriver : public ILVFSDriver
	{
		Q_OBJECT

	private:
		//LVFSDriver(void);
		~LVFSDriver(void);
		LVFSDriver(FileProxy * pProxy);
		LVFSDriver(const LVFSDriver &); 
		LVFSDriver& operator=(const LVFSDriver &); 
		void run();
	
	public slots:
			virtual void mount(Data::GeneralSettings& generalSettings);
			virtual void unmount();

	private:	//filesystem calls
		//static bool __stdcall 
		//	DokanCreateEmptyFile(LPCWSTR FileName);

		static int __stdcall 
			DokanCreateFile(
			LPCWSTR					FileName,
			DWORD					AccessMode,
			DWORD					ShareMode,
			DWORD					CreationDisposition,
			DWORD					FlagsAndAttributes,
			PDOKAN_FILE_INFO		DokanFileInfo);

		static int __stdcall
			DokanCreateDirectory(
			LPCWSTR					FileName,
			PDOKAN_FILE_INFO		DokanFileInfo);

		static int __stdcall
			DokanOpenDirectory(
			LPCWSTR					FileName,
			PDOKAN_FILE_INFO		DokanFileInfo);

		static int __stdcall
			DokanCloseFile(
			LPCWSTR					FileName,
			PDOKAN_FILE_INFO		DokanFileInfo);

		static int __stdcall
			DokanCleanup(
			LPCWSTR					FileName,
			PDOKAN_FILE_INFO		DokanFileInfo);

		static int __stdcall
			DokanReadFile(
			LPCWSTR				FileName,
			LPVOID				Buffer,
			DWORD				BufferLength,
			LPDWORD				ReadLength,
			LONGLONG			Offset,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanWriteFile(
			LPCWSTR		FileName,
			LPCVOID		Buffer,
			DWORD		NumberOfBytesToWrite,
			LPDWORD		NumberOfBytesWritten,
			LONGLONG			Offset,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanFlushFileBuffers(
			LPCWSTR		FileName,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanGetFileInformation(
			LPCWSTR							FileName,
			LPBY_HANDLE_FILE_INFORMATION	HandleFileInformation,
			PDOKAN_FILE_INFO				DokanFileInfo);

		static int __stdcall
			DokanFindFiles(
			LPCWSTR				FileName,
			PFillFindData		FillFindData, // function pointer
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanDeleteFile(
			LPCWSTR				FileName,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanDeleteDirectory(
			LPCWSTR				FileName,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanMoveFile(
			LPCWSTR				FileName, // existing file name
			LPCWSTR				NewFileName,
			BOOL				ReplaceIfExisting,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanLockFile(
			LPCWSTR				FileName,
			LONGLONG			ByteOffset,
			LONGLONG			Length,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanSetEndOfFile(
			LPCWSTR				FileName,
			LONGLONG			ByteOffset,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanSetAllocationSize(
			LPCWSTR				FileName,
			LONGLONG			AllocSize,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanSetFileAttributes(
			LPCWSTR				FileName,
			DWORD				FileAttributes,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanSetFileTime(
			LPCWSTR				FileName,
			CONST FILETIME*		CreationTime,
			CONST FILETIME*		LastAccessTime,
			CONST FILETIME*		LastWriteTime,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanUnlockFile(
			LPCWSTR				FileName,
			LONGLONG			ByteOffset,
			LONGLONG			Length,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanGetFileSecurity(
			LPCWSTR					FileName,
			PSECURITY_INFORMATION	SecurityInformation,
			PSECURITY_DESCRIPTOR	SecurityDescriptor,
			ULONG				BufferLength,
			PULONG				LengthNeeded,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanSetFileSecurity(
			LPCWSTR					FileName,
			PSECURITY_INFORMATION	SecurityInformation,
			PSECURITY_DESCRIPTOR	SecurityDescriptor,
			ULONG				SecurityDescriptorLength,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanGetVolumeInformation(
			LPWSTR		VolumeNameBuffer,
			DWORD		VolumeNameSize,
			LPDWORD		VolumeSerialNumber,
			LPDWORD		MaximumComponentLength,
			LPDWORD		FileSystemFlags,
			LPWSTR		FileSystemNameBuffer,
			DWORD		FileSystemNameSize,
			PDOKAN_FILE_INFO	DokanFileInfo);

		static int __stdcall
			DokanWMUnmount(
			PDOKAN_FILE_INFO	DokanFileInfo);

		static void
			GetFilePath(
			PWCHAR	filePath,
			ULONG	numberOfElements,
			LPCWSTR FileName);

	private:
		static FileProxy* _pFileProxy;
		//static NotificationDevice* _pNotificationDevice;

		static LVFSDriver* _pDriverInstance;
		
		static PDOKAN_OPERATIONS _pDriverOperations;
		static PDOKAN_OPTIONS _pDriverOptions;
		static QMutex _DriverMutex;

		static QString _rootDirectory;
		static QString _mountPoint;

	public:
		static LVFSDriver* createDriver(FileProxy*);
	
	private:
		static void startDriver();
		static void stopDriver();

signals:
		//void showDriverMessage(const Notification&);
		//void mounted();
		//void unmounted();
	};
}
#endif //LVFSDriver_H