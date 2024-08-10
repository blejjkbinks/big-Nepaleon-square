/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_map.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/06 16:35:54 by rdomange          #+#    #+#             */
/*   Updated: 2024/08/10 17:47:44 by rdomange         ###   ########.fr       */
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

char	*get_rules(int fd, t_bsq *bsq, int *cap)
{
	char	*line;
	int		i;

	line = (char *)malloc(*cap * sizeof(char));
	if (!line && write(1, "malloc fail\n", 12) + 2)
		return (NULL);
	while (read(fd, &line[bsq->i], 1) == 1 && line[bsq->i] != '\n')
		if (bsq->i++ == *cap)
			line = ft_realloc(line, bsq, cap, 0);
	i = 0;
	while ('0' <= line[i] && line[i] <= '9' && i < bsq->i - 3)
		bsq->y = (bsq->y * 10) + line[i++] - '0';
	if (bsq->i < 4 || !bsq->y || i != bsq->i - 3 || line[bsq->i] != '\n')
		return (ft_free(NULL));
	while (i++ < bsq->i)
		if (!(' ' <= line[i - 1] && line[i - 1] <= '~'))
			return (ft_free(line));
	bsq->e = line[bsq->i - 3];
	bsq->o = line[bsq->i - 2];
	bsq->f = line[bsq->i - 1];
	if (bsq->e == bsq->o || bsq->e == bsq->f || bsq->o == bsq->f)
		return (ft_free(line));
	bsq->i = 0;
	return (line);
}

char	*get_map(int fd, t_bsq *bsq)
{
	char	*map;
	int		cap;
	char	c;

	cap = 4;
	map = get_rules(fd, bsq, &cap);
	while (read(fd, &c, 1) == 1 && map)
	{
		if ((bsq->i / 8) == cap && !bsq->x)
			map = ft_realloc(map, bsq, &cap, 1);
		if (c == '\n' && !bsq->x)
			map = ft_realloc(map, bsq, &cap, 2);
		if (c == '\n' && bsq->x && !(bsq->i % bsq->x) && bsq->s++ < bsq->y)
			bsq->i--;
		else if (c == bsq->e || c == bsq->o)
			ft_bit(map, bsq->i, c == bsq->o);
		else
			return (ft_free(map));
		bsq->i++;
		if (bsq->x && bsq->i == bsq->x * bsq->y && c == '\n')
			return (map);
	}
	if ((bsq->i != bsq->x * bsq->y || !bsq->x) && map)
		return (ft_free(map));
	return (map);
}
