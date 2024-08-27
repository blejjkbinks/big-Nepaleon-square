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
		return (ft_free(line));
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
