/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 12:00:00 by rdomange          #+#    #+#             */
/*   Updated: 2026/10/27 12:00:00 by rdomange         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq.h"

void	print_line(t_bsq *bsq)
{
	char	*line;
	long	c;

	line = (char *)bsq->w;
	c = -1;
	while (++c < bsq->x)
	{
		line[c * (1 + SPACED)] = bsq->e;
		if (ft_bit(bsq->m, bsq->r * bsq->x + c, 2))
			line[c * (1 + SPACED)] = bsq->o;
		if (SPACED)
			line[c * 2 + 1] = ' ';
	}
	flush_line(bsq, line);
}

void	reprint(t_bsq *bsq, char *path)
{
	char	buf[READ_BUF];
	long	n;
	long	k;
	int		fd;

	fd = open(path, O_RDONLY);
	buf[0] = 0;
	n = 1;
	while (n == 1 && buf[0] != '\n')
		n = read(fd, buf, 1);
	n = read(fd, buf, READ_BUF);
	while (n > 0 && bsq->r < bsq->y)
	{
		k = -1;
		while (++k < n && bsq->r < bsq->y)
			put_char(bsq, buf[k]);
		n = read(fd, buf, READ_BUF);
	}
	close(fd);
}

void	put_char(t_bsq *bsq, char ch)
{
	char	*line;
	long	c;

	line = (char *)bsq->w;
	c = bsq->i - bsq->r * bsq->x;
	if (ch != '\n' && c < bsq->x)
	{
		line[c * (1 + SPACED)] = ch;
		if (SPACED)
			line[c * 2 + 1] = ' ';
		bsq->i++;
	}
	else if (ch == '\n')
		flush_line(bsq, line);
}

void	flush_line(t_bsq *bsq, char *line)
{
	long	t;
	long	l;
	long	c;

	t = bsq->b / bsq->x - bsq->s + 1;
	l = bsq->b % bsq->x - bsq->s + 1;
	c = l - 1;
	while (bsq->r >= t && bsq->r < t + bsq->s && ++c < l + bsq->s)
		line[c * (1 + SPACED)] = bsq->f;
	line[bsq->x * (1 + SPACED) - SPACED] = '\n';
	write(1, line, bsq->x * (1 + SPACED) - SPACED + 1);
	bsq->r++;
	bsq->i = bsq->r * bsq->x;
}
