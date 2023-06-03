//
// Created by Mika Cybertron on 5/20/2023.
//
#include <unistd.h>
#include "Platinmods/PlatinmodsMemoryPatch.h"

int main() {

    pid_t pid = Platinmods::getPid("com.Acessor.Il2CppPOC");
    
    Platinmods::MemoryPatch patch1 = Platinmods::MemoryPatch(pid, "libil2cpp.so", "0x524E24", "7F 08 A0 E3 1E FF 2F E1");
    
    // Write Modified Hex
    bool result = patch1.Modify();
    if (result) {
        printf("Offset patches successfully!\n");
    } else {
        printf("Patch failed\n");
    }
    
    // Restore Original Hex
    patch1.Restore();
    
    return 0;
}