#ifndef _UTIL_WORD_H_
#define _UTIL_WORD_H_

static inline uint16_t lowWord(uint32_t ww)
{
  return (uint16_t) ((ww) & 0xFFFF);
}

static inline uint16_t highWord(uint32_t ww)
{
  return (uint16_t) ((ww) >> 16);
}


#endif /* _UTIL_WORD_H_ */
