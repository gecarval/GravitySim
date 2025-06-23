/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reads.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gecarval <gecarval@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 20:37:31 by gecarval          #+#    #+#             */
/*   Updated: 2024/11/08 08:16:35 by gecarval         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/renderer.h"

void create_gsim(t_data *data) {
	int			i;
	t_particle *tmp;

	srand((unsigned int)time(NULL));
	data->gsim->part = (t_particle *)malloc(sizeof(t_particle));
	if (!data->gsim->part) display_error(data, "part malloc error\n");
	i = -1;
	tmp = data->gsim->part;
	while (++i < data->num_of_particles) {
		pthread_mutex_init(&tmp->mutex, NULL);
		tmp->pos = create_vector(rand() % data->winx, rand() % data->winy);
		tmp->prev_pos = create_vector(0, 0);
		tmp->vel = create_vector(0, 0);
		tmp->acel = create_vector(0, 0);
		tmp->mass = 1;
		tmp->r = data->radius;
		tmp->color = WHITE;
		if (i + 1 == data->num_of_particles) break;
		tmp->next = (t_particle *)malloc(sizeof(t_particle));
		if (!tmp->next) display_error(data, "part malloc error\n");
		tmp = tmp->next;
	}
}
