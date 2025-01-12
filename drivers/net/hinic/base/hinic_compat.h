/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2017 Huawei Technologies Co., Ltd
 */

#ifndef _HINIC_COMPAT_H_
#define _HINIC_COMPAT_H_

#include <stdint.h>
#include <sys/time.h>
#include <rte_common.h>
#include <rte_byteorder.h>
#include <rte_memzone.h>
#include <rte_memcpy.h>
#include <rte_malloc.h>
#include <rte_atomic.h>
#include <rte_spinlock.h>
#include <rte_cycles.h>
#include <rte_log.h>
#include <rte_config.h>

typedef uint8_t   u8;
typedef int8_t    s8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef int32_t   s32;
typedef uint64_t  u64;

#ifndef dma_addr_t
typedef uint64_t  dma_addr_t;
#endif

#ifndef gfp_t
#define gfp_t unsigned
#endif

#ifndef bool
#define bool int
#endif

#ifndef FALSE
#define FALSE	(0)
#endif

#ifndef TRUE
#define TRUE	(1)
#endif

#ifndef false
#define false	(0)
#endif

#ifndef true
#define true	(1)
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define HINIC_ERROR	(-1)
#define HINIC_OK	(0)

#ifndef BIT
#define BIT(n) (1 << (n))
#endif

#define upper_32_bits(n) ((u32)(((n) >> 16) >> 16))
#define lower_32_bits(n) ((u32)(n))

/* Returns X / Y, rounding up.  X must be nonnegative to round correctly. */
#define DIV_ROUND_UP(X, Y) (((X) + ((Y) - 1)) / (Y))

/* Returns X rounded up to the nearest multiple of Y. */
#define ROUND_UP(X, Y) (DIV_ROUND_UP(X, Y) * (Y))

#undef  ALIGN
#define ALIGN(x, a)  RTE_ALIGN(x, a)

#define PTR_ALIGN(p, a)		((typeof(p))ALIGN((unsigned long)(p), (a)))

/* Reported driver name. */
#define HINIC_DRIVER_NAME "net_hinic"

extern int hinic_logtype;

#define PMD_DRV_LOG(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, hinic_logtype, \
		HINIC_DRIVER_NAME": " fmt "\n", ##args)

/* common definition */
#ifndef ETH_ALEN
#define ETH_ALEN		6
#endif
#define ETH_HLEN		14
#define ETH_CRC_LEN		4
#define VLAN_PRIO_SHIFT		13
#define VLAN_N_VID		4096

/* bit order interface */
#define cpu_to_be16(o) rte_cpu_to_be_16(o)
#define cpu_to_be32(o) rte_cpu_to_be_32(o)
#define cpu_to_be64(o) rte_cpu_to_be_64(o)
#define cpu_to_le32(o) rte_cpu_to_le_32(o)
#define be16_to_cpu(o) rte_be_to_cpu_16(o)
#define be32_to_cpu(o) rte_be_to_cpu_32(o)
#define be64_to_cpu(o) rte_be_to_cpu_64(o)
#define le32_to_cpu(o) rte_le_to_cpu_32(o)

/* virt memory and dma phy memory */
#define __iomem
#define GFP_KERNEL		RTE_MEMZONE_IOVA_CONTIG
#define HINIC_PAGE_SHIFT	12
#define HINIC_PAGE_SIZE		RTE_PGSIZE_4K
#define HINIC_MEM_ALLOC_ALIGNE_MIN	8

#define HINIC_PAGE_SIZE_DPDK	6

static inline int hinic_test_bit(int nr, volatile unsigned long *addr)
{
	int res;

	rte_mb();
	res = ((*addr) & (1UL << nr)) != 0;
	rte_mb();
	return res;
}

static inline void hinic_set_bit(unsigned int nr, volatile unsigned long *addr)
{
	__sync_fetch_and_or(addr, (1UL << nr));
}

static inline void hinic_clear_bit(int nr, volatile unsigned long *addr)
{
	__sync_fetch_and_and(addr, ~(1UL << nr));
}

