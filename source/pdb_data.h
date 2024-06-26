#pragma once

#define d0top_data_size		177
#define pkg_d0end_data_size	110
#define iconfile_data_size	2088
#define install_data_pdb_size 123

#define PDB_HDR_FILENAME	"\x00\x00\x00\xCB"
#define PDB_HDR_DATETIME	"\x00\x00\x00\xCC"
#define PDB_HDR_URL			"\x00\x00\x00\xCA"
#define PDB_HDR_ICON		"\x00\x00\x00\x6A"
#define PDB_HDR_TITLE		"\x00\x00\x00\x69"
#define PDB_HDR_SIZE		"\x00\x00\x00\xCE"
#define PDB_HDR_CONTENT		"\x00\x00\x00\xD9"
#define PDB_HDR_UNUSED		"\x00\x00\x00\x00"
#define PDB_HDR_DLSIZE		"\x00\x00\x00\xD0"

typedef enum {
    PresenceUnknown,
    PresenceIncomplete,
    PresenceInstalled,
    PresenceMissing,
} DbPresence;

typedef enum {
    SortByTitle,
    SortByRegion,
    SortByName,
    SortBySize,
} DbSort;

typedef enum {
    SortAscending,
    SortDescending,
} DbSortOrder;

typedef enum {
    DbFilterRegionASA = 0x01,
    DbFilterRegionEUR = 0x02,
    DbFilterRegionJPN = 0x04,
    DbFilterRegionUSA = 0x08,

    // TODO: implement these two
    DbFilterInstalled = 0x10,
    DbFilterMissing   = 0x20,

    DbFilterContentGame     = 0x000100,
    DbFilterContentDLC      = 0x000200,
    DbFilterContentTheme    = 0x000400,
    DbFilterContentAvatar   = 0x000800,
    DbFilterContentDemo     = 0x001000,
    DbFilterContentUpdate   = 0x002000,
    DbFilterContentEmulator = 0x004000,
    DbFilterContentApp      = 0x008000,
    DbFilterContentTool     = 0x010000,

    DbFilterAllRegions = DbFilterRegionUSA | DbFilterRegionEUR | DbFilterRegionJPN | DbFilterRegionASA,
    DbFilterAllContent = DbFilterContentGame | DbFilterContentDLC | DbFilterContentTheme | DbFilterContentAvatar | 
                         DbFilterContentDemo | DbFilterContentUpdate | DbFilterContentEmulator | DbFilterContentApp | DbFilterContentTool,
    DbFilterAll = DbFilterAllRegions | DbFilterAllContent | DbFilterInstalled | DbFilterMissing,
} DbFilter;

typedef enum {
    ContentUnknown,
    ContentGame,
    ContentDLC,
    ContentTheme,
    ContentAvatar,
    ContentDemo,
    ContentUpdate,
    ContentEmulator,
    ContentApp,
    ContentTool
} ContentType;

typedef struct {
    DbPresence presence;
    const char* content;
    ContentType type;
    const char* name;
    const char* description;
    const uint8_t* rap;
    const char* url;
    const uint8_t* digest;
    int64_t size;
} DbItem;

// ---------------------------------------------------------------------
// https://www.psdevwiki.com/ps3/Project_Database_(PDB)
// ---------------------------------------------------------------------
const unsigned char install_data_pdb[install_data_pdb_size] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x6B, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 
	0x03, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x6C, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xDA, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0xCD, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01
};

