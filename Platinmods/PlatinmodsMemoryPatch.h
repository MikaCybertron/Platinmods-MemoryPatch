//
// Created by Mika Cybertron on 5/19/2023.
//
#include <cstdio>
#include <cstdlib>
#include <memory.h>
#include <ctime>
#include <android/log.h>
#include <jni.h>
#include <unistd.h>
#include <string>
#include <dirent.h>
//#include <sys/mman.h> // PROT_READ | PROT_WRITE | PROT_EXEC

#include "PlatinmodsUtils.h"

namespace Platinmods {

    typedef long long DWORD;

    class MemoryPatch {
    private:
        std::string packageName = "";
        std::string libraryName = "";
        pid_t pid = 0;
        size_t size = 0;
        DWORD baseAddress = 0;
        DWORD absoluteAddress = 0;
        char patchedHex[128];
        char originalHex[128];

        /*!
         * Reads an address content into a buffer
         */
        bool memRead(pid_t pid, DWORD address, char *buff, size_t size);

        /*!
         * Writes buffer content to an address
         */
        bool memWrite(pid_t pid, DWORD address, char *buff, size_t size);

        /*!
         * Validate patch
         */
        bool IsValid();

        /*!
         * Validate PID Process
         */
        bool IsValidPID();

    public:

        MemoryPatch& operator=(const MemoryPatch& other) {
            if (this == &other) {
                return *this;
            }

            packageName = other.packageName;
            libraryName = other.libraryName;
            pid = other.pid;
            size = other.size;
            baseAddress = other.baseAddress;
            absoluteAddress = other.absoluteAddress;
            strncpy(patchedHex, other.patchedHex, sizeof(other.patchedHex));
            strncpy(originalHex, other.originalHex, sizeof(other.originalHex));

            return *this;
        }

        /*!
         * Return nothing
         */
        MemoryPatch();

        /*!
         * expects package name
         */
        MemoryPatch(std::string packageName);

        /*!
         * expects pid
         */
        MemoryPatch(pid_t pid);

        /*!
         * expects package name, library name and relative address
         * compatible hex format (0xffff & ffff & ff ff)
         */
        MemoryPatch(std::string packageName, std::string libraryName, std::string address, std::string hexCode);

        /*!
         * Recommended to use this one.
         *
         * expects pid, library name and relative address
         * compatible hex format (0xffff & ffff & ff ff)
         */
        MemoryPatch(pid_t pid, std::string libraryName, std::string address, std::string hexCode);

        /*!
         * Applies custom modification patches to target addresses without calling functions from `Modify()` or `Restore()`
         * compatible hex format (0xffff & ffff & ff ff)
         */
        bool CustomPatchHex(std::string libraryName, std::string address, std::string hexCode);

        /*!
         * Read Hex Codes
         */
        bool ReadHex(DWORD address, size_t size, std::string& outputHexString);

        /*!
         * Applies patch modifications to target address
         */
        bool Modify();

        /*!
         * Restores patch to original value
         */
        bool Restore();

        /*!
         * Returns Base Address of library
         */
        DWORD getBaseAddress() const;

        /*!
         * Returns Real Offsets or Relative Address  (Absolute Address - Base Address)
         */
        DWORD getRealOffsets() const;

        /*!
         * Returns Absolute Address (Base Address + Relative Address)
         */
        DWORD getAbsoluteAddress() const;

        /*!
         * Returns original bytes as hex string
         */
        std::string get_OrigBytes() const;

        /*!
         * Returns patch bytes as hex string
         */
        std::string get_PatchBytes() const;

        /*!
         * Returns current pid process
         */
        pid_t getPid() const;
    };


    /*!
     * Get pid process by package name
     */
    pid_t getPid(std::string packageName);

    /*!
     * Get base address of library
     */
    DWORD getLibraryBase(pid_t pid, std::string libraryName);
}
