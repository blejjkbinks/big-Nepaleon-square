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

int	ft_bit(char *arr, int i, int val)
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

char	*ft_realloc(char *src, t_bsq *bsq, int *cap, int step)
{
	char	*dst;
	int		i;

	if (step == 2)
		bsq->x = bsq->i;
	if (step == 2)
		*cap = ((bsq->x * bsq->y) + 8) / 8;
	else
		*cap *= 2;
	dst = (char *)malloc(*cap * sizeof(char));
	if (!dst && write(1, "malloc fail\n", 12) + 2)
		return (NULL);
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

int	neighbours(int *buffer, char *map, t_bsq *bsq, long i)
{
	int	cap;
	int	ret;

	cap = bsq->x + 2;
	if (ft_bit(map, i, 2))
		ret = 0;
	else if (!((i / bsq->x) * (i % bsq->x)))
		ret = 1;
	else
	{
		ret = buffer[(i - 1) % cap];
		if (ret > buffer[(i + 1) % cap])
			ret = buffer[(i + 1) % cap];
		if (ret > buffer[(i + 2) % cap])
			ret = buffer[(i + 2) % cap];
		ret++;
	}
	return (ret);
}

int	insquare(t_bsq bsq, long i)
{
	if (!bsq.s)
		return (0);
	if (!(i / bsq.x > (bsq.i / bsq.x) - bsq.s))
		return (0);
	if (!(i / bsq.x <= bsq.i / bsq.x))
		return (0);
	if (!(i % bsq.x > (bsq.i % bsq.x) - bsq.s))
		return (0);
	if (!(i % bsq.x <= bsq.i % bsq.x))
		return (0);
	return (1);
}
