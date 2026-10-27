/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 12:00:00 by rdomange          #+#    #+#             */
/*   Updated: 2026/10/27 12:00:00 by rdomange         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq.h"

int	get_map(int fd, t_bsq *bsq, int keep)
{
	char	*buf;
	long	len;
	long	cap;
	int		ok;

	if (!get_rules(fd, bsq))
		return (0);
	buf = first_line(fd, bsq, &len, &cap);
	if (buf)
		buf = fit_buf(buf, len, &cap, bsq);
	ok = (buf && alloc_map(bsq, keep));
	while (ok && len > 0 && parse(buf, len, bsq) && bsq->r < bsq->y)
		len = read(fd, buf, cap);
	free(buf);
	return (bsq->r == bsq->y);
}

int	get_rules(int fd, t_bsq *bsq)
{
	char	line[14];
	long	n;
	long	k;
	long	i;

	n = 0;
	k = read(fd, &line[0], 1);
	while (k == 1 && line[n] != '\n' && ++n < 14)
		k = read(fd, &line[n], 1);
	i = 0;
	while (i < n - 3 && '0' <= line[i] && line[i] <= '9')
		bsq->y = bsq->y * 10 + line[i++] - '0';
	if (k != 1 || n == 14 || n < 4 || i != n - 3
		|| !bsq->y || bsq->y > 2147483647)
		return (0);
	while (i < n && ' ' <= line[i] && line[i] <= '~')
		i++;
	if (i != n)
		return (0);
	bsq->e = line[n - 3];
	bsq->o = line[n - 2];
	bsq->f = line[n - 1];
	return (bsq->e != bsq->o && bsq->e != bsq->f && bsq->o != bsq->f);
}

char	*first_line(int fd, t_bsq *bsq, long *len, long *cap)
{
	char	*buf;
	long	n;
	long	k;

	*len = 0;
	buf = ft_grow(NULL, 0, cap, 64);
	while (buf)
	{
		n = read(fd, buf + *len, *cap - *len);
		if (n <= 0)
			return (ft_free(buf));
		k = *len;
		*len += n;
		while (k < *len && buf[k] != '\n')
			k++;
		bsq->x = k;
		if (k < *len && k)
			return (buf);
		if (k < *len)
			return (ft_free(buf));
		if (*len == *cap)
			buf = ft_grow(buf, *len, cap, *cap * 2);
	}
	return (NULL);
}

char	*fit_buf(char *buf, long len, long *cap, t_bsq *bsq)
{
	long	want;

	want = bsq->y * (bsq->x + 1);
	if (want > READ_BUF)
		want = READ_BUF;
	if (want > *cap)
		return (ft_grow(buf, len, cap, want));
	return (buf);
}

int	parse(char *buf, long len, t_bsq *bsq)
{
	long	k;

	k = -1;
	while (++k < len && bsq->r < bsq->y)
	{
		if (bsq->i == (bsq->r + 1) * bsq->x && buf[k] == '\n')
			bsq->r++;
		else if (bsq->i < (bsq->r + 1) * bsq->x
			&& (buf[k] == bsq->e || buf[k] == bsq->o))
			solve_cell(bsq, buf[k] == bsq->o);
		else
			return (0);
	}
	return (1);
}
