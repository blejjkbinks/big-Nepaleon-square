/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq_write.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/07 15:17:56 by rdomange          #+#    #+#             */
/*   Updated: 2024/08/07 15:18:02 by rdomange         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq.h"

char	*ft_realloc(t_buf *buf, long size)
{
	char	*dst;
	long	i;

	dst = (char *)malloc(size);
	if (!dst)
		write(2, "malloc fail\n", 12);
	i = -1;
	while (dst && ++i < buf->len)
		dst[i] = buf->str[i];
	free(buf->str);
	buf->str = dst;
	buf->cap = size;
	return (dst);
}

int	alloc_map(t_bsq *bsq)
{
	long	c;

	bsq->map = (char *)malloc((bsq->x * bsq->y + 7) / 8);
	bsq->row = (int *)malloc((bsq->x + 1) * sizeof(int));
	if ((!bsq->map || !bsq->row) && write(2, "malloc fail\n", 12))
		return (0);
	c = -1;
	while (++c <= bsq->x)
		bsq->row[c] = 0;
	return (1);
}

void	solve_cell(t_bsq *bsq, int obstacle)
{
	long	c;
	int		top;
	int		*row;

	row = bsq->row;
	c = bsq->i - bsq->r * bsq->x;
	if (c == 0)
		bsq->d = 0;
	top = row[c + 1];
	row[c + 1] = row[c];
	if (top < row[c + 1])
		row[c + 1] = top;
	if (bsq->d < row[c + 1])
		row[c + 1] = bsq->d;
	row[c + 1] = (row[c + 1] + 1) * !obstacle;
	bsq->d = top;
	if (row[c + 1] > bsq->s)
	{
		bsq->s = row[c + 1];
		bsq->b = bsq->i;
	}
	bsq->map[bsq->i / 8] = (bsq->map[bsq->i / 8] & ~(1 << (bsq->i % 8)))
		| (obstacle << (bsq->i % 8));
	bsq->i++;
}

void	print_square(t_bsq *bsq)
{
	long	r;

	r = -1;
	while (++r < bsq->y)
		print_line(bsq, (char *)bsq->row, r);
}

void	print_line(t_bsq *bsq, char *line, long r)
{
	long	c;
	long	t;
	long	l;

	t = bsq->b / bsq->x - bsq->s + 1;
	l = bsq->b % bsq->x - bsq->s + 1;
	c = -1;
	while (++c < bsq->x)
	{
		line[c * (1 + SPACED)] = bsq->e;
		if ((bsq->map[(r * bsq->x + c) / 8] >> ((r * bsq->x + c) % 8)) & 1)
			line[c * (1 + SPACED)] = bsq->o;
		else if (r >= t && r < t + bsq->s && c >= l && c < l + bsq->s)
			line[c * (1 + SPACED)] = bsq->f;
		if (SPACED)
			line[c * 2 + 1] = ' ';
	}
	line[bsq->x * (1 + SPACED) - SPACED] = '\n';
	write(1, line, bsq->x * (1 + SPACED) - SPACED + 1);
}

/*
** not used anymore: its calls are written inline, in solve_cell and
** print_line, to save 200M function calls on a 10000 by 10000 map.
** cell i is bit i % 8 of byte i / 8. val 0 clears it, 1 sets it,
** anything else only reads it. returns the bit, after the change
**
** int	ft_bit(char *arr, long i, int val)
** {
** 	if (val == 0)
** 		arr[i / 8] &= ~(1 << (i % 8));
** 	else if (val == 1)
** 		arr[i / 8] |= (1 << (i % 8));
** 	return ((arr[i / 8] >> (i % 8)) & 1);
** }
*/
