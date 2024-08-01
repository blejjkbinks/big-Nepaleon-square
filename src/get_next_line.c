/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/24 12:50:15 by rdomange          #+#    #+#             */
/*   Updated: 2024/07/30 01:12:28 by rdomange         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq.h"

static char	*ft_realloc_line(char *src, size_t *size, int mult)
{
	char	*dst;
	size_t	i;

	*size *= mult;
	if (!src || *size == 0)
		return (NULL);
	dst = (char *)malloc((*size + 1) * sizeof(char));
	if (!dst)
		return (NULL);
	i = 0;
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = 0;
	free(src);
	return (dst);
}

static void	ft_init(char **ret, size_t *cap, size_t *len, int *r)
{
	*len = 0;
	*cap = 500;
	*r = 0;
	*ret = (char *)malloc(*cap * sizeof(char));
}

static int	ft_read_char(int fd, char *c, int *r)
{
	*r = (int)read(fd, c, sizeof(char));
	return (*r);
}

static char	*ft_read_line(int fd)
{
	char	*ret;
	size_t	cap;
	size_t	len;
	int		r;

	ft_init(&ret, &cap, &len, &r);
	if (!ret)
		return (NULL);
	while (ft_read_char(fd, &ret[len], &r) > 0)
	{
		if (len + 1 == cap)
			ret = ft_realloc_line(ret, &cap, 2);
		if (!ret)
			return (NULL);
		ret[++len] = 0;
		if (ret[len - 1] == '\n')
			break ;
	}
	if (r < 0 || len == 0)
	{
		free(ret);
		return (NULL);
	}
	return (ret);
}

char	*get_next_line(int fd)
{
	char	*line;
	size_t	len;

	if (fd < 0)
		return (NULL);
	line = ft_read_line(fd);
	len = ft_strlen(line);
	line = ft_realloc_line(line, &len, 1);
	return (line);
}
