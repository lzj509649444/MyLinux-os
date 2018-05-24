;fat12 file system header
; ----------------------------------------------------------------------
BS_OEMName	DB 'lzj317bb'	; OEM name ,should be 8 characters

BPB_BytsPerSec	DW 512		; bytes per sector
BPB_SecPerClus	DB 1		; sectors per cluster
BPB_RsvdSecCnt	DW 1		; how many sectors boot code reserved
BPB_NumFATs	    DB 2		; how many fat table
BPB_RootEntCnt	DW 224		; maximum of root directory
BPB_TotSec16	DW 2880		; logic sectors
BPB_Media	    DB 0xF0		; media descriptor
BPB_FATSz16 	DW 9		; sectors of one fat table reserved
BPB_SecPerTrk	DW 18		; sectors of per track
BPB_NumHeads	DW 2		; headers
BPB_HiddSec 	DD 0		; hidden sectors
BPB_TotSec32	DD 0		;

BS_DrvNum	    DB 0		; dirver id
BS_Reserved1	DB 0		; unused
BS_BootSig	    DB 29h		; extended sign of boot (29h)
BS_VolID	    DD 0		; volume series
BS_VolLab	    DB 'lzj317bb.01'    ; volume id,should be 11 characters
BS_FileSysType	DB 'FAT12   '	    ; file system type,should be 8 characters

