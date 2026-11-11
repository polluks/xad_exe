#ifndef XADMASTER_EXE_C
#define XADMASTER_EXE_C

#include "xadClient.h"
#include <stdio.h>

#ifndef XADMASTERVERSION
#define XADMASTERVERSION 13
#endif

#define XADCID_EXE 5051

#define GET_LE16 EndGetI16
#define GET_LE32 EndGetI32

#define EXE_VERSION    1
#define EXE_REVISION   0
#define EXE_RECOGSIZE  256

XADCLIENTVERSTR("exe 1.0 (30.07.2026)")

static const xadSTRING *peRsrcTypeName(xadUINT32 id)
{
  switch (id) {
    case 1:  return "CURSOR";
    case 2:  return "BITMAP";
    case 3:  return "ICON";
    case 4:  return "MENU";
    case 5:  return "DIALOG";
    case 6:  return "STRING";
    case 7:  return "FONTDIR";
    case 8:  return "FONT";
    case 9:  return "ACCELERATOR";
    case 10: return "RCDATA";
    case 11: return "MESSAGETABLE";
    case 12: return "GROUP_CRSR";
    case 14: return "GROUP_ICON";
    case 16: return "VERSION";
    case 17: return "DLGINCLUDE";
    case 19: return "PLUGPLAY";
    case 20: return "VXD";
    case 21: return "ANICURSOR";
    case 22: return "ANIICON";
    case 23: return "HTML";
    case 24: return "MANIFEST";
    default: return NULL;
  }
}

static xadUINT32 sectNameLen(const xadUINT8 *name)
{
  xadUINT32 i;
  for (i = 0; i < 8 && name[i]; ++i);
  return i;
}

static xadINT32 walkResourceDir(
  struct xadArchiveInfo *ai,
  xadUINT32 rsrcVA,
  xadUINT32 rsrcRaw,
  xadUINT32 dirOff,
  xadINT32 level,
  xadUINT32 typeID,
  xadUINT32 nameID,
  xadUINT32 timeDateStamp,
  xadUINT32 *entryNumber)
{
  xadUINT8 hdr[16], entry[8];
  xadUINT32 i, numNamed, numID, numTotal;
  xadUINT32 entryName, subOff;
  xadINT32 err;

  if (level > 3) return 0;

  err = xadHookAccess(XADM XADAC_INPUTSEEK, rsrcRaw + dirOff, 0, ai);
  if (err) return err;

  err = xadHookAccess(XADM XADAC_READ, 16, hdr, ai);
  if (err) return err;

  numNamed = GET_LE16(hdr + 12);
  numID = GET_LE16(hdr + 14);
  numTotal = numNamed + numID;

  for (i = 0; i < numTotal; ++i)
  {
    xadUINT32 thisTypeID, thisNameID;
    xadBOOL isNamed;

    err = xadHookAccess(XADM XADAC_READ, 8, entry, ai);
    if (err) return err;

    entryName = GET_LE32(entry);
    subOff = GET_LE32(entry + 4);
    isNamed = (entryName & 0x80000000) ? 1 : 0;

    thisTypeID = (level == 0) ? (entryName & 0x7FFFFFFF) : typeID;
    thisNameID = (level == 1) ? (entryName & 0x7FFFFFFF) : nameID;

    if (subOff & 0x80000000)
    {
      err = walkResourceDir(ai, rsrcVA, rsrcRaw,
        subOff & 0x7FFFFFFF, level + 1,
        thisTypeID, thisNameID,
        timeDateStamp, entryNumber);
      if (err) return err;
    }
    else if (level >= 2)
    {
      xadUINT8 dataEntry[8];
      xadUINT32 dataRVA, dataSize, dataOff;
      char fname[128];
      struct xadFileInfo *fi;
      xadINT32 flen;
      const xadSTRING *tname;
      xadUINT32 langID;

      err = xadHookAccess(XADM XADAC_INPUTSEEK, rsrcRaw + (subOff & 0x7FFFFFFF), 0, ai);
      if (err) return err;

      err = xadHookAccess(XADM XADAC_READ, 8, dataEntry, ai);
      if (err) return err;

      dataRVA = GET_LE32(dataEntry);
      dataSize = GET_LE32(dataEntry + 4);
      dataOff = dataRVA - rsrcVA + rsrcRaw;

      langID = isNamed ? 0 : entryName;

      tname = peRsrcTypeName(thisTypeID);
      if (tname)
        flen = sprintf(fname, "rsrc/%s/%lu/%lu",
          tname, (unsigned long)thisNameID, (unsigned long)langID);
      else
        flen = sprintf(fname, "rsrc/%lu/%lu/%lu",
          (unsigned long)thisTypeID, (unsigned long)thisNameID, (unsigned long)langID);

      if (flen <= 0) continue;

      fi = (struct xadFileInfo *)xadAllocObjectA(XADM XADOBJ_FILEINFO,
        XAD_OBJNAMESIZE, (xadUINT32)flen + 1, TAG_DONE);
      if (!fi) return XADERR_NOMEMORY;

      xadCopyMem(XADM fname, fi->xfi_FileName, (xadUINT32)flen);
      fi->xfi_FileName[flen] = 0;

      fi->xfi_DataPos = dataOff;
      fi->xfi_Size = dataSize;
      fi->xfi_EntryNumber = (*entryNumber)++;
      fi->xfi_Flags = XADFIF_SEEKDATAPOS | XADFIF_EXTRACTONBUILD;

      xadConvertDates(XADM XAD_DATEUNIX, timeDateStamp,
        XAD_MAKELOCALDATE, 1,
        XAD_GETDATEXADDATE, &fi->xfi_Date, TAG_DONE);

      err = xadAddFileEntryA(XADM fi, ai, TAG_DONE);
      if (err) return err;
    }
  }
  return 0;
}

