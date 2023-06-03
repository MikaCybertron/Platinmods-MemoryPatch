//
// Created by Mika Cybertron on 5/19/2023.
//


#include "PlatinmodsMemoryPatch.h"

#define LOGGER_ENABLED
#include "../includes/Logger.h"


namespace Platinmods {

    MemoryPatch::MemoryPatch() {
        this->packageName = "";
        this->libraryName = "";
        this->pid = 0;
        this->size = 0;
        this->absoluteAddress = 0;
    }

    MemoryPatch::MemoryPatch(std::string packageName) {
        this->pid = ::Platinmods::getPid(packageName);
        if(!IsValidPID())
        {
            return;
        }
    }

    MemoryPatch::MemoryPatch(pid_t pid) {
        this->pid = pid;
        if(!IsValidPID())
        {
            return;
        }
    }

    MemoryPatch::MemoryPatch(std::string packageName, std::string libraryName, std::string address, std::string hexCode) {


        this->packageName = packageName;
        this->libraryName = libraryName;
        this->pid = ::Platinmods::getPid(packageName);

        if(!IsValidPID())
        {
            return;
        }

        bool isValid = PlatinmodsUtils::validateHexString(hexCode);
        if(!isValid)
        {
            LOGE("Error: Invalid hex codes: %s", hexCode.c_str());
            return;
        }

        this->size = hexCode.length() / 2;

        this->baseAddress = getLibraryBase(pid, libraryName);

        if(baseAddress < 1)
        {
            LOGE("Error: Invalid base address.");
            return;
        }
        this->absoluteAddress = baseAddress + std::stoll(address, nullptr, 16);

        std::string origHex = "";
        bool result = MemoryPatch::ReadHex(absoluteAddress, size, origHex);
        if(!result)
        {
            origHex = hexCode;
        }

        // Create Buffer of Hex Codes
        PlatinmodsUtils::fromHex(hexCode, patchedHex);

        PlatinmodsUtils::fromHex(origHex, originalHex);
    }

    MemoryPatch::MemoryPatch(pid_t pid, std::string libraryName, std::string address, std::string hexCode) {

        this->libraryName = libraryName;
        this->pid = pid;

        if(!IsValidPID())
        {
            return;
        }

        bool isValid = PlatinmodsUtils::validateHexString(hexCode);
        if(!isValid)
        {
            LOGE("Error: Invalid hex codes: %s", hexCode.c_str());
            return;
        }

        this->size = hexCode.length() / 2;

        this->baseAddress = getLibraryBase(pid, libraryName);

        if(baseAddress < 1)
        {
            LOGE("Error: Invalid base address.");
            return;
        }
        this->absoluteAddress = baseAddress + std::stoll(address, nullptr, 16);

        std::string origHex = "";
        bool result = MemoryPatch::ReadHex(absoluteAddress, size, origHex);
        if(!result)
        {
            origHex = hexCode;
        }

        // Create Buffer of Hex Codes
        PlatinmodsUtils::fromHex(hexCode, patchedHex);

        PlatinmodsUtils::fromHex(origHex, originalHex);
    }


    bool MemoryPatch::CustomPatchHex(std::string libraryName, std::string address, std::string hexCode) {

        DWORD baseAddress = getLibraryBase(pid, libraryName);

        if(baseAddress < 1)
        {
            LOGE("Error: Invalid base address.");
            return false;
        }

        DWORD absoluteAddress = baseAddress + std::stoll(address, nullptr, 16);

        char hexData[128];

        bool isValid = PlatinmodsUtils::validateHexString(hexCode);
        if(!isValid)
        {
            LOGE("Error: Invalid hex codes: %s", hexCode.c_str());
            return false;
        }

        size_t size = hexCode.length() / 2;

        // Create Buffer of Hex Codes
        PlatinmodsUtils::fromHex(hexCode, hexData);

        return memWrite(pid, absoluteAddress, hexData, size);
    }

    bool MemoryPatch::ReadHex(DWORD address, size_t size, std::string& outputHexString) {
        char buffer[size];

        bool result = memRead(pid, address, buffer, size);

        if(result)
        {
            char hexBuffer[3]; // Buffer to hold two hex characters and a null terminator

            for (int i = 0; i < size; i++) {
                snprintf(hexBuffer, sizeof(hexBuffer), "%02X", (unsigned char) buffer[i]);
                outputHexString += hexBuffer;
                outputHexString += " ";
            }

            // Validate Hex String
            PlatinmodsUtils::validateHexString(outputHexString);
        }
        else
        {
            LOGE("Error: Failed to read hex in memory.");
        }
        return result;
    }

