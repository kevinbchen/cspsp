/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * pspidstor.h - Prototypes for the sceIdstorage library.
 *
 *
 */
#ifndef PSPIDSTOR_H
#define PSPIDSTOR_H

#ifdef __cplusplus
extern "C" {
#endif

// Idstorage Fonction 

int sceIdStorageFlush(void);
/* sceIdStorageLookup() - Retrieves the value associated with a key 
 * args: key    = id-storage key 
 *       offset = offset within the 512-byte leaf 
 *       buf    = buffer with enough storage 
 *       len    = amount of data to retrieve (offset + len must be <= 512 bytes) 
 */ 
int sceIdStorageLookup(u16 key, u32 offset, void *buf, u32 len); 

/* sceIdStorageReadLeaf() - Retrieves the whole 512-byte container for the key 
 * args: key = id-storage key 
 *       buf = buffer with at last 512-bytes of storage 
 */ 
int sceIdStorageReadLeaf(u16 key, void *buf);
/* sceIdStorageWriteLeaf() - Write the whole 512-byte container for the key 
 * args: key = id-storage key 
 *       buf = buffer with at last 512-bytes of storage 
 */ 
int sceIdStorageWriteLeaf(u16 key, void *buf);

#ifdef __cplusplus
}
#endif

#endif /* PSPIDSTOR_H */
