#include "xadClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Undef XADCLIENTVERSTR to provide our own */
#undef XADCLIENTVERSTR
#define XADCLIENTVERSTR(text)

/* Pull in the client source */
#include "xad_exe.c"

static void print_entry(struct xadFileInfo *fi, int indent)
{
  int i;
  for (i = 0; i < indent; i++) putchar(' ');
  printf("file: %s  size: %llu  pos: %llu  flags: 0x%08lx\n",
    fi->xfi_FileName,
    (unsigned long long)fi->xfi_Size,
    (unsigned long long)fi->xfi_DataPos,
    (unsigned long)fi->xfi_Flags);
}

int main(int argc, char **argv)
{
  FILE *f;
  xadUINT8 header[256];
  xadSize fileSize;
  struct xadMasterBase mb;
  struct xadArchiveInfo ai;
  struct xadFileInfo *fi;
  xadBOOL rc;
  xadERROR err;
  int count = 0;

  if (argc < 2) {
    fprintf(stderr, "usage: %s <pe-file> [extract-entry]\n", argv[0]);
    return 1;
  }

  f = fopen(argv[1], "rb");
  if (!f) { perror("fopen"); return 1; }

  fseek(f, 0, SEEK_END);
  fileSize = ftell(f);
  fseek(f, 0, SEEK_SET);

  memset(&ai, 0, sizeof(ai));
  ai.xai_InSize = fileSize;
  ai.xai_PrivateClient = (xadPTR)f;
  memset(&mb, 0, sizeof(mb));

  /* Recognition */
  memset(header, 0, sizeof(header));
  fread(header, 1, EXE_RECOGSIZE > fileSize ? (size_t)fileSize : EXE_RECOGSIZE, f);
  fseek(f, 0, SEEK_SET);

  rc = exe_RecogData(EXE_RECOGSIZE > fileSize ? (size_t)fileSize : EXE_RECOGSIZE,
    header, &mb);
  if (!rc) {
    fprintf(stderr, "Not a PE file\n");
    fclose(f);
    return 1;
  }
  printf("Recognized as PE file\n\n");

  /* GetInfo */
  err = exe_GetInfo(&ai, &mb);
  fseek(f, 0, SEEK_SET);  /* reset for extraction */

  if (err && err != XADERR_EMPTY) {
    fprintf(stderr, "GetInfo error: 0x%04x\n", err);
    fclose(f);
    return 1;
  }

  /* List entries */
  printf("Entries listed by GetInfo:\n");
  for (fi = ai.xai_FileInfo; fi; fi = fi->xfi_Next) {
    print_entry(fi, 2);
    count++;
  }
  printf("Total: %d entries\n\n", count);

  /* Extract a specific entry? */
  if (argc >= 3) {
    int entryNum = atoi(argv[2]);
    struct xadFileInfo *target = ai.xai_FileInfo;
    while (target && target->xfi_EntryNumber != (xadUINT32)entryNum)
      target = target->xfi_Next;
    if (!target) {
      fprintf(stderr, "Entry %d not found\n", entryNum);
    } else {
      printf("Extracting entry %d (%s)...\n", entryNum, target->xfi_FileName);
      fseek(f, (long)target->xfi_DataPos, SEEK_SET);
      ai.xai_CurFile = target;
      ai.xai_InPos = target->xfi_DataPos;
      ai.xai_OutSize = 0;
      ai.xai_PrivateClient = (xadPTR)f;
      err = exe_UnArchive(&ai, &mb);
      if (err) fprintf(stderr, "Extract error: 0x%04x\n", err);
      else printf("Extracted %llu bytes\n", (unsigned long long)target->xfi_Size);
    }
  }

  fclose(f);
  return 0;
}
