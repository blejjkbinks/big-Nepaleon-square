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
** for each map, main calls get_map, then prints the square or "map error".
** get_map runs the reading steps, and stops at the first one that fails:
**     1. read_rules       the rules line         -> y, empty, obstacle, full
**     2. read_first_line  the first line         -> x
**     3. alloc_map        the bitmap and the row of ints, at their exact size
**     4. read_map         every line: each cell is stored as a bit and solved
**                         right away                -> the biggest square
** then main:
**     5. print_square     the map with the square
*/

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>

#define BUF_START 64	//first size of the read buffer, in bytes
#define BUF_GROW 2		//the read buffer grows by this factor

#ifndef SPACED
# define SPACED 0		//1 prints a space between cells: make spaced
#endif

typedef struct s_buf
{
	char	*str;	//read buffer: the first line, then chunks of cap bytes
	long	len;	//bytes in str
	long	cap;	//size of str
}	t_buf;

typedef struct s_bsq
{
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

char	*get_map(int fd, t_bsq *bsq);
int		read_rules(int fd, t_bsq *bsq);
int		read_first_line(int fd, t_bsq *bsq, t_buf *buf);
int		read_map(int fd, t_bsq *bsq, t_buf *buf);
char	*ft_realloc(t_buf *buf, long size);
int		alloc_map(t_bsq *bsq);
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
	char	*map;
	int		fd;
	int		i;

	fd = 0;
	i = 1;
	while (i < argc || (argc == 1 && i == 1))
	{
		bsq = (t_bsq){0};
		if (argc > 1)
			fd = open(argv[i], O_RDONLY);
		// read and solve the map, NULL if it's invalid
		map = get_map(fd, &bsq);
		close(fd);
		if (map)
			print_square(&bsq);
		else
			write(2, "map error\n", 10);
		// whatever get_map allocated, even if it stopped halfway
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
** steps 1 to 4, one after the other: && stops at the first one that fails.
** the read buffer only lives here, and is freed here whatever happens.
** the bitmap and the int row are freed by main: print_square still needs
** them. returns the bitmap, solved, or NULL
*/
char	*get_map(int fd, t_bsq *bsq)
{
	t_buf	buf;
	int		ok;

	buf = (t_buf){0};
	ok = read_rules(fd, bsq)
		&& read_first_line(fd, bsq, &buf)
		&& alloc_map(bsq)
		&& read_map(fd, bsq, &buf);
	free(buf.str);
	if (!ok)
		return (NULL);
	return (bsq->map);
}

/*
** 1. the rules line: digits, then the empty, obstacle and full characters.
** it's at most 14 bytes: 10 digits for INT_MAX, 3 characters and '\n'.
** it's read one byte at a time into a stack array, so we never read past
** its '\n'
*/
int	read_rules(int fd, t_bsq *bsq)
{
	char	l[14];
	long	n;
	long	i;

	// read until '\n', at most 14 bytes
	n = 0;
	while (1)
	{
		if (read(fd, &l[n], 1) != 1)
			return (0);
		if (l[n] == '\n')
			break ;
		n++;
		if (n == 14)
			return (0);
	}

	// at least one digit, then exactly 3 characters
	if (n < 4)
		return (0);
	i = 0;
	while (i < n - 3 && '0' <= l[i] && l[i] <= '9')
	{
		bsq->y = bsq->y * 10 + l[i] - '0';
		i++;
	}
	if (i != n - 3 || bsq->y == 0 || bsq->y > INT_MAX)
		return (0);

	// the 3 characters must be printable and all different
	while (i < n)
	{
		if (l[i] < ' ' || l[i] > '~')
			return (0);
		i++;
	}
	bsq->e = l[n - 3];
	bsq->o = l[n - 2];
	bsq->f = l[n - 1];
	if (bsq->e == bsq->o || bsq->e == bsq->f || bsq->o == bsq->f)
		return (0);
	return (1);
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
int	read_first_line(int fd, t_bsq *bsq, t_buf *buf)
{
	long	n;
	long	k;

	ft_realloc(buf, BUF_START);
	k = 0;
	while (buf->str)
	{
		n = read(fd, buf->str + buf->len, buf->cap - buf->len);
		if (n <= 0)
			break ;
		buf->len += n;

		// look for '\n' in the bytes we just read
		while (k < buf->len && buf->str[k] != '\n')
			k++;
		if (k < buf->len)
			break ;

		// not found and the buffer is full: grow it
		if (buf->len == buf->cap)
			ft_realloc(buf, buf->cap * BUF_GROW);
	}
	bsq->x = k;

	// fails with no buffer, no '\n' before the end of the file, or an empty
	// first line
	return (buf->str && k < buf->len && k > 0);
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
int	read_map(int fd, t_bsq *bsq, t_buf *buf)
{
	long	k;
	long	row_end;
	char	c;

	k = 0;
	while (bsq->r < bsq->y)
	{
		// buffer used up: read the next chunk
		if (k == buf->len)
		{
			buf->len = read(fd, buf->str, buf->cap);
			k = 0;
			if (buf->len <= 0)
				return (0);
		}
		c = buf->str[k];
		row_end = (bsq->r + 1) * bsq->x;
		if (bsq->i == row_end && c == '\n')
			bsq->r++;
		else if (bsq->i < row_end && (c == bsq->e || c == bsq->o))
			solve_cell(bsq, c == bsq->o);
		else
			return (0);
		k++;
	}
	return (1);
}

/*
** grow the read buffer to size bytes: a new buffer, the first len bytes
** copied into it, the old one freed. buf->str can be NULL to just allocate
*/
char	*ft_realloc(t_buf *buf, long size)
{
	char	*dst;
	long	i;

	dst = (char *)malloc(size);
	if (!dst)
		write(2, "malloc fail\n", 12);
	i = 0;
	while (dst && i < buf->len)
	{
		dst[i] = buf->str[i];
		i++;
	}
	free(buf->str);
	buf->str = dst;
	buf->cap = size;
	return (dst);
}

/*
** 3. we know x and y now: allocate the bitmap once, at its exact size, and
** the int row. the row starts all 0: row[0] is the column left of the map,
** and the rest is the row above the map, so the edges need no special case
*/
int	alloc_map(t_bsq *bsq)
{
	long	c;

	bsq->map = (char *)malloc((bsq->x * bsq->y + 7) / 8);
	bsq->row = (int *)malloc((bsq->x + 1) * sizeof(int));
	if (!bsq->map || !bsq->row)
	{
		write(2, "malloc fail\n", 12);
		return (0);
	}
	c = 0;
	while (c <= bsq->x)
	{
		bsq->row[c] = 0;
		c++;
	}
	return (1);
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
** 5. the map with the square.
** the int row isn't needed anymore, so its memory is the line buffer
*/
void	print_square(t_bsq *bsq)
{
	long	r;

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
