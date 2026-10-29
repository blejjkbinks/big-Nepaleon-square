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

void	alloc_map(t_bsq *bsq)
{
	long	c;

	if (bsq->err)
		return ;
	bsq->map = (char *)malloc((bsq->x * bsq->y + 7) / 8);
	bsq->row = (int *)malloc((bsq->x + 1) * sizeof(int));
	if (!bsq->map || !bsq->row)
		bsq->err = write(2, "malloc fail\n", 12);
	c = -1;
	while (!bsq->err && ++c <= bsq->x)
		bsq->row[c] = 0;
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
	ft_bit(bsq->map, bsq->i++, obstacle);
}

void	print_square(t_bsq *bsq)
{
	long	r;

	if (bsq->err && write(2, "map error\n", 10))
		return ;
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
		if (ft_bit(bsq->map, r * bsq->x + c, 2))
			line[c * (1 + SPACED)] = bsq->o;
		else if (r >= t && r < t + bsq->s && c >= l && c < l + bsq->s)
			line[c * (1 + SPACED)] = bsq->f;
		if (SPACED)
			line[c * 2 + 1] = ' ';
	}
	line[bsq->x * (1 + SPACED) - SPACED] = '\n';
	write(1, line, bsq->x * (1 + SPACED) - SPACED + 1);
}

int	ft_bit(char *arr, long i, int val)
{
	if (val == 0)
		arr[i / 8] &= ~(1 << (i % 8));
	else if (val == 1)
		arr[i / 8] |= (1 << (i % 8));
	return ((arr[i / 8] >> (i % 8)) & 1);
}
