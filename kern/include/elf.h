#ifndef __LIBS_ELF_H__
#define __LIBS_ELF_H__

#include <defs.h>

#define ELF_MAGIC   0x464C457FU         // "\x7FELF" in little endian
/* Type for a 16-bit quantity.  */
typedef uint16_t Elf32_Half;
typedef uint16_t Elf64_Half;

/* Types for signed and unsigned 32-bit quantities.  */
typedef uint32_t Elf32_Word;
typedef	int32_t  Elf32_Sword;
typedef uint32_t Elf64_Word;
typedef	int32_t  Elf64_Sword;

/* Types for signed and unsigned 64-bit quantities.  */
typedef uint64_t Elf32_Xword;
typedef	int64_t  Elf32_Sxword;
typedef uint64_t Elf64_Xword;
typedef	int64_t  Elf64_Sxword;

/* Type of addresses.  */
typedef uint32_t Elf32_Addr;
typedef uint64_t Elf64_Addr;

/* Type of file offsets.  */
typedef uint32_t Elf32_Off;
typedef uint64_t Elf64_Off;

/* Type for version symbol information.  */
typedef Elf32_Half Elf32_Versym;
typedef Elf64_Half Elf64_Versym;
#define EI_NIDENT  12

/* file header */
// struct __elfhdr {
//     uint32_t e_magic;     // must equal ELF_MAGIC
//     uint8_t e_elf[12];
//     unsigned short e_type;      // 1=relocatable, 2=executable, 3=shared object, 4=core image
//     unsigned short e_machine;   // 3=x86, 4=68K, etc.
//     uint32_t e_version;   // file version, always 1
//     uint32_t e_entry;     // entry point if executable
//     uint32_t e_phoff;     // file position of program header or 0
//     uint32_t e_shoff;     // file position of section header or 0
//     uint32_t e_flags;     // architecture-specific flags, usually 0
//     unsigned short e_ehsize;    // size of this elf header
//     unsigned short e_phentsize; // size of an entry in program header
//     unsigned short e_phnum;     // number of entries in program header or 0
//     unsigned short e_shentsize; // size of an entry in section header
//     unsigned short e_shnum;     // number of entries in section header or 0
//     unsigned short e_shstrndx;  // section number that contains section name strings
// };

// struct elfhdr32 {
//     uint32_t e_magic;     // must equal ELF_MAGIC
//     uint8_t e_elf[12];
//     uint32_t e_type;      // 1=relocatable, 2=executable, 3=shared object, 4=core image
//     uint32_t e_machine;   // 3=x86, 4=68K, etc.
//     uint32_t e_version;   // file version, always 1
//     uint32_t e_entry;     // entry point if executable
//     uint32_t e_phoff;     // file position of program header or 0
//     uint32_t e_shoff;     // file position of section header or 0
//     uint32_t e_flags;     // architecture-specific flags, usually 0
//     uint32_t e_ehsize;    // size of this elf header
//     uint32_t e_phentsize; // size of an entry in program header
//     uint32_t e_phnum;     // number of entries in program header or 0
//     uint32_t e_shentsize; // size of an entry in section header
//     uint32_t e_shnum;     // number of entries in section header or 0
//     uint32_t e_shstrndx;  // section number that contains section name strings
// };

