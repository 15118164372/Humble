
/* d3des.h -
*
*	Headers and defines for d3des.c
*	Graven Imagery, 1992.
*
* Copyright (c) 1988,1989,1990,1991,1992 by Richard Outerbridge
*	(GEnie : OUTER; CIS : [71755,204])
*/

#define D2_DES		/* include double-length support */ 
#define D3_DES		/* include triple-length support */ 

#ifdef D3_DES 
#ifndef D2_DES 
#define D2_DES		/* D2_DES is needed for D3_DES */ 
#endif 
#endif 

#define EN0	0	/* MODE == encrypt */ 
#define DE1	1	/* MODE == decrypt */ 

/* A useful alias on 68000-ish machines, but NOT USED HERE. 

typedef union {
    unsigned long blok[2];
    unsigned short word[4];
    unsigned char byte[8];
} M68K;
*/

typedef struct
{
    unsigned long KnL[32];
    unsigned long KnR[32];
    unsigned long Kn3[32];
}DESContext;

DESContext *getcontext(void);
void freecontext(DESContext *context);

void deskey(DESContext *, unsigned char *, short);
/*		      hexkey[8]     MODE
* Sets the internal key register according to the hexadecimal
* key contained in the 8 bytes of hexkey, according to the DES,
* for encryption or decryption according to MODE.
*/

void usekey(DESContext *, unsigned long *);
/*		    cookedkey[32]
* Loads the internal key register with the data in cookedkey.
*/

void cpkey(DESContext *, unsigned long *);
/*		   cookedkey[32]
* Copies the contents of the internal key register into the storage
* located at &cookedkey[0].
*/

void des(DESContext *, unsigned char *, unsigned char *);
/*		    from[8]	      to[8]
* Encrypts/Decrypts (according to the key currently loaded in the
* internal key register) one block of eight bytes at address 'from'
* into the block at address 'to'.  They can be the same.
*/

#ifdef D2_DES 

#define desDkey(context,a,b)	des2key((context),(a),(b)) 
void des2key(DESContext *, unsigned char *, short);
/*		      hexkey[16]     MODE
* Sets the internal key registerS according to the hexadecimal
* keyS contained in the 16 bytes of hexkey, according to the DES,
* for DOUBLE encryption or decryption according to MODE.
* NOTE: this clobbers all three key registers!
*/

void Ddes(DESContext *, unsigned char *, unsigned char *);
/*		    from[8]	      to[8]
* Encrypts/Decrypts (according to the keyS currently loaded in the
* internal key registerS) one block of eight bytes at address 'from'
* into the block at address 'to'.  They can be the same.
*/

void D2des(DESContext *, unsigned char *, unsigned char *);
/*		    from[16]	      to[16]
* Encrypts/Decrypts (according to the keyS currently loaded in the
* internal key registerS) one block of SIXTEEN bytes at address 'from'
* into the block at address 'to'.  They can be the same.
*/

void makekey(DESContext *, char *, unsigned char *);
/*		*password,	single-length key[8]
* With a double-length default key, this routine hashes a NULL-terminated
* string into an eight-byte random-looking key, suitable for use with the
* deskey() routine.
*/

#define makeDkey(context,a,b)	make2key((context), (a),(b)) 
void make2key(DESContext *, char *, unsigned char *);
/*		*password,	double-length key[16]
* With a double-length default key, this routine hashes a NULL-terminated
* string into a sixteen-byte random-looking key, suitable for use with the
* des2key() routine.
*/

#ifndef D3_DES	/* D2_DES only */ 

#define useDkey(context,a)	use2key((context),(a)) 
#define cpDkey(context,a)	cp2key((context),(a)) 

void use2key(DESContext *, unsigned long *);
/*		    cookedkey[64]
* Loads the internal key registerS with the data in cookedkey.
* NOTE: this clobbers all three key registers!
*/

void cp2key(DESContext *, unsigned long *);
/*		   cookedkey[64]
* Copies the contents of the internal key registerS into the storage
* located at &cookedkey[0].
*/

#else	/* D3_DES too */ 

#define useDkey(context,a)	use3key((context),(a)) 
#define cpDkey(context,a)	cp3key((context),(a)) 

void des3key(DESContext *, unsigned char *, short);
/*		      hexkey[24]     MODE
* Sets the internal key registerS according to the hexadecimal
* keyS contained in the 24 bytes of hexkey, according to the DES,
* for DOUBLE encryption or decryption according to MODE.
*/

void use3key(DESContext *, unsigned long *);
/*		    cookedkey[96]
* Loads the 3 internal key registerS with the data in cookedkey.
*/

void cp3key(DESContext *, unsigned long *);
/*		   cookedkey[96]
* Copies the contents of the 3 internal key registerS into the storage
* located at &cookedkey[0].
*/

void make3key(DESContext *, char *, unsigned char *);
/*		*password,	triple-length key[24]
* With a triple-length default key, this routine hashes a NULL-terminated
* string into a twenty-four-byte random-looking key, suitable for use with
* the des3key() routine.
*/

void D3des(DESContext *, unsigned char *, unsigned char *);

#endif	/* D3_DES */ 
#endif	/* D2_DES */ 

/* d3des.h V5.09 rwo 9208.04 15:06 Graven Imagery
********************************************************************/
