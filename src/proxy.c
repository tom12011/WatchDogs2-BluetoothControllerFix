#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <xinput.h>

typedef DWORD (WINAPI *PFN_XInputGetState)(DWORD, XINPUT_STATE *);
typedef DWORD (WINAPI *PFN_XInputSetState)(DWORD, XINPUT_VIBRATION *);
typedef DWORD (WINAPI *PFN_XInputGetCapabilities)(DWORD, DWORD, XINPUT_CAPABILITIES *);
typedef DWORD (WINAPI *PFN_XInputGetDSoundAudioDeviceGuids)(DWORD, GUID *, GUID *);

#ifndef WD2_DISRUPT_TIMESTAMP
#define WD2_DISRUPT_TIMESTAMP 0x59774D30u
#endif
#ifndef WD2_DISRUPT_SIZE_OF_IMAGE
#define WD2_DISRUPT_SIZE_OF_IMAGE 0x0C0B3000u
#endif
#ifndef WD2_CLASSIFIER_BRANCH_RVA
#define WD2_CLASSIFIER_BRANCH_RVA 0x068D077Bu
#endif
#ifndef WD2_CLASSIFIER_CAVE_RVA
#define WD2_CLASSIFIER_CAVE_RVA 0x068DAD60u
#endif

static const BYTE g_original_branch[6] = { 0x0F, 0x84, 0xA3, 0x00, 0x00, 0x00 };
static const BYTE g_patched_branch[6]  = { 0x0F, 0x84, 0xDF, 0xA5, 0x00, 0x00 };
static const BYTE g_original_cave[25] = {
    0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
    0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
    0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC
};
static const BYTE g_patched_cave[25] = {
    0xC7, 0x87, 0x88, 0x25, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0xC7, 0x87, 0x8C, 0x25, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0xE9, 0x08, 0x5A, 0xFF, 0xFF
};

static BOOL g_patch_applied;
static HMODULE g_real_module;
static PFN_XInputGetState g_real_get_state;
static PFN_XInputSetState g_real_set_state;
static PFN_XInputGetCapabilities g_real_get_capabilities;
static PFN_XInputGetDSoundAudioDeviceGuids g_real_get_dsound_guids;
static INIT_ONCE g_real_init_once = INIT_ONCE_STATIC_INIT;

static BOOL bytes_equal(const BYTE *left, const BYTE *right, SIZE_T length) {
    SIZE_T index;
    for (index = 0; index < length; ++index) {
        if (left[index] != right[index]) return FALSE;
    }
    return TRUE;
}

static BOOL write_code(BYTE *destination, const BYTE *source, SIZE_T length) {
    DWORD original_protection;
    DWORD ignored;
    SIZE_T index;
    if (!VirtualProtect(destination, length, PAGE_EXECUTE_READWRITE, &original_protection)) return FALSE;
    for (index = 0; index < length; ++index) destination[index] = source[index];
    FlushInstructionCache(GetCurrentProcess(), destination, length);
    VirtualProtect(destination, length, original_protection, &ignored);
    return TRUE;
}

static BOOL get_patch_locations(BYTE **branch, BYTE **cave) {
    HMODULE disrupt = GetModuleHandleW(L"Disrupt_64.dll");
    PIMAGE_DOS_HEADER dos;
    PIMAGE_NT_HEADERS64 nt;
    if (!disrupt) return FALSE;
    dos = (PIMAGE_DOS_HEADER)disrupt;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return FALSE;
    nt = (PIMAGE_NT_HEADERS64)((BYTE *)disrupt + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
        nt->FileHeader.TimeDateStamp != WD2_DISRUPT_TIMESTAMP ||
        nt->OptionalHeader.SizeOfImage != WD2_DISRUPT_SIZE_OF_IMAGE) {
        return FALSE;
    }
    *branch = (BYTE *)disrupt + WD2_CLASSIFIER_BRANCH_RVA;
    *cave = (BYTE *)disrupt + WD2_CLASSIFIER_CAVE_RVA;
    return TRUE;
}

