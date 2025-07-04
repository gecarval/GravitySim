/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calls.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gecarval <gecarval@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 20:32:15 by gecarval          #+#    #+#             */
/*   Updated: 2024/11/14 15:21:25 by gecarval         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/renderer.h"

void free_part(t_particle *part, t_data *data) {
	t_particle *tmp;
	int			i;

	i = -1;
	while (++i < data->num_of_particles) {
		tmp = part;
		part = part->next;
		pthread_mutex_destroy(&tmp->mutex);
		free(tmp);
	}
}

int exit_data(t_data *data, int fd) {
	if (data != NULL && data->qt != NULL) free_quadtree(data->qt);
	if (data != NULL && data->hashmap != NULL) free_hashmap(data->hashmap);
	if (data != NULL && data->col_processors != NULL)
		free(data->col_processors);
	if (data != NULL && data->processors != NULL) free(data->processors);
	if (data != NULL && data->img != NULL && data->img->img_ptr != NULL)
		mlx_destroy_image(data->ini, data->img->img_ptr);
	if (data != NULL && data->img != NULL) free(data->img);
	if (data != NULL && data->gsim != NULL && data->gsim->part != NULL)
		free_part(data->gsim->part, data);
	if (data != NULL && data->gsim != NULL) free(data->gsim);
	if (data != NULL && data->win != NULL)
		mlx_clear_window(data->ini, data->win);
	if (data != NULL && data->win != NULL)
		mlx_destroy_window(data->ini, data->win);
	if (data != NULL && data->ini != NULL) mlx_destroy_display(data->ini);
	if (data != NULL && data->ini != NULL) free(data->ini);
	if (data != NULL) free(data);
	exit(fd);
	return (0);
}

void display_error(t_data *data, char *msg) {
	ft_putstr_fd(msg, 2);
	exit_data(data, 1);
}

void ft_set_params(t_data *data, int part_n, int radius) {
	data->num_of_particles = part_n;
	data->radius = radius;
}

void ft_init_program(t_data *data) {
	ft_set_params(data, 5000, 1);
	data->ini = mlx_init();
	if (!data->ini) display_error(data, "init error\n");
	data->win =
		mlx_new_window(data->ini, data->winx, data->winy, "Gravity Sim");
	if (!data->win) display_error(data, "window error\n");
	data->gsim = (t_gravsim *)malloc(sizeof(t_gravsim));
	if (!data->gsim) display_error(data, "gravitysim malloc error\n");
	data->gsim->part = NULL;
	data->qt = NULL;
	data->hashmap = NULL;
	data->processors =
		(t_processor *)ft_calloc(sizeof(t_processor), MAX_THREADS);
	if (!data->processors) display_error(data, "processors malloc error\n");
	data->col_processors =
		(t_processor *)ft_calloc(sizeof(t_processor), data->num_of_particles);
	if (!data->col_processors)
		display_error(data, "col_processors malloc error\n");
	data->img = (t_img *)malloc(sizeof(t_img));
	if (!data->img) display_error(data, "img malloc error\n");
	data->img->img_ptr = mlx_new_image(data->ini, data->winx, data->winy);
	if (!data->img->img_ptr) display_error(data, "img ptr error\n");
	data->img->img_px = mlx_get_data_addr(data->img->img_ptr, &data->img->bpp,
										  &data->img->llen, &data->img->end);
	if (!data->img->img_px) display_error(data, "img data error\n");
}
