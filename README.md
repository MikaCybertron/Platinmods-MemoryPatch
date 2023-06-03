# Platinmods-MemoryPatch
Platinmods MemoryPatch is a C++ library or template for External Hex Patching into Process Memory which only work for Android 7 and higher. The original source is from <a href="https://github.com/ac3ss0r/ZMemory">ZMemory</a>, but i changed most of the code to make it work like <a href="https://github.com/MJx0/KittyMemory">KittyMemory</a>.

### Video Preview
[![Platinmods MemoryPatch and PM Patcher (External Hex Patcher)](https://i.imgur.com/8sKe7dG.png)](https://youtu.be/sZFxejiARiU)

### Usage example
This example patches a method replace values and restore value, example apk from <a href="https://github.com/acessors/Il2Cpp-Exploitation-POC">Il2Cpp Exploitation POC</a> is used.
```c++
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
```

# Creditds
- <a href="https://github.com/ac3ss0r">acessor</a>
- <a href="https://github.com/MJx0">MJx0</a>
