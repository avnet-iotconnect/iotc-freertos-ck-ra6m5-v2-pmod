#
# Script to generate EmbedFS blob, akin to Renesas' original EmbedFS.exe utility
# which is Windows only and thus had to be reverse engineered for this
#
#  Created on: Aug 11, 2024
#      Author: evoirin

"""
EmbedFS File Format Documentation

* uint32_t - magic number 0x87654321 (Little endian mode) or
                          0x12345678 (Big endian mode - not supported in this file)
* uint32_t - unknown (always 0x00000001)

* <Directory entries, repeat for as many directories there are in the directory>

    * Directory entry header
        * uint32_t - header size
        * uint32_t - always 0
        * uint32_t - total size of the directory, including header and all files and file headers
        * char[] - Directory name, relative to root path, includes backslash at the beginning.
                   null terminated, uses backslash \ as path separator,  aligned to 32-bits

    * <File entries, repeat for as many files as the directory has>

        * File entry header
            * uint32_t - total size of file entry including header
            * uint32_t - header size
            * uint32_t - file size
            * char[] - File name, null terminated, aligned to 32-bits
                       (The directory name and the backslash is not included here.)
        * u8[file size] - File data

* 12 bytes - Empty header (3x 32-bit nulls) -> End of file marker
* Name of the last directory, aligned to 4 byte boundaries.
  NOTE: Only seems to exist if there's more than the root directory, i.e. one or more subdirectories exist.
"""

import os
import platform
import struct

class EmbedFSDirHeader:
    def __init__(self, data: bytes, offset: int):
        self.headerLen, self.dummy, self.totalLen = struct.unpack_from('<III', data, offset)
        self.firstFileOffset = offset + self.headerLen
        self.name = embedFS_readNullTerminatedString(data, offset + 12).replace('\\', '/')

        print(f'DirHeader @ {hex(offset)}: headerLen {self.headerLen}, totalLen {self.totalLen}, name "{self.name}"')

class EmbedFSFileHeader:
    def __init__(self, data: bytes, offset: int):
        self.totalLen, self.headerLen, self.fileLen = struct.unpack_from('<III', data, offset)
        self.dataOffset = offset + self.headerLen
        self.nextFileOffset = offset + self.totalLen
        self.name = embedFS_readNullTerminatedString(data, offset + 12).replace('\\', '/')

        print(f'FileHeader @ {hex(offset)}: totalLen {self.totalLen}, headerLen {self.headerLen}, fileLen {self.fileLen}, name "{self.name}"')

class EmbedFSDirEntry:
    # Creates directory entry with all files.
    def __init__(self, dirname: str, basePath: str):
        self.relativePath = '\\' + os.path.relpath(dirname, basePath)
        self.fileList = []
        # Fix root directory
        if self.relativePath == '\\.':
            self.relativePath = '\\'

        print(f'EmbedFSDirEntry: {self.relativePath}')

        for file in os.listdir(dirname):
            filename = os.path.join(dirname, file)
            if os.path.isfile(filename):
                fileEntry = EmbedFSFileEntry(filename, dirname)
                self.fileList.append(fileEntry)

    def to_bytes(self):
        hdrFilename = embedFS_getBackslashedStringBytes(self.relativePath)
        paddedFilenameLen = embedFS_getPaddedSize(len(hdrFilename) + 1) # +1 for null terminator
        hdrLen = 12 + paddedFilenameLen
        totalLen = hdrLen

        ret = bytearray(hdrLen)

        struct.pack_into(f'<III{paddedFilenameLen}s', ret, 0, hdrLen, 0, totalLen, hdrFilename) # We will fill totalLen later once we know everything

        for file in self.fileList:
            ret.extend(file.to_bytes())

        # We have all files in the binary blob now, update total count
        struct.pack_into(f'<I', ret, 8, len(ret))

        return ret


class EmbedFSFileEntry:
    def __init__(self, filename: str, basePath: str):
        self.relativePath = os.path.relpath(filename, basePath)
        with open(filename, 'rb') as f:
            self.data = f.read()

        print(f'EmbedFSFileEntry: {self.relativePath}')

    def to_bytes(self):
        # Pad filename to 32 bits
        hdrFilename = embedFS_getBackslashedStringBytes(self.relativePath)
        paddedFilenameLen = embedFS_getPaddedSize(len(hdrFilename) + 1) # +1 for null terminator
        dataLen = len(self.data)

        # Craft data blob
        # (Header size (12) + filename length + data length) padded to 32 bits
        hdrLen = 12 + paddedFilenameLen
        totalLen = embedFS_getPaddedSize(hdrLen + dataLen)
        ret = bytearray(totalLen)
        struct.pack_into(f'<III{paddedFilenameLen}s{dataLen}s', ret, 0, totalLen, hdrLen, dataLen, hdrFilename, self.data)

        return ret

def embedFS_getPaddedSize(size):
    return size + (4 - (size % 4)) % 4

