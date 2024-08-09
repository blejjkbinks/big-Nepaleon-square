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

int	neighbours(int *buffer, char *map, t_bsq *bsq, int i)
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

int	insquare(long i, t_bsq bsq)
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

void	*ft_free(void *ptr)
{
	free(ptr);
	return (NULL);
}
