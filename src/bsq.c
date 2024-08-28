/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/06 16:36:04 by rdomange          #+#    #+#             */
/*   Updated: 2024/08/09 13:58:04 by rdomange         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq.h"

int	main(int argc, char **argv)
{
	int		i;
	int		fd;
	t_bsq	bsq;
	char	*map;

	fd = 0;
	i = 0;
	while (++i < argc || (argc == 1 && i == 1))
	{
		bsq = (t_bsq){0, 0, 0, 0, 0, 0, 0};
		if (argc > 1)
			fd = open(argv[i], O_RDONLY);
		map = get_map(fd, &bsq);
		close(fd);
		if (map)
			ft_bsq(map, &bsq);
		else
			write(1, "map error\n", 10);
		if (argc > 2 && i < argc - 1)
			write(1, "\n", 1);
	}
	return (0);
}

int	ft_bsq(char *map, t_bsq *bsq)
{
	int		*buffer;
	int		cap;
	long	i;

	cap = bsq->x + 2;
	buffer = (int *)malloc(cap * sizeof(int));
	if (!buffer && write(1, "malloc fail\n", 12) + 2)
		return (0);
	i = 0;
	bsq->s = 0;
	while (i < bsq->x * bsq->y)
	{
		buffer[i % cap] = neighbours(buffer, map, bsq, i);
		if (buffer[i % cap] > bsq->s)
		{
			bsq->s = buffer[i % cap];
			bsq->i = i;
		}
		i++;
	}
	print_square(map, *bsq);
	free(buffer);
	free(map);
	return (bsq->s);
}

void	print_square(char *map, t_bsq bsq)
{
	long	i;

	i = 0;
	while (i < bsq.x * bsq.y)
	{
		if (ft_bit(map, i, 2))
			write(1, &bsq.o, 1);
		else if (insquare(i, bsq))
			write(1, &bsq.f, 1);
		else
			write(1, &bsq.e, 1);
		write(1, " ", 1);
		i++;
		if (!(i % bsq.x))
			write(1, "\n", 1);
	}
}

char	*get_map(int fd, t_bsq *bsq)
{
	char	*map;
	int		cap;
	char	c;

	cap = 4;
	map = get_rules(fd, bsq, &cap);
	while (map && read(fd, &c, 1) == 1)
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
