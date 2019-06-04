//=====================================================================
//
// KCP - A Better ARQ Protocol Implementation
// skywind3000 (at) gmail.com, 2010-2011
//  
// Features:
// + Average RTT reduce 30% - 40% vs traditional ARQ like tcp.
// + Maximum RTT reduce three times vs tcp.
// + Lightweight, distributed as a single source file.
//
//=====================================================================
#ifndef __IKCP_H__
#define __IKCP_H__

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>


//=====================================================================
// 32BIT INTEGER DEFINITION 
//=====================================================================
#ifndef __INTEGER_32_BITS__
#define __INTEGER_32_BITS__
#if defined(_WIN64) || defined(WIN64) || defined(__amd64__) || \
	defined(__x86_64) || defined(__x86_64__) || defined(_M_IA64) || \
	defined(_M_AMD64)
typedef unsigned int ISTDUINT32;
typedef int ISTDINT32;
#elif defined(_WIN32) || defined(WIN32) || defined(__i386__) || \
	defined(__i386) || defined(_M_X86)
typedef unsigned long ISTDUINT32;
typedef long ISTDINT32;
#elif defined(__MACOS__)
typedef UInt32 ISTDUINT32;
typedef SInt32 ISTDINT32;
#elif defined(__APPLE__) && defined(__MACH__)
#include <sys/types.h>
typedef u_int32_t ISTDUINT32;
typedef int32_t ISTDINT32;
#elif defined(__BEOS__)
#include <sys/inttypes.h>
typedef u_int32_t ISTDUINT32;
typedef int32_t ISTDINT32;
#elif (defined(_MSC_VER) || defined(__BORLANDC__)) && (!defined(__MSDOS__))
typedef unsigned __int32 ISTDUINT32;
typedef __int32 ISTDINT32;
#elif defined(__GNUC__)
#include <stdint.h>
typedef uint32_t ISTDUINT32;
typedef int32_t ISTDINT32;
#else 
typedef unsigned long ISTDUINT32;
typedef long ISTDINT32;
#endif
#endif


//=====================================================================
// Integer Definition
//=====================================================================
#ifndef __IINT8_DEFINED
#define __IINT8_DEFINED
typedef char IINT8;
#endif

#ifndef __IUINT8_DEFINED
#define __IUINT8_DEFINED
typedef unsigned char IUINT8;
#endif

#ifndef __IUINT16_DEFINED
#define __IUINT16_DEFINED
typedef unsigned short IUINT16;
#endif

#ifndef __IINT16_DEFINED
#define __IINT16_DEFINED
typedef short IINT16;
#endif

#ifndef __IINT32_DEFINED
#define __IINT32_DEFINED
typedef ISTDINT32 IINT32;
#endif

#ifndef __IUINT32_DEFINED
#define __IUINT32_DEFINED
typedef ISTDUINT32 IUINT32;
#endif

#ifndef __IINT64_DEFINED
#define __IINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 IINT64;
#else
typedef long long IINT64;
#endif
#endif

#ifndef __IUINT64_DEFINED
#define __IUINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 IUINT64;
#else
typedef unsigned long long IUINT64;
#endif
#endif

#ifndef INLINE
#if defined(__GNUC__)

#if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define INLINE         __inline__ __attribute__((always_inline))
#else
#define INLINE         __inline__
#endif

#elif (defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__))
#define INLINE __inline
#else
#define INLINE 
#endif
#endif

#if (!defined(__cplusplus)) && (!defined(inline))
#define inline INLINE
#endif


//=====================================================================
// QUEUE DEFINITION                    ��˫�ˣ���ʽ����
//=====================================================================
#ifndef __IQUEUE_DEF__
#define __IQUEUE_DEF__

struct IQUEUEHEAD {
	struct IQUEUEHEAD *next, *prev;
};

typedef struct IQUEUEHEAD iqueue_head;


//---------------------------------------------------------------------
// queue init                                                         
//---------------------------------------------------------------------
#define IQUEUE_HEAD_INIT(name) { &(name), &(name) }
#define IQUEUE_HEAD(name) \
	struct IQUEUEHEAD name = IQUEUE_HEAD_INIT(name)

#define IQUEUE_INIT(ptr) ( \
	(ptr)->next = (ptr), (ptr)->prev = (ptr))

