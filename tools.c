/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anonymous <anonymous@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 17:38:51 by gecarval          #+#    #+#             */
/*   Updated: 2024/09/20 21:12:20 by gecarval         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/renderer.h"

// HOOKS
int	mouse_released(int key, t_data *data)
{
	if (key == 1)
		data->click_hold = 0;
	ft_printf("%d\n", key);
	return (0);
}

int	mouse_click(int key, int x, int y, t_data *data)
{
	(void)x;
	(void)y;
	if (key == 1 && data->click_hold == 0)
		data->click_hold = 1;
	else
		data->click_hold = 0;
	return (0);
}

void	put_particle(int x, int y, t_data *data)
{
	t_particle	*tmp;

	data->num_of_particles++;
	tmp = (t_particle *)malloc(sizeof(t_particle));
	if (!tmp)
		display_error(data, "part malloc error\n");
	tmp->pos = create_vector(x, y);
	tmp->prev_pos = tmp->pos;
	tmp->vel = create_vector(0, 0);
	tmp->acel = create_vector(0, 0);
	tmp->mass = 1;
	tmp->r = data->radius;
	tmp->color = WHITE;
	tmp->next = data->gsim->part;
	data->gsim->part = tmp;
}

int	mlx_anim(t_data *data)
{
	int	i;
	
	if (data->anigsim == 1)
	{
		part_sim(data);
		mlx_put_image_to_window(data->ini, data->win, data->img->img_ptr, 0, 0);
		i = 0;
		while (i < data->timing)
			i++;
		if (data->click_hold == 1)
		{
			mlx_mouse_get_pos(data->ini, data->win, &data->mposx, &data->mposy);
			if (data->mposx < 0 || data->mposx > data->winx)
				data->click_hold = 0;
			if (data->mposy < 0 || data->mposy > data->winy)
				data->click_hold = 0;
			put_particle(data->mposx, data->mposy, data);
		}
	}
	return (0);
}

int	mlx_cooked(int key, t_data *data)
{
	if (key == ESC)
		exit_data(data, 0);
	if (key == '1')
		data->anigsim *= -1;
	if (key == ']')
		data->timing += 10000000;
	if (key == '[')
		if (data->timing > 9999999)
			data->timing -= 10000000;
	if (key == 't')
		reset_pos(data);
	if (key == 'y')
		data->show_tree *= -1;
	if (key == ' ')
	{
		render_background(data, 0x000000);
		mlx_put_image_to_window(data->ini, data->win, data->img->img_ptr, 0, 0);
		water_mark(data);
	}
	return (0);
}

// GRAPH
void	pixel_to_img(int x, int y, t_data *data, int color)
{
	char	*pixel;

	if ((x < 0 || x > data->winx) || (y < 0 || y > data->winy))
		return ;
	pixel = data->img->img_px + y * data->img->llen + x * (data->img->bpp / 8);
	*(int *)pixel = color;
}

void	pixel_to_img_float(float_t x, float_t y, t_data *data, int color)
{
	char	*pixel;

	if ((x < 0 || x > data->winx) || (y < 0 || y > data->winy))
		return ;
	pixel = data->img->img_px + (int)y * data->img->llen + (int)x * (data->img->bpp / 8);
	*(int *)pixel = color;
}

void	water_mark(t_data *data)
{
	mlx_string_put(data->ini, data->win,
		15, 15, 120000, "renders by gecarval");
	mlx_string_put(data->ini, data->win,
		15, 60, 120000, "Press 1 for Life Sim");
}

void	render_background(t_data *data, int color)
{
	int	x;
	int	y;

	x = -1;
	while (++x < data->winx)
	{
		y = -1;
		while (++y < data->winy)
			pixel_to_img(x, y, data, color);
	}
}

// FREE
void	ft_free_matrix(void **vec)
{
	int	i;

	i = -1;
	while (vec[++i])
		free(vec[i]);
	free(vec);
}

void	ft_free_tensor(void ***z)
{
	int	i;

	i = -1;
	while (z[++i])
		ft_free_matrix(z[i]);
	free(z);
}

// DELTAS
void	defdel(t_delta *a, float_t ini, float_t fin)
{
	a->ini = ini;
	a->fin = fin;
}

t_delta	revdel(t_delta a)
{
	int	tmp;

	tmp = a.ini;
	a.ini = a.fin;
	a.fin = tmp;
	return (a);
}

// RULES
void	reset_pos(t_data *data)
{
	int		i;
	t_particle	*tmp;

	i = 0;
	tmp = data->gsim->part;
	while (i < data->num_of_particles)
	{
		tmp->pos = create_vector(rand() % data->winx, rand() % data->winy);
		tmp = tmp->next;
		i++;
	}
}

// MATH
float_t	rand_float_t(float_t min, float_t max)
{
	return (((float_t)rand() / ((float_t)RAND_MAX / (max - min)) + min));
}
