#ifndef BITARRAY_H_
#define BITARRAY_H_

	#include <stdbool.h>
	#include <limits.h>
	#include <unistd.h>

	/* position of bit within character */
	#define BIT_CHAR(bit)         ((bit) / CHAR_BIT)

	/* array index for character containing bit */
	#define BIT_IN_CHAR(bit)      (0x80 >> (CHAR_BIT - 1 - ((bit)  % CHAR_BIT)))


	typedef struct {
		char *bitarray;
		size_t size;
	} t_bitarray;

	t_bitarray 	*bitarray_create(char *bitarray, size_t size);
	bool 		 bitarray_test_bit(t_bitarray*, off_t bit_index);
	void		 bitarray_set_bit(t_bitarray*, off_t bit_index);
	void		 bitarray_clean_bit(t_bitarray*, off_t bit_index);
	size_t		 bitarray_get_max_bit(t_bitarray*);
	void 		 bitarray_destroy(t_bitarray*);

#endif /* BITARRAY_H_ */