#define IOFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define ICONTAINEROF(ptr, type, member) ( \
		(type*)( ((char*)((type*)ptr)) - IOFFSETOF(type, member)) )

#define IQUEUE_ENTRY(ptr, type, member) ICONTAINEROF(ptr, type, member)


//---------------------------------------------------------------------
// queue operation                     
//---------------------------------------------------------------------
#define IQUEUE_ADD(node, head) ( \
	(node)->prev = (head), (node)->next = (head)->next, \
	(head)->next->prev = (node), (head)->next = (node))

#define IQUEUE_ADD_TAIL(node, head) ( \
	(node)->prev = (head)->prev, (node)->next = (head), \
	(head)->prev->next = (node), (head)->prev = (node))

#define IQUEUE_DEL_BETWEEN(p, n) ((n)->prev = (p), (p)->next = (n))

#define IQUEUE_DEL(entry) (\
	(entry)->next->prev = (entry)->prev, \
	(entry)->prev->next = (entry)->next, \
	(entry)->next = 0, (entry)->prev = 0)

#define IQUEUE_DEL_INIT(entry) do { \
	IQUEUE_DEL(entry); IQUEUE_INIT(entry); } while (0)

#define IQUEUE_IS_EMPTY(entry) ((entry) == (entry)->next)

#define iqueue_init		IQUEUE_INIT
#define iqueue_entry	IQUEUE_ENTRY
#define iqueue_add		IQUEUE_ADD
#define iqueue_add_tail	IQUEUE_ADD_TAIL
#define iqueue_del		IQUEUE_DEL
#define iqueue_del_init	IQUEUE_DEL_INIT
#define iqueue_is_empty IQUEUE_IS_EMPTY

#define IQUEUE_FOREACH(iterator, head, TYPE, MEMBER) \
	for ((iterator) = iqueue_entry((head)->next, TYPE, MEMBER); \
		&((iterator)->MEMBER) != (head); \
		(iterator) = iqueue_entry((iterator)->MEMBER.next, TYPE, MEMBER))

#define iqueue_foreach(iterator, head, TYPE, MEMBER) \
	IQUEUE_FOREACH(iterator, head, TYPE, MEMBER)

#define iqueue_foreach_entry(pos, head) \
	for( (pos) = (head)->next; (pos) != (head) ; (pos) = (pos)->next )


#define __iqueue_splice(list, head) do {	\
		iqueue_head *first = (list)->next, *last = (list)->prev; \
		iqueue_head *at = (head)->next; \
		(first)->prev = (head), (head)->next = (first);		\
		(last)->next = (at), (at)->prev = (last); }	while (0)

#define iqueue_splice(list, head) do { \
	if (!iqueue_is_empty(list)) __iqueue_splice(list, head); } while (0)

#define iqueue_splice_init(list, head) do {	\
	iqueue_splice(list, head);	iqueue_init(list); } while (0)


#ifdef _MSC_VER
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4996)
#endif

#endif


//---------------------------------------------------------------------
// WORD ORDER
//---------------------------------------------------------------------
#ifndef IWORDS_BIG_ENDIAN
#ifdef _BIG_ENDIAN_
#if _BIG_ENDIAN_
#define IWORDS_BIG_ENDIAN 1
#endif
#endif
#ifndef IWORDS_BIG_ENDIAN
#if defined(__hppa__) || \
            defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
            (defined(__MIPS__) && defined(__MIPSEB__)) || \
            defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
            defined(__sparc__) || defined(__powerpc__) || \
            defined(__mc68000__) || defined(__s390x__) || defined(__s390__)
#define IWORDS_BIG_ENDIAN 1
#endif
#endif
#ifndef IWORDS_BIG_ENDIAN
#define IWORDS_BIG_ENDIAN  0
#endif
#endif
/*
4           1   1     2 (Byte)
+---+---+---+---+---+---+---+---+
|     conv      |cmd|frg|  wnd  |
+---+---+---+---+---+---+---+---+
|     ts        |     sn        |
+---+---+---+---+---+---+---+---+
|     una       |     len       |
+---+---+---+---+---+---+---+---+
|                               |
+             DATA              +
|                               |
+---+---+---+---+---+---+---+---+

**/

