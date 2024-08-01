/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mallocmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 16:48:44 by rdomange          #+#    #+#             */
/*   Updated: 2024/07/31 16:48:48 by rdomange         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq.h"

int	ft_bit(unsigned int *arr, int i, int val)
{
	int	bsize;
	int	ret;

	if (!arr)
		return (-1);
	bsize = sizeof(unsigned int) * 8;
	if (val == 1)
		arr[i / bsize] |= (1UL << (i % bsize));
	else if (val == 0)
		arr[i / bsize] &= ~(1UL << (i % bsize));
	ret = (arr[i / bsize] & (1UL << (i % bsize))) != 0;
	return (ret);
}

static unsigned int	*ft_freenull(char *line, unsigned int *map)
{
	if (line)
		free(line);
	if (map)
		free(map);
	return (NULL);
}

unsigned int	*ft_rdmap(int fd, t_data *bsq, unsigned int *map, char *line)
{
	int		i;
	int		j;

	i = 0;
	while (i < bsq->y)
	{
		j = ft_strlen(line) - 1;
		if (!line || line[j] != '\n' || j != bsq->x)
			return (ft_freenull(line, map));
		j = 0;
		while (j < bsq->x)
		{
			if (line[j] != bsq->e && line[j] != bsq->o)
				return (ft_freenull(line, map));
			ft_bit(map, (bsq->x * i) + j, line[j] == bsq->o);
			j++;
		}
		free(line);
		line = get_next_line(fd);
		i++;
	}
	return (map);
}

unsigned int	*ft_mallocmap(int fd, t_data *bsq)
{
	unsigned int	*map;
	int				n;
	char			*line;

	map = NULL;
	line = get_next_line(fd);
	if (!line || line[ft_strlen(line) - 1] != '\n')
		return (ft_freenull(line, map));
	bsq->x = ft_strlen(line) - 1;
	n = (sizeof(unsigned int) * 8);
	n = ((bsq->y * bsq->x) + n - 1) / n;
	map = (unsigned int *)malloc(n * sizeof(unsigned int));
	if (!map)
		return (ft_freenull(line, map));
	return (ft_rdmap(fd, bsq, map, line));
}