const unsigned char mp4_d0top_data[d0top_data_size] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65,
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x6B, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x04, 0x80, 0x02, 0x3E, 0x1A, 0x00, 0x00, 0x00,
	0x6C, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x04, 0x9D, 0xC0, 0xD3, 0x64, 0x00, 0x00, 0x00, 0xF9, 0x00, 0x00, 0x00,
	0x20, 0x00, 0x00, 0x00, 0x20, 0x9B, 0x6D, 0x4C, 0x40, 0x8C, 0x37, 0x68,
	0x53, 0x69, 0x5F, 0xD2, 0x43, 0x89, 0x21, 0x43, 0x76, 0x10, 0x93, 0xF8,
	0xBC, 0x71, 0xDA, 0x5D, 0xDB, 0x94, 0xE3, 0x84, 0x6D, 0xE1, 0x1F, 0x66,
	0x1E, 0x00, 0x00, 0x00, 0xD0, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char jpg_d0top_data[d0top_data_size] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65,
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x6B, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x04, 0x80, 0x02, 0x3E, 0x1A, 0x00, 0x00, 0x00,
	0x6C, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x04, 0x9D, 0xC0, 0xD3, 0x64, 0x00, 0x00, 0x00, 0xF9, 0x00, 0x00, 0x00,
	0x20, 0x00, 0x00, 0x00, 0x20, 0x9B, 0x6D, 0x4C, 0x40, 0x8C, 0x37, 0x68,
	0x53, 0x69, 0x5F, 0xD2, 0x43, 0x89, 0x21, 0x43, 0x76, 0x10, 0x93, 0xF8,
	0xBC, 0x71, 0xDA, 0x5D, 0xDB, 0x94, 0xE3, 0x84, 0x6D, 0xE1, 0x1F, 0x66,
	0x1E, 0x00, 0x00, 0x00, 0xD0, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char pkg_d0top_data[d0top_data_size] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65,
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, //01 active download ? !!! 00 paused
	0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x6B, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x04, 0x80, 0x02, 0x3E, 0x1A, 0x00, 0x00, 0x00,
	0x6C, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x04, 0x9D, 0xC0, 0xD3, 0x64, 0x00, 0x00, 0x00, 0xF9, 0x00, 0x00, 0x00,
	0x20, 0x00, 0x00, 0x00, 0x20, 0x9B, 0x6D, 0x4C, 0x40, 0x8C, 0x37, 0x68,
	0x53, 0x69, 0x5F, 0xD2, 0x43, 0x89, 0x21, 0x43, 0x76, 0x10, 0x93, 0xF8,
	0xBC, 0x71, 0xDA, 0x5D, 0xDB, 0x94, 0xE3, 0x84, 0x6D, 0xE1, 0x1F, 0x66,
	0x1E, 0x00, 0x00, 0x00, 0xD0, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char pkg_d0end_data[pkg_d0end_data_size] = {
	0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0xF1, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0xF2, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xF4, 0x00, 0x00,
	0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x19, 0x62, 0x00, 0x00,
	0x00, 0xDA, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0xCD, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x00, 0xEC, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x01, 0x00
};

const unsigned char other_d0end_data[52] = {
	0x00, 0x00, 0x00, 0xDA, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0xCD, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0xEC, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x00
};

// ICON_FILE (PNG data)

