#include <dl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <elf.h>

#include <kernel.h>

static char * program_header_types[] = {
    "PT_NULL",
    "PT_LOAD",
    "PT_DYNAMIC",
    "PT_INTERP",
    "PT_NOTE",
    "PT_SHLIB",
    "PT_PHDR",
    "PT_TLS",
    "UNKNOWN",
};

static char * section_types[] = {
    "SHT_NULL",
    "SHT_PROGBITS",
    "SHT_SYMTAB",
    "SHT_STRTAB",
    "SHT_RELA",
    "SHT_HASH",
    "SHT_DYNAMIC",
    "SHT_NOTE",
    "SHT_NOBITS",
    "SHT_REL",
    "SHT_SHLIB",
    "SHT_DYNSYM",
    "SHT_INIT_ARRAY",
    "SHT_FINI_ARRAY",
    "SHT_PREINIT_ARRAY",
    "SHT_GROUP",
    "SHT_SYMTAB_SHNDX",
    "UNKNOWN",
};

static char * dynamic_tag_types[] = {
    "DT_NULL",
    "DT_NEEDED",
    "DT_PLTRELSZ",
    "DT_PLTGOT",
    "DT_HASH",
    "DT_STRTAB",
    "DT_SYMTAB",
    "DT_RELA",
    "DT_RELASZ",
    "DT_RELAENT",
    "DT_STRSZ",
    "DT_SYMENT",
    "DT_INIT",
    "DT_FINI",
    "DT_SONAME",
    "DT_RPATH",
    "DT_SYMBOLIC",
    "DT_REL",
    "DT_RELSZ",
    "DT_RELENT",
    "DT_PLTREL",
    "DT_DEBUG",
    "DT_TEXTREL",
    "DT_JMPREL",
    "DT_BIND_NOW",
    "DT_INIT_ARRAY",
    "DT_FINI_ARRAY",
    "DT_INIT_ARRAYSZ",
    "DT_FINI_ARRAYSZ",
    "DT_RUNPATH",
    "DT_FLAGS",
    "DT_ENCODING",
    "DT_PREINIT_ARRAY",
    "DT_PREINIT_ARRAYSZ",
    "UNKNOWN"
};

static char * mips_dynamic_tag_types[] = {
    "DT_MIPS_RLD_VERSION",
    "DT_MIPS_TIME_STAMP",
    "DT_MIPS_ICHECKSUM",
    "DT_MIPS_IVERSION",
    "DT_MIPS_FLAGS",
    "DT_MIPS_BASE_ADDRESS",
    "DT_MIPS_MSYM",
    "DT_MIPS_CONFLICT",
    "DT_MIPS_LIBLIST",
    "DT_MIPS_LOCAL_GOTNO",
    "DT_MIPS_CONFLICTNO",
    "DT_MIPS_LIBLISTNO",
    "DT_MIPS_SYMTABNO",
    "DT_MIPS_UNREFEXTNO",
    "DT_MIPS_GOTSYM",
    "DT_MIPS_HIPAGENO",
    "DT_MIPS_RLD_MAP",
    "DT_MIPS_DELTA_CLASS",
    "DT_MIPS_DELTA_CLASS_NO",
    "DT_MIPS_DELTA_INSTANCE",
    "DT_MIPS_DELTA_INSTANCE_NO",
    "DT_MIPS_DELTA_RELOC",
    "DT_MIPS_DELTA_RELOC_NO",
    "DT_MIPS_DELTA_SYM",
    "DT_MIPS_DELTA_SYM_NO",
    "DT_MIPS_DELTA_CLASSSYM",
    "DT_MIPS_DELTA_CLASSSYM_NO",
    "DT_MIPS_CXX_FLAGS",
    "DT_MIPS_PIXIE_INIT",
    "DT_MIPS_SYMBOL_LIB",
    "DT_MIPS_LOCALPAGE_GOTIDX",
    "DT_MIPS_LOCAL_GOTIDX",
    "DT_MIPS_HIDDEN_GOTIDX",
    "DT_MIPS_PROTECTED_GOTIDX",
    "DT_MIPS_OPTIONS",
    "DT_MIPS_INTERFACE",
    "DT_MIPS_DYNSTR_ALIGN",
    "DT_MIPS_INTERFACE_SIZE",
    "DT_MIPS_RLD_TEXT_RESOLVE_ADDR",
    "DT_MIPS_PERF_SUFFIX",
    "DT_MIPS_COMPACT_SIZE",
    "DT_MIPS_GP_VALUE",
    "DT_MIPS_AUX_DYNAMIC",
    "DT_MIPS_PLTGOT",
    "DT_MIPS_RWPLT",
    "DT_MIPS_RLD_MAP_REL",
    "UNKNOWN"
};

