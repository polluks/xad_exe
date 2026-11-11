#ifndef LIBRARIES_XADMASTER_H
#define LIBRARIES_XADMASTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdarg.h>

typedef uint32_t           xadUINT32;
typedef int32_t            xadINT32;
typedef uint16_t           xadUINT16;
typedef int16_t            xadINT16;
typedef uint8_t            xadUINT8;
typedef int8_t             xadINT8;
typedef uint64_t           xadSize;
typedef int64_t            xadSignSize;
typedef void *             xadPTR;
typedef char               xadSTRING;
typedef xadSTRING *        xadSTRPTR;
typedef unsigned int       xadUINT;
typedef int                xadINT;
typedef xadINT             xadERROR;
typedef xadINT             xadBOOL;
typedef xadUINT32          xadTag;

#define XADFALSE        0
#define XADTRUE         1
#define XADMEMF_ANY     0
#define XADMEMF_CLEAR   (1L << 16)
#define XADMEMF_PUBLIC  (1L << 0)

#define TAG_DONE   ((xadUINT32)0x50000000UL)

struct TagItem {
  xadUINT32 ti_Tag;
  xadSize   ti_Data;
};

#define XADM

struct xadMasterBase {
  xadPTR dummy;
};

struct xadDate {
  xadUINT32 xd_Micros;
  xadINT32  xd_Year;
  xadUINT8  xd_Month;
  xadUINT8  xd_WeekDay;
  xadUINT8  xd_Day;
  xadUINT8  xd_Hour;
  xadUINT8  xd_Minute;
  xadUINT8  xd_Second;
};

struct xadClient;
struct xadArchiveInfo;
struct xadDiskInfo;
struct xadTextInfo;
struct xadSkipInfo;
struct xadImageInfo;

struct xadFileInfo {
  struct xadFileInfo * xfi_Next;
  xadUINT32            xfi_EntryNumber;
  xadSTRPTR            xfi_EntryInfo;
  xadPTR               xfi_PrivateInfo;
  xadUINT32            xfi_Flags;
  xadSTRPTR            xfi_FileName;
  xadSTRPTR            xfi_Comment;
  xadUINT32            xfi_Protection;
  xadUINT32            xfi_OwnerUID;
  xadUINT32            xfi_OwnerGID;
  xadSTRPTR            xfi_UserName;
  xadSTRPTR            xfi_GroupName;
  xadSize              xfi_Size;
  xadSize              xfi_GroupCrSize;
  xadSize              xfi_CrunchSize;
  xadSTRPTR            xfi_LinkName;
  struct xadDate       xfi_Date;
  xadUINT16            xfi_Generation;
  xadSize              xfi_DataPos;
  struct xadFileInfo * xfi_MacFork;
  xadUINT16            xfi_UnixProtect;
  xadUINT8             xfi_DosProtect;
  xadUINT8             xfi_FileType;
};

struct xadClient {
  struct xadClient * xc_Next;
  xadUINT16          xc_Version;
  xadUINT16          xc_MasterVersion;
  xadUINT16          xc_ClientVersion;
  xadUINT16          xc_ClientRevision;
  xadSize            xc_RecogSize;
  xadUINT32          xc_Flags;
  xadUINT32          xc_Identifier;
  xadSTRPTR          xc_ArchiverName;
  xadBOOL         (* xc_RecogData)(xadSize size, const xadUINT8 *data,
                  struct xadMasterBase *xadMasterBase);
  xadERROR        (* xc_GetInfo)(struct xadArchiveInfo *ai,
                  struct xadMasterBase *xadMasterBase);
  xadERROR        (* xc_UnArchive)(struct xadArchiveInfo *ai,
                  struct xadMasterBase *xadMasterBase);
  void            (* xc_Free)(struct xadArchiveInfo *ai,
                  struct xadMasterBase *xadMasterBase);
};

struct xadArchiveInfo {
  struct xadClient *   xai_Client;
  xadPTR               xai_PrivateClient;
  xadSTRPTR            xai_Password;
  xadUINT32            xai_Flags;
  xadUINT32            xai_LowCyl;
  xadUINT32            xai_HighCyl;
  xadSize              xai_InPos;
  xadSize              xai_InSize;
  xadSize              xai_OutPos;
  xadSize              xai_OutSize;
  struct xadFileInfo * xai_FileInfo;
  struct xadDiskInfo * xai_DiskInfo;
  struct xadFileInfo * xai_CurFile;
  struct xadDiskInfo * xai_CurDisk;
  xadERROR             xai_LastError;
  xadSize *            xai_MultiVolume;
  struct xadSkipInfo * xai_SkipInfo;
  struct xadImageInfo *xai_ImageInfo;
  xadSTRPTR            xai_InName;
};

struct xadDiskInfo {
  struct xadDiskInfo * xdi_Next;
  xadUINT32            xdi_EntryNumber;
  xadSTRPTR            xdi_EntryInfo;
  xadPTR               xdi_PrivateInfo;
  xadUINT32            xdi_Flags;
  xadUINT32            xdi_SectorSize;
  xadUINT32            xdi_TotalSectors;
  xadUINT32            xdi_Cylinders;
  xadUINT32            xdi_CylSectors;
  xadUINT32            xdi_Heads;
  xadUINT32            xdi_TrackSectors;
  xadUINT32            xdi_LowCyl;
  xadUINT32            xdi_HighCyl;
  xadUINT32            xdi_BlockInfoSize;
  xadUINT8 *           xdi_BlockInfo;
  struct xadTextInfo * xdi_TextInfo;
  xadSize              xdi_DataPos;
};

#define XADOBJ_FILEINFO         0x0002
#define XAD_OBJNAMESIZE         0x40000014UL

#define XADFIB_SEEKDATAPOS      0
#define XADFIB_EXTRACTONBUILD   4
#define XADFIF_SEEKDATAPOS      (1L<<XADFIB_SEEKDATAPOS)
#define XADFIF_EXTRACTONBUILD   (1L<<XADFIB_EXTRACTONBUILD)

#define XADCF_FILEARCHIVER      (1UL<<0)
#define XADCF_EXECRUNCHER       (1UL<<6)
#define XADCF_FREEFILEINFO      (1UL<<30)

#define XADAC_READ              10
#define XADAC_COPY              12
#define XADAC_INPUTSEEK         13

#define XADERR_OK               0x0000
#define XADERR_ILLEGALDATA      0x0006
#define XADERR_NOTSUPPORTED     0x0007
#define XADERR_NOMEMORY         0x0005
#define XADERR_EMPTY            0x0015
#define XADERR_BREAK            0x000D

#define XAD_DATEUNIX            0x40000046UL
#define XAD_MAKELOCALDATE       0x4000005BUL
#define XAD_GETDATEXADDATE      0x40000053UL

#define XADCLIENT_VERSION       1

xadPTR xadAllocObjectA(xadUINT32 type, ...);
xadPTR xadAllocVec(xadSize size, xadUINT32 flags);
void xadFreeObjectA(xadPTR obj, xadUINT32 flags);
xadERROR xadAddFileEntryA(struct xadFileInfo *fi, struct xadArchiveInfo *ai, ...);
xadERROR xadHookAccess(xadUINT32 command, xadSignSize data, xadPTR buffer, struct xadArchiveInfo *ai, ...);
void xadCopyMem(const void *s, xadPTR d, xadSize size);
xadERROR xadConvertDates(xadUINT32 intag, ...);

#ifdef __cplusplus
}
#endif

#endif
