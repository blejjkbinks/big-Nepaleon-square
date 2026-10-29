/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq_read.c                                         :+:      :+:    :+:   */
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
	t_bsq	bsq;
	char	*map;
	int		fd;
	int		i;

	fd = 0;
	i = 0;
	while (++i < argc || (argc == 1 && i == 1))
	{
		bsq = (t_bsq){0};
		if (argc > 1)
			fd = open(argv[i], O_RDONLY);
		map = get_map(fd, &bsq);
		close(fd);
		if (map)
			print_square(&bsq);
		else
			write(2, "map error\n", 10);
		free(map);
		free(bsq.row);
		if (argc > 2 && i < argc - 1)
			write(1, "\n", 1);
	}
	return (0);
}

char	*get_map(int fd, t_bsq *bsq)
{
	long	c;

	read_rules(fd, bsq);
	read_first_line(fd, bsq);
	if (!bsq->err)
		bsq->map = (char *)malloc((bsq->x * bsq->y + 7) / 8);
	if (!bsq->err)
		bsq->row = (int *)malloc((bsq->x + 1) * sizeof(int));
	if (!bsq->err && (!bsq->map || !bsq->row))
		bsq->err = write(2, "malloc fail\n", 12);
	c = -1;
	while (!bsq->err && ++c <= bsq->x)
		bsq->row[c] = 0;
	read_map(fd, bsq);
	free(bsq->buf);
	if (!bsq->err)
		return (bsq->map);
	free(bsq->map);
	return (NULL);
}

void	read_rules(int fd, t_bsq *bsq)
{
	char	l[14];
	long	n;
	long	k;
	long	i;

	n = 0;
	k = 0;
	while (n < 14 && read(fd, &l[n], 1) == 1 && ++k && l[n] != '\n')
		n++;
	i = 0;
	while (i < n - 3 && '0' <= l[i] && l[i] <= '9')
		bsq->y = bsq->y * 10 + l[i++] - '0';
	k = (k == n + 1 && n > 3 && i == n - 3);
	while (k && i < n && ' ' <= l[i] && l[i] <= '~')
		i++;
	bsq->e = l[(n - 3) * k];
	bsq->o = l[(n - 2) * k];
	bsq->f = l[(n - 1) * k];
	bsq->err = (!k || i != n || !bsq->y || bsq->y > INT_MAX
			|| bsq->e == bsq->o || bsq->e == bsq->f || bsq->o == bsq->f);
}

void	read_first_line(int fd, t_bsq *bsq)
{
	long	n;
	long	k;

	if (bsq->err)
		return ;
	bsq->buf = ft_realloc(NULL, 0, &bsq->cap, BUF_START);
	k = 0;
	n = 1;
	while (bsq->buf && k == bsq->len && n > 0)
	{
		n = read(fd, bsq->buf + bsq->len, bsq->cap - bsq->len);
		if (n > 0)
			bsq->len += n;
		while (k < bsq->len && bsq->buf[k] != '\n')
			k++;
		if (k == bsq->len && bsq->len == bsq->cap)
			bsq->buf = ft_realloc(bsq->buf, bsq->len, &bsq->cap,
					bsq->cap * BUF_GROW);
	}
	bsq->x = k;
	bsq->err = (!bsq->buf || k == bsq->len || k == 0);
}

void	read_map(int fd, t_bsq *bsq)
{
	long	k;

	k = -1;
	while (!bsq->err && bsq->r < bsq->y)
	{
		if (++k == bsq->len)
		{
			bsq->len = read(fd, bsq->buf, bsq->cap);
			k = 0;
		}
		if (bsq->len <= 0)
			bsq->err = 1;
		else if (bsq->i == (bsq->r + 1) * bsq->x && bsq->buf[k] == '\n')
			bsq->r++;
		else if (bsq->i < (bsq->r + 1) * bsq->x
			&& (bsq->buf[k] == bsq->e || bsq->buf[k] == bsq->o))
			solve_cell(bsq, bsq->buf[k] == bsq->o);
		else
			bsq->err = 1;
	}
}
