#ifndef __LIBS_ATOMIC_H__
#define __LIBS_ATOMIC_H__

#include <loongarch.h>
#include <intr.h>
#include <assert.h>
/* Atomic operations that C can't guarantee us. Useful for resource counting etc.. */




typedef struct {
  volatile int counter;
} atomic_t;

/* *
 * atomic_read - read atomic variable
 * @v:  pointer of type atomic_t
 *
 * Atomically reads the value of @v.
 * */
static __always_inline int
atomic_read(const atomic_t *v) {
  return v->counter;
}

/* *
 * atomic_set - set atomic variable
 * @v:  pointer of type atomic_t
 * @i:  required value
 *
 * Atomically sets the value of @v to @i.
 * */
static __always_inline void
atomic_set(atomic_t *v, int i) {
  v->counter = i;
}

/* *
 * atomic_add - add integer to atomic variable
 * @v:  pointer of type atomic_t
 * @i:  integer value to add
 *
 * Atomically adds @i to @v.
 * */
static __always_inline void
atomic_add(atomic_t *v, int i) {
  v->counter += i;
}

/* *
 * atomic_sub - subtract integer from atomic variable
 * @v:  pointer of type atomic_t
 * @i:  integer value to subtract
 *
 * Atomically subtracts @i from @v.
 * */
static __always_inline void
atomic_sub(atomic_t *v, int i) {
  v->counter -= i;
}

/* *
 * atomic_sub_test_zero - subtract value from variable and test result
 * @v:  pointer of type atomic_t
 * @i:  integer value to subtract
 *
 * Atomically subtracts @i from @v and
 * returns true if the result is zero, or false for all other cases.
 * */
static __always_inline bool
atomic_sub_test_zero(atomic_t *v, int i) {
  unsigned char c = 0;
  int intr_flag;
  local_intr_save (intr_flag);
  v->counter -= i;
  if (v->counter == 0)
    c = 1;
  local_intr_restore (intr_flag);
  return c != 0;

}

/* *
 * atomic_inc - increment atomic variable
 * @v:  pointer of type atomic_t
 *
 * Atomically increments @v by 1.
 * */
static __always_inline void
atomic_inc(atomic_t *v) {
  atomic_add(v, 1);
}

/* *
 * atomic_dec - decrement atomic variable
 * @v:  pointer of type atomic_t
 *
 * Atomically decrements @v by 1.
 * */
static __always_inline void
atomic_dec(atomic_t *v) {
  atomic_sub(v, 1);
}

/* *
 * atomic_inc_test_zero - increment and test
 * @v:  pointer of type atomic_t
 *
 * Atomically increments @v by 1 and
 * returns true if the result is zero, or false for all other cases.
 * */
static __always_inline bool
atomic_inc_test_zero(atomic_t *v) {
  unsigned char c = 0;
  int intr_flag;
  local_intr_save (intr_flag);
  atomic_inc(v);
  if (v->counter == 0)
    c = 1;
  local_intr_restore (intr_flag);
  return c != 0;

}

/* *
 * atomic_dec_test_zero - decrement and test
 * @v:  pointer of type atomic_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other cases.
 * */
static __always_inline bool
atomic_dec_test_zero(atomic_t *v) {
  unsigned char c = 0;
  int intr_flag;
  local_intr_save (intr_flag);
  atomic_dec(v);
  if (v->counter == 0)
    c = 1;
  local_intr_restore (intr_flag);
  return c != 0;
}

/* *
 * atomic_add_return - add integer and return
 * @i:  integer value to add
 * @v:  pointer of type atomic_t
 *
 * Atomically adds @i to @v and returns @i + @v
 * Requires Modern 486+ processor
 * */
static __always_inline int
atomic_add_return(atomic_t *v, int i) {
  int intr_flag;
  local_intr_save (intr_flag);
  v->counter += i;
  local_intr_restore (intr_flag);
  return v->counter;

}

/* *
 * atomic_sub_return - subtract integer and return
 * @v:  pointer of type atomic_t
 * @i:  integer value to subtract
 *
 * Atomically subtracts @i from @v and returns @v - @i
 * */
static __always_inline int
atomic_sub_return(atomic_t *v, int i) {
  return atomic_add_return(v, -i);
}

/* *
 * set_bit - Atomically set a bit in memory
 * @nr:     the bit to set
 * @addr:   the address to start counting from
 *
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 * */
static __always_inline void
set_bit(int nr, uint32_t *addr) {
  int intr_flag;
  local_intr_save (intr_flag);
  *addr |= (1 << nr);
  local_intr_restore (intr_flag);
}

/* *
 * clear_bit - Atomically clears a bit in memory
 * @nr:     the bit to clear
 * @addr:   the address to start counting from
 * */
static __always_inline void
clear_bit(int nr, volatile uint32_t *addr) {
  int intr_flag;
  local_intr_save (intr_flag);
  *addr &= ~(1 << nr);
  local_intr_restore (intr_flag);

}

/* *
 * change_bit - Atomically toggle a bit in memory
 * @nr:     the bit to change
 * @addr:   the address to start counting from
 * */
static __always_inline void
change_bit(int nr, volatile uint32_t *addr) {
  int intr_flag;
  local_intr_save (intr_flag);
  //asm volatile ("l.xor %0, %0, %1" :"=m" (*(volatile long *)addr) : "Ir" (1 << nr));
  //#error Fill HERE
  //asm volatile ("ldr r0, [%0]");
  *(volatile long*)addr ^= (1<<nr);
  local_intr_restore (intr_flag);

}

/* *
 * test_and_set_bit - Atomically set a bit and return its old value
 * @nr:     the bit to set
 * @addr:   the address to count from
 * */
static __always_inline bool
test_and_set_bit(int nr, volatile uint32_t *addr) {
  unsigned char c = 0;
  int intr_flag;
  local_intr_save (intr_flag);
  if (*addr & (1 << nr))
    c = 1;
  *addr |= (1 << nr);
  local_intr_restore (intr_flag);
  return c != 0;

}

/* *
 * test_and_clear_bit - Atomically clear a bit and return its old value
 * @nr:     the bit to clear
 * @addr:   the address to count from
 * */
static __always_inline bool
test_and_clear_bit(int nr, volatile uint32_t *addr) {
  unsigned char c = 0;
  int intr_flag;
  local_intr_save (intr_flag);
  if (*addr & (1 << nr))
    c = 1;
  *addr &= ~(1 << nr);
  local_intr_restore (intr_flag);
  return c != 0;

}

/* *
 * test_and_change_bit - Atomically change a bit and return its old value
 * @nr:     the bit to change
 * @addr:   the address to count from
 * */
static __always_inline bool
test_and_change_bit(int nr, volatile uint32_t *addr) {
  unsigned char c = 0;
  int intr_flag;
  local_intr_save (intr_flag);
  if (*addr & (1 << nr))
    c = 1;
  //      asm volatile ("l.xor %0, %0, %1" :"=m" (*(volatile long *)addr) : "Ir" (1 << nr));
  //#error Fill HERE
  *(volatile long*)addr ^= (1<<nr); 
  local_intr_restore (intr_flag);
  return 0;

}

/* *
 * test_bit - Determine whether a bit is set
 * @nr:     the bit to test
 * @addr:   the address to count from
 * */
static __always_inline bool
test_bit(int nr, volatile uint32_t *addr) {
  unsigned char c = 0;
  int intr_flag;
  local_intr_save (intr_flag);
  if (*addr & (1 << nr))
    c = 1;
  local_intr_restore (intr_flag);
  return c != 0;

}

#endif /* !__LIBS_ATOMIC_H__ */

