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

	i = 0;
	while (++i < argc || (argc == 1 && i == 1))
	{
		fd = 0;
		if (argc > 1)
			fd = open(argv[i], O_RDONLY);
		bsq = (t_bsq){0, 0, 0, 0, 0, 0, 0};
		map = get_map(fd, &bsq);
		if (map)
			ft_bsq(map, &bsq);
		else
			write(1, "map error\n", 10);
		close(fd);
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
		if (!(i % bsq.x) && i)
			write(1, "\n", 1);
	}
}

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
