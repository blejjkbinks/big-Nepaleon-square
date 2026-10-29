/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   norminette.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/07 15:17:56 by rdomange          #+#    #+#             */
/*   Updated: 2024/08/07 15:18:02 by rdomange         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq.h"

void	solve(t_bsq *bsq)
{
	long	r;

	if (bsq->err)
		return ;
	bsq->row = (int *)malloc((bsq->x + 1) * sizeof(int));
	if (!bsq->row)
		bsq->err = write(2, "malloc fail\n", 12);
	r = -1;
	while (bsq->row && ++r <= bsq->x)
		bsq->row[r] = 0;
	r = -1;
	while (bsq->row && ++r < bsq->y)
		solve_row(bsq, r);
}

void	solve_row(t_bsq *bsq, long r)
{
	long	c;
	int		diag;
	int		top;
	int		*row;

	row = bsq->row;
	c = -1;
	diag = 0;
	while (++c < bsq->x)
	{
		top = row[c + 1];
		row[c + 1] = row[c];
		if (top < row[c + 1])
			row[c + 1] = top;
		if (diag < row[c + 1])
			row[c + 1] = diag;
		row[c + 1] = (row[c + 1] + 1) * !ft_bit(bsq->map, r * bsq->x + c, 2);
		diag = top;
		if (row[c + 1] > bsq->s)
		{
			bsq->s = row[c + 1];
			bsq->b = r * bsq->x + c;
		}
	}
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
