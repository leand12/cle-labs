#include <stdio.h>
#include <stdlib.h>

int is_vowel(int c) {
  if (c >= 0x61) c -= 0x20;  // upper case
  return  // AEIOU
      c == 0x41 || c == 0x45 || c == 0x49 || c == 0x4F || c == 0x55;
}

int is_vowel_pt(int c) {
  if (c >= 0xE0) c -= 0x20;  // upper case
  return is_vowel(c) ||
         // ÀÁÂÃ ÈÉÊ ÌÍ
         (0xC0 <= c && c <= 0xC3) || (0xC8 <= c && c <= 0xCA) || (0xCC <= c && c <= 0xCD) ||
         // ÒÓÔÕ ÙÚ
         (0xD2 <= c && c <= 0xD5) || (0xD9 <= c && c <= 0xDA);
}

int is_alpha(int c) {
  // a-z A-Z
  return (0x41 <= c && c <= 0x5A) || (0x61 <= c && c <= 0x7A);
}

int is_alpha_pt(int c) {
  if (c >= 0xE0) c -= 0x20;  // upper case
  return is_alpha(c) ||
         // apostrophe ('), cedilla (Ç)
         c == 0x27 || c == 0xC7 ||
         // ÀÁÂÃ ÈÉÊ ÌÍ
         (0xC0 <= c && c <= 0xC3) || (0xC8 <= c && c <= 0xCA) || (0xCC <= c && c <= 0xCD) ||
         // ÒÓÔÕ ÙÚ
         (0xD2 <= c && c <= 0xD5) || (0xD9 <= c && c <= 0xDA);
}

int f_getc(FILE *fp) {
  int c = fgetc(fp);
  if (c == EOF) return EOF;
  if ((c & 0x80) == 0) {
    // 1 byte :  0xxx x
    return c;
  }
  if ((c & 0xE0) == 0xC0) {
    // 2 bytes : 110x x
    return ((c & 0x1F) << 6) + (fgetc(fp) & 0x3F);
  }
  if ((c & 0xF0) == 0xE0) { //1100 0010  000 1000 0000    1011 1010
    // 3 bytes : 1110 x
    return ((c & 0x0F) << 12) + ((fgetc(fp) & 0x3F) << 6) + (fgetc(fp) & 0x3F);
  }
  if ((c & 0xF8) == 0xF0) {
    // 4 bytes : 1111 0
    return ((c & 0x07) << 18) + ((fgetc(fp) & 0x3F) << 12) + ((fgetc(fp) & 0x3F) << 6) + (fgetc(fp) & 0x3F);
  }
  return 0;
}

int main(int argc, char **argv) {
  int words = 0, vowels = 0, consonants = 0;
  for (int i = 1; i < argc; ++i) {
    int ch, lch;
    FILE *fp;

    fp = fopen(argv[i], "rb");

    if (fp == NULL) {
      printf("Error! File %s not found.\n", argv[i]);
      exit(1);
    }

    while (1) {
      while (EOF != (ch = f_getc(fp)) && !is_alpha_pt(ch))
        ;  // skip until beginning of a word
      if (ch == EOF) break;
      vowels += is_vowel_pt(ch);

      do {
        lch = ch;
      } while (EOF != (ch = f_getc(fp)) && is_alpha_pt(ch))
        ;  // skip until ending of a word
      consonants += !is_vowel_pt(lch);
      words++;
      if (ch == EOF) break;
    }
    fclose(fp);
  }
  printf("%d %d %d \n", words, vowels, consonants);

  return 0;
}
