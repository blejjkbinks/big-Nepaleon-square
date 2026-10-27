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

int	ft_bit(char *arr, long i, int val)
{
	int	bsize;
	int	ret;

	if (!arr)
		return (-1);
	ret = 3;
	bsize = sizeof(char) * 8;
	if (val == 0)
		arr[i / bsize] &= ~(1 << (i % bsize));
	else if (val == 1)
		arr[i / bsize] |= (1 << (i % bsize));
	else if (val == 2)
		ret = (arr[i / bsize] & (1 << (i % bsize))) != 0;
	else
		return (-2);
	return (ret);
}

void	*ft_free(void *ptr)
{
	free(ptr);
	return (NULL);
}

char	*ft_realloc(char *src, t_bsq *bsq, long *cap, int step)
{
	char	*dst;
	long	i;

	if (step == 2)
		bsq->x = bsq->i;
	if (step == 2)
		*cap = ((bsq->x * bsq->y) + 8) / 8;
	else
		*cap *= 2;
	dst = (char *)malloc(*cap * sizeof(char));
	if (!dst && write(2, "malloc fail\n", 12) + 2)
		return (ft_free(src));
	i = 0;
	if (step == 0)
		bsq->i *= 8;
	while (i < bsq->i)
	{
		ft_bit(dst, i, ft_bit(src, i, 2));
		i++;
	}
	if (step == 0)
		bsq->i /= 8;
	free(src);
	return (dst);
}

void	solve_row(int *row, char *map, t_bsq *bsq, long r)
{
	long	c;
	int		diag;
	int		top;

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
		row[c + 1] = (row[c + 1] + 1) * !ft_bit(map, r * bsq->x + c, 2);
		diag = top;
		if (row[c + 1] > bsq->s)
		{
			bsq->s = row[c + 1];
			bsq->b = r * bsq->x + c;
		}
	}
}

void	print_line(char *map, t_bsq *bsq, char *line, long r)
{
	long	c;
	long	k;
	long	t;
	long	l;

	t = bsq->b / bsq->x - bsq->s + 1;
	l = bsq->b % bsq->x - bsq->s + 1;
	c = -1;
	k = 0;
	while (++c < bsq->x)
	{
		line[k] = bsq->e;
		if (ft_bit(map, r * bsq->x + c, 2))
			line[k] = bsq->o;
		else if (r >= t && r < t + bsq->s && c >= l && c < l + bsq->s)
			line[k] = bsq->f;
		if (SPACED && c + 1 < bsq->x)
			line[++k] = ' ';
		k++;
	}
	line[k] = '\n';
	write(1, line, k + 1);
}