static char * symbol_types[] = {
    "STT_NOTYPE",
    "STT_OBJECT",
    "STT_FUNC",
    "STT_SECTION",
    "STT_FILE",
    "STT_COMMON",
    "STT_TLS",
    "STT_NUM",
    "STT_LOOS",
    "STT_GNU_IFUNC",
    "STT_HIOS",
    "STT_LOPROC",
    "STT_HIPROC"
};

static char * binding_types[] = {
    "STB_LOCAL",
    "STB_GLOBAL",
    "STB_WEAK",
    "STB_NUM",
    "STB_LOOS",
    "STB_GNU_UNIQUE",
    "STB_HIOS",
    "STB_LOPROC",
    "STB_HIPROC"
};

static char * reloc_types[] = {
    "R_MIPS_NONE",
    "R_MIPS_16",
    "R_MIPS_32",
    "R_MIPS_REL32",
    "R_MIPS_26",
    "R_MIPS_HI16",
    "R_MIPS_LO16",
    "R_MIPS_GPREL16",
    "R_MIPS_LITERAL",
    "R_MIPS_GOT16",
    "R_MIPS_PC16",
    "R_MIPS_CALL16",
    "R_MIPS_GPREL32",
    "R_MIPS_SHIFT5",
    "R_MIPS_SHIFT6",
    "R_MIPS_64",
    "R_MIPS_GOT_DISP",
    "R_MIPS_GOT_PAGE",
    "R_MIPS_GOT_OFST",
    "R_MIPS_GOT_HI16",
    "R_MIPS_GOT_LO16",
    "R_MIPS_SUB",
    "R_MIPS_INSERT_A",
    "R_MIPS_INSERT_B",
    "R_MIPS_DELETE",
    "R_MIPS_HIGHER",
    "R_MIPS_HIGHEST",
    "R_MIPS_CALL_HI16",
    "R_MIPS_CALL_LO16",
    "R_MIPS_SCN_DISP",
    "R_MIPS_REL16",
    "R_MIPS_ADD_IMMEDIATE",
    "R_MIPS_PJUMP",
    "R_MIPS_RELGOT",
    "R_MIPS_JALR",
    "R_MIPS_TLS_DTPMOD32",
    "R_MIPS_TLS_DTPREL32",
    "R_MIPS_TLS_DTPMOD64",
    "R_MIPS_TLS_DTPREL64",
    "R_MIPS_TLS_GD",
    "R_MIPS_TLS_LDM",
    "R_MIPS_TLS_DTPREL_HI16",
    "R_MIPS_TLS_DTPREL_LO16",
    "R_MIPS_TLS_GOTTPREL",
    "R_MIPS_TLS_TPREL32",
    "R_MIPS_TLS_TPREL64",
    "R_MIPS_TLS_TPREL_HI16",
    "R_MIPS_TLS_TPREL_LO16",
    "R_MIPS_GLOB_DAT",
    "R_MIPS_COPY",
    "R_MIPS_JUMP_SLOT"
};

/**
 * load context for the currently loading module
 */
struct load_context_t {
    FILE* file; 
    struct module_t* module;
    Elf32_Ehdr elf_header;
    Elf32_Phdr* elf_program_headers;
    Elf32_Shdr* elf_sections;
    char* elf_section_nametable;
};

/**
 * create a new load context for a shared object
 * @param file the file pointer for the shared object
 * @return a pointer to the new load context, or NULL on failure
 */
static struct load_context_t* create_load_context(FILE* file) {
    struct load_context_t* ctx = (struct load_context_t*) malloc(sizeof(struct load_context_t));
    if (!ctx) {
        dl_raise("out of memory for load context");
        return NULL;
    }
    ctx->file = file;
    ctx->module = NULL;
    ctx->elf_program_headers = NULL;
    ctx->elf_sections = NULL;
    ctx->elf_section_nametable = NULL;
    return ctx;
}

/**
 * destroy a load context, cleaning up any allocated resources
 * @param ctx the load context to destroy
 * @return the loaded module or NULL if the context was invalid
 */
