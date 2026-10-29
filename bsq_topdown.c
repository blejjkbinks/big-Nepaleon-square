/*
** bsq_topdown.c
**
** TEMPORARY FILE, not part of the build. the same program as src/, with the
** same steps and function names, in the order it runs, written without the
** norm: no tricks to save lines, every check spelled out.
**
** build and run it on its own:
**     cc -Wall -Wextra -Werror bsq_topdown.c -o bsq_topdown
**     ./bsq_topdown map
**
** the steps, for each map (each one does nothing once err is set):
**     1. read_rules       the rules line         -> y, empty, obstacle, full
**     2. read_first_line  the first line         -> x
**     3. alloc_map        the bitmap and the row of ints, at their exact size
**     4. read_map         every line: each cell is stored as a bit and solved
**                         right away                -> the biggest square
**     5. print_square     the map with the square, or "map error"
*/

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUF_START 64	//first size of the read buffer, in bytes
#define BUF_GROW 2		//the read buffer grows by this factor

#ifndef SPACED
# define SPACED 0		//1 prints a space between cells: make spaced
#endif

typedef struct s_bsq
{
	int		fd;		//the map's file descriptor
	int		err;	//set by a step that fails, the next steps do nothing
	char	*buf;	//read buffer: the first line, then chunks of cap bytes
	long	len;	//bytes in buf
	long	cap;	//size of buf
	char	*map;	//bitmap of obstacles, x * y bits
	int		*row;	//row of square sizes, x + 1 ints, row[0] stays 0
	long	x;		//width, length of the first line
	long	y;		//height, number from the rules line
	char	e;		//empty character
	char	o;		//obstacle character
	char	f;		//full character, draws the square
	long	i;		//cells read so far, index of the next cell
	long	r;		//rows read so far
	long	b;		//index of the bottom-right cell of the biggest square
	int		s;		//size of the biggest square
	int		d;		//diag, the top-left neighbour of the current cell
}	t_bsq;

void	read_rules(t_bsq *bsq);
void	read_first_line(t_bsq *bsq);
void	read_map(t_bsq *bsq);
char	*ft_grow(char *src, long len, long *cap, long size);
void	alloc_map(t_bsq *bsq);
void	solve_cell(t_bsq *bsq, int obstacle);
void	print_square(t_bsq *bsq);
void	print_line(t_bsq *bsq, char *line, long r);
int		bit_get(char *bitmap, long i);
void	bit_set(char *bitmap, long i, int value);

/*
** one map per argument, or the standard input when there are none
*/
int	main(int argc, char **argv)
{
	t_bsq	bsq;
	int		i;

	i = 1;
	while (i < argc || (argc == 1 && i == 1))
	{
		bsq = (t_bsq){0};
		if (argc > 1)
			bsq.fd = open(argv[i], O_RDONLY);
		read_rules(&bsq);
		read_first_line(&bsq);
		alloc_map(&bsq);
		read_map(&bsq);
		print_square(&bsq);
		close(bsq.fd);
		free(bsq.buf);
		free(bsq.map);
		free(bsq.row);
		// a line break between maps
		if (argc > 2 && i < argc - 1)
			write(1, "\n", 1);
		i++;
	}
	return (0);
}