# Since windows uses backslashes and linux uses forward slashes, we must make sure everything written to files conforms to what EmbedFS uses (backslashes)
def embedFS_getBackslashedStringBytes(s: str):
    return s.replace('/', '\\').encode()

# Checks if the header at the offset given is a directory or not (not a guarantee that it's a file if not!!)
def embedFS_isEntryDirectory(data, offset):
    dummy, indicator = struct.unpack_from('<II', data, offset)

    print(f'isEntryDirectory @ {hex(offset)} indicator: {indicator}')

    if indicator == 0:
        return True
    else:
        return False

# Read a null terminated string from the given offset
def embedFS_readNullTerminatedString(data, offset):
    ret = str()
    while data[offset] != 0x00:
        ret = ret + chr(data[offset])
        offset += 1
    return ret

# Check if end of data is reached or end marker is found (i.e. 3x zero uint32_ts)
def embedFS_isEOF(data, offset):
    if (offset + 12 >= len(data)):
        return True

    i1, i2, i3 = struct.unpack_from('<III', data, offset)

    return (i1 == 0) and (i2 == 0) and (i3 == 0)


# Unpack a directory from embedFS data blob. Returns total length of the directory
def embedFS_unpackDirectory(data: bytes, offset: int, unpackPath: str):
    if not embedFS_isEntryDirectory(data, offset):
        raise Exception('Invalid directory entry')

    # Read directory header and create the unpack path
    dirHdr = EmbedFSDirHeader(data, offset)
    unpackPath += dirHdr.name
    print(f'unpackDirectory @ {hex(offset)}: unpackPath: {unpackPath}')
    os.makedirs(unpackPath, exist_ok=True)

    # First file starts after directory header
    offset = dirHdr.firstFileOffset

    # Process file headers until we don't have file headers anymore
    while (not embedFS_isEOF(data, offset)) and (not embedFS_isEntryDirectory(data, offset)):
        fileToProcess = EmbedFSFileHeader(data, offset)
        newFileName = os.path.join(unpackPath, fileToProcess.name)
        with open(newFileName, 'wb') as out:
            out.write(data[fileToProcess.dataOffset:fileToProcess.dataOffset+fileToProcess.fileLen])
        offset = fileToProcess.nextFileOffset

    return dirHdr.totalLen

def embedFS_unpackFile(file, path):
    print(f'unpackFile: {file} -> {path}')

    with open(file, 'rb') as f:
        data = f.read()

    magicnum, unkInt = struct.unpack_from('<II', data, 0)

    if magicnum != 0x87654321:
        raise Exception('Magic number error!')

    if unkInt != 0x00000001:
        raise Exception('Unknown header field error!')

    offset = 8

    while (not embedFS_isEOF(data, offset)) and embedFS_isEntryDirectory(data, offset):
        offset += embedFS_unpackDirectory(data, offset, path)

# Returns of all subdirectories in a path, recursively, akin to EmbedFS's method
def embedFS_getAllSubdirs(path: str, first: bool=True):
    ret = list()
    # EmbedFS does it like this, so not really the way you'd expect recursivity to work. We try to mimick this behavior
    #   /
    #       /dir1
    #       /dir2
    #       /dir3
    #           /dir1sub
    #           /dir2sub
    #           /dir3sub

    # If this is the initial call, i.e. root level, add the path itself to the list
    if first:
        ret.append(path)

    subdirsToProcess = list()
    for subdir in os.listdir(path):
        subdirname = os.path.join(path, subdir)
        if os.path.isdir(subdirname):
            subdirsToProcess.append(subdirname)

    ret += subdirsToProcess

    for subdir in subdirsToProcess:
        ret += embedFS_getAllSubdirs(subdir, first=False)

    return ret

def embedFS_packDirectory(path, file):
    print(f'packDirectory: {path} -> {file}')

    # Get all directories recursively

    dirList = embedFS_getAllSubdirs(path)
    print(dirList)

    with open(file, 'wb') as of:
        # Write header
        of.write(struct.pack('<II', 0x87654321, 0x00000001))

        # Write all directory entries to the output file
        for dir in dirList:
            dirEntry = EmbedFSDirEntry(dir, path) # Instantiating a DirEntry autmatically processes the files in them as well
            of.write(dirEntry.to_bytes())

        # Add end of file marker
        of.write(struct.pack('<III', 0, 0, 0))

        # Write last directory's name IF there's more than a single directory. No clue why EmbedFS does this but we'll replicate it...
        if len(dirList) > 1:
            finalDir = '\\' + os.path.relpath(dirList[len(dirList) - 1], path)
            finalDirEncoded = embedFS_getBackslashedStringBytes(finalDir)
            finalDirEncodedLen = embedFS_getPaddedSize(len(finalDirEncoded) + 1)
            of.write(struct.pack(f'<{finalDirEncodedLen}s', finalDirEncoded))