static inline int hinic_test_and_clear_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = (1UL << nr);

	return __sync_fetch_and_and(addr, ~mask) & mask;
}

static inline int hinic_test_and_set_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = (1UL << nr);

	return __sync_fetch_and_or(addr, mask) & mask;
}

void *dma_zalloc_coherent(void *dev, size_t size, dma_addr_t *dma_handle,
			  gfp_t flag);
void *dma_zalloc_coherent_aligned(void *dev, size_t size,
				dma_addr_t *dma_handle, gfp_t flag);
void *dma_zalloc_coherent_aligned256k(void *dev, size_t size,
				dma_addr_t *dma_handle, gfp_t flag);
void dma_free_coherent(void *dev, size_t size, void *virt, dma_addr_t phys);

/* dma pool alloc and free */
#define	pci_pool dma_pool
#define	pci_pool_alloc(pool, flags, handle) dma_pool_alloc(pool, flags, handle)
#define	pci_pool_free(pool, vaddr, addr) dma_pool_free(pool, vaddr, addr)

struct dma_pool *dma_pool_create(const char *name, void *dev, size_t size,
				size_t align, size_t boundary);
void dma_pool_destroy(struct dma_pool *pool);
void *dma_pool_alloc(struct pci_pool *pool, int flags, dma_addr_t *dma_addr);
void dma_pool_free(struct pci_pool *pool, void *vaddr, dma_addr_t dma);

#define kzalloc(size, flag) rte_zmalloc(NULL, size, HINIC_MEM_ALLOC_ALIGNE_MIN)
#define kzalloc_aligned(size, flag) rte_zmalloc(NULL, size, RTE_CACHE_LINE_SIZE)
#define kfree(ptr)            rte_free(ptr)

/* mmio interface */
static inline void writel(u32 value, volatile void  *addr)
{
	*(volatile u32 *)addr = value;
}

static inline u32 readl(const volatile void *addr)
{
	return *(const volatile u32 *)addr;
}

#define __raw_writel(value, reg) writel((value), (reg))
#define __raw_readl(reg) readl((reg))

/* Spinlock related interface */
#define hinic_spinlock_t rte_spinlock_t

#define spinlock_t rte_spinlock_t
#define spin_lock_init(spinlock_prt)	rte_spinlock_init(spinlock_prt)
#define spin_lock_deinit(lock)
#define spin_lock(spinlock_prt)		rte_spinlock_lock(spinlock_prt)
#define spin_unlock(spinlock_prt)	rte_spinlock_unlock(spinlock_prt)

static inline unsigned long get_timeofday_ms(void)
{
	struct timeval tv;

	(void)gettimeofday(&tv, NULL);

	return (unsigned long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

#define jiffies	get_timeofday_ms()
#define msecs_to_jiffies(ms)	(ms)
#define time_before(now, end)	((now) < (end))

/* misc kernel utils */
static inline u16 ilog2(u32 n)
{
	u16 res = 0;

	while (n > 1) {
		n >>= 1;
		res++;
	}

	return res;
}

/**
 * hinic_cpu_to_be32 - convert data to big endian 32 bit format
 * @data: the data to convert
 * @len: length of data to convert, must be Multiple of 4B
 **/
static inline void hinic_cpu_to_be32(void *data, u32 len)
{
	u32 i;
	u32 *mem = (u32 *)data;

	for (i = 0; i < (len >> 2); i++) {
		*mem = cpu_to_be32(*mem);
		mem++;
	}
}

/**
 * hinic_be32_to_cpu - convert data from big endian 32 bit format
 * @data: the data to convert
 * @len: length of data to convert, must be Multiple of 4B
 **/
static inline void hinic_be32_to_cpu(void *data, u32 len)
{
	u32 i;
	u32 *mem = (u32 *)data;

	for (i = 0; i < (len >> 2); i++) {
		*mem = be32_to_cpu(*mem);
		mem++;
	}
}

#endif /* _HINIC_COMPAT_H_ */