static struct module_t* destroy_load_context(struct load_context_t* ctx) {
    printf("destroying load context\n");
    if (!ctx) return NULL;
    if (ctx->elf_section_nametable) free(ctx->elf_section_nametable);
    if (ctx->elf_sections) free(ctx->elf_sections);
    if (ctx->elf_program_headers) free(ctx->elf_program_headers);
    struct module_t* module = ctx->module;
    free(ctx);
    return module;
}

/**
 * resolve a symbol in the module's symbol table
 * @param module the module containing the symbol
 * @param dynsym_index the index of the symbol in the dynamic symbol table
 * @return a pointer to the symbol's address, or NULL if not found
 * used by the lazy linker
 */
void* dl_resolve(struct module_t* module, uint32_t dynsym_index) {
    Elf32_Sym* symbol = &module->loader_context.dynsym[dynsym_index];
    if(symbol->st_name == STN_UNDEF) {
        printf("undefined symbol: %ld\n", dynsym_index);
        abort();
    }
    const char* name = &module->loader_context.dynstr[symbol->st_name];
    struct symbol_t* resolved = dl_find_global_symbol(name);
    const char* error = dlerror();
    if(!resolved || error) {
        printf("failed to resolve symbol: %s", name);
        if(error) {
            printf(", reason: %s", error);
        }
        printf("\n");
        abort();
    }
    uint32_t got_index = module->loader_context.local_gotno + (dynsym_index - module->loader_context.gotsym);
    uintptr_t* got_base = (uintptr_t*) (module->base + module->loader_context.pltgot);
    got_base[got_index] = (uintptr_t) resolved->address;
    printf("resolved symbol: %s at %p into got entry %ld\n", name, resolved->address, got_index);
    return resolved->address;
}

/**
 * lazy resolver stub stored in the global offset table
 * @note do not call!
 */
void dl_resolve_stub()
{
    asm volatile (
        "addiu   $sp,$sp,-32\n"
        "sd      $a0,24($sp)\n"
        "sd      $a1,16($sp)\n"
        "sd      $t3,8($sp)\n"
        "sd      $v0,0($sp)\n"
        "move    $a1, $t8\n"
        "lw      $a0, -32748($gp)\n"
        "jal     dl_resolve\n"
        "move    $t3,$v0\n"
        "ld      $a0,24($sp)\n"
        "ld      $a1,16($sp)\n"
        "ld      $ra,8($sp)\n"
        "ld      $v0,0($sp)\n"
        "addiu   $sp,$sp,32\n"
        "jr      $t3\n"
    );
}

struct module_t* dl_load_module(FILE* file)
{
    struct load_context_t* ctx = create_load_context(file);

    // read elf header and check it

    if (fread(&ctx->elf_header, sizeof(Elf32_Ehdr), 1, ctx->file) != 1) {
        destroy_load_context(ctx);
        dl_raise("failed to read elf header");
        return NULL;
    }

    printf("elf header:\n");
    printf("  Magic:   %02x %02x %02x %02x\n",
           ctx->elf_header.e_ident[EI_MAG0],
           ctx->elf_header.e_ident[EI_MAG1],
           ctx->elf_header.e_ident[EI_MAG2],
           ctx->elf_header.e_ident[EI_MAG3]);
    printf("  Class:   %d\n", ctx->elf_header.e_ident[EI_CLASS]);
    printf("  Data:    %d\n", ctx->elf_header.e_ident[EI_DATA]);
    printf("  Version: %d\n", ctx->elf_header.e_ident[EI_VERSION]);
    printf("  OS/ABI:  %d\n", ctx->elf_header.e_ident[EI_OSABI]);
    printf("  Type:    %d\n", ctx->elf_header.e_type);
    printf("  Machine: %d\n", ctx->elf_header.e_machine);
    printf("  Version: %ld\n", ctx->elf_header.e_version);
    printf("  Entry:   0x%08lX\n", ctx->elf_header.e_entry);
    printf("  PH Offset: 0x%08lX\n", ctx->elf_header.e_phoff);
    printf("  SH Offset: 0x%08lX\n", ctx->elf_header.e_shoff);
    printf("  Flags:   0x%08lX\n", ctx->elf_header.e_flags);
    printf("  EH Size: %d\n", ctx->elf_header.e_ehsize);
    printf("  PH Ent Size: %d\n", ctx->elf_header.e_phentsize);
    printf("  PH Num:  %d\n", ctx->elf_header.e_phnum);
    printf("  SH Ent Size: %d\n", ctx->elf_header.e_shentsize);
    printf("  SH Num:  %d\n", ctx->elf_header.e_shnum);
    printf("  SH Strndx: %d\n", ctx->elf_header.e_shstrndx);

