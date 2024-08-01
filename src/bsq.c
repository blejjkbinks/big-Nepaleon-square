/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/30 00:36:43 by rdomange          #+#    #+#             */
/*   Updated: 2024/07/30 16:10:01 by rdomange         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq.h"
#include <stdio.h>

static int	ft_firstline(int fd, t_data *bsq)
{
	char	*line;
	int		len;

	line = get_next_line(fd);
	len = ft_strlen(line) - 1;
	if (len <= 3 || fd == -1 || !line)
	{
		if (line)
			free(line);
		return (1);
	}
	bsq->y = ft_atoi(line, len - 3);
	bsq->e = line[len - 3];
	bsq->o = line[len - 2];
	bsq->f = line[len - 1];
	free(line);
	if (bsq->y < 1 || bsq->e == bsq->o || bsq->e == bsq->f || bsq->o == bsq->f)
		return (1);
	return (0);
}

static int	ft_biggestsquare(int fd, t_data *bsq, unsigned int **map)
{
	int	*buffer;

	*map = ft_mallocmap(fd, bsq);
	if (!map)
		return (1);
	buffer = (int *)malloc((bsq->x * 2) * sizeof(int));
	bsq->i = 0;
	bsq->j = 0;
	bsq->s = 0;
	i_love_norm(bsq, *map, buffer);
	free(buffer);
	return (0);
}

static void	ft_printsquare(t_data *bsq, unsigned int *map)
{
	int	i;
	int	j;

	i = 0;
	while (i < bsq->y)
	{
		j = 0;
		while (j < bsq->x)
		{
			if (insquare(i, j, bsq))
				ft_putchar((char)bsq->f);
			else if (ft_bit(map, (bsq->x * i) + j, 2))
				ft_putchar((char)bsq->o);
			else
				ft_putchar((char)bsq->e);
			ft_putchar(' ');
			j++;
		}
		ft_putchar('\n');
		i++;
	}
}

static int	ft_bsq(int fd)
{
	t_data			bsq;
	unsigned int	*map;

	map = NULL;
	if (ft_firstline(fd, &bsq))
		return (ft_putstr("map error\n"));
	if (ft_biggestsquare(fd, &bsq, &map))
		return (ft_putstr("map error\n"));
	ft_printsquare(&bsq, map);
	free(map);
	return (0);
}

int	main(int argc, char **argv)
{
	int	i;
	int	fd;

	if (argc == 1)
		ft_bsq(0);
	i = 0;
	while (++i < argc)
	{
		fd = open(argv[i], O_RDONLY);
		ft_bsq(fd);
		close(fd);
	}
	return (0);
}