/*
** 1. the rules line: digits, then the empty, obstacle and full characters.
** it's at most 14 bytes: 10 digits for INT_MAX, 3 characters and '\n'.
** it's read one byte at a time into a stack array, so we never read past
** its '\n'
*/
void	read_rules(t_bsq *bsq)
{
	char	l[14];
	long	n;
	long	i;

	// read until '\n', at most 14 bytes
	bsq->err = 1;
	n = 0;
	while (1)
	{
		if (read(bsq->fd, &l[n], 1) != 1)
			return ;
		if (l[n] == '\n')
			break ;
		n++;
		if (n == 14)
			return ;
	}

	// at least one digit, then exactly 3 characters
	if (n < 4)
		return ;
	i = 0;
	while (i < n - 3 && '0' <= l[i] && l[i] <= '9')
	{
		bsq->y = bsq->y * 10 + l[i] - '0';
		i++;
	}
	if (i != n - 3 || bsq->y == 0 || bsq->y > 2147483647)
		return ;

	// the 3 characters must be printable and all different
	while (i < n)
	{
		if (l[i] < ' ' || l[i] > '~')
			return ;
		i++;
	}
	bsq->e = l[n - 3];
	bsq->o = l[n - 2];
	bsq->f = l[n - 1];
	if (bsq->e == bsq->o || bsq->e == bsq->f || bsq->o == bsq->f)
		return ;
	bsq->err = 0;
}

/*
** 2. the first line: we don't know x until we find its '\n'.
** read in chunks into a buffer that starts at BUF_START bytes and grows by
** BUF_GROW until the
** '\n' is in it. buf then holds the first line, and maybe the start of the
** next lines, which read_map parses from the start of buf.
** the buffer keeps this size for the rest of the map: every later read()
** asks for about one line
*/
void	read_first_line(t_bsq *bsq)
{
	long	n;
	long	k;

	if (bsq->err)
		return ;
	bsq->buf = ft_grow(NULL, 0, &bsq->cap, BUF_START);
	k = 0;
	while (bsq->buf)
	{
		n = read(bsq->fd, bsq->buf + bsq->len, bsq->cap - bsq->len);
		if (n <= 0)
			break ;
		bsq->len += n;

		// look for '\n' in the bytes we just read
		while (k < bsq->len && bsq->buf[k] != '\n')
			k++;
		if (k < bsq->len)
			break ;

		// not found and the buffer is full: grow it
		if (bsq->len == bsq->cap)
			bsq->buf = ft_grow(bsq->buf, bsq->len, &bsq->cap,
					bsq->cap * BUF_GROW);
	}
	bsq->x = k;

	// no buffer, no '\n' before the end of the file, or an empty first line
	if (!bsq->buf || k == bsq->len || k == 0)
		bsq->err = 1;
}

/*
** 4. check every byte, and solve each cell as soon as it's read.
** bsq->i (cells read) and bsq->r (rows read) say what the next byte must be,
** so it doesn't matter where a chunk ends:
**     - after x cells in the row: '\n'
**     - before that: the empty or the obstacle character
** a line that is too long fails here, before anything is written past the
** bitmap. bytes after the last row are ignored
*/
void	read_map(t_bsq *bsq)
{
	long	k;
	long	row_end;
	char	c;

	if (bsq->err)
		return ;
	k = 0;
	while (bsq->r < bsq->y)
	{
		// buffer used up: read the next chunk
		if (k == bsq->len)
		{
			bsq->len = read(bsq->fd, bsq->buf, bsq->cap);
			k = 0;
			if (bsq->len <= 0)
			{
				bsq->err = 1;
				return ;
			}
		}
		c = bsq->buf[k];
		row_end = (bsq->r + 1) * bsq->x;
		if (bsq->i == row_end && c == '\n')
			bsq->r++;
		else if (bsq->i < row_end && (c == bsq->e || c == bsq->o))
			solve_cell(bsq, c == bsq->o);
		else
		{
			bsq->err = 1;
			return ;
		}
		k++;
	}
}

/*
** make a new buffer of size bytes, copy the first len bytes of src into it,
** free src. src can be NULL to just allocate
*/
char	*ft_grow(char *src, long len, long *cap, long size)
{
	char	*dst;
	long	i;

	*cap = size;
	dst = (char *)malloc(size);
	if (!dst)
	{
		write(2, "malloc fail\n", 12);
		free(src);
		return (NULL);
	}
	i = 0;
	while (i < len)
	{
		dst[i] = src[i];
		i++;
	}
	free(src);
	return (dst);
}