    if (ctx->elf_header.e_ident[EI_MAG0] != ELFMAG0 ||
        ctx->elf_header.e_ident[EI_MAG1] != ELFMAG1 ||
        ctx->elf_header.e_ident[EI_MAG2] != ELFMAG2 ||
        ctx->elf_header.e_ident[EI_MAG3] != ELFMAG3) {
        destroy_load_context(ctx);
        dl_raise("not a valid elf file");
        return NULL;
    }

    if(ctx->elf_header.e_ident[EI_CLASS] != ELFCLASS32) {
        destroy_load_context(ctx);
        dl_raise("only 32-bit elf files are supported");
        return NULL;
    }

    if(ctx->elf_header.e_ident[EI_DATA] != ELFDATA2LSB) {
        destroy_load_context(ctx);
        dl_raise("only little-endian elf files are supported");
        return NULL;
    }

    if(ctx->elf_header.e_ident[EI_VERSION] != EV_CURRENT) {
        destroy_load_context(ctx);
        dl_raise("unsupported elf version");
        return NULL;
    }

    if(ctx->elf_header.e_ident[EI_OSABI] != ELFOSABI_NONE) {
        destroy_load_context(ctx);
        dl_raise("unsupported abi");
        return NULL;
    }

    if (ctx->elf_header.e_type != ET_DYN) {
        destroy_load_context(ctx);
        dl_raise("not a dynamic object");
        return NULL;
    }

    if (ctx->elf_header.e_machine != EM_MIPS) {
        destroy_load_context(ctx);
        dl_raise("unsupported architecture");
        return NULL;
    }

    if(ctx->elf_header.e_shentsize != sizeof(Elf32_Shdr)) {
        destroy_load_context(ctx);
        dl_raise("invalid section header size");
        return NULL;
    }

    // allocate and read program headers

    ctx->elf_program_headers = (Elf32_Phdr*) malloc(ctx->elf_header.e_phnum * sizeof(Elf32_Phdr));
    if (!ctx->elf_program_headers) {
        destroy_load_context(ctx);
        dl_raise("out of memory for program headers");
        return NULL;
    }

    fseek(file, ctx->elf_header.e_phoff, SEEK_SET);
    if (fread(ctx->elf_program_headers, sizeof(Elf32_Phdr), ctx->elf_header.e_phnum, file) != ctx->elf_header.e_phnum) {
        destroy_load_context(ctx);
        dl_raise("failed to read program headers");
        return NULL;
    }

    // allocate and read section headers

    ctx->elf_sections = (Elf32_Shdr*) malloc(ctx->elf_header.e_shnum * sizeof(Elf32_Shdr));
    if (!ctx->elf_sections) {
        destroy_load_context(ctx);
        dl_raise("out of memory for sections");
        return NULL;
    }

    fseek(file, ctx->elf_header.e_shoff, SEEK_SET);
    if (fread(ctx->elf_sections, sizeof(Elf32_Shdr), ctx->elf_header.e_shnum, file) != ctx->elf_header.e_shnum) {
        destroy_load_context(ctx);
        dl_raise("failed to read section headers");
        return NULL;
    }

    // allocate and read section string table

    ctx->elf_section_nametable = (char*) malloc(ctx->elf_sections[ctx->elf_header.e_shstrndx].sh_size * sizeof(char));
    if (!ctx->elf_section_nametable) {
        destroy_load_context(ctx);
        dl_raise("out of memory for section name tables");
        return NULL;
    }

    fseek(file, ctx->elf_sections[ctx->elf_header.e_shstrndx].sh_offset, SEEK_SET);
    if (fread(ctx->elf_section_nametable, 1, ctx->elf_sections[ctx->elf_header.e_shstrndx].sh_size, file) != ctx->elf_sections[ctx->elf_header.e_shstrndx].sh_size) {
        destroy_load_context(ctx);
        dl_raise("failed to read section name tables");
        return NULL;
    }

    // calculate module size by adding up all LOAD segments, may cause memory waste

    size_t module_size = 0;

