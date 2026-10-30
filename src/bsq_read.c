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
		free(bsq.map);
		free(bsq.row);
		if (argc > 2 && i < argc - 1)
			write(1, "\n", 1);
	}
	return (0);
}

char	*get_map(int fd, t_bsq *bsq)
{
	t_buf	buf;
	int		ok;

	buf = (t_buf){0};
	ok = (read_rules(fd, bsq) && read_first_line(fd, bsq, &buf)
			&& alloc_map(bsq) && read_map(fd, bsq, &buf));
	free(buf.str);
	if (!ok)
		return (NULL);
	return (bsq->map);
}

int	read_rules(int fd, t_bsq *bsq)
{
	char	l[14];
	long	n;
	long	ok;
	long	i;

	n = 0;
	l[0] = 0;
	while (read(fd, &l[n], 1) == 1 && l[n] != '\n' && ++n < 14)
		l[n] = 0;
	i = 0;
	while (i < n - 3 && '0' <= l[i] && l[i] <= '9')
		bsq->y = bsq->y * 10 + l[i++] - '0';
	ok = (n < 14 && l[n] == '\n' && n > 3 && i == n - 3);
	while (ok && i < n && ' ' <= l[i] && l[i] <= '~')
		i++;
	bsq->e = l[(n - 3) * ok];
	bsq->o = l[(n - 2) * ok];
	bsq->f = l[(n - 1) * ok];
	return (ok && i == n && bsq->y && bsq->y <= INT_MAX
		&& bsq->e != bsq->o && bsq->e != bsq->f && bsq->o != bsq->f);
}

int	read_first_line(int fd, t_bsq *bsq, t_buf *buf)
{
	long	n;
	long	k;

	ft_realloc(buf, BUF_START);
	k = 0;
	n = 1;
	while (buf->str && k == buf->len && n > 0)
	{
		n = read(fd, buf->str + buf->len, buf->cap - buf->len);
		if (n > 0)
			buf->len += n;
		while (k < buf->len && buf->str[k] != '\n')
			k++;
		if (k == buf->len && buf->len == buf->cap)
			ft_realloc(buf, buf->cap * BUF_GROW);
	}
	bsq->x = k;
	return (buf->str && k < buf->len && k > 0);
}

int	read_map(int fd, t_bsq *bsq, t_buf *buf)
{
	long	k;

	k = -1;
	while (bsq->r < bsq->y)
	{
		if (++k == buf->len)
		{
			buf->len = read(fd, buf->str, buf->cap);
			k = 0;
		}
		if (buf->len <= 0)
			return (0);
		else if (bsq->i == (bsq->r + 1) * bsq->x && buf->str[k] == '\n')
			bsq->r++;
		else if (bsq->i < (bsq->r + 1) * bsq->x
			&& (buf->str[k] == bsq->e || buf->str[k] == bsq->o))
			solve_cell(bsq, buf->str[k] == bsq->o);
		else
			return (0);
	}
	return (1);
}