XADRECOGDATA(exe)
{
  (void)xadMasterBase;
  xadUINT32 peOff;

  if (size < 128) return 0;
  if (data[0] != 'M' || data[1] != 'Z') return 0;

  peOff = GET_LE32(data + 0x3C);
  if (peOff + 4 > size || peOff < 64) return 0;

  if (data[peOff] == 'P' && data[peOff+1] == 'E' &&
      data[peOff+2] == 0 && data[peOff+3] == 0)
    return 1;

  return 0;
}

XADGETINFO(exe)
{
  (void)xadMasterBase;
  xadUINT8 buf[248];
  xadUINT32 i, numSects, peOff, sectOff, optHdrSz;
  xadUINT32 rsrcVA = 0, rsrcRaw = 0;
  xadUINT32 timeDateStamp;
  xadINT32 err;

  err = xadHookAccess(XADM XADAC_INPUTSEEK, 0x3C, 0, ai);
  if (err) return XADERR_ILLEGALDATA;

  err = xadHookAccess(XADM XADAC_READ, 4, buf, ai);
  if (err) return XADERR_ILLEGALDATA;
  peOff = GET_LE32(buf);

  err = xadHookAccess(XADM XADAC_INPUTSEEK, peOff + 4, 0, ai);
  if (err) return XADERR_ILLEGALDATA;

  err = xadHookAccess(XADM XADAC_READ, 20, buf, ai);
  if (err) return XADERR_ILLEGALDATA;

  numSects = GET_LE16(buf + 2);
  timeDateStamp = GET_LE32(buf + 4);
  optHdrSz = GET_LE16(buf + 16);
  sectOff = peOff + 4 + 20 + optHdrSz;

  for (i = 0; i < numSects; ++i)
  {
    xadUINT8 sect[40];
    xadUINT32 nameLen, virtAddr, rawSize, rawOff;
    struct xadFileInfo *fi;
    xadUINT32 ns;

    err = xadHookAccess(XADM XADAC_INPUTSEEK, sectOff + i * 40, 0, ai);
    if (err) return err;

    err = xadHookAccess(XADM XADAC_READ, 40, sect, ai);
    if (err) return err;

    nameLen = sectNameLen(sect);
    if (nameLen > 7) nameLen = 7;

    ns = nameLen;
    if (sect[0] != '.' && sect[0] != '_')
      ns = nameLen + 1;

    if (!(fi = (struct xadFileInfo *)xadAllocObjectA(XADM XADOBJ_FILEINFO,
      XAD_OBJNAMESIZE, ns + 1, TAG_DONE)))
      return XADERR_NOMEMORY;

    if (sect[0] == '.' || sect[0] == '_')
    {
      xadCopyMem(XADM sect + ((sect[0] == '.') ? 1 : 0),
        fi->xfi_FileName, nameLen);
      fi->xfi_FileName[nameLen] = 0;
    }
    else
    {
      fi->xfi_FileName[0] = '_';
      xadCopyMem(XADM sect, fi->xfi_FileName + 1, nameLen);
      fi->xfi_FileName[nameLen + 1] = 0;
    }

    virtAddr = GET_LE32(sect + 12);
    rawSize = GET_LE32(sect + 16);
    rawOff = GET_LE32(sect + 20);

    fi->xfi_DataPos = rawOff;
    fi->xfi_Size = rawSize;
    fi->xfi_EntryNumber = i;
    fi->xfi_Flags = XADFIF_SEEKDATAPOS | XADFIF_EXTRACTONBUILD;

    xadConvertDates(XADM XAD_DATEUNIX, timeDateStamp,
      XAD_MAKELOCALDATE, 1,
      XAD_GETDATEXADDATE, &fi->xfi_Date, TAG_DONE);

    if ((sect[0] == '.' || sect[0] == '_') &&
        (sect[1] == 'r' || sect[1] == 'R') &&
        (sect[2] == 's' || sect[2] == 'S') &&
        (sect[3] == 'r' || sect[3] == 'R') &&
        (sect[4] == 'c' || sect[4] == 'C'))
    {
      rsrcVA = virtAddr;
      rsrcRaw = rawOff;
    }

    err = xadAddFileEntryA(XADM fi, ai, TAG_DONE);
    if (err) return err;
  }

  if (rsrcVA && rsrcRaw)
  {
    xadUINT32 entryNumber = numSects;
    err = walkResourceDir(ai, rsrcVA, rsrcRaw, 0, 0, 0, 0,
      timeDateStamp, &entryNumber);
    if (err) return err;
  }

  if (!ai->xai_FileInfo)
    return XADERR_EMPTY;

  return 0;
}

XADFREE(exe)
{
  (void)ai;
  (void)xadMasterBase;
}

XADUNARCHIVE(exe)
{
  (void)xadMasterBase;
  return xadHookAccess(XADM XADAC_COPY, ai->xai_CurFile->xfi_Size, 0, ai);
}

XADFIRSTCLIENT(exe)
{
  XADNEXTCLIENT,
  XADCLIENT_VERSION,
  XADMASTERVERSION,
  EXE_VERSION,
  EXE_REVISION,
  EXE_RECOGSIZE,
  XADCF_FILEARCHIVER | XADCF_FREEFILEINFO,
  XADCID_EXE,
  "exe",
  XADRECOGDATAP(exe),
  XADGETINFOP(exe),
  XADUNARCHIVEP(exe),
  XADFREEP(exe)
};

#undef XADNEXTCLIENT
#define XADNEXTCLIENT XADNEXTCLIENTNAME(exe)

#endif