    printf("program headers\n");
    printf("##: type       flags      offset     vaddr      paddr      filesz     memsz      align\n");
    for(int i = 0; i < ctx->elf_header.e_phnum; i++) {
        Elf32_Phdr* phdr = &ctx->elf_program_headers[i];
        if(phdr->p_type == PT_LOAD) {
            printf("%2d: %-10s 0x%08lX 0x%08lX 0x%08lX 0x%08lX 0x%08lX 0x%08lX 0x%02lX\n",
                   i, 
                   program_header_types[phdr->p_type < PT_NUM ? phdr->p_type : PT_NUM], 
                   phdr->p_flags, 
                   phdr->p_offset, 
                   phdr->p_vaddr, 
                   phdr->p_paddr, 
                   phdr->p_filesz, 
                   phdr->p_memsz, 
                   phdr->p_align
                );
            if(phdr->p_paddr != phdr->p_vaddr) {
                destroy_load_context(ctx);
                dl_raise("virtual memory unsupported");
                return NULL;
            }
            uintptr_t end = phdr->p_vaddr + phdr->p_memsz;
            if(module_size < end) {
                module_size = end;
            }
        }
    }    

    // allocate module memory

    printf("module size: 0x%08X\n", module_size);
    ctx->module = dl_allocate_module(module_size);
    if (!ctx->module) {
        destroy_load_context(ctx);
        dl_raise("failed to allocate module");
        return NULL;
    }
    printf("module base allocated at %p\n", ctx->module->base);

    // read sections, locate .dynamic, .dynsym and .dynstr sections

    Elf32_Section dynamic_index = 0, dynsym_index = 0, dynstr_index = 0;

    printf("sections\n");
    printf("###: type           flags      offset     addr       size       addralign name\n");
    for (int i = 0; i < ctx->elf_header.e_shnum; i++) {
        Elf32_Shdr* section = &ctx->elf_sections[i];
        const char* section_name;
        if(section->sh_name != SHN_UNDEF) {
            section_name = &ctx->elf_section_nametable[section->sh_name];
        } else {
            section_name = "<unnamed>";
        }

        printf("%3d: %-14s 0x%08lX 0x%08lX 0x%08lX 0x%08lX 0x%02lX      %s\n",
               i,
               section_types[section->sh_type < SHT_NUM ? section->sh_type : SHT_NUM],
               section->sh_flags,
               section->sh_offset,
               section->sh_addr,
               section->sh_size,
               section->sh_addralign,
               section_name
        );

        // load ALLOC sections into the module base

        if(section->sh_flags & SHF_ALLOC) {
            if(section->sh_addr + section->sh_size > module_size) {
                destroy_load_context(ctx);
                dl_raise("section extends beyond module size");
                return NULL;
            }
            switch(section->sh_type) {
                case SHT_NOBITS:
                    memset(ctx->module->base + section->sh_addr, 0, section->sh_size);
                break;
                default:
                    fseek(file, section->sh_offset, SEEK_SET);
                    if (fread(ctx->module->base + section->sh_addr, 1, section->sh_size, file) != section->sh_size) {
                        struct module_t* module = destroy_load_context(ctx);
                        dl_free_module(module);
                        dl_raise("failed to read section data");
                        return NULL;
                    }
                break;
            }
        }

        switch(section->sh_type) {
            case SHT_DYNAMIC:
                dynamic_index = i;
            break;
            case SHT_DYNSYM:
                dynsym_index = i;
                dynstr_index = section->sh_link;
            break;
        }
    }

    // check if all sections were found

    if(dynamic_index) {
        printf("dynamic table found at offset 0x%08lX\n", ctx->elf_sections[dynamic_index].sh_offset);
        ctx->module->loader_context.dynamic = (Elf32_Dyn*) (ctx->module->base + ctx->elf_sections[dynamic_index].sh_addr);
        ctx->module->loader_context.dynamic_count = ctx->elf_sections[dynamic_index].sh_size / sizeof(Elf32_Dyn);
    } else {
        printf("no dynamic table found\n");
        struct module_t* module = destroy_load_context(ctx);
        dl_free_module(module);
        dl_raise("no dynamic table found");
        return NULL;
    }

    if(dynsym_index) {
        printf("dynamic symbol table found at offset 0x%08lX\n", ctx->elf_sections[dynsym_index].sh_offset);
        ctx->module->loader_context.dynsym = (Elf32_Sym*) (ctx->module->base + ctx->elf_sections[dynsym_index].sh_addr);
        ctx->module->loader_context.dynsym_count = ctx->elf_sections[dynsym_index].sh_size / sizeof(Elf32_Sym);
    } else {
        printf("no dynamic symbol table found\n");
        struct module_t* module = destroy_load_context(ctx);
        dl_free_module(module);
        dl_raise("no dynamic symbol table found");
        return NULL;
    }

