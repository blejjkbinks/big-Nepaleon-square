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
		bsq = (t_bsq){0, 0, 0, 0, 0, 0, 0, 0, 0};
		if (argc > 1)
			fd = open(argv[i], O_RDONLY);
		map = get_map(fd, &bsq);
		close(fd);
		if (map)
			ft_bsq(map, &bsq);
		else
			write(2, "map error\n", 10);
		if (argc > 2 && i < argc - 1)
			write(1, "\n", 1);
	}
	return (0);
}

int	ft_bsq(char *map, t_bsq *bsq)
{
	int		*row;
	long	i;

	row = (int *)malloc((bsq->x + 1) * sizeof(int));
	if (!row && write(2, "malloc fail\n", 12) + 2)
		return (ft_free(map) != NULL);
	i = 0;
	while (i <= bsq->x)
		row[i++] = 0;
	i = 0;
	while (i < bsq->y)
		solve_row(row, map, bsq, i++);
	i = 0;
	while (i < bsq->y)
		print_line(map, bsq, (char *)row, i++);
	free(row);
	free(map);
	return (bsq->s);
}
