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

	fd = 0;
	i = 0;
	while (++i < argc || (argc == 1 && i == 1))
	{
		bsq = (t_bsq){0};
		if (argc > 1)
			fd = open(argv[i], O_RDONLY);
		if (get_map(fd, &bsq, argc == 1))
			ft_print(&bsq, argv[i]);
		else
			write(2, "map error\n", 10);
		close(fd);
		free(bsq.w);
		free(bsq.m);
		if (argc > 2 && i < argc - 1)
			write(1, "\n", 1);
	}
	return (0);
}

void	ft_print(t_bsq *bsq, char *path)
{
	bsq->r = 0;
	bsq->i = 0;
	if (!bsq->m)
		reprint(bsq, path);
	while (bsq->m && bsq->r < bsq->y)
		print_line(bsq);
	if (bsq->r < bsq->y)
		write(2, "map error\n", 10);
}