    if(dynstr_index) {
        printf("dynamic string table found at offset 0x%08lX\n", ctx->elf_sections[dynstr_index].sh_offset);
        ctx->module->loader_context.dynstr = (char*) (ctx->module->base + ctx->elf_sections[dynstr_index].sh_addr);
    } else {
        printf("no dynamic string table found\n");
        struct module_t* module = destroy_load_context(ctx);
        dl_free_module(module);
        dl_raise("no dynamic string table found");
        return NULL;
    }

    // parse dynamic tags
    
    printf("dynamic tags\n");
    printf("###: tag                          value\n");
    
    // count NEEDED tags to allocate dependency array

    size_t needed_count = 0;
    uint32_t needed_index = 0;
    for(size_t j = 0; j < ctx->module->loader_context.dynamic_count; j++) {
        Elf32_Dyn* dyn = &ctx->module->loader_context.dynamic[j];
        if(dyn->d_tag == DT_NEEDED) needed_count++;
    }

    ctx->module->dependencies = (char**) malloc((needed_count + 1) * sizeof(char*));
    if(!ctx->module->dependencies) {
        struct module_t* module = destroy_load_context(ctx);
        dl_free_module(module);
        dl_raise("failed to allocate memory for dependencies");
        return NULL;
    }

    ctx->module->dependencies[needed_count] = NULL;

    for(size_t j = 0; j < ctx->module->loader_context.dynamic_count; j++) {
        Elf32_Dyn* dyn = &ctx->module->loader_context.dynamic[j];

        if(dyn->d_tag >= DT_MIPS_RLD_VERSION && dyn->d_tag <= DT_MIPS_RLD_MAP_REL) {
            printf("%3zu: %-28s 0x%08lX\n", j, mips_dynamic_tag_types[dyn->d_tag - DT_MIPS_RLD_VERSION], dyn->d_un.d_val);
        } else if(dyn->d_tag >= DT_NUM) {
            printf("%3zu: 0x%08lX                   0x%08lX\n", j, dyn->d_tag, dyn->d_un.d_val);
        } else {
            printf("%3zu: %-28s 0x%08lX\n", j, dynamic_tag_types[dyn->d_tag], dyn->d_un.d_val);
        }

        switch(dyn->d_tag) {
            case DT_SONAME:
                ctx->module->name = &ctx->module->loader_context.dynstr[dyn->d_un.d_val];
                break;
            case DT_NEEDED:
                ctx->module->dependencies[needed_index++] = &ctx->module->loader_context.dynstr[dyn->d_un.d_val];
                break;
            case DT_MIPS_RLD_VERSION:
                ctx->module->loader_context.rld_version = dyn->d_un.d_val;
                break;
            case DT_MIPS_FLAGS:
                ctx->module->loader_context.flags = dyn->d_un.d_val;
                break;
            case DT_MIPS_BASE_ADDRESS:
                ctx->module->loader_context.base_address = dyn->d_un.d_val;
                break;
            case DT_MIPS_LOCAL_GOTNO:
                ctx->module->loader_context.local_gotno = dyn->d_un.d_val;
                break;
            case DT_MIPS_SYMTABNO:
                ctx->module->loader_context.symtabno = dyn->d_un.d_val;
                break;
            case DT_MIPS_GOTSYM:
                ctx->module->loader_context.gotsym = dyn->d_un.d_val;
                break;
            case DT_MIPS_RLD_MAP:
                ctx->module->loader_context.rld_map = dyn->d_un.d_val;
                break;
            case DT_PLTGOT:
                ctx->module->loader_context.pltgot = dyn->d_un.d_val;
                break;
        }
    }

    // TODO: implement base address relocation

    if(ctx->module->loader_context.base_address != 0) {
        struct module_t* module = destroy_load_context(ctx);
        dl_free_module(module);
        dl_raise("base address is not zero");
        return NULL;
    }

    // relocate global offset table

    uint32_t got_local_count = ctx->module->loader_context.local_gotno;
    uintptr_t* got_local_base = (uintptr_t*) (ctx->module->base + ctx->module->loader_context.pltgot);
    uint32_t got_global_count = ctx->module->loader_context.dynsym_count - ctx->module->loader_context.gotsym;
    uintptr_t* got_global_base = got_local_base + got_local_count;