//=====================================================================
// SEGMENT
//=====================================================================
struct IKCPSEG
{
	struct IQUEUEHEAD node;	// �������Ӷ�� KCP segment��Ҳ����ǰ�����ָ��
	IUINT32 conv;			// �Ự��ţ�ͨ��˫������һ�²���ʹ�� KCP Э�齻�����ݣ�
	IUINT32 cmd;            // �������ͣ�RUDP_CMD_PUSH | RUDP_CMD_ACK | RUDP_CMD_WASK��RUDP_CMD_WINS
	IUINT32 frg;            // ��Ƭ�ı�ţ���������ݴ���MSSʱ����Ҫ�����ݽ��з�Ƭ��frg ��¼�˷�Ƭʱ�ĵ������ (�����ڰ��е�λ�ã������ָ��n�����Ľ��з��ͣ� frg = n - index - 1  [��������])
	IUINT32 wnd;            // ��д�����Ŀ��ô��ڴ�С (���մ���δ��ʹ�õĴ�С��wnd = rcv_wnd - rcv_queue.Length)
	IUINT32 ts;             // ts��¼�˷���ʱ��ʱ������������� RTT
	IUINT32 sn;             // data���ĵı�Ż���ack���ĵ�ȷ�ϱ��        ack��ǰ���ģ��ȴ���Ӧ��֡ID
	IUINT32 una;            // ��ǰ��δȷ�ϵ����ݰ��ı��                ack��ǰδӦ����С֡��
	IUINT32 len;
	IUINT32 resendts;       // ��һ���ش���ʱ��
	IUINT32 rto;            // ��ʱ�ش����Ƶ�ʱ��ϵ�������ǻᶯ̬������
	IUINT32 fastack;        // ���ֵ���ۼӣ�������һ����ֵ��ʱ��ᴥ��һ���ش����ۼƹ����յ��������֡�ŵ�ackӦ������ᱻ�ۼ�       ���ڿ��ش�
	IUINT32 xmit;           // �ñ��ı��ش���������xmit > dead_link ��ʾ���ӶϿ�
	char data[1];			// ʵ�ʴ��������
};


//---------------------------------------------------------------------
// IKCPCB
//---------------------------------------------------------------------
struct IKCPCB
{
	IUINT32 conv, mtu, mss, state;
	IUINT32 snd_una;            // ���Ͷ��еȴ���ack��С֡��
	IUINT32 snd_nxt;            // ���Ͷ�����һ�������͵�֡��
	IUINT32 rcv_nxt;            // ���ն�����һ�������յ�֡��
	IUINT32 ts_recent, ts_lastack, ssthresh;
	IINT32 rx_rttval;
	IINT32 rx_srtt;
	IINT32 rx_rto;
	IINT32 rx_minrto;
	IUINT32 snd_wnd;
	IUINT32 rcv_wnd;			// ���մ��ڴ�С
	IUINT32 rmt_wnd;
	IUINT32 cwnd;
	IUINT32 probe;              // ���󴰿�size | ���ʹ���size
	IUINT32 current;
	IUINT32 interval;           // ˢ�¼��ʱ��
	IUINT32 ts_flush;           // ��һ��flush��ʱ��
	IUINT32 xmit;               // �ش�����
	IUINT32 nrcv_buf;
	IUINT32 nsnd_buf;
	IUINT32 nrcv_que;
	IUINT32 nsnd_que;
	IUINT32 nodelay;
	IUINT32 updated;            // �Ƿ��ǵ�һ�θ���
	IUINT32 ts_probe;           // ��һ������̽��Զ�˴��ڴ�С��ʱ��
	IUINT32 probe_wait;

	IUINT32 dead_link;          // ��ʱ�ش��������ֵ���������ӶϿ�
	IUINT32 incr;               // ��������
	struct IQUEUEHEAD snd_queue;    // ���Ͷ��� buffer -> snd_queue -> snd_buf    snd_buf = ����
	struct IQUEUEHEAD rcv_queue;    // ���ն��� rcv_buf -> rcv_queue -> buffer    rcv_buf = ����
	struct IQUEUEHEAD snd_buf;
	struct IQUEUEHEAD rcv_buf;
	IUINT32 *acklist;               // �����ص�ackӦ������.���յ�һ�����ݱ���ʱ�������Ӧ��ACK���ĵ�[sn��]�Լ�[ʱ���ts]ͬʱ���뵽acklist�У����γ��� [sn1, ts1, sn2, ts2 ...] ���б�
	IUINT32 ackcount;				// ��¼acklist�д�ŵ�ACK���ĵ�����
	IUINT32 ackblock;				// acklist ����Ŀ��ó��ȣ��� acklist ����������ʱ����Ҫ��������
	void *user;
	char *buffer;                   // �ڲ���������
	int fastresend;                 // �����ش�
	int nocwnd, stream;
	int logmask;
	int(*output)(const char *buf, int len, struct IKCPCB *kcp, void *user);
	void(*writelog)(const char *log, struct IKCPCB *kcp, void *user);
};