/*
** 3. we know x and y now: allocate the bitmap once, at its exact size, and
** the int row. the row starts all 0: row[0] is the column left of the map,
** and the rest is the row above the map, so the edges need no special case
*/
void	alloc_map(t_bsq *bsq)
{
	long	c;

	if (bsq->err)
		return ;
	bsq->map = (char *)malloc((bsq->x * bsq->y + 7) / 8);
	bsq->row = (int *)malloc((bsq->x + 1) * sizeof(int));
	if (!bsq->map || !bsq->row)
	{
		write(2, "malloc fail\n", 12);
		bsq->err = 1;
		return ;
	}
	c = 0;
	while (c <= bsq->x)
	{
		bsq->row[c] = 0;
		c++;
	}
}

/*
** 4. one cell, cell number bsq->i, in column c of row bsq->r.
** column c is at row[c + 1]. before we overwrite it, row[c + 1] still holds
** the row above (top), row[c] already holds the current row (left), and d is
** the old row[c] (top-left), kept from the previous cell.
** a cell is 0 on an obstacle, otherwise 1 + the smallest of the three
*/
void	solve_cell(t_bsq *bsq, int obstacle)
{
	long	c;
	int		top;
	int		value;

	c = bsq->i - bsq->r * bsq->x;
	if (c == 0)
		bsq->d = 0;
	top = bsq->row[c + 1];
	value = 0;
	if (!obstacle)
	{
		value = bsq->row[c];
		if (top < value)
			value = top;
		if (bsq->d < value)
			value = bsq->d;
		value++;
	}
	bsq->row[c + 1] = value;
	bsq->d = top;

	// strictly bigger: on a tie, the first one found (top, then left) wins
	if (value > bsq->s)
	{
		bsq->s = value;
		bsq->b = bsq->i;
	}
	bit_set(bsq->map, bsq->i, obstacle);
	bsq->i++;
}

/*
** 5. the map with the square, or the error.
** the int row isn't needed anymore, so its memory is the line buffer
*/
void	print_square(t_bsq *bsq)
{
	long	r;

	if (bsq->err)
	{
		write(2, "map error\n", 10);
		return ;
	}
	r = 0;
	while (r < bsq->y)
	{
		print_line(bsq, (char *)bsq->row, r);
		r++;
	}
}

/*
** 5. one row: build it in the line buffer, then one write.
** b is the bottom-right cell of the square, so the square spans rows
** top .. top + s - 1 and columns left .. left + s - 1.
** spaced: the cell is at c * 2, the space after it at c * 2 + 1, and the
** last cell's space becomes the '\n'
*/
void	print_line(t_bsq *bsq, char *line, long r)
{
	long	c;
	long	top;
	long	left;
	long	end;

	top = bsq->b / bsq->x - bsq->s + 1;
	left = bsq->b % bsq->x - bsq->s + 1;
	c = 0;
	while (c < bsq->x)
	{
		if (bit_get(bsq->map, r * bsq->x + c))
			line[c * (1 + SPACED)] = bsq->o;
		else if (r >= top && r < top + bsq->s && c >= left && c < left + bsq->s)
			line[c * (1 + SPACED)] = bsq->f;
		else
			line[c * (1 + SPACED)] = bsq->e;
		if (SPACED)
			line[c * 2 + 1] = ' ';
		c++;
	}
	end = bsq->x * (1 + SPACED) - SPACED;
	line[end] = '\n';
	write(1, line, end + 1);
}

/*
** cell i is bit i % 8 of byte i / 8
*/
int	bit_get(char *bitmap, long i)
{
	return ((bitmap[i / 8] >> (i % 8)) & 1);
}

void	bit_set(char *bitmap, long i, int value)
{
	if (value)
		bitmap[i / 8] |= (1 << (i % 8));
	else
		bitmap[i / 8] &= ~(1 << (i % 8));
}