const unsigned char iconfile_data[iconfile_data_size] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0xb0, 0x08, 0x04, 0x00, 0x00, 0x00, 0xd4, 0x7c, 0x7f,
	0x8c, 0x00, 0x00, 0x07, 0xef, 0x49, 0x44, 0x41, 0x54, 0x78, 0xda, 0xed, 0xdc, 0x4d, 0x68, 0x14,
	0x77, 0x1c, 0xc6, 0xf1, 0xdf, 0x4c, 0x76, 0x93, 0x2c, 0xae, 0x04, 0x0a, 0x12, 0x5b, 0xbc, 0x6d,
	0xd1, 0xa2, 0x48, 0x09, 0x14, 0x4a, 0x31, 0x22, 0x8a, 0xa6, 0x5a, 0x8c, 0x08, 0x46, 0xad, 0x1e,
	0x6c, 0x4a, 0xd5, 0x8b, 0x87, 0xd2, 0xb8, 0x49, 0xb4, 0xb5, 0xba, 0xd9, 0xc4, 0xb7, 0x44, 0x51,
	0x94, 0x94, 0x52, 0x8a, 0x44, 0x45, 0x31, 0xc9, 0x8a, 0xc1, 0xa4, 0x22, 0xb1, 0x28, 0xb4, 0x7a,
	0x10, 0x5a, 0x72, 0x10, 0x6d, 0x8d, 0x84, 0xd2, 0xe2, 0x41, 0xc9, 0x29, 0xd6, 0xb0, 0xd1, 0x4d,
	0x66, 0x7a, 0xd0, 0xa9, 0x31, 0xee, 0x26, 0x33, 0xfb, 0xf6, 0xcc, 0x3f, 0xf3, 0x7c, 0x6e, 0x92,
	0xc5, 0xfc, 0x77, 0xf8, 0xf2, 0xec, 0xec, 0x1a, 0xa3, 0x99, 0x42, 0x84, 0xa3, 0xa3, 0x0f, 0x40,
	0xde, 0xc6, 0x00, 0x09, 0x8a, 0x01, 0x12, 0x14, 0x03, 0x24, 0x28, 0x06, 0x48, 0x50, 0x0c, 0x90,
	0xa0, 0x18, 0x20, 0x41, 0x31, 0x40, 0x82, 0x62, 0x80, 0x04, 0xc5, 0x00, 0x09, 0x8a, 0x01, 0x12,
	0x14, 0x03, 0x24, 0x28, 0x06, 0x48, 0x50, 0x0c, 0x90, 0xa0, 0x7c, 0xe8, 0x03, 0x64, 0x6e, 0x74,
	0xf6, 0x40, 0xc5, 0xad, 0x65, 0xd7, 0xca, 0x1e, 0x86, 0xe2, 0x33, 0xd0, 0x67, 0xc9, 0x8f, 0xc0,
	0xf0, 0x9c, 0x81, 0x15, 0x7d, 0x8b, 0xae, 0x87, 0x7a, 0x7d, 0x8f, 0xd0, 0x67, 0xc9, 0x94, 0xa6,
	0xf6, 0xcf, 0x03, 0xc6, 0x17, 0xb4, 0xd7, 0xb6, 0xad, 0x7d, 0x5a, 0x82, 0x3e, 0x07, 0x46, 0x70,
	0xa8, 0xba, 0x6b, 0x63, 0x4b, 0xe0, 0x2e, 0xfa, 0x1c, 0x99, 0x50, 0x38, 0x40, 0xa3, 0xa4, 0x23,
	0x72, 0x7c, 0xc7, 0xa8, 0x1f, 0x7d, 0x0e, 0x2c, 0x5f, 0xa2, 0xe6, 0x44, 0x55, 0xa3, 0x3e, 0x84,
	0x3e, 0x47, 0xba, 0x94, 0x0d, 0x70, 0x64, 0xee, 0xf6, 0xd8, 0xbd, 0x85, 0xe8, 0x53, 0xb8, 0xc3,
	0xfc, 0x3b, 0x3f, 0x54, 0x15, 0xf7, 0xa3, 0x4f, 0x91, 0x1e, 0x45, 0x03, 0x7c, 0xfa, 0xe1, 0xc6,
	0xee, 0xc7, 0xb3, 0xd0, 0xa7, 0x70, 0x8f, 0xd2, 0xc1, 0xf6, 0xca, 0xe0, 0x6d, 0xf4, 0x29, 0xd2,
	0xa1, 0x64, 0x80, 0x23, 0x73, 0xd7, 0xdd, 0x64, 0x7e, 0xaf, 0x2b, 0x1d, 0xbc, 0x58, 0xae, 0xe2,
	0x0a, 0x2a, 0xf8, 0x31, 0x8c, 0x51, 0xb2, 0x3d, 0xc6, 0xfc, 0x26, 0x7a, 0x3c, 0x6b, 0x7b, 0xcc,
	0x50, 0xf0, 0xcd, 0x98, 0x82, 0x01, 0x76, 0xee, 0xe3, 0xbd, 0x5f, 0x32, 0xf7, 0x16, 0xc6, 0xbe,
	0x45, 0x9f, 0xc1, 0x39, 0xe5, 0x5e, 0x82, 0xe3, 0x0b, 0x96, 0xf6, 0x79, 0xfd, 0x9d, 0x6f, 0x2a,
	0xbe, 0xc4, 0x8d, 0x32, 0xd5, 0x3e, 0x94, 0x51, 0x6e, 0x01, 0x2f, 0xd4, 0x33, 0xbf, 0x54, 0x46,
	0xfd, 0x1d, 0x61, 0xf4, 0x19, 0x9c, 0x52, 0x6c, 0x01, 0x47, 0x67, 0x2f, 0xff, 0xd3, 0xab, 0x1f,
	0x3b, 0xdb, 0x11, 0x1c, 0xfa, 0xf9, 0x3d, 0xb5, 0xfe, 0x75, 0x44, 0xb1, 0x05, 0x1c, 0xa8, 0x60,
	0x7e, 0x93, 0x79, 0x5a, 0x32, 0x50, 0x81, 0x3e, 0x83, 0x33, 0x8a, 0x05, 0x78, 0x6b, 0x19, 0xfa,
	0x04, 0x6e, 0xa7, 0xda, 0x15, 0x52, 0x2c, 0xc0, 0x6b, 0x65, 0xe8, 0x13, 0xb8, 0x9d, 0x6a, 0x57,
	0x48, 0xb1, 0x00, 0x1f, 0x86, 0xd0, 0x27, 0x70, 0x3b, 0xd5, 0xae, 0x90, 0x62, 0x01, 0x7a, 0xe5,
	0x07, 0xae, 0xd2, 0xa7, 0xda, 0x15, 0x52, 0x2c, 0x40, 0x9a, 0x6e, 0x18, 0x20, 0x41, 0x31, 0x40,
	0x82, 0x62, 0x80, 0x04, 0xc5, 0x00, 0x09, 0x8a, 0x01, 0x12, 0x14, 0x03, 0x24, 0x28, 0x06, 0x48,
	0x50, 0x0c, 0x90, 0xa0, 0x18, 0x20, 0x41, 0x31, 0x40, 0x82, 0x62, 0x80, 0x04, 0xc5, 0x00, 0x09,
	0x8a, 0x01, 0x12, 0x14, 0x03, 0x24, 0x28, 0x06, 0x48, 0x50, 0xd3, 0xe0, 0xf7, 0x03, 0xe6, 0x53,
	0x60, 0xf8, 0xc7, 0x1d, 0x9a, 0x91, 0xfa, 0xeb, 0xa6, 0xbe, 0xb5, 0x55, 0xb5, 0x1f, 0x09, 0xc5,
	0x62, 0x80, 0x8e, 0xf8, 0x9f, 0xcf, 0x3b, 0x2b, 0x93, 0x04, 0x28, 0xba, 0xff, 0x18, 0x03, 0x74,
	0x82, 0x2f, 0xc1, 0x04, 0xc5, 0x00, 0x09, 0x8a, 0x01, 0x12, 0x14, 0x03, 0x24, 0x28, 0x06, 0x48,
	0x50, 0x0c, 0x90, 0xa0, 0x18, 0x20, 0x41, 0x31, 0x40, 0x82, 0x62, 0x80, 0x04, 0xc5, 0x00, 0x09,
	0x8a, 0x01, 0x12, 0x14, 0x03, 0x24, 0x28, 0x06, 0x48, 0x50, 0x0c, 0x90, 0xa0, 0x18, 0x20, 0x41,
	0x31, 0x40, 0x82, 0x62, 0x80, 0x04, 0xc5, 0x00, 0x09, 0x8a, 0x01, 0x12, 0x14, 0x03, 0x24, 0x28,
	0x06, 0x48, 0x50, 0x0c, 0x90, 0xa0, 0x18, 0x20, 0x41, 0x31, 0x40, 0x82, 0x62, 0x80, 0x04, 0xc5,
	0x00, 0x09, 0x8a, 0x01, 0x12, 0x14, 0x03, 0x24, 0x28, 0x06, 0x48, 0x50, 0x0c, 0x90, 0xa0, 0x18,
	0x20, 0x41, 0x31, 0x40, 0x82, 0x62, 0x80, 0x04, 0xc5, 0x00, 0x5f, 0x9a, 0xf9, 0x64, 0xd1, 0x2f,
	0x9a, 0x99, 0x8f, 0xef, 0xa4, 0x8f, 0xed, 0x3c, 0x39, 0xef, 0x2f, 0xf4, 0xf3, 0x75, 0x0b, 0x06,
	0x28, 0x22, 0x22, 0x81, 0xe1, 0x73, 0x9b, 0x8e, 0xaf, 0x5c, 0x7b, 0x29, 0xf7, 0xdf, 0x49, 0x33,
	0x1b, 0x1a, 0x37, 0x7d, 0x79, 0x6a, 0xd5, 0xbb, 0x7f, 0xa3, 0x9f, 0xb3, 0x3b, 0x30, 0x40, 0x11,
	0x99, 0xf9, 0xa4, 0xa3, 0xea, 0x9d, 0x2b, 0x5a, 0xfc, 0xeb, 0xcd, 0x55, 0x9d, 0xb9, 0x5d, 0x41,
	0x7d, 0x6c, 0xff, 0xde, 0x55, 0x51, 0x31, 0x8b, 0xee, 0x9f, 0x59, 0xc1, 0x15, 0x14, 0x61, 0x80,
	0xf2, 0x62, 0xfd, 0xde, 0xbe, 0x2a, 0x22, 0xa2, 0x3d, 0xab, 0xff, 0x6c, 0x4d, 0x57, 0xee, 0xbe,
	0x93, 0x66, 0x46, 0x9a, 0x2a, 0xf6, 0x8b, 0x29, 0x22, 0x52, 0xf8, 0xe0, 0xd4, 0xaa, 0xd0, 0x3f,
	0xe8, 0xe7, 0x8e, 0xe7, 0xf9, 0x00, 0x5f, 0xac, 0x9f, 0xf5, 0x27, 0x2d, 0xbe, 0x67, 0xd3, 0xba,
	0x58, 0x6e, 0x56, 0x50, 0x1f, 0xdb, 0xbf, 0xf7, 0x93, 0x06, 0xf9, 0xff, 0xef, 0x2e, 0xba, 0x7f,
	0x76, 0x39, 0x57, 0xd0, 0xe3, 0x01, 0xbe, 0x5a, 0x3f, 0x8b, 0xf6, 0x6c, 0xd7, 0x96, 0x5c, 0xac,
	0xe0, 0xf8, 0xf5, 0xb3, 0x70, 0x05, 0x3d, 0x1e, 0x60, 0xf0, 0xdf, 0xce, 0x71, 0xeb, 0x67, 0xd1,
	0xe2, 0x7b, 0x36, 0x65, 0xfb, 0x5e, 0x50, 0x37, 0x5e, 0x5f, 0x3f, 0x0b, 0x57, 0xd0, 0xc3, 0x01,
	0x06, 0x86, 0xcf, 0x7f, 0x3a, 0xfb, 0x6a, 0xb2, 0xaf, 0x68, 0xcf, 0xea, 0xb7, 0xac, 0xe9, 0xca,
	0x5e, 0x82, 0x9a, 0x19, 0x69, 0x9c, 0xb8, 0x7e, 0x96, 0xc2, 0x07, 0xde, 0x7e, 0x47, 0xec, 0xd9,
	0x00, 0x93, 0xaf, 0x9f, 0x45, 0x1b, 0xc9, 0xde, 0xbd, 0x60, 0xaa, 0xf5, 0xb3, 0x78, 0xfb, 0x1d,
	0xb1, 0x47, 0x03, 0x4c, 0xbd, 0x7e, 0x96, 0x6c, 0xad, 0xa0, 0x6e, 0xa4, 0x5e, 0x3f, 0x8b, 0x97,
	0x57, 0xd0, 0x93, 0x01, 0x4e, 0xbe, 0x7e, 0x16, 0x6d, 0x64, 0xcf, 0xe6, 0x4c, 0x57, 0x50, 0x37,
	0x9a, 0xf6, 0x4d, 0xb6, 0x7e, 0x16, 0xef, 0xae, 0xa0, 0x07, 0x03, 0xf4, 0x3f, 0x3f, 0xb7, 0x79,
	0xf2, 0xf5, 0xb3, 0x68, 0x23, 0x99, 0xad, 0xa0, 0x9d, 0xf5, 0xb3, 0x14, 0x3e, 0xf8, 0xbe, 0xca,
	0xff, 0x1c, 0x7d, 0x6d, 0xf2, 0xcf, 0x83, 0x01, 0x26, 0x0a, 0x7f, 0x5f, 0x22, 0x05, 0xf6, 0x1e,
	0x9b, 0xc9, 0x0a, 0xda, 0x5d, 0xbf, 0x17, 0xcc, 0x99, 0xcd, 0x07, 0x12, 0x85, 0xe8, 0x6b, 0x93,
	0x7f, 0x1e, 0x0c, 0x50, 0xa4, 0xa9, 0xa6, 0xe7, 0x80, 0xdd, 0x67, 0x9e, 0xee, 0x0a, 0x3a, 0x59,
	0x3f, 0x11, 0x73, 0x46, 0x43, 0xfb, 0xd5, 0x0a, 0xf4, 0x75, 0x41, 0xf0, 0x64, 0x80, 0x86, 0x1e,
	0xa9, 0xbb, 0x7c, 0x38, 0x97, 0x2b, 0xe8, 0x74, 0xfd, 0x22, 0xb1, 0x9e, 0x55, 0xa6, 0x86, 0xbe,
	0x2e, 0x08, 0x9e, 0x0c, 0x50, 0x24, 0xb7, 0x2b, 0xe8, 0x7c, 0xfd, 0xae, 0x7c, 0x8c, 0xbe, 0x1e,
	0x28, 0x9e, 0x0d, 0x30, 0x77, 0x2b, 0xc8, 0xf5, 0x73, 0xc2, 0x87, 0x3e, 0x00, 0x52, 0x53, 0x8d,
	0x6e, 0xac, 0xde, 0x2d, 0x86, 0x9d, 0xc7, 0x6a, 0x23, 0xf5, 0x5b, 0x12, 0xbe, 0x1b, 0x4b, 0xa6,
	0x7c, 0x1c, 0xd7, 0xcf, 0x11, 0x4f, 0x07, 0x68, 0xe8, 0x91, 0x3a, 0x53, 0xab, 0xdc, 0x2d, 0x63,
	0x76, 0x1e, 0xad, 0x8d, 0xec, 0xd9, 0xfc, 0xd6, 0xf1, 0xa9, 0x1e, 0xd5, 0x14, 0xf9, 0xa8, 0xd5,
	0x7e, 0x7e, 0x91, 0xd8, 0x4f, 0x2b, 0xd1, 0x57, 0x01, 0x2b, 0x3f, 0x3f, 0x85, 0x9e, 0x35, 0x1f,
	0x64, 0xfd, 0xb8, 0xba, 0xb1, 0xf7, 0x88, 0xdd, 0x15, 0x14, 0x11, 0x9f, 0x8c, 0x66, 0xeb, 0x3b,
	0x9b, 0x45, 0xcd, 0x67, 0x62, 0xeb, 0xb3, 0xff, 0xe2, 0xfb, 0x9b, 0x52, 0x2f, 0xe7, 0x9e, 0xbd,
	0x07, 0xb4, 0x18, 0x7a, 0xa4, 0xae, 0xfb, 0x90, 0xdd, 0x7b, 0xc1, 0x2c, 0xe6, 0x57, 0xdc, 0x7c,
	0xa6, 0x73, 0x83, 0x77, 0xef, 0xfd, 0x2c, 0x9e, 0x0f, 0x50, 0x44, 0xa4, 0x71, 0x67, 0xb7, 0xed,
	0x77, 0xc4, 0xd9, 0x61, 0x16, 0xb5, 0xb4, 0xc5, 0xd6, 0xa3, 0x9f, 0xb7, 0x1b, 0x78, 0xfa, 0x1e,
	0xd0, 0x62, 0xe8, 0x0d, 0x75, 0x62, 0xfb, 0x5e, 0x30, 0x73, 0x66, 0x71, 0xf3, 0xe9, 0xce, 0x0d,
	0xe8, 0x67, 0xed, 0x0e, 0x5c, 0xc0, 0x97, 0xf2, 0xb7, 0x82, 0x5c, 0xbf, 0xf1, 0xb8, 0x80, 0x2f,
	0xe5, 0x6b, 0x05, 0xb9, 0x7e, 0xaf, 0xe3, 0x02, 0x8e, 0x93, 0xfb, 0x15, 0xe4, 0xfa, 0x4d, 0xc4,
	0x05, 0x1c, 0x27, 0xd7, 0x2b, 0xc8, 0xf5, 0x7b, 0x13, 0x03, 0x9c, 0xa0, 0x71, 0xa7, 0x48, 0xe5,
	0x2e, 0xdb, 0x9f, 0x0b, 0x3a, 0x60, 0x16, 0x35, 0x9f, 0xe6, 0xfa, 0x4d, 0xc4, 0x97, 0xe0, 0x09,
	0x0c, 0xbd, 0xa1, 0xd6, 0xc1, 0xe7, 0x82, 0xb6, 0xf1, 0x73, 0xbf, 0xe4, 0xb8, 0x80, 0x49, 0x64,
	0x7f, 0x05, 0xb9, 0x7e, 0xa9, 0x70, 0x01, 0x93, 0x30, 0xf4, 0x86, 0xda, 0xee, 0x83, 0xd9, 0x5b,
	0x41, 0xb3, 0xb8, 0xe5, 0x34, 0xd7, 0x2f, 0x39, 0x06, 0x98, 0x42, 0x63, 0xb8, 0xfb, 0x60, 0x96,
	0xae, 0x8e, 0xef, 0xfc, 0x49, 0xbe, 0xf5, 0x48, 0x85, 0x01, 0xa6, 0x90, 0xb5, 0x15, 0xf4, 0x77,
	0xb4, 0x1e, 0xdb, 0xca, 0xf5, 0x4b, 0x85, 0xf7, 0x80, 0x93, 0x68, 0x0c, 0x8b, 0x96, 0xe1, 0xbd,
	0xa0, 0xaf, 0xa3, 0xb5, 0x65, 0x2b, 0xfa, 0x79, 0xb8, 0x19, 0x17, 0x70, 0x12, 0x19, 0xaf, 0xa0,
	0xbf, 0xa3, 0xb5, 0x79, 0x1b, 0xd7, 0x6f, 0x32, 0x5c, 0xc0, 0x29, 0x64, 0xb0, 0x82, 0xbe, 0xf6,
	0xd6, 0x23, 0x5c, 0xbf, 0x29, 0x70, 0x01, 0xa7, 0x60, 0xe8, 0x0d, 0xb5, 0x3d, 0xe9, 0xac, 0xa0,
	0xbf, 0xb3, 0xb5, 0x85, 0xeb, 0x37, 0x25, 0x2e, 0xa0, 0x0d, 0xd1, 0xb0, 0xe9, 0x74, 0x05, 0x79,
	0xef, 0x67, 0x13, 0x17, 0xd0, 0x06, 0xc7, 0x2b, 0xe8, 0xef, 0xe4, 0xbd, 0x9f, 0x4d, 0x5c, 0x40,
	0x9b, 0xa2, 0x61, 0x43, 0x5f, 0x53, 0x6f, 0x6b, 0x05, 0x7d, 0x17, 0xbe, 0x3b, 0xfa, 0x05, 0xfa,
	0xbc, 0xaa, 0xe0, 0x02, 0xda, 0x64, 0xe8, 0xd1, 0xb0, 0xad, 0x15, 0xf4, 0x77, 0xb6, 0x1e, 0xe1,
	0xe7, 0x7e, 0xb6, 0x71, 0x01, 0x1d, 0xb0, 0xb1, 0x82, 0x5c, 0x3f, 0x87, 0xb8, 0x80, 0x0e, 0x4c,
	0xb9, 0x82, 0x5c, 0x3f, 0xc7, 0x18, 0xa0, 0x43, 0xd1, 0xf0, 0xe5, 0x43, 0x29, 0xae, 0x9a, 0xde,
	0x75, 0xb8, 0x79, 0x1b, 0xfa, 0x7c, 0xaa, 0x61, 0x80, 0x0e, 0xa5, 0x5c, 0xc1, 0x82, 0x9e, 0x83,
	0x4d, 0x5f, 0x71, 0xfd, 0x9c, 0xe2, 0x3d, 0x60, 0x1a, 0x92, 0xdc, 0x0b, 0xea, 0x97, 0x0f, 0x35,
	0xd5, 0xa0, 0xcf, 0xa5, 0x22, 0x2e, 0x60, 0x1a, 0x0c, 0x3d, 0x1a, 0xee, 0x39, 0x30, 0x6e, 0x05,
	0x0b, 0x7a, 0x0e, 0x44, 0xc3, 0x06, 0xaf, 0x65, 0x1a, 0xb8, 0x80, 0x69, 0x8a, 0xd6, 0x1a, 0x05,
	0x2f, 0x57, 0x90, 0xeb, 0x97, 0x01, 0x06, 0x98, 0x26, 0x43, 0x8f, 0x86, 0xf5, 0xb1, 0xd5, 0xdf,
	0x88, 0xf4, 0xec, 0x8f, 0x86, 0xd1, 0xa7, 0x51, 0x17, 0x03, 0xcc, 0x40, 0xb4, 0xd6, 0x28, 0x10,
	0xe1, 0xfa, 0x65, 0xc2, 0xf3, 0xbf, 0x9e, 0x6d, 0xfa, 0xe1, 0xaf, 0x67, 0x23, 0xb2, 0x8d, 0x01,
	0x12, 0x14, 0x03, 0x24, 0x28, 0x06, 0x48, 0x50, 0x0c, 0x90, 0xa0, 0x18, 0x20, 0x41, 0x31, 0x40,
	0x82, 0x62, 0x80, 0x04, 0xc5, 0x00, 0x09, 0x8a, 0x01, 0x12, 0x14, 0x03, 0x24, 0x28, 0x06, 0x48,
	0x50, 0x0c, 0x90, 0xa0, 0x14, 0x0b, 0x30, 0x30, 0x8c, 0x3e, 0x81, 0xdb, 0xa9, 0x76, 0x85, 0x14,
	0x0b, 0x70, 0xce, 0x00, 0xfa, 0x04, 0x6e, 0xa7, 0xda, 0x15, 0x52, 0x2c, 0xc0, 0x15, 0x7d, 0xe8,
	0x13, 0xb8, 0x9d, 0x6a, 0x57, 0x48, 0xb1, 0x00, 0x17, 0x5d, 0x47, 0x9f, 0xc0, 0xed, 0x54, 0xbb,
	0x42, 0x8a, 0x05, 0x18, 0xea, 0x0d, 0x0e, 0xa1, 0xcf, 0xe0, 0x66, 0xc1, 0xa1, 0x50, 0x2f, 0xfa,
	0x0c, 0xce, 0x28, 0x16, 0xa0, 0xef, 0x51, 0x75, 0x17, 0xfa, 0x0c, 0x6e, 0x56, 0xdd, 0xe5, 0x7b,
	0x84, 0x3e, 0x83, 0x33, 0x8a, 0xfd, 0x9f, 0x10, 0x91, 0xf8, 0x82, 0xa5, 0x7d, 0xa3, 0x7e, 0xf4,
	0x29, 0xdc, 0xc9, 0x97, 0xb8, 0xf1, 0x7e, 0xe0, 0x0f, 0xf4, 0x29, 0x9c, 0x51, 0x6c, 0x01, 0x45,
	0x02, 0x77, 0x6b, 0x4e, 0xa0, 0xcf, 0xe0, 0x56, 0x35, 0x27, 0x54, 0xcb, 0x4f, 0xc1, 0x05, 0x14,
	0x31, 0x4a, 0xaa, 0x7f, 0xbd, 0xb7, 0x10, 0x7d, 0x0a, 0xf7, 0x99, 0x7f, 0xa7, 0x6d, 0xb1, 0xae,
	0xdc, 0x1d, 0xb2, 0x82, 0x01, 0x8a, 0x8c, 0xcc, 0x5d, 0x77, 0xf3, 0xf1, 0x2c, 0xf4, 0x29, 0xdc,
	0xa5, 0x74, 0xf0, 0x62, 0x79, 0x71, 0x3f, 0xfa, 0x14, 0xce, 0x29, 0xf7, 0x12, 0x2c, 0x22, 0x52,
	0xdc, 0xdf, 0x5e, 0x59, 0x3a, 0x88, 0x3e, 0x85, 0x9b, 0x94, 0x0e, 0xb6, 0x57, 0xaa, 0x98, 0x9f,
	0xa2, 0x01, 0x8a, 0x04, 0x6f, 0x5f, 0x2c, 0x9f, 0x7f, 0x07, 0x7d, 0x0a, 0xb7, 0x98, 0x7f, 0xe7,
	0x62, 0x79, 0xf0, 0x36, 0xfa, 0x14, 0xe9, 0x51, 0x34, 0x40, 0x91, 0xe2, 0xfe, 0xb6, 0xc5, 0x75,
	0x47, 0x7d, 0x09, 0xf4, 0x39, 0xd0, 0x7c, 0x89, 0xba, 0xa3, 0x6d, 0x8b, 0xd5, 0x5c, 0x3f, 0x11,
	0x45, 0xef, 0x01, 0x5f, 0x89, 0x2f, 0x68, 0xaf, 0x6d, 0x5b, 0xfb, 0xb4, 0x04, 0x7d, 0x0e, 0x8c,
	0xe0, 0xd0, 0xe7, 0x97, 0x36, 0x1c, 0x09, 0xdc, 0x45, 0x9f, 0x23, 0x13, 0x8a, 0x07, 0x28, 0x22,
	0x32, 0x3a, 0x7b, 0xa0, 0xe2, 0xd6, 0xb2, 0x6b, 0x65, 0x0f, 0x43, 0xf1, 0x19, 0xe8, 0xb3, 0xe4,
	0x47, 0x60, 0x78, 0xce, 0xc0, 0x8a, 0xbe, 0x45, 0xd7, 0x43, 0xbd, 0xaa, 0x7d, 0xec, 0xfc, 0xa6,
	0x69, 0x10, 0x20, 0xa9, 0x4c, 0xd9, 0x7b, 0x40, 0x9a, 0x1e, 0x18, 0x20, 0x41, 0x31, 0x40, 0x82,
	0x62, 0x80, 0x04, 0xc5, 0x00, 0x09, 0x8a, 0x01, 0x12, 0x14, 0x03, 0x24, 0x28, 0x06, 0x48, 0x50,
	0x0c, 0x90, 0xa0, 0x18, 0x20, 0x41, 0x31, 0x40, 0x82, 0x62, 0x80, 0x04, 0xc5, 0x00, 0x09, 0x8a,
	0x01, 0x12, 0xd4, 0x7f, 0xa4, 0xb4, 0x6f, 0x86, 0x52, 0x2f, 0x3d, 0x90, 0x00, 0x00, 0x00, 0x00,
	0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

// ---------------------------------------------------------------------
// https://www.psdevwiki.com/ps3/Keys#TMDB_Key
// ---------------------------------------------------------------------
const unsigned char tmdb_hmac_key[64] = {
	0xF5, 0xDE, 0x66, 0xD2, 0x68, 0x0E, 0x25, 0x5B, 0x2D, 0xF7, 0x9E, 0x74, 0xF8, 0x90, 0xEB, 0xF3,
	0x49, 0x26, 0x2F, 0x61, 0x8B, 0xCA, 0xE2, 0xA9, 0xAC, 0xCD, 0xEE, 0x51, 0x56, 0xCE, 0x8D, 0xF2,
	0xCD, 0xF2, 0xD4, 0x8C, 0x71, 0x17, 0x3C, 0xDC, 0x25, 0x94, 0x46, 0x5B, 0x87, 0x40, 0x5D, 0x19,
	0x7C, 0xF1, 0xAE, 0xD3, 0xB7, 0xE9, 0x67, 0x1E, 0xEB, 0x56, 0xCA, 0x67, 0x53, 0xC2, 0xE6, 0xB0
};
