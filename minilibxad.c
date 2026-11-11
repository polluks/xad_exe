#include "xadmaster.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

xadPTR xadAllocObjectA(xadUINT32 type, ...)
{
  if (type == XADOBJ_FILEINFO) {
    va_list ap;
    xadUINT32 tag;
    xadUINT32 namesize = 0;

    va_start(ap, type);
    while ((tag = (xadUINT32)va_arg(ap, xadUINT32)) != TAG_DONE) {
      if (tag == XAD_OBJNAMESIZE)
        namesize = (xadUINT32)va_arg(ap, xadUINT32);
      else
        (void)va_arg(ap, void *);
    }
    va_end(ap);

    struct xadFileInfo *fi = calloc(1, sizeof(struct xadFileInfo));
    if (!fi) return 0;
    if (namesize > 0) {
      fi->xfi_FileName = calloc(1, namesize);
      if (!fi->xfi_FileName) { free(fi); return 0; }
    }
    return fi;
  }
  return 0;
}

xadPTR xadAllocVec(xadSize size, xadUINT32 flags)
{
  (void)flags;
  return calloc(1, (size_t)size);
}

void xadFreeObjectA(xadPTR obj, xadUINT32 flags)
{
  (void)flags;
  if (obj) {
    struct xadFileInfo *fi = (struct xadFileInfo *)obj;
    if (fi->xfi_FileName) free(fi->xfi_FileName);
    if (fi->xfi_Comment) free(fi->xfi_Comment);
    if (fi->xfi_EntryInfo) free(fi->xfi_EntryInfo);
    free(fi);
  }
}

xadERROR xadAddFileEntryA(struct xadFileInfo *fi, struct xadArchiveInfo *ai, ...)
{
  struct xadFileInfo **pp = &ai->xai_FileInfo;
  while (*pp) pp = &(*pp)->xfi_Next;
  *pp = fi;
  fi->xfi_Next = 0;
  return XADERR_OK;
}

xadERROR xadHookAccess(xadUINT32 command, xadSignSize data, xadPTR buffer,
  struct xadArchiveInfo *ai, ...)
{
  FILE *f;
  if (!ai->xai_PrivateClient) return XADERR_NOTSUPPORTED;
  f = (FILE *)ai->xai_PrivateClient;
  switch (command) {
    case XADAC_INPUTSEEK:
      if (fseek(f, data, SEEK_SET) != 0) return XADERR_ILLEGALDATA;
      ai->xai_InPos = ftell(f);
      return XADERR_OK;
    case XADAC_READ:
      if (fread(buffer, 1, (size_t)data, f) != (size_t)data)
        return XADERR_ILLEGALDATA;
      ai->xai_InPos = ftell(f);
      return XADERR_OK;
    case XADAC_COPY: {
      xadSize total = data;
      xadUINT8 buf[65536];
      while (total > 0) {
        xadSize chunk = total > (xadSize)sizeof(buf) ? (xadSize)sizeof(buf) : total;
        size_t got = fread(buf, 1, (size_t)chunk, f);
        if (got == 0) return XADERR_ILLEGALDATA;
        fwrite(buf, 1, got, stdout);
        total -= (xadSize)got;
      }
      ai->xai_InPos = ftell(f);
      ai->xai_OutSize += data;
      return XADERR_OK;
    }
    default:
      return XADERR_NOTSUPPORTED;
  }
}

void xadCopyMem(const void *s, xadPTR d, xadSize size)
{
  memmove(d, s, (size_t)size);
}

xadERROR xadConvertDates(xadUINT32 intag, ...)
{
  (void)intag;
  return XADERR_OK;
}

const xadSTRING *xad_GetClientVersion(void)
{
  return "exe test client 1.0";
}
