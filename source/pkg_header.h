#ifndef PKG_HEADER_H
#define PKG_HEADER_H

#define SWAP16(x)                                         \
    ((uint16_t)((((uint16_t)(x)&UINT16_C(0x00FF)) << 8) | \
                (((uint16_t)(x)&UINT16_C(0xFF00)) >> 8)))

#define SWAP32(x)                                              \
    ((uint32_t)((((uint32_t)(x)&UINT32_C(0x000000FF)) << 24) | \
                (((uint32_t)(x)&UINT32_C(0x0000FF00)) << 8) |  \
                (((uint32_t)(x)&UINT32_C(0x00FF0000)) >> 8) |  \
                (((uint32_t)(x)&UINT32_C(0xFF000000)) >> 24)))

#define SWAP64(x)                                                                \
    ((uint64_t)((uint64_t)(((uint64_t)(x)&UINT64_C(0x00000000000000FF)) << 56) | \
                (uint64_t)(((uint64_t)(x)&UINT64_C(0x000000000000FF00)) << 40) | \
                (uint64_t)(((uint64_t)(x)&UINT64_C(0x0000000000FF0000)) << 24) | \
                (uint64_t)(((uint64_t)(x)&UINT64_C(0x00000000FF000000)) << 8) |  \
                (uint64_t)(((uint64_t)(x)&UINT64_C(0x000000FF00000000)) >> 8) |  \
                (uint64_t)(((uint64_t)(x)&UINT64_C(0x0000FF0000000000)) >> 24) | \
                (uint64_t)(((uint64_t)(x)&UINT64_C(0x00FF000000000000)) >> 40) | \
                (uint64_t)(((uint64_t)(x)&UINT64_C(0xFF00000000000000)) >> 56)))

#define BE16(x) (x)
#define BE32(x) (x)
#define BE64(x) (x)

#define LE16(x) SWAP16(x)
#define LE32(x) SWAP32(x)
#define LE64(x) SWAP64(x)

#define PKG_MAGIC 0x7F504B47

// Constants
enum : u32
{
	PKG_HEADER_SIZE  = 0xC0, // sizeof(pkg_header) + sizeof(pkg_unk_checksum)
	PKG_HEADER_SIZE2 = 0x280,
	PKG_MAX_FILENAME_SIZE = 256,
};

enum : u16
{
	PKG_RELEASE_TYPE_RELEASE      = 0x8000,
	PKG_RELEASE_TYPE_DEBUG        = 0x0000,

	PKG_PLATFORM_TYPE_PS3         = 0x0001,
	PKG_PLATFORM_TYPE_PSP_PSVITA  = 0x0002,
};

enum : u32
{
	PKG_FILE_ENTRY_NPDRM          = 1,
	PKG_FILE_ENTRY_NPDRMEDAT      = 2,
	PKG_FILE_ENTRY_REGULAR        = 3,
	PKG_FILE_ENTRY_FOLDER         = 4,
	PKG_FILE_ENTRY_UNK0           = 5,
	PKG_FILE_ENTRY_UNK1           = 6,
	PKG_FILE_ENTRY_SDAT           = 9,

	PKG_FILE_ENTRY_OVERWRITE      = 0x80000000,
	PKG_FILE_ENTRY_PSP            = 0x10000000,

	PKG_FILE_ENTRY_KNOWN_BITS     = 0xff | PKG_FILE_ENTRY_PSP | PKG_FILE_ENTRY_OVERWRITE,
};