struct __elfhdr
{
  uint32_t e_magic;
  uint8_t	e_ident[EI_NIDENT];	/*  other info */
  Elf64_Half	e_type;			/* Object file type */
  Elf64_Half	e_machine;		/* Architecture */
  Elf64_Word	e_version;		/* Object file version */
  Elf64_Addr	e_entry;		/* Entry point virtual address */
  Elf64_Off	e_phoff;		/* Program header table file offset */
  Elf64_Off	e_shoff;		/* Section header table file offset */
  Elf64_Word	e_flags;		/* Processor-specific flags */
  Elf64_Half	e_ehsize;		/* ELF header size in bytes */
  Elf64_Half	e_phentsize;		/* Program header table entry size */
  Elf64_Half	e_phnum;		/* Program header table entry count */
  Elf64_Half	e_shentsize;		/* Section header table entry size */
  Elf64_Half	e_shnum;		/* Section header table entry count */
  Elf64_Half	e_shstrndx;		/* Section header string table index */
};
struct elfhdr64
{
  uint32_t e_magic;
  uint8_t	e_ident[EI_NIDENT];	/*  other info */
  Elf64_Word	e_type;			/* Object file type */
  Elf64_Word	e_machine;		/* Architecture */
  Elf64_Word	e_version;		/* Object file version */
  Elf64_Addr	e_entry;		/* Entry point virtual address */
  Elf64_Off	e_phoff;		/* Program header table file offset */
  Elf64_Off	e_shoff;		/* Section header table file offset */
  Elf64_Word	e_flags;		/* Processor-specific flags */
  Elf64_Word	e_ehsize;		/* ELF header size in bytes */
  Elf64_Word	e_phentsize;		/* Program header table entry size */
  Elf64_Word	e_phnum;		/* Program header table entry count */
  Elf64_Word	e_shentsize;		/* Section header table entry size */
  Elf64_Word	e_shnum;		/* Section header table entry count */
  Elf64_Word	e_shstrndx;		/* Section header string table index */
};


static inline void _load_elfhdr(unsigned char* base, struct elfhdr64 *hdr)
{
  struct __elfhdr *eh = (struct __elfhdr*)base;
  hdr->e_magic = eh->e_magic;
  hdr->e_version = eh->e_version;
  hdr->e_entry = eh->e_entry;
  hdr->e_phoff = eh->e_phoff;
  hdr->e_shoff = eh->e_shoff;
  hdr->e_flags = eh->e_flags;

  Elf64_Word t = *(Elf64_Word*)(&eh->e_type);
  hdr->e_type = t & 0xFFFF;
  hdr->e_machine = t >> 16;

  t = *(Elf64_Word*)(&eh->e_ehsize);
  hdr->e_ehsize = t & 0xFFFF;
  hdr->e_phentsize = t >> 16;

  t = *(Elf64_Word*)(&eh->e_phnum);
  hdr->e_phnum = t & 0xFFFF;
  hdr->e_shentsize = t >> 16;

  t = *(Elf64_Word*)(&eh->e_shnum);
  hdr->e_shnum = t & 0xFFFF;
  hdr->e_shstrndx = t >> 16;
}

/* program section header */
// struct proghdr {
//     uint32_t p_type;   // loadable code or data, dynamic linking info,etc.
//     uint32_t p_offset; // file offset of segment
//     uint32_t p_va;     // virtual address to map segment
//     uint32_t p_pa;     // physical address, not used
//     uint32_t p_filesz; // size of segment in file
//     uint32_t p_memsz;  // size of segment in memory (bigger if contains bss）
//     uint32_t p_flags;  // read/write/execute bits
//     uint32_t p_align;  // required alignment, invariably hardware page size
// };

struct proghdr
{
  Elf64_Word	p_type;			/* Segment type */
  Elf64_Word	p_flags;		/* Segment flags */
  Elf64_Off	p_offset;		/* Segment file offset */
  Elf64_Addr	p_va;		/* Segment virtual address */
  Elf64_Addr	p_pa;		/* Segment physical address */
  Elf64_Xword	p_filesz;		/* Segment size in file */
  Elf64_Xword	p_memsz;		/* Segment size in memory */
  Elf64_Xword	p_align;		/* Segment alignment */
};

/* values for Proghdr::p_type */
#define ELF_PT_LOAD                     1

/* flag bits for Proghdr::p_flags */
#define ELF_PF_X                        1
#define ELF_PF_W                        2
#define ELF_PF_R                        4

#endif /* !__LIBS_ELF_H__ */

