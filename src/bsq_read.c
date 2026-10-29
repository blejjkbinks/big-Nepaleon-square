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
	int		i;

	i = 0;
	while (++i < argc || (argc == 1 && i == 1))
	{
		bsq = (t_bsq){0};
		if (argc > 1)
			bsq.fd = open(argv[i], O_RDONLY);
		read_rules(&bsq);
		read_first_line(&bsq);
		alloc_map(&bsq);
		read_map(&bsq);
		print_square(&bsq);
		close(bsq.fd);
		free(bsq.buf);
		free(bsq.map);
		free(bsq.row);
		if (argc > 2 && i < argc - 1)
			write(1, "\n", 1);
	}
	return (0);
}

void	read_rules(t_bsq *bsq)
{
	char	l[14];
	long	n;
	long	k;
	long	i;

	n = 0;
	k = read(bsq->fd, &l[0], 1);
	while (k == 1 && l[n] != '\n' && ++n < 14)
		k = read(bsq->fd, &l[n], 1);
	i = 0;
	while (i < n - 3 && '0' <= l[i] && l[i] <= '9')
		bsq->y = bsq->y * 10 + l[i++] - '0';
	k = (k == 1 && n < 14 && n > 3 && i == n - 3);
	while (k && i < n && ' ' <= l[i] && l[i] <= '~')
		i++;
	bsq->e = l[(n - 3) * k];
	bsq->o = l[(n - 2) * k];
	bsq->f = l[(n - 1) * k];
	bsq->err = (!k || i != n || !bsq->y || bsq->y > 2147483647
			|| bsq->e == bsq->o || bsq->e == bsq->f || bsq->o == bsq->f);
}

void	read_first_line(t_bsq *bsq)
{
	long	n;
	long	k;

	if (bsq->err)
		return ;
	bsq->buf = ft_grow(NULL, 0, &bsq->cap, BUF_START);
	k = 0;
	n = 1;
	while (bsq->buf && k == bsq->len && n > 0)
	{
		n = read(bsq->fd, bsq->buf + bsq->len, bsq->cap - bsq->len);
		if (n > 0)
			bsq->len += n;
		while (k < bsq->len && bsq->buf[k] != '\n')
			k++;
		if (k == bsq->len && bsq->len == bsq->cap)
			bsq->buf = ft_grow(bsq->buf, bsq->len, &bsq->cap,
					bsq->cap * BUF_GROW);
	}
	bsq->x = k;
	bsq->err = (!bsq->buf || k == bsq->len || k == 0);
}

void	read_map(t_bsq *bsq)
{
	long	k;

	k = -1;
	while (!bsq->err && bsq->r < bsq->y)
	{
		if (++k == bsq->len)
		{
			bsq->len = read(bsq->fd, bsq->buf, bsq->cap);
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

char	*ft_grow(char *src, long len, long *cap, long size)
{
	char	*dst;
	long	i;

	*cap = size;
	dst = (char *)malloc(size);
	if (!dst && write(2, "malloc fail\n", 12))
		free(src);
	i = -1;
	while (dst && ++i < len)
		dst[i] = src[i];
	if (dst)
		free(src);
	return (dst);
}
