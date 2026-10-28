/*
** bsq_topdown.c
**
** TEMPORARY FILE, not part of the build. the whole program in one file, in
** the order it runs, so it can be read from top to bottom. it does exactly
** what src/ does, without the norm: functions are as long as they need to
** be, and there are no tricks to save lines.
**
** build and run it on its own:
**     cc -Wall -Wextra -Werror bsq_topdown.c -o bsq_topdown
**     ./bsq_topdown map
**
** the steps, for each map:
**     1. read the rules line          -> y, empty, obstacle, full
**     2. read the first line          -> x
**     3. allocate                     -> the int row, and the bitmap for stdin
**     4. read the rest, solve as we go
**     5. print: from the bitmap (stdin), or by reading the file again
*/

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define READ_BUF 4096	//max bytes per read() after the first line

#ifndef SPACED
# define SPACED 0		//1 prints a space between cells: make spaced
#endif

typedef struct s_bsq
{
	long	x;		//width, length of the first line
	long	y;		//height, number from the rules line
	char	e;		//empty character
	char	o;		//obstacle character
	char	f;		//full character, draws the square
	long	i;		//cells read so far, index of the next cell
	long	r;		//rows read (or printed) so far
	long	b;		//index of the bottom-right cell of the biggest square
	int		s;		//size of the biggest square
	int		d;		//diag, the top-left neighbour of the current cell
	int		*w;		//row of square sizes, x + 1 ints, w[0] stays 0
	char	*m;		//bitmap of obstacles, NULL when the file is read again
}	t_bsq;

int		read_rules(int fd, t_bsq *bsq);
char	*read_first_line(int fd, t_bsq *bsq, long *len, long *cap);
char	*grow(char *src, long len, long *cap, long size);
int		allocate(t_bsq *bsq, int keep_bitmap);
int		parse_chunk(char *buf, long len, t_bsq *bsq);
void	solve_cell(t_bsq *bsq, int obstacle);
void	print_from_bitmap(t_bsq *bsq);
void	print_from_file(t_bsq *bsq, char *path);
void	print_char(t_bsq *bsq, char c);
void	flush_line(t_bsq *bsq);
int		bit_get(char *bitmap, long i);
void	bit_set(char *bitmap, long i, int value);