    printf("global offset table\n");
    printf("local entries\n");
    printf("#####: value\n");
    for (size_t i = 0; i < got_local_count; i++) {
        printf("%05zu: 0x%08lX\n", i, (unsigned long) got_local_base[i]);
    }
    printf("global entries\n");
    printf("#####: value      symbol\n");
    for (size_t i = 0; i < got_global_count; i++) {

        Elf32_Sym* sym = (Elf32_Sym*) (ctx->module->base + ctx->elf_sections[dynsym_index].sh_addr + (ctx->module->loader_context.gotsym + i) * sizeof(Elf32_Sym));
        const char* name;
        if(sym->st_name != STN_UNDEF) {
            name = &ctx->module->loader_context.dynstr[sym->st_name];
        } else {
            name = "<unnamed>";
        }

        printf("%5lu: 0x%08lX %s\n", i + got_local_count, (unsigned long) got_global_base[i], name);
    }

    printf("relocating local entries\n");
    printf("#####: value        relocated\n");
    for (size_t i = 0; i < got_local_count; i++) {
        uintptr_t relocated = (uintptr_t) ctx->module->base + got_local_base[i];
        printf("%5zu: 0x%08lX -> 0x%08lX\n", i, (unsigned long) got_local_base[i], (unsigned long) relocated);
        got_local_base[i] = relocated;
    }

    printf("relocating global entries\n");
    printf("#####: value        relocated\n");
    for (size_t i = 0; i < got_global_count; i++) {
        uintptr_t relocated = (uintptr_t) ctx->module->base + got_global_base[i];
        printf("%5zu: 0x%08lX -> 0x%08lX\n", i, (unsigned long) got_global_base[i], (unsigned long) relocated);
        got_global_base[i] = relocated;
    }

    // install lazy resolver and module pointer

    got_local_base[0] = (uintptr_t) dl_resolve_stub;
    got_local_base[1] = (uintptr_t) ctx->module; // module pointer (GNU extension)

    // find and relocate sections

    printf("finding relocatable sections\n");
    for(Elf32_Section i = 0; i < ctx->elf_header.e_shnum; i++) {
        Elf32_Shdr* section = &ctx->elf_sections[i];
        const char* name;

        if(section->sh_name != SHN_UNDEF) {
            name = &ctx->elf_section_nametable[section->sh_name];
        } else {
            name = "<unnamed>";
        }

        if(section->sh_type != SHT_REL) {
            continue;
        }

        printf("section %02d (%s) at 0x%08lX: contains relocations for section %li\n", i, name, (unsigned long) section->sh_addr, section->sh_info);

        if(section->sh_entsize != sizeof(Elf32_Rel)) {
            printf("invalid relocation entry size for section %s\n", name);
            struct module_t* module = destroy_load_context(ctx);
            dl_free_module(module);     
            dl_raise("invalid relocation entry size");
            return NULL;
        }

        Elf32_Rel* reloc_data = (Elf32_Rel*) (ctx->module->base + section->sh_addr);

        printf("reloc_data: 0x%08lX\n", (unsigned long) reloc_data);

        Elf32_Shdr* reloc_section = &ctx->elf_sections[section->sh_info];

        printf("######: offset   type           symbol\n");
        for(size_t j = 0; j < section->sh_size / sizeof(Elf32_Rel); j++) {
            Elf32_Rel* reloc = &reloc_data[j];

            if(ELF32_R_TYPE(reloc->r_info) == R_MIPS_NONE) {
                continue;
            }

            Elf32_Sym* symbol = &ctx->module->loader_context.dynsym[ELF32_R_SYM(reloc->r_info)];
            Elf32_Shdr* symbol_section = &ctx->elf_sections[symbol->st_shndx];

            printf("%6zu: %08lX %-14s %03lu: ", j, reloc->r_offset, reloc_types[ELF32_R_TYPE(reloc->r_info)], ELF32_R_SYM(reloc->r_info));

            if((symbol->st_info & 0xF) != STT_SECTION) {
                printf("shared object should only contain STT_SECTION symbols\n");
                struct module_t* module = destroy_load_context(ctx);
                dl_free_module(module);
                dl_raise("unknown symbol type");
                return NULL;
            }

            if(ELF32_R_TYPE(reloc->r_info) != R_MIPS_REL32) {
                printf("shared object should only contain R_MIPS_REL32 relocations\n");
                struct module_t* module = destroy_load_context(ctx);
                dl_free_module(module);
                dl_raise("unknown symbol type");
                return NULL;
            }

            printf("internal reloc to section %i (%s); ", 
                    symbol->st_shndx, 
                    &ctx->elf_section_nametable[symbol_section->sh_name]);

            uintptr_t* reloc_target = (uintptr_t*) (ctx->module->base + reloc_section->sh_addr + reloc->r_offset);
            uintptr_t relocated;

            // A - EA + S
            if(ELF32_R_SYM(reloc->r_info) < ctx->module->loader_context.gotsym) {
                // local symbol
                // TODO: check this
                relocated = (uintptr_t) ctx->module->base + *reloc_target;
            } else {
                // global symbol
                // TODO: check this
                relocated = (uintptr_t) ctx->module->base + got_global_base[ELF32_R_SYM(reloc->r_info) - ctx->module->loader_context.gotsym] + *reloc_target;
            }

            printf("relocating to 0x%08lX -> 0x%08lX\n", (unsigned long) *reloc_target, (unsigned long) relocated);

            *reloc_target = relocated;

        }        

    }

