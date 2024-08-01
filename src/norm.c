/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   norm.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 17:33:30 by rdomange          #+#    #+#             */
/*   Updated: 2024/07/31 17:33:39 by rdomange         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq.h"

int	insquare(int i, int j, t_data *bsq)
{
	if (!(i >= (bsq->i - bsq->s) && bsq->i > i))
		return (0);
	if (!(j >= (bsq->j - bsq->s) && bsq->j > j))
		return (0);
	return (1);
}

static int	norm_yum(int *buffer, t_data *bsq, int x, int y)
{
	int	v;

	v = buffer[(bsq->x * ((y - 1) % 2)) + (x - 1)];
	v = ft_min(v, buffer[(bsq->x * (y % 2) + (x - 1))]);
	v = ft_min(v, buffer[(bsq->x * ((y - 1) % 2) + x)]) + 1;
	return (v);
}

static void	norm_yay(t_data *bsq, int x, int y, int v)
{
	bsq->s = v;
	bsq->i = y + 1;
	bsq->j = x + 1;
}

void	i_love_norm(t_data *bsq, unsigned int *map, int *buffer)
{
	int	x;
	int	y;
	int	v;

	y = 0;
	while (y < bsq->y)
	{
		x = 0;
		while (x < bsq->x)
		{
			if (ft_bit(map, (bsq->x * y) + x, 2))
				v = 0;
			else if (!(x * y))
				v = !ft_bit(map, (bsq->x * y) + x, 2);
			else
				v = norm_yum(buffer, bsq, x, y);
			buffer[(bsq->x * (y % 2)) + x] = v;
			if (v > bsq->s)
				norm_yay(bsq, x, y, v);
			x++;
		}
		y++;
	}
}
