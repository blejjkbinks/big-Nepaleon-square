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

char	*ft_grow(char *src, long len, long *cap, long size)
{
	char	*dst;
	long	i;

	*cap = size;
	dst = (char *)malloc(size);
	if (!dst && write(2, "malloc fail\n", 12) + 2)
		return (ft_free(src));
	i = -1;
	while (++i < len)
		dst[i] = src[i];
	free(src);
	return (dst);
}

void	solve_cell(t_bsq *bsq, int obstacle)
{
	long	c;
	int		top;

	c = bsq->i - bsq->r * bsq->x;
	if (c == 0)
		bsq->d = 0;
	top = bsq->w[c + 1];
	bsq->w[c + 1] = bsq->w[c];
	if (top < bsq->w[c + 1])
		bsq->w[c + 1] = top;
	if (bsq->d < bsq->w[c + 1])
		bsq->w[c + 1] = bsq->d;
	bsq->w[c + 1] = (bsq->w[c + 1] + 1) * !obstacle;
	bsq->d = top;
	if (bsq->w[c + 1] > bsq->s)
	{
		bsq->s = bsq->w[c + 1];
		bsq->b = bsq->i;
	}
	if (bsq->m)
		ft_bit(bsq->m, bsq->i, obstacle);
	bsq->i++;
}

int	alloc_map(t_bsq *bsq, int keep)
{
	long	c;

	bsq->w = (int *)malloc((bsq->x + 1) * sizeof(int));
	if (bsq->w && keep)
		bsq->m = (char *)malloc((bsq->x * bsq->y + 7) / 8);
	if (!bsq->w || (keep && !bsq->m))
	{
		write(2, "malloc fail\n", 12);
		return (0);
	}
	c = 0;
	while (c <= bsq->x)
		bsq->w[c++] = 0;
	return (1);
}