typedef struct IKCPCB ikcpcb;

#define IKCP_LOG_OUTPUT			1
#define IKCP_LOG_INPUT			2
#define IKCP_LOG_SEND			4
#define IKCP_LOG_RECV			8
#define IKCP_LOG_IN_DATA		16
#define IKCP_LOG_IN_ACK			32
#define IKCP_LOG_IN_PROBE		64
#define IKCP_LOG_IN_WINS		128
#define IKCP_LOG_OUT_DATA		256
#define IKCP_LOG_OUT_ACK		512
#define IKCP_LOG_OUT_PROBE		1024
#define IKCP_LOG_OUT_WINS		2048

#ifdef __cplusplus
extern "C" {
#endif

	//---------------------------------------------------------------------
	// interface
	//---------------------------------------------------------------------

	// create a new kcp control object, 'conv' must equal in two endpoint
	// from the same connection. 'user' will be passed to the output callback
	// output callback can be setup like this: 'kcp->output = my_udp_output'
	ikcpcb* ikcp_create(IUINT32 conv, void *user);

	// release kcp control object
	void ikcp_release(ikcpcb *kcp);

	// set output callback, which will be invoked by kcp
	void ikcp_setoutput(ikcpcb *kcp, int(*output)(const char *buf, int len,
		ikcpcb *kcp, void *user));

	// user/upper level recv: returns size, returns below zero for EAGAIN
	int ikcp_recv(ikcpcb *kcp, char *buffer, int len);

	// user/upper level send, returns below zero for error
	int ikcp_send(ikcpcb *kcp, const char *buffer, int len);

	// update state (call it repeatedly, every 10ms-100ms), or you can ask 
	// ikcp_check when to call it again (without ikcp_input/_send calling).
	// 'current' - current timestamp in millisec. 
	void ikcp_update(ikcpcb *kcp, IUINT32 current);

	// Determine when should you invoke ikcp_update:
	// returns when you should invoke ikcp_update in millisec, if there 
	// is no ikcp_input/_send calling. you can call ikcp_update in that
	// time, instead of call update repeatly.
	// Important to reduce unnacessary ikcp_update invoking. use it to 
	// schedule ikcp_update (eg. implementing an epoll-like mechanism, 
	// or optimize ikcp_update when handling massive kcp connections)
	IUINT32 ikcp_check(const ikcpcb *kcp, IUINT32 current);

	// when you received a low level packet (eg. UDP packet), call it
	int ikcp_input(ikcpcb *kcp, const char *data, long size);

	// flush pending data
	void ikcp_flush(ikcpcb *kcp);

	// check the size of next message in the recv queue
	int ikcp_peeksize(const ikcpcb *kcp);

	// change MTU size, default is 1400
	int ikcp_setmtu(ikcpcb *kcp, int mtu);

	// set maximum window size: sndwnd=32, rcvwnd=32 by default
	int ikcp_wndsize(ikcpcb *kcp, int sndwnd, int rcvwnd);

	// get how many packet is waiting to be sent
	int ikcp_waitsnd(const ikcpcb *kcp);

	// fastest: ikcp_nodelay(kcp, 1, 20, 2, 1)
	// nodelay: 0:disable(default), 1:enable
	// interval: internal update timer interval in millisec, default is 100ms 
	// resend: 0:disable fast resend(default), 1:enable fast resend
	// nc: 0:normal congestion control(default), 1:disable congestion control
	int ikcp_nodelay(ikcpcb *kcp, int nodelay, int interval, int resend, int nc);


	void ikcp_log(ikcpcb *kcp, int mask, const char *fmt, ...);

	// setup allocator
	void ikcp_allocator(void* (*new_malloc)(size_t), void(*new_free)(void*));

	// read conv
	IUINT32 ikcp_getconv(const void *ptr);


#ifdef __cplusplus
}
#endif

#endif

