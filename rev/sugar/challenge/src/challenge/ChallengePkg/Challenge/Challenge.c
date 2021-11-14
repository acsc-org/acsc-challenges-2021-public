#include <Uefi.h>

#include <Library/BaseCryptLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>

#include <Protocol/BlockIo.h>

CONST UINT8 KEY[16]     = {0xa1, 0x86, 0x28, 0x23, 0x14, 0xbb, 0x20, 0x35, 0x3f, 0xea, 0x9f, 0xb3, 0xb0, 0x9e, 0xf6, 0xcd};
CONST UINT8 IV[16]      = {0x00};

EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
    EFI_STATUS Status;

    gST->ConOut->ClearScreen (gST->ConOut);
    Print (L"Input flag: ");

    CHAR16 Flag[256];
    ZeroMem (Flag, sizeof(Flag));

    EFI_INPUT_KEY Key;
    for (int i = 0; i < sizeof(Flag) / sizeof(Flag[0]) - 1; i++) {
        gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, NULL);
        gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);

        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
            break;
        }

        Flag[i] = Key.UnicodeChar;
        gST->ConOut->OutputString(SystemTable->ConOut, &Flag[i]);
    }
    Print (L"\n");

    if (StrLen(Flag) != 5 + 32 + 1) {
        Print(L"Wrong!\n");
        goto end;
    }
    if (StrnCmp(L"ACSC{", Flag, 5) != 0) {
        Print(L"Wrong!\n");
        goto end;
    }
    if (StrnCmp(L"}", &Flag[5 + 32], 1) != 0) {
        Print(L"Wrong!\n");
        goto end;
    }

    EFI_DEVICE_PATH_PROTOCOL *DevicePath = ConvertTextToDevicePath (
        L"PciRoot(0x0)/Pci(0x1,0x1)/Ata(0x0)"
    );

    EFI_HANDLE BlockIoHandle;
    Status = gBS->LocateDevicePath(
        &gEfiBlockIoProtocolGuid,
        &DevicePath,
        &BlockIoHandle
    );
    if (EFI_ERROR(Status)) {
        Print(L"ERROR: gBS->LocateDevicePath() failed: %r\n", Status);
        goto end;
    }

    EFI_BLOCK_IO_PROTOCOL *BlockIo;
    Status = gBS->HandleProtocol (
        BlockIoHandle,
        &gEfiBlockIoProtocolGuid,
        (VOID **) &BlockIo
    );
    if (EFI_ERROR(Status)) {
        Print(L"ERROR: gBS->HandleProtocol() failed: %r\n", Status);
        goto end;
    }

    UINT8 Block[512];
    Status = BlockIo->ReadBlocks (
        BlockIo,
        BlockIo->Media->MediaId,
        1,
        sizeof(Block),
        (VOID*) Block
    );
    if (EFI_ERROR(Status)) {
        Print(L"ERROR: BlockIo->ReadBlocks() failed: %r\n", Status);
        goto end;
    }

    if (*((UINT64*)Block) != 0x5452415020494645ULL) {
        Print(L"ERROR: Header signature mismatch.\n");
        goto end;
    }

    VOID* AesContext = AllocateZeroPool (AesGetContextSize());
    if (!AesInit (AesContext, KEY, sizeof(KEY) * 8)) {
        Print(L"ERROR: AesInit() failed.\n");
        goto end;
    }
	UINT8 Output[16];
    if (!AesCbcEncrypt (
		AesContext,
		&Block[0x38],   // Disk GUID
		16,
		IV,
		Output)) {
        Print(L"ERROR: AesCbcEncrypt() failed.\n");
        goto end;
    }
	FreePool (AesContext);

    UINT8 FlagBin[16];
    Status = StrHexToBytes (&Flag[5], 32, FlagBin, sizeof(FlagBin));
    if (EFI_ERROR(Status)) {
        Print(L"ERROR: StrHexToBytes() failed: %r\n", Status);
        goto end;
    }

    if (CompareMem(Output, FlagBin, sizeof(Output)) != 0) {
        Print(L"Wrong!\n");
        goto end;
    }

    Print(L"Correct!\n");

end:
    gRT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

    return EFI_SUCCESS;
}
