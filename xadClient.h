#ifndef XADMASTER_XADCLIENT_H
#define XADMASTER_XADCLIENT_H

#include "xadmaster.h"

#include <ctype.h>
#include <string.h>
#include "ConvertE.c"

#define XADRECOGDATA(name) static xadBOOL name##_RecogData( \
          xadSize size, \
          const xadUINT8 *data, \
          struct xadMasterBase *xadMasterBase)

#define XADGETINFO(name)   static xadERROR name##_GetInfo( \
          struct xadArchiveInfo *ai, \
          struct xadMasterBase *xadMasterBase)

#define XADUNARCHIVE(name) static xadERROR name##_UnArchive( \
          struct xadArchiveInfo *ai, \
          struct xadMasterBase *xadMasterBase)

#define XADFREE(name)      static void name##_Free( \
          struct xadArchiveInfo *ai, \
          struct xadMasterBase *xadMasterBase)

#define XADRECOGDATAP(name) (xadBOOL (*)()) name##_RecogData
#define XADGETINFOP(name)   (xadERROR (*)()) name##_GetInfo
#define XADUNARCHIVEP(name) (xadERROR (*)()) name##_UnArchive
#define XADFREEP(name)      (void (*)()) name##_Free

#ifdef XADMASTERFILE
  #define XADFIRSTCLIENT(name)  static const struct xadClient name##_Client =
  #define XADCLIENTVERSTR(text)
#else
  #define XADFIRSTCLIENT(name)  const struct xadClient FirstClient =
  #define XADCLIENTVERSTR(text)                                         \
  const xadSTRING *xad_GetClientVersion() { return text; }              \
  extern const struct xadClient FirstClient;                            \
  const struct xadClient *xad_GetClient() { return &FirstClient; }
#endif

#define XADCLIENT(name)         static const struct xadClient name##_Client =
#define XADNEXTCLIENT           0

#define XADNEXTCLIENTNAME(name) (struct xadClient *) &name##_Client

#endif