static BOOL apply_activation_patch(void) {
    BYTE *branch;
    BYTE *cave;
    if (!get_patch_locations(&branch, &cave)) return FALSE;
    if (!bytes_equal(branch, g_original_branch, sizeof(g_original_branch)) ||
        !bytes_equal(cave, g_original_cave, sizeof(g_original_cave))) return FALSE;
    if (!write_code(cave, g_patched_cave, sizeof(g_patched_cave))) return FALSE;
    if (!write_code(branch, g_patched_branch, sizeof(g_patched_branch))) {
        write_code(cave, g_original_cave, sizeof(g_original_cave));
        return FALSE;
    }
    g_patch_applied = TRUE;
    return TRUE;
}

static void remove_activation_patch(void) {
    BYTE *branch;
    BYTE *cave;
    if (!g_patch_applied || !get_patch_locations(&branch, &cave)) return;
    if (!bytes_equal(branch, g_patched_branch, sizeof(g_patched_branch)) ||
        !bytes_equal(cave, g_patched_cave, sizeof(g_patched_cave))) return;
    if (!write_code(branch, g_original_branch, sizeof(g_original_branch))) return;
    if (!write_code(cave, g_original_cave, sizeof(g_original_cave))) return;
    g_patch_applied = FALSE;
}

static BOOL CALLBACK initialize_real_xinput(PINIT_ONCE once, PVOID parameter, PVOID *context) {
    static const WCHAR suffix[] = L"\\xinput9_1_0.dll";
    WCHAR path[MAX_PATH];
    UINT length;
    SIZE_T index;
    (void)once;
    (void)parameter;
    (void)context;
    length = GetSystemDirectoryW(path, MAX_PATH);
    if (!length || length + (sizeof(suffix) / sizeof(suffix[0])) > MAX_PATH) return TRUE;
    for (index = 0; index < sizeof(suffix) / sizeof(suffix[0]); ++index) path[length + index] = suffix[index];
    g_real_module = LoadLibraryW(path);
    if (!g_real_module) return TRUE;
    g_real_get_state = (PFN_XInputGetState)GetProcAddress(g_real_module, "XInputGetState");
    g_real_set_state = (PFN_XInputSetState)GetProcAddress(g_real_module, "XInputSetState");
    g_real_get_capabilities = (PFN_XInputGetCapabilities)GetProcAddress(g_real_module, "XInputGetCapabilities");
    g_real_get_dsound_guids = (PFN_XInputGetDSoundAudioDeviceGuids)GetProcAddress(g_real_module, "XInputGetDSoundAudioDeviceGuids");
    return TRUE;
}

static void ensure_real_xinput(void) {
    InitOnceExecuteOnce(&g_real_init_once, initialize_real_xinput, NULL, NULL);
}

DWORD WINAPI XInputGetState(DWORD user_index, XINPUT_STATE *state) {
    ensure_real_xinput();
    return g_real_get_state ? g_real_get_state(user_index, state) : ERROR_PROC_NOT_FOUND;
}

DWORD WINAPI XInputSetState(DWORD user_index, XINPUT_VIBRATION *vibration) {
    ensure_real_xinput();
    return g_real_set_state ? g_real_set_state(user_index, vibration) : ERROR_PROC_NOT_FOUND;
}

DWORD WINAPI XInputGetCapabilities(DWORD user_index, DWORD flags, XINPUT_CAPABILITIES *capabilities) {
    ensure_real_xinput();
    return g_real_get_capabilities ? g_real_get_capabilities(user_index, flags, capabilities) : ERROR_PROC_NOT_FOUND;
}

DWORD WINAPI XInputGetDSoundAudioDeviceGuids(DWORD user_index, GUID *render_guid, GUID *capture_guid) {
    ensure_real_xinput();
    return g_real_get_dsound_guids ? g_real_get_dsound_guids(user_index, render_guid, capture_guid) : ERROR_PROC_NOT_FOUND;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(instance);
        if (!apply_activation_patch()) {
            SetLastError(ERROR_BAD_EXE_FORMAT);
            return FALSE;
        }
    } else if (reason == DLL_PROCESS_DETACH && reserved == NULL) {
        remove_activation_patch();
    }
    return TRUE;
}