    // everything is loaded, destroy the load context

    struct module_t* module = destroy_load_context(ctx);

    // count 'exported' symbols

    size_t symbol_count = 0;
    uint32_t symbol_index = 0;
    for(size_t i = 0; i < module->loader_context.dynsym_count; i++) {
        Elf32_Sym* sym = &module->loader_context.dynsym[i];
        if(
            sym->st_shndx != SHN_UNDEF && 
            sym->st_name != STN_UNDEF && 
            (ELF32_ST_TYPE(sym->st_info) == STT_OBJECT || ELF32_ST_TYPE(sym->st_info) == STT_FUNC)
        ) symbol_count++;
    }

    module->symbols = (struct symbol_t*) malloc(symbol_count * sizeof(struct symbol_t));
    if (!module->symbols) {
        dl_free_module(module);
        dl_raise("out of memory for module symbols");
        return NULL;
    }

    // print symbols and copy 'exported' symbols

    printf("module symbols\n");
    printf("######: size     address  type           bind        index name\n");
    for(size_t i = 0; i < module->loader_context.dynsym_count; i++) {
        Elf32_Sym* sym = &module->loader_context.dynsym[i];
        char* name;

        if(sym->st_name != STN_UNDEF) {
            name = &module->loader_context.dynstr[sym->st_name];
        } else {
            name = "<unnamed>";
        }

        sym->st_value = (uintptr_t) module->base + (uintptr_t) sym->st_value;

        if(
            sym->st_shndx != SHN_UNDEF && 
            sym->st_name != STN_UNDEF && 
            (ELF32_ST_TYPE(sym->st_info) == STT_OBJECT || ELF32_ST_TYPE(sym->st_info) == STT_FUNC)
        ) {
            struct symbol_t* symbol = &module->symbols[symbol_index++];
            symbol->name = name;
            symbol->address = (void*) sym->st_value;
            symbol->info = sym->st_info;
        }

        printf("%6zu: %08lX %08lX %-14s %-10s %6u %s\n", i, sym->st_size, sym->st_value,
                symbol_types[ELF32_ST_TYPE(sym->st_info) > STT_LOPROC ? STT_LOPROC : ELF32_ST_TYPE(sym->st_info)],
                binding_types[ELF32_ST_BIND(sym->st_info) > STB_LOPROC ? STB_LOPROC : ELF32_ST_BIND(sym->st_info)],
                sym->st_shndx, name);
    }

    // new code, new cache

    FlushCache(2);
    FlushCache(0);

    // print module name and dependencies

    printf("module name: %s\n", module->name);
    for(size_t i = 0; module->dependencies[i]; i++) {
        printf("  dependency: %s\n", module->dependencies[i]);
    }

    return module;
}

void dl_resolve_module(struct module_t* module)
{
    if (!module) {
        dl_raise("invalid module");
        return;
    }

    for(size_t i = 0; i < module->loader_context.dynsym_count; i++) {
        Elf32_Sym* sym = &module->loader_context.dynsym[i];
        if(sym->st_shndx != SHN_UNDEF) continue;
        if(sym->st_name == STN_UNDEF) continue;
        dl_resolve(module, i);
    }
}

struct symbol_t* dl_module_find_symbol(struct module_t* module, const char* name) {
    if (!module || !name) {
        dl_raise("invalid module or name");
        return NULL;
    }

    for(uint32_t i = 0; i < module->loader_context.dynsym_count; i++) {
        struct symbol_t* symbol = &module->symbols[i];
        if (!strcmp(symbol->name, name)) {
            return symbol;
        }
    }

    return NULL;
}