    bool MemoryPatch::Modify() {

        if(!IsValid())
        {
            return false;
        }
        return memWrite(pid, absoluteAddress, patchedHex, size);
    }

    bool MemoryPatch::Restore() {

        if(!IsValid())
        {
            return false;
        }
        return memWrite(pid, absoluteAddress, originalHex, size);
    }

    bool MemoryPatch::memRead(pid_t pid, DWORD address, char *buff, size_t size) {
        char path[64];
        sprintf(path, "/proc/%d/mem", pid);
        FILE *fp = fopen(path, "rb");  // Open the file in binary mode
        if (fp == NULL) {
            printf("memRead() failed to read process %d memory", pid);
            return false;
        }
        fseeko64(fp, address, SEEK_SET);  // Use SEEK_SET as the reference position
        bool ok = fread(buff, 1, size, fp) == size;  // Read the specified number of bytes
        fclose(fp);
        return ok;
    }

    bool MemoryPatch::memWrite(pid_t pid, DWORD address, char *buff, size_t size) {
        char path[64];
        sprintf(path, "/proc/%d/mem", pid);
        FILE *fp = fopen(path, "w");
        if (fp == NULL) {
            printf("memWrite() failed to write process %d memory", pid);
            return false;
        }
        fseeko64(fp, address, 0);
        bool ok = fwrite(buff, 1, size, fp) == size;
        fclose(fp);
        return ok;
    }

    bool MemoryPatch::IsValid() {

        if(pid == 0 || baseAddress == 0 || absoluteAddress == 0)
        {
            LOGE("Error: Invalid patch");
            return false;
        }
        return true;
    }

    bool MemoryPatch::IsValidPID() {

        if(pid == 0)
        {
            LOGE("Error: Failed to get PID Process.");
            return false;
        }
        return true;
    }

    DWORD MemoryPatch::getBaseAddress() const {

        return baseAddress;
    }

    DWORD MemoryPatch::getRealOffsets() const {

        return absoluteAddress - baseAddress;
    }

    DWORD MemoryPatch::getAbsoluteAddress() const {

        return absoluteAddress;
    }

    std::string MemoryPatch::get_OrigBytes() const {

        return PlatinmodsUtils::Hex2String(originalHex, size);
    }

    std::string MemoryPatch::get_PatchBytes() const {
        return PlatinmodsUtils::Hex2String(patchedHex, size);
    }

    pid_t MemoryPatch::getPid() const {
        return pid;
    }

    pid_t getPid(std::string packageName) {
        auto proc_dir = opendir("/proc");
        if (proc_dir == nullptr) {
            return 0;
        }
        FILE* fp;
        dirent* pid_file;
        char cmd_line[128];

        while ((pid_file = readdir(proc_dir))) {
            if (pid_file->d_type != DT_DIR ||
                strcmp(pid_file->d_name, ".") == 0 ||
                strcmp(pid_file->d_name, "..") == 0)
                continue;

            std::string file_path = "/proc/";
            file_path += pid_file->d_name;
            file_path += "/cmdline";
            fp = fopen(file_path.c_str(), "r");
            if (fp != nullptr) {
                std::fgets(cmd_line, sizeof(cmd_line), fp);
                std::fclose(fp);
                if (cmd_line == packageName) {
                    closedir(proc_dir);
                    return (int) std::strtol(pid_file->d_name, nullptr, 0);
                }
            }
        }
        closedir(proc_dir);
        return 0;
    }

    DWORD getLibraryBase(pid_t pid, std::string libraryName) {
        DWORD start_address = 0;
        char om[64], line[1024];
        strcpy(om, libraryName.c_str());
        std::string path = "/proc/" + std::to_string(pid) + "/maps";
        FILE *p = fopen(path.c_str(), "r");
        if (p) {
            while (fgets(line, sizeof(line), p)) {

                if (strstr(line, libraryName.c_str()))
                {
                    sscanf(line, "%lx-%*lx", &start_address); // NOLINT(cert-err34-c)
                    break;
                }
            }
            fclose(p);
        }
        return start_address;
    }
}