enum : u32
{
	PKG_CONTENT_TYPE_UNKNOWN_1      = 0x01, // ?
	PKG_CONTENT_TYPE_UNKNOWN_2      = 0x02, // ?
	PKG_CONTENT_TYPE_UNKNOWN_3      = 0x03, // ?
	PKG_CONTENT_TYPE_GAME_DATA      = 0x04, // GameData (also patches)
	PKG_CONTENT_TYPE_GAME_EXEC      = 0x05, // GameExec
	PKG_CONTENT_TYPE_PS1_EMU        = 0x06, // PS1emu
	PKG_CONTENT_TYPE_PC_ENGINE      = 0x07, // PSP & PCEngine
	PKG_CONTENT_TYPE_UNKNOWN_4      = 0x08, // ?
	PKG_CONTENT_TYPE_THEME          = 0x09, // Theme
	PKG_CONTENT_TYPE_WIDGET         = 0x0A, // Widget
	PKG_CONTENT_TYPE_LICENSE        = 0x0B, // License
	PKG_CONTENT_TYPE_VSH_MODULE     = 0x0C, // VSHModule
	PKG_CONTENT_TYPE_PSN_AVATAR     = 0x0D, // PSN Avatar
	PKG_CONTENT_TYPE_PSP_GO         = 0x0E, // PSPgo
	PKG_CONTENT_TYPE_MINIS          = 0x0F, // Minis
	PKG_CONTENT_TYPE_NEOGEO         = 0x10, // NEOGEO
	PKG_CONTENT_TYPE_VMC            = 0x11, // VMC
	PKG_CONTENT_TYPE_PS2_CLASSIC    = 0x12, // ?PS2Classic? Seen on PS2 classic
	PKG_CONTENT_TYPE_UNKNOWN_5      = 0x13, // ?
	PKG_CONTENT_TYPE_PSP_REMASTERED = 0x14, // ?
	PKG_CONTENT_TYPE_PSP2_GD        = 0x15, // PSVita Game Data
	PKG_CONTENT_TYPE_PSP2_AC        = 0x16, // PSVita Additional Content
	PKG_CONTENT_TYPE_PSP2_LA        = 0x17, // PSVita LiveArea
	PKG_CONTENT_TYPE_PSM_1          = 0x18, // PSVita PSM ?
	PKG_CONTENT_TYPE_WT             = 0x19, // Web TV ?
	PKG_CONTENT_TYPE_UNKNOWN_6      = 0x1A, // ?
	PKG_CONTENT_TYPE_UNKNOWN_7      = 0x1B, // ?
	PKG_CONTENT_TYPE_UNKNOWN_8      = 0x1C, // ?
	PKG_CONTENT_TYPE_PSM_2          = 0x1D, // PSVita PSM ?
	PKG_CONTENT_TYPE_UNKNOWN_9      = 0x1E, // ?
	PKG_CONTENT_TYPE_PSP2_THEME     = 0x1F, // PSVita Theme
};

// Structs
struct PKGHeader
{
	uint32_t pkg_magic;    // Magic (0x7f504b47) (" PKG")
	uint16_t pkg_type;     // Release type (Retail:0x8000, Debug:0x0000)
	uint16_t pkg_platform; // Platform type (PS3:0x0001, PSP:0x0002)
	uint32_t meta_offset;  // Metadata offset. Usually 0xC0 for PS3, usually 0x280 for PSP and PSVita
	uint32_t meta_count;   // Metadata item count
	uint32_t meta_size;    // Metadata size.
	uint32_t file_count;   // Number of files
	uint64_t pkg_size;     // PKG size in bytes
	uint64_t data_offset;  // Encrypted data offset
	uint64_t data_size;    // Encrypted data size in bytes
	char content_id[48];      // Title ID
	uint64_t qa_digest[2]; // This should be the hash of "files + attribs"
	uint8_t klicensee[128];   // Nonce
	// + some stuff
};

// Extended header in PSP and PSVita packages
struct PKGExtHeader
{
	uint32_t magic;                            // 0x7F657874 (" ext")
	uint32_t unknown_1;                        // Maybe version. always 1
	uint32_t ext_hdr_size;                     // Extended header size. ex: 0x40
	uint32_t ext_data_size;                    // ex: 0x180
	uint32_t main_and_ext_headers_hmac_offset; // ex: 0x100
	uint32_t metadata_header_hmac_offset;      // ex: 0x360, 0x390, 0x490
	uint64_t tail_offset;                      // tail size seams to be always 0x1A0
	uint32_t padding1;
	uint32_t pkg_key_id;                       // Id of the AES key used for decryption. PSP = 0x1, PSVita = 0xC0000002, PSM = 0xC0000004
	uint32_t full_header_hmac_offset;          // ex: none (old pkg): 0, 0x930
	u8 padding2[20];
};

struct PKGEntry
{
	uint32_t name_offset;  // File name offset
	uint32_t name_size;    // File name size
	uint64_t file_offset;  // File offset
	uint64_t file_size;    // File size
	uint32_t type;         // File type
	uint32_t pad;          // Padding (zeros)
};

#endif //PKG_HEADER_H