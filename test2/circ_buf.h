#ifndef _LINUX_CIRC_BUF_H
#define _LINUX_CIRC_BUF_H 1

struct circ_buf {
    char *buf;
    int head;
    int tail;
};

/* Return count in buffer. */
#define CIRC_CNT(head, tail, size) (((head)-(tail)) & ((size)-1))

/* Return space available, 0..size-1. We always leave one free char
   as a completely full buffer has head == tail, which is the same as
   empty */
#define CIRC_SPACE(head, tail, size) CIRC_CNT((tail), ((head)+1), (size))

#ifdef WIN32
// 貌似 vs2010 不支持 ({}) 表达式 ...
inline int CIRC_CNT_TO_END(int head, int tail, int size)
{
    int end = size - tail;
    int n = (head + end) & (size-1);
    return n < end ? n : end;
}

inline int CIRC_SPACE_TO_END(int head, int tail, int size)
{
    int end = size - 1 - head;
    int n = (end + tail) & (size-1);
    return n <= end ? n : end+1;
}

#else
/* Return count up to the end of the buffer. Carefully avoid
 * accessing head and tail more than once, so they can change
 * underneath us without returing inconsistent result. */
#define CIRC_CNT_TO_END(head, tail, size) \
    ({int end = (size) - (tail); \
      int n = ((head) + end) & ((size)-1); \
      n < end ? n : end;})

/* Return space available up to the end of the buffer */
#define CIRC_SPACE_TO_END(head, tail, size) \
    ({int end = (size) - 1 - (head); \
      int n = (end + (tail)) & ((size)-1); \
      n <= end ? n : end+1;})

#endif
#endif /** _LINUX_CIRC_BUF_H */