/*
** one map per argument, or the standard input when there are none
*/
int	main(int argc, char **argv)
{
	int		i;
	int		fd;
	int		ok;
	int		keep_bitmap;
	t_bsq	bsq;
	char	*buf;
	long	len;
	long	cap;

	i = 1;
	while (i < argc || (argc == 1 && i == 1))
	{
		bsq = (t_bsq){0};
		buf = NULL;
		fd = 0;
		if (argc > 1)
			fd = open(argv[i], O_RDONLY);
		// the standard input can't be read twice, so it's kept as a bitmap
		keep_bitmap = (argc == 1);

		// 1. rules line: y and the three characters
		ok = read_rules(fd, &bsq);

		// 2. first line: x. buf keeps the bytes read so far, which can
		//    already include the start of the second line
		if (ok)
		{
			buf = read_first_line(fd, &bsq, &len, &cap);
			ok = (buf != NULL);
		}

		// 3. allocate. the read buffer grows to READ_BUF, or to the whole
		//    map if that's smaller. the int row is x + 1 ints. the bitmap
		//    is x * y bits, allocated once, only for the standard input
		if (ok && cap < READ_BUF && cap < bsq.y * (bsq.x + 1))
		{
			if (bsq.y * (bsq.x + 1) < READ_BUF)
				buf = grow(buf, len, &cap, bsq.y * (bsq.x + 1));
			else
				buf = grow(buf, len, &cap, READ_BUF);
			ok = (buf != NULL);
		}
		if (ok)
			ok = allocate(&bsq, keep_bitmap);

		// 4. parse what's in the buffer, solving each cell as it's read,
		//    then read the next chunk, until y rows are done
		while (ok && bsq.r < bsq.y)
		{
			ok = parse_chunk(buf, len, &bsq);
			if (ok && bsq.r < bsq.y)
			{
				len = read(fd, buf, cap);
				ok = (len > 0);
			}
		}
		free(buf);
		close(fd);

		// 5. print the map with the square, or the error
		if (ok)
		{
			bsq.r = 0;
			bsq.i = 0;
			if (bsq.m)
				print_from_bitmap(&bsq);
			else
				print_from_file(&bsq, argv[i]);
			// a pipe passed by name can't be read a second time
			if (bsq.r < bsq.y)
				write(2, "map error\n", 10);
		}
		else
			write(2, "map error\n", 10);
		free(bsq.w);
		free(bsq.m);

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
** it's read one byte at a time, so we never read past its '\n'
*/
int	read_rules(int fd, t_bsq *bsq)
{
	char	line[14];
	long	n;
	long	i;

	// read until '\n', at most 14 bytes
	n = 0;
	while (1)
	{
		if (read(fd, &line[n], 1) != 1)
			return (0);
		if (line[n] == '\n')
			break ;
		n++;
		if (n == 14)
			return (0);
	}

	// at least one digit, then exactly 3 characters
	if (n < 4)
		return (0);
	i = 0;
	while (i < n - 3 && '0' <= line[i] && line[i] <= '9')
	{
		bsq->y = bsq->y * 10 + line[i] - '0';
		i++;
	}
	if (i != n - 3 || bsq->y == 0 || bsq->y > 2147483647)
		return (0);

	// the 3 characters must be printable and all different
	while (i < n)
	{
		if (line[i] < ' ' || line[i] > '~')
			return (0);
		i++;
	}
	bsq->e = line[n - 3];
	bsq->o = line[n - 2];
	bsq->f = line[n - 1];
	if (bsq->e == bsq->o || bsq->e == bsq->f || bsq->o == bsq->f)
		return (0);
	return (1);
}

/*
** 2. the first line: we don't know x until we find its '\n'.
** read in chunks into a buffer that starts at 64 bytes and doubles until the
** '\n' is in it. this is the only buffer that grows.
** returns the buffer, with *len bytes in it: the first line, and maybe the
** start of the next lines, which get parsed in step 4
*/
char	*read_first_line(int fd, t_bsq *bsq, long *len, long *cap)
{
	char	*buf;
	long	n;
	long	k;

	*len = 0;
	buf = grow(NULL, 0, cap, 64);
	while (buf)
	{
		n = read(fd, buf + *len, *cap - *len);
		if (n <= 0)
		{
			free(buf);
			return (NULL);
		}

		// look for '\n' in the bytes we just read
		k = *len;
		*len += n;
		while (k < *len && buf[k] != '\n')
			k++;
		if (k < *len)
		{
			// found it: the first line is k characters long, and can't be empty
			bsq->x = k;
			if (k == 0)
			{
				free(buf);
				return (NULL);
			}
			return (buf);
		}

		// not found and the buffer is full: double it
		if (*len == *cap)
			buf = grow(buf, *len, cap, *cap * 2);
	}
	return (NULL);
}

/*
** make a new buffer of size bytes, copy the first len bytes of src into it,
** free src. src can be NULL to just allocate
*/
char	*grow(char *src, long len, long *cap, long size)
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
** 3. the int row, all 0: w[0] is the column left of the map, and the rest is
** the row above the map, so the edges need no special case.
** the bitmap, only when the map can't be read twice
*/
int	allocate(t_bsq *bsq, int keep_bitmap)
{
	long	c;

	bsq->w = (int *)malloc((bsq->x + 1) * sizeof(int));
	if (bsq->w && keep_bitmap)
		bsq->m = (char *)malloc((bsq->x * bsq->y + 7) / 8);
	if (!bsq->w || (keep_bitmap && !bsq->m))
	{
		write(2, "malloc fail\n", 12);
		return (0);
	}
	c = 0;
	while (c <= bsq->x)
	{
		bsq->w[c] = 0;
		c++;
	}
	return (1);
}

/*
** 4. check every byte of a chunk. bsq->i (cells read) and bsq->r (rows read)
** say what the next byte must be, so it doesn't matter where a chunk ends:
**     - after x cells in the row: '\n'
**     - before that: the empty or the obstacle character
** a line that is too long fails here, before anything is written past the
** map. bytes after the last row are ignored
*/
int	parse_chunk(char *buf, long len, t_bsq *bsq)
{
	long	k;
	long	row_end;

	k = 0;
	while (k < len && bsq->r < bsq->y)
	{
		row_end = (bsq->r + 1) * bsq->x;
		if (bsq->i == row_end && buf[k] == '\n')
			bsq->r++;
		else if (bsq->i < row_end && (buf[k] == bsq->e || buf[k] == bsq->o))
			solve_cell(bsq, buf[k] == bsq->o);
		else
			return (0);
		k++;
	}
	return (1);
}

/*
** 4. one cell of the dynamic programming. column c is at w[c + 1]. before
** we overwrite it, w[c + 1] still holds the row above (top), w[c] already
** holds the current row (left), and d is the old w[c] (top-left).
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
	top = bsq->w[c + 1];
	value = 0;
	if (!obstacle)
	{
		value = bsq->w[c];
		if (top < value)
			value = top;
		if (bsq->d < value)
			value = bsq->d;
		value++;
	}
	bsq->w[c + 1] = value;
	bsq->d = top;

	// strictly bigger: on a tie, the first one found (top, then left) wins
	if (value > bsq->s)
	{
		bsq->s = value;
		bsq->b = bsq->i;
	}
	if (bsq->m)
		bit_set(bsq->m, bsq->i, obstacle);
	bsq->i++;
}

/*
** 5a. standard input: build each row from the bitmap. the int row isn't
** needed anymore, so its memory is reused as the line buffer
*/
void	print_from_bitmap(t_bsq *bsq)
{
	char	*line;
	long	c;

	line = (char *)bsq->w;
	while (bsq->r < bsq->y)
	{
		c = 0;
		while (c < bsq->x)
		{
			line[c * (1 + SPACED)] = bsq->e;
			if (bit_get(bsq->m, bsq->r * bsq->x + c))
				line[c * (1 + SPACED)] = bsq->o;
			if (SPACED)
				line[c * 2 + 1] = ' ';
			c++;
		}
		flush_line(bsq);
	}
}

/*
** 5b. file: open it again, skip the rules line, and copy each row into the
** line buffer, one chunk at a time. the read buffer is on the stack
*/
void	print_from_file(t_bsq *bsq, char *path)
{
	char	buf[READ_BUF];
	long	n;
	long	k;
	int		fd;

	fd = open(path, O_RDONLY);
	buf[0] = 0;
	n = 1;
	while (n == 1 && buf[0] != '\n')
		n = read(fd, buf, 1);
	n = read(fd, buf, READ_BUF);
	while (n > 0 && bsq->r < bsq->y)
	{
		k = 0;
		while (k < n && bsq->r < bsq->y)
		{
			print_char(bsq, buf[k]);
			k++;
		}
		n = read(fd, buf, READ_BUF);
	}
	close(fd);
}

/*
** 5b. one byte of the file: a cell goes into the line buffer, '\n' prints
** the line. if the file changed since step 4, a too-long line is cut, so
** nothing is ever written past the line buffer
*/
void	print_char(t_bsq *bsq, char c)
{
	char	*line;
	long	col;

	line = (char *)bsq->w;
	col = bsq->i - bsq->r * bsq->x;
	if (c == '\n')
		flush_line(bsq);
	else if (col < bsq->x)
	{
		line[col * (1 + SPACED)] = c;
		if (SPACED)
			line[col * 2 + 1] = ' ';
		bsq->i++;
	}
}

/*
** 5. draw the square on the current row if it crosses it, add '\n', write
** the whole row at once. b is the bottom-right cell, so the square spans
** rows top .. top + s - 1 and columns left .. left + s - 1
*/
void	flush_line(t_bsq *bsq)
{
	char	*line;
	long	top;
	long	left;
	long	c;
	long	end;

	line = (char *)bsq->w;
	top = bsq->b / bsq->x - bsq->s + 1;
	left = bsq->b % bsq->x - bsq->s + 1;
	if (bsq->r >= top && bsq->r < top + bsq->s)
	{
		c = left;
		while (c < left + bsq->s)
		{
			line[c * (1 + SPACED)] = bsq->f;
			c++;
		}
	}
	// spaced: the last cell's space becomes the '\n'
	end = bsq->x * (1 + SPACED) - SPACED;
	line[end] = '\n';
	write(1, line, end + 1);
	bsq->r++;
	bsq->i = bsq->r * bsq->x;
